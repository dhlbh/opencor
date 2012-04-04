//==============================================================================
// CellML file class
//==============================================================================

#include "cellmlfile.h"

//==============================================================================

#include <QTime>
#include <QUrl>

//==============================================================================

#ifdef Q_WS_X11
    #include <stdint.h>
    //---GRY--- THE ABOVE HEADER FILE IS REQUIRED ON LINUX, SO WE CAN USE
    //          uint32_t (SO WE CAN 'PROPERLY' MAKE USE OF THE CellML API). NOW,
    //          WE WOULD NORMALLY HAVE
    //              #include <cstdint>
    //          BUT THIS IS NOT CURRENTLY SUPPORTED BY THE CURRENT ISO C++
    //          STANDARD. INDEED, TO INCLUDE IT WILL GENERATE THE FOLLOWING
    //          ERROR AT COMPILE TIME:
    //              error: #error This file requires compiler and library
    //              support for the upcoming ISO C++ standard, C++0x. This
    //              support is currently experimental, and must be enabled with
    //              the -std=c++0x or -std=gnu++0x compiler options.
    //          AND WELL, WE DON'T WANT TO ENABLE EITHER OF THESE OPTIONS SINCE
    //          SUPPORT IS STILL ONLY EXPERIMENTAL, SO...
#endif

//==============================================================================

#include "IfaceCellML_APISPEC.hxx"
#include "IfaceVACSS.hxx"

#include "CellMLBootstrap.hpp"
#include "VACSSBootstrap.hpp"

//==============================================================================

namespace OpenCOR {
namespace CellMLSupport {

//==============================================================================

CellmlFile::CellmlFile(const QString &pFileName) :
    mFileName(pFileName),
    mModel(0)
{
    // Instantiate our runtime object

    mRuntime = new CellmlFileRuntime();

    // Reset ourselves

    reset();
}

//==============================================================================

CellmlFile::~CellmlFile()
{
    // Delete some internal objects
    // Note: mModel gets automatically deleted, if needed, so...

    delete mRuntime;
}

//==============================================================================

void CellmlFile::reset()
{
    // Reset all of the file's properties
    // Note: setting mModel to zero will automatically delete the current
    //       instance, if any

    mModel = 0;

    mIssues.clear();

    mLoadingNeeded       = true;
    mIsValidNeeded       = true;
    mRuntimeUpdateNeeded = true;
}

//==============================================================================

bool CellmlFile::load()
{
    if (!mLoadingNeeded)
        // The file is already loaded, so...

        return true;

    // Reset any issues that we may have found before

    mIssues.clear();

    // Get a bootstrap object

    ObjRef<iface::cellml_api::CellMLBootstrap> cellmlBootstrap = CreateCellMLBootstrap();

    // Get its model loader

    ObjRef<iface::cellml_api::DOMModelLoader> modelLoader = cellmlBootstrap->modelLoader();

    // Try to load the model

qDebug("---------------------------------------");
qDebug("%s:", qPrintable(mFileName));

    try {
QTime time;

time.start();

        mModel = modelLoader->loadFromURL(QUrl::fromLocalFile(mFileName).toString().toStdWString().c_str());

qDebug(" - CellML Loading time: %s s", qPrintable(QString::number(0.001*time.elapsed(), 'g', 3)));
    } catch (iface::cellml_api::CellMLException &) {
        // Something went wrong with the loading of the model, so...

        mIssues.append(CellmlFileIssue(CellmlFileIssue::Error,
                                       tr("the model could not be loaded (%1)").arg(QString::fromStdWString(modelLoader->lastErrorMessage()))));

        return false;
    }

    // In the case of a non CellML 1.0 model, we want all imports to be fully
    // instantiated

    if (QString::fromStdWString(mModel->cellmlVersion()).compare(Cellml_1_0))
        try {
QTime time;

time.start();

            mModel->fullyInstantiateImports();

qDebug(" - CellML full instantiation time: %s s", qPrintable(QString::number(0.001*time.elapsed(), 'g', 3)));
        } catch (...) {
            // Something went wrong with the full instantiation of the imports,
            // so...

            mIssues.append(CellmlFileIssue(CellmlFileIssue::Error,
                                           tr("the model's imports could not be fully instantiated")));

            return false;
        }

    // All done, so...

    mLoadingNeeded = false;

    return true;
}

//==============================================================================

bool CellmlFile::reload()
{
    // We want to reload the file, so we must first reset it

    reset();

    // Now, we can try to (re)load the file

    return load();
}

//==============================================================================

bool CellmlFile::isValid()
{
    if (!mIsValidNeeded)
        // The file has already been validated, so...

        return mIsValid;

    // Load (but not reload!) the file, if needed

    if (load()) {
        // The file was properly loaded (or was already loaded), so check
        // whether it is CellML valid
        // Note: validateModel() is somewhat slow, but there is (unfortunately)
        //       nothing we can do about it. Then, there is getPositionInXML()
        //       which is painfully slow, but unlike for validateModel() its use
        //       is not essential (even though it would be nice from an
        //       end-user's perspective). So, rather than retrieve the
        //       line/column of every single warning/error, we only keep track
        //       of the various warnings/errors and only retrieve their
        //       corresponding line/column when requested (definitely not neat
        //       from an end-user's perspective, but we just can't afford the
        //       time it takes to fully validate a model that has many
        //       warnings/errors)...

QTime time;

time.start();

        ObjRef<iface::cellml_services::VACSService> vacssService = CreateVACSService();
        ObjRef<iface::cellml_services::CellMLValidityErrorSet> cellmlValidityErrorSet = vacssService->validateModel(mModel);

qDebug(" - CellML validation time: %s s", qPrintable(QString::number(0.001*time.elapsed(), 'g', 3)));

        // Determine the number of errors and warnings
        // Note: CellMLValidityErrorSet::nValidityErrors() returns any type of
        //       validation issue, be it an error or a warning, so we need to
        //       determine the number of true errors

        int cellmlErrorsCount = 0;

time.restart();

        for (int i = 0, iMax = cellmlValidityErrorSet->nValidityErrors(); i < iMax; ++i) {
            ObjRef<iface::cellml_services::CellMLValidityError> cellmlValidityIssue = cellmlValidityErrorSet->getValidityError(i);

            DECLARE_QUERY_INTERFACE_OBJREF(cellmlRepresentationValidityError, cellmlValidityIssue,
                                           cellml_services::CellMLRepresentationValidityError);

            // Determine the issue's location

            uint32_t line = 0;
            uint32_t column = 0;
            QString importedFile = QString();

            if (cellmlRepresentationValidityError) {
                // We are dealing with a CellML representation issue, so
                // determine its line and column

                ObjRef<iface::dom::Node> errorNode = cellmlRepresentationValidityError->errorNode();

                line = vacssService->getPositionInXML(errorNode,
                                                      cellmlRepresentationValidityError->errorNodalOffset(),
                                                      &column);
            } else {
                // We are not dealing with a CellML representation issue, so
                // check whether we are dealing with a semantic one

                DECLARE_QUERY_INTERFACE_OBJREF(cellmlSemanticValidityError, cellmlValidityIssue,
                                               cellml_services::CellMLSemanticValidityError);

                if (cellmlSemanticValidityError) {
                    // We are dealing with a CellML semantic issue, so determine
                    // its line and column

                    ObjRef<iface::cellml_api::CellMLElement> cellmlElement = cellmlSemanticValidityError->errorElement();

                    DECLARE_QUERY_INTERFACE_OBJREF(cellmlDomElement, cellmlElement,
                                                   cellml_api::CellMLDOMElement);

                    ObjRef<iface::dom::Element> domElement = cellmlDomElement->domElement();

                    line = vacssService->getPositionInXML(domElement, 0,
                                                          &column);

                    // Also determine its imported file, if any

                    while (true) {
                        // Retrieve the CellML element's parent

                        ObjRef<iface::cellml_api::CellMLElement> cellmlElementParent = cellmlElement->parentElement();

                        if (!cellmlElementParent)
                            // There is no parent, so...

                            break;

                        // Check whether the parent is an imported file

                        DECLARE_QUERY_INTERFACE_OBJREF(importedCellmlFile, cellmlElementParent,
                                                       cellml_api::Model);

                        if (!importedCellmlFile)
                            // This is not an imported file, so...

                            continue;

                        // Retrieve the imported CellML element

                        ObjRef<iface::cellml_api::CellMLElement> importedCellmlElement = importedCellmlFile->parentElement();

                        if (!importedCellmlElement)
                            // This is not an imported CellML element, so...

                            break;

                        // Check whether the imported CellML element is an
                        // import CellML element

                        DECLARE_QUERY_INTERFACE_OBJREF(importCellmlElement, importedCellmlElement,
                                                       cellml_api::CellMLImport);

                        if (!importCellmlElement)
                            // This is not an import CellML element, so...

                            break;

                        ObjRef<iface::cellml_api::URI> href = importCellmlElement->xlinkHref();

                        importedFile = QString::fromStdWString(href->asText());

                        break;
                    }
                }
            }

            // Determine the issue's type

            CellmlFileIssue::Type issueType;

            if (cellmlValidityIssue->isWarningOnly()) {
                // We are dealing with a warning

                issueType = CellmlFileIssue::Warning;
            } else {
                // We are dealing with an error

                ++cellmlErrorsCount;

                issueType = CellmlFileIssue::Error;
            }

            // Append the issue to our list

            mIssues.append(CellmlFileIssue(issueType,
                                           QString::fromStdWString(cellmlValidityIssue->description()),
                                           line, column, importedFile));
        }

qDebug(" - CellML warnings vs. errors time: %s s", qPrintable(QString::number(0.001*time.elapsed(), 'g', 3)));

        if (cellmlErrorsCount)
            // There are CellML errors, so...

            mIsValid = false;
        else
            // Everything went as expected, so...

            mIsValid = true;

        mIsValidNeeded = false;

        return mIsValid;
    } else {
        // Something went wrong with the loading of the file, so...

        return false;
    }
}

//==============================================================================

CellmlFileIssues CellmlFile::issues() const
{
    // Return the file's issue(s)

    return mIssues;
}

//==============================================================================

CellmlFileRuntime * CellmlFile::runtime()
{
    if (!mRuntimeUpdateNeeded)
        // There is no need for the runtime to be updated, so...

        return mRuntime;

    // Load (but not reload!) the file, if needed

    if (load()) {
        // The file is loaded, so return an updated version of its runtime

        mRuntime->update(mModel);

        mRuntimeUpdateNeeded = !mRuntime->isValid();

        return mRuntime;
    } else {
        // The file isn't valid, so reset its runtime

        return mRuntime->update();
    }
}

//==============================================================================

QString CellmlFile::modelName() const
{
    // Return the model's name

    return QString::fromStdWString(mModel->name());
}

//==============================================================================

CellmlFileImports CellmlFile::imports() const
{
    CellmlFileImports res = CellmlFileImports();

    // Iterate through the imports and add them to our list

    iface::cellml_api::CellMLImportIterator *cellmlFileImportsIterator = mModel->imports()->iterateImports();
    iface::cellml_api::CellMLImport *cellmlImport;

    while (cellmlImport = cellmlFileImportsIterator->nextImport()) {
        // We have an import, so add it to our list

        CellmlFileImport *cellmlFileImport = new CellmlFileImport(QString::fromStdWString(cellmlImport->xlinkHref()->asText()));

        res.append(cellmlFileImport);

        // Keep track of any unit imports...

        iface::cellml_api::ImportUnitsIterator *importUnitsIterator = cellmlImport->units()->iterateImportUnits();
        iface::cellml_api::ImportUnits *importUnits;

        while (importUnits = importUnitsIterator->nextImportUnits())
            cellmlFileImport->addUnits(QString::fromStdWString(importUnits->name()),
                                       QString::fromStdWString(importUnits->unitsRef()));

        // ... and of any component imports

        iface::cellml_api::ImportComponentIterator *importComponentIterator = cellmlImport->components()->iterateImportComponents();
        iface::cellml_api::ImportComponent *importComponent;

        while (importComponent = importComponentIterator->nextImportComponent())
            cellmlFileImport->addComponent(QString::fromStdWString(importComponent->name()),
                                           QString::fromStdWString(importComponent->componentRef()));
    }

    // Return our list of imports for the model

    return res;
}

//==============================================================================

}   // namespace CellMLSupport
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================

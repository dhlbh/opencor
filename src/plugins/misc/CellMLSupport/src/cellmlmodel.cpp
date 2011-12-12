//==============================================================================
// CellML model class
//==============================================================================

#include "cellmlmodel.h"

//==============================================================================

#include <QUrl>

//==============================================================================

#include "IfaceVACSS.hxx"

#include "CellMLBootstrap.hpp"
#include "VACSSBootstrap.hpp"

//==============================================================================

namespace OpenCOR {
namespace CellMLSupport {

//==============================================================================

CellmlModelIssue::CellmlModelIssue(const Type &pType, const QString &pMessage,
                                   const uint32_t &pLine,
                                   const uint32_t &pColumn,
                                   const QString &pImportedModel) :
    mType(pType),
    mMessage(pMessage),
    mLine(pLine),
    mColumn(pColumn),
    mImportedModel(pImportedModel)
{
}

//==============================================================================

CellmlModelIssue::Type CellmlModelIssue::type() const
{
    // Return the issue's type

    return mType;
}

//==============================================================================

QString CellmlModelIssue::message() const
{
    // Return the issue's message

    return mMessage;
}

//==============================================================================

uint32_t CellmlModelIssue::line() const
{
    // Return the issue's line

    return mLine;
}

//==============================================================================

uint32_t CellmlModelIssue::column() const
{
    // Return the issue's column

    return mColumn;
}

//==============================================================================

QString CellmlModelIssue::importedModel() const
{
    // Return the issue's imported model

    return mImportedModel;
}

//==============================================================================

void CellmlModelRuntime::reset()
{
    // Reset all of the runtime's properties

//---GRY--- TO BE DONE...
}

//==============================================================================

CellmlModel::CellmlModel(const QString &pFileName) :
    mFileName(pFileName),
    mModel(0)
{
    // Get a bootstrap object

    mCellmlBootstrap = CreateCellMLBootstrap();

    // Retrieve the model loader

    mModelLoader = mCellmlBootstrap->modelLoader();
}

//==============================================================================

void CellmlModel::reset()
{
    // Reset all of the model's properties

    delete mModel; mModel = 0;

    mIssues.clear();
}

//==============================================================================

bool CellmlModel::load()
{
    if (mModel) {
        // The model is already loaded, so...

        return true;
    } else {
        // Try to load the model

        try {
            mModel = mModelLoader->loadFromURL(QUrl::fromLocalFile(mFileName).toString().toStdWString().c_str());
        } catch (iface::cellml_api::CellMLException &) {
            // Something went wrong with the loading of the model, generate an
            // error message

            mIssues.append(CellmlModelIssue(CellmlModelIssue::Error,
                                            QString("the model could not be loaded (%1)").arg(QString::fromWCharArray(mModelLoader->lastErrorMessage()))));

            return false;
        }

        // In the case of a non CellML 1.0 model, we want all imports to be
        // fully instantiated

        if (QString::fromWCharArray(mModel->cellmlVersion()).compare(Cellml_1_0))
            mModel->fullyInstantiateImports();

        // All done, so...

        return true;
    }
}

//==============================================================================

bool CellmlModel::reload()
{
    // We want to reload the model, so we must first reset it

    reset();

    // Now, we can try to load the model

    return load();
}

//==============================================================================

bool CellmlModel::isValid()
{
    // Load (but not reload!) the model, if needed

    if (load()) {
        // The model was properly loaded (or was already loaded), so check
        // whether it is CellML valid

        ObjRef<iface::cellml_services::VACSService> vacss = CreateVACSService();
        ObjRef<iface::cellml_services::CellMLValidityErrorSet> cellmlValidityErrorSet = vacss->validateModel(mModel);

        // Determine the number of errors and warnings
        // Note: CellMLValidityErrorSet::nValidityErrors() returns any type of
        //       validation issue, be it an error or a warning, so we need to
        //       determine the number of true errors

        uint32_t nbOfCellmlIssues = cellmlValidityErrorSet->nValidityErrors();
        uint32_t nbOfCellmlErrors = 0;

        for (uint32_t i = 0; i < nbOfCellmlIssues; ++i) {
            // Determine the issue's location

            ObjRef<iface::cellml_services::CellMLValidityError> cellmlValidityIssue = cellmlValidityErrorSet->getValidityError(i);

            DECLARE_QUERY_INTERFACE_OBJREF(cellmlRepresentationValidityError, cellmlValidityIssue,
                                           cellml_services::CellMLRepresentationValidityError);

            uint32_t line = CellMLSupport::Undefined;
            uint32_t column = CellMLSupport::Undefined;
            QString importedModel = QString();

            if (cellmlRepresentationValidityError) {
                // We are dealing with a CellML representation issue, so
                // determine its line and column

                ObjRef<iface::dom::Node> errorNode = cellmlRepresentationValidityError->errorNode();

                line = vacss->getPositionInXML(errorNode,
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

                    DECLARE_QUERY_INTERFACE_OBJREF(cellmlDomElement, cellmlElement, cellml_api::CellMLDOMElement);

                    ObjRef<iface::dom::Element> domElement = cellmlDomElement->domElement();

                    line = vacss->getPositionInXML(domElement, 0, &column);

                    // Also determine its imported model, if any

                    while (true) {
                        // Retrieve the CellML element's parent

                        iface::cellml_api::CellMLElement *cellmlElementParent = already_AddRefd<iface::cellml_api::CellMLElement>(cellmlElement->parentElement());

                        if (!cellmlElementParent)
                            // There is no parent, so...

                            break;

                        // Check whether the parent is an imported model

                        DECLARE_QUERY_INTERFACE_OBJREF(importedCellmlModel,
                                                       cellmlElementParent,
                                                       cellml_api::Model);

                        if (!importedCellmlModel)
                            // This is not a model, so...

                            continue;

                        // Retrieve the imported CellML element

                        ObjRef<iface::cellml_api::CellMLElement> importedCellmlElement = importedCellmlModel->parentElement();

                        if (!importedCellmlElement)
                            // This is not an imported CellML element, so...

                            break;

                        // Check whether the imported CellML element is an
                        // import CellML element

                        DECLARE_QUERY_INTERFACE(importCellmlElement,
                                                importedCellmlElement,
                                                cellml_api::CellMLImport);

                        if (!importCellmlElement)
                            // This is not an import CellML element, so...

                            break;

                        ObjRef<iface::cellml_api::URI> href = importCellmlElement->xlinkHref();
                        RETURN_INTO_WSTRING(hrefWideString, href->asText());

                        importedModel = QString::fromStdWString(hrefWideString);

                        break;
                    }
                }
            }

            // Determine the issue's type

            CellmlModelIssue::Type issueType;

            if (cellmlValidityIssue->isWarningOnly()) {
                // We are dealing with a warning

                issueType = CellmlModelIssue::Warning;
            } else {
                // We are dealing with an error

                ++nbOfCellmlErrors;

                issueType = CellmlModelIssue::Error;
            }

            // Address the issue of tracker item #3166
            // https://tracker.physiomeproject.org/show_bug.cgi?id=3166
            //---GRY--- THIS WILL HOPEFULLY GET FIXED AT SOME POINT...

            if (line != Undefined)
                ++line;

            // Append the issue to our list

            mIssues.append(CellmlModelIssue(issueType,
                                            QString::fromStdWString(cellmlValidityIssue->description()),
                                            line, column, importedModel));
        }

        if (nbOfCellmlErrors)
            // There are CellML errors, so...

            return false;

        // Everything went as expected, so...

        return true;
    } else {
        // Something went wrong with the loading of the model, so...

        return false;
    }
}

//==============================================================================

QList<CellmlModelIssue> CellmlModel::issues()
{
    // Return the issue(s)

    return mIssues;
}

//==============================================================================

CellmlModelRuntime CellmlModel::runtime()
{
    // Reset our runtime

    mRuntime.reset();

    // Check that the model is valid

    if (isValid()) {
    }

    // We are done, so return the resulting runtime

    return mRuntime;
}

//==============================================================================

}   // namespace CellMLSupport
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================

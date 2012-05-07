//==============================================================================
// CellML file class
//==============================================================================

#include "cellmlfile.h"

//==============================================================================

#include <QRegExp>
#include <QTime>
#include <QUrl>

//==============================================================================

#ifdef Q_WS_X11
    #include <stdint.h>
    // Note: the above header file is required on Linux, so we can use uint32_t
    //       (required to 'properly' make use of the CellML API). Now, we would
    //       normally have
    //           #include <cstdint>
    //       but this is not supported by the current ISO C++ standard. Indeed,
    //       to include it will generate the following error at compile time:
    //           error: #error This file requires compiler and library support
    //           for the upcoming ISO C++ standard, C++0x. This support is
    //           currently experimental, and must be enabled with the -std=c++0x
    //           or -std=gnu++0x compiler options.
    //       and well, we don't want to enable either of these options since
    //       support is still only experimental, so...
#endif

//==============================================================================

#include "IfaceVACSS.hxx"

#include "CellMLBootstrap.hpp"
#include "VACSSBootstrap.hpp"

//==============================================================================

namespace OpenCOR {
namespace CellMLSupport {

//==============================================================================

CellmlFile::CellmlFile(const QString &pFileName) :
    mFileName(pFileName),
    mCellmlApiModel(0),
    mModel(0),
    mImports(CellmlFileImports()),
    mUnits(CellmlFileUnits()),
    mComponents(CellmlFileComponents()),
    mGroups(CellmlFileGroups()),
    mConnections(CellmlFileConnections()),
    mMetadata(CellmlFileRdfTriples())
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

    reset();

    delete mRuntime;
}

//==============================================================================

void CellmlFile::reset()
{
    // Reset all of the file's properties
    // Note: setting mCellmlApiModel to zero will automatically delete the
    //       current instance, if any

    mCellmlApiModel = 0;

    delete mModel;

    clearImports();
    clearUnits();
    clearComponents();
    clearGroups();
    clearConnections();
    clearMetadata();

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

        mCellmlApiModel = modelLoader->loadFromURL(QUrl::fromLocalFile(mFileName).toString().toStdWString().c_str());

qDebug(" - CellML Loading time: %s s", qPrintable(QString::number(0.001*time.elapsed(), 'g', 3)));
    } catch (iface::cellml_api::CellMLException &) {
        // Something went wrong with the loading of the model, so...

        mIssues.append(CellmlFileIssue(CellmlFileIssue::Error,
                                       tr("the model could not be loaded (%1)").arg(QString::fromStdWString(modelLoader->lastErrorMessage()))));

        return false;
    }

    // In the case of a non CellML 1.0 model, we want all imports to be fully
    // instantiated

    if (QString::fromStdWString(mCellmlApiModel->cellmlVersion()).compare(Cellml_1_0))
        try {
QTime time;

time.start();

            mCellmlApiModel->fullyInstantiateImports();

qDebug(" - CellML full instantiation time: %s s", qPrintable(QString::number(0.001*time.elapsed(), 'g', 3)));
        } catch (...) {
            // Something went wrong with the full instantiation of the imports,
            // so...

            mIssues.append(CellmlFileIssue(CellmlFileIssue::Error,
                                           tr("the model's imports could not be fully instantiated")));

            return false;
        }

    // Extract/retrieve various things from mCellmlApiModel

    mModel = new CellmlFileModel(mCellmlApiModel);

    // Iterate through the imports and add them to our list

    ObjRef<iface::cellml_api::CellMLImportSet> imports = mCellmlApiModel->imports();
    ObjRef<iface::cellml_api::CellMLImportIterator> importIterator = imports->iterateImports();

    forever {
        ObjRef<iface::cellml_api::CellMLImport> import = importIterator->nextImport();

        if (import)
            // We have an import, so add it to our list

            mImports.append(new CellmlFileImport(import));
        else
            // No more imports, so...

            break;
    }

    // Iterate through the units and add them to our list

    ObjRef<iface::cellml_api::UnitsSet> units = mCellmlApiModel->localUnits();
    ObjRef<iface::cellml_api::UnitsIterator> unitIterator = units->iterateUnits();

    forever {
        ObjRef<iface::cellml_api::Units> unit = unitIterator->nextUnits();

        if (unit)
            // We have a unit, so add it to our list

            mUnits.append(new CellmlFileUnit(unit));
        else
            // No more units, so...

            break;
    }

    // Iterate through the components and add them to our list

    ObjRef<iface::cellml_api::CellMLComponentSet> components = mCellmlApiModel->localComponents();
    ObjRef<iface::cellml_api::CellMLComponentIterator> componentIterator = components->iterateComponents();

    forever {
        ObjRef<iface::cellml_api::CellMLComponent> component = componentIterator->nextComponent();

        if (component)
            // We have a component, so add it to our list

            mComponents.append(new CellmlFileComponent(component));
        else
            // No more components, so...

            break;
    }

    // Iterate through the groups and add them to our list

    ObjRef<iface::cellml_api::GroupSet> groups = mCellmlApiModel->groups();
    ObjRef<iface::cellml_api::GroupIterator> groupIterator = groups->iterateGroups();

    forever {
        ObjRef<iface::cellml_api::Group> group = groupIterator->nextGroup();

        if (group)
            // We have a group, so add it to our list

            mGroups.append(new CellmlFileGroup(group));
        else
            // No more groups, so...

            break;
    }

    // Iterate through the connections and add them to our list

    ObjRef<iface::cellml_api::ConnectionSet> connections = mCellmlApiModel->connections();
    ObjRef<iface::cellml_api::ConnectionIterator> connectionIterator = connections->iterateConnections();

    forever {
        ObjRef<iface::cellml_api::Connection> connection = connectionIterator->nextConnection();

        if (connection)
            // We have a connection, so add it to our list

            mConnections.append(new CellmlFileConnection(connection));
        else
            // No more connections, so...

            break;
    }

    // Retrieve all the metadata associated with the model

    ObjRef<iface::cellml_api::RDFRepresentation> rdfRepresentation = mCellmlApiModel->getRDFRepresentation(L"http://www.cellml.org/RDF/API");

    if (rdfRepresentation) {
        ObjRef<iface::rdf_api::RDFAPIRepresentation> rdfApiRepresentation;
        QUERY_INTERFACE(rdfApiRepresentation, rdfRepresentation,
                        rdf_api::RDFAPIRepresentation);

        if (rdfApiRepresentation) {
            ObjRef<iface::cellml_api::URI> xmlBase = mCellmlApiModel->xmlBase();
            QString uriBase = QString::fromStdWString(xmlBase->asText());

            ObjRef<iface::rdf_api::DataSource> dataSource = rdfApiRepresentation->source();
            ObjRef<iface::rdf_api::TripleSet> triples = dataSource->getAllTriples();
            ObjRef<iface::rdf_api::TripleEnumerator> tripleEnumerator = triples->enumerateTriples();

            forever {
                ObjRef<iface::rdf_api::Triple> triple = tripleEnumerator->getNextTriple();

                if (triple) {
                    // We have a triple, so retrieve its subject so we can
                    // determine to which group of triples it should be added
                    // Note: the triple is part of an rdf:Description element
                    //       which is itself part of a rdf:RDF element. The
                    //       rdf:Description element has an rdf:about attribute
                    //       which may or not have a value assigned to it. If no
                    //       value was assigned, then the triple is valid at the
                    //       whole CellML model level. On the other hand, if a
                    //       value was assigned (and of the format #<id>), then
                    //       it will be associated to any CellML element which
                    //       cmeta:id value is <id>. A couple of examples:
                    //
                    // <rdf:RDF xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#" xmlns:dc="http://purl.org/dc/elements/1.1/" xmlns:vCard="http://www.w3.org/2001/vcard-rdf/3.0#">
                    //   <rdf:Description rdf:about="">
                    //     <dc:creator rdf:parseType="Resource">
                    //       <vCard:EMAIL rdf:parseType="Resource">
                    //         <rdf:type rdf:resource="http://imc.org/vCard/3.0#internet"/>
                    //         <rdf:value>someone@somewhere.com</rdf:value>
                    //       </vCard:EMAIL>
                    //     </dc:creator>
                    //   </rdf:Description>
                    // </rdf:RDF>
                    //
                    // <variable units="micromolar" public_interface="out" cmeta:id="C_C" name="C" initial_value="0.01">
                    //   <rdf:RDF xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#" xmlns:bqbiol="http://biomodels.net/biology-qualifiers/">
                    //     <rdf:Description rdf:about="#C_C">
                    //       <bqbiol:isVersionOf>
                    //         <rdf:Bag>
                    //           <rdf:li rdf:resource="urn:miriam:uniprot:Q4KLA0"/>
                    //           <rdf:li rdf:resource="urn:miriam:interpro:IPR006670"/>
                    //           <rdf:li rdf:resource="urn:miriam:obo.sbo:sbo%3A0000252"/>
                    //         </rdf:Bag>
                    //       </bqbiol:isVersionOf>
                    //     </rdf:Description>
                    //   </rdf:RDF>
                    // </variable>

                    CellmlFileRdfTriple *rdfTriple = new CellmlFileRdfTriple(triple);

                    if (rdfTriple->subject()->type() == CellmlFileRdfTripleElement::UriReference)
                        // We have a triple of which we can make sense, so add
                        // it to the correct group of triples
                        // Note: we want the name of the group to be the same as
                        //       the cmeta:id of a CellML element. This means
                        //       that we must remove the URI base (and optional
                        //       hash character) which makes the beginning of
                        //       the triple's subject's URI reference...

                        mMetadata.insertMulti(rdfTriple->subject()->uriReference().remove(QRegExp("^"+QRegExp::escape(uriBase)+"#?")),
                                              rdfTriple);
                    else
                        // Not a triple we recognise, so...

                        delete rdfTriple;
                } else {
                    // No more triples, so...

                    break;
                }
            }
        }
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
        ObjRef<iface::cellml_services::CellMLValidityErrorSet> cellmlValidityErrorSet = vacssService->validateModel(mCellmlApiModel);

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

                    forever {
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

        mRuntime->update(mCellmlApiModel);

        mRuntimeUpdateNeeded = !mRuntime->isValid();

        return mRuntime;
    } else {
        // The file isn't valid, so reset its runtime

        return mRuntime->update();
    }
}

//==============================================================================

CellmlFileModel * CellmlFile::model() const
{
    // Return the CellML file's model

    return mModel;
}

//==============================================================================

CellmlFileImports CellmlFile::imports() const
{
    // Return the CellML file's imports

    return mImports;
}

//==============================================================================

CellmlFileUnits CellmlFile::units() const
{
    // Return the CellML file's units

    return mUnits;
}

//==============================================================================

CellmlFileComponents CellmlFile::components() const
{
    // Return the CellML file's components

    return mComponents;
}

//==============================================================================

CellmlFileGroups CellmlFile::groups() const
{
    // Return the CellML file's groups

    return mGroups;
}

//==============================================================================

CellmlFileConnections CellmlFile::connections() const
{
    // Return the CellML file's connections

    return mConnections;
}

//==============================================================================

CellmlFileRdfTriples CellmlFile::metadata() const
{
    // Return the CellML file's metadata

    return mMetadata;
}

//==============================================================================

void CellmlFile::clearImports()
{
    // Delete all the imports and clear our list

    foreach (CellmlFileImport *import, mImports)
        delete import;

    mImports.clear();
}

//==============================================================================

void CellmlFile::clearUnits()
{
    // Delete all the units and clear our list

    foreach (CellmlFileUnit *unit, mUnits)
        delete unit;

    mUnits.clear();
}

//==============================================================================

void CellmlFile::clearComponents()
{
    // Delete all the components and clear our list

    foreach (CellmlFileComponent *component, mComponents)
        delete component;

    mComponents.clear();
}

//==============================================================================

void CellmlFile::clearGroups()
{
    // Delete all the groups and clear our list

    foreach (CellmlFileGroup *group, mGroups)
        delete group;

    mGroups.clear();
}

//==============================================================================

void CellmlFile::clearConnections()
{
    // Delete all the connections and clear our list

    foreach (CellmlFileConnection *connection, mConnections)
        delete connection;

    mConnections.clear();
}

//==============================================================================

void CellmlFile::clearMetadata()
{
    // Delete all the metadata and clear our list

    foreach (CellmlFileRdfTriple *triple, mMetadata)
        delete triple;

    mMetadata.clear();
}

//==============================================================================

}   // namespace CellMLSupport
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================

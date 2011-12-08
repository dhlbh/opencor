//==============================================================================
// CellML model class
//==============================================================================

#include "cellmlmodel.h"

//==============================================================================

#include "CellMLBootstrap.hpp"

//==============================================================================

namespace OpenCOR {
namespace CellMLSupport {

//==============================================================================

CellmlModel::CellmlModel(const QUrl &pUrl) :
    mUrl(pUrl)
{
    // Get a bootstrap object

    ObjRef<iface::cellml_api::CellMLBootstrap> cellmlBootstrap = CreateCellMLBootstrap();

    // Retrieve the model loader

    ObjRef<iface::cellml_api::DOMModelLoader> modelLoader = cellmlBootstrap->modelLoader();

    // Load the CellML model which URL we were given

    try {
        mModel = modelLoader->loadFromURL(pUrl.toString().toStdWString().c_str());
    } catch (iface::cellml_api::CellMLException &) {
        // Something went wrong, so generate an error message

        mErrorMessage = QString("Model loader error: %1").arg(QString::fromWCharArray(modelLoader->lastErrorMessage()));

        return;
    }

    // In the case of a CellML 1.1 model, we want all imports to be fully
    // instantiated

    if (!QString::fromWCharArray(mModel->cellmlVersion()).compare("1.0"))
        mModel->fullyInstantiateImports();
}

//==============================================================================

bool CellmlModel::isValid()
{
    // The CellML model object is valid if there is no error message

    return mErrorMessage.isEmpty();
}

//==============================================================================

QString CellmlModel::errorMessage()
{
    // Return the error message

    return mErrorMessage;
}

//==============================================================================

}   // namespace CellMLSupport
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================

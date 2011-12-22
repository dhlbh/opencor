//==============================================================================
// CellML model runtime class
//==============================================================================

#include "cellmlmodelruntime.h"

//==============================================================================

#include <QDebug>

//==============================================================================

#include "CCGSBootstrap.hpp"

//==============================================================================

namespace OpenCOR {
namespace CellMLSupport {

//==============================================================================

CellmlModelRuntime::CellmlModelRuntime()
{
    // Initialise the runtime's properties

    reset();
}

//==============================================================================

bool CellmlModelRuntime::isValid()
{
    // The runtime is valid if no issue was found

    return mIssues.isEmpty();
}

//==============================================================================

CellmlModelRuntime::ModelType CellmlModelRuntime::modelType()
{
    // Return the type of model for the runtime

    return mModelType;
}

//==============================================================================

QList<CellmlModelIssue> CellmlModelRuntime::issues()
{
    // Return the issue(s)

    return mIssues;
}

//==============================================================================

void CellmlModelRuntime::resetOdeCodeInformation()
{
    /*delete mOdeCodeInformation;*/ mOdeCodeInformation = 0;
    //---GRY--- WE CANNOT delete mOdeCodeInformation AT THIS STAGE. FOR THIS, WE
    //          WOULD NEED TO USE THE CLEANED UP C++ INTERFACE (SEE THE MAIN
    //          COMMENT AT THE BEGINNING OF THE cellmlmodel.cpp FILE)
}

//==============================================================================

void CellmlModelRuntime::resetDaeCodeInformation()
{
    /*delete mDaeCodeInformation;*/ mDaeCodeInformation = 0;
    //---GRY--- WE CANNOT delete mDaeCodeInformation AT THIS STAGE. FOR THIS, WE
    //          WOULD NEED TO USE THE CLEANED UP C++ INTERFACE (SEE THE MAIN
    //          COMMENT AT THE BEGINNING OF THE cellmlmodel.cpp FILE)
}

//==============================================================================

void CellmlModelRuntime::reset()
{
    // Reset all of the runtime's properties

//---GRY--- TO BE DONE...

    mModelType = Undefined;

    resetOdeCodeInformation();
    resetDaeCodeInformation();

    mIssues.clear();
}

//==============================================================================

CellmlModelRuntime::ModelType CellmlModelRuntime::getModelType(iface::cellml_api::Model *pModel)
{
    // Determine the type of model we are dealing with (i.e. either a 'simple'
    // ODE model or a DAE model)
    // Note #1: in order to do so, we need to get a 'normal' code generator (as
    //          opposed an IDA, i.e. DAE, code generator) since if the model is
    //          correctly constraint, then we can check whether some of its
    //          equations were flagged as needing a Newton-Raphson evaluation in
    //          which case we would be dealing with a DAE model...
    // Note #2: ideally, there would be a more convenient way to determine the
    //          type of a model, but well... there isn't, so...

    // Get a code generator bootstrap and then create a code generator

    ObjRef<iface::cellml_services::CodeGeneratorBootstrap> codeGeneratorBootstrap = CreateCodeGeneratorBootstrap();
    ObjRef<iface::cellml_services::CodeGenerator> codeGenerator = codeGeneratorBootstrap->createCodeGenerator();

    // Generate some code for the model (i.e. 'compile' the model)

    try {
        mOdeCodeInformation = codeGenerator->generateCode(pModel);

        // Check that the code generation went fine

        QString codeGenerationErrorMessage = QString::fromStdWString(mOdeCodeInformation->errorMessage());

        if (codeGenerationErrorMessage.isEmpty()) {
            // The code generation went fine, so check the model's constraint
            // level

            iface::cellml_services::ModelConstraintLevel constraintLevel = mOdeCodeInformation->constraintLevel();

            if (constraintLevel == iface::cellml_services::UNDERCONSTRAINED) {
                mIssues.append(CellmlModelIssue(CellmlModelIssue::Error,
                                                tr("the model is underconstrained (i.e. some variables need to be initialised or computed)")));
            } else if (constraintLevel == iface::cellml_services::UNSUITABLY_CONSTRAINED) {
                mIssues.append(CellmlModelIssue(CellmlModelIssue::Error,
                                                tr("the model is unsuitably constrained (i.e. some variables could not be found and/or some equations could not be used)")));
            } else if (constraintLevel == iface::cellml_services::OVERCONSTRAINED) {
                mIssues.append(CellmlModelIssue(CellmlModelIssue::Error,
                                                tr("the model is overconstrained (i.e. some variables are either both initialised and computed or computed more than once)")));
            }
        } else {
            // The code generation didn't go fine, so...

            mIssues.append(CellmlModelIssue(CellmlModelIssue::Error,
                                            tr("a problem occurred during the compilation of the model: %1").arg(codeGenerationErrorMessage.toLatin1().constData())));
        }
    } catch (iface::cellml_api::CellMLException &) {
        mIssues.append(CellmlModelIssue(CellmlModelIssue::Error,
                                        tr("the model could not be compiled")));
    } catch (...) {
        mIssues.append(CellmlModelIssue(CellmlModelIssue::Error,
                                        tr("an unexpected problem occurred while trying to compile the model")));
    }

    if (mIssues.count()) {
        // Something went wrong at some point, so...

        resetOdeCodeInformation();

        return Undefined;
    } else {
        // Everything went fine, so return the model's type (i.e. a 'simple'
        // ODE model or a DAE model)
        // Note: this can be done by checking whether some equations were
        //       flagged as needing a Newton-Raphson evaluation...

        return mOdeCodeInformation->flaggedEquations()->length()?Dae:Ode;
    }
}

//==============================================================================

CellmlModelRuntime * CellmlModelRuntime::update(iface::cellml_api::Model *pModel,
                                                const bool &pValidModel)
{
    // Reset the runtime's properties

    reset();

    // Check that the model is valid

    if (pValidModel) {
        // The model is valid, so retrieve its type

        mModelType = getModelType(pModel);



//        qDebug("---------------------------------------");
//        qDebug("\nRequired functions:");
//        qDebug(QString::fromStdWString(codeInformation->functionsString()).toLatin1().constData());
//        qDebug("---------------------------------------");
//        qDebug("\nInitialisations:");
//        qDebug(QString::fromStdWString(codeInformation->initConstsString()).toLatin1().constData());
//        qDebug("---------------------------------------");
//        qDebug("\nVariable evaluations:");
//        qDebug(QString::fromStdWString(codeInformation->variablesString()).toLatin1().constData());
//        qDebug("---------------------------------------");



        // Just testing the generation of 'C code'...

//qDebug() << QString::fromStdWString(getModelAsCCode(pModel));
    } else {
        // The model is not valid, so...

        mIssues.append(CellmlModelIssue(CellmlModelIssue::Error,
                                        tr("the model is not valid")));
    }

    // We are done, so return ourselves

    return this;
}

//==============================================================================

}   // namespace CellMLSupport
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================

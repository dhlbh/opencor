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

void CellmlModelRuntime::couldNotCompileModelIssue()
{
    mIssues.append(CellmlModelIssue(CellmlModelIssue::Error,
                                    tr("the model could not be compiled")));
}

//==============================================================================

void CellmlModelRuntime::unexpectedProblemDuringModelCompilationIssue()
{
    mIssues.append(CellmlModelIssue(CellmlModelIssue::Error,
                                    tr("an unexpected problem occurred while trying to compile the model")));
}

//==============================================================================

void CellmlModelRuntime::checkCodeInformation(iface::cellml_services::CodeInformation *pCodeInformation)
{
    if (!pCodeInformation)
        // No code information was provided, so...

        return;

    // Retrieve the code information's latest error message

    QString codeGenerationErrorMessage = QString::fromStdWString(pCodeInformation->errorMessage());

    if (codeGenerationErrorMessage.isEmpty()) {
        // The code generation went fine, so check the model's constraint level

        iface::cellml_services::ModelConstraintLevel constraintLevel = pCodeInformation->constraintLevel();

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
}

//==============================================================================

void CellmlModelRuntime::getOdeCodeInformation(iface::cellml_api::Model *pModel)
{
    // Get a code generator bootstrap and create an ODE code generator

    ObjRef<iface::cellml_services::CodeGeneratorBootstrap> codeGeneratorBootstrap = CreateCodeGeneratorBootstrap();
    ObjRef<iface::cellml_services::CodeGenerator> codeGenerator = codeGeneratorBootstrap->createCodeGenerator();

    // Generate some code for the model (i.e. 'compile' the model)

    try {
        mOdeCodeInformation = codeGenerator->generateCode(pModel);

        // Check that the code generation went fine

        checkCodeInformation(mOdeCodeInformation);
    } catch (iface::cellml_api::CellMLException &) {
        couldNotCompileModelIssue();
    } catch (...) {
        unexpectedProblemDuringModelCompilationIssue();
    }

    // Check the outcome of the ODE code generation

    if (mIssues.count())
        // Something went wrong at some point, so...

        resetOdeCodeInformation();
}

//==============================================================================

void CellmlModelRuntime::getDaeCodeInformation(iface::cellml_api::Model *pModel)
{
    // Get a code generator bootstrap and create a DAE code generator

    ObjRef<iface::cellml_services::CodeGeneratorBootstrap> codeGeneratorBootstrap = CreateCodeGeneratorBootstrap();
    ObjRef<iface::cellml_services::IDACodeGenerator> codeGenerator = codeGeneratorBootstrap->createIDACodeGenerator();

    // Generate some code for the model (i.e. 'compile' the model)

    try {
        mDaeCodeInformation = codeGenerator->generateIDACode(pModel);

        // Check that the code generation went fine

        checkCodeInformation(mDaeCodeInformation);
    } catch (iface::cellml_api::CellMLException &) {
        couldNotCompileModelIssue();
    } catch (...) {
        unexpectedProblemDuringModelCompilationIssue();
    }

    // Check the outcome of the DAE code generation

    if (mIssues.count())
        // Something went wrong at some point, so...

        resetDaeCodeInformation();
}

//==============================================================================

CellmlModelRuntime * CellmlModelRuntime::update(iface::cellml_api::Model *pModel,
                                                const bool &pValidModel)
{
    // Reset the runtime's properties

    reset();

    // Check that the model is valid

    if (pValidModel) {
        // The model is valid, so make sure that it's either a 'simple' ODE
        // model or a DAE model
        // Note #1: in order to do so, we need to get a 'normal' code generator
        //          (as opposed to an IDA, i.e. DAE, code generator) since if
        //          the model is correctly constrained, then we can check
        //          whether some of its equations were flagged as needing a
        //          Newton-Raphson evaluation, in which case we would be dealing
        //          with a DAE model...
        // Note #2: ideally, there would be a more convenient way to determine
        //          the type of a model, but well... there isn't, so...

        getOdeCodeInformation(pModel);

        // Retrieve the model's type
        // Note: this can be done by checking whether some equations were
        //       flagged as needing a Newton-Raphson evaluation...

        if (mOdeCodeInformation) {
            // An ODE code information could be retrieved, so we can determine
            // the model's type

            mModelType = mOdeCodeInformation->flaggedEquations()->length()?Dae:Ode;

            // If the model is of DAE type, then we must get the 'right' code
            // information

            if (mModelType == Dae)
                getDaeCodeInformation(pModel);

            // Retrieve some code

            if (mModelType ==Ode) {
                qDebug("---------------------------------------");
                qDebug("\ninitConstsString():");
                qDebug(QString::fromStdWString(mOdeCodeInformation->initConstsString()).toLatin1().constData());
                qDebug("---------------------------------------");
                qDebug("\nratesString():");
                qDebug(QString::fromStdWString(mOdeCodeInformation->ratesString()).toLatin1().constData());
                qDebug("---------------------------------------");
                qDebug("\nvariablesString():");
                qDebug(QString::fromStdWString(mOdeCodeInformation->variablesString()).toLatin1().constData());
                qDebug("---------------------------------------");
                qDebug("\nfunctionsString():");
                qDebug(QString::fromStdWString(mOdeCodeInformation->functionsString()).toLatin1().constData());
                qDebug("---------------------------------------");
            } else {
                qDebug("---------------------------------------");
                qDebug("\ninitConstsString():");
                qDebug(QString::fromStdWString(mDaeCodeInformation->initConstsString()).toLatin1().constData());
                qDebug("---------------------------------------");
                qDebug("\nratesString():");
                qDebug(QString::fromStdWString(mDaeCodeInformation->ratesString()).toLatin1().constData());
                qDebug("---------------------------------------");
                qDebug("\nvariablesString():");
                qDebug(QString::fromStdWString(mDaeCodeInformation->variablesString()).toLatin1().constData());
                qDebug("---------------------------------------");
                qDebug("\nfunctionsString():");
                qDebug(QString::fromStdWString(mDaeCodeInformation->functionsString()).toLatin1().constData());
                qDebug("---------------------------------------");
                qDebug("\nessentialVariablesString():");
                qDebug(QString::fromStdWString(mDaeCodeInformation->essentialVariablesString()).toLatin1().constData());
                qDebug("---------------------------------------");
                qDebug("\nstateInformationString():");
                qDebug(QString::fromStdWString(mDaeCodeInformation->stateInformationString()).toLatin1().constData());
                qDebug("---------------------------------------");
                qDebug("\nrootInformationString():");
                qDebug(QString::fromStdWString(mDaeCodeInformation->rootInformationString()).toLatin1().constData());
                qDebug("---------------------------------------");
            }
        } else {
            // No ODE code information could be retrieved, so...

            mModelType = Undefined;
        }
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

//==============================================================================
// IDA solver class
//==============================================================================

#ifndef IDASOLVER_H
#define IDASOLVER_H

//==============================================================================

#include "coredaesolver.h"

//==============================================================================

#include "nvector/nvector_serial.h"

//==============================================================================

namespace OpenCOR {
namespace IDASolver {

//==============================================================================

static const QString MaximumStepProperty = "Maximum step";
static const QString MaximumNumberOfStepsProperty = "Maximum number of steps";
static const QString RelativeToleranceProperty = "Relative tolerance";
static const QString AbsoluteToleranceProperty = "Absolute tolerance";

//==============================================================================

struct IDASolverUserData
{
    double *constants;
    double *algebraic;
    double *condVar;

    double *oldRates;
    double *oldStates;

    OpenCOR::CoreSolver::CoreDaeSolver::ComputeResidualsFunction computeResiduals;
    OpenCOR::CoreSolver::CoreDaeSolver::ComputeEssentialVariablesFunction computeEssentialVariables;
    OpenCOR::CoreSolver::CoreDaeSolver::ComputeRootInformationFunction computeRootInformation;
};

//==============================================================================

class IDASolver : public CoreSolver::CoreDaeSolver
{
public:
    explicit IDASolver();
    ~IDASolver();

    virtual void initialize(const double &pVoiStart, const int &pStatesCount,
                            const int &pCondVarCount, double *pConstants,
                            double *pRates, double *pStates, double *pAlgebraic,
                            double *pCondVar,
                            ComputeEssentialVariablesFunction pComputeEssentialVariables,
                            ComputeResidualsFunction pComputeResiduals,
                            ComputeRootInformationFunction pComputeRootInformation);

    virtual void solve(double &pVoi, const double &pVoiEnd) const;

private:
    void *mSolver;
    N_Vector mStatesVector;
    N_Vector mRatesVector;
    IDASolverUserData mUserData;

    double mMaximumStep;
    int mMaximumNumberOfSteps;
    double mRelativeTolerance;
    double mAbsoluteTolerance;

    virtual bool isValidProperty(const QString &pName) const;
};

//==============================================================================

}   // namespace IDASolver
}   // namespace OpenCOR

//==============================================================================

#endif

//==============================================================================
// End of file
//==============================================================================

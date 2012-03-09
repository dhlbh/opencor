//==============================================================================
// CVODE solver class
//==============================================================================

#ifndef CVODESOLVER_H
#define CVODESOLVER_H

//==============================================================================

#include "coreodesolver.h"

//==============================================================================

#include "nvector/nvector_serial.h"

//==============================================================================

namespace OpenCOR {
namespace CVODESolver {

//==============================================================================

static const QString MaximumStepProperty = "Maximum step";
static const QString MaximumNumberOfStepsProperty = "Maximum number of steps";
static const QString RelativeToleranceProperty = "Relative tolerance";
static const QString AbsoluteToleranceProperty = "Absolute tolerance";

//==============================================================================

class CVODESolver : public CoreSolver::CoreOdeSolver
{
public:
    explicit CVODESolver();
    ~CVODESolver();

    virtual void initialize(const double &pVoiStart, const int &pStatesCount,
                            double *pConstants, double *pRates,
                            double *pStates, double *pAlgebraic);

    virtual void solve(double &pVoi, const double &pVoiEnd,
                       OpenCOR::CoreSolver::CoreOdeSolver::ComputeRatesFunction pComputeRates) const;

private:
    void *mSolver;
    N_Vector mStates;

    double mMaximumStep;
    int mMaximumNumberOfSteps;
    double mRelativeTolerance;
    double mAbsoluteTolerance;

    virtual bool isValidProperty(const QString &pName) const;
};

//==============================================================================

}   // namespace CVODESolver
}   // namespace OpenCOR

//==============================================================================

#endif

//==============================================================================
// End of file
//==============================================================================

//==============================================================================
// Computer external function class
//==============================================================================

#include "computerexternalfunction.h"

//==============================================================================

namespace OpenCOR {
namespace Computer {

//==============================================================================

ComputerExternalFunction::ComputerExternalFunction(const QString &pName,
                                                   const int &pNbOfParameters,
                                                   void *pFunction) :
    mName(pName),
    mNbOfParameters(pNbOfParameters),
    mFunction(pFunction)
{
}

//==============================================================================

QString ComputerExternalFunction::name() const
{
    // Return the external function's name

    return mName;
}

//==============================================================================

int ComputerExternalFunction::nbOfParameters() const
{
    // Return the external function's number of parameters

    return mNbOfParameters;
}

//==============================================================================

void * ComputerExternalFunction::function() const
{
    // Return the external function's function

    return mFunction;
}

//==============================================================================

bool ComputerExternalFunctions::contains(const ComputerExternalFunction &pExternalFunction) const
{
    // Check whether an external function is already in our list of external
    // functions needed by the function
    // Note: we only check the name of the external function, not the number of
    //       its parameters (since the latter should always be the same for a
    //       given external function)...

    for (int i = 0, iMax = count(); i < iMax; ++i) {
        const ComputerExternalFunction &externalFunction = at(i);

        if (!externalFunction.name().compare(pExternalFunction.name()))
            // The external function is already in our list of external
            // functions, so...

            return true;
    }

    // The external function couldn't be found in our list of external
    // functions, so...

    return false;
}

//==============================================================================

}   // namespace Computer
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================

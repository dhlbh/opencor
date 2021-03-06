//==============================================================================
// CoreSolver plugin
//==============================================================================

#ifndef CORESOLVERPLUGIN_H
#define CORESOLVERPLUGIN_H

//==============================================================================

#include "plugininfo.h"

//==============================================================================

namespace OpenCOR {
namespace CoreSolver {

//==============================================================================

PLUGININFO_FUNC CoreSolverPluginInfo();

//==============================================================================

class CoreSolverPlugin : public QObject
{
    Q_OBJECT
};

//==============================================================================

}   // namespace CoreSolver
}   // namespace OpenCOR

//==============================================================================

#endif

//==============================================================================
// End of file
//==============================================================================

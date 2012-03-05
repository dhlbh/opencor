//==============================================================================
// ForwardEulerSolver plugin
//==============================================================================

#include "forwardeulersolverplugin.h"

//==============================================================================

namespace OpenCOR {
namespace ForwardEulerSolver {

//==============================================================================

PLUGININFO_FUNC ForwardEulerSolverPluginInfo()
{
    Descriptions descriptions;

    descriptions.insert("en", "An implementation of the Forward Euler method to solve ODEs");
    descriptions.insert("fr", "Une impl�mentation de la m�thode Forward Euler pour r�soudres des EDOs");

    return PluginInfo(PluginInfo::V001,
                      PluginInfo::General,
                      PluginInfo::Simulation,
                      true,
                      QStringList() << "Core",
                      descriptions);
}

//==============================================================================

Q_EXPORT_PLUGIN2(ForwardEulerSolver, ForwardEulerSolverPlugin)

//==============================================================================

}   // namespace ForwardEulerSolver
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================

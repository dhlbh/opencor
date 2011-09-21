#include "viewerplugin.h"

namespace OpenCOR {
namespace Viewer {

PLUGININFO_FUNC ViewerPluginInfo()
{
    PluginInfoDescriptions descriptions;

    descriptions.insert("en", "A plugin to graphically visualise various concepts (e.g. mathematical equations)");
    descriptions.insert("fr", "Une extension pour visualiser graphiquement diff�rents concepts (par exemple des �quations math�matiques)");

    return PluginInfo(PluginInterface::V001,
                      PluginInfo::Gui,
                      PluginInfo::Editing,
                      true,
                      QStringList() << "CoreEditing" << "QtMmlWidget",
                      descriptions);
}

Q_EXPORT_PLUGIN2(Viewer, ViewerPlugin)

ViewerPlugin::ViewerPlugin() :
    GuiInterface("Viewer")
{
}

} }

#include "viewerplugin.h"

namespace OpenCOR {
namespace Viewer {

PLUGININFO_FUNC ViewerPluginInfo()
{
    QMap<QString, QString> descriptions;

    descriptions.insert("en", "A plugin to graphically visualise mathematical equations");
    descriptions.insert("fr", "Une extension pour visualiser graphiquement des �quations math�matiques");

    return PluginInfo(PluginInfo::Gui,
                       QStringList() << "Core" << "QtMmlWidget",
                       descriptions);
}

Q_EXPORT_PLUGIN2(Viewer, ViewerPlugin)

ViewerPlugin::ViewerPlugin() :
    GuiInterface("Viewer")
{
}

} }

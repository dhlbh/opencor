#include "filebrowserplugin.h"

namespace OpenCOR {
namespace FileBrowser {

PLUGININFO_FUNC FileBrowserPluginInfo()
{
    QMap<QString, QString> descriptions;

    descriptions.insert("en", "A plugin to access your local files");
    descriptions.insert("fr", "Une extension pour acc�der � vos fichiers locaux");

    return PluginInfo(PluginInterface::V001, PluginInfo::Gui, true,
                      QStringList() << "Core",
                      descriptions);
}

Q_EXPORT_PLUGIN2(FileBrowser, FileBrowserPlugin)

FileBrowserPlugin::FileBrowserPlugin() :
    GuiInterface("FileBrowser")
{
}

} }

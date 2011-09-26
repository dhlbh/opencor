#include "fileorganiserplugin.h"

namespace OpenCOR {
namespace FileOrganiser {

PLUGININFO_FUNC FileOrganiserPluginInfo()
{
    Descriptions descriptions;

    descriptions.insert("en", "A plugin to organise your files");
    descriptions.insert("fr", "Une extension pour organiser vos fichiers");

    return PluginInfo(PluginInfo::V001,
                      PluginInfo::Gui,
                      PluginInfo::Organisation,
                      true,
                      QStringList() << "Core",
                      descriptions);
}

Q_EXPORT_PLUGIN2(FileOrganiser, FileOrganiserPlugin)

} }

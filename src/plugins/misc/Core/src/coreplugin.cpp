#include "coreplugin.h"

namespace OpenCOR {
namespace Core {

PLUGININFO_FUNC CorePluginInfo()
{
    QMap<QString, QString> descriptions;

    descriptions.insert("en", "The core plugin for OpenCOR");
    descriptions.insert("fr", "L'extension de base pour OpenCOR");

    return PluginInfo(PluginInfo::General,
                      QStringList(),
                      descriptions);
}

Q_EXPORT_PLUGIN2(Core, CorePlugin)

} }

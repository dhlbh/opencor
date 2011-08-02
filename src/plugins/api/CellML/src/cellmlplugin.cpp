#include "cellmlplugin.h"

namespace OpenCOR {
namespace CellML {

PLUGININFO_FUNC CellMLPluginInfo()
{
    QMap<QString, QString> descriptions;

    descriptions.insert("en", "A plugin to use the <a href=\"http://www.cellml.org/tools/api/\">CellML API</a>");
    descriptions.insert("fr", "Une extension pour utiliser l'<a href=\"http://www.cellml.org/tools/api/\">API CellML</a>");

    return PluginInfo(PluginInterface::V001, PluginInfo::Gui,
                      PluginInfo::Api, false,
                      QStringList() << "Core",
                      descriptions);
}

Q_EXPORT_PLUGIN2(CellML, CellMLPlugin)

} }

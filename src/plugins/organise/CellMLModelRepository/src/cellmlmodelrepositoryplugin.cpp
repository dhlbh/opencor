#include "cellmlmodelrepositoryplugin.h"

namespace OpenCOR {
namespace CellMLModelRepository {

PLUGININFO_FUNC CellMLModelRepositoryPluginInfo()
{
    QMap<QString, QString> descriptions;

    descriptions.insert("en", "A plugin to access the <a href=\"http://models.cellml.org/\">CellML Model Repository</a>");
    descriptions.insert("fr", "Une extension pour acc�der au <a href=\"http://models.cellml.org/\">R�pertoire de Mod�les CellML</a>");

    return PluginInfo(PluginInterface::V001, PluginInfo::Gui, true,
                      QStringList() << "Core",
                      descriptions);
}

Q_EXPORT_PLUGIN2(CellMLModelRepository, CellMLModelRepositoryPlugin)

CellMLModelRepositoryPlugin::CellMLModelRepositoryPlugin() :
    GuiInterface("CellMLModelRepository")
{
}

} }

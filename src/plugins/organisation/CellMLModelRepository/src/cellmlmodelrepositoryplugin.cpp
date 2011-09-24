#include "cellmlmodelrepositoryplugin.h"

namespace OpenCOR {
namespace CellMLModelRepository {

PLUGININFO_FUNC CellMLModelRepositoryPluginInfo()
{
    Descriptions descriptions;

    descriptions.insert("en", "A plugin to access the <a href=\"http://models.cellml.org/\">CellML Model Repository</a>");
    descriptions.insert("fr", "Une extension pour acc�der au <a href=\"http://models.cellml.org/\">R�pertoire de Mod�les CellML</a>");

    return PluginInfo(CoreInterface::V001,
                      PluginInfo::Gui,
                      PluginInfo::Organisation,
                      true,
                      QStringList() << "Core",
                      descriptions);
}

Q_EXPORT_PLUGIN2(CellMLModelRepository, CellMLModelRepositoryPlugin)

CellMLModelRepositoryPlugin::CellMLModelRepositoryPlugin() :
    GuiInterface("CellMLModelRepository")
{
}

} }

#include "cellmlmodelrepositoryplugin.h"

namespace OpenCOR {

PLUGININFO_FUNC CellMLModelRepositoryPluginInfo()
{
    PluginInfo info;

    info.type         = Gui;
    info.dependencies = QStringList() << "Core";

    return info;
}

Q_EXPORT_PLUGIN2(CellMLModelRepository, CellMLModelRepositoryPlugin)

}
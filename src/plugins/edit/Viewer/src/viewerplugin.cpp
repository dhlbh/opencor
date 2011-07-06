#include "viewerplugin.h"

#include <QMessageBox>

namespace OpenCOR {
namespace Viewer {

PLUGININFO_FUNC ViewerPluginInfo()
{
    PluginInfo res;

    res.type         = PluginInfo::Gui;
    res.dependencies = QStringList() << "Core" << "QtMmlWidget";

    return res;
}

void ViewerPlugin::setupUi(QMainWindow *pMainWindow)
{
//---GRY--- TO BE DONE...

QMessageBox::information(0, "Viewer plugin", "Viewer plugin...");
}

Q_EXPORT_PLUGIN2(Viewer, ViewerPlugin)

} }

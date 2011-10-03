#include "rawviewplugin.h"

#include <QTabBar>

namespace OpenCOR {
namespace RawView {

PLUGININFO_FUNC RawViewPluginInfo()
{
    Descriptions descriptions;

    descriptions.insert("en", "A plugin to edit files using a text editor");
    descriptions.insert("fr", "Une extension pour �diter des fichiers � l'aide d'un �diteur de texte");

    return PluginInfo(PluginInfo::V001,
                      PluginInfo::Gui,
                      PluginInfo::Editing,
                      true,
                      QStringList() << "CoreEditing" << "QScintilla",
                      descriptions);
}

Q_EXPORT_PLUGIN2(RawView, RawViewPlugin)

RawViewPlugin::RawViewPlugin()
{
    // Set our settings

    mGuiSettings->addView(GuiViewSettings::Editing);
}

void RawViewPlugin::retranslateUi()
{
    // Retranslate our view's tab

    GuiViewSettings *view = mGuiSettings->views().at(0);

    view->tabBar()->setTabText(view->tabIndex(), tr("Raw"));
}

} }

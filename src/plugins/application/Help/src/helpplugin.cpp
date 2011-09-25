#include "helpplugin.h"
#include "helpwindow.h"

#include <QAction>
#include <QMainWindow>

namespace OpenCOR {
namespace Help {

PLUGININFO_FUNC HelpPluginInfo()
{
    Descriptions descriptions;

    descriptions.insert("en", "A plugin to provide help");
    descriptions.insert("fr", "Une extension pour fournir de l'aide");

    return PluginInfo(PluginInfo::V001,
                      PluginInfo::Gui,
                      PluginInfo::Application,
                      true,
                      QStringList() << "Core",
                      descriptions);
}

Q_EXPORT_PLUGIN2(Help, HelpPlugin)

HelpPlugin::HelpPlugin() :
    GuiInterface("Help")
{
}

void HelpPlugin::initialize()
{
    // Create an action to show/hide our help window

    mHelpAction = newAction(mMainWindow, true,
                            ":/oxygen/apps/help-browser.png");

    // Create our help window

    mHelpWindow = new HelpWindow(mMainWindow);

    // Create and set our data

    mData = new GuiHelpSettings(mHelpAction, mHelpWindow);

    // Some connections to handle the visibility of our help window

    connect(mHelpAction, SIGNAL(triggered(bool)),
            mHelpWindow, SLOT(setVisible(bool)));
    connect(mHelpWindow, SIGNAL(visibilityChanged(bool)),
            mHelpAction, SLOT(setChecked(bool)));
}

void HelpPlugin::finalize()
{
    // Delete our data

    delete (GuiHelpSettings *) mData;
}

void HelpPlugin::retranslateUi()
{
    // Retranslate our action

    retranslateAction(mHelpAction, tr("&Help"),
                      tr("Show/hide the OpenCOR help"),
                      tr("F1"));

    // Retranslate our help window

    mHelpWindow->retranslateUi();
}

} }

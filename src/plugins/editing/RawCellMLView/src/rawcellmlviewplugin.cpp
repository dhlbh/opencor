//==============================================================================
// RawCellMLView plugin
//==============================================================================

#include "cellmlfilemanager.h"
#include "rawcellmlviewplugin.h"
#include "rawcellmlviewwidget.h"

//==============================================================================

#include <QMainWindow>

//==============================================================================

namespace OpenCOR {
namespace RawCellMLView {

//==============================================================================

PLUGININFO_FUNC RawCellMLViewPluginInfo()
{
    Descriptions descriptions;

    descriptions.insert("en", "A plugin to edit <a href=\"http://www.cellml.org/\">CellML</a> files using an XML editor");
    descriptions.insert("fr", "Une extension pour �diter des fichiers <a href=\"http://www.cellml.org/\">CellML</a> � l'aide d'un �diteur XML");

    return PluginInfo(PluginInfo::V001,
                      PluginInfo::Gui,
                      PluginInfo::Editing,
                      true,
                      QStringList() << "CoreCellMLEditing" << "QScintillaSupport",
                      descriptions);
}

//==============================================================================

Q_EXPORT_PLUGIN2(RawCellMLView, RawCellMLViewPlugin)

//==============================================================================

RawCellMLViewPlugin::RawCellMLViewPlugin()
{
    // Set our settings

    mGuiSettings->addView(GuiViewSettings::Editing, 0);
}

//==============================================================================

void RawCellMLViewPlugin::initialize()
{
    // Create our generic raw CellML view widget

    mViewWidget = new RawCellmlViewWidget(mMainWindow);

    // Hide our generic raw CellML view widget since it may not initially be
    // shown in our central widget

    mViewWidget->setVisible(false);
}

//==============================================================================

void RawCellMLViewPlugin::loadSettings(QSettings *pSettings)
{
    // Retrieve our generic raw CellML view widget settings

    loadViewSettings(pSettings, mViewWidget);
}

//==============================================================================

void RawCellMLViewPlugin::saveSettings(QSettings *pSettings) const
{
    // Retrieve our generic raw CellML view widget settings

    saveViewSettings(pSettings, mViewWidget);
}

//==============================================================================

QWidget * RawCellMLViewPlugin::viewWidget(const QString &pFileName, const int &)
{
    // Check that we are dealing with a CellML file and, if so, return our
    // generic raw CellML view widget

    CellMLSupport::CellmlFile *cellmlFile = CellMLSupport::CellmlFileManager::instance()->cellmlFile(pFileName);

    if (!cellmlFile)
        // We are not dealing with a CellML file, so...

        return 0;

    // We are dealing with a CellML file, so update our generic raw CellML view
    // widget using the passed CellML file

    mViewWidget->initialize(pFileName);

    // Our generic raw CellML view widget is now ready, so...

    return mViewWidget;
}

//==============================================================================

QString RawCellMLViewPlugin::viewName(const int &pViewIndex)
{
    // We have only one view, so return its name otherwise call the GuiInterface
    // implementation of viewName

    switch (pViewIndex) {
    case 0:
        return tr("Raw CellML");
    default:
        return GuiInterface::viewName(pViewIndex);
    }
}

//==============================================================================

}   // namespace RawCellMLView
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================

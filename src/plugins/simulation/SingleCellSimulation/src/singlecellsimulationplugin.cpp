//==============================================================================
// SingleCellSimulation plugin
//==============================================================================

#include "cellmlfilemanager.h"
#include "singlecellsimulationplugin.h"
#include "singlecellsimulationview.h"

//==============================================================================

#include <QMainWindow>

//==============================================================================

namespace OpenCOR {
namespace SingleCellSimulation {

//==============================================================================

PLUGININFO_FUNC SingleCellSimulationPluginInfo()
{
    Descriptions descriptions;

    descriptions.insert("en", "A plugin to run single cell simulations");
    descriptions.insert("fr", "Une extension pour ex�cuter des simulations unicellulaires");

    return PluginInfo(PluginInfo::V001,
                      PluginInfo::Gui,
                      PluginInfo::Simulation,
                      true,
                      QStringList() << "CoreSimulation" << "CellMLSupport" << "Qwt",
                      descriptions);
}

//==============================================================================

Q_EXPORT_PLUGIN2(SingleCellSimulation, SingleCellSimulationPlugin)

//==============================================================================

SingleCellSimulationPlugin::SingleCellSimulationPlugin()
{
    // Set our settings

    mGuiSettings->addView(GuiViewSettings::Simulation, 0);
}

//==============================================================================

void SingleCellSimulationPlugin::initialize()
{
    // Create our single cell simulation view

    mSingleCellSimulationView = new SingleCellSimulationView(mMainWindow);

    // Hide our single cell simulation view since it may not initially be shown
    // in our central widget

    mSingleCellSimulationView->setVisible(false);
}

//==============================================================================

void SingleCellSimulationPlugin::retranslateUi()
{
    // Retranslate our single cell simulation view

    mSingleCellSimulationView->retranslateUi();
}

//==============================================================================

QWidget * SingleCellSimulationPlugin::viewWidget(const QString &pFileName,
                                                 const int &)
{
    // Check that we are dealing with a CellML file and, if so, return our
    // generic simulation view widget

    CellMLSupport::CellmlFile *cellmlFile = CellMLSupport::CellmlFileManager::instance()->cellmlFile(pFileName);

    if (!cellmlFile)
        // We are not dealing with a CellML file, so...

        return GuiInterface::newViewWidget(pFileName);

    // The expected file extension, so return our generic simulation view widget

    return mSingleCellSimulationView;
}

//==============================================================================

QString SingleCellSimulationPlugin::viewName(const int &pViewIndex)
{
    // We have only one view, so return its name otherwise call the GuiInterface
    // implementation of viewName

    switch (pViewIndex) {
    case 0:
        return tr("Single Cell");
    default:
        return GuiInterface::viewName(pViewIndex);
    }
}

//==============================================================================

}   // namespace SingleCellSimulation
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================

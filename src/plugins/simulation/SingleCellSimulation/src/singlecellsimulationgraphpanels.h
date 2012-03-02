//==============================================================================
// Single cell simulation graph panels widget
//==============================================================================

#ifndef SINGLECELLSIMULATIONGRAPHPANELS_H
#define SINGLECELLSIMULATIONGRAPHPANELS_H

//==============================================================================

#include <QSplitter>

//==============================================================================

namespace OpenCOR {
namespace SingleCellSimulation {

//==============================================================================

class SingleCellSimulationGraphPanel;

//==============================================================================

class SingleCellSimulationGraphPanels : public QSplitter
{
    Q_OBJECT

public:
    explicit SingleCellSimulationGraphPanels(QWidget *pParent = 0);

    SingleCellSimulationGraphPanel * addGraphPanel();
    void removeGraphPanel();

private:
    int mGraphPanelsCount;

private Q_SLOTS:
    void graphPanelActivated(SingleCellSimulationGraphPanel *pGraphPanel);
};

//==============================================================================

}   // namespace SingleCellSimulation
}   // namespace OpenCOR

//==============================================================================

#endif

//==============================================================================
// End of file
//==============================================================================

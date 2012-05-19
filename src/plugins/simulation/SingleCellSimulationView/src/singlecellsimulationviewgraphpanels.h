//==============================================================================
// Single cell simulation view graph panels widget
//==============================================================================

#ifndef SINGLECELLSIMULATIONVIEWGRAPHPANELS_H
#define SINGLECELLSIMULATIONVIEWGRAPHPANELS_H

//==============================================================================

#include "commonwidget.h"

//==============================================================================

#include <QSplitter>

//==============================================================================

namespace Ui {
    class SingleCellSimulationViewGraphPanels;
}

//==============================================================================

namespace OpenCOR {
namespace SingleCellSimulationView {

//==============================================================================

class SingleCellSimulationViewGraphPanel;

//==============================================================================

class SingleCellSimulationViewGraphPanels : public QSplitter,
                                            public Core::CommonWidget
{
    Q_OBJECT

public:
    explicit SingleCellSimulationViewGraphPanels(QWidget *pParent);
    ~SingleCellSimulationViewGraphPanels();

    virtual void loadSettings(QSettings *pSettings);
    virtual void saveSettings(QSettings *pSettings) const;

    SingleCellSimulationViewGraphPanel * addGraphPanel();
    void removeGraphPanel();

    SingleCellSimulationViewGraphPanel * activeGraphPanel();

protected:
    virtual void wheelEvent(QWheelEvent *pEvent);

private:
    Ui::SingleCellSimulationViewGraphPanels *mGui;

Q_SIGNALS:
    void grapPanelAdded(SingleCellSimulationViewGraphPanel *pGraphPanel);
    void grapPanelRemoved();

    void removeGraphPanelsEnabled(const bool &pEnabled);

private Q_SLOTS:
    void graphPanelActivated(SingleCellSimulationViewGraphPanel *pGraphPanel);
};

//==============================================================================

}   // namespace SingleCellSimulationView
}   // namespace OpenCOR

//==============================================================================

#endif

//==============================================================================
// End of file
//==============================================================================

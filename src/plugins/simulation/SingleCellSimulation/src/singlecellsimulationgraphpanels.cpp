//==============================================================================
// Single cell simulation graph panels widget
//==============================================================================

#include "singlecellsimulationgraphpanel.h"
#include "singlecellsimulationgraphpanels.h"

//==============================================================================

namespace OpenCOR {
namespace SingleCellSimulation {

//==============================================================================

SingleCellSimulationGraphPanels::SingleCellSimulationGraphPanels(QWidget *pParent) :
    QSplitter(Qt::Vertical, pParent),
    mGraphPanelsCount(0)
{
    // Add a default graph panel

    addGraphPanel();
}

//==============================================================================

SingleCellSimulationGraphPanel * SingleCellSimulationGraphPanels::addGraphPanel()
{
    // Check whether there is already a graph panel and if not, then delete our
    // dummy widget which ensures that we don't shrink

    if (!mGraphPanelsCount && count()) {
        // No previous graph panel, so...
        // Note: the test for count() above is for the very first time we want
        //       to add a graph panel and therefore where no dummy widget will
        //       be present...

        QWidget *dummyWidget = qobject_cast<QWidget*>(widget(0));

        dummyWidget->hide();

        delete dummyWidget;
    }

    // Create a new graph panel

    SingleCellSimulationGraphPanel *res = new SingleCellSimulationGraphPanel(this);

    // Add the graph panel to ourselves

    addWidget(res);

    // Keep track of the fact that we have one more graph panel

    ++mGraphPanelsCount;

    // Create a connection to keep track of whenever the graph panel gets
    // activated

    connect(res, SIGNAL(activated(SingleCellSimulationGraphPanel *)),
            this, SLOT(graphPanelActivated(SingleCellSimulationGraphPanel *)));

    // Activate it

    res->setActive(true);

    // Return our newly created graph panel

    return res;
}

//==============================================================================

void SingleCellSimulationGraphPanels::removeGraphPanel()
{
    if (!mGraphPanelsCount)
        // We don't have any graph panel to remove, so...

        return;

    // Remove the current graph panel

    for (int i = 0, iMax = count(); i < iMax; ++i) {
        SingleCellSimulationGraphPanel *graphPanel = qobject_cast<SingleCellSimulationGraphPanel *>(widget(i));

        if (graphPanel->isActive()) {
            // We are dealing with the currently active graph panel, so remove
            // it

            graphPanel->hide();

            delete graphPanel;

            // Keep track of the fact that we have one less graph panel

            --mGraphPanelsCount;

            // Activate the next graph panel or the last one available, if any

            iMax = count();

            if (!iMax)
                // No more graph panel, so...

                break;
            else if (i < iMax)
                // There is a next graph panel, so activate it

                qobject_cast<SingleCellSimulationGraphPanel *>(widget(i))->setActive(true);
            else
                // We were dealing with the last graph panel, but there is still
                // at least one left, so activate the new last graph panel

                qobject_cast<SingleCellSimulationGraphPanel *>(widget(count()-1))->setActive(true);

            // We are all done, so...

            break;
        }
    }

    // Check whether we have any graph panel left and if so add a dummy widget
    // which will ensure that we don't shrink

    if (!mGraphPanelsCount)
        addWidget(new QWidget(this));
}

//==============================================================================

void SingleCellSimulationGraphPanels::graphPanelActivated(SingleCellSimulationGraphPanel *pGraphPanel)
{
    // A graph panel has been activated, so inactivate all the others

    for (int i = 0, iMax = count(); i < iMax; ++i) {
        SingleCellSimulationGraphPanel *graphPanel = qobject_cast<SingleCellSimulationGraphPanel *>(widget(i));

        if (graphPanel != pGraphPanel)
            // We are not dealing with the graph panel that just got activated,
            // so inactivate it

            graphPanel->setActive(false);
    }
}

//==============================================================================

}   // namespace SingleCellSimulation
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================

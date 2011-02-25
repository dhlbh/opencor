#include "cellmlmodelrepositorywidget.h"

#include <QPaintEvent>

CellmlModelRepositoryWidget::CellmlModelRepositoryWidget(QWidget *pParent) :
    QWebView(pParent),
    CommonWidget(pParent)
{
    // Load the CellML Model Repository page

    setUrl(QUrl("http://models.cellml.org/"));
}

void CellmlModelRepositoryWidget::retranslateUi()
{
    // Nothing to do for now...
}

void CellmlModelRepositoryWidget::loadSettings(const QSettings &, const QString &)
{
    // Nothing to do for now...
}

void CellmlModelRepositoryWidget::saveSettings(QSettings &, const QString &)
{
    // Nothing to do for now...
}

QSize CellmlModelRepositoryWidget::sizeHint() const
{
    // Suggest a default size for the PMR explorer widget
    // Note: this is critical if we want a docked widget, with a PMR explorer
    //       widget on it, to have a decent size when docked to the main window

    return defaultSize(0.15);
}

void CellmlModelRepositoryWidget::paintEvent(QPaintEvent *pEvent)
{
    // Default handling of the event

    QWebView::paintEvent(pEvent);

    // Draw a border in case we are docked

    drawBorderIfDocked();

    // Accept the event

    pEvent->accept();
}

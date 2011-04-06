#include "tabwidget.h"

#include <QPainter>
#include <QPaintEvent>

TabWidget::TabWidget(QWidget *pParent) :
    QTabWidget(pParent),
    CommonWidget(pParent)
{
    setTabsClosable(true);
}

void TabWidget::paintEvent(QPaintEvent *pEvent)
{
    // Display the CellML logo in place of the tab widget itself, in case the
    // tab widget doesn't have any tab associated with it

    if (!count()) {
        // There are no tabs, so display our CellML logo

        QPixmap pixmap;

        pixmap.load(":cellmlLogo");

        QPainter paint(this);

        paint.drawPixmap(0.5*(width()-pixmap.width()),
                         0.5*(height()-pixmap.height()),
                         pixmap);

        // Accept the event

        pEvent->accept();
    } else {
        // There are tabs, so revert to the default paint handler

        QTabWidget::paintEvent(pEvent);
    }
}
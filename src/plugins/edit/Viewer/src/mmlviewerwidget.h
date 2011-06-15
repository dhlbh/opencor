#ifndef MMLVIEWERWIDGET_H
#define MMLVIEWERWIDGET_H

#include "commonwidget.h"

#include <QtMmlWidget>

namespace OpenCOR {

class MmlViewerWidget : public QtMmlWidget, public CommonWidget
{
    Q_OBJECT

public:
    explicit MmlViewerWidget(const QString &pName, QWidget *pParent);
    ~MmlViewerWidget();

    bool setContent(const QString &pContent, QString *pErrorMsg = 0,
                    int *pErrorLine = 0, int *pErrorColumn = 0);

protected:
    virtual QSize sizeHint() const;

    virtual void paintEvent(QPaintEvent *pEvent);
    virtual void resizeEvent(QResizeEvent *pEvent);

private:
    QtMmlWidget *mTestMmlWidget;
};

}

#endif
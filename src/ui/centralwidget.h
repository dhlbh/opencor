#ifndef CENTRALWIDGET_H
#define CENTRALWIDGET_H

#include <QWidget>

namespace Ui {
    class CentralWidget;
}

class CentralWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CentralWidget(QWidget *parent = 0);
    ~CentralWidget();

protected:
    virtual void paintEvent(QPaintEvent *pEvent);

private:
    Ui::CentralWidget *mUi;
};

#endif

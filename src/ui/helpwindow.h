#ifndef HELPWINDOW_H
#define HELPWINDOW_H

#include <QDockWidget>

namespace Ui {
    class HelpWindow;
}

class HelpWidget;

class QHelpEngine;
class QUrl;

class HelpWindow : public QDockWidget
{
    Q_OBJECT

public:
    explicit HelpWindow(QHelpEngine *pEngine, const QUrl& pHomepage,
                        QDockWidget *pParent = 0);
    ~HelpWindow();

    void resetAll();

    void gotoHomepage();

    void setZoomFactor(const double& pZoomFactor);
    double zoomFactor();

private:
    Ui::HelpWindow *mUi;

    HelpWidget *mHelpWidget;

private Q_SLOTS:
    void on_actionForward_triggered();
    void on_actionBack_triggered();
    void on_actionContents_triggered();
};

#endif // HELPWINDOW_H

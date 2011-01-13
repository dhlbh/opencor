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
    explicit HelpWindow(QHelpEngine *pEngine, const QUrl& pHomepage, QDockWidget *pParent = 0);
    ~HelpWindow();

    void resetAll();

    void gotoHomepage();

    void setHelpWidgetTextSizeMultiplier(const double& pHelpWidgetTextSizeMultiplier);
    double helpWidgetTextSizeMultiplier();

private:
    Ui::HelpWindow *mUi;

    HelpWidget *mHelpWidget;

private slots:
    void on_homepageButton_clicked();
    void on_backwardButton_clicked();
    void on_forwardButton_clicked();
};

#endif // HELPWINDOW_H

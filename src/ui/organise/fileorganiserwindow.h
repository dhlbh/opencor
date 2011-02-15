#ifndef FILEORGANISERWINDOW_H
#define FILEORGANISERWINDOW_H

#include "commonwidget.h"

#include <QDockWidget>

namespace Ui {
    class FileOrganiserWindow;
}

class FileOrganiserWidget;

class FileOrganiserWindow : public QDockWidget, public CommonWidget
{
    Q_OBJECT

public:
    explicit FileOrganiserWindow(QWidget *pParent = 0);
    ~FileOrganiserWindow();

    virtual void retranslateUi();

    virtual void defaultSettings();

private:
    Ui::FileOrganiserWindow *mUi;

    FileOrganiserWidget *mFileOrganiserWidget;
};

#endif

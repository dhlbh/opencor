//==============================================================================
// File organiser window
//==============================================================================

#ifndef FILEORGANISERWINDOW_H
#define FILEORGANISERWINDOW_H

//==============================================================================

#include "organisationwidget.h"

//==============================================================================

#include <QAbstractItemModel>

//==============================================================================

namespace Ui {
    class FileOrganiserWindow;
}

//==============================================================================

namespace OpenCOR {
namespace FileOrganiser {

//==============================================================================

class FileOrganiserWidget;

//==============================================================================

class FileOrganiserWindow : public Core::OrganisationWidget
{
    Q_OBJECT

public:
    explicit FileOrganiserWindow(QWidget *pParent = 0);
    ~FileOrganiserWindow();

    virtual void retranslateUi();

    virtual void loadSettings(QSettings *pSettings);
    virtual void saveSettings(QSettings *pSettings) const;

protected:
    virtual void updateActions();

private:
    Ui::FileOrganiserWindow *mUi;

    FileOrganiserWidget *mFileOrganiserWidget;

private Q_SLOTS:
    void on_actionNew_triggered();
    void on_actionDelete_triggered();

    void customContextMenu(const QPoint &) const;
    void itemDoubleClicked(const QModelIndex &itemIndex);

    void needUpdateActions();
};

//==============================================================================

}   // namespace FileOrganiser
}   // namespace OpenCOR

//==============================================================================

#endif

//==============================================================================
// End of file
//==============================================================================

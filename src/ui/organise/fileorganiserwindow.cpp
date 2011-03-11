#include "fileorganiserwindow.h"
#include "fileorganiserwidget.h"

#include "ui_fileorganiserwindow.h"

static const QString SettingsFileOrganiserWindow = "FileOrganiserWindow";

FileOrganiserWindow::FileOrganiserWindow(QWidget *pParent) :
    DockWidget(pParent),
    mUi(new Ui::FileOrganiserWindow)
{
    // Set up the UI

    mUi->setupUi(this);

    // Create and add the file organiser widget

    mFileOrganiserWidget = new FileOrganiserWidget(this);

    setWidget(mFileOrganiserWidget);

    // Prevent objects from being dropped on the window

    setAcceptDrops(false);
}

FileOrganiserWindow::~FileOrganiserWindow()
{
    // Delete the UI

    delete mUi;
}

void FileOrganiserWindow::retranslateUi()
{
    // Translate the whole window

    mUi->retranslateUi(this);

    // Retranslate the file organiser widget

    mFileOrganiserWidget->retranslateUi();
}

void FileOrganiserWindow::loadSettings(const QSettings &pSettings,
                                       const QString &)
{
    // Retrieve the settings of the file organiser widget

    mFileOrganiserWidget->loadSettings(pSettings, SettingsFileOrganiserWindow);
}

void FileOrganiserWindow::saveSettings(QSettings &pSettings, const QString &)
{
    // Keep track of the settings of the file organiser widget

    mFileOrganiserWidget->saveSettings(pSettings, SettingsFileOrganiserWindow);
}

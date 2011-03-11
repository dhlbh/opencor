#include "viewerwindow.h"
#include "mmlviewerwidget.h"

#include "ui_viewerwindow.h"

static const QString SettingsViewerWindow = "ViewerWindow";

ViewerWindow::ViewerWindow(QWidget *pParent) :
    DockWidget(pParent),
    mUi(new Ui::ViewerWindow)
{
    // Set up the UI

    mUi->setupUi(this);

    // Create and add the viewer widget

    mMmlViewerWidget = new MmlViewerWidget(this);

    setWidget(mMmlViewerWidget);

    // Prevent objects from being dropped on the viewer widget

    mMmlViewerWidget->setAcceptDrops(false);
}

ViewerWindow::~ViewerWindow()
{
    // Delete the UI

    delete mUi;
}

void ViewerWindow::retranslateUi()
{
    // Translate the whole window

    mUi->retranslateUi(this);

    // Retranslate the viewer widget

    mMmlViewerWidget->retranslateUi();
}

void ViewerWindow::loadSettings(const QSettings &pSettings, const QString &)
{
    // Retrieve the settings of the viewer widget

    mMmlViewerWidget->loadSettings(pSettings, SettingsViewerWindow);
}

void ViewerWindow::saveSettings(QSettings &pSettings, const QString &)
{
    // Keep track of the settings of the viewer widget

    mMmlViewerWidget->saveSettings(pSettings, SettingsViewerWindow);
}

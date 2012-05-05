//==============================================================================
// Check for updates window
//==============================================================================

#include "checkforupdateswindow.h"

//==============================================================================

#include "ui_checkforupdateswindow.h"

//==============================================================================

namespace OpenCOR {

//==============================================================================

CheckForUpdatesWindow::CheckForUpdatesWindow(QWidget *pParent) :
    QDialog(pParent),
    mGui(new Ui::CheckForUpdatesWindow)
{
    // Set up the GUI

    mGui->setupUi(this);
}

//==============================================================================

CheckForUpdatesWindow::~CheckForUpdatesWindow()
{
    // Delete the GUI

    delete mGui;
}

//==============================================================================

}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================

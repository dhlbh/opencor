//==============================================================================
// Core plugin
//==============================================================================

#include "centralwidget.h"
#include "coreplugin.h"
#include "organisationwidget.h"
#include "plugin.h"

//==============================================================================

#include <QAction>
#include <QFileDialog>
#include <QMainWindow>
#include <QSettings>

//==============================================================================

namespace OpenCOR {
namespace Core {

//==============================================================================

PLUGININFO_FUNC CorePluginInfo()
{
    Descriptions descriptions;

    descriptions.insert("en", "The core plugin for OpenCOR");
    descriptions.insert("fr", "L'extension de base pour OpenCOR");

    return PluginInfo(PluginInfo::V001,
                      PluginInfo::Gui,
                      PluginInfo::Miscellaneous,
                      false,
                      QStringList(),
                      descriptions);
}

//==============================================================================

Q_EXPORT_PLUGIN2(Core, CorePlugin)

//==============================================================================

void CorePlugin::initialize()
{
    // Create our central widget

    mCentralWidget = new CentralWidget(mMainWindow);

    // Create our different File actions
//---GRY--- ALL THE SAVE-RELATED ACTIONS SHOULD BE INVISIBLE UNLESS THE EDITING
//          MODE IS ACTIVE

    mFileOpenAction = newAction(mMainWindow, false,
                                ":/oxygen/actions/document-open.png",
                                QKeySequence::Open);

    mFileSaveAction    = newAction(mMainWindow, false,
                                   ":/oxygen/actions/document-save.png",
                                   QKeySequence::Save,
                                   mCentralWidget->isModeEnabled(GuiViewSettings::Editing));
    mFileSaveAsAction  = newAction(mMainWindow, false,
                                   ":/oxygen/actions/document-save-as.png",
                                   QKeySequence::SaveAs,
                                   mCentralWidget->isModeEnabled(GuiViewSettings::Editing));
    mFileSaveAllAction = newAction(mMainWindow, false,
                                   ":/oxygen/actions/document-save-all.png",
                                   QKeySequence::UnknownKey,
                                   mCentralWidget->isModeEnabled(GuiViewSettings::Editing));

    mFilePreviousAction = newAction(mMainWindow, false,
                                    ":/oxygen/actions/go-previous.png",
#ifndef Q_WS_MAC
    //---GRY--- QKeySequence::PreviousChild SHOULD, ON WINDOWS AND LINUX,
    //          CORRESPOND TO Ctrl+Shift+Tab, BUT SOMEHOW IT CORRESPONDS TO
    //          Ctrl+Shift+Backtab AND THOUGH IT WORKS ON LINUX (BUT NOT ON
    //          WINDOWS), IT DOESN'T LOOK GOOD, SO...

                                    QList<QKeySequence>() << QKeySequence(Qt::CTRL|Qt::SHIFT|Qt::Key_Tab));
#else
                                    QKeySequence::PreviousChild);
#endif
    mFileNextAction     = newAction(mMainWindow, false,
                                    ":/oxygen/actions/go-next.png",
                                    QKeySequence::NextChild);

    mFileCloseAction    = newAction(mMainWindow, false,
                                    ":/oxygen/actions/document-close.png",
                                    QKeySequence::Close);
    mFileCloseAllAction = newAction(mMainWindow);

    mFilePrintAction = newAction(mMainWindow, false,
                                 ":/oxygen/actions/document-print.png",
                                 QKeySequence::Print);

    // Some connections to handle our various actions

    connect(mFileOpenAction, SIGNAL(triggered(bool)),
            this, SLOT(openFile()));

    connect(mFilePreviousAction, SIGNAL(triggered(bool)),
            mCentralWidget, SLOT(previousFile()));
    connect(mFileNextAction, SIGNAL(triggered(bool)),
            mCentralWidget, SLOT(nextFile()));

    connect(mFileCloseAction, SIGNAL(triggered(bool)),
            mCentralWidget, SLOT(closeFile()));
    connect(mFileCloseAllAction, SIGNAL(triggered(bool)),
            mCentralWidget, SLOT(closeAllFiles()));

    // Some connections to handle the result of our various actions

    connect(mCentralWidget, SIGNAL(fileOpened(const QString &)),
            this, SLOT(needUpdateActions()));

    connect(mCentralWidget, SIGNAL(fileClosed(const QString &)),
            this, SLOT(needUpdateActions()));

    // Set our settings

    mGuiSettings->addMenuAction(GuiMenuActionSettings::File, mFileOpenAction);
    mGuiSettings->addMenuAction(GuiMenuActionSettings::File);
    mGuiSettings->addMenuAction(GuiMenuActionSettings::File, mFileSaveAction);
    mGuiSettings->addMenuAction(GuiMenuActionSettings::File, mFileSaveAsAction);
    mGuiSettings->addMenuAction(GuiMenuActionSettings::File, mFileSaveAllAction);
    mGuiSettings->addMenuAction(GuiMenuActionSettings::File);
    mGuiSettings->addMenuAction(GuiMenuActionSettings::File, mFilePreviousAction);
    mGuiSettings->addMenuAction(GuiMenuActionSettings::File, mFileNextAction);
    mGuiSettings->addMenuAction(GuiMenuActionSettings::File);
    mGuiSettings->addMenuAction(GuiMenuActionSettings::File, mFileCloseAction);
    mGuiSettings->addMenuAction(GuiMenuActionSettings::File, mFileCloseAllAction);
    mGuiSettings->addMenuAction(GuiMenuActionSettings::File);
    mGuiSettings->addMenuAction(GuiMenuActionSettings::File, mFilePrintAction);
    mGuiSettings->addMenuAction(GuiMenuActionSettings::File);

    mGuiSettings->addCentralWidget(mCentralWidget);

    // Initialise the enabled state of our various actions

    updateActions();
}

//==============================================================================

void CorePlugin::initializationsDone(const Plugins &pLoadedPlugins)
{
    // Retrieve the file types supported by the plugins

    foreach (Plugin *loadedPlugin, pLoadedPlugins) {
        FileInterface *fileInterface = qobject_cast<FileInterface *>(loadedPlugin->instance());

        if (fileInterface)
            // The plugin implements our file interface, so...

            mSupportedFileTypes << fileInterface->fileTypes();
    }

    // Check, based on the loaded plugins, which views, if any, our central
    // widget should support

    foreach (Plugin *loadedPlugin, pLoadedPlugins) {
        GuiInterface *guiInterface = qobject_cast<GuiInterface *>(loadedPlugin->instance());

        if (guiInterface)
            // The plugin implements our GUI interface, so go through each view
            // supported by the plugin and enable whatever mode is required

            foreach (GuiViewSettings *viewSettings,
                     guiInterface->guiSettings()->views())
                mCentralWidget->addView(loadedPlugin, viewSettings);
    }

    // Some connections to handle certain plugin's windows

    foreach (Plugin *loadedPlugin, pLoadedPlugins) {
        GuiInterface *guiInterface = qobject_cast<GuiInterface *>(loadedPlugin->instance());

        if (guiInterface) {
            foreach (GuiWindowSettings *windowSettings,
                     guiInterface->guiSettings()->windows())
                switch (windowSettings->type()) {
                case GuiWindowSettings::Organisation:
                    // The plugin's window is of organisation type, so we want
                    // its filesOpened signal to trigger out central widget's
                    // openFiles slot

                    connect((OrganisationWidget *) windowSettings->window(), SIGNAL(filesOpened(const QStringList &)),
                            mCentralWidget, SLOT(openFiles(const QStringList &)));

                    break;
                default:
                    // Uninteresting type, so...

                    ;
                }
        }
    }
}

//==============================================================================

static const QString SettingsFileDialogDirectory = "FileDialogDirectory";

//==============================================================================

void CorePlugin::loadSettings(QSettings *pSettings)
{
    // Retrieve the central widget settings

    pSettings->beginGroup(mCentralWidget->objectName());
        mCentralWidget->loadSettings(pSettings);
    pSettings->endGroup();

    // Retrieve the active directory

    mActiveDir.setPath(pSettings->value(SettingsFileDialogDirectory,
                                        QDir::currentPath()).toString());
}

//==============================================================================

void CorePlugin::saveSettings(QSettings *pSettings) const
{
    // Keep track of the central widget settings

    pSettings->beginGroup(mCentralWidget->objectName());
        mCentralWidget->saveSettings(pSettings);
    pSettings->endGroup();

    // Keep track of the active directory

    pSettings->setValue(SettingsFileDialogDirectory, mActiveDir.path());
}

//==============================================================================

void CorePlugin::setFocus()
{
    // Set the focus to the central widget

    mCentralWidget->setFocus();
}

//==============================================================================

void CorePlugin::retranslateUi()
{
    // Retranslate our different File actions

    retranslateAction(mFileOpenAction, tr("&Open..."), tr("Open a file"));

    retranslateAction(mFileSaveAction, tr("&Save"),
                      tr("Save the current file"));
    retranslateAction(mFileSaveAsAction, tr("Save &As..."),
                      tr("Save the current file under a different name"));
    retranslateAction(mFileSaveAllAction, tr("Save All"),
                      tr("Save all the files"));

    retranslateAction(mFilePreviousAction, tr("Previous"),
                      tr("Select the previous file"));
    retranslateAction(mFileNextAction, tr("Next"),
                      tr("Select the next file"));

    retranslateAction(mFileCloseAction, tr("Clos&e"),
                      tr("Close the current file"));
    retranslateAction(mFileCloseAllAction, tr("Close All"),
                      tr("Close all the files"));

    retranslateAction(mFilePrintAction, tr("&Print..."),
                      tr("Print the current file"));

    // Retranslate our central widget

    mCentralWidget->retranslateUi();
}

//==============================================================================

void CorePlugin::updateActions()
{
    // Make sure that our various actions are properly enabled/disabled

    mFileOpenAction->setEnabled(true);

    mFileSaveAction->setEnabled(false);
    mFileSaveAsAction->setEnabled(false);
    mFileSaveAllAction->setEnabled(false);

    mFilePreviousAction->setEnabled(mCentralWidget->openedFilesCount() > 1);
    mFileNextAction->setEnabled(mCentralWidget->openedFilesCount() > 1);

    mFileCloseAction->setEnabled(mCentralWidget->openedFilesCount());
    mFileCloseAllAction->setEnabled(mCentralWidget->openedFilesCount());

    mFilePrintAction->setEnabled(false);
}

//==============================================================================

void CorePlugin::openFile()
{
    // Ask for the file(s) to be opened

    QString supportedFileTypes;

    foreach (const FileType &supportedFileType, mSupportedFileTypes)
        supportedFileTypes +=  ";;"
                              +supportedFileType.description()
                              +" (*."+supportedFileType.fileExtension()+")";

    QStringList files = QFileDialog::getOpenFileNames(mMainWindow,
                                                      tr("Open File"),
                                                      mActiveDir.path(),
                                                      tr("All Files")
                                                      +" (*"
#ifdef Q_WS_WIN
                                                      +".*"
#endif
                                                      +")"+supportedFileTypes);

    if (files.count())
        // There is at least one file which is to be opened, so we can keep
        // track of the folder in which it is
        // Note #1: we use the last file to determine the folder that is to be
        //          remembered since on Windows 7, at least, it's possible to
        //          search for files from within the file dialog box, the last
        //          file should be the one we are 'interested' in...
        // Note #2: this doesn't, unfortunately, address the case where the user
        //          goes to a directory and then closes the file dialog box
        //          without selecting any file. There might be a way to get it
        //          to work, but that would involve using the exec method rather
        //          than the static getOpenFilesNames method, which would result
        //          in a non-native looking file dialog box (on Windows 7 at
        //          least), so it's not an option unfortunately...

        mActiveDir = QFileInfo(files.at(files.count()-1)).path();

    // Open the file(s)

    mCentralWidget->openFiles(files);
}

//==============================================================================

void CorePlugin::needUpdateActions()
{
    // Something requires the actions to be udpated

    updateActions();
}

//==============================================================================

}   // namespace Core
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================

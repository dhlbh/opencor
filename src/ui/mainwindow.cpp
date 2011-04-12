#include "centralwidget.h"
#include "viewerwindow.h"
#include "common.h"
#include "dockwidget.h"
#include "mainwindow.h"
#include "filebrowserwindow.h"
#include "fileorganiserwindow.h"
#include "helpwindow.h"
#include "cellmlmodelrepositorywindow.h"
#include "utils.h"

#include "ui_mainwindow.h"

#ifdef Q_WS_WIN
    #include <windows.h>
#endif

#include <QCoreApplication>
#include <QDesktopServices>
#include <QDesktopWidget>
#include <QHelpEngine>
#include <QMessageBox>
#include <QProcess>
#include <QSettings>
#include <QShowEvent>

#include <QxtTemporaryDir>

static const QString OpencorHelpHomepageUrl = "qthelp://opencor/doc/userIndex.html";

static const QString SystemLocale  = "";
static const QString EnglishLocale = "en";
static const QString FrenchLocale  = "fr";

MainWindow::MainWindow(QWidget *pParent) :
    QMainWindow(pParent),
    mUi(new Ui::MainWindow),
    mNeedResetAll(false),
    mLocale(SystemLocale)
{
    // Set the name of the main window to that of the application

    setWindowTitle(qApp->applicationName());

    // Set up the UI

    mUi->setupUi(this);
    // Note: the application icon (which needs to be set for Linux, but neither
    //       for Windows nor Mac OS X, since it's set through CMake in those
    //       cases (see CMakeLists.txt)) is set within the UI file. This being
    //       said, it's good to have it set for all three platforms, since it
    //       can then be used in, for example, the about box, so...

//---GRY--- THE BELOW CODE WOULD BE GOOD TO HAVE, BUT THERE IS A KNOWN BUG (SEE
//          http://bugreports.qt.nokia.com/browse/QTBUG-3116) WHICH MAKES THAT
//          CODE TO CAUSE PROBLEMS WITH THE LOADING/RESTORING OF THE APPLICATION
//          GEOMETRY, SO...

//#ifdef Q_WS_MAC
//    // Make the application look more like a Mac OS X application

//    setUnifiedTitleAndToolBarOnMac(true);
//#endif

    // Set the New toolbar button with its dropdown menu using our custom-made
    // action (actionNew)
    // Note: ideally, this would be done using the GUI designer in QtCreator,
    //       but it's not quite clear to me how this can be achieved, so...

    QMenu *actionNewMenu = new QMenu(this);

    mUi->actionNew->setMenu(actionNewMenu);

    actionNewMenu->addAction(mUi->actionCellML10File);
    actionNewMenu->addAction(mUi->actionCellML11File);

    mUi->fileToolbar->insertAction(mUi->actionOpen, mUi->actionNew);
    mUi->fileToolbar->insertSeparator(mUi->actionOpen);

    // Set the central widget

    mCentralWidget = new CentralWidget(this);

    setCentralWidget(mCentralWidget);

    // Create a temporary directory where to put OpenCOR's resources

    mTempDir = new QxtTemporaryDir();

    // Extract the help files

    QString applicationBaseName = QFileInfo(qApp->applicationFilePath()).baseName();

    mQchFileName = mTempDir->path()+QDir::separator()+applicationBaseName+".qch";
    mQhcFileName = mTempDir->path()+QDir::separator()+applicationBaseName+".qhc";

    saveResourceAs(":qchFile", mQchFileName);
    saveResourceAs(":qhcFile", mQhcFileName);

    // Set up the help engine

    mHelpEngine = new QHelpEngine(mQhcFileName);

    mHelpEngine->setupData();

    // Create our document manager

    mDocumentManager = new DocumentManager();

    // Specify some general docking settings

    setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
    setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

    // Create our various dock windows

    mCellmlModelRepositoryWindow = new CellmlModelRepositoryWindow(this);
    mFileBrowserWindow           = new FileBrowserWindow(this);
    mFileOrganiserWindow         = new FileOrganiserWindow(this);

    mViewerWindow = new ViewerWindow(this);

    mHelpWindow = new HelpWindow(mHelpEngine, QUrl(OpencorHelpHomepageUrl), this);

    // Some connections to handle the various toolbars

    connect(mUi->actionFileToolbar, SIGNAL(triggered(bool)),
            mUi->fileToolbar, SLOT(setVisible(bool)));
    connect(mUi->fileToolbar->toggleViewAction(), SIGNAL(toggled(bool)),
            mUi->actionFileToolbar, SLOT(setChecked(bool)));

    connect(mUi->actionEditToolbar, SIGNAL(triggered(bool)),
            mUi->editToolbar, SLOT(setVisible(bool)));
    connect(mUi->editToolbar->toggleViewAction(), SIGNAL(toggled(bool)),
            mUi->actionEditToolbar, SLOT(setChecked(bool)));

    connect(mUi->actionRunToolbar, SIGNAL(triggered(bool)),
            mUi->runToolbar, SLOT(setVisible(bool)));
    connect(mUi->runToolbar->toggleViewAction(), SIGNAL(toggled(bool)),
            mUi->actionRunToolbar, SLOT(setChecked(bool)));

    connect(mUi->actionHelpToolbar, SIGNAL(triggered(bool)),
            mUi->helpToolbar, SLOT(setVisible(bool)));
    connect(mUi->helpToolbar->toggleViewAction(), SIGNAL(toggled(bool)),
            mUi->actionHelpToolbar, SLOT(setChecked(bool)));

    // A connection to handle the status bar

    connect(mUi->actionStatusBar, SIGNAL(triggered(bool)),
            statusBar(), SLOT(setVisible(bool)));

    // Some connections to handle various menu items

    connect(mUi->actionExit, SIGNAL(triggered(bool)),
            this, SLOT(close()));
    connect(mUi->actionResetAll, SIGNAL(triggered(bool)),
            this, SLOT(resetAll()));

    // Some connections to handle the visibility of our various dock windows

    connect(mUi->actionCellmlModelRepository, SIGNAL(triggered(bool)),
            mCellmlModelRepositoryWindow, SLOT(setVisible(bool)));
    connect(mCellmlModelRepositoryWindow, SIGNAL(visibilityChanged(bool)),
            mUi->actionCellmlModelRepository, SLOT(setChecked(bool)));

    connect(mUi->actionFileBrowser, SIGNAL(triggered(bool)),
            mFileBrowserWindow, SLOT(setVisible(bool)));
    connect(mFileBrowserWindow, SIGNAL(visibilityChanged(bool)),
            mUi->actionFileBrowser, SLOT(setChecked(bool)));

    connect(mUi->actionFileOrganiser, SIGNAL(triggered(bool)),
            mFileOrganiserWindow, SLOT(setVisible(bool)));
    connect(mFileOrganiserWindow, SIGNAL(visibilityChanged(bool)),
            mUi->actionFileOrganiser, SLOT(setChecked(bool)));

    connect(mUi->actionViewer, SIGNAL(triggered(bool)),
            mViewerWindow, SLOT(setVisible(bool)));
    connect(mViewerWindow, SIGNAL(visibilityChanged(bool)),
            mUi->actionViewer, SLOT(setChecked(bool)));

    connect(mUi->actionHelp, SIGNAL(triggered(bool)),
            mHelpWindow, SLOT(setVisible(bool)));
    connect(mHelpWindow, SIGNAL(visibilityChanged(bool)),
            mUi->actionHelp, SLOT(setChecked(bool)));

    // Some connections to handle the File menu

    connect(mUi->actionClose, SIGNAL(triggered(bool)),
            this, SLOT(closeFile()));
    connect(mUi->actionCloseAll, SIGNAL(triggered(bool)),
            this, SLOT(closeFiles()));

    // A connection to handle the file browser window

    connect(mFileBrowserWindow, SIGNAL(filesOpened(const QStringList &)),
            this, SLOT(openFiles(const QStringList &)));

    // A connection to handle the file organiser window

    connect(mFileOrganiserWindow, SIGNAL(filesOpened(const QStringList &)),
            this, SLOT(openFiles(const QStringList &)));

    // Some connections to handle the central widget

    connect(mCentralWidget, SIGNAL(filesDropped(const QStringList &)),
            this, SLOT(openFiles(const QStringList &)));
    connect(mCentralWidget, SIGNAL(fileClosed(const QString &)),
            this, SLOT(unmanageFile(const QString &)));

    // Retrieve the user settings from the previous session, if any

    loadSettings();

    // Bring ourselves to the foreground. Indeed, when restarting OpenCOR (as a
    // result of a reset all), OpenCOR will on Mac OS X be behind other
    // applications. This behaviour has, on occasions, also been observed on
    // Windows, so... rather than making the following code Mac OS X specific,
    // we may as well make it general, since it doesn't have any adverse effect
    // on either Windows or Linux

    activateWindow();

    raise();   // Just to be on the safe side
}

MainWindow::~MainWindow()
{
    // Delete some internal objects

    delete mDocumentManager;
    delete mHelpEngine;
    delete mUi;

    // Delete the help files and then temporary directory by deleting the
    // temporary directory object

    QFile(mQchFileName).remove();
    QFile(mQhcFileName).remove();

    delete mTempDir;
}

bool MainWindow::needResetAll()
{
    // Simply return whether or not we need to reset everything

    return mNeedResetAll;
}

void MainWindow::changeEvent(QEvent *pEvent)
{
    // Default handling of the event

    QMainWindow::changeEvent(pEvent);

    // If the system's locale has changed, then update OpenCOR's locale in case
    // the user wants to use the system's locale

    if (   (pEvent->type() == QEvent::LocaleChange)
        && (mUi->actionSystem->isChecked()))
        setLocale(SystemLocale);
}

void MainWindow::showEvent(QShowEvent *pEvent)
{
    // Default handling of the event

    QMainWindow::showEvent(pEvent);

    // The first time we show OpenCOR, we want to make sure that its menu is
    // fine (i.e. it respects OpenCOR's settings that were loaded in the
    // constructor). Various connections (set in the constructor) take care of
    // this, but there still remains one menu item (which specifies whether the
    // status bar is to be shown or not) for which no connection can be set. So,
    // we have to 'manually' set the status of that menu item here (as opposed
    // to, say, the constructor), since we may need (on Windows at least) all of
    // OpenCOR to be visible in order to be able to determine whether the status
    // bar is visible or not

    static bool firstTime = true;

    if (firstTime) {
        firstTime = false;

        mUi->actionStatusBar->setChecked(statusBar()->isVisible());
    }
}

void MainWindow::closeEvent(QCloseEvent *pEvent)
{
    // Keep track of our default settings
    // Note: it must be done here, as opposed to the destructor, otherwise some
    //       settings (e.g. dock windows) won't be properly saved

    saveSettings();

    // Default handling of the event

    QMainWindow::closeEvent(pEvent);
}

void MainWindow::loadDockWindowSettings(DockWidget *pDockWindow,
                                        const bool &pNeedDefaultSettings,
                                        QSettings &pSettings,
                                        const Qt::DockWidgetArea &pDockArea,
                                        DockWidget *pDockWidget)
{
    CommonWidget *commonWidget = dynamic_cast<CommonWidget *>(pDockWindow);

    if (commonWidget) {
        // The dynamic casting was successful, so we are really dealing with
        // the right kind of dock window and can therefore go ahead

        if (pNeedDefaultSettings) {
            // Hide the dock window, so that we can set things up without having
            // the screen flashing

            pDockWindow->setVisible(false);

            // Position the dock window to its default location

            if (pDockWidget)
                tabifyDockWidget(pDockWidget, pDockWindow);
            else
                addDockWidget(pDockArea, pDockWindow);
        }

        // Load the dock window's settings

        commonWidget->loadSettings(pSettings);

        if (pNeedDefaultSettings)
            // Make the dock window visible

            pDockWindow->setVisible(true);
    }
}

static const QString SettingsLocale              = "Locale";
static const QString SettingsGeometry            = "Geometry";
static const QString SettingsState               = "State";
static const QString SettingsStatusBarVisibility = "StatusBarVisibility";
static const QString SettingsDebugModeEnabled    = "DebugModeEnabled";

void MainWindow::loadSettings()
{
    QSettings settings(qApp->applicationName());

    settings.beginGroup(objectName());
        // Retrieve the language to be used by OpenCOR

        setLocale(settings.value(SettingsLocale, SystemLocale).toString());

        // Retrieve the geometry and state of the main window

        bool needDefaultSettings =    !restoreGeometry(settings.value(SettingsGeometry).toByteArray())
                                   || !restoreState(settings.value(SettingsState).toByteArray());

        if (needDefaultSettings) {
            // The geometry and/or state of the main window couldn't be
            // retrieved, so go with some default settins

            // Default size and position of the main window

            double ratio = 1.0/13.0;
            QRect desktopGeometry = qApp->desktop()->availableGeometry();
            int horizSpace = ratio*desktopGeometry.width();
            int vertSpace  = ratio*desktopGeometry.height();

            setGeometry(desktopGeometry.left()+horizSpace,
                        desktopGeometry.top()+vertSpace,
                        desktopGeometry.width()-2*horizSpace,
                        desktopGeometry.height()-2*vertSpace);
        } else {
            // The geometry and state of the main window could be retrieved, so
            // carry on with the loading of the settings

            // Retrieve whether the status bar is to be shown

            mUi->statusBar->setVisible(settings.value(SettingsStatusBarVisibility,
                                                      true).toBool());
        }

        // Retrieve whether we are working in debug mode or not

        mUi->actionDebugMode->setChecked(settings.value(SettingsDebugModeEnabled,
                                                        false).toBool());

        // Retrieve the settings of the various dock windows

        loadDockWindowSettings(mCellmlModelRepositoryWindow, needDefaultSettings, settings, Qt::LeftDockWidgetArea);
        loadDockWindowSettings(mFileBrowserWindow, needDefaultSettings, settings, Qt::LeftDockWidgetArea);
        loadDockWindowSettings(mFileOrganiserWindow, needDefaultSettings, settings, Qt::LeftDockWidgetArea);

        loadDockWindowSettings(mViewerWindow, needDefaultSettings, settings, Qt::TopDockWidgetArea);

        loadDockWindowSettings(mHelpWindow, needDefaultSettings, settings, Qt::RightDockWidgetArea);
    settings.endGroup();
}

void MainWindow::saveSettings()
{
    QSettings settings(qApp->applicationName());

    settings.beginGroup(objectName());
        // Keep track of the language to be used by OpenCOR

        settings.setValue(SettingsLocale, mLocale);

        // Keep track of the geometry of the main window

        settings.setValue(SettingsGeometry, saveGeometry());

        // Keep track of the state of the main window

        settings.setValue(SettingsState, saveState());

        // Keep track of whether the status bar is to be shown

        settings.setValue(SettingsStatusBarVisibility,
                          mUi->statusBar->isVisible());

        // Keep track of whether we are working in debug mode or not

        settings.setValue(SettingsDebugModeEnabled,
                          mUi->actionDebugMode->isChecked());

        // Keep track of the settings of the various dock windows

        mCellmlModelRepositoryWindow->saveSettings(settings);
        mFileBrowserWindow->saveSettings(settings);
        mFileOrganiserWindow->saveSettings(settings);

        mViewerWindow->saveSettings(settings);

        mHelpWindow->saveSettings(settings);
    settings.endGroup();
}

void MainWindow::setLocale(const QString &pLocale)
{
    if ((pLocale != mLocale) || (pLocale == SystemLocale)) {
        // The new locale is different from the existing one, so we need to
        // translate everything

        QString realLocale = (pLocale == SystemLocale)?
                                 QLocale::system().name().left(2):
                                 pLocale;

        mLocale = pLocale;

        // Specify the language to be used by OpenCOR

        qApp->removeTranslator(&mQtTranslator);
        mQtTranslator.load(":qt_"+realLocale);
        qApp->installTranslator(&mQtTranslator);

        qApp->removeTranslator(&mAppTranslator);
        mAppTranslator.load(":app_"+realLocale);
        qApp->installTranslator(&mAppTranslator);

        // Translate the whole GUI (including any 'child' window) and update the
        // actions just to be on the safe side

        mUi->retranslateUi(this);

        updateActions();

        mCellmlModelRepositoryWindow->retranslateUi();
        mFileBrowserWindow->retranslateUi();
        mFileOrganiserWindow->retranslateUi();

        mViewerWindow->retranslateUi();

        mHelpWindow->retranslateUi();
    }

    // Update the checked menu item
    // Note: it has to be done every single time, since selecting a menu item
    //       will automatically toggle its checked status, so...

    mUi->actionSystem->setChecked(mLocale == SystemLocale);

    mUi->actionEnglish->setChecked(mLocale == EnglishLocale);
    mUi->actionFrench->setChecked(mLocale == FrenchLocale);
}

void MainWindow::singleAppMsgRcvd(const QString &)
{
    // We have just received a message from another instance of OpenCOR, so
    // bring ourselves to the foreground
    // Note: one would normally use activateWindow(), but depending on the
    //       operating system it may or not bring OpenCOR to the foreground,
    //       so... instead we do what follows, depending on the operating
    //       system...

#ifdef Q_WS_WIN
    // Retrieve OpenCOR's window Id

    WId mainWinId = winId();

    // Bring OpenCOR to the foreground

    DWORD foregroundThreadProcId = GetWindowThreadProcessId(GetForegroundWindow(),
                                                            NULL);
    DWORD mainThreadProcId = GetWindowThreadProcessId(mainWinId, NULL);

    if (foregroundThreadProcId != mainThreadProcId) {
        // OpenCOR's thread process Id is not that of the foreground window, so
        // attach the foreground thread to OpenCOR's, set OpenCOR to the
        // foreground, and detach the foreground thread from OpenCOR's

        AttachThreadInput(foregroundThreadProcId, mainThreadProcId, true);

        SetForegroundWindow(mainWinId);
        SetFocus(mainWinId);

        AttachThreadInput(foregroundThreadProcId, mainThreadProcId, false);
    } else {
        // OpenCOR's thread process Id is that of the foreground window, so
        // just set OpenCOR to the foreground

        SetForegroundWindow(mainWinId);
    }

    // Show/restore OpenCOR, depending on its current state

    if (IsIconic(mainWinId))
        ShowWindow(mainWinId, SW_RESTORE);
    else
        ShowWindow(mainWinId, SW_SHOW);

    // Note: under Windows, to use activateWindow() will only highlight the
    //       application in the taskbar, since under Windows no application
    //       should be allowed to bring itself to the foreground when another
    //       application is already in the foreground. Fair enough, but it
    //       happens that, here, the user wants OpenCOR to be brought to the
    //       foreground, hence the above code to get the effect we are after...
#else
    // Do what one should normally do

    activateWindow();

    raise();   // Just to be on the safe side
    // Note: raise() never seems to be required on Mac OS X, but to use
    //       activateWindow() under Linux may or not give the expected result,
    //       so...
#endif

    // Now, we must handle the arguments that were passed to OpenCOR

    // TODO: handle the arguments passed to the 'official' instance of OpenCOR
}

void MainWindow::updateActions()
{
    // Make sure that our various actions are properly enabled/disabled

    // File actions

    mUi->actionOpen->setEnabled(false);
    mUi->actionReopen->setEnabled(false);

    mUi->actionSave->setEnabled(false);
    mUi->actionSaveAs->setEnabled(false);
    mUi->actionSaveAll->setEnabled(false);

    mUi->actionClose->setEnabled(mDocumentManager->count());
    mUi->actionCloseAll->setEnabled(mDocumentManager->count());

    mUi->actionPrint->setEnabled(false);

    // Edit actions

    mUi->actionUndo->setEnabled(false);
    mUi->actionRedo->setEnabled(false);

    mUi->actionCut->setEnabled(false);
    mUi->actionCopy->setEnabled(false);
    mUi->actionPaste->setEnabled(false);
    mUi->actionDelete->setEnabled(false);

    mUi->actionFind->setEnabled(false);
    mUi->actionFindNext->setEnabled(false);
    mUi->actionFindPrevious->setEnabled(false);
    mUi->actionReplace->setEnabled(false);

    mUi->actionSelectAll->setEnabled(false);

    // Run actions

    mUi->actionRun->setEnabled(false);
    mUi->actionStop->setEnabled(false);
}

void MainWindow::on_actionNew_triggered()
{
    notYetImplemented("void MainWindow::on_actionNew_triggered()");
}

void MainWindow::on_actionCellML10File_triggered()
{
    notYetImplemented("void MainWindow::on_actionCellML10File_triggered()");
}

void MainWindow::on_actionCellML11File_triggered()
{
    notYetImplemented("void MainWindow::on_actionCellML11File_triggered()");
}

void MainWindow::on_actionOpen_triggered()
{
    notYetImplemented("void MainWindow::on_actionOpen_triggered()");
}

void MainWindow::on_actionReopen_triggered()
{
    notYetImplemented("void MainWindow::on_actionReopen_triggered()");
}

void MainWindow::on_actionSave_triggered()
{
    notYetImplemented("void MainWindow::on_actionSave_triggered()");
}

void MainWindow::on_actionSaveAs_triggered()
{
    notYetImplemented("void MainWindow::on_actionSaveAs_triggered()");
}

void MainWindow::on_actionSaveAll_triggered()
{
    notYetImplemented("void MainWindow::on_actionSaveAll_triggered()");
}

void MainWindow::on_actionPrint_triggered()
{
    notYetImplemented("void MainWindow::on_actionPrint_triggered()");
}

void MainWindow::on_actionUndo_triggered()
{
    notYetImplemented("void MainWindow::on_actionUndo_triggered()");
}

void MainWindow::on_actionRedo_triggered()
{
    notYetImplemented("void MainWindow::on_actionRedo_triggered()");
}

void MainWindow::on_actionCut_triggered()
{
    notYetImplemented("void MainWindow::on_actionCut_triggered()");
}

void MainWindow::on_actionCopy_triggered()
{
    notYetImplemented("void MainWindow::on_actionCopy_triggered()");
}

void MainWindow::on_actionPaste_triggered()
{
    notYetImplemented("void MainWindow::on_actionPaste_triggered()");
}

void MainWindow::on_actionDelete_triggered()
{
    notYetImplemented("void MainWindow::on_actionDelete_triggered()");
}

void MainWindow::on_actionFind_triggered()
{
    notYetImplemented("void MainWindow::on_actionFind_triggered()");
}

void MainWindow::on_actionFindNext_triggered()
{
    notYetImplemented("void MainWindow::on_actionFindNext_triggered()");
}

void MainWindow::on_actionFindPrevious_triggered()
{
    notYetImplemented("void MainWindow::on_actionFindPrevious_triggered()");
}

void MainWindow::on_actionReplace_triggered()
{
    notYetImplemented("void MainWindow::on_actionReplace_triggered()");
}

void MainWindow::on_actionSelectAll_triggered()
{
    notYetImplemented("void MainWindow::on_actionSelectAll_triggered()");
}

void MainWindow::on_actionFullScreen_triggered()
{
    // Switch to / back from full screen mode

    if (mUi->actionFullScreen->isChecked())
        showFullScreen();
    else
        showNormal();
}

void MainWindow::on_actionRun_triggered()
{
    notYetImplemented("void MainWindow::on_actionRun_triggered()");
}

void MainWindow::on_actionStop_triggered()
{
    notYetImplemented("void MainWindow::on_actionStop_triggered()");
}

void MainWindow::on_actionSystem_triggered()
{
    // Select the system's language as the language used by OpenCOR

    setLocale(SystemLocale);
}

void MainWindow::on_actionEnglish_triggered()
{
    // Select English as the language used by OpenCOR

    setLocale(EnglishLocale);
}

void MainWindow::on_actionFrench_triggered()
{
    // Select French as the language used by OpenCOR

    setLocale(FrenchLocale);
}

void MainWindow::on_actionPreferences_triggered()
{
    notYetImplemented("void MainWindow::on_actionPreferences_triggered()");
}

void MainWindow::on_actionHomePage_triggered()
{
    // Look up OpenCOR home page

    QDesktopServices::openUrl(QUrl(OpencorHomepageUrl));
}

void MainWindow::on_actionCellMLHomePage_triggered()
{
    // Look up CellML home page

    QDesktopServices::openUrl(QUrl(CellmlHomepageUrl));
}

void MainWindow::on_actionUpdates_triggered()
{
    notYetImplemented("void MainWindow::on_actionUpdates_triggered()");
}

void MainWindow::on_actionAbout_triggered()
{
    // Display some information about OpenCOR

    QMessageBox::about(this, qApp->applicationName(),
                        "<h1 align=center><b>"+qApp->applicationName()+" "+qApp->applicationVersion()+"</b></h1>"
                       +"<h3 align=center><em>"+getOsName()+"</em></h3>"
                       +"<p align=center><em>&copy;2011</em></p>"
                       +"<a href=\""+QString(OpencorHomepageUrl)+"\">"+qApp->applicationName()+"</a> "+tr("is a cross-platform <a href=\"http://www.cellml.org/\">CellML</a>-based modelling environment which can be used to organise, edit, simulate and analyse CellML files."));
}

void MainWindow::resetAll()
{
    if( QMessageBox::question(this, qApp->applicationName(),
                              tr("You are about to reset <strong>all</strong> of your user settings. Are you sure that this is what you want?"),
                              QMessageBox::Yes|QMessageBox::No,
                              QMessageBox::Yes) == QMessageBox::Yes ) {
        // We want to reset everything, so...
        // Note: the actual resetting is done in the main procedure

        mNeedResetAll = true;

        // Quit OpenCOR
        // Note: the closeEvent method won't get called and this is exactly what
        //       we want, since we don't want to save OpenCOR's settings

        qApp->quit();
    }
}

bool MainWindow::openFile(const QString &pFileName, const bool &pUserFeedback)
{
    // Register the file with our document manager

    DocumentManager::ManageStatus status = mDocumentManager->manage(pFileName);

    switch (status) {
    case DocumentManager::Added:
        // The file has been added to the manager, so we can carry on with the
        // opening of the file in our central widget

        mCentralWidget->openFile(pFileName);

        break;
    case DocumentManager::AlreadyManaged:
        // The file is already managed, so activate it

        mCentralWidget->activateFile(pFileName);

        break;
    case DocumentManager::DoesNotExist:
        // The file doesn't exist, so let the user know about it, if requested

        if (pUserFeedback)
            QMessageBox::information(0, qApp->applicationName()+" Information",
                                     QString(tr("Sorry, but the '%1' file does not exist.")).arg(pFileName));

        break;
    }

    // Update the actions, if pontentially needed

    if (status != DocumentManager::DoesNotExist) {
        updateActions();

        return true;
    } else {
        return false;
    }
}

void MainWindow::openFiles(const QStringList &pFileNames)
{
    // One or several files are to be opened

    for (int i = 0; i < pFileNames.count(); ++i)
        openFile(pFileNames.at(i));
}

void MainWindow::unmanageFile(const QString &pFileName,
                              const bool &pUserFeedback)
{
    // Unregister the given file from our document manager

    DocumentManager::UnmanageStatus status = mDocumentManager->unmanage(pFileName);

    switch (status) {
    case DocumentManager::Removed:
        // The file has been removed, so update the actions, just in case

        updateActions();

        break;
    case DocumentManager::NotManaged:
        // The file is not managed, so...

        if (pUserFeedback)
            QMessageBox::information(0, qApp->applicationName()+" Information",
                                     QString(tr("Sorry, but the '%1' file is not currently managed.")).arg(pFileName));

        break;
    }
}

bool MainWindow::closeFile(const bool &pUserFeedback)
{
    // Remove the current file from our central widget

    QString fileName = mCentralWidget->closeFile();

    // If a file was really closed, then unregister it from our document manager

    if (!fileName.isEmpty()) {
        unmanageFile(fileName, pUserFeedback);

        // Everything went file (i.e. a file was closed), so...

        return true;
    } else {
        // There was no file to close, so...

        return false;
    }
}

void MainWindow::closeFiles()
{
    // Remove all the files until there is none left

    while (closeFile()) {}
}

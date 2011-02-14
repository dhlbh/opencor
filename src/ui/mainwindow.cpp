#include "centralwidget.h"
#include "mainwindow.h"
#include "helpwindow.h"
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

#define OPENCOR_HOMEPAGE "http://opencor.sourceforge.net/"
#define OPENCOR_HELP_HOMEPAGE "qthelp://opencor/doc/userIndex.html"

#define SETTINGS_GENERAL_LOCALE "General_Locale"
#define SETTINGS_GENERAL_GEOMETRY "General_Geometry"
#define SETTINGS_GENERAL_STATE "General_State"
#define SETTINGS_MAINWINDOW_STATUSBARVISIBILITY "MainWindow_StatusBarVisibility"
#define SETTINGS_HELPWINDOW_ZOOMLEVEL "HelpWindow_ZoomLevel"

#define SYSTEM_LOCALE ""
#define ENGLISH_LOCALE "en"
#define FRENCH_LOCALE  "fr"

MainWindow::MainWindow(QWidget *pParent) :
    QMainWindow(pParent),
    mUi(new Ui::MainWindow),
    mLocale(SYSTEM_LOCALE)
{
    // Set the name of the main window to that of the application

    setWindowTitle(qApp->applicationName());

    // Set up the UI

    mUi->setupUi(this);
    // Note: the application icon (which needs to be set in the case of Linux,
    //       unlike in the case of Windows and Mac OS X, since it's set through
    //       CMake in those cases (see CMakeLists.txt)) is set within the UI
    //       file. Otherwise, it's good to have it set for all three platforms,
    //       since it can then be used in, for example, the about box...

    // Set the New toolbar button with its dropdown menu using our custom-made
    // action (actionNew)
    // Note: ideally, this would be done using the GUI designer in QtCreator,
    //       but it's not quite clear to me how this can be achieved, so...

    QMenu *actionNewMenu = new QMenu;

    mUi->actionNew->setMenu(actionNewMenu);

    actionNewMenu->addAction(mUi->actionCellML10File);
    actionNewMenu->addAction(mUi->actionCellML11File);

    mUi->fileToolbar->insertAction(mUi->actionOpen, mUi->actionNew);
    mUi->fileToolbar->insertSeparator(mUi->actionOpen);

    // Set the central widget

    mCentralWidget = new CentralWidget(this);

    setCentralWidget(mCentralWidget);

    // Allow for things to be dropped on us

    setAcceptDrops(true);

    // Create a temporary directory where to put OpenCOR's resources

    mTempDir = new QxtTemporaryDir();

    // Extract the help files

    QString applicationBaseName(QFileInfo(qApp->applicationFilePath()).baseName());

    mQchFileName = mTempDir->path()+QDir::separator()+applicationBaseName+".qch";
    mQhcFileName = mTempDir->path()+QDir::separator()+applicationBaseName+".qhc";

    saveResourceAs(":qchFile", mQchFileName);
    saveResourceAs(":qhcFile", mQhcFileName);

    // Set up the help engine

    mHelpEngine = new QHelpEngine(mQhcFileName);

    mHelpEngine->setupData();

    // Create the help window

    mHelpWindow = new HelpWindow(mHelpEngine, QUrl(OPENCOR_HELP_HOMEPAGE));

    // Some connections

    connect(mUi->actionExit, SIGNAL(triggered(bool)),
            this, SLOT(close()));
    connect(mUi->actionResetAll, SIGNAL(triggered(bool)),
            this, SLOT(resetAll()));

    connect(mUi->actionFileToolbar, SIGNAL(triggered(bool)),
            mUi->fileToolbar, SLOT(setVisible(bool)));
    connect(mUi->fileToolbar->toggleViewAction(), SIGNAL(toggled(bool)),
            mUi->actionFileToolbar, SLOT(setChecked(bool)));

    connect(mUi->actionHelpToolbar, SIGNAL(triggered(bool)),
            mUi->helpToolbar, SLOT(setVisible(bool)));
    connect(mUi->helpToolbar->toggleViewAction(), SIGNAL(toggled(bool)),
            mUi->actionHelpToolbar, SLOT(setChecked(bool)));

    connect(mUi->actionHelp, SIGNAL(triggered(bool)),
            mHelpWindow, SLOT(setVisible(bool)));
    connect(mHelpWindow, SIGNAL(visibilityChanged(bool)),
            mUi->actionHelp, SLOT(setChecked(bool)));

    connect(mUi->actionStatusBar, SIGNAL(triggered(bool)),
            statusBar(), SLOT(setVisible(bool)));

    // Default user settings (useful the very first time we start OpenCOR or
    // after a reset all)

    defaultSettings();

    // Retrieve our default settings

    loadSettings();

    // Bring ourselves to the foreground. Indeed, when restarting OpenCOR (as a
    // result of a reset all), OpenCOR will on Mac OS X be behind other
    // applications. This behaviour has, on occasions, also been observed on
    // Windows, so... rather than making the following code Mac OS X specific,
    // we may as well make it general, since it doesn't have any adverse effect
    // on Windows or Linux

    activateWindow();

    raise();   // Just to be on the safe side
}

MainWindow::~MainWindow()
{
    // Delete some internal objects

    delete mHelpEngine;
    delete mUi;

    // Delete the help files and then temporary directory by deleting the
    // temporary directory object

    QFile(mQchFileName).remove();
    QFile(mQhcFileName).remove();

    delete mTempDir;
}

void MainWindow::notYetImplemented(const QString &method)
{
    QMessageBox::information(this, qApp->applicationName()+" Information", "Sorry, but the '"+method+"' method has not yet been implemented.");
}

void MainWindow::changeEvent(QEvent *pEvent)
{
    QMainWindow::changeEvent(pEvent);

    switch (pEvent->type())
    {
        case QEvent::LocaleChange:
            // The system's locale has changed, so update OpenCOR's locale in
            // case we want to use the system's locale

            if (mUi->actionSystem->isChecked())
                setLocale(SYSTEM_LOCALE);

            break;
        default:
            break;
    }
}

void MainWindow::showEvent(QShowEvent *pEvent)
{
    static bool firstTime = true;

    if (firstTime) {
        firstTime = false;

        // The first time we show OpenCOR, we want to make sure that its menu
        // is fine (i.e. it respects OpenCOR's settings that were loaded in the
        // constructor)
        // Note: we must do this here (as opposed to, say, the constructor),
        //       because we need everything to be visible before deciding what
        //       needs to be done with the menus (e.g. we can't, within the
        //       constructor, tell whether the status bar is visible)

        updateMenuStatus();

        // Accept the event

        pEvent->accept();
    } else {
        // Nothing to do, so...

        pEvent->ignore();
    }
}

void MainWindow::closeEvent(QCloseEvent *pEvent)
{
    // Keep track of our default settings
    // Note: it must be done here, as opposed to the destructor, otherwise some
    //       settings (e.g. docked windows) won't be properly saved

    saveSettings();

    // Accept the event

    pEvent->accept();
}

void MainWindow::dragEnterEvent(QDragEnterEvent *pEvent)
{
    // Accept the proposed action for the event, but only if we are dropping
    // URLs

    if (pEvent->mimeData()->hasUrls())
        pEvent->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *pEvent)
{
    QList<QUrl> urlList = pEvent->mimeData()->urls();
    QString urls;

    for (int i = 0; i < urlList.size(); ++i)
        urls += "<LI>"+urlList.at(i).path()+"</LI>";

    QMessageBox::information(this, qApp->applicationName(), "You have just dropped the following files: <UL>"+urls+"</UL>");

    // Accept the proposed action for the event

    pEvent->acceptProposedAction();
}

void MainWindow::defaultSettings()
{
    // Default language to be used by OpenCOR

    setLocale(SYSTEM_LOCALE);

    // Default size and position of the main window

    double spaceRatio = 1.0/13.0;
    QRect desktopGeometry = qApp->desktop()->availableGeometry();
    int horizSpace = spaceRatio*desktopGeometry.width();
    int vertSpace  = spaceRatio*desktopGeometry.height();

    setGeometry(desktopGeometry.left()+horizSpace, desktopGeometry.top()+vertSpace, desktopGeometry.width()-2*horizSpace, desktopGeometry.height()-2*vertSpace);

    // Default visibility and location of the various toolbars

    addToolBar(Qt::TopToolBarArea, mUi->fileToolbar);
    addToolBar(Qt::TopToolBarArea, mUi->helpToolbar);

    mUi->fileToolbar->setVisible(true);
    mUi->helpToolbar->setVisible(true);

    // Default size and position of the help window

    mHelpWindow->setVisible(false);   // So we can set things up without having
                                      // the screen flashing

    addDockWidget(Qt::RightDockWidgetArea, mHelpWindow);

    mHelpWindow->defaultSettings();

    mHelpWindow->setVisible(true);
}

void MainWindow::loadSettings()
{
    QSettings settings(qApp->applicationName());

    // Retrieve the language to be used by OpenCOR

    setLocale(settings.value(SETTINGS_GENERAL_LOCALE, SYSTEM_LOCALE).toString());

    // Retrieve the geometry of the main window

    restoreGeometry(settings.value(SETTINGS_GENERAL_GEOMETRY).toByteArray());

    // Retrieve the state of the main window

    restoreState(settings.value(SETTINGS_GENERAL_STATE).toByteArray());

    // Retrieve whether the status bar is to be shown

    mUi->statusBar->setVisible(settings.value(SETTINGS_MAINWINDOW_STATUSBARVISIBILITY, true).toBool());

    // Retrieve the zoom level for the help widget

    mHelpWindow->setZoomLevel(settings.value(SETTINGS_HELPWINDOW_ZOOMLEVEL, mHelpWindow->defaultZoomLevel()).toInt());
}

void MainWindow::saveSettings()
{
    QSettings settings(qApp->applicationName());

    // Keep track of the language to be used by OpenCOR

    settings.setValue(SETTINGS_GENERAL_LOCALE, mLocale);

    // Keep track of the geometry of the main window

    settings.setValue(SETTINGS_GENERAL_GEOMETRY, saveGeometry());

    // Keep track of the state of the main window

    settings.setValue(SETTINGS_GENERAL_STATE, saveState());

    // Keep track of whether the status bar is to be shown

    settings.setValue(SETTINGS_MAINWINDOW_STATUSBARVISIBILITY, mUi->statusBar->isVisible());

    // Keep track of the text size multiplier for the help widget

    settings.setValue(SETTINGS_HELPWINDOW_ZOOMLEVEL, mHelpWindow->zoomLevel());
}

void MainWindow::setLocale(const QString &pLocale)
{
    if ((pLocale != mLocale) || (pLocale == SYSTEM_LOCALE)) {
        QString realLocale = (pLocale == SYSTEM_LOCALE)?QLocale::system().name().left(2):pLocale;

        mLocale = pLocale;

        // Specify the language to be used by OpenCOR

        qApp->removeTranslator(&mQtTranslator);
        mQtTranslator.load(":qt_"+realLocale);
        qApp->installTranslator(&mQtTranslator);

        qApp->removeTranslator(&mAppTranslator);
        mAppTranslator.load(":app_"+realLocale);
        qApp->installTranslator(&mAppTranslator);

        // Translate the whole GUI (including any 'child' window), should the
        // language have changed

        mUi->retranslateUi(this);

        mHelpWindow->retranslateUi();
    }

    // Update the checked menu item
    // Note: it has to be done every single time, since selecting a menu item
    //       will automatically toggle its checked status, so...

    mUi->actionSystem->setChecked(mLocale == SYSTEM_LOCALE);

    mUi->actionEnglish->setChecked(mLocale == ENGLISH_LOCALE);
    mUi->actionFrench->setChecked(mLocale == FRENCH_LOCALE);
}

void MainWindow::updateMenuStatus()
{
    // Update the checked status of the toolbars menu items

    mUi->actionFileToolbar->setChecked(mUi->fileToolbar->isVisible());
    mUi->actionHelpToolbar->setChecked(mUi->helpToolbar->isVisible());

    // Update the checked status of the status bar menu item

    mUi->actionStatusBar->setChecked(mUi->statusBar->isVisible());
}

void MainWindow::singleAppMsgRcvd(const QString&)
{
    // We have just received a message from another instance of OpenCOR, so
    // bring ourselves to the foreground
    // Note: one would normally use activateWindow(), but depending on the
    //       operating system it may or not bring OpenCOR to the foreground,
    //       so... instead we do what follows, depending on the operating
    //       system...

#ifdef Q_WS_WIN
    // Retrieve OpenCOR's window Id

    WId mwWinId = winId();

    // Restore OpenCOR, should it be minimized

    if (IsIconic(mwWinId))
        SendMessage(mwWinId, WM_SYSCOMMAND, SC_RESTORE, 0);

    // Bring OpenCOR to the foreground

    DWORD foregroundThreadPId = GetWindowThreadProcessId(GetForegroundWindow(), NULL);
    DWORD mwThreadPId         = GetWindowThreadProcessId(mwWinId, NULL);

    if (foregroundThreadPId != mwThreadPId) {
        // OpenCOR's thread process Id is not that of the foreground window, so
        // attach the foreground thread to OpenCOR's, set OpenCOR to the
        // foreground, and detach the foreground thread from OpenCOR's

        AttachThreadInput(foregroundThreadPId, mwThreadPId, true);

        SetForegroundWindow(mwWinId);

        AttachThreadInput(foregroundThreadPId, mwThreadPId, false);
    } else {
        // OpenCOR's thread process Id is that of the foreground window, so
        // just set OpenCOR to the foreground

        SetForegroundWindow(mwWinId);
    }

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

void MainWindow::on_actionSystem_triggered()
{
    // Select the system's language as the language used by OpenCOR

    setLocale(SYSTEM_LOCALE);
}

void MainWindow::on_actionEnglish_triggered()
{
    // Select English as the language used by OpenCOR

    setLocale(ENGLISH_LOCALE);
}

void MainWindow::on_actionFrench_triggered()
{
    // Select French as the language used by OpenCOR

    setLocale(FRENCH_LOCALE);
}

void MainWindow::on_actionHomePage_triggered()
{
    // Look up OpenCOR's home page

    QDesktopServices::openUrl(QUrl(OPENCOR_HOMEPAGE));
}

void MainWindow::on_actionAbout_triggered()
{
    // Display some information about OpenCOR

    QMessageBox::about(this, qApp->applicationName(),
                       QString("")+
                       "<CENTER>"+
                           "<H1><B>"+qApp->applicationName()+" "+qApp->applicationVersion()+"</B></H1>"+
                           "<H3><I>"+getOsName()+"</I></H3>"+
                       "</CENTER>"+
                       "<BR>"+
                       "<A HREF = \""+QString(OPENCOR_HOMEPAGE)+"\">"+qApp->applicationName()+"</A> "+tr("is a cross-platform <A HREF = \"http://www.cellml.org/\">CellML</A>-based modelling environment which can be used to organise, edit, simulate and analyse CellML files."));
}

void MainWindow::resetAll()
{
    if( QMessageBox::question(this, qApp->applicationName(),
                              tr("You are about to reset all the user settings. Are you sure about this?"),
                              QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes ) {
        // Clear all the user settings and restart OpenCOR (indeed, a restart
        // will ensure that all the docked windows are, for instance, properly
        // reset with regards to their dimensions)

        QSettings(qApp->applicationName()).clear();

        // Restart OpenCOR, but without providing any of the argument with
        // which OpenCOR was originally started, since we indeed want to reset
        // everything

        QProcess::startDetached(qApp->applicationFilePath(), QStringList(), qApp->applicationDirPath());

        // Quit OpenCOR
        // Note: the closeEvent method won't get called and this is exactly
        //       what we want, since we don't want to save OpenCOR's settings

        qApp->quit();
    }
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

void MainWindow::on_actionClose_triggered()
{
    notYetImplemented("void MainWindow::on_actionClose_triggered()");
}

void MainWindow::on_actionCloseAll_triggered()
{
    notYetImplemented("void MainWindow::on_actionCloseAll_triggered()");
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

void MainWindow::on_actionFullScreen_triggered()
{
    // Switch to / back from full screen mode

    if (mUi->actionFullScreen->isChecked())
        showFullScreen();
    else
        showNormal();
}

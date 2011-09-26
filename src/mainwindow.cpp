#include "checkforupdateswindow.h"
#include "common.h"
#include "guiinterface.h"
#include "i18ninterface.h"
#include "mainwindow.h"
#include "plugin.h"
#include "pluginmanager.h"
#include "pluginswindow.h"
#include "preferenceswindow.h"
#include "utils.h"

#include "ui_mainwindow.h"

#ifdef Q_WS_WIN
    #include <windows.h>
#endif

#include <QDesktopServices>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#ifdef Q_WS_MAC
    #include <QShortcut>
#endif
#include <QUrl>

namespace OpenCOR {

MainWindow::MainWindow(QWidget *pParent) :
    QMainWindow(pParent),
    mUi(new Ui::MainWindow)
{
    // Create our settings object

    mSettings = new QSettings(qApp->applicationName());

    // Create our plugin manager (which will automatically load our various
    // plugins)

    mPluginManager = new PluginManager(mSettings, PluginInfo::Gui);

    // Specify some general docking settings

    setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
    setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

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

    // Some connections to handle our Help toolbar

    GuiInterface::connectToolBarToToolBarAction(mUi->helpToolbar,
                                                mUi->actionHelpToolbar);

    // A connection to handle the status bar

    connect(mUi->actionStatusBar, SIGNAL(triggered(bool)),
            statusBar(), SLOT(setVisible(bool)));

    // Some connections to handle our various menu items

    connect(mUi->actionExit, SIGNAL(triggered(bool)),
            this, SLOT(close()));
    connect(mUi->actionResetAll, SIGNAL(triggered(bool)),
            this, SLOT(resetAll()));

    // Initialise our various plugins

    QList<Plugin *> loadedPlugins = mPluginManager->loadedPlugins();

    foreach (Plugin *plugin, loadedPlugins) {
        // Do various things that are related to our differennt plugin
        // interfaces
        // Note: the order in which we do those things is important since, for
        //       example, the call to CoreInterface::initialize() may need some
        //       information which has been set as part of the GUI interface
        //       (e.g. the pointer to the main window)...

        // GUI interface

        GuiInterface *guiInterface = qobject_cast<GuiInterface *>(plugin->instance());

        if (guiInterface) {
            // Keep track of some information

            guiInterface->setMainWindow(this);
            guiInterface->setGuiPluginName(plugin->name());
        }

        // Internationalisation interface

        I18nInterface *i18nInterface = qobject_cast<I18nInterface *>(plugin->instance());

        if (i18nInterface)
            // Keep track of some information

            i18nInterface->setI18nPluginName(plugin->name());

        // Core interface

        CoreInterface *coreInterface = qobject_cast<CoreInterface *>(plugin->instance());

        if (coreInterface) {
            // Keep track of some information

            coreInterface->setLoadedPlugins(loadedPlugins);

            // Initialise the plugin

            coreInterface->initialize();
        }

        // Back to the GUI interface

        if (guiInterface)
            // Initialise the plugin further (i.e. do things which can only be
            // done by OpenCOR itself)

            initializeGuiPlugin(plugin->name(), guiInterface);
    }

#ifdef Q_WS_MAC
    // A special shortcut to have OpenCOR minimised on Mac OS X when pressing
    // Cmd+M
    // Note: indeed, when pressing Cmd+M on Mac OS X, the active application
    //       is expected to minimise itself, so...

    new QShortcut(QKeySequence("Ctrl+M"),
                  this, SLOT(showMinimized()));

    // A special shortcut to have OpenCOR hidden on Mac OS X when pressing
    // Cmd+H
    // Note: indeed, when pressing Cmd+H on Mac OS X, the active application
    //       is expected to hide itself, so...

    new QShortcut(QKeySequence("Ctrl+H"),
                  this, SLOT(hide()));
#endif

    // Retrieve the user settings from the previous session, if any

    loadSettings();

    // Initialise the checking state of the full screen action, since OpenCOR
    // may (re)start in full screen mode

    mUi->actionFullScreen->setChecked(isFullScreen());

    // Bring ourselves to the foreground
    // Note: indeed, when starting/restarting OpenCOR (as a result of a Reset
    //       All in the case of a restart), OpenCOR will on Mac OS X be behind
    //       any other application. Now, because it doesn't harm to bring
    //       ourselves to the foreground when on Windows or Linux, we may as
    //       well do it on those platforms too...

    raise();
}

MainWindow::~MainWindow()
{
    // Finalize our various plugins
    // Note: we only need to test for our default interface since we want to
    //       call the finalize method and this method is not overriden by any
    //       other interface, so...

    foreach (Plugin *plugin, mPluginManager->loadedPlugins()) {
        CoreInterface *coreInterface = qobject_cast<CoreInterface *>(plugin->instance());

        if (coreInterface)
            coreInterface->finalize();
    }

    // Delete some internal objects

    delete mPluginManager;
    delete mSettings;
    delete mUi;
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

void MainWindow::initializeGuiPlugin(const QString &pPluginName,
                                     GuiInterface *pGuiInterface)
{
    // Check whether we are dealing with our special Help plugin

    if (!pPluginName.compare(HelpPlugin) && pGuiInterface->data()) {
        // We are dealing with our special Help plugin and its data is set, so
        // we can make use of it

        QAction *helpAction = ((GuiHelpSettings *) pGuiInterface->data())->helpAction();

        // Add the action to our help menu

        mUi->menuHelp->insertAction(mUi->actionHomePage, helpAction);
        mUi->menuHelp->insertSeparator(mUi->actionHomePage);

        // As well as to our help tool bar

        mUi->helpToolbar->insertAction(mUi->actionHomePage, helpAction);
        mUi->helpToolbar->insertSeparator(mUi->actionHomePage);
    }

    // Deal with the menus and actions that the plugin may want us to add
    // (insert)

    // Add the menus to our menu bar or merge them to existing menus, if needed
    // Note: we must do that in reverse order since we are inserting menus,
    //       as opposed to appending menus...

    QListIterator<GuiMenuSettings *> menuIter(pGuiInterface->guiSettings()->menus());

    menuIter.toBack();

    while (menuIter.hasPrevious()) {
        // Insert the menu in the right place

        GuiMenuSettings *menuSettings = menuIter.previous();

        QMenu *newMenu = menuSettings->menu();
        QString newMenuName = newMenu->objectName();

        QMenu *oldMenu = mMenus.value(newMenuName);

        if (oldMenu) {
            // A menu with the same name already exists, so add the contents of
            // the new menu to the existing one

            oldMenu->addSeparator();
            oldMenu->addActions(newMenu->actions());

            // Delete the new menu, since we don't need it...
            // Note: it's not critical since the menu never gets shown, but it
            //       doesn't harm either, so...

            delete newMenu;
        } else {
            // No menu with the same name already exists, so add the new menu to
            // our menu bar

            switch (menuSettings->type()) {
            default:   // View
                mUi->menuBar->insertAction(mUi->menuView->menuAction(),
                                           newMenu->menuAction());
            }

            // Keep track of the new menu

            mMenus.insert(newMenuName, newMenu);
        }
    }

    // Add the actions/separators to our different menus
    // Note: as for the menus above, we must do that in reverse order since we
    //       are inserting actions, as opposed to appending actions...

    QListIterator<GuiMenuActionSettings *> menuActionIter(pGuiInterface->guiSettings()->menuActions());

    menuActionIter.toBack();

    while (menuActionIter.hasPrevious()) {
        // Inster the action/separator to the right menu

        GuiMenuActionSettings *menuActionSettings = menuActionIter.previous();

        switch (menuActionSettings->type()) {
        default:   // File
            if(menuActionSettings->action())
                mUi->menuFile->insertAction(mUi->menuFile->actions().first(),
                                            menuActionSettings->action());
            else
                mUi->menuFile->insertSeparator(mUi->menuFile->actions().first());
        }
    }

    // Add the toolbars (including to the View|Toolbars menu)

    foreach (GuiToolBarSettings *toolbarSettings,
             pGuiInterface->guiSettings()->toolbars()) {
        QToolBar *newToolbar = toolbarSettings->toolbar();
        QString newToolbarName = newToolbar->objectName();

        QToolBar *oldToolbar = mToolbars.value(newToolbarName);

        if (oldToolbar) {
            // A toolbar with the same name already exists, so add the contents
            // of the new toolbar to the existing one

            oldToolbar->addSeparator();
            oldToolbar->addActions(newToolbar->actions());

            // Delete the new toolbar, since we don't need it...
            // Note: this prevents from the toolbar from being shown in the
            //       top-left corner of the main window

            delete newToolbar;
        } else {
            // No toolbar with the same name already exists, so add the new
            // toolbar

            addToolBar(toolbarSettings->defaultDockingArea(), newToolbar);

            // Also add a toolbar action to our View|Toolbars menu

            mUi->menuToolbars->addAction(toolbarSettings->toolbarAction());

            // Keep track of the new toolbar

            mToolbars.insert(newToolbarName, newToolbar);
        }
    }

    // Reorder the View|Toolbars menu, just in case

    reorderViewToolbarsMenu();
}

static const QString SettingsLocale              = "Locale";
static const QString SettingsGeometry            = "Geometry";
static const QString SettingsState               = "State";
static const QString SettingsStatusBarVisibility = "StatusBarVisibility";

void MainWindow::loadSettings()
{
    // Retrieve the language to be used by OpenCOR

    setLocale(mSettings->value(SettingsLocale, SystemLocale).toString());

    // Retrieve the geometry and state of the main window

    bool needDefaultSettings =    !restoreGeometry(mSettings->value(SettingsGeometry).toByteArray())
                               || !restoreState(mSettings->value(SettingsState).toByteArray());

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

        mUi->statusBar->setVisible(mSettings->value(SettingsStatusBarVisibility,
                                                    true).toBool());
    }

    // Retrieve the settings of our various plugins

    foreach (Plugin *plugin, mPluginManager->loadedPlugins()) {
        GuiInterface *guiInterface = qobject_cast<GuiInterface *>(plugin->instance());

        if (guiInterface) {
            mSettings->beginGroup(plugin->name());
                guiInterface->loadSettings(mSettings, needDefaultSettings);
            mSettings->endGroup();
        }
    }
}

void MainWindow::saveSettings() const
{
    // Keep track of the language to be used by OpenCOR

    mSettings->setValue(SettingsLocale, mLocale);

    // Keep track of the geometry of the main window

    mSettings->setValue(SettingsGeometry, saveGeometry());

    // Keep track of the state of the main window

    mSettings->setValue(SettingsState, saveState());

    // Keep track of whether the status bar is to be shown

    mSettings->setValue(SettingsStatusBarVisibility,
                        mUi->statusBar->isVisible());

    // Keep track of the settings of our various plugins

    foreach (Plugin *plugin, mPluginManager->loadedPlugins()) {
        GuiInterface *guiInterface = qobject_cast<GuiInterface *>(plugin->instance());

        if (guiInterface) {
            mSettings->beginGroup(plugin->name());
                guiInterface->saveSettings(mSettings);
            mSettings->endGroup();
        }
    }
}

QString MainWindow::locale() const
{
    // Return the current locale

    const QString systemLocale = QLocale::system().name().left(2);

    return (mLocale == SystemLocale)?systemLocale:mLocale;
}

void MainWindow::setLocale(const QString &pLocale)
{
    if ((pLocale != mLocale) || (pLocale == SystemLocale)) {
        // The new locale is different from the existing one, so we need to
        // translate everything

        const QString systemLocale = QLocale::system().name().left(2);

        QString realLocale = (pLocale == SystemLocale)?systemLocale:pLocale;

        mLocale = pLocale;

        // Specify the language to be used by OpenCOR

        qApp->removeTranslator(&mQtTranslator);
        mQtTranslator.load(":qt_"+realLocale);
        qApp->installTranslator(&mQtTranslator);

        qApp->removeTranslator(&mAppTranslator);
        mAppTranslator.load(":app_"+realLocale);
        qApp->installTranslator(&mAppTranslator);

        // Retranslate OpenCOR

        mUi->retranslateUi(this);

        // Update the locale of our various plugins

        foreach (Plugin *plugin, mPluginManager->loadedPlugins()) {
            I18nInterface *i18nInterface = qobject_cast<I18nInterface *>(plugin->instance());

            if (i18nInterface)
                i18nInterface->setLocale(realLocale);
        }

        // Reorder the View|Toolbars menu, just in case

        reorderViewToolbarsMenu();
    }

    // Update the checked menu item
    // Note: it has to be done every single time, since selecting a menu item
    //       will automatically toggle its checked status, so...

    mUi->actionSystem->setChecked(mLocale == SystemLocale);

    mUi->actionEnglish->setChecked(mLocale == EnglishLocale);
    mUi->actionFrench->setChecked(mLocale == FrenchLocale);
}

void MainWindow::reorderViewToolbarsMenu()
{
    // Reorder the View|Toolbars menu
    // Note: this is useful after having added a new menu item or after having
    //       changed the locale

    QStringList menuItemTitles;
    QMap<QString, QAction *> menuItemActions;

    // Retrieve the title of the menu items and keep track of their actions

    foreach(QAction *menuItemAction, mUi->menuToolbars->actions()) {
        QString menuItemTitle = menuItemAction->text().remove("&");

        menuItemTitles.append(menuItemTitle);
        menuItemActions.insert(menuItemTitle, menuItemAction);
    }

    // Sort the menu items

    menuItemTitles.sort();

    // Add the menu items actions in the new order
    // Note: to use addAction will effectively 'move' the menu items to the end
    //       of the menu, so since we do it in the right order, we end up with
    //       the menu items being properly ordered...

    foreach(const QString menuItemTitle, menuItemTitles)
        mUi->menuToolbars->addAction(menuItemActions.value(menuItemTitle));
}

#ifdef Q_WS_WIN
void MainWindow::singleAppMsgRcvd(const QString &) const
#else
void MainWindow::singleAppMsgRcvd(const QString &)
#endif
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
    // Do what one would normally do

    activateWindow();

    raise();   // Just to be on the safe side
    // Note: raise() never seems to be required on Mac OS X, but to use
    //       activateWindow() on its own under Linux may or not give the
    //       expected result, so...
#endif

    // Now, we must handle the arguments that were passed to OpenCOR

    // TODO: handle the arguments passed to the 'official' instance of OpenCOR
}

void MainWindow::on_actionFullScreen_triggered()
{
    // Switch to / back from full screen mode

    if (mUi->actionFullScreen->isChecked())
        showFullScreen();
    else
        showNormal();
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

void MainWindow::on_actionPlugins_triggered()
{
    // Plugins' preferences

    if (mPluginManager->plugins().count()) {
        // There are some plugins, so we can show the plugins window

        OpenCOR::PluginsWindow pluginsWindow(mPluginManager, this);

        pluginsWindow.exec();
    } else {
        // There are no plugins, so...

        QMessageBox::warning(this, tr("Plugins"),
                             tr("No plugin could be found."));
    }
}

void MainWindow::on_actionPreferences_triggered()
{
    // User's preferences

    OpenCOR::PreferencesWindow preferencesWindow(this);

    preferencesWindow.exec();
}

void MainWindow::on_actionHomePage_triggered()
{
    // Look up OpenCOR home page

    QDesktopServices::openUrl(QUrl(OpencorHomepageUrl));
}

void MainWindow::on_actionCheckForUpdates_triggered()
{
    // Check for updates

    OpenCOR::CheckForUpdatesWindow checkForUpdatesWindow(this);

    checkForUpdatesWindow.exec();
}

void MainWindow::on_actionAbout_triggered()
{
    // Display some information about OpenCOR

    QMessageBox::about(this, tr("About"),
                        "<h1 align=center><b>"+getAppVersion(qApp)+"</b></h1>"
                       +"<h3 align=center><em>"+getOsName()+"</em></h3>"
                       +"<p align=center><em>"+getAppCopyright(true)+"</em></p>"
                       +"<a href=\""+QString(OpencorHomepageUrl)+"\">"+qApp->applicationName()+"</a> "+tr("is a cross-platform <a href=\"http://www.cellml.org/\">CellML</a>-based modelling environment which can be used to organise, edit, simulate and analyse CellML files."));
}

void MainWindow::restart(const bool &pSaveSettings) const
{
    // Restart OpenCOR after saving its settings, if required
    // Note: the closeEvent method won't get called when exiting OpenCOR and
    //       this is exactly what we want in case we don't want to save its
    //       settings (e.g. when resetting all)

    if (pSaveSettings)
        saveSettings();

    qApp->exit(NeedRestart);
}

void MainWindow::resetAll()
{
    if( QMessageBox::question(this, qApp->applicationName(),
                              tr("You are about to reset <strong>all</strong> of your user settings. Are you sure that this is what you want?"),
                              QMessageBox::Yes|QMessageBox::No,
                              QMessageBox::Yes) == QMessageBox::Yes ) {
        // We want to reset everything, so clear all the user settings and
        // restart OpenCOR (indeed, a restart will ensure that the various dock
        // windows are, for instance, properly reset with regards to their
        // dimensions)

        QSettings(qApp->applicationName()).clear();

        // Restart OpenCOR without first saving its settings

        restart(false);
    }
}

}

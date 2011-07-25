#include "guiinterface.h"

#include <QAction>
#include <QApplication>

#include <QDebug>

namespace OpenCOR {

GuiSettingsCorePlugin::GuiSettingsCorePlugin(Core::CentralWidget *pCentralWidget) :
    mCentralWidget(pCentralWidget)
{
}

Core::CentralWidget * GuiSettingsCorePlugin::centralWidget() const
{
    // Return the central widget

    return mCentralWidget;
}

GuiSettingsHelpPlugin::GuiSettingsHelpPlugin(QAction *pHelpAction,
                                             Core::DockWidget *pHelpWindow) :
    mHelpAction(pHelpAction),
    mHelpWindow(pHelpWindow)
{
}

QAction * GuiSettingsHelpPlugin::helpAction() const
{
    // Return the help action

    return mHelpAction;
}

Core::DockWidget * GuiSettingsHelpPlugin::helpWindow() const
{
    // Return the help window

    return mHelpWindow;
}

GuiSettingsCentralWidget::GuiSettingsCentralWidget(Core::CentralWidget *pCentralWidget) :
    mCentralWidget(pCentralWidget)
{
}

Core::CentralWidget * GuiSettingsCentralWidget::centralWidget() const
{
    // Return the central widget

    return mCentralWidget;
}

GuiSettingsAction::GuiSettingsAction(const GuiSettingsActionType &pType,
                                     QAction *pAction) :
    mType(pType),
    mAction(pAction)
{
}

GuiSettingsAction::GuiSettingsActionType GuiSettingsAction::type() const
{
    // Return the action's type

    return mType;
}

QAction * GuiSettingsAction::action() const
{
    // Return the action itself

    return mAction;
}

GuiSettingsWindow::GuiSettingsWindow(const Qt::DockWidgetArea &pDefaultDockingArea,
                                     Core::DockWidget *pWindow) :
    mDefaultDockingArea(pDefaultDockingArea),
    mWindow(pWindow)
{
}

Qt::DockWidgetArea GuiSettingsWindow::defaultDockingArea() const
{
    // Return the window's default docking area

    return mDefaultDockingArea;
}

Core::DockWidget * GuiSettingsWindow::window() const
{
    // Return the window itself

    return mWindow;
}

void GuiSettings::addCentralWidget(Core::CentralWidget *pCentralWidget)
{
    // Add the central widget
    // Note: we can have only one central widget, so...

    if (mCentralWidget.isEmpty())
        mCentralWidget << GuiSettingsCentralWidget(pCentralWidget);
}

void GuiSettings::addAction(const GuiSettingsAction::GuiSettingsActionType &pType,
                            QAction *pAction)
{
    // Add a new action to our list

    mActions << GuiSettingsAction(pType, pAction);
}

void GuiSettings::addWindow(const Qt::DockWidgetArea &pDefaultDockingArea,
                            Core::DockWidget *pWindow)
{
    // Add a new dock widget to our list

    mWindows << GuiSettingsWindow(pDefaultDockingArea, pWindow);
}

QList<GuiSettingsCentralWidget> GuiSettings::centralWidget() const
{
    // Return our central widget

    return mCentralWidget;
}

QList<GuiSettingsAction> GuiSettings::actions() const
{
    // Return our actions

    return mActions;
}

QList<GuiSettingsWindow> GuiSettings::windows() const
{
    // Return our windows

    return mWindows;
}

GuiInterface::GuiInterface(const QString &pPluginName) :
    mData(0),
    mPluginName(pPluginName)
{
}

void GuiInterface::initialize(QMainWindow *)
{
    // Nothing to do by default...
}

GuiSettings GuiInterface::settings() const
{
    // Return the plugin's settings

    return mSettings;
}

void * GuiInterface::data() const
{
    // Return the plugin's data

    return mData;
}

QString GuiInterface::pluginName() const
{
    // Return the plugin's name

    return mPluginName;
}

void GuiInterface::setLocale(const QString &pLocale)
{
    // Update the plugin's translator

    qApp->removeTranslator(&mTranslator);
    mTranslator.load(QString(":%1_%2").arg(mPluginName, pLocale));
    qApp->installTranslator(&mTranslator);

    // Retranslate the plugin

    retranslateUi();
}

QAction * GuiInterface::newAction(QMainWindow *pMainWindow,
                                  const bool &pCheckable,
                                  const QString &pIconResource)
{
    // Create and return an action

    QAction *res = new QAction(pMainWindow);

    res->setCheckable(pCheckable);
    res->setIcon(QIcon(pIconResource));

    return res;
}

void GuiInterface::retranslateUi()
{
    // Nothing to do by default...
}

}

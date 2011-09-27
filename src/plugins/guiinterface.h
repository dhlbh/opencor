#ifndef GUIINTERFACE_H
#define GUIINTERFACE_H

#include <QtPlugin>

#include <QList>

class QAction;
class QDockWidget;
class QMainWindow;
class QMenu;
class QSettings;
class QTabBar;
class QToolBar;

namespace OpenCOR {
namespace Core {
    class CentralWidget;
    class DockWidget;
}

static const QString CorePlugin = "Core";

class GuiCoreSettings
{
public:
    explicit GuiCoreSettings(Core::CentralWidget *pCentralWidget);

    Core::CentralWidget * centralWidget() const;

private:
    Core::CentralWidget *mCentralWidget;
};

class GuiMenuSettings
{
public:
    enum GuiMenuSettingsType
    {
        View
    };

    explicit GuiMenuSettings(const GuiMenuSettingsType &pType,
                             QMenu *pMenu);

    GuiMenuSettingsType type() const;
    QMenu * menu() const;

private:
    GuiMenuSettingsType mType;
    QMenu *mMenu;
};

class GuiMenuActionSettings
{
public:
    enum GuiMenuActionSettingsType
    {
        File
    };

    explicit GuiMenuActionSettings(const GuiMenuActionSettingsType &pType,
                                   QAction *pAction);

    GuiMenuActionSettingsType type() const;
    QAction * action() const;

private:
    GuiMenuActionSettingsType mType;
    QAction *mAction;
};

class GuiToolBarSettings
{
public:
    explicit GuiToolBarSettings(const Qt::ToolBarArea &pDefaultDockingArea,
                                QToolBar *pToolbar, QAction *pAction);

    Qt::ToolBarArea defaultDockingArea() const;
    QToolBar * toolbar() const;
    QAction * action() const;

private:
    Qt::ToolBarArea mDefaultDockingArea;
    QToolBar *mToolbar;
    QAction *mAction;
};

class GuiViewSettings
{
public:
    enum Mode
    {
        Editing,
        Simulation,
        Analysis
    };

    explicit GuiViewSettings(const Mode &pMode);

    Mode mode() const;

    void setTabBar(QTabBar *pTabBar);
    void setTabIndex(const int &pTabIndex);

    QTabBar * tabBar() const;
    int tabIndex() const;

private:
    Mode mMode;

    QTabBar *mTabBar;
    int mTabIndex;
};

class GuiWindowSettings
{
public:
    enum GuiWindowSettingsType
    {
        Organisation,
        Help
    };

    explicit GuiWindowSettings(const Qt::DockWidgetArea &pDefaultDockingArea,
                               Core::DockWidget *pWindow,
                               const GuiWindowSettingsType &pType,
                               QAction *pAction);

    Qt::DockWidgetArea defaultDockingArea() const;
    Core::DockWidget * window() const;
    GuiWindowSettingsType type() const;
    QAction * action() const;

private:
    Qt::DockWidgetArea mDefaultDockingArea;
    Core::DockWidget *mWindow;
    GuiWindowSettingsType mType;
    QAction *mAction;
};

class GuiSettings
{
public:
    ~GuiSettings();

    void addMenu(const GuiMenuSettings::GuiMenuSettingsType &pType,
                 QMenu *pMenu);
    void addMenuAction(const GuiMenuActionSettings::GuiMenuActionSettingsType &pType,
                       QAction *pAction = 0);
    void addToolBar(const Qt::ToolBarArea &pDefaultDockingArea,
                    QToolBar *pToolbar, QAction *pAction);
    void addView(const GuiViewSettings::Mode &pMode);
    void addWindow(const Qt::DockWidgetArea &pDefaultDockingArea,
                   Core::DockWidget *pWindow,
                   const GuiWindowSettings::GuiWindowSettingsType &pType,
                   QAction *pAction);

    QList<GuiMenuSettings *> menus() const;
    QList<GuiMenuActionSettings *> menuActions() const;
    QList<GuiToolBarSettings *> toolbars() const;
    QList<GuiViewSettings *> views() const;
    QList<GuiWindowSettings *> windows() const;

private:
    QList<GuiMenuSettings *> mMenus;
    QList<GuiMenuActionSettings *> mMenuActions;
    QList<GuiToolBarSettings *> mToolbars;
    QList<GuiViewSettings *> mViews;
    QList<GuiWindowSettings *> mWindows;
};

class GuiInterface
{
public:
    explicit GuiInterface();
    ~GuiInterface();

    virtual void loadSettings(QSettings *pSettings);
    virtual void saveSettings(QSettings *pSettings) const;

    GuiSettings * guiSettings() const;
    void * data() const;

    void setMainWindow(QMainWindow *pMainWindow);
    void setGuiPluginName(const QString &pGuiPluginName);

    static void connectToolBarToAction(QToolBar *pToolbar, QAction *pAction);
    static void connectDockWidgetToAction(QDockWidget *pDockWidget, QAction *pAction);

    static void retranslateMenu(QMenu *pMenu, const QString &pTitle);

protected:
    QMainWindow *mMainWindow;

    GuiSettings *mGuiSettings;
    void *mData;
    // Note: mData is used only by the Core and Help plugins which are both one
    //       of a kind and therefore require special treatment (as opposed to
    //       generic treatment)

    static QMenu * newMenu(QMainWindow *pMainWindow, const QString &pName);
    static QToolBar * newToolBar(QMainWindow *pMainWindow,
                                 const QString &pName);
    static QAction * newAction(QMainWindow *pMainWindow,
                               const bool &pCheckable = false,
                               const QString &pIconResource = QString(),
                               const bool &pVisible = true);

    static void retranslateAction(QAction *pAction, const QString &pText,
                                  const QString &pStatusTip,
                                  const QString &pShortcut = QString());

    void loadWindowSettings(QSettings *pSettings,
                            Core::DockWidget *pWindow);
    void saveWindowSettings(QSettings *pSettings,
                            Core::DockWidget *pWindow) const;

private:
    QString mGuiPluginName;
};

}

Q_DECLARE_INTERFACE(OpenCOR::GuiInterface, "OpenCOR.GuiInterface")

#endif

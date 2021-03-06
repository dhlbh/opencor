//==============================================================================
// Plugin manager
//==============================================================================

#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

//==============================================================================

#include "coreinterface.h"
#include "plugininfo.h"

//==============================================================================

#include <QObject>
#include <QSettings>

//==============================================================================

namespace OpenCOR {

//==============================================================================

class Plugin;

//==============================================================================

class PluginManager : public QObject
{
    Q_OBJECT

public:
    explicit PluginManager(QSettings *pSettings,
                           const PluginInfo::Type &pGuiOrConsoleType);
    ~PluginManager();

    Plugins plugins() const;
    Plugins loadedPlugins() const;

    QString pluginsDir() const;
    Plugin * plugin(const QString &pName) const;

    QSettings * settings() const;

    PluginInfo::Version version() const;

private:
    QSettings *mSettings;

    PluginInfo::Version mVersion;
    PluginInfo::Type mGuiOrConsoleType;

    QString mPluginsDir;
    Plugins mPlugins;
};

//==============================================================================

}   // namespace OpenCOR

//==============================================================================

#endif

//==============================================================================
// End of file
//==============================================================================

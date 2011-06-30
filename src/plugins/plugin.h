#ifndef PLUGIN_H
#define PLUGIN_H

#include <QObject>

namespace OpenCOR {

class PluginInfo;

class Plugin : public QObject
{
    Q_OBJECT

public:
    explicit Plugin(const QString &pName);
    ~Plugin();

    QString name();

    bool isLoaded();

    static QString pluginName(const QString &pPluginFileName);
    static PluginInfo pluginInfo(const QString &pPluginFileName);

private:
    QString mName;

    bool mLoaded;
};

}

#endif

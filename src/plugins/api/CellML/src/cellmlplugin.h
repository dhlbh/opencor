#ifndef CELLMLPLUGIN_H
#define CELLMLPLUGIN_H

#include "plugininterface.h"

#include <QLibrary>

namespace OpenCOR {
namespace CellML {

PLUGININFO_FUNC CellMLPluginInfo();

class CellMLPlugin : public PluginInterface
{
    Q_OBJECT
    Q_INTERFACES(OpenCOR::PluginInterface)

public:
    virtual void initialize(const QList<Plugin *> &);
    virtual void finalize();

private:
    QStringList mLibFileNames;
    QList<QLibrary *> mLibs;

    void loadLibrary(const QString pLibName);
};

} }

#endif

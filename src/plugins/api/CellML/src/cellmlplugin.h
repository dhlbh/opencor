#ifndef CELLMLPLUGIN_H
#define CELLMLPLUGIN_H

#include "coreinterface.h"
#include "apiinterface.h"

namespace OpenCOR {
namespace CellML {

PLUGININFO_FUNC CellMLPluginInfo();

class CellMLPlugin : public ApiInterface
{
    Q_OBJECT
    Q_INTERFACES(OpenCOR::CoreInterface)
    Q_INTERFACES(OpenCOR::ApiInterface)

public:
    virtual QList<FileType> fileTypes() const;
};

} }

#endif

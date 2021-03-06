//==============================================================================
// CellML API plugin
//==============================================================================

#ifndef CELLMLAPIPLUGIN_H
#define CELLMLAPIPLUGIN_H

//==============================================================================

#include "plugininfo.h"

//==============================================================================

namespace OpenCOR {
namespace CellMLAPI {

//==============================================================================

PLUGININFO_FUNC CellMLAPIPluginInfo();

//==============================================================================

class CellMLAPIPlugin : public QObject
{
    Q_OBJECT
};

//==============================================================================

}   // namespace CellMLAPI
}   // namespace OpenCOR

//==============================================================================

#endif

//==============================================================================
// End of file
//==============================================================================

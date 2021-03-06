//==============================================================================
// QtMmlWidget plugin
//==============================================================================

#ifndef QTMMLWIDGETPLUGIN_H
#define QTMMLWIDGETPLUGIN_H

//==============================================================================

#include "plugininfo.h"

//==============================================================================

namespace OpenCOR {
namespace QtMmlWidget {

//==============================================================================

PLUGININFO_FUNC QtMmlWidgetPluginInfo();

//==============================================================================

class QtMmlWidgetPlugin : public QObject
{
    Q_OBJECT
};

//==============================================================================

}   // namespace QtMmlWidget
}   // namespace OpenCOR

//==============================================================================

#endif

//==============================================================================
// End of file
//==============================================================================

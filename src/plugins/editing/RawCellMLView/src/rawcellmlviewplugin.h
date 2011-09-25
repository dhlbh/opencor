#ifndef RAWCELLMLVIEWPLUGIN_H
#define RAWCELLMLVIEWPLUGIN_H

#include "guiinterface.h"
#include "plugininfo.h"

namespace OpenCOR {
namespace RawCellMLView {

PLUGININFO_FUNC RawCellMLViewPluginInfo();

class RawCellMLViewPlugin : public GuiInterface
{
    Q_OBJECT
    Q_INTERFACES(OpenCOR::CoreInterface)
    Q_INTERFACES(OpenCOR::GuiInterface)

public:
    explicit RawCellMLViewPlugin();

protected:
    virtual void retranslateUi();
};

} }

#endif

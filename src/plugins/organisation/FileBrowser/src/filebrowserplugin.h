#ifndef FILEBROWSERPLUGIN_H
#define FILEBROWSERPLUGIN_H

#include "guiinterface.h"

namespace OpenCOR {
namespace FileBrowser {

PLUGININFO_FUNC FileBrowserPluginInfo();

class FileBrowserPlugin : public GuiInterface
{
    Q_OBJECT
    Q_INTERFACES(OpenCOR::CoreInterface)
    Q_INTERFACES(OpenCOR::GuiInterface)

public:
    explicit FileBrowserPlugin();
};

} }

#endif

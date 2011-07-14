#ifndef HELPPLUGIN_H
#define HELPPLUGIN_H

#include "guiinterface.h"

namespace OpenCOR {
namespace Help {

PLUGININFO_FUNC HelpPluginInfo();

class HelpWindow;

class HelpPlugin : public GuiInterface
{
    Q_OBJECT
    Q_INTERFACES(OpenCOR::GuiInterface)

public:
    explicit HelpPlugin();

    virtual void initialize(MainWindow *pMainWindow);
    virtual void finalize();

    virtual void retranslateUi();

private:
    HelpWindow *mHelpWindow;
};

} }

#endif

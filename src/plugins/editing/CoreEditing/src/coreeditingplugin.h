#ifndef COREEDITINGPLUGIN_H
#define COREEDITINGPLUGIN_H

#include "coreinterface.h"
#include "guiinterface.h"
#include "i18ninterface.h"
#include "plugininfo.h"

namespace OpenCOR {
namespace CoreEditing {

PLUGININFO_FUNC CoreEditingPluginInfo();

static const QString EditGroup = "Edit";

class CoreEditingPlugin : public QObject, public CoreInterface,
                          public GuiInterface, public I18nInterface
{
    Q_OBJECT
    Q_INTERFACES(OpenCOR::CoreInterface)
    Q_INTERFACES(OpenCOR::GuiInterface)
    Q_INTERFACES(OpenCOR::I18nInterface)

public:
    virtual void initialize();

protected:
    virtual void retranslateUi();

    virtual void updateActions();

private:
    QMenu *mEditMenu;

    QToolBar *mEditToolbar;
    QAction *mEditToolbarAction;

    QAction *mEditUndoAction;
    QAction *mEditRedoAction;
    QAction *mEditCutAction;
    QAction *mEditCopyAction;
    QAction *mEditPasteAction;
    QAction *mEditDeleteAction;
    QAction *mEditFindAction;
    QAction *mEditFindNextAction;
    QAction *mEditPreviousAction;
    QAction *mEditReplaceAction;
    QAction *mEditSelectAllAction;
};

} }

#endif

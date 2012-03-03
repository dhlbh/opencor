//==============================================================================
// File browser widget
//==============================================================================

#ifndef FILEBROWSERWIDGET_H
#define FILEBROWSERWIDGET_H

//==============================================================================

#include "treeview.h"

//==============================================================================

#include <QFileSystemModel>

//==============================================================================

namespace OpenCOR {
namespace FileBrowser {

//==============================================================================

class FileBrowserModel : public QFileSystemModel
{
    Q_OBJECT

public:
    explicit FileBrowserModel(QObject *pParent = 0);

    virtual Qt::ItemFlags flags(const QModelIndex &pIndex) const;
};

//==============================================================================

class FileBrowserWidget : public Core::TreeView
{
    Q_OBJECT

public:
    explicit FileBrowserWidget(const QString &pName, QWidget *pParent = 0);
    ~FileBrowserWidget();

    virtual void loadSettings(QSettings *pSettings);
    virtual void saveSettings(QSettings *pSettings) const;

    bool goToPath(const QString &pPath, const bool &pExpand = false);

    void goToHomeFolder(const bool &pExpand = false);
    void goToParentFolder();

    void goToPreviousFileOrFolder();
    void goToNextFileOrFolder();

    QString currentPath() const;
    QString currentPathDir() const;
    QString currentPathParent() const;

    QString pathOf(const QModelIndex &pIndex) const;

protected:
    virtual QSize sizeHint() const;

    virtual bool viewportEvent(QEvent *pEvent);

    virtual void keyPressEvent(QKeyEvent *pEvent);

    virtual void mousePressEvent(QMouseEvent *pEvent);
    virtual void mouseMoveEvent(QMouseEvent *pEvent);

private:
    FileBrowserModel *mDataModel;

    bool mNeedDefColWidth;

    QStringList mInitPathDirs;
    QString mInitPathDir;
    QString mInitPath;

    QStringList mPreviousItems;
    QStringList mNextItems;

    void deselectFolders() const;

    QStringList selectedFiles() const;

    void emitItemChangedRelatedSignals();

    void updateItems(const QString &pItemPath, QStringList &pItems) const;
    void goToOtherItem(QStringList &pItems, QStringList &pOtherItems);

Q_SIGNALS:
    void filesOpened(const QStringList &pFileNames);

    void notHomeFolder(const bool &pNotHomeFolder);
    void goToParentFolderEnabled(const bool &pEnabled);

    void goToPreviousFileOrFolderEnabled(const bool &pEnabled);
    void goToNextFileOrFolderEnabled(const bool &pEnabled);

private Q_SLOTS:
    void itemChanged(const QModelIndex &pCrtItem, const QModelIndex &pPrevItem);

    void directoryLoaded(const QString &pPath);
};

//==============================================================================

}   // namespace FileBrowser
}   // namespace OpenCOR

//==============================================================================

#endif

//==============================================================================
// End of file
//==============================================================================

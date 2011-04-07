#ifndef FILEORGANISERWIDGET_H
#define FILEORGANISERWIDGET_H

#include "treeview.h"

#include <QStandardItemModel>
#include <QString>

class QStandardItem;

static const QString FileOrganiserMimeType = "opencor/file-organiser";

class FileOrganiserItemModel : public QStandardItemModel
{
public:
    virtual QStringList mimeTypes() const;
};

class FileOrganiserWidget : public TreeView
{
    Q_OBJECT

public:
    explicit FileOrganiserWidget(const QString &pName, QWidget *pParent);

    virtual void loadSettings(QSettings &pSettings);
    virtual void saveSettings(QSettings &pSettings);

    bool isFolderItem(const QModelIndex &pItemIndex);

    bool newFolder();
    bool deleteItems();

    QString filePathOf(const QModelIndex &pFileIndex);

protected:
    virtual QSize sizeHint() const;

    virtual bool viewportEvent(QEvent *pEvent);

    virtual void dragEnterEvent(QDragEnterEvent *pEvent);
    virtual void dragMoveEvent(QDragMoveEvent *pEvent);
    virtual void dropEvent(QDropEvent *pEvent);

    virtual void keyPressEvent(QKeyEvent *pEvent);

private:
    FileOrganiserItemModel *mDataModel;

    void loadItemSettings(QSettings &pSettings, QStandardItem *pParentItem);
    void saveItemSettings(QSettings &pSettings, QStandardItem *pItem,
                          const int &pParentItemIndex);

    QString newFolderName(QStandardItem *pFolderItem);
    bool newFile(const QString &pFileName, QStandardItem *pParentItem);

    void resizeToContents();

    QStringList selectedFiles();

Q_SIGNALS:
    void filesOpened(const QStringList &pFileNames);

private slots:
    void expandedFolder(const QModelIndex &pFolderIndex);
    void collapsedFolder(const QModelIndex &pFolderIndex);
};

#endif

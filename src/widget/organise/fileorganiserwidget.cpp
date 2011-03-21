#include "fileorganiserwidget.h"

#include <QDir>
#include <QFileInfo>
#include <QHelpEvent>
#include <QSettings>
#include <QStandardItemModel>
#include <QUrl>

enum FileOrganiserItemRole {
    FileOrganiserItemFolder = Qt::UserRole,
    FileOrganiserItemPath = Qt::UserRole+1
};

FileOrganiserWidget::FileOrganiserWidget(const QString &pName,
                                         QWidget *pParent) :
    QTreeView(pParent),
    CommonWidget(pName, this, pParent)
{
    // Create an instance of the data model that we want to view

    mDataModel = new QStandardItemModel;

    // Set some properties for the file organiser widget itself

    setAcceptDrops(true);
#ifdef Q_WS_MAC
    setAttribute(Qt::WA_MacShowFocusRect, 0);
    // Note: the above removes the focus border since it messes up our toolbar
#endif
    setEditTriggers(QAbstractItemView::EditKeyPressed);
    setHeaderHidden(true);
    setModel(mDataModel);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setUniformRowHeights(true);

    // Some connections

    connect(this, SIGNAL(expanded(const QModelIndex &)),
            this, SLOT(resizeToContents()));
    connect(this, SIGNAL(collapsed(const QModelIndex &)),
            this, SLOT(resizeToContents()));
}

static const QString SettingsDataModel = "DataModel";

void FileOrganiserWidget::loadItemSettings(QSettings &pSettings,
                                           QStandardItem *pParentItem)
{
    // Recursively retrieve the item settings

    static int crtItemIndex = -1;
    QStringList itemInfo;

    itemInfo = pSettings.value(QString::number(++crtItemIndex)).toStringList();

    if (itemInfo != QStringList()) {
        QString textOrPath  = itemInfo.at(0);
        int parentItemIndex = itemInfo.at(1).toInt();
        int nbOfChildItems  = itemInfo.at(2).toInt();
        bool expanded       = itemInfo.at(3).toInt();

        // Create the item, in case we are not dealing with the root folder item

        QStandardItem *childParentItem;

        if (parentItemIndex == -1) {
            // We are dealing with the root folder item, so don't do anything
            // except for keeping track of it for when retrieving its child
            // items, if any

            childParentItem = mDataModel->invisibleRootItem();
        } else {
            // This is not the root folder item, so we can create the item which
            // is either a folder or a file, depending on its number of child
            // items

            if (nbOfChildItems >= 0) {
                // We are dealing with a folder item

                QStandardItem *folderItem = new QStandardItem(QIcon(":folder"),
                                                              textOrPath);

                folderItem->setData(true, FileOrganiserItemFolder);

                pParentItem->appendRow(folderItem);

                // Expand the folder item, if necessary

                if (expanded)
                    setExpanded(folderItem->index(), true);

                // The folder item is to be the parent of any of its child item

                childParentItem = folderItem;
            } else {
                // We are dealing with a file item

                QStandardItem *fileItem = new QStandardItem(QIcon(":file"),
                                                            QFileInfo(textOrPath).fileName());

                fileItem->setData(textOrPath, FileOrganiserItemPath);

                pParentItem->appendRow(fileItem);

                // A file cannot have child items, so...

                childParentItem = 0;
            }
        }

        // Retrieve any child item
        // Note: the test on childParentItem is not necessary (since
        //       nbOfChildItems will be equal to -1 in the case of a file item),
        //       but it doesn't harm having it, so...

        if (childParentItem)
            for (int i = 0; i < nbOfChildItems; ++i)
                loadItemSettings(pSettings, childParentItem);
    }
}

void FileOrganiserWidget::loadSettings(QSettings &pSettings)
{
    pSettings.beginGroup(objectName());
        // Retrieve the data model

        pSettings.beginGroup(SettingsDataModel);
            loadItemSettings(pSettings, 0);
        pSettings.endGroup();
    pSettings.endGroup();
}

void FileOrganiserWidget::saveItemSettings(QSettings &pSettings,
                                           QStandardItem *pItem,
                                           const int &pParentItemIndex)
{
    // Recursively keep track of the item settings

    static int crtItemIndex = -1;
    QStringList itemInfo;

    // The item information consists of:
    //  - The name of the folder item or the path of the file item
    //  - The index of its parent
    //  - The number of child items the (folder) item has, if any
    //  - Whether the (folder) items is expanded or not

    if (   (pItem == mDataModel->invisibleRootItem())
        || pItem->data(FileOrganiserItemFolder).toBool())
        // We are dealing with a folder item (be it the root folder item or not)

        itemInfo << pItem->text() << QString::number(pParentItemIndex)
                 << QString::number(pItem->rowCount())
                 << QString(isExpanded(pItem->index())?"1":"0");
    else
        // We are dealing with a file item

        itemInfo << pItem->data(FileOrganiserItemPath).toString()
                 << QString::number(pParentItemIndex) << "-1" << "0";

    pSettings.setValue(QString::number(++crtItemIndex), itemInfo);

    // Keep track of any child item

    int childParentItemIndex = crtItemIndex;

    for (int i = 0; i < pItem->rowCount(); ++i)
        saveItemSettings(pSettings, pItem->child(i), childParentItemIndex);
}

void FileOrganiserWidget::saveSettings(QSettings &pSettings)
{
    pSettings.beginGroup(objectName());
        // Keep track of the data model

        pSettings.remove(SettingsDataModel);
        pSettings.beginGroup(SettingsDataModel);
            saveItemSettings(pSettings, mDataModel->invisibleRootItem(), -1);
        pSettings.endGroup();
    pSettings.endGroup();
}

QSize FileOrganiserWidget::sizeHint() const
{
    // Suggest a default size for the file organiser widget
    // Note: this is critical if we want a docked widget, with a file organiser
    //       widget on it, to have a decent size when docked to the main window

    return defaultSize(0.15);
}

bool FileOrganiserWidget::viewportEvent(QEvent *pEvent)
{
    if (pEvent->type() == QEvent::ToolTip) {
        // We need to show a tool tip, so make sure that it's up to date by
        // setting it to the path of the current file item or to nothing if we
        // are dealing with a folder item

        QHelpEvent *helpEvent = static_cast<QHelpEvent *>(pEvent);
        QStandardItem *crtItem = mDataModel->itemFromIndex(indexAt(QPoint(helpEvent->x(),
                                                                          helpEvent->y())));

        if (crtItem)
            setToolTip(QDir::toNativeSeparators(crtItem->data(FileOrganiserItemFolder).toBool()?
                                                    "":
                                                    crtItem->data(FileOrganiserItemPath).toString()));
    }

    // Default handling of the event

    return QTreeView::viewportEvent(pEvent);
}

void FileOrganiserWidget::dragEnterEvent(QDragEnterEvent *pEvent)
{
    // Accept the proposed action for the event, but only if we are dropping
    // URLs

    if (pEvent->mimeData()->hasUrls())
        pEvent->acceptProposedAction();
    else
        pEvent->ignore();
}

void FileOrganiserWidget::dragMoveEvent(QDragMoveEvent *pEvent)
{
    // Accept the proposed action for the event

    pEvent->acceptProposedAction();
}

void FileOrganiserWidget::dropEvent(QDropEvent *pEvent)
{
    // Add the dropped documents to the folder corresponding to the mouse
    // position

    QModelIndex crtItemIndex = indexAt(QPoint(pEvent->pos().x(),
                                              pEvent->pos().y()));
    QStandardItem *crtItem = mDataModel->itemFromIndex(crtItemIndex);

    if (crtItem) {
        // The mouse position corresponds to an item, so retrieve its parent
        // (folder) item in case it's a file item

        if (!crtItem->data(FileOrganiserItemFolder).toBool())
            // The current item is a file item, so we must get its parent
            // (folder) item
            crtItem = mDataModel->itemFromIndex(crtItemIndex.parent());
    }

    if (!crtItem)
        // If we don't have a valid item by now, then it means that the item
        // must be the root (folder) item, so...

        crtItem = mDataModel->invisibleRootItem();

    // We are dropping the documents over a valid item, so add them to it

    QList<QUrl> urlList = pEvent->mimeData()->urls();

    for (int i = 0; i < urlList.count(); ++i)
        newFile(urlList.at(i).toLocalFile(), crtItem);

    // Accept the proposed action for the event

    pEvent->acceptProposedAction();
}

QString FileOrganiserWidget::newFolderName(QStandardItem *pFolderItem)
{
    // Come up with the name for a new folder which is to be under pFolderItem

    // Retrieve the name of the folders under pFolderItem

    QStringList subFolderNames;

    for (int i = 0; i < pFolderItem->rowCount(); ++i)
        subFolderNames.append(pFolderItem->child(i)->text());

    // Compare the suggested name of our new folder with that of the folders
    // under pFolderItem, this until we come up with a suggested name which is
    // not already taken

    static const QString baseFolderName = "New Folder";
    static const QString templateFolderName = baseFolderName+" (%1)";
    int folderNb = 1;
    QString folderName = baseFolderName;

    while (subFolderNames.contains(folderName))
        folderName = templateFolderName.arg(++folderNb);

    return folderName;
}

bool FileOrganiserWidget::newFolder()
{
    // Create a folder item under current folder item or root item, depending on
    // the situation

    QModelIndexList itemsList = selectionModel()->selectedIndexes();
    int nbOfSelectedItems = itemsList.count();

    if (nbOfSelectedItems <= 1) {
        // Either no or one folder item is currently selected, so create the new
        // folder item under the root item or the existing folder item, resp.

        QStandardItem *crtFolderItem = !nbOfSelectedItems?
                                           mDataModel->invisibleRootItem():
                                           mDataModel->itemFromIndex(itemsList.at(0));
        QStandardItem *newFolderItem = new QStandardItem(QIcon(":folder"),
                                                         newFolderName(crtFolderItem));

        newFolderItem->setData(true, FileOrganiserItemFolder);

        crtFolderItem->appendRow(newFolderItem);

        // Some post-processing, but only if no other item is currently being
        // edited

        if (state() != QAbstractItemView::EditingState) {
            // Go to the newly added folder

            setCurrentIndex(newFolderItem->index());

            // Offer the user to edit the newly added folder item

            edit(newFolderItem->index());
        }

        return true;
    } else {
        // Several folder items are selected, so...
        // Note: we should never come here (i.e. the caller to this function
        //       should ensure that a folder can be created before calling this
        //       function), but it's better to be safe than sorry

        return false;
    }
}

bool FileOrganiserWidget::newFile(const QString &pFileName,
                                  QStandardItem *pParentItem)
{
    // Add the file the passed parent item

    if (pParentItem) {
        // The passed parent item is valid, so add the file to it

        QStandardItem *newFileItem = new QStandardItem(QIcon(":file"),
                                                       QFileInfo(pFileName).fileName());

        newFileItem->setData(pFileName, FileOrganiserItemPath);

        pParentItem->appendRow(newFileItem);

        // Go to the newly added file

        setCurrentIndex(newFileItem->index());

        return true;
    } else {
        // The passed parent item is not valid, so...
        // Note: we should never come here (i.e. the caller to this function
        //       should ensure that the passed parent item is valid), but it's
        //       better to be safe than sorry

        return false;
    }
}

bool FileOrganiserWidget::deleteItems()
{
    // Remove all the selected items

    QModelIndexList itemsList = selectionModel()->selectedIndexes();

    if (itemsList.isEmpty()) {
        // Nothing to delete, so...
        // Note: we should never come here (i.e. the caller to this function
        //       should ensure that a folder can be created before calling this
        //       function), but it's better to be safe than sorry

        return false;
    } else {
        // There are some items to delete, so delete them one by one, making
        // sure we update our list of items (this is because the row value of
        // the remaining selected items may become different after deleting an
        // item)

        while(!itemsList.isEmpty()) {
            mDataModel->removeRow(itemsList.first().row(),
                                  itemsList.first().parent());

            itemsList = selectionModel()->selectedIndexes();
        }

        // Resize the widget to its contents in case its width was too wide (and
        // therefore required a horizontal scrollbar), but is now fine

        resizeToContents();

        return true;
    }
}

void FileOrganiserWidget::resizeToContents()
{
    // Make sure that the first column allows for all of its contents to be
    // visible

    resizeColumnToContents(0);
}

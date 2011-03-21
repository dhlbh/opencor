#include "docktoolbar.h"
#include "filebrowserwindow.h"
#include "filebrowserwidget.h"

#include "ui_filebrowserwindow.h"

#include <QFileInfo>
#include <QMenu>
#include <QSettings>

FileBrowserWindow::FileBrowserWindow(QWidget *pParent) :
    DockWidget(pParent),
    mUi(new Ui::FileBrowserWindow),
    mKeepTrackOfPrevItem(true)
{
    // Set up the UI

    mUi->setupUi(this);

    // Create a toolbar with different buttons
    // Note: this sadly can't be done using the design mode, so...

    DockToolBar *toolbar = new DockToolBar(this);

    toolbar->addAction(mUi->actionHome);
    toolbar->addSeparator();
    toolbar->addAction(mUi->actionParent);
    toolbar->addSeparator();
    toolbar->addAction(mUi->actionPrevious);
    toolbar->addAction(mUi->actionNext);

    mUi->verticalLayout->addWidget(toolbar);

    // Create and add the file browser widget

    mFileBrowserWidget = new FileBrowserWidget("FileBrowserWidget", this);

    mUi->verticalLayout->addWidget(mFileBrowserWidget);

    // We want our own context menu for the file organiser widget

    mFileBrowserWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    // Some connections

    connect(mFileBrowserWidget, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(customContextMenu(const QPoint &)));

    connect(mFileBrowserWidget->selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),
            this, SLOT(currentItemChanged(const QModelIndex &, const QModelIndex &)));
}

FileBrowserWindow::~FileBrowserWindow()
{
    // Delete the UI

    delete mUi;
}

void FileBrowserWindow::updateActions()
{
    // Make sure that the various actions are properly enabled/disabled

    static QString homeFolder = mFileBrowserWidget->homeFolder();

    mUi->actionHome->setEnabled(mFileBrowserWidget->currentPath() != homeFolder);

    mUi->actionParent->setEnabled(mFileBrowserWidget->currentPathParent() != "");

    mUi->actionPrevious->setEnabled(mPrevItems.count());
    mUi->actionNext->setEnabled(mNextItems.count());
}

void FileBrowserWindow::retranslateUi()
{
    // Translate the whole window

    mUi->retranslateUi(this);

    // Make sure that the enabled state of the various actions is correct
    // (indeed, to translate everything messes things up in that respect,
    // so...)

    updateActions();

    // Retranslate the file browser widget

    mFileBrowserWidget->retranslateUi();
}

void FileBrowserWindow::loadSettings(QSettings &pSettings)
{
    pSettings.beginGroup(objectName());
        // Retrieve the settings of the file browser widget
        // Note: we must make sure that we don't keep track of the previous item

        mKeepTrackOfPrevItem = false;

        mFileBrowserWidget->loadSettings(pSettings);

        mKeepTrackOfPrevItem = true;

        // Make sure that the current path is expanded
        // Note: this is important in case the current path is that of the C:
        //       drive or the root of the file system which, during the
        //       loadSettings above, won't trigger a directoryLoaded signal in
        //       the file browser widget

        if (!mFileBrowserWidget->isExpanded(mFileBrowserWidget->currentIndex()))
            mFileBrowserWidget->setExpanded(mFileBrowserWidget->currentIndex(),
                                            true);

        // Make sure that all the actions are up-to-date

        updateActions();
    pSettings.endGroup();
}

void FileBrowserWindow::saveSettings(QSettings &pSettings)
{
    pSettings.beginGroup(objectName());
        // Keep track of the settings of the file browser widget

        mFileBrowserWidget->saveSettings(pSettings);
    pSettings.endGroup();
}

void FileBrowserWindow::currentItemChanged(const QModelIndex &,
                                           const QModelIndex &pPrevItem)
{
    if (!mKeepTrackOfPrevItem)
        // We don't want to keep track of the previous item, so...

        return;

    // A new item has been selected, so we need to keep track of the old one in
    // case we want to go back to it

    mPrevItems.append(mFileBrowserWidget->pathOf(pPrevItem));

    // Reset the list of next items since that list doesn't make sense anymore

    mNextItems.clear();

    // Since a new item has been selected, we must update the actions

    updateActions();
}

void FileBrowserWindow::on_actionHome_triggered()
{
    // Go to the home folder (and ask for it to be expanded)

    mFileBrowserWidget->gotoHomeFolder(true);
}

void FileBrowserWindow::on_actionParent_triggered()
{
    // Go to the parent item

    mFileBrowserWidget->gotoPath(mFileBrowserWidget->currentPathParent());

    // Going to the parent item may have changed some actions, so...

    updateActions();
}

void FileBrowserWindow::gotoOtherItem(QStringList &pItems,
                                      QStringList &pOtherItems)
{
    // Go to the previous/next item and move the last item from our list of
    // items to our list of other items
    // Note: we must make sure that we don't keep track of the previous item
    //       when doing this

    mKeepTrackOfPrevItem = false;
        QString crtPath = mFileBrowserWidget->currentPath();

        pOtherItems.append(crtPath);

        QString newItemPath = pItems.last();

        while (   !pItems.isEmpty()
               && !QFileInfo(newItemPath).exists()) {
            // The new item doesn't exist anymore, so remove it from our list of
            // items and other items

            updateItems(newItemPath, pItems);
            updateItems(newItemPath, pOtherItems);

            // Try with the next new item

            if (!pItems.isEmpty()) {
                // The list is not empty, so make the last item our next new
                // item

                newItemPath = pItems.last();

                // The next new item cannot, however, be the same as the current
                // path

                while (!pItems.isEmpty() && (newItemPath == crtPath)) {
                    pItems.removeLast();

                    if (!pItems.isEmpty())
                        newItemPath = pItems.last();
                    else
                        newItemPath = "";
                }
            } else {
                // The list is empty, so...

                newItemPath = "";
            }
        }

        if (!newItemPath.isEmpty()) {
            // We have a valid new item, so go to its path and remove it from
            // our list of items

            mFileBrowserWidget->gotoPath(newItemPath);

            pItems.removeLast();
        }

        // Make sure that the last item in the lists of items and other items
        // isn't that of the current path (this may happen if some items got
        // deleted by the user)

        crtPath = mFileBrowserWidget->currentPath();

        if (!pItems.isEmpty() && (pItems.last() == crtPath))
            pItems.removeLast();

        if (!pOtherItems.isEmpty() && (pOtherItems.last() == crtPath))
            pOtherItems.removeLast();
    mKeepTrackOfPrevItem = true;

    // Going to the previous item may have changed some actions, so...

    updateActions();
}

void FileBrowserWindow::on_actionPrevious_triggered()
{
    gotoOtherItem(mPrevItems, mNextItems);
}

void FileBrowserWindow::on_actionNext_triggered()
{
    gotoOtherItem(mNextItems, mPrevItems);
}

void FileBrowserWindow::updateItems(const QString &pItemPath,
                                    QStringList &pItems)
{
    // Remove any instance of pItemPath in pItems

    pItems.removeAll(pItemPath);

    // Because of the above, we may have two or more consective identital items
    // in the list, so we must reduce that to one

    if (pItems.count() > 1) {
        QStringList newItems;
        QString prevItem = pItems.at(0);

        newItems.append(prevItem);

        for (int i = 1; i < pItems.count(); ++i) {
            QString crtItem = pItems.at(i);

            if (crtItem != prevItem) {
                // The current and previous items are different, so we want to
                // keep track of it and add it to our new list

                newItems.append(crtItem);

                prevItem = crtItem;
            }
        }

        // Update the old list of items with our new one

        pItems = newItems;
    }
}

void FileBrowserWindow::customContextMenu(const QPoint &)
{
    // Create a custom context menu which items match the contents of our
    // toolbar

    QMenu menu;

    menu.addAction(mUi->actionHome);

    menu.exec(QCursor::pos());
}

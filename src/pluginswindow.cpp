#include "mainwindow.h"
#include "plugin.h"
#include "pluginmanager.h"
#include "pluginswindow.h"

#include "ui_pluginswindow.h"

#include <QDesktopServices>
#include <QMessageBox>
#include <QPushButton>
#include <QUrl>

namespace OpenCOR {

PluginDelegate::PluginDelegate(QStandardItemModel *pDataModel,
                               QObject *pParent) :
    QStyledItemDelegate(pParent),
    mDataModel(pDataModel)
{
}

void PluginDelegate::paint(QPainter *pPainter,
                           const QStyleOptionViewItem &pOption,
                           const QModelIndex &pIndex) const
{
    // Paint the item as normal, except for the items which are not checkable
    // (i.e. plugins which the user cannot decide whether to load) in which case
    // we paint them as if they were disabled

    QStandardItem *pluginItem = mDataModel->itemFromIndex(pIndex);

    QStyleOptionViewItemV4 option(pOption);

    initStyleOption(&option, pIndex);

    if (!pluginItem->isCheckable())
        option.state &= ~QStyle::State_Enabled;

    QStyledItemDelegate::paint(pPainter, option, pIndex);
}

PluginsWindow::PluginsWindow(PluginManager *pPluginManager,
                             QWidget *pParent) :
    QDialog(pParent),
    mUi(new Ui::PluginsWindow),
    mPluginManager(pPluginManager),
    mMainWindow(qobject_cast<MainWindow *>(pParent))
{
    // Set up the UI

    mUi->setupUi(this);

    // Update the note label

    mUi->noteLabel->setText(mUi->noteLabel->text().arg(qApp->applicationName()));

    // Set up the tree view with a delegate, so that we can select plugins that
    // are shown as 'disabled' (to reflect the fact that users cannot decide
    // whether they should be loaded)

    mDataModel = new QStandardItemModel;
    mPluginDelegate = new PluginDelegate(mDataModel);

    mUi->treeView->setModel(mDataModel);
    mUi->treeView->setItemDelegate(mPluginDelegate);

    // Populate the data model

QStandardItem *groupItem = new QStandardItem("General");

mDataModel->invisibleRootItem()->appendRow(groupItem);

    foreach (Plugin *plugin, mPluginManager->plugins()) {
        QStandardItem *pluginItem = new QStandardItem((plugin->status() == Plugin::Loaded)?
                                                          QIcon(":oxygen/actions/dialog-ok-apply.png"):
                                                          QIcon(":oxygen/actions/edit-delete.png"),
                                                      plugin->name());

        // Only manageable plugins and plugins that are of the right type are
        // checkable

        pluginItem->setCheckable(   plugin->info().manageable()
                                 && (plugin->info().type() != PluginInfo::Console));

        if (pluginItem->isCheckable()) {
            // Retrieve the loading state of the plugin

            pluginItem->setCheckState((Plugin::load(mPluginManager->settings(),
                                                    plugin->name()))?
                                          Qt::Checked:
                                          Qt::Unchecked);

            // We are dealing with a manageable plugin, so add it to our list of
            // manageable plugins

            mManageablePlugins << pluginItem;
        } else {
            // We are dealing with an unmanageable plugin, so add it to our list
            // of unmanageable plugins

            mUnmanageablePlugins << pluginItem;
        }

        // Add the plugin to our data model

//        mDataModel->invisibleRootItem()->appendRow(pluginItem);
groupItem->appendRow(pluginItem);
    }

    // Make sure that the loading state of all the plugins is right, including
    // that of the plugins which the user cannot manage

    updatePluginsLoadingState(0, true);

    // Select the first plugin

    mUi->treeView->selectionModel()->select(mDataModel->index(0, 0),
                                            QItemSelectionModel::Select);

    // Expand the whole tree view and make sure that the it only takes as much
    // width as necessary
    // Note: for some reasons (maybe because we have check boxes?), the
    //       retrieved column size gives us a width that is slightly too small
    //       and therefore requires a horizontal scroll bar, hence we add 15% to
    //       it (the extra 15% seems to be enough even to even account for a big
    //       number of plugins which would then require a vertical scroll bar)

    mUi->treeView->expandAll();
    mUi->treeView->resizeColumnToContents(0);

    mUi->treeView->setMinimumWidth(1.15*mUi->treeView->columnWidth(0));
    mUi->treeView->setMaximumWidth(mUi->treeView->minimumWidth());

    // Make, through the note label, sure that the window has a minimum width

    mUi->noteLabel->setMinimumWidth(2.5*mUi->treeView->minimumWidth());

    // Connection to handle a plugin's information

    connect(mUi->treeView->selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),
            this, SLOT(updatePluginInfo(const QModelIndex &, const QModelIndex &)));

    // Connection to handle the activation of a link in the description

    connect(mUi->descriptionValue, SIGNAL(linkActivated(const QString &)),
            this, SLOT(openLink(const QString &)));

    // Connection to handle the window's buttons

    connect(mUi->buttonBox->button(QDialogButtonBox::Apply), SIGNAL(clicked(bool)),
            this, SLOT(apply()));

    // Make sure that the window has a reasonable starting size

    mUi->verticalLayout->setSizeConstraint(QLayout::SetMinimumSize);
}

PluginsWindow::~PluginsWindow()
{
    // Delete some internal objects

    delete mDataModel;
    delete mPluginDelegate;
    delete mUi;
}

void PluginsWindow::retranslateUi()
{
    // Retranslate the whole window

    mUi->retranslateUi(this);
}

void PluginsWindow::updatePluginInfo(const QModelIndex &pNewIndex,
                                     const QModelIndex &) const
{
    // Check whether we are really dealing with a plugin item or whether we are
    // dealing with a group item

    QStandardItem *pluginItem = mDataModel->itemFromIndex(pNewIndex);

    if (!pluginItem->parent()) {
        // This is not a plugin item, but a group item, so hide it and leave...

        mUi->detailsWidget->setVisible(false);

        return;
    } else {
        // This is a plugin item, so make sure that the details widget is
        // visible

        mUi->detailsWidget->setVisible(true);
    }

    // Update the information view with the plugin's information

    QString pluginName = pluginItem->text();
    Plugin *plugin = mPluginManager->plugin(pluginName);
    PluginInfo pluginInfo = plugin->info();

    // The plugin's name

    mUi->nameValue->setText(pluginName);

    // The plugin's type

    switch (pluginInfo.type()) {
    case PluginInfo::General:
        mUi->typeValue->setText(tr("General"));

        break;
    case PluginInfo::Console:
        mUi->typeValue->setText(tr("Console"));

        break;
    case PluginInfo::Gui:
        mUi->typeValue->setText(tr("GUI"));

        break;
    default:
        mUi->typeValue->setText(tr("Undefined"));

        break;
    }

    // The plugin's dependencies

    QStringList dependencies = pluginInfo.dependencies();

    if (dependencies.isEmpty())
        dependencies << tr("None");

    if (dependencies.count() > 1)
        mUi->dependenciesValue->setText("- "+dependencies.join("\n- "));
    else
        mUi->dependenciesValue->setText(dependencies.first());

    // The plugin's description

    QString description = pluginInfo.description(mMainWindow->locale());

    mUi->descriptionValue->setText(description.isEmpty()?
                                       tr("None"):
                                       description);

    // The plugin's status

    mUi->statusValue->setText(plugin->statusDescription());
}

void PluginsWindow::updatePluginsLoadingState(QStandardItem *pChangedPluginItem,
                                              const bool &pInitializing)
{
    // Disable the connection that handles a change in a plugin's loading state
    // (otherwise what we are doing here is going to be completely uneffective)

    disconnect(mDataModel, SIGNAL(itemChanged(QStandardItem *)),
               this, SLOT(updatePluginsLoadingState(QStandardItem *)));

    // Prevent the list view from being updated, since we may end up changing
    // quite a bit of its visual contents

    mUi->treeView->setUpdatesEnabled(false);

    // Check whether we came here as a result of checking a plugin and, if so,
    // then make sure that all of that plugin's dependencies are also checked

    if (   pChangedPluginItem
        && (pChangedPluginItem->checkState() == Qt::Checked))
        foreach (const QString &requiredPlugin,
                 mPluginManager->plugin(pChangedPluginItem->text())->info().fullDependencies())
            foreach (QStandardItem *pluginItem, mManageablePlugins)
                if (!pluginItem->text().compare(requiredPlugin))
                    // We are dealing with one of the plugin's dependencies, so
                    // make sure it's checked

                    pluginItem->setCheckState(Qt::Checked);

    // At this stage, all the plugins which should be checked are checked, so
    // now we must update the manageable plugins that are currently checked to
    // make sure that they should still be checked indeed. This means going
    // through each of the plugins and keep them checked only if all of their
    // dependencies are checked. Note that it is fine to do it this way since
    // all we need is one plugin's dependency to be unchecked for the plugin
    // itself to also be unchecked, so...

    foreach (QStandardItem *pluginItem, mManageablePlugins)
        if (pluginItem->checkState() == Qt::Checked)
            foreach (const QString &requiredPlugin,
                     mPluginManager->plugin(pluginItem->text())->info().fullDependencies())
                foreach (QStandardItem *otherPluginItem, mManageablePlugins)
                    if (!otherPluginItem->text().compare(requiredPlugin)) {
                        // We have found the plugin's dependency

                        if (otherPluginItem->checkState() == Qt::Unchecked)
                            // The plugin's dependency is unchecked which means
                            // that the plugin cannot be checked, so...

                            pluginItem->setCheckState(Qt::Unchecked);

                        break;
                    }

    // Finally, we need to see whether our unmanageable plugins should be
    // checked or unchecked

    foreach (QStandardItem *pluginItem, mUnmanageablePlugins) {
        // First, reset the loading state of the unamanageable plugin

        pluginItem->setCheckState(Qt::Unchecked);

        // Next, go through the checked manageable plugins' dependencies

        foreach (QStandardItem *otherPluginItem, mManageablePlugins)
            if (otherPluginItem->checkState() == Qt::Checked)
                // The manageable plugin is checked, so carry on...

                foreach (const QString &requiredPlugin,
                         mPluginManager->plugin(otherPluginItem->text())->info().fullDependencies())
                    if (!requiredPlugin.compare(pluginItem->text())) {
                        // The manageable plugin does require the unamanageable
                        // plugin, so...

                        pluginItem->setCheckState(Qt::Checked);

                        break;
                    }
    }

    // Re-enable the updating of the list view

    mUi->treeView->setUpdatesEnabled(true);

    // Check whether the OK and apply buttons should be enabled

    if (pInitializing)
        // We are initialising the plugins window, so retrieve the initial
        // loading state of the plugins

        foreach (QStandardItem *pluginItem,
                 mManageablePlugins+mUnmanageablePlugins)
            mInitialLoadingStates.insert(pluginItem->text(),
                                         pluginItem->checkState() == Qt::Checked);

    bool buttonsEnabled = false;

    foreach (QStandardItem *pluginItem, mManageablePlugins+mUnmanageablePlugins)
        if (   mInitialLoadingStates.value(pluginItem->text())
            != (pluginItem->checkState() == Qt::Checked)) {
            // The loading state of the plugin has changed, so...

            buttonsEnabled = true;

            break;
        }

    mUi->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(buttonsEnabled);
    mUi->buttonBox->button(QDialogButtonBox::Apply)->setEnabled(buttonsEnabled);

    // Re-enable the connection that handles a change in a plugin's loading
    // state

    connect(mDataModel, SIGNAL(itemChanged(QStandardItem *)),
            this, SLOT(updatePluginsLoadingState(QStandardItem *)));
}

void PluginsWindow::openLink(const QString &pLink) const
{
    // Open the link in the user's browser

    QDesktopServices::openUrl(QUrl(pLink));
}

void PluginsWindow::on_buttonBox_accepted()
{
    // Keep track of the loading state of the various plugins over which the
    // user has control (i.e. the ones that are checkable)

    foreach (QStandardItem *pluginItem, mManageablePlugins)
        if (pluginItem->isCheckable())
            Plugin::setLoad(mPluginManager->settings(), pluginItem->text(),
                            pluginItem->checkState() == Qt::Checked);

    // Confirm that we accepted the changes

    accept();
}

void PluginsWindow::on_buttonBox_rejected()
{
    // Simple cancel whatever was done here

    reject();
}

void PluginsWindow::apply()
{
    if( QMessageBox::question(this, qApp->applicationName(),
                              tr("%1 must be restarted for your changes to be effective. Are you sure that this is what you want?").arg(qApp->applicationName()),
                              QMessageBox::Yes|QMessageBox::No,
                              QMessageBox::Yes) == QMessageBox::Yes ) {
        // Do what is done when clicking on the OK button

        on_buttonBox_accepted();

        // Exit OpenCOR with the request to restart it after having saved its settings

        mMainWindow->restart(true);
    }
}

}

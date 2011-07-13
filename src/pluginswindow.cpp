#include "mainwindow.h"
#include "plugin.h"
#include "pluginmanager.h"
#include "pluginswindow.h"

#include "ui_pluginswindow.h"

#include <QDesktopServices>
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
        option.state ^= QStyle::State_Enabled;

    QStyledItemDelegate::paint(pPainter, option, pIndex);
}

PluginsWindow::PluginsWindow(PluginManager *pPluginManager,
                             QWidget *pParent) :
    QDialog(pParent),
    mUi(new Ui::PluginsWindow),
    mPluginManager(pPluginManager)
{
    // Set up the UI

    mUi->setupUi(this);

    // Set up the tree view with a delegate, so that we can select plugins that
    // are shown as 'disabled' (to reflect the fact that users cannot decide
    // whether they should be loaded)

    mDataModel = new QStandardItemModel;
    mPluginDelegate = new PluginDelegate(mDataModel);

    mUi->listView->setModel(mDataModel);
    mUi->listView->setItemDelegate(mPluginDelegate);

    // Populate the data model

    foreach(Plugin *plugin, mPluginManager->plugins()) {
        QStandardItem *pluginItem = new QStandardItem(plugin->name());

        // Only plugins that have dependencies are checkable

        pluginItem->setCheckable(plugin->info().dependencies().count());

        // A plugin should be shown as checked (i.e. to be loaded at startup)
        // if it is a required plugin or a plugin which is explicitly required
        // to be loaded

        if (pluginItem->isCheckable())
            // We are dealing with a plugin which has dependencies and may
            // therefore be explicitly required to be loaded, so...

            pluginItem->setCheckState((Plugin::load(mPluginManager->settings(),
                                                    plugin->name()))?
                                          Qt::Checked:
                                          Qt::Unchecked);
        else
            // We are dealing with a plugin that has no dependencies, so show it
            // checked only if it is actually loaded

            pluginItem->setCheckState((plugin->status() == Plugin::Loaded)?
                                          Qt::Checked:
                                          Qt::Unchecked);

        // Add the plugin to our data model

        mDataModel->invisibleRootItem()->appendRow(pluginItem);
    }

    // Make sure that the list view only takes as much space as necessary
    // Note: for some reasons (maybe because we have check boxes?), the
    //       retrieved column size gives us a width that is slightly too small
    //       and therefore requires a horizontal scroll bar, hence we add 15% to
    //       it (the extra 15% seems to be enough to even account for a big
    //       number of plugins which would then require a vertical scroll bar)

    mUi->listView->setMinimumWidth(1.15*mUi->listView->sizeHintForColumn(0));
    mUi->listView->setMaximumWidth(mUi->listView->minimumWidth());

    // Connection to handle a plugin's information

    connect(mUi->listView->selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),
            this, SLOT(updatePluginInfo(const QModelIndex &, const QModelIndex &)));

    // Connection to handle the activation of a link in the description

    connect(mUi->descriptionValue, SIGNAL(linkActivated(const QString &)),
            this, SLOT(openLink(const QString &)));

    // Make sure that the window has a reasonable starting size

    layout()->setSizeConstraint(QLayout::SetMinimumSize);
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
    // Translate the whole window

    mUi->retranslateUi(this);
}

void PluginsWindow::updatePluginInfo(const QModelIndex &pNewIndex,
                                     const QModelIndex &)
{
    // Update the information view with the plugin's information

    QString pluginName = mDataModel->itemFromIndex(pNewIndex)->text();
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

    QString description = pluginInfo.description(qobject_cast<MainWindow *>(parent())->locale());

    mUi->descriptionValue->setText(description.isEmpty()?
                                       tr("None"):
                                       description);

    // The plugin's status

    mUi->statusValue->setText(plugin->statusDescription());
}

void PluginsWindow::openLink(const QString &pLink)
{
    // Open the link in the user's browser

    QDesktopServices::openUrl(QUrl(pLink));
}

void OpenCOR::PluginsWindow::on_buttonBox_accepted()
{
    // Take into account the user's changes, if any, to which plugin should be
    // loaded

//---GRY--- TO BE DONE...

    accept();
}

void OpenCOR::PluginsWindow::on_buttonBox_rejected()
{
    // Simple cancel whatever was done here

    reject();
}

}

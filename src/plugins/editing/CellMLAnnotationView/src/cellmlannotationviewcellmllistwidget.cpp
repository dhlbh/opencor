//==============================================================================
// CellML annotation view CellML list widget
//==============================================================================

#include "borderedwidget.h"
#include "cellmlannotationviewcellmldetailswidget.h"
#include "cellmlannotationviewcellmlelementitem.h"
#include "cellmlannotationviewcellmlelementitemdelegate.h"
#include "cellmlannotationviewcellmllistwidget.h"
#include "cellmlannotationviewdetailswidget.h"
#include "cellmlannotationviewwidget.h"
#include "treeview.h"

//==============================================================================

#include "ui_cellmlannotationviewcellmllistwidget.h"

//==============================================================================

#include <QMenu>
#include <QStandardItemModel>

//==============================================================================

namespace OpenCOR {
namespace CellMLAnnotationView {

//==============================================================================

CellmlAnnotationViewCellmlListWidget::CellmlAnnotationViewCellmlListWidget(CellmlAnnotationViewWidget *pParent) :
    Widget(pParent),
    mParent(pParent),
    mGui(new Ui::CellmlAnnotationViewCellmlListWidget),
    mIndexes(QList<QModelIndex>())
{
    // Set up the GUI

    mGui->setupUi(this);

    // Create and customise our tree view which will contain all of the imports,
    // units, components, groups and connections from a CellML file

    mTreeView     = new Core::TreeView(pParent);
    mDataModel    = new QStandardItemModel(mTreeView);
    mItemDelegate = new CellmlElementItemDelegate(mTreeView, mDataModel);

    mTreeView->setModel(mDataModel);
    mTreeView->setItemDelegate(mItemDelegate);

    mTreeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mTreeView->setFrameShape(QFrame::NoFrame);
    mTreeView->setHeaderHidden(true);
    mTreeView->setRootIsDecorated(false);
    mTreeView->setSelectionMode(QAbstractItemView::SingleSelection);
    // Note: the selection mode we are opting for means that there is always a
    //       node which is selected, so that's something we must keep in mind
    //       when showing the context menu...

    // Populate ourselves

    mGui->layout->addWidget(mTreeView);

    // We want a context menu for our tree view

    mTreeView->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(mTreeView, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(treeViewContextMenu(const QPoint &)));

    // Set an event filter for our tree view

    mTreeView->installEventFilter(this);

    // Some connections to handle the expansion/collapse of a node

    connect(mTreeView, SIGNAL(expanded(const QModelIndex &)),
            this, SLOT(resizeTreeViewToContents()));
    connect(mTreeView, SIGNAL(collapsed(const QModelIndex &)),
            this, SLOT(resizeTreeViewToContents()));

    // Some connections to handle the change of node

    connect(mTreeView->selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),
            this, SLOT(updateNode(const QModelIndex &, const QModelIndex &)));

    // Populate our tree view

    populateDataModel();

    // Make our tree view our focus proxy

    setFocusProxy(mTreeView);

    // Expand our tree view enough so that we can see the meaningful parts of
    // the CellML file

    mTreeView->expandToDepth(1);

    // Resize our tree view, just to be on the safe side

    resizeTreeViewToContents();
}

//==============================================================================

CellmlAnnotationViewCellmlListWidget::~CellmlAnnotationViewCellmlListWidget()
{
    // Delete the GUI

    delete mGui;
}

//==============================================================================

void CellmlAnnotationViewCellmlListWidget::retranslateUi()
{
    // Retranslate our GUI

    mGui->retranslateUi(this);

    // Retranslate some of the nodes in our tree view

    retranslateDataItem(static_cast<CellmlElementItem *>(mDataModel->invisibleRootItem()));
}

//==============================================================================

void CellmlAnnotationViewCellmlListWidget::retranslateDataItem(CellmlElementItem *pCellmlElementItem)
{
    // Retranslate some of the node's children

    for (int i = 0, iMax = pCellmlElementItem->rowCount(); i < iMax; ++i)
        retranslateDataItem(static_cast<CellmlElementItem *>(pCellmlElementItem->child(i)));

    // Check whether we are dealing with a category or not

    if (pCellmlElementItem->isCategory())
        // We are dealing with a category, so retranslate its type

        switch (pCellmlElementItem->type()) {
        case CellmlElementItem::Import:
            pCellmlElementItem->setText(tr("Imports"));

            break;
        case CellmlElementItem::Unit:
            pCellmlElementItem->setText(tr("Units"));

            break;
        case CellmlElementItem::Component:
            pCellmlElementItem->setText(tr("Components"));

            break;
        case CellmlElementItem::Variable:
            pCellmlElementItem->setText(tr("Variables"));

            break;
        case CellmlElementItem::Group:
            pCellmlElementItem->setText(tr("Groups"));

            break;
        case CellmlElementItem::RelationshipReference:
            pCellmlElementItem->setText(tr("Relationship References"));

            break;
        case CellmlElementItem::ComponentReference:
            pCellmlElementItem->setText(tr("Component References"));

            break;
        case CellmlElementItem::Connection:
            pCellmlElementItem->setText(tr("Connections"));

            break;
        default:
            // Not a type we can retranslate, so do nothing...

            ;
        }
    else
        // We are not dealing with a category, so check the type and see whether
        // a node needs retranslating

        switch (pCellmlElementItem->type()) {
        case CellmlElementItem::Group:
            pCellmlElementItem->setText(tr("Group #%1").arg(pCellmlElementItem->number()));

            break;
        case CellmlElementItem::Connection:
            pCellmlElementItem->setText(tr("Connection #%1").arg(pCellmlElementItem->number()));

            break;
        default:
            // Not a sub-type we can retranslate, so do nothing...

            ;
        }
}

//==============================================================================

bool CellmlAnnotationViewCellmlListWidget::eventFilter(QObject *pObject,
                                                       QEvent *pEvent)
{
    Q_UNUSED(pObject);

    switch (pEvent->type()) {
    case QEvent::FocusIn: {
        // Note: to override focusInEvent() is not what we want since it won't
        //       be triggered when selecting a node in the CellML list after
        //       having selecting a node in the metadata list. Indeed, it's
        //       already 'in focus', so...

        updateNode(mTreeView->currentIndex(), QModelIndex());

        return true;
    }
    default:
        // Another type of event which we don't handle ourselves, so...

        return QWidget::eventFilter(pObject, pEvent);
    }
}

//==============================================================================

void CellmlAnnotationViewCellmlListWidget::populateDataModel()
{
    // Make sure that the CellML file was properly loaded

    if (mParent->cellmlFile()->issues().count()) {
        // Something went wrong while trying to load the CellML file, so report
        // it and leave

        mDataModel->invisibleRootItem()->appendRow(new CellmlElementItem(mParent->cellmlFile()->issues().first().type() == CellMLSupport::CellmlFileIssue::Error,
                                                                         mParent->cellmlFile()->issues().first().formattedMessage()));

        return;
    }

    // Retrieve the model's root

    CellmlElementItem *modelItem = new CellmlElementItem(CellmlElementItem::Model,
                                                         mParent->cellmlFile()->model());

    mDataModel->invisibleRootItem()->appendRow(modelItem);

    // Retrieve the model's imports

    if (mParent->cellmlFile()->imports()->count()) {
        // Imports category

        CellmlElementItem *importsItem = new CellmlElementItem(CellmlElementItem::Import,
                                                               tr("Imports"));

        modelItem->appendRow(importsItem);

        // Retrieve the model's imports themselves

        foreach (CellMLSupport::CellmlFileImport *import,
                 *mParent->cellmlFile()->imports()) {
            // A model import

            CellmlElementItem *importItem = new CellmlElementItem(CellmlElementItem::Import,
                                                                  import);

            importsItem->appendRow(importItem);

            // Retrieve the model's import's units

            if (import->units().count()) {
                // Units category

                CellmlElementItem *unitsItem = new CellmlElementItem(CellmlElementItem::ImportUnit,
                                                                     tr("Units"));

                importItem->appendRow(unitsItem);

                // Retrieve the model's import's units themselves

                foreach (CellMLSupport::CellmlFileImportUnit *unit,
                         import->units())
                    // A model's import's unit

                    unitsItem->appendRow(new CellmlElementItem(CellmlElementItem::ImportUnit,
                                                               unit));
            }

            // Retrieve the model's import's components

            if (import->components().count()) {
                // Components category

                CellmlElementItem *componentsItem = new CellmlElementItem(CellmlElementItem::ImportComponent,
                                                                          tr("Components"));

                importItem->appendRow(componentsItem);

                // Retrieve the model's import's components themselves

                foreach (CellMLSupport::CellmlFileImportComponent *component,
                         import->components())
                    // A model's import's component

                    componentsItem->appendRow(new CellmlElementItem(CellmlElementItem::ImportComponent,
                                                                    component));
            }
        }
    }

    // Retrieve the model's units

    populateUnitsDataModel(modelItem, mParent->cellmlFile()->units());

    // Retrieve the model's components

    if (mParent->cellmlFile()->components()->count()) {
        // Components category

        CellmlElementItem *componentsItem = new CellmlElementItem(CellmlElementItem::Component,
                                                                  tr("Components"));

        modelItem->appendRow(componentsItem);

        // Retrieve the model's components themselves

        foreach (CellMLSupport::CellmlFileComponent *component,
                 *mParent->cellmlFile()->components()) {
            // A model's component

            CellmlElementItem *componentItem = new CellmlElementItem(CellmlElementItem::Component,
                                                                     component);

            componentsItem->appendRow(componentItem);

            // Retrieve the model's component's units

            populateUnitsDataModel(componentItem, component->units());

            // Retrieve the model's component's variables

            if (component->variables()->count()) {
                // Variables category

                CellmlElementItem *variablesItem = new CellmlElementItem(CellmlElementItem::Variable,
                                                                         tr("Variables"));

                componentItem->appendRow(variablesItem);

                // Retrieve the model's component's variables themselves

                foreach (CellMLSupport::CellmlFileVariable *variable,
                         *component->variables())
                    variablesItem->appendRow(new CellmlElementItem(CellmlElementItem::Variable,
                                                                   variable));
            }
        }
    }

    // Retrieve the model's groups

    if (mParent->cellmlFile()->groups()->count()) {
        // Groups category

        CellmlElementItem *groupsItem = new CellmlElementItem(CellmlElementItem::Group,
                                                              tr("Groups"));

        modelItem->appendRow(groupsItem);

        // Retrieve the model's groups themselves

        int counter = 0;

        foreach (CellMLSupport::CellmlFileGroup *group,
                 *mParent->cellmlFile()->groups()) {
            // A model's group

            CellmlElementItem *groupItem = new CellmlElementItem(CellmlElementItem::Group,
                                                                 group,
                                                                 ++counter);

            groupsItem->appendRow(groupItem);

            // Retrieve the model's group's relationship references

            if (group->relationshipReferences().count()) {
                // Relationship references category

                CellmlElementItem *relationshipReferencesItem = new CellmlElementItem(CellmlElementItem::RelationshipReference,
                                                                                      tr("Relationship References"));

                groupItem->appendRow(relationshipReferencesItem);

                // Retrieve the model's group's relationship references
                // themselves

                foreach (CellMLSupport::CellmlFileRelationshipReference *relationshipReference,
                         group->relationshipReferences())
                    relationshipReferencesItem->appendRow(new CellmlElementItem(CellmlElementItem::RelationshipReference,
                                                                                relationshipReference));
            }

            // Retrieve the model's group's component references

            if (group->componentReferences().count()) {
                // Component references category

                CellmlElementItem *componentReferencesItem = new CellmlElementItem(CellmlElementItem::ComponentReference,
                                                                                   tr("Component References"));

                groupItem->appendRow(componentReferencesItem);

                // Retrieve the model's group's relationship references
                // themselves

                foreach (CellMLSupport::CellmlFileComponentReference *componentReference,
                         group->componentReferences())
                    populateComponentReferenceDataModel(componentReferencesItem,
                                                        componentReference);
            }
        }
    }

    // Retrieve the model's connections

    if (mParent->cellmlFile()->connections()->count()) {
        // Connections category

        CellmlElementItem *connectionsItem = new CellmlElementItem(CellmlElementItem::Connection,
                                                                   tr("Connections"));

        modelItem->appendRow(connectionsItem);

        // Retrieve the model's connections themselves

        int counter = 0;

        foreach (CellMLSupport::CellmlFileConnection *connection,
                 *mParent->cellmlFile()->connections()) {
            // A model's connection

            CellmlElementItem *connectionItem = new CellmlElementItem(CellmlElementItem::Connection,
                                                                      connection,
                                                                      ++counter);

            connectionsItem->appendRow(connectionItem);

            // Component mapping

            connectionItem->appendRow(new CellmlElementItem(CellmlElementItem::ComponentMapping,
                                                            connection->componentMapping()));

            // Variable mappings

            foreach (CellMLSupport::CellmlFileMapVariablesItem *mapVariablesItem,
                     connection->variableMappings())
                connectionItem->appendRow(new CellmlElementItem(CellmlElementItem::VariableMapping,
                                                                mapVariablesItem));
        }
    }
}

//==============================================================================

void CellmlAnnotationViewCellmlListWidget::populateUnitsDataModel(CellmlElementItem *pCellmlElementItem,
                                                                  CellMLSupport::CellmlFileUnits *pUnits)
{
    // Retrieve the units

    if (pUnits->count()) {
        // Units category

        CellmlElementItem *unitsItem = new CellmlElementItem(CellmlElementItem::Unit,
                                                             tr("Units"));

        pCellmlElementItem->appendRow(unitsItem);

        // Retrieve the units themselves

        foreach (CellMLSupport::CellmlFileUnit *unit, *pUnits) {
            CellmlElementItem *unitItem = new CellmlElementItem(CellmlElementItem::Unit,
                                                                unit);

            unitsItem->appendRow(unitItem);

            // Retrieve the unit's unit elements

            if (unit->unitElements().count())
                foreach (CellMLSupport::CellmlFileUnitElement *unitElement,
                         unit->unitElements())
                    unitItem->appendRow(new CellmlElementItem(CellmlElementItem::UnitElement,
                                                              unitElement));

        }
    }
}

//==============================================================================

void CellmlAnnotationViewCellmlListWidget::populateComponentReferenceDataModel(CellmlElementItem *pCellmlElementItem,
                                                                               CellMLSupport::CellmlFileComponentReference *pComponentReference)
{
    CellmlElementItem *componentReferenceItem = new CellmlElementItem(CellmlElementItem::ComponentReference,
                                                                      pComponentReference);

    pCellmlElementItem->appendRow(componentReferenceItem);

    // Retrieve the component reference's children

    foreach (CellMLSupport::CellmlFileComponentReference *componentReference,
             pComponentReference->componentReferences())
        populateComponentReferenceDataModel(componentReferenceItem, componentReference);
}

//==============================================================================

void CellmlAnnotationViewCellmlListWidget::resizeTreeViewToContents()
{
    // Resize our tree view so that its contents is visible

    mTreeView->resizeColumnToContents(0);
}

//==============================================================================

void CellmlAnnotationViewCellmlListWidget::updateNode(const QModelIndex &pNewIndex,
                                                      const QModelIndex &pOldIndex)
{
    Q_UNUSED(pOldIndex);

    // Make sure that the details GUI is valid and that we have a valid
    // pNewIndex

    if (!mParent->detailsWidget() || (pNewIndex == QModelIndex()))
        return;

    // Keep track of the fact that there is a node to update

    mIndexes << pNewIndex;

    // Make sure that we are not already updating a node by checking that the
    // CellML file for which we want to update the node is not in our list of
    // CellML files being updated

    static QStringList cellmlFileBeingUpdated;

    QString cellmlFileName = mParent->cellmlFile()->fileName();

    if (cellmlFileBeingUpdated.contains(cellmlFileName))
        return;

    cellmlFileBeingUpdated << cellmlFileName;

    // Loop while there are nodes to update
    // Note: this is done because a node may take time to update and we may end
    //       up in a situation where several nodes need updating, so...

    while (mIndexes.count()) {
        // Retrieve the first node to update

        QModelIndex crtIndex = mIndexes.first();

        mIndexes.removeFirst();

        // Retrieve all the CellML items which properties we want to be added to
        // the details GUI

        CellmlAnnotationViewCellmlElementDetailsWidget::Items items = CellmlAnnotationViewCellmlElementDetailsWidget::Items();

        do {
            CellmlElementItem *crtCellmlElementItem = static_cast<CellmlElementItem *>(mDataModel->itemFromIndex(crtIndex));

            // Add the item based on its type, but only if it's not a category

            if (!crtCellmlElementItem->isCategory())
                switch (crtCellmlElementItem->type()) {
                case CellmlElementItem::Model:
                    items << CellmlAnnotationViewCellmlElementDetailsWidget::item(CellmlAnnotationViewCellmlElementDetailsWidget::Model,
                                                                                  crtCellmlElementItem->element());

                    break;
                case CellmlElementItem::Import:
                    items << CellmlAnnotationViewCellmlElementDetailsWidget::item(CellmlAnnotationViewCellmlElementDetailsWidget::Import,
                                                                                  crtCellmlElementItem->element());

                    break;
                case CellmlElementItem::ImportUnit:
                    items << CellmlAnnotationViewCellmlElementDetailsWidget::item(CellmlAnnotationViewCellmlElementDetailsWidget::ImportUnit,
                                                                                  crtCellmlElementItem->element());

                    break;
                case CellmlElementItem::ImportComponent:
                    items << CellmlAnnotationViewCellmlElementDetailsWidget::item(CellmlAnnotationViewCellmlElementDetailsWidget::ImportComponent,
                                                                                  crtCellmlElementItem->element());

                    break;
                case CellmlElementItem::Unit:
                    items << CellmlAnnotationViewCellmlElementDetailsWidget::item(CellmlAnnotationViewCellmlElementDetailsWidget::Unit,
                                                                                  crtCellmlElementItem->element());

                    break;
                case CellmlElementItem::UnitElement:
                    items << CellmlAnnotationViewCellmlElementDetailsWidget::item(CellmlAnnotationViewCellmlElementDetailsWidget::UnitElement,
                                                                                  crtCellmlElementItem->element());

                    break;
                case CellmlElementItem::Component:
                    items << CellmlAnnotationViewCellmlElementDetailsWidget::item(CellmlAnnotationViewCellmlElementDetailsWidget::Component,
                                                                                  crtCellmlElementItem->element());

                    break;
                case CellmlElementItem::Variable:
                    items << CellmlAnnotationViewCellmlElementDetailsWidget::item(CellmlAnnotationViewCellmlElementDetailsWidget::Variable,
                                                                                  crtCellmlElementItem->element());

                    break;
                case CellmlElementItem::Group:
                    items << CellmlAnnotationViewCellmlElementDetailsWidget::item(CellmlAnnotationViewCellmlElementDetailsWidget::Group,
                                                                                  crtCellmlElementItem->element(),
                                                                                  crtCellmlElementItem->number());

                    break;
                case CellmlElementItem::RelationshipReference:
                    items << CellmlAnnotationViewCellmlElementDetailsWidget::item(CellmlAnnotationViewCellmlElementDetailsWidget::RelationshipReference,
                                                                                  crtCellmlElementItem->element());

                    break;
                case CellmlElementItem::ComponentReference:
                    items << CellmlAnnotationViewCellmlElementDetailsWidget::item(CellmlAnnotationViewCellmlElementDetailsWidget::ComponentReference,
                                                                                  crtCellmlElementItem->element());

                    break;
                case CellmlElementItem::Connection:
                    items << CellmlAnnotationViewCellmlElementDetailsWidget::item(CellmlAnnotationViewCellmlElementDetailsWidget::Connection,
                                                                                  crtCellmlElementItem->element(),
                                                                                  crtCellmlElementItem->number());

                    break;
                case CellmlElementItem::ComponentMapping:
                    items << CellmlAnnotationViewCellmlElementDetailsWidget::item(CellmlAnnotationViewCellmlElementDetailsWidget::ComponentMapping,
                                                                                  crtCellmlElementItem->element());

                    break;
                case CellmlElementItem::VariableMapping:
                    items << CellmlAnnotationViewCellmlElementDetailsWidget::item(CellmlAnnotationViewCellmlElementDetailsWidget::VariableMapping,
                                                                                  crtCellmlElementItem->element());

                    break;
                default:
                    // Either an error, warning, category or metadata, so
                    // nothing to show/do...

                    ;
                }

            // Go to the next index

            crtIndex = crtIndex.parent();
        } while (crtIndex != QModelIndex());

        // Reverse the list, so that we start from the CellML item's parents and
        // finish with the CellML item itself

        int itemsCount = items.count();

        for (int i = 0, iMax = itemsCount >> 1; i < iMax; ++i)
            items.swap(i, itemsCount-(i+1));

        // Update the details GUI

        mParent->detailsWidget()->updateGui(items);
    }

    // We are done, so...

    cellmlFileBeingUpdated.removeAt(cellmlFileBeingUpdated.indexOf(cellmlFileName));
}

//==============================================================================

void CellmlAnnotationViewCellmlListWidget::treeViewContextMenu(const QPoint &pPosition) const
{
    Q_UNUSED(pPosition);

    // Determine whether to show context menu or not by checking whether the
    // current item is the same as the one over which we are

    CellmlElementItem *posItem = static_cast<CellmlElementItem *>(mDataModel->itemFromIndex(mTreeView->indexAt(mTreeView->mapFromGlobal(QCursor::pos()-mTreeView->pos()))));
    CellmlElementItem *crtItem = currentCellmlElementItem();

    bool showContextMenu = (posItem == crtItem) && crtItem->hasChildren();

    // Create a custom context menu which items match the contents of our
    // toolbar

    if (showContextMenu) {
        // Update the enabled status of our actions

        mGui->actionExpandAll->setEnabled(!indexIsAllExpanded(mTreeView->currentIndex()));
        mGui->actionCollapseAll->setEnabled(mTreeView->isExpanded(mTreeView->currentIndex()));

        // Create and show the context menu

        QMenu menu;

        menu.addAction(mGui->actionExpandAll);
        menu.addAction(mGui->actionCollapseAll);

        menu.exec(QCursor::pos());
    }
}

//==============================================================================

void CellmlAnnotationViewCellmlListWidget::on_actionExpandAll_triggered()
{
    // Expand all the nodes below the current one
    // Note: we disable and then re-enable updates before expanding all the
    //       index since it may end up in quite a few updates...

    mTreeView->setUpdatesEnabled(false);
        indexExpandAll(mTreeView->currentIndex());
    mTreeView->setUpdatesEnabled(true);
}

//==============================================================================

void CellmlAnnotationViewCellmlListWidget::on_actionCollapseAll_triggered()
{
    // Collapse all the nodes below the current one
    // Note: see the note in on_actionExpandAll_triggered() above...

    mTreeView->setUpdatesEnabled(false);
        indexCollapseAll(mTreeView->currentIndex());
    mTreeView->setUpdatesEnabled(true);
}

//==============================================================================

void CellmlAnnotationViewCellmlListWidget::indexExpandAll(const QModelIndex &pIndex) const
{
    // Recursively expand all the nodes below the current one
    // Note: the test with against pIndex.child(0, 0) is to ensure that we are
    //       not trying to expand an index which item has no children. Indeed,
    //       a call to expand() is quite expensive, so the fewer of those we
    //       make the better...

    if (pIndex.child(0, 0) != QModelIndex()) {
        mTreeView->expand(pIndex);

        QStandardItem *item = mDataModel->itemFromIndex(pIndex);

        for (int i = 0, iMax = item->rowCount(); i < iMax; ++i)
            indexExpandAll(item->child(i)->index());
    }
}

//==============================================================================

void CellmlAnnotationViewCellmlListWidget::indexCollapseAll(const QModelIndex &pIndex) const
{
    // Recursively collapse all the nodes below the current one
    // Note: see the note in indexExpandAll() above...

    if (pIndex.child(0, 0) != QModelIndex()) {
        QStandardItem *item = mDataModel->itemFromIndex(pIndex);

        for (int i = 0, iMax = item->rowCount(); i < iMax; ++i)
            indexCollapseAll(item->child(i)->index());

        mTreeView->collapse(pIndex);
    }
}

//==============================================================================

bool CellmlAnnotationViewCellmlListWidget::indexIsAllExpanded(const QModelIndex &pIndex) const
{
    // Recursively check that the current node and all of its children are
    // expanded
    // Note: see the note in indexExpandAll() above...

    if (pIndex.child(0, 0) != QModelIndex()) {
        QStandardItem *item = mDataModel->itemFromIndex(pIndex);

        for (int i = 0, iMax = item->rowCount(); i < iMax; ++i)
            if (!indexIsAllExpanded(item->child(i)->index()))
                return false;

        return mTreeView->isExpanded(pIndex);
    } else {
        return true;
    }
}

//==============================================================================

Core::TreeView * CellmlAnnotationViewCellmlListWidget::treeView() const
{
    // Return our tree view

    return mTreeView;
}

//==============================================================================

CellmlElementItem * CellmlAnnotationViewCellmlListWidget::currentCellmlElementItem() const
{
    // Return the current CellML element item

    return static_cast<CellmlElementItem *>(mDataModel->itemFromIndex(mTreeView->currentIndex()));
}

//==============================================================================

}   // namespace CellMLAnnotationView
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================

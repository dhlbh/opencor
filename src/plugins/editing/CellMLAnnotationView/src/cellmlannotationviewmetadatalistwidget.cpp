//==============================================================================
// CellML annotation view metadata list widget
//==============================================================================

#include "borderedwidget.h"
#include "cellmlannotationviewcellmldetailswidget.h"
#include "cellmlannotationviewcellmlelementitem.h"
#include "cellmlannotationviewcellmlelementitemdelegate.h"
#include "cellmlannotationviewdetailswidget.h"
#include "cellmlannotationviewmetadatalistwidget.h"
#include "cellmlannotationviewwidget.h"
#include "treeview.h"

//==============================================================================

#include "ui_cellmlannotationviewmetadatalistwidget.h"

//==============================================================================

#include <QStandardItemModel>

//==============================================================================

namespace OpenCOR {
namespace CellMLAnnotationView {

//==============================================================================

CellmlAnnotationViewMetadataListWidget::CellmlAnnotationViewMetadataListWidget(CellmlAnnotationViewWidget *pParent) :
    Widget(pParent),
    mParent(pParent),
    mGui(new Ui::CellmlAnnotationViewMetadataListWidget),
    mIndexes(QList<QModelIndex>())
{
    // Set up the GUI

    mGui->setupUi(this);

    // Create and customise our tree view which will contain all of the metadata
    // from a CellML file

    mTreeView  = new Core::TreeView(pParent);
    mDataModel = new QStandardItemModel(mTreeView);

    mTreeView->setModel(mDataModel);

    mTreeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mTreeView->setFrameShape(QFrame::NoFrame);
    mTreeView->setHeaderHidden(true);
    mTreeView->setRootIsDecorated(false);
    mTreeView->setSelectionMode(QAbstractItemView::SingleSelection);

    // Populate ourselves

    mGui->layout->addWidget(mTreeView);

    // Set an event filter for our tree view

    mTreeView->installEventFilter(this);

    // A connection to handle the change of node

    connect(mTreeView->selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),
            this, SLOT(updateNode(const QModelIndex &, const QModelIndex &)));

    // Populate our tree view

    populateDataModel();

    // Resize our tree view, just to be on the safe side

    mTreeView->resizeColumnToContents(0);
}

//==============================================================================

CellmlAnnotationViewMetadataListWidget::~CellmlAnnotationViewMetadataListWidget()
{
    // Delete the GUI

    delete mGui;
}

//==============================================================================

bool CellmlAnnotationViewMetadataListWidget::eventFilter(QObject *pObject,
                                                         QEvent *pEvent)
{
    Q_UNUSED(pObject);

    switch (pEvent->type()) {
    case QEvent::FocusIn: {
        // Note: to override focusInEvent() is not what we want since it won't
        //       be triggered when selecting a node in the metadata list after
        //       having selecting a node in the CellML list. Indeed, it's
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

void CellmlAnnotationViewMetadataListWidget::populateDataModel()
{
    // Make sure that the CellML file was properly loaded

    if (mParent->cellmlFile()->issues().count())
        // There are issues, so...

        return;

    // Retrieve the id of the different groups of triples

    QString uriBase = mParent->cellmlFile()->uriBase();
    QStringList ids = QStringList();

    foreach (CellMLSupport::CellmlFileRdfTriple *rdfTriple,
             mParent->cellmlFile()->rdfTriples())
        // Retrieve the RDF triple's subject so we can determine to which group
        // of RDF triples it should be added
        // Note: the RDF triple is part of an rdf:Description element which is
        //       itself part of an rdf:RDF element. The rdf:Description element
        //       has an rdf:about attribute which may or not have a value
        //       assigned to it. If no value was assigned, then the RDF triple
        //       is valid at the whole CellML model level. On the other hand, if
        //       a value was assigned (and of the format #<id>), then it will be
        //       associated to any CellML element which cmeta:id value is <id>.
        //       A couple of examples:
        //
        // <rdf:RDF xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#" xmlns:dc="http://purl.org/dc/elements/1.1/" xmlns:vCard="http://www.w3.org/2001/vcard-rdf/3.0#">
        //   <rdf:Description rdf:about="">
        //     <dc:creator rdf:parseType="Resource">
        //       <vCard:EMAIL rdf:parseType="Resource">
        //         <rdf:type rdf:resource="http://imc.org/vCard/3.0#internet"/>
        //         <rdf:value>someone@somewhere.com</rdf:value>
        //       </vCard:EMAIL>
        //     </dc:creator>
        //   </rdf:Description>
        // </rdf:RDF>
        //
        // <variable units="micromolar" public_interface="out" cmeta:id="C_C" name="C" initial_value="0.01">
        //   <rdf:RDF xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#" xmlns:bqbiol="http://biomodels.net/biology-qualifiers/">
        //     <rdf:Description rdf:about="#C_C">
        //       <bqbiol:isVersionOf rdf:resource="http://identifiers.org/uniprot/Q4KLA0"/>
        //       <bqbiol:isVersionOf rdf:resource="http://identifiers.org/interpro/IPR006670"/>
        //       <bqbiol:isVersionOf rdf:resource="http://identifiers.org/obo.sbo/sbo:0000252"/>
        //     </rdf:Description>
        //   </rdf:RDF>
        // </variable>

        if (rdfTriple->subject()->type() == CellMLSupport::CellmlFileRdfTripleElement::UriReference) {
            // We have an RDF triple of which we can make sense, so add it to
            // the correct group of RDF triples
            // Note: we want the id of the group to be the same as that of the
            //       cmeta:id of a CellML element. This means that we must
            //       remove the URI base (and hash character) which makes the
            //       beginning of the RDF triple's subject's URI reference...

            QString id = rdfTriple->subject()->uriReference().remove(QRegExp("^"+QRegExp::escape(uriBase)+"#?"));

            if (!ids.contains(id)) {
                // The id hasn't already been added, so add it and keep track of
                // it

                QStandardItem *item = new QStandardItem(id);

                item->setIcon(QIcon(":CellMLSupport_metadataNode"));

                mDataModel->invisibleRootItem()->appendRow(item);

                ids << id;
            }
        }

    // Sort the ids

    mDataModel->sort(0);
}

//==============================================================================

void CellmlAnnotationViewMetadataListWidget::updateNode(const QModelIndex &pNewIndex,
                                                        const QModelIndex &pOldIndex)
{
    Q_UNUSED(pOldIndex);

    // Make sure that the details GUI is valid and that we have a valid
    // pNewIndex

    if (!mParent->detailsWidget() || (pNewIndex == QModelIndex()))
        return;

    // Keep track of the fact that there is a node to update

    mIndexes << pNewIndex;

    // Make sure that we are not already updating a node

    static bool alreadyUpdatingNode = false;

    if (alreadyUpdatingNode)
        return;

    alreadyUpdatingNode = true;

    // Loop while there are nodes to update
    // Note: this is done because a node may take time to update, so we may end
    //       up in a situation where several nodes need updating...

    while (mIndexes.count()) {
        // Retrieve the first node to update

        QModelIndex crtIndex = mIndexes.first();

        mIndexes.removeFirst();

        // Update the details GUI

        mParent->detailsWidget()->updateGui(mParent->rdfTriples(mDataModel->itemFromIndex(crtIndex)->text()));
    }

    // We are done, so...

    alreadyUpdatingNode = false;
}

//==============================================================================

Core::TreeView * CellmlAnnotationViewMetadataListWidget::treeView() const
{
    // Return our tree view

    return mTreeView;
}

//==============================================================================

QStringList CellmlAnnotationViewMetadataListWidget::ids() const
{
    // Return the full list of ids

    QStringList res = QStringList();

    for (int i = 0, iMax = mDataModel->invisibleRootItem()->rowCount(); i < iMax; ++i)
        res << mDataModel->invisibleRootItem()->child(i)->text();

    return res;
}

//==============================================================================

}   // namespace CellMLAnnotationView
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================

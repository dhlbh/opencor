//==============================================================================
// CellML annotation view metadata raw view details widget
//==============================================================================

#include "cellmlannotationviewmetadatarawviewdetailswidget.h"
#include "cellmlannotationviewwidget.h"
#include "treeview.h"

//==============================================================================

#include "ui_cellmlannotationviewmetadatarawviewdetailswidget.h"

//==============================================================================

namespace OpenCOR {
namespace CellMLAnnotationView {

//==============================================================================

CellmlAnnotationViewMetadataRawViewDetailsWidget::CellmlAnnotationViewMetadataRawViewDetailsWidget(CellmlAnnotationViewWidget *pParent) :
    Widget(pParent),
    mParent(pParent),
    mGui(new Ui::CellmlAnnotationViewMetadataRawViewDetailsWidget)
{
    // Set up the GUI

    mGui->setupUi(this);

    mTreeView  = new Core::TreeView(this);
    mDataModel = new QStandardItemModel(mTreeView);

    mTreeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mTreeView->setFrameShape(QFrame::NoFrame);
    mTreeView->setModel(mDataModel);
    mTreeView->setRootIsDecorated(false);
    mTreeView->setSelectionMode(QAbstractItemView::SingleSelection);

    mGui->layout->addWidget(mTreeView);

    // Some further initialisations which are done as part of retranslating the
    // GUI (so that they can be updated when changing languages)

    retranslateUi();
}

//==============================================================================

CellmlAnnotationViewMetadataRawViewDetailsWidget::~CellmlAnnotationViewMetadataRawViewDetailsWidget()
{
    // Delete the GUI

    delete mGui;
}

//==============================================================================

void CellmlAnnotationViewMetadataRawViewDetailsWidget::retranslateUi()
{
    // Retranslate our GUI

    mGui->retranslateUi(this);

    // Update the header labels

    mDataModel->setHorizontalHeaderLabels(QStringList() << tr("#")
                                                        << tr("Subject")
                                                        << tr("Predicate")
                                                        << tr("Object"));
}

//==============================================================================

void CellmlAnnotationViewMetadataRawViewDetailsWidget::updateGui(const CellMLSupport::CellmlFileRdfTriples &pRdfTriples)
{
    // Hide ourselves (to avoid any flickering during the update)

    setVisible(false);

    // Remove all previous RDF triples from our tree view

    while (mDataModel->rowCount())
        foreach (QStandardItem *item, mDataModel->takeRow(0))
            delete item;

    // Add the 'new' RDF triples to our tree view
    // Note: for the RDF triple's subject, we try to remove the CellML file's
    //       URI base, thus only leaving the equivalent of a CellML element
    //       cmeta:id which will speak more to the user than a possibly long URI
    //       reference...

    QString uriBase = mParent->cellmlFile()->uriBase();
    int rdfTripleCounter = 0;

    foreach (CellMLSupport::CellmlFileRdfTriple *rdfTriple, pRdfTriples)
        mDataModel->invisibleRootItem()->appendRow(QList<QStandardItem *>() << new QStandardItem(QString::number(++rdfTripleCounter))
                                                                            << new QStandardItem(rdfTriple->subject()->asString().remove(QRegExp("^"+QRegExp::escape(uriBase)+"#?")))
                                                                            << new QStandardItem(rdfTriple->predicate()->asString())
                                                                            << new QStandardItem(rdfTriple->object()->asString()));

    // Make sure that all the columns have their contents fit

    mTreeView->resizeColumnToContents(0);
    mTreeView->resizeColumnToContents(1);
    mTreeView->resizeColumnToContents(2);
    mTreeView->resizeColumnToContents(3);

    // Re-show ourselves

    setVisible(true);
}

//==============================================================================

}   // namespace CellMLAnnotationView
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================

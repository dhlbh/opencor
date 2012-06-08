//==============================================================================
// CellML annotation view CellML details widget
//==============================================================================

#include "borderedwidget.h"
#include "cellmlannotationviewcellmldetailswidget.h"
#include "cellmlannotationviewmetadatabiomodelsdotnetviewdetailswidget.h"
#include "cellmlannotationviewwidget.h"
#include "cellmlannotationviewmetadataviewdetailswidget.h"

//==============================================================================

#include "ui_cellmlannotationviewcellmldetailswidget.h"

//==============================================================================

#include <QComboBox>
#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QWebView>

//==============================================================================

#include <QJsonParser>

//==============================================================================

namespace OpenCOR {
namespace CellMLAnnotationView {

//==============================================================================

CellmlAnnotationViewCellmlDetailsWidget::CellmlAnnotationViewCellmlDetailsWidget(CellmlAnnotationViewWidget *pParent) :
    QSplitter(pParent),
    CommonWidget(pParent),
    mParent(pParent),
    mGui(new Ui::CellmlAnnotationViewCellmlDetailsWidget)
{
    // Set up the GUI

    mGui->setupUi(this);

    // Create our details widgets

    mCellmlElementDetails = new CellmlAnnotationViewCellmlElementDetailsWidget(pParent);
    mMetadataViewDetails  = new CellmlAnnotationViewMetadataViewDetailsWidget(pParent);
    mWebView              = new QWebView(pParent);

    // A connection to handle the looking up of a resource and a resource id

    connect(mMetadataViewDetails->bioModelsDotNetView(), SIGNAL(resourceLookupRequested(const QString &)),
            this, SLOT(resourceLookupRequested(const QString &)));
    connect(mMetadataViewDetails->bioModelsDotNetView(), SIGNAL(resourceIdLookupRequested(const QString &, const QString &)),
            this, SLOT(resourceIdLookupRequested(const QString &, const QString &)));

    // Add our details widgets to our splitter

    addWidget(new Core::BorderedWidget(mCellmlElementDetails,
                                       false, true, true, false));
    addWidget(new Core::BorderedWidget(mMetadataViewDetails,
                                       true, true, true, false));
    addWidget(new Core::BorderedWidget(mWebView,
                                       true, true, false, false));

    // Keep track of our splitter being moved

    connect(this, SIGNAL(splitterMoved(int,int)),
            this, SLOT(emitSplitterMoved()));
}

//==============================================================================

CellmlAnnotationViewCellmlDetailsWidget::~CellmlAnnotationViewCellmlDetailsWidget()
{
    // Delete the GUI

    delete mGui;
}

//==============================================================================

void CellmlAnnotationViewCellmlDetailsWidget::retranslateUi()
{
    // Retranslate our GUI

    mGui->retranslateUi(this);

    mCellmlElementDetails->retranslateUi();
    mMetadataViewDetails->retranslateUi();
}

//==============================================================================

void CellmlAnnotationViewCellmlDetailsWidget::updateGui(const CellmlAnnotationViewCellmlElementDetailsWidget::Items &pItems)
{
    // Stop tracking any change in the cmeta:id value of our CellML element

    if (mCellmlElementDetails->cmetaIdValue()) {
        disconnect(mCellmlElementDetails->cmetaIdValue(), SIGNAL(currentIndexChanged(const QString &)),
                   this, SLOT(newCmetaIdValue(const QString &)));
        disconnect(mCellmlElementDetails->cmetaIdValue(), SIGNAL(editTextChanged(const QString &)),
                   this, SLOT(newCmetaIdValue(const QString &)));
    }

    // 'Clean up' our web view

    mWebView->setUrl(QString());

    // Update our CellML element details GUI

    mCellmlElementDetails->updateGui(pItems);

    // Track any change in the cmeta:id value of our CellML element

    connect(mCellmlElementDetails->cmetaIdValue(), SIGNAL(currentIndexChanged(const QString &)),
            this, SLOT(newCmetaIdValue(const QString &)));
    connect(mCellmlElementDetails->cmetaIdValue(), SIGNAL(editTextChanged(const QString &)),
            this, SLOT(newCmetaIdValue(const QString &)));

    // Update our metadata details GUI

    newCmetaIdValue(mCellmlElementDetails->cmetaIdValue()->currentText());
}

//==============================================================================

void CellmlAnnotationViewCellmlDetailsWidget::updateSizes(const QList<int> &pSizes)
{
    // The splitter of another CellmlAnnotationViewCellmlDetailsWidget object
    // has been moved, so update our sizes

    setSizes(pSizes);
}

//==============================================================================

void CellmlAnnotationViewCellmlDetailsWidget::emitSplitterMoved()
{
    // Let whoever know that our splitter has been moved

    emit splitterMoved(sizes());
}

//==============================================================================

void CellmlAnnotationViewCellmlDetailsWidget::newCmetaIdValue(const QString &pCmetaIdValue)
{
    // The cmeta:id value of our CellML element has changed, so update its
    // metadata details, but first 'clean up' our web view

    mWebView->setUrl(QString());

    if (pCmetaIdValue.isEmpty())
        // The CellML element doesn't have a cmeta:id value, so we don't want
        // any metadata to be shown for it (not even the model-wide metadata)

        mMetadataViewDetails->updateGui();
    else
        mMetadataViewDetails->updateGui(mParent->rdfTriples(pCmetaIdValue));
}

//==============================================================================

void CellmlAnnotationViewCellmlDetailsWidget::resourceLookupRequested(const QString &pResource)
{
    // The user requested a resource to be looked up, so retrieve it using
    // identifiers.org

    mWebView->setUrl("http://identifiers.org/"+pResource+"/?redirect=true");
}

//==============================================================================

void CellmlAnnotationViewCellmlDetailsWidget::resourceIdLookupRequested(const QString &pResource,
                                                                        const QString &pId)
{
    // The user requested a resource id to be looked up, so retrieve it using
    // identifiers.org

    mWebView->setUrl("http://identifiers.org/"+pResource+"/"+pId+"?profile=most_reliable&redirect=true");
}

//==============================================================================

}   // namespace CellMLAnnotationView
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================

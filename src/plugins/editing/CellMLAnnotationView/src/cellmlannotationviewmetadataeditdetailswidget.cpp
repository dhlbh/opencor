//==============================================================================
// CellML annotation view metadata edit details widget
//==============================================================================

#include "cellmlannotationviewlistswidget.h"
#include "cellmlannotationviewmetadataeditdetailswidget.h"
#include "cellmlannotationviewmetadatalistwidget.h"
#include "cellmlannotationviewwidget.h"
#include "coreutils.h"
#include "treeview.h"

//==============================================================================

#include "ui_cellmlannotationviewmetadataeditdetailswidget.h"

//==============================================================================

#include <QComboBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QStackedWidget>
#include <QVariant>
#include <QVBoxLayout>

//==============================================================================

#include <QJsonParser>

//==============================================================================

namespace OpenCOR {
namespace CellMLAnnotationView {

//==============================================================================

CellmlAnnotationViewMetadataEditDetailsWidget::CellmlAnnotationViewMetadataEditDetailsWidget(CellmlAnnotationViewWidget *pParent) :
    QScrollArea(pParent),
    CommonWidget(pParent),
    mGui(new Ui::CellmlAnnotationViewMetadataEditDetailsWidget),
    mMainWidget(0),
    mMainLayout(0),
    mFormWidget(0),
    mFormLayout(0),
    mGridWidget(0),
    mGridLayout(0),
    mErrorMsg(QString()),
    mTermUrl(QString()),
    mOtherTermUrl(QString())
{
    // Set up the GUI

    mGui->setupUi(this);

    // Create a stacked widget which will contain our GUI

    mWidget = new QStackedWidget(this);

    // Add our stacked widget to our scroll area

    setWidget(mWidget);

    // Create a network access manager so that we can then retrieve a list of
    // CellML models from the CellML Model Repository

    mNetworkAccessManager = new QNetworkAccessManager(this);

    // Make sure that we get told when the download of our Internet file is
    // complete

    connect(mNetworkAccessManager, SIGNAL(finished(QNetworkReply *)),
            this, SLOT(finished(QNetworkReply *)) );
}

//==============================================================================

CellmlAnnotationViewMetadataEditDetailsWidget::~CellmlAnnotationViewMetadataEditDetailsWidget()
{
    // Delete the GUI

    delete mGui;
}

//==============================================================================

void CellmlAnnotationViewMetadataEditDetailsWidget::retranslateUi()
{
    // Retranslate our GUI

    mGui->retranslateUi(this);

    // For the rest of our GUI, it's easier to just update it, so...

    updateGui();
}

//==============================================================================

void CellmlAnnotationViewMetadataEditDetailsWidget::updateGui(const bool &pPopulate)
{
    // Note: we are using certain layouts to dislay the contents of our view,
    //       but this unfortunately results in some very bad flickering on Mac
    //       OS X. This can, however, be addressed using a stacked widget, so...

    // Prevent ourselves from being updated (to avoid any flickering)

    setUpdatesEnabled(false);

    // Create a widget which will contain our GUI

    QWidget *newMainWidget = new QWidget(this);
    QVBoxLayout *newMainLayout = new QVBoxLayout(newMainWidget);

    newMainLayout->setMargin(0);

    newMainWidget->setLayout(newMainLayout);

    // Populate our GUI, but only if requested

    QWidget *newFormWidget = 0;
    QFormLayout *newFormLayout = 0;
    QWidget *newGridWidget = 0;
    QGridLayout *newGridLayout = 0;

    if (pPopulate) {
        // Deal with the form part of our GUI

        // Create a form widget which will contain our qualifier and term fields

        newFormWidget = new QWidget(newMainWidget);
        newFormLayout = new QFormLayout(newFormWidget);

        newFormWidget->setLayout(newFormLayout);

        // Add our qualifier and term fields

        QComboBox *qualifierValue = new QComboBox(newFormWidget);

        qualifierValue->addItems(CellMLSupport::CellmlFileRdfTriple::qualifiersAsStringList());

        newFormLayout->addRow(Core::newLabel(newFormWidget, tr("Qualifier:"), true),
                              qualifierValue);

        QLineEdit *termValue = new QLineEdit(newFormWidget);

        connect(termValue, SIGNAL(textChanged(const QString &)),
                this, SLOT(newTerm(const QString &)));

        newFormLayout->addRow(Core::newLabel(newFormWidget, tr("Term:"), true),
                              termValue);

        // Let people know that the GUI has been populated

        emit guiPopulated(qualifierValue, termValue);

        // Deal with the grid part of our GUI

        // Create a new widget and layout

        newGridWidget = new QWidget(newMainWidget);
        newGridLayout = new QGridLayout(newGridWidget);

        newGridWidget->setLayout(newGridLayout);

        // Populate our new layout, but only if there is at least one RDF triple
        // Note: the two calls to setRowStretch() ensures that our grid layout
        //       takes as much vertical space as possible (otherwise our form
        //       layout might take some vertical space making it look a bit odd
        //       if there are no data available)...

        newGridLayout->setRowStretch(0, 1);

        newGridLayout->addWidget(Core::newLabel(newMainWidget,
                                                tr("No data available..."),
                                                false, 1.25, Qt::AlignCenter),
                                 1, 0);

        newGridLayout->setRowStretch(2, 1);

        // Add our 'internal' widgets to our new main widget

        newMainLayout->addWidget(newFormWidget);
        newMainLayout->addWidget(Core::newLineWidget(mWidget));
        newMainLayout->addWidget(newGridWidget);
    }

    // Add our new widget to our stacked widget

    mWidget->addWidget(newMainWidget);

    // Remove the contents of our old form layout

    if (mFormWidget)
        for (int i = 0, iMax = mFormLayout->count(); i < iMax; ++i) {
            QLayoutItem *item = mFormLayout->takeAt(0);

            delete item->widget();
            delete item;
        }

    // Remove the contents of our old grid layout

    if (mGridWidget)
        for (int i = 0, iMax = mGridLayout->count(); i < iMax; ++i) {
            QLayoutItem *item = mGridLayout->takeAt(0);

            delete item->widget();
            delete item;
        }

    // Get rid of our old main widget and layout (and its contents)

    if (mMainWidget) {
        mWidget->removeWidget(mMainWidget);

        for (int i = 0, iMax = mMainLayout->count(); i < iMax; ++i) {
            QLayoutItem *item = mMainLayout->takeAt(0);

            delete item->widget();
            delete item;
        }

        delete mMainWidget;
    }

    // Keep track of our new main widgets and layouts

    mMainWidget = newMainWidget;
    mMainLayout = newMainLayout;

    mFormWidget = newFormWidget;
    mFormLayout = newFormLayout;

    mGridWidget = newGridWidget;
    mGridLayout = newGridLayout;

    // Allow ourselves to be updated again

    setUpdatesEnabled(true);
}

//==============================================================================

void CellmlAnnotationViewMetadataEditDetailsWidget::newTerm(const QString &pTerm)
{
    // Retrieve some possible ontological terms based on the given term

    QString termUrl = QString("http://www.semanticsbml.org/semanticSBML/annotate/search.json?q=%1&full_info=1").arg(pTerm);

    if (mTermUrl.isEmpty()) {
        // No other term is being looked up, so keep track of the given term and
        // look it up

        mTermUrl = termUrl;

        mNetworkAccessManager->get(QNetworkRequest(termUrl));
    } else {
        // Another term is already being looked up, so keep track of the given
        // term for later

        mOtherTermUrl = termUrl;
    }
}

//==============================================================================

void CellmlAnnotationViewMetadataEditDetailsWidget::finished(QNetworkReply *pNetworkReply)
{
    // Output the list of models, should we have retrieved it without any
    // problem

    if (pNetworkReply->error() == QNetworkReply::NoError) {
        // Parse the JSON code

        QJson::Parser jsonParser;
        bool parsingOk;

        QVariantMap resultMap = jsonParser.parse(pNetworkReply->readAll(), &parsingOk).toMap();

static int counter = 0;
qDebug("---[%03d]--------------------------------------", ++counter);
qDebug("URL: %s", qPrintable(pNetworkReply->url().toString()));

        if (parsingOk) {
            // Retrieve the list of CellML models

            foreach (const QVariant &ontologicalsTermVariant, resultMap["result"].toList()) {
                QVariantList ontologicalTermVariant = ontologicalsTermVariant.toList();

                for (int i = 0, iMax = ontologicalTermVariant.count(); i < iMax; ++i) {
                    QVariantMap ontologicalTermMap = ontologicalTermVariant[i].toMap();

qDebug(">>> %s ---> %s", qPrintable(ontologicalTermMap["uri"].toString()),
                         qPrintable(ontologicalTermMap["name"].toString()));
                }
            }

            // Everything went fine, so...

            mErrorMsg = QString();
        } else {
            // Something went wrong, so...

            mErrorMsg = jsonParser.errorString();
        }
    } else {
        // Something went wrong, so...

        mErrorMsg = pNetworkReply->errorString();
    }

    // We are done, so...

    mTermUrl = QString();

    // Check whether there is another term to look up

    if (!mOtherTermUrl.isEmpty()) {
        mNetworkAccessManager->get(QNetworkRequest(mOtherTermUrl));

        mOtherTermUrl = QString();
    }
}

//==============================================================================

}   // namespace CellMLAnnotationView
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================

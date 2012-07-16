//==============================================================================
// CellML annotation view metadata BioModels.Net view details widget
//==============================================================================

#ifndef CELLMLANNOTATIONVIEWMETADATABIOMODELSDOTNETVIEWDETAILSWIDGET_H
#define CELLMLANNOTATIONVIEWMETADATABIOMODELSDOTNETVIEWDETAILSWIDGET_H

//==============================================================================

#include "cellmlfile.h"
#include "commonwidget.h"

//==============================================================================

#include <QScrollArea>

//==============================================================================

namespace Ui {
    class CellmlAnnotationViewMetadataBioModelsDotNetViewDetailsWidget;
}

//==============================================================================

class QGridLayout;
class QStackedWidget;

//==============================================================================

namespace OpenCOR {
namespace CellMLAnnotationView {

//==============================================================================

class CellmlAnnotationViewWidget;

//==============================================================================

class CellmlAnnotationViewMetadataBioModelsDotNetViewDetailsWidget : public QScrollArea,
                                                                     public Core::CommonWidget
{
    Q_OBJECT

private:
    enum Type {
        Unknown,
        Qualifier,
        Resource,
        Id
    };

public:
    explicit CellmlAnnotationViewMetadataBioModelsDotNetViewDetailsWidget(CellmlAnnotationViewWidget *pParent,
                                                                          const bool &pEditingMode);
    ~CellmlAnnotationViewMetadataBioModelsDotNetViewDetailsWidget();

    virtual void retranslateUi();

    void updateGui(const CellMLSupport::CellmlFileRdfTriples &pRdfTriples,
                   const QString &pRdfTripleInformation = QString(),
                   const Type &pType = Unknown,
                   const bool &pRetranslate = false);

private:
    CellMLSupport::CellmlFile *mCellmlFile;

    Ui::CellmlAnnotationViewMetadataBioModelsDotNetViewDetailsWidget *mGui;

    QStackedWidget *mWidget;

    QWidget *mGridWidget;
    QGridLayout *mGridLayout;

    CellMLSupport::CellmlFileRdfTriples mRdfTriples;

    QString mRdfTripleInformation;
    Type mType;

    bool mLookupInformation;

    bool mEditingMode;

    QMap<QObject *, CellMLSupport::CellmlFileRdfTriple *> mRdfTriplesMapping;

    void genericLookup(const QString &pRdfTripleInformation = QString(),
                       const Type &pType = Unknown,
                       const bool &pRetranslate = false);

    QString rdfTripleInformation(const int &pRow) const;

Q_SIGNALS:
    void qualifierLookupRequested(const QString &pQualifier,
                                  const bool &pRetranslate);
    void resourceLookupRequested(const QString &pResource,
                                 const bool &pRetranslate);
    void resourceIdLookupRequested(const QString &pResource, const QString &pId,
                                   const bool &pRetranslate);
    void unknownLookupRequested();

    void metadataUpdated();

private Q_SLOTS:
    void disableLookupInformation();

    void lookupQualifier(const QString &pRdfTripleInformation,
                         const bool &pRetranslate = false);
    void lookupResource(const QString &pRdfTripleInformation,
                        const bool &pRetranslate = false);
    void lookupResourceId(const QString &pRdfTripleInformation,
                          const bool &pRetranslate = false);

    void removeRdfTriple();
};

//==============================================================================

}   // namespace CellMLAnnotationView
}   // namespace OpenCOR

//==============================================================================

#endif

//==============================================================================
// End of file
//==============================================================================

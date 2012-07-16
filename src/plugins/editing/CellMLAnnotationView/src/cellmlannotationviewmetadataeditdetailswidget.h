//==============================================================================
// CellML annotation view metadata edit details widget
//==============================================================================

#ifndef CELLMLANNOTATIONVIEWMETADATAEDITDETAILSWIDGET_H
#define CELLMLANNOTATIONVIEWMETADATAEDITDETAILSWIDGET_H

//==============================================================================

#include "commonwidget.h"

//==============================================================================

#include <QScrollArea>

//==============================================================================

namespace Ui {
    class CellmlAnnotationViewMetadataEditDetailsWidget;
}

//==============================================================================

class QComboBox;
class QFormLayout;
class QGridLayout;
class QLineEdit;
class QNetworkAccessManager;
class QNetworkReply;
class QPushButton;
class QStackedWidget;
class QVBoxLayout;

//==============================================================================

namespace OpenCOR {
namespace CellMLAnnotationView {

//==============================================================================

class CellmlAnnotationViewWidget;

//==============================================================================

class CellmlAnnotationViewMetadataEditDetailsWidget : public QScrollArea,
                                                      public Core::CommonWidget
{
    Q_OBJECT

private:
    struct Item
    {
        QString name;
        QString resource;
        QString id;

        bool operator<(const Item &pItem) const;
    };

    typedef QList<Item> Items;

public:
    explicit CellmlAnnotationViewMetadataEditDetailsWidget(CellmlAnnotationViewWidget *pParent);
    ~CellmlAnnotationViewMetadataEditDetailsWidget();

    virtual void retranslateUi();

    void updateGui(const Items &pItems, const QString &pErrorMsg,
                   const bool &pRetranslate = false);

private:
    Ui::CellmlAnnotationViewMetadataEditDetailsWidget *mGui;

    QStackedWidget *mWidget;

    QWidget *mMainWidget;
    QVBoxLayout *mMainLayout;

    QWidget *mFormWidget;
    QFormLayout *mFormLayout;

    QScrollArea *mItemsScrollArea;

    QWidget *mGridWidget;
    QGridLayout *mGridLayout;

    QNetworkAccessManager *mNetworkAccessManager;

    QComboBox *mQualifierValue;

    QString mTerm;
    QString mTermUrl;
    QString mOtherTermUrl;

    Items mItems;
    QString mErrorMsg;

    QString mQualifier;
    QString mResource;
    QString mId;

    bool mLookupInformation;

    void updateItemsGui(const Items &pItems, const QString &pErrorMsg);

    static Item item(const QString &pName,
                     const QString &pResource, const QString &pId);

    void genericLookup(const QString &pQualifier = QString(),
                       const QString &pResource = QString(),
                       const QString &pId = QString(),
                       const bool &pRetranslate = false);

Q_SIGNALS:
    void guiPopulated(QComboBox *pQualifierValue, QPushButton *pLookupButton,
                      QLineEdit *pTermValue);

    void qualifierLookupRequested(const QString &pQualifier,
                                  const bool &pRetranslate);
    void resourceLookupRequested(const QString &pResource,
                                 const bool &pRetranslate);
    void resourceIdLookupRequested(const QString &pResource, const QString &pId,
                                   const bool &pRetranslate);

private Q_SLOTS:
    void disableLookupInformation();

    void lookupQualifier(const bool &pRetranslate = false);

    void lookupTerm(const QString &pTerm);
    void termLookupFinished(QNetworkReply *pNetworkReply);
};

//==============================================================================

}   // namespace CellMLAnnotationView
}   // namespace OpenCOR

//==============================================================================

#endif

//==============================================================================
// End of file
//==============================================================================

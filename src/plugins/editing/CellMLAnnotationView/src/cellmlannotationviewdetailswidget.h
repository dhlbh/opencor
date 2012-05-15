//==============================================================================
// CellML annotation view details widget
//==============================================================================

#ifndef CELLMLANNOTATIONVIEWDETAILSWIDGET_H
#define CELLMLANNOTATIONVIEWDETAILSWIDGET_H

//==============================================================================

#include "cellmlfile.h"
#include "widget.h"

//==============================================================================

#include <QScrollArea>
#include <QStandardItem>
#include <QStyledItemDelegate>

//==============================================================================

namespace Ui {
    class CellmlAnnotationViewDetailsWidget;
}

//==============================================================================

class QFormLayout;
class QVBoxLayout;

//==============================================================================

namespace OpenCOR {
namespace CellMLAnnotationView {

//==============================================================================

class CellmlAnnotationViewDetailsWidget : public QScrollArea,
                                          public Core::CommonWidget
{
    Q_OBJECT

public:
    enum Type
    {
        Model,
        Import,
        ImportUnit,
        ImportComponent,
        Unit,
        UnitElement,
        Component,
        Variable,
        Group,
        RelationshipReference,
        ComponentReference,
        Connection,
        ComponentMapping,
        VariableMapping
    };

    struct CellmlItem
    {
        Type type;
        CellMLSupport::CellmlFileElement *element;
        QString name;
    };

    typedef QList<CellmlItem> CellmlItems;

    explicit CellmlAnnotationViewDetailsWidget(QWidget *pParent);
    ~CellmlAnnotationViewDetailsWidget();

    virtual void retranslateUi();

    static CellmlItem cellmlItem(const Type &pType,
                                 CellMLSupport::CellmlFileElement *pElement,
                                 const QString &pName = QString());

    void updateGui(const CellmlItems &pCellmlItems);
    void updateGui(const CellMLSupport::CellmlFileRdfTriples &pRdfTriples);

    QWidget * focusProxyWidget();

private:
    Ui::CellmlAnnotationViewDetailsWidget *mGui;

    CellmlItems mCellmlItems;
    CellMLSupport::CellmlFileRdfTriples mRdfTriples;

    QWidget *mCellmlWidget;
    QFormLayout *mCellmlFormLayout;
    QLineEdit *mCellmlCmetaIdValue;

    QWidget *mMetadataWidget;
    QVBoxLayout *mMetadataFormLayout;

    void addRowToFormLayout(const QString &pLabel, const QString &pValue);

    void updateGui(const CellmlItems &pCellmlItems,
                   const CellMLSupport::CellmlFileRdfTriples &pRdfTriples);

    QString typeAsString(const Type &pType) const;
};

//==============================================================================

}   // namespace CellMLAnnotationView
}   // namespace OpenCOR

//==============================================================================

#endif

//==============================================================================
// End of file
//==============================================================================

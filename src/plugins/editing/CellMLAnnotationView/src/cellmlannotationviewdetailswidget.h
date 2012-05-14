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
class QLabel;

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
        Empty,
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
        VariableMapping,
        Metadata
    };

    struct Item
    {
        Type type;
        CellMLSupport::CellmlFileElement *element;
        QString name;
    };

    typedef QList<Item> Items;

    explicit CellmlAnnotationViewDetailsWidget(QWidget *pParent);
    ~CellmlAnnotationViewDetailsWidget();

    virtual void retranslateUi();

    static Item item(const Type &pType = Empty,
                     CellMLSupport::CellmlFileElement *pElement = 0,
                     const QString &pName = QString());
    static Item item(const QString &pName);

    void updateGui(const Items &pItems);

    QWidget * focusProxyWidget();

private:
    Ui::CellmlAnnotationViewDetailsWidget *mGui;

    Items mItems;

    QWidget *mWidget;
    QFormLayout *mFormLayout;
    QLineEdit *mCmetaIdValue;

    void addRowToFormLayout(const QString &pLabel, const QString &pValue);

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

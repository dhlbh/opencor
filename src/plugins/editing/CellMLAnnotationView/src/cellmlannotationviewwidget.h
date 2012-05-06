//==============================================================================
// CellML annotation view widget
//==============================================================================

#ifndef CELLMLANNOTATIONVIEWWIDGET_H
#define CELLMLANNOTATIONVIEWWIDGET_H

//==============================================================================

#include "cellmlfile.h"
#include "widget.h"

//==============================================================================

#include <QStandardItem>
#include <QStyledItemDelegate>

//==============================================================================

namespace Ui {
    class CellmlAnnotationViewWidget;
}

//==============================================================================

class QSplitter;
class QStandardItemModel;
class QTextEdit;

//==============================================================================

namespace OpenCOR {

//==============================================================================

namespace Core {
    class TreeView;
}   // namespace Core

//==============================================================================

namespace CellMLAnnotationView {

//==============================================================================

class CellmlItemDelegate : public QStyledItemDelegate
{
public:
    explicit CellmlItemDelegate(QWidget *pParent,
                                QStandardItemModel *pDataModel);

    virtual void paint(QPainter *pPainter, const QStyleOptionViewItem &pOption,
                       const QModelIndex &pIndex) const;

private:
    QStandardItemModel *mCellmlDataModel;
};

//==============================================================================

class CellmlElementItem : public QStandardItem
{
public:
    enum Type
    {
        None             = QStandardItem::UserType,
        Error            = QStandardItem::UserType+1,
        Warning          = QStandardItem::UserType+2,
        Model            = QStandardItem::UserType+3,
        Import           = QStandardItem::UserType+4,
        ImportUnit       = QStandardItem::UserType+5,
        ImportComponent  = QStandardItem::UserType+6,
        Unit             = QStandardItem::UserType+7,
        UnitElement      = QStandardItem::UserType+8,
        Component        = QStandardItem::UserType+9,
        Variable         = QStandardItem::UserType+10,
        MathmlElement    = QStandardItem::UserType+11,
        Group            = QStandardItem::UserType+12,
        RelationshipRef  = QStandardItem::UserType+13,
        ComponentRef     = QStandardItem::UserType+14,
        Connection       = QStandardItem::UserType+15,
        ComponentMapping = QStandardItem::UserType+16,
        VariableMapping  = QStandardItem::UserType+17,
        Metadata         = QStandardItem::UserType+18
    };

    explicit CellmlElementItem(const bool &pError, const QString &pText);
    explicit CellmlElementItem(const Type &pType, const QString &pText);
    explicit CellmlElementItem(const Type &pType,
                               CellMLSupport::CellmlFileElement *pElement,
                               const QString &pText = QString());
    explicit CellmlElementItem(CellMLSupport::CellmlFileMathmlElement *pMathmlElement,
                               const QString &pText);
    explicit CellmlElementItem(const QString &pText);

    bool isCategory() const;
    virtual int type() const;

    CellMLSupport::CellmlFileElement * element() const;
    CellMLSupport::CellmlFileMathmlElement * mathmlElement() const;

private:
    bool mCategory;
    Type mType;

    CellMLSupport::CellmlFileElement *mElement;
    CellMLSupport::CellmlFileMathmlElement *mMathmlElement;

    void setIcon(const Type &pType);
};

//==============================================================================

class CellmlAnnotationViewDetailsWidget;

//==============================================================================

class CellmlAnnotationViewWidget : public Core::Widget
{
    Q_OBJECT

public:
    explicit CellmlAnnotationViewWidget(QWidget *pParent,
                                        const QString &pFileName);
    ~CellmlAnnotationViewWidget();

    virtual void retranslateUi();

    QList<int> horizontalSplitterSizes() const;
    QList<int> verticalSplitterSizes() const;

private:
    Ui::CellmlAnnotationViewWidget *mGui;

    QSplitter *mHorizontalSplitter;
    QSplitter *mVerticalSplitter;

    Core::TreeView *mCellmlTreeView;
    QStandardItemModel *mCellmlDataModel;
    CellmlItemDelegate *mCellmlItemDelegate;

    Core::TreeView *mMetadataTreeView;
    QStandardItemModel *mMetadataDataModel;

    CellmlAnnotationViewDetailsWidget *mDetails;

    void retranslateCellmlDataItem(CellmlElementItem *pCellmlElementItem);

    void initTreeView(Core::TreeView *pTreeView);

    void populateCellmlDataModel(const QString &pFileName);
    void populateUnitsDataModel(CellmlElementItem *pCellmlElementItem,
                                const CellMLSupport::CellmlFileUnits pUnits);
    void populateComponentRefDataModel(CellmlElementItem *pCellmlElementItem,
                                       CellMLSupport::CellmlFileComponentRef *pComponentRef);

    void populateMetadataDataModel(const QString &pFileName);

Q_SIGNALS:
    void horizontalSplitterMoved(const QList<int> &pHorizontalSizes);
    void verticalSplitterMoved(const QList<int> &pVerticalSizes);

public Q_SLOTS:
    void updateHorizontalSplitter(const QList<int> &pHorizontalSizes);
    void updateVerticalSplitter(const QList<int> &pVerticalSizes);

private Q_SLOTS:
    void emitHorizontalSplitterMoved();
    void emitVerticalSplitterMoved();

    void resizeCellmlTreeViewToContents();

    void updateCellmlNode(const QModelIndex &pNewIndex,
                          const QModelIndex &pOldIndex);
    void updateMetadataNode(const QModelIndex &pNewIndex,
                            const QModelIndex &pOldIndex);
};

//==============================================================================

}   // namespace CellMLAnnotationView
}   // namespace OpenCOR

//==============================================================================

#endif

//==============================================================================
// End of file
//==============================================================================

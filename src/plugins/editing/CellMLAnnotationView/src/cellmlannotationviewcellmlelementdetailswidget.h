//==============================================================================
// CellML annotation view CellML element details widget
//==============================================================================

#ifndef CELLMLANNOTATIONVIEWCELLMLELEMENTDETAILSWIDGET_H
#define CELLMLANNOTATIONVIEWCELLMLELEMENTDETAILSWIDGET_H

//==============================================================================

#include "cellmlfile.h"
#include "commonwidget.h"

//==============================================================================

#include <QScrollArea>

//==============================================================================

namespace Ui {
    class CellmlAnnotationViewCellmlElementDetailsWidget;
}

//==============================================================================

class QFormLayout;
class QComboBox;

//==============================================================================

namespace OpenCOR {
namespace CellMLAnnotationView {

//==============================================================================

class CellmlAnnotationViewWidget;

//==============================================================================

class CellmlAnnotationViewCellmlElementDetailsWidget : public QScrollArea,
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

    struct Item
    {
        Type type;
        CellMLSupport::CellmlFileElement *element;
        int number;

        bool operator==(const Item &pItem) const;
    };

    typedef QList<Item> Items;

    explicit CellmlAnnotationViewCellmlElementDetailsWidget(CellmlAnnotationViewWidget *pParent,
                                                            CellMLSupport::CellmlFile *pCellmlFile);
    ~CellmlAnnotationViewCellmlElementDetailsWidget();

    virtual void retranslateUi();

    static Item item(const Type &pType,
                     CellMLSupport::CellmlFileElement *pElement,
                     const int &pNumber = -1);

    void updateGui(const Items &pItems);
    void updateGui();

    QComboBox * cmetaIdValue() const;

private:
    CellmlAnnotationViewWidget *mParent;
    CellMLSupport::CellmlFile *mCellmlFile;

    Ui::CellmlAnnotationViewCellmlElementDetailsWidget *mGui;

    Items mItems;

    QWidget *mWidget;
    QFormLayout *mLayout;
    QComboBox *mCmetaIdValue;

    void addRow(const QString &pLabel, const QString &pValue);

    QString typeAsString(const Type &pType) const;

Q_SIGNALS:
    void cellmlElementMetadataDetailsRequested(const CellMLSupport::CellmlFileRdfTriples &pRdfTriples);
    void metadataEditingRequested(const QString &pCmetaId);

private Q_SLOTS:
    void newCmetaId(const QString &pCmetaId);
    void trackCmetaId(const QString &pCmetaId);

    void editMetadata();
};

//==============================================================================

}   // namespace CellMLAnnotationView
}   // namespace OpenCOR

//==============================================================================

#endif

//==============================================================================
// End of file
//==============================================================================

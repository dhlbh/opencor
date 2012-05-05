//==============================================================================
// CellML annotation view details widget
//==============================================================================

#ifndef CELLMLANNOTATIONVIEWDETAILSWIDGET_H
#define CELLMLANNOTATIONVIEWDETAILSWIDGET_H

//==============================================================================

#include "cellmlfile.h"
#include "widget.h"

//==============================================================================

#include <QStandardItem>
#include <QStyledItemDelegate>

//==============================================================================

namespace Ui {
    class CellmlAnnotationViewDetailsWidget;
}

//==============================================================================

namespace OpenCOR {
namespace CellMLAnnotationView {

//==============================================================================

class CellmlAnnotationViewDetailsWidget : public Core::Widget
{
    Q_OBJECT

public:
    enum Type
    {
        Empty,
        Model,
        Import,
        Unit,
        UnitElement,
        Component,
        Variable,
        MathmlElement,
        Group,
        RelationshipRef,
        ComponentRef,
        Connection,
        ComponentMapping,
        VariableMapping,
        Metadata
    };

    explicit CellmlAnnotationViewDetailsWidget(QWidget *pParent);
    ~CellmlAnnotationViewDetailsWidget();

    virtual void retranslateUi();

    void update(const Type &pType,
                CellMLSupport::CellmlFileElement *pElement = 0);
    void update(const Type &pType,
                CellMLSupport::CellmlFileMathmlElement *pMathmlElement);

private:
    Ui::CellmlAnnotationViewDetailsWidget *mUi;

    Type mType;

    CellMLSupport::CellmlFileElement *mElement;
    CellMLSupport::CellmlFileMathmlElement *mMathmlElement;

    void updateUi(const Type &pType,
                  CellMLSupport::CellmlFileElement *pElement,
                  CellMLSupport::CellmlFileMathmlElement *pMathmlElement);
};

//==============================================================================

}   // namespace CellMLAnnotationView
}   // namespace OpenCOR

//==============================================================================

#endif

//==============================================================================
// End of file
//==============================================================================

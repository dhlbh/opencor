//==============================================================================
// CellML annotation view CellML details widget
//==============================================================================

#ifndef CELLMLANNOTATIONVIEWCELLMLDETAILSWIDGET_H
#define CELLMLANNOTATIONVIEWCELLMLDETAILSWIDGET_H

//==============================================================================

#include "cellmlannotationviewcellmlelementdetailswidget.h"
#include "commonwidget.h"

//==============================================================================

namespace Ui {
    class CellmlAnnotationViewCellmlDetailsWidget;
}

//==============================================================================

#include <QSplitter>

//==============================================================================

namespace OpenCOR {
namespace CellMLAnnotationView {

//==============================================================================

class CellmlAnnotationViewMetadataViewDetailsWidget;
class CellmlAnnotationViewWidget;

//==============================================================================

class CellmlAnnotationViewCellmlDetailsWidget : public QSplitter,
                                                public Core::CommonWidget
{
    Q_OBJECT

public:
    explicit CellmlAnnotationViewCellmlDetailsWidget(CellmlAnnotationViewWidget *pParent);
    ~CellmlAnnotationViewCellmlDetailsWidget();

    virtual void retranslateUi();

    virtual QWidget * focusProxyWidget() const;

    void updateGui(const CellmlAnnotationViewCellmlElementDetailsWidget::Items &pItems);
    void finalizeGui();

private:
    CellmlAnnotationViewWidget *mParent;

    Ui::CellmlAnnotationViewCellmlDetailsWidget *mGui;

    CellmlAnnotationViewCellmlElementDetailsWidget *mCellmlElementDetails;
    CellmlAnnotationViewMetadataViewDetailsWidget *mMetadataViewDetails;

Q_SIGNALS:
    void splitterMoved(const QList<int> &pSizes);

public Q_SLOTS:
    void updateSplitter(const QList<int> &pSizes);

private Q_SLOTS:
    void emitSplitterMoved();
};

//==============================================================================

}   // namespace CellMLAnnotationView
}   // namespace OpenCOR

//==============================================================================

#endif

//==============================================================================
// End of file
//==============================================================================

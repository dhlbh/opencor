//==============================================================================
// CellML annotation view details widget
//==============================================================================

#include "cellmlannotationviewdetailswidget.h"

//==============================================================================

#include "ui_cellmlannotationviewdetailswidget.h"

//==============================================================================

#include <QLabel>
#include <QLineEdit>

//==============================================================================

namespace OpenCOR {
namespace CellMLAnnotationView {

//==============================================================================

CellmlAnnotationViewDetailsWidget::CellmlAnnotationViewDetailsWidget(QWidget *pParent) :
    Widget(pParent),
    mGui(new Ui::CellmlAnnotationViewDetailsWidget),
    mType(Empty)
{
    // Set up the GUI

    mGui->setupUi(this);
}

//==============================================================================

CellmlAnnotationViewDetailsWidget::~CellmlAnnotationViewDetailsWidget()
{
    // Delete the GUI

    delete mGui;
}

//==============================================================================

void CellmlAnnotationViewDetailsWidget::retranslateUi()
{
    // Retranslate our GUI

    mGui->retranslateUi(this);

    // Update the GUI (since some labels get reinitialised as a result of the
    // retranslation)

    updateGui(mType, mElement, mMathmlElement);
}

//==============================================================================

void CellmlAnnotationViewDetailsWidget::update(const Type &pType,
                                               CellMLSupport::CellmlFileElement *pElement)
{
    Q_ASSERT(   (((pType == Empty) || (pType == Metadata)) && !pElement)
             || ((pType != Empty) && (pType != Metadata) && pElement));

    // Update the GUI

    updateGui(pType, pElement, 0);
}

//==============================================================================

void CellmlAnnotationViewDetailsWidget::update(const Type &pType,
                                               CellMLSupport::CellmlFileMathmlElement *pMathmlElement)
{
    // Update the GUI

    updateGui(pType, 0, pMathmlElement);
}

//==============================================================================

void CellmlAnnotationViewDetailsWidget::updateGui(const Type &pType,
                                                  CellMLSupport::CellmlFileElement *pElement,
                                                  CellMLSupport::CellmlFileMathmlElement *pMathmlElement)
{
    // Keep track of the new type and elements

    bool needUpdatingGui = mType != pType;

    mType = pType;

    mElement = pElement;
    mMathmlElement = pMathmlElement;

    // Determine which widget should be shown/hidden

    bool showCmetaId = false;
    bool showName = false;
    bool showUri = false;
    bool showRelationshipRef = false;
    bool showComponentRef = false;

    switch (pType) {
    case Model:
        showCmetaId = true;
        showName = true;

        break;
    case Import:
        showCmetaId = true;
        showUri = true;

        break;
    case Unit:
        showCmetaId = true;
        showName = true;

        break;
    case UnitElement:
        showCmetaId = true;
        showName = true;

        break;
    case Component:
        showCmetaId = true;
        showName = true;

        break;
    case Variable:
        showCmetaId = true;
        showName = true;

        break;
    case MathmlElement:
        break;
    case Group:
        showCmetaId = true;

        break;
    case RelationshipRef:
        showCmetaId = true;
        showRelationshipRef = true;

        break;
    case ComponentRef:
        showCmetaId = true;
        showComponentRef = true;

        break;
    case Connection:
        showCmetaId = true;

        break;
    case ComponentMapping:
        showCmetaId = true;

        break;
    case VariableMapping:
        showCmetaId = true;

        break;
    case Metadata:
        showCmetaId = true;

        break;
    default:
        // Empty

        break;
    };

    // Show/hide the relevant widgets, but only if required

    if (needUpdatingGui) {
        // Remove everything from our form layout

        for (int i = 0, iMax = mGui->formLayout->count(); i < iMax; ++i) {
            QLayoutItem *item = mGui->formLayout->takeAt(0);

            delete item->widget();
            delete item;
        }

        // Add whatever we need
        // Note: as long as all of the widgets' parent is ourselves, then they
        //       will get automatically deleted, so no need to delete them in
        //       CellmlAnnotationViewDetailsWidget's destructor...

        if (showCmetaId) {
            mCmetaIdLabel = new QLabel(tr("cmeta:id:"), this);
            mCmetaIdValue = new QLineEdit(this);

            mGui->formLayout->addRow(mCmetaIdLabel, mCmetaIdValue);
        } else {
            mCmetaIdLabel = 0;
            mCmetaIdValue = 0;
        }

        if (showName) {
            mNameLabel = new QLabel(tr("Name:"), this);
            mNameValue = new QLabel(this);

            mGui->formLayout->addRow(mNameLabel, mNameValue);
        } else {
            mNameLabel = 0;
            mNameValue = 0;
        }

        if (showUri) {
            mUriLabel = new QLabel(tr("URI:"), this);
            mUriValue = new QLabel(this);

            mGui->formLayout->addRow(mUriLabel, mUriValue);
        } else {
            mUriLabel = 0;
            mUriValue = 0;
        }

        if (showRelationshipRef) {
            mRelationshipRefLabel = new QLabel(tr("Relationship reference:"), this);
            mRelationshipRefValue = new QLabel(this);

            mGui->formLayout->addRow(mRelationshipRefLabel, mRelationshipRefValue);
        } else {
            mRelationshipRefLabel = 0;
            mRelationshipRefValue = 0;
        }

        if (showComponentRef) {
            mComponentRefLabel = new QLabel(tr("Component reference:"), this);
            mComponentRefValue = new QLabel(this);

            mGui->formLayout->addRow(mComponentRefLabel, mComponentRefValue);
        } else {
            mComponentRefLabel = 0;
            mComponentRefValue = 0;
        }
    }

    // Update the value of the widgets which are shown

    if (showCmetaId)
        mCmetaIdValue->setText(pElement->cmetaId());

    if (showName)
        mNameValue->setText(static_cast<CellMLSupport::CellmlFileNamedElement *>(pElement)->name());

    if (showUri)
        mUriValue->setText(static_cast<CellMLSupport::CellmlFileImport *>(pElement)->uri());

    if (showRelationshipRef)
        mRelationshipRefValue->setText(static_cast<CellMLSupport::CellmlFileRelationshipRef *>(pElement)->relationship());

    if (showComponentRef)
        mComponentRefValue->setText(static_cast<CellMLSupport::CellmlFileComponentRef *>(pElement)->component());
}

//==============================================================================

}   // namespace CellMLAnnotationView
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================

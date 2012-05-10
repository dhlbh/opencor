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

    updateGui(mType, mElement, true);
}

//==============================================================================

void CellmlAnnotationViewDetailsWidget::updateGui(const Type &pType,
                                                  CellMLSupport::CellmlFileElement *pElement,
                                                  const bool &pNeedRetranslating)
{
    Q_ASSERT(   (((pType == Empty) || (pType == Metadata)) && !pElement)
             || ((pType != Empty) && (pType != Metadata) && pElement));

    // Keep track of the new type and element

    mType = pType;
    mElement = pElement;

    // Determine which widget should be shown/hidden

    bool showCmetaId = (pType != Metadata) && (pType != Empty);
    bool showName = false;
    bool showXlinkHref = false;
    bool showUnitReference = false;
    bool showComponentReference = false;
    bool showUnit = false;
    bool showInitialValue = false;
    bool showPublicInterface = false;
    bool showPrivateInterface = false;
    bool showRelationship = false;
    bool showRelationshipNamespace = false;
    bool showComponent = false;
    bool showFirstComponent = false;
    bool showSecondComponent = false;
    bool showFirstVariable = false;
    bool showSecondVariable = false;

    switch (pType) {
    case Model:
        showName = true;

        break;
    case Import:
        showXlinkHref = true;

        break;
    case ImportUnit:
        showName = true;
        showUnitReference = true;

        break;
    case ImportComponent:
        showName = true;
        showComponentReference = true;

        break;
    case Unit:
        showName = true;

        break;
    case UnitElement:
        showName = true;

        break;
    case Component:
        showName = true;

        break;
    case Variable:
        showName = true;
        showUnit = true;
        showInitialValue = true;
        showPublicInterface = true;
        showPrivateInterface = true;

        break;
    case Group:
        break;
    case RelationshipReference:
        showRelationship = true;
        showRelationshipNamespace = true;

        break;
    case ComponentReference:
        showComponent = true;

        break;
    case Connection:
        break;
    case ComponentMapping:
        showFirstComponent = true;
        showSecondComponent = true;

        break;
    case VariableMapping:
        showFirstVariable = true;
        showSecondVariable = true;

        break;
    case Metadata:
        break;
    default:
        // Empty, so...

        ;
    };

    // Hide ourselves (since we may potentially update ourselves quite a bit and
    // we want to avoid any flickering)
    // Note #1: one would normally use setUpdatesEnabled(), but it still results
    //          in bad flickering on Mac OS X, so...
    // Note #2: it's surprising that setVisible() doesn't cause any flickering
    //          on any of the platforms we are targetting, but let's not
    //          complain...

    setVisible(false);

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

    if (showCmetaId)
        mGui->formLayout->addRow(new QLabel(tr("cmeta:id:"), this),
                                 new QLineEdit(pElement->cmetaId(), this));

    if (showName)
        addRowToFormLayout(tr("Name:"),
                           static_cast<CellMLSupport::CellmlFileNamedElement *>(pElement)->name());

    if (showXlinkHref)
        addRowToFormLayout(tr("xlink:href:"),
                           static_cast<CellMLSupport::CellmlFileImport *>(pElement)->xlinkHref());

    if (showUnitReference)
        addRowToFormLayout(tr("Unit reference:"),
                           static_cast<CellMLSupport::CellmlFileImportUnit *>(pElement)->unitReference());

    if (showComponentReference)
        addRowToFormLayout(tr("Component reference:"),
                           static_cast<CellMLSupport::CellmlFileImportComponent *>(pElement)->componentReference());

    if (showUnit)
        addRowToFormLayout(tr("Unit:"),
                           static_cast<CellMLSupport::CellmlFileVariable *>(pElement)->unit());

    if (showInitialValue) {
        QString initialValue = static_cast<CellMLSupport::CellmlFileVariable *>(pElement)->initialValue();

        addRowToFormLayout(tr("Initial value:"),
                           initialValue.isEmpty()?"/":initialValue);
    }

    if (showPublicInterface)
        addRowToFormLayout(tr("Public interface:"),
                           static_cast<CellMLSupport::CellmlFileVariable *>(pElement)->publicInterfaceAsString());

    if (showPrivateInterface)
        addRowToFormLayout(tr("Private interface:"),
                           static_cast<CellMLSupport::CellmlFileVariable *>(pElement)->privateInterfaceAsString());

    if (showRelationship)
        addRowToFormLayout(tr("Relationship:"),
                           static_cast<CellMLSupport::CellmlFileRelationshipReference *>(pElement)->relationship());

    if (showRelationshipNamespace) {
        QString relationshipNamespace = static_cast<CellMLSupport::CellmlFileRelationshipReference *>(pElement)->relationshipNamespace();

        addRowToFormLayout(tr("Relationship namespace:"),
                           relationshipNamespace.isEmpty()?"/":relationshipNamespace);
    }

    if (showComponent)
        addRowToFormLayout(tr("Component:"),
                           static_cast<CellMLSupport::CellmlFileComponentReference *>(pElement)->component());

    if (showFirstComponent)
        addRowToFormLayout(tr("First component:"),
                           static_cast<CellMLSupport::CellmlFileMapComponents *>(pElement)->firstComponent());

    if (showSecondComponent)
        addRowToFormLayout(tr("Second component:"),
                           static_cast<CellMLSupport::CellmlFileMapComponents *>(pElement)->secondComponent());

    if (showFirstVariable)
        addRowToFormLayout(tr("First variable:"),
                           static_cast<CellMLSupport::CellmlFileMapVariablesItem *>(pElement)->firstVariable());

    if (showSecondVariable)
        addRowToFormLayout(tr("Second variable:"),
                           static_cast<CellMLSupport::CellmlFileMapVariablesItem *>(pElement)->secondVariable());

    // Re-show ourselves

    setVisible(true);
}

//==============================================================================

void CellmlAnnotationViewDetailsWidget::addRowToFormLayout(const QString &pLabel,
                                                           const QString &pValue)
{
    // Add a row to our form layout

    mGui->formLayout->addRow(new QLabel(pLabel, this),
                             new QLabel(pValue, this));
}

//==============================================================================

}   // namespace CellMLAnnotationView
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================

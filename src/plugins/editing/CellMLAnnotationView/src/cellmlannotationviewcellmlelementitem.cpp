//==============================================================================
// CellML element item
//==============================================================================

#include "cellmlannotationviewcellmlelementitem.h"
#include "cellmlfileimport.h"

//==============================================================================

namespace OpenCOR {
namespace CellMLAnnotationView {

//==============================================================================

CellmlElementItem::CellmlElementItem(const bool &pError, const QString &pText) :
    QStandardItem(pText),
    mCategory(false),
    mType(pError?Error:Warning),
    mElement(0)
{
    // Constructor for either an error or a warning

    // Disable the item and use its text as a tooltip (in case it's too long and
    // therefore doesn't fit within the allocated space we have)
    // Note: the item will get 're-enabled' by our item delegate...

    setEnabled(false);
    setToolTip(pText);

    // Set the icon for the item

    setIcon(mType);
}

//==============================================================================

CellmlElementItem::CellmlElementItem(const Type &pType,
                                     const QString &pText) :
    QStandardItem(pText),
    mCategory(true),
    mType(pType),
    mElement(0)
{
    // Constructor for a category

    // Disable the item
    // Note: the item will get 're-enabled' by our item delegate...

    setEnabled(false);

    // Set the icon for the item

    setIcon(pType);
}

//==============================================================================

CellmlElementItem::CellmlElementItem(const Type &pType,
                                     CellMLSupport::CellmlFileElement *pElement,
                                     const QString &pText) :
    QStandardItem(pText),
    mCategory(false),
    mType(pType),
    mElement(pElement)
{
    static const QChar rightArrow = QChar(0x2192);

    // Set the text for some types

    switch (pType) {
    case Import:
        setText(static_cast<CellMLSupport::CellmlFileImport *>(pElement)->xlinkHref());

        break;
    case RelationshipReference:
        setText(static_cast<CellMLSupport::CellmlFileRelationshipReference *>(pElement)->relationship());

        break;
    case ComponentReference:
        setText(static_cast<CellMLSupport::CellmlFileComponentReference *>(pElement)->component());

        break;
    case ComponentMapping: {
        CellMLSupport::CellmlFileMapComponents *mapComponents = static_cast<CellMLSupport::CellmlFileMapComponents *>(pElement);

        setText(QString("%1 %2 %3").arg(mapComponents->firstComponent(),
                                        rightArrow,
                                        mapComponents->secondComponent()));

        break;
    }
    case VariableMapping: {
        CellMLSupport::CellmlFileMapVariablesItem *mapVariables = static_cast<CellMLSupport::CellmlFileMapVariablesItem *>(pElement);

        setText(QString("%1 %2 %3").arg(mapVariables->firstVariable(),
                                        rightArrow,
                                        mapVariables->secondVariable()));

        break;
    }
    default:
        // Another type of element, so either pText was used or it has a name
        // which we can use

        if (pText.isEmpty())
            setText(static_cast<CellMLSupport::CellmlFileNamedElement *>(pElement)->name());
    }

    // Set the icon for the item

    setIcon(pType);
}

//==============================================================================

void CellmlElementItem::setIcon(const Type &pType)
{
    // Determine the icon to be used for the item

    switch (pType) {
    case Error:
        QStandardItem::setIcon(QIcon(":CellMLSupport_errorNode"));

        break;
    case Warning:
        QStandardItem::setIcon(QIcon(":CellMLSupport_warningNode"));

        break;
    case Model:
        QStandardItem::setIcon(QIcon(":CellMLSupport_modelNode"));

        break;
    case Import:
        QStandardItem::setIcon(QIcon(":CellMLSupport_importNode"));

        break;
    case ImportUnit:
    case Unit:
        QStandardItem::setIcon(QIcon(":CellMLSupport_unitNode"));

        break;
    case UnitElement:
        QStandardItem::setIcon(QIcon(":CellMLSupport_unitElementNode"));

        break;
    case ImportComponent:
    case Component:
    case ComponentMapping:
        QStandardItem::setIcon(QIcon(":CellMLSupport_componentNode"));

        break;
    case Variable:
    case VariableMapping:
        QStandardItem::setIcon(QIcon(":CellMLSupport_variableNode"));

        break;
    case Group:
        QStandardItem::setIcon(QIcon(":CellMLSupport_groupNode"));

        break;
    case RelationshipReference:
        QStandardItem::setIcon(QIcon(":CellMLSupport_relationshipReferenceNode"));

        break;
    case ComponentReference:
        QStandardItem::setIcon(QIcon(":CellMLSupport_componentReferenceNode"));

        break;
    case Connection:
        QStandardItem::setIcon(QIcon(":CellMLSupport_connectionNode"));

        break;
    default:
        // Type which doesn't require an icon, so...

        ;
    }
}

//==============================================================================

bool CellmlElementItem::isCategory() const
{
    // Return wether the CellML element item is a category or not

    return mCategory;
}

//==============================================================================

int CellmlElementItem::type() const
{
    // Return the CellML element item's type

    return mType;
}

//==============================================================================

CellMLSupport::CellmlFileElement * CellmlElementItem::element() const
{
    // Return the CellML element item's element

    return mElement;
}

//==============================================================================

}   // namespace CellMLAnnotationView
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================

//==============================================================================
// CellML annotation view widget
//==============================================================================

#include "borderedwidget.h"
#include "cellmlannotationviewwidget.h"
#include "cellmlfilemanager.h"
#include "coreutils.h"
#include "treeview.h"

//==============================================================================

#include "ui_cellmlannotationviewwidget.h"

//==============================================================================

#include <QSplitter>
#include <QTextEdit>

//==============================================================================

namespace OpenCOR {
namespace CellMLAnnotationView {

//==============================================================================

CellmlAnnotationViewWidget::CellmlAnnotationViewWidget(QWidget *pParent,
                                                       const QString &pFileName) :
    Widget(pParent),
    mUi(new Ui::CellmlAnnotationViewWidget)
{
    // Set up the UI

    mUi->setupUi(this);

    // Create our horizontal splitter

    mHorizontalSplitter = new QSplitter(Qt::Horizontal, this);

    // Create and customise a tree view which will contain all of the units,
    // components, groups and connections from the CellML file

    mTreeView = new Core::TreeView(this);

    mTreeView->setFrameShape(QFrame::NoFrame);
    mTreeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mTreeView->header()->setVisible(false);

    // Populate our horizontal splitter with the aforementioned tree view, as
    // well as with a dummy (for now) widget
    //---GRY--- THE DUMMY WIDGET WILL EVENTUALLY GET REPLACED BY SOMETHING THAT
    //          WILL ALLOW THE USER TO EDIT METADATA, MAKE USE OF RICORDO, ETC.

    mHorizontalSplitter->addWidget(new Core::BorderedWidget(mTreeView,
                                                            Core::BorderedWidget::Right));
    mHorizontalSplitter->addWidget(new Core::BorderedWidget(new QWidget(this),
                                                            Core::BorderedWidget::Left));

    mUi->verticalLayout->addWidget(mHorizontalSplitter);

    // Keep track of our horizontal splitter being moved

    connect(mHorizontalSplitter, SIGNAL(splitterMoved(int,int)),
            this, SLOT(emitHorizontalSplitterMoved()));

    // Create a (temporary) debug output
    //---GRY--- THIS WIDGET IS ONLY FOR US WHILE WE ARE WORKING ON THIS
    //          PLUGIN...

    mDebugOutput = new QTextEdit(this);

    mDebugOutput->setFrameStyle(QFrame::NoFrame);
    mDebugOutput->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    mUi->verticalLayout->addWidget(Core::newLineWidget(this));
    mUi->verticalLayout->addWidget(mDebugOutput);

    // Initialise our tree view

    initTreeView(pFileName);
}

//==============================================================================

QList<int> CellmlAnnotationViewWidget::horizontalSplitterSizes() const
{
    // Return our horizontal splitter's sizes

    return mHorizontalSplitter->sizes();
}

//==============================================================================

void CellmlAnnotationViewWidget::initTreeView(const QString &pFileName)
{
    // Initialise our tree view with the units, components, groups and
    // connections from the CellML file

    // Retrieve our CellML file object and load the CellML file

    CellMLSupport::CellmlFile *cellmlFile = CellMLSupport::CellmlFileManager::instance()->cellmlFile(pFileName);

    cellmlFile->load();

    mDebugOutput->append(QString("File name: %1").arg(pFileName));

    if (cellmlFile->issues().count()) {
        // Something went wrong while trying to load the CellML file, so report
        // it and leave

        mDebugOutput->append(QString("    [Error] %1").arg(cellmlFile->issues().first().formattedMessage()));

        return;
    }

    // Output the name of the CellML model

    mDebugOutput->append(QString("    Model: %1 [%2]").arg(cellmlFile->model()->name(),
                                                                cellmlFile->model()->cmetaId()));

    // Retrieve the model's imports

    if (cellmlFile->imports().isEmpty()) {
        mDebugOutput->append(QString("    No imports"));
    } else {
        mDebugOutput->append(QString("    Imports:"));

        foreach (CellMLSupport::CellmlFileImport *import,
                 cellmlFile->imports()) {
            mDebugOutput->append(QString("        From %1 [%2]:").arg(import->uri(),
                                                                      import->cmetaId()));

            if (import->units().isEmpty()) {
                mDebugOutput->append(QString("            No units"));
            } else {
                mDebugOutput->append(QString("            Units:"));

                foreach (CellMLSupport::CellmlFileImportUnit *unit,
                         import->units())
                    mDebugOutput->append(QString("                Unit: %1 ---> %2 [%3]").arg(unit->name(), unit->referenceName(), unit->cmetaId()));
            }

            if (import->components().isEmpty()) {
                mDebugOutput->append(QString("            No components"));
            } else {
                mDebugOutput->append(QString("            Components:"));

                foreach (CellMLSupport::CellmlFileImportComponent *component,
                         import->components())
                    mDebugOutput->append(QString("                Component: %1 ---> %2 [%3]").arg(component->name(), component->referenceName(), component->cmetaId()));
            }
        }
    }

    // Retrieve the model's unit definitions

    initUnitsTreeView("    ", cellmlFile->units());

    // Retrieve the model's components

    if (cellmlFile->components().isEmpty()) {
        mDebugOutput->append(QString("    No components"));
    } else {
        mDebugOutput->append(QString("    Components:"));

        foreach (CellMLSupport::CellmlFileComponent *component,
                 cellmlFile->components()) {
            mDebugOutput->append(QString("        %1 [%2]").arg(component->name(),
                                                                component->cmetaId()));

            initUnitsTreeView("            ", component->units());

            if (component->variables().isEmpty()) {
                mDebugOutput->append(QString("            No variables"));
            } else {
                mDebugOutput->append(QString("            Variables:"));

                foreach (CellMLSupport::CellmlFileVariable *variable,
                         component->variables()) {
                    QString variablePublicInterface = (variable->publicInterface() == CellMLSupport::CellmlFileVariable::In)?
                                                          "in":
                                                          (variable->publicInterface() == CellMLSupport::CellmlFileVariable::Out)?
                                                              "out":
                                                              "none";
                    QString variablePrivateInterface = (variable->privateInterface() == CellMLSupport::CellmlFileVariable::In)?
                                                          "in":
                                                          (variable->privateInterface() == CellMLSupport::CellmlFileVariable::Out)?
                                                              "out":
                                                              "none";

                    mDebugOutput->append(QString("                %1 | Unit: %2 | Initial value: %3 | Public interface: %4 | Private interface: %5 [%6]").arg(variable->name(),
                                                                                                                                                              variable->unit(),
                                                                                                                                                              variable->initialValue(),
                                                                                                                                                              variablePublicInterface,
                                                                                                                                                              variablePrivateInterface,
                                                                                                                                                              variable->cmetaId()));
                }
            }

            if (component->mathmlElements().isEmpty()) {
                mDebugOutput->append(QString("            No MathML elements"));
            } else {
                mDebugOutput->append(QString("            MathML elements:"));

                int counter = 0;

                foreach (CellMLSupport::CellmlFileMathmlElement *mathmlElement,
                         component->mathmlElements())
                    mDebugOutput->append(QString("                MathML element #%1").arg(QString::number(++counter)));
            }
        }
    }

    // Retrieve the model's groups

    if (cellmlFile->groups().isEmpty()) {
        mDebugOutput->append(QString("    No groups"));
    } else {
        mDebugOutput->append(QString("    Groups:"));

        foreach (CellMLSupport::CellmlFileGroup *group, cellmlFile->groups()) {
            mDebugOutput->append(QString("        Group [%1]").arg(group->cmetaId()));

            if (group->relationshipRefs().isEmpty()) {
                mDebugOutput->append(QString("            No relationship ref(erence)s"));
            } else {
                mDebugOutput->append(QString("            Relationship ref(erence)s:"));

                foreach (CellMLSupport::CellmlFileRelationshipRef *relationshipRef,
                         group->relationshipRefs()) {
                    mDebugOutput->append(QString("                %1 [%2]").arg(relationshipRef->name(),
                                                                                relationshipRef->cmetaId()));
                    mDebugOutput->append(QString("                    Relationship: %1").arg(relationshipRef->relationship()));
                    mDebugOutput->append(QString("                    Relationship namespace: %1").arg(relationshipRef->relationshipNamespace()));
                }
            }

            if (group->componentRefs().isEmpty()) {
                mDebugOutput->append(QString("            No component ref(erence)s"));
            } else {
                mDebugOutput->append(QString("            Component ref(erence)s:"));

                foreach (CellMLSupport::CellmlFileComponentRef *componentRef,
                         group->componentRefs())
                    initComponentRefTreeView("                ", componentRef);
            }
        }
    }

    // Retrieve the model's connections

    if (cellmlFile->connections().isEmpty()) {
        mDebugOutput->append(QString("    No connections"));
    } else {
        mDebugOutput->append(QString("    Connections:"));

        foreach (CellMLSupport::CellmlFileConnection *connection, cellmlFile->connections()) {
            mDebugOutput->append(QString("        Connection [%1]").arg(connection->cmetaId()));
            mDebugOutput->append(QString("            Components: %1 ---> %2 [%3]").arg(connection->componentMapping()->firstComponentName(),
                                                                                        connection->componentMapping()->secondComponentName(),
                                                                                        connection->componentMapping()->cmetaId()));

            if (connection->variableMappings().isEmpty()) {
                mDebugOutput->append(QString("            No variables"));
            } else {
                mDebugOutput->append(QString("            Variables:"));

                foreach (CellMLSupport::CellmlFileMapVariablesItem *mapVariablesItem,
                         connection->variableMappings())
                    mDebugOutput->append(QString("                %1 ---> %2 [%3]").arg(mapVariablesItem->firstVariableName(),
                                                                                        mapVariablesItem->secondVariableName(),
                                                                                        mapVariablesItem->cmetaId()));
            }
        }
    }
}

//==============================================================================

void CellmlAnnotationViewWidget::initUnitsTreeView(const QString &pLeadingSpace,
                                                   const CellMLSupport::CellmlFileUnits pUnits)
{
    if (pUnits.isEmpty()) {
        mDebugOutput->append(QString("%1No units").arg(pLeadingSpace));
    } else {
        mDebugOutput->append(QString("%1Units:").arg(pLeadingSpace));

        foreach (CellMLSupport::CellmlFileUnit *unit, pUnits) {
            mDebugOutput->append(QString("%1    %2 [%3]").arg(pLeadingSpace,
                                                              unit->name(),
                                                              unit->cmetaId()));
            mDebugOutput->append(QString("%1        Base unit: %2").arg(pLeadingSpace,
                                                                        unit->isBaseUnit()?"yes":"no"));

            if (unit->unitElements().isEmpty()) {
                mDebugOutput->append(QString("%1        No unit elements").arg(pLeadingSpace));
            } else {
                mDebugOutput->append(QString("%1        Unit elements:").arg(pLeadingSpace));

                foreach (CellMLSupport::CellmlFileUnitElement *unitElement,
                         unit->unitElements()) {
                    mDebugOutput->append(QString("%1            %2 | Prefix: %3 | Multiplier: %4 | Offset: %5 | Exponent: %6 [%7]").arg(pLeadingSpace,
                                                                                                                                        unitElement->name(),
                                                                                                                                        QString::number(unitElement->prefix()),
                                                                                                                                        QString::number(unitElement->multiplier()),
                                                                                                                                        QString::number(unitElement->offset()),
                                                                                                                                        QString::number(unitElement->exponent()),
                                                                                                                                        unitElement->cmetaId()));
                }
            }
        }
    }
}

//==============================================================================

void CellmlAnnotationViewWidget::initComponentRefTreeView(const QString &pLeadingSpace,
                                                          CellMLSupport::CellmlFileComponentRef *pComponentRef)
{
    mDebugOutput->append(QString("%1%2 [%3]").arg(pLeadingSpace,
                                                  pComponentRef->name(),
                                                  pComponentRef->cmetaId()));

    foreach (CellMLSupport::CellmlFileComponentRef *componentRef,
             pComponentRef->componentRefs())
        initComponentRefTreeView(pLeadingSpace+"    ", componentRef);
}

//==============================================================================

void CellmlAnnotationViewWidget::updateHorizontalSplitter(const QList<int> &pSizes)
{
    // Update our horizontal splitter after a the horizontal splitter of another
    // CellmlAnnotationViewWidget object has been moved

    mHorizontalSplitter->setSizes(pSizes);
}

//==============================================================================

void CellmlAnnotationViewWidget::emitHorizontalSplitterMoved()
{
    // Let whoever know that our horizontal splitter has been moved

    emit horizontalSplitterMoved(mHorizontalSplitter->sizes());
}

//==============================================================================

}   // namespace CellMLAnnotationView
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================

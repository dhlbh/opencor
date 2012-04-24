//==============================================================================
// CellML file group
//==============================================================================

#ifndef CELLMLFILEGROUP_H
#define CELLMLFILEGROUP_H

//==============================================================================

#include "cellmlfilecomponentref.h"
#include "cellmlfileelement.h"
#include "cellmlfilerelationshipref.h"
#include "cellmlsupportglobal.h"

//==============================================================================

namespace OpenCOR {
namespace CellMLSupport {

//==============================================================================

class CELLMLSUPPORT_EXPORT CellmlFileGroup : public CellmlFileElement
{
public:
    explicit CellmlFileGroup(iface::cellml_api::Group *pGroup);
    ~CellmlFileGroup();

    CellmlFileRelationshipRefs relationshipRefs() const;
    CellmlFileComponentRefs componentRefs() const;

private:
    CellmlFileRelationshipRefs mRelationshipRefs;
    CellmlFileComponentRefs mComponentRefs;
};

//==============================================================================

typedef QList<CellmlFileGroup *> CellmlFileGroups;

//==============================================================================

}   // namespace CellMLSupport
}   // namespace OpenCOR

//==============================================================================

#endif

//==============================================================================
// End of file
//==============================================================================
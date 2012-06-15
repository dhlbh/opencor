//==============================================================================
// CellML file RDF triple
//==============================================================================

#include "cellmlfilerdftriple.h"

//==============================================================================

#include <QStringList>

//==============================================================================

namespace OpenCOR {
namespace CellMLSupport {

//==============================================================================

CellmlFileRdfTriple::CellmlFileRdfTriple(iface::rdf_api::Triple *pRdfTriple) :
    mType(Unknown),
    mModelQualifier(ModelUnknown),
    mBioQualifier(BioUnknown),
    mResource(QString()),
    mId(QString())
{
    // Retrieve the RDF triple's subject, predicate and object information

    ObjRef<iface::rdf_api::Resource> subject = pRdfTriple->subject();
    ObjRef<iface::rdf_api::Resource> predicate = pRdfTriple->predicate();
    ObjRef<iface::rdf_api::Node> object = pRdfTriple->object();

    mSubject   = new CellmlFileRdfTripleElement(subject);
    mPredicate = new CellmlFileRdfTripleElement(predicate);
    mObject    = new CellmlFileRdfTripleElement(object);

    // Determine the type of the RDF triple

    // Note: at this stage, we only recognise model and bio(logy) qualifiers as
    //       described at http://biomodels.net/qualifiers/. This means that the
    //       predicate of the RDF triple must have one of the following two
    //       forms:
    //
    //          http://biomodels.net/model-qualifiers/<xxx>
    //          http://biomodels.net/biology-qualifiers/<yyy>
    //
    //       where <xxx> and <yyy> are one of the values in modelQualifiers and
    //       bioQualifiers below. The object of the RDF triple must have one of
    //       the following two formats:
    //
    //          urn:miriam:<resource>:<identifier>
    //          http://identifiers.org/<resource>/<identifier>
    //
    //       For example:
    //
    //          urn:miriam:uniprot:Q4KLA0
    //          http://identifiers.org/uniprot/Q4KLA0
    //
    //       The former is a MIRIAM URN while the latter is an identifiers.org
    //       URI. MIRIAM URNs are being deprecated in favour of identifiers.org
    //       URIs. Still, we should and do support both formats and keep track
    //       of their information in the form of a resource and an id...

    QStringList modelQualifiers = QStringList() << "is" << "isDerivedFrom"
                                                << "isDescribedBy";
    QStringList bioQualifiers = QStringList() << "encodes" << "hasPart"
                                              << "hasProperty" << "hasVersion"
                                              << "is" << "isDescribedBy"
                                              << "isEncodedBy" << "isHomologTo"
                                              << "isPartOf" << "isPropertyOf"
                                              << "isVersionOf" << "occursIn"
                                              << "hasTaxon";

    for (int i = 0, iMax = modelQualifiers.count(); i < iMax; ++i)
        if (!mPredicate->asString().compare(QString("http://biomodels.net/model-qualifiers/%1").arg(modelQualifiers[i]))) {
            // It looks like we might be dealing with a model qualifier

            mType = BioModelsDotNetQualifier;

            mModelQualifier = (ModelQualifier) (i+1);

            break;
        }

    if (mType == Unknown)
        for (int i = 0, iMax = bioQualifiers.count(); i < iMax; ++i)
            if (!mPredicate->asString().compare(QString("http://biomodels.net/biology-qualifiers/%1").arg(bioQualifiers[i]))){
                // It looks like we might be dealing with a model qualifier

                mType = BioModelsDotNetQualifier;

                mBioQualifier = (BioQualifier) (i+1);

                break;
            }

    if (mType == BioModelsDotNetQualifier) {
        // We seem to be dealing with either a model or a bio(logy) qualifier,
        // so check whether its object is either a valid MIRIAM URN or a valid
        // identifiers.org URI

        QString objectAsString = mObject->asString();

        if (QRegExp("^urn:miriam:[a-zA-Z0-9\\._%]+:[a-zA-Z0-9\\._%]+").exactMatch(objectAsString)) {
            // The object is a valid MIRIAM URN, so retrieve its corresponding
            // resource and id

            QStringList objectAsStringList = objectAsString.split(":");

            mResource = objectAsStringList[2].replace("%3A", ":");
            mId       = objectAsStringList[3].replace("%3A", ":");
        } else if (QRegExp("^http://identifiers.org/[a-zA-Z0-9\\._:]+/[a-zA-Z0-9\\._:]+").exactMatch(objectAsString)) {
            // The object is a valid identifiers.org URI, so retrieve its
            // corresponding resource and id

            QStringList objectAsStringList = objectAsString.remove("http://identifiers.org/").split("/");

            mResource = objectAsStringList[0];
            mId       = objectAsStringList[1];
        } else {
            // The object is neither a valid MIRIAM URN nor a valid
            // identifiers.org URI which means that the RDF triple is not a
            // valid model/bio(logy) qualifier, so...

            mType = Unknown;

            mModelQualifier = ModelUnknown;
            mBioQualifier   = BioUnknown;
        }
    }
}

//==============================================================================

CellmlFileRdfTriple::~CellmlFileRdfTriple()
{
    // Delete some internal objects

    delete mSubject;
    delete mPredicate;
    delete mObject;
}

//==============================================================================

CellmlFileRdfTripleElement * CellmlFileRdfTriple::subject() const
{
    // Return the RDF triple's subject

    return mSubject;
}

//==============================================================================

CellmlFileRdfTripleElement * CellmlFileRdfTriple::predicate() const
{
    // Return the RDF triple's predicate

    return mPredicate;
}

//==============================================================================

CellmlFileRdfTripleElement * CellmlFileRdfTriple::object() const
{
    // Return the RDF triple's object

    return mObject;
}

//==============================================================================

CellmlFileRdfTriple::Type CellmlFileRdfTriple::type() const
{
    // Return the RDF triple's type

    return mType;
}

//==============================================================================

CellmlFileRdfTriple::ModelQualifier CellmlFileRdfTriple::modelQualifier() const
{
    // Return the RDF triple's model qualifier

    return mModelQualifier;
}

//==============================================================================

QString CellmlFileRdfTriple::modelQualifierAsString() const
{
    // Return the RDF triple's model qualifier as a string

    switch (mModelQualifier) {
    case ModelIs:
        return "model:is";
    case ModelIsDerivedFrom:
        return "model:isDerivedFrom";
    case ModelIsDescribedBy:
        return "model:isDescribedBy";
    default:
        // ModelUnknown

        return "model:unknown";
    }
}

//==============================================================================

CellmlFileRdfTriple::BioQualifier CellmlFileRdfTriple::bioQualifier() const
{
    // Return the RDF triple's bio(logy) qualifier

    return mBioQualifier;
}

//==============================================================================

QString CellmlFileRdfTriple::bioQualifierAsString() const
{
    // Return the RDF triple's bio(logy) qualifier as a string

    switch (mBioQualifier) {
    case BioEncodes:
        return "bio:encodes";
    case BioHasPart:
        return "bio:hasPart";
    case BioHasProperty:
        return "bio:hasProperty";
    case BioHasVersion:
        return "bio:hasVersion";
    case BioIs:
        return "bio:is";
    case BioIsDescribedBy:
        return "bio:isDescribedBy";
    case BioIsEncodedBy:
        return "bio:isEncodedBy";
    case BioIsHomologTo:
        return "bio:isHomologTo";
    case BioIsPartOf:
        return "bio:isPartOf";
    case BioIsPropertyOf:
        return "bio:isPropertyOf";
    case BioIsVersionOf:
        return "bio:isVersionOf";
    case BioOccursIn:
        return "bio:occursIn";
    case BioHasTaxon:
        return "bio:hasTaxon";
    default:
        // BioUnknown

        return "bio:unknown";
    }
}

//==============================================================================

QString CellmlFileRdfTriple::resource() const
{
    // Return the RDF triple's resource

    return mResource;
}

//==============================================================================

QString CellmlFileRdfTriple::id() const
{
    // Return the RDF triple's id

    return mId;
}

//==============================================================================

CellmlFileRdfTriple::Type CellmlFileRdfTriples::type() const
{
    // Return the type of the RDF triples
    // Note: if the RDF triples are of different types each then the overall
    //       type is considered to be unknown

    if (isEmpty()) {
        // There are no RDF triples, so...

        return CellmlFileRdfTriple::Unknown;
    } else {
        // There is at least one RDF triple, so retrieve the subject and type of
        // the first RDF triple and consider its type as the default type for
        // all our RDF triples

        CellmlFileRdfTriple *crtRdfTriple = first();

        QString subject = crtRdfTriple->subject()->asString();
        CellmlFileRdfTriple::Type res = crtRdfTriple->type();

        // Go through the remaining RDF triples and make sure that their type
        // is consistent with that of the first one

        for (int i = 2, iMax = count(); i < iMax; ++i) {
            crtRdfTriple = at(i);

            if (   crtRdfTriple->subject()->asString().compare(subject)
                || (crtRdfTriple->type() != res))
                // The subject and/or the type of the current RDF triple is
                // different from that of the first RDF triple, so...

                return CellmlFileRdfTriple::Unknown;
        }

        // All of the RDF triples have the same type, so...

        return res;
    }
}

//==============================================================================

}   // namespace CellMLSupport
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================

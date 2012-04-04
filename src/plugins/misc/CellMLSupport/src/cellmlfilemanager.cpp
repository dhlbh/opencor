//==============================================================================
// CellML file manager
//==============================================================================

#include "cellmlfilemanager.h"
#include "cellmlsupportplugin.h"
#include "coreutils.h"
#include "filemanager.h"

//==============================================================================

#include <QDir>

//==============================================================================

namespace OpenCOR {
namespace CellMLSupport {

//==============================================================================

CellmlFileManager::CellmlFileManager() :
    mCellmlFiles(CellmlFiles())
{
    // Create some connections to keep track of some events related to our
    // 'global' file manager

    connect(Core::FileManager::instance(), SIGNAL(fileManaged(const QString &)),
            this, SLOT(fileManaged(const QString &)));
    connect(Core::FileManager::instance(), SIGNAL(fileUnmanaged(const QString &)),
            this, SLOT(fileUnmanaged(const QString &)));
}

//==============================================================================

CellmlFileManager::~CellmlFileManager()
{
    // Remove all the managed files

    foreach (CellmlFile *cellmlFile, mCellmlFiles)
        delete cellmlFile;
}

//==============================================================================

CellmlFileManager * CellmlFileManager::instance()
{
    // Return the 'global' instance of our CellML file manager class

    static CellmlFileManager instance;

    return (CellmlFileManager *) Core::instance("OpenCOR::CellMLSupport::CellmlFileManager", &instance);
}

//==============================================================================

CellmlFile * CellmlFileManager::cellmlFile(const QString &pFileName)
{
    // Return the CellmlFile object, if any, associated with the requested file

    return mCellmlFiles.value(Core::nativeCanonicalFileName(pFileName));
}

//==============================================================================

void CellmlFileManager::fileManaged(const QString &pFileName)
{
    QString nativeFileName = Core::nativeCanonicalFileName(pFileName);

    if (isCellmlFile(nativeFileName))
        // We are dealing with a CellML file, so we can add it to our list of
        // managed CellML files

        mCellmlFiles.insert(pFileName, new CellmlFile(nativeFileName));
}

//==============================================================================

void CellmlFileManager::fileUnmanaged(const QString &pFileName)
{
    QString nativeFileName = Core::nativeCanonicalFileName(pFileName);

    if (isCellmlFile(nativeFileName)) {
        // We are dealing with a CellML file, so we can remove it from our list
        // of managed CellML files after having deleted it

        delete cellmlFile(nativeFileName);

        mCellmlFiles.remove(nativeFileName);
    }
}

//==============================================================================

}   // namespace CellMLSupport
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================

//==============================================================================
// CellML file manager
//==============================================================================

#include "cellmlfilemanager.h"
#include "cellmlsupportplugin.h"
#include "filemanager.h"

//==============================================================================

namespace OpenCOR {
namespace CellMLSupport {

//==============================================================================

CellmlFile::CellmlFile(const QString &pFileName) :
    mFileName(pFileName)
{
}

//==============================================================================

QString CellmlFile::fileName() const
{
    // Return the file name of the CellML file

    return mFileName;
}

//==============================================================================

CellmlFileManager::CellmlFileManager()
{
    // Create some connections to keep track of some events related to our
    // 'global' file manager
qDebug(">>> CellmlFileManager::CellmlFileManager() -- 1 -- %d", this);
qDebug(">>> CellmlFileManager::CellmlFileManager() -- 2 -- %d", Core::FileManager::instance());
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
    // Return our 'global' CellML file manager

    static CellmlFileManager instance;
qDebug(">>> CellmlFileManager::instance() -- %d", &instance);

    return &instance;
}

//==============================================================================

void CellmlFileManager::fileManaged(const QString &pFileName)
{
qDebug(">>> CellmlFileManager::fileManaged(%s)", qPrintable(pFileName));

    if (isCellmlFile(pFileName))
        // We are dealing with a CellML file, so we can add it to our list of
        // managed CellML files

        mCellmlFiles << new CellmlFile(pFileName);
}

//==============================================================================

void CellmlFileManager::fileUnmanaged(const QString &pFileName)
{
    if (isCellmlFile(pFileName))
        // We are dealing with a CellML file, so we can remove it from our list
        // of managed CellML files

        foreach (CellmlFile *cellmlFile, mCellmlFiles)
            if (cellmlFile->fileName() == pFileName) {
                // The CellML file has been found, so remove it

                mCellmlFiles.removeAt(mCellmlFiles.indexOf(cellmlFile));

                delete cellmlFile;
            }
}

//==============================================================================

}   // namespace CellMLSupport
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================

//==============================================================================
// Some useful functions
//==============================================================================

#include "utils.h"

//==============================================================================

#include <QCoreApplication>
#include <QDate>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <QResource>

//==============================================================================

namespace OpenCOR {

//==============================================================================

QString exec(const QString &pProg, const QString &pArgs)
{
    if (QFileInfo(pProg).exists()) {
        // The program exists, so we can execute it

        QProcess process;

        process.start(pProg,  pArgs.split(' '));
        process.waitForFinished();

        return process.readAll().trimmed();
    } else {
        // The program doesn't exist, so...

        return QString();
    }
}

//==============================================================================

QString getOsName()
{
#ifdef Q_WS_WIN
    switch (QSysInfo::WindowsVersion) {
    case QSysInfo::WV_NT:
        return "Microsoft Windows NT";
    case QSysInfo::WV_2000:
        return "Microsoft Windows 2000";
    case QSysInfo::WV_XP:
        return "Microsoft Windows XP";
    case QSysInfo::WV_2003:
        return "Microsoft Windows 2003";
    case QSysInfo::WV_VISTA:
        return "Microsoft Windows Vista";
    case QSysInfo::WV_WINDOWS7:
        return "Microsoft Windows 7";
    default:
        return "Microsoft Windows";
    }
#elif defined(Q_WS_MAC)
    // Note #1: the version of Qt that we use on OS X only supports Mac OS X
    //          10.5 and above, so...
    // Note #2: from version 10.7, Apple uses OS X rather than Mac OS X...

    switch (QSysInfo::MacintoshVersion) {
    case QSysInfo::MV_10_5:
        return "Mac OS X 10.5 (Leopard)";
    case QSysInfo::MV_10_6:
        return "Mac OS X 10.6 (Snow Leopard)";
    case QSysInfo::MV_10_7:
        return "OS X 10.7 (Lion)";
    case QSysInfo::MV_10_8:
        return "OS X 10.8 (Mountain Lion)";
    default:
        return "Mac OS X";
    }
#else
    QString os = exec("/bin/uname", "-o");

    if (os.isEmpty())
        // We couldn't find /bin/uname, so...

        return "Unknown";
    else
        return os+" "+exec("/bin/uname", "-r");
#endif
}

//==============================================================================

QString getAppVersion(QCoreApplication *pApp)
{
    QString bitVersion;
    static int sizeOfPointer = sizeof(void *);

    switch (sizeOfPointer) {
    case 4:
        bitVersion = " (32-bit)";

        break;
    case 8:
        bitVersion = " (64-bit)";

        break;
    default:
        // Not a size that we could recognise, so...

        bitVersion = "";
    }

    return  pApp->applicationName()+" "+pApp->applicationVersion()+bitVersion;
}

//==============================================================================

QString getAppCopyright(const bool &pHtml)
{
    return QObject::tr("Copyright")+" "+QString(pHtml?"&copy;":"")+"2011-"+QString::number(QDate().currentDate().year());
}

//==============================================================================

}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================

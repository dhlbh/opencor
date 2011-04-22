#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QList>
#include <QObject>

class QString;
class QTimer;

class File
{
public:
    enum FileStatus
    {
        Changed,
        Unchanged,
        Deleted
    };

    explicit File(const QString &pFileName);

    QString fileName() const;

    FileStatus check();

private:
    QString mFileName;
    QString mSha1;

    QString sha1() const;
};

class FileManager : public QObject
{
    Q_OBJECT

public:
    enum ManageStatus
    {
        DoesNotExist,
        Added,
        AlreadyManaged
    };

    enum UnmanageStatus
    {
        Removed,
        NotManaged
    };

    explicit FileManager(const int &pTimerInterval = 1000);
    ~FileManager();

    ManageStatus manage(const QString &pFileName);
    UnmanageStatus unmanage(const QString &pFileName);

    int count() const;

private:
    QTimer *mTimer;
    QList<File *> mFiles;

    File * isManaged(const QString &pFileName);

Q_SIGNALS:
    void fileContentsChanged(const QString &pFileName);
    void fileDeleted(const QString &pFileName);

private Q_SLOTS:
    void check();
};

#endif

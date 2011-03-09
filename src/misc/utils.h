#ifndef UTILS_H
#define UTILS_H

#include <QString>

QString exec(const QString &pProg, const QString &pArgs);

QString getOsName();

QByteArray resourceAsByteArray(const QString &pResource);
void saveResourceAs(const QString &pResource, const QString &pFilename);

bool removePath(const QString &pPath, const bool &pRecursive = true);

#ifdef QT_GUI_LIB
void notYetImplemented(const QString &method);
#endif

#endif

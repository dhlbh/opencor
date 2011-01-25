#include "ui/mainwindow.h"

#include <QFileInfo>

#include <QtSingleApplication>

#include <QxtCommandOptions>

int main(int pArgc, char *pArgv[])
{
    QtSingleApplication app(pArgc, pArgv);

    // Send a message (containing the arguments that were passed to this
    // instance of OpenCOR) to the 'official' instance of OpenCOR, should there
    // be one. If there is no 'official' instance of OpenCOR, then just carry
    // on as normal, otherwise exit since we only want one instance of OpenCOR
    // at any given time

    if (app.isRunning())
    {
        app.sendMessage(app.arguments().join(" "));

        return 0;
    }

    // Set the name of the organization

    app.setOrganizationName("World");

    // Set the name of the application

    app.setApplicationName(QFileInfo(app.applicationFilePath()).baseName());

    // Retrieve and set the version of the application

    QFile versionFile(":version");

    versionFile.open(QIODevice::ReadOnly);

    QString version = QString(versionFile.readLine()).trimmed();

    if (version.endsWith(".0"))
        // There is no actual patch information, so trim it

        version.truncate(version.length()-2);

    versionFile.close();

    app.setApplicationVersion(version);

    // Create the main window

    MainWindow win;

    // Keep track of the main window (useful for QtSingleApplication)

    app.setActivationWindow(&win);

    // Make sure that OpenCOR can handle the message sent by another
    // instance of itself

    QObject::connect(&app, SIGNAL(messageReceived(const QString&)),
                     &win, SLOT(singleAppMsgRcvd(const QString&)));

    // Show the main window

    win.show();

    // Execute the application

    return app.exec();
}

#include <qtsingleapplication.h>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QtSingleApplication app(argc, argv);

    // Try to send a message (containing the arguments that were passed to this
    // instance of OpenCOR) to the 'official' instance of OpenCOR. If there is
    // no 'official' instance, then just carry on as normal, otherwise exit
    // since we only want one instance of OpenCOR at any given time

    if (app.sendMessage(app.arguments().join(" ")))
        return 0;

    // Create the main window

    MainWindow win;

    // Set the application icon, but only for Linux, since in the case of
    // Windows and Mac, it's done through CMake (see CMakeLists.txt)

#ifdef Q_WS_X11
    win.setWindowIcon(QIcon(QString(":appIcon")));
#endif

    // Set the name of the main window to OpenCOR

    win.setWindowTitle("OpenCOR");

    // Keep track of the main window (useful for QtSingleApplication)

    app.setActivationWindow(&win);

    // Make sure that OpenCOR can respond to the empty message (see above)
    // 'asking' it to bring itself to the foreground

    QObject::connect(&app, SIGNAL(messageReceived(const QString&)),
                     &win, SLOT(singleAppMsgRcvd(const QString&)));

    // Show the main window

    win.show();

    // Execute the application

    return app.exec();
}

#include "mainWindow.h"
#include "pragaProject.h"
#include "pragaShell.h"

#include <cstdio>
#include <QApplication>
#include <QtNetwork/QNetworkProxy>
#include <QMessageBox>


PragaProject myProject;


bool setProxy(QString hostName, unsigned short port)
{
    QNetworkProxy myProxy;

    myProxy.setType(QNetworkProxy::HttpProxy);
    myProxy.setHostName(hostName);
    myProxy.setPort(port);

    try {
       QNetworkProxy::setApplicationProxy(myProxy);
    }
    catch (...) {
        QMessageBox::information(nullptr, "Error in proxy configuration!", "");
        return false;
    }

    return true;
}

QCoreApplication* createApplication(int &argc, char *argv[])
{
    for (int i = 1; i < argc; ++i)
        if (!qstrcmp(argv[i], "-no-gui"))
            return new QCoreApplication(argc, argv);
    return new QApplication(argc, argv);
}

int main(int argc, char *argv[])
{

    int modality = MODE_GUI;

    // check if called from command prompt
    if (attachOutputToConsole())
        modality = MODE_BATCH;

    QApplication myApp(argc, argv);

    QString currentPath = myApp.applicationDirPath() + "/";
    if (! myProject.loadCommonSettings(currentPath + "default.ini"))
        return -1;

    if (! myProject.loadParameters(myProject.getPath() + "DATA/settings/parameters.ini"))
        return -1;

    if (! myProject.loadPragaSettings())
        return -1;

    QNetworkProxyFactory::setUseSystemConfiguration(true);

    if (modality == MODE_GUI)
    {
        QApplication::setOverrideCursor(Qt::ArrowCursor);
        MainWindow w;
        w.show();
        return myApp.exec();
    }
    else if (modality == MODE_CONSOLE)
    {
        return pragaShell(&myProject);
    }
    else if (modality == MODE_BATCH)
    {
        printf("\n PRAGA v0.1 \n");
        for (int i = 0; i < argc; i++)
            printf("argv[%d] %s\n", i, argv[i]);

        // Send "enter" to release application from the console
        // This is a hack, but if not used the console doesn't know the application has
        // returned. The "enter" key only sent if the console window is in focus.
        if (isConsoleForeground()) sendEnterKey();
    }

}

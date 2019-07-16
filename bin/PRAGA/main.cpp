#include "mainWindow.h"
#include "pragaProject.h"
#include "pragaShell.h"

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


int main(int argc, char *argv[])
{
    int modality = MODE_GUI;

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

}






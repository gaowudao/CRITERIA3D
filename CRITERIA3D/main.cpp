#include <QApplication>
#include <QtNetwork/QNetworkProxy>
#include <QMessageBox>

#include "mainwindow.h"
#include "crit3dProject.h"


Crit3DProject myProject;


bool setProxy(QString hostName, int port)
{
    QNetworkProxy myProxy;

    myProxy.setType(QNetworkProxy::HttpProxy);
    myProxy.setHostName(hostName);
    myProxy.setPort(port);

    try {
       QNetworkProxy::setApplicationProxy(myProxy);
    }
    catch (...) {
        QMessageBox::information(NULL, "Error in proxy configuration!", "");
        return false;
    }

    return true;
}


int main(int argc, char *argv[])
{
    QApplication myApp(argc, argv);

    QString currentPath = myApp.applicationDirPath() + "/";

    if (! myProject.readGenericSettings(currentPath))
        return -1;

    if (! myProject.readCriteria3DSettings())
        return -1;

    QNetworkProxyFactory::setUseSystemConfiguration(true);

    QApplication::setOverrideCursor(Qt::ArrowCursor);
    MainWindow w;
    w.show();

    return myApp.exec();
}






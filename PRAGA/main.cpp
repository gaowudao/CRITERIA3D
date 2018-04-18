#include "mainwindow.h"
#include <QApplication>
#include <QtNetwork/QNetworkProxy>
#include <QDebug>
#include <QMessageBox>

#include "project.h"
#include "formRunInfo.h"

Project myProject;

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
    myProject.path = myApp.applicationDirPath();

    //setProxy("172.16.1.21", 8080);
    QNetworkProxyFactory::setUseSystemConfiguration(true);

    environment test = praga;

    QApplication::setOverrideCursor(Qt::ArrowCursor);
    MainWindow w(test);
    w.show();

    return myApp.exec();
}






#include "mainwindow.h"
#include <QApplication>
#include <QtNetwork/QNetworkProxy>
#include <QDebug>
#include <QMessageBox>

#include "project.h"
#include "forminfo.h"

Project myProject;

bool setProxy(QString hostName, int port)
{
    QNetworkProxy myProxy;

    formInfo myInfo;
    myInfo.start("Init Proxy...", 0);

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

    myInfo.close();

    return true;
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    setProxy("172.16.1.21", 8080);

    environment test = praga;

    QApplication::setOverrideCursor(Qt::ArrowCursor);
    MainWindow w(test);
    w.show();

    return a.exec();
}

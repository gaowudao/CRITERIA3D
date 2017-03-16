#include "download.h"
#include "dbMeteoPoints.h"
#include "mainwindow.h"
#include <QApplication>
#include <QtNetwork/QNetworkProxy>
#include <QDebug>

// test

bool setProxy(QString hostName, int port)
{
    QNetworkProxy myProxy;

    myProxy.setType(QNetworkProxy::HttpProxy);
    myProxy.setHostName(hostName);
    myProxy.setPort(port);

    qDebug("Init Proxy...");
    try {
       QNetworkProxy::setApplicationProxy(myProxy);
    }
    catch (...) {
       qDebug("Error in proxy configuration!");
       return false;
    }

    return true;
}

int main(int argc, char *argv[])
{
//    QStringList datasets;
//    datasets << "agrmet" << "cer" ;

    //qDebug() << getVarName("/home/laura/criteria3D/DATA/template/prova.db",139);
    //qDebug() << getId("/home/laura/criteria3D/DATA/template/prova.db", "SEMIH_RHAIR");

    //QStringList dActive = getDatasetsActive("/home/laura/criteria3D/DATA/template/prova.db");
    //qDebug() << dActive;
    //qDebug() << dActive.at(0);

    QApplication a(argc, argv);

    //setProxy("172.16.1.21", 8080);

//    QList<int> test;
//    DbMeteoPoints* prova = new DbMeteoPoints("/home/laura/criteria3D/DATA/template/prova.db");
//    test = prova->getHourlyVar();
//    qDebug() << test;

    MainWindow w;
    w.show();



    return a.exec();


}

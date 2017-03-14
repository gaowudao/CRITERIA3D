#include "download.h"
#include "dbMeteoPoints.h"
#include "mainwindow.h"
#include <QApplication>
#include <QDebug>

// test

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
    MainWindow w;
    w.show();


    return a.exec();


}

/*
#---------------------------------------------------
#
#   PRAGA GIS
#
#   this file is part of CRITERIA3D distribution
#
#---------------------------------------------------
*/


#include <QApplication>
#include <QtNetwork/QNetworkProxy>
#include <QMessageBox>

#include "criteriaGeoProject.h"
#include "mainWindow.h"


CriteriaGeoProject myProject;


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
    QApplication myApp(argc, argv);

    QString currentPath = myApp.applicationDirPath() + "/";

    //if (! myProject.loadCommonSettings(currentPath + "default.ini"))
    //    return -1;

    QNetworkProxyFactory::setUseSystemConfiguration(true);

    QApplication::setOverrideCursor(Qt::ArrowCursor);
    MainWindow w;
    w.show();

    return myApp.exec();
}



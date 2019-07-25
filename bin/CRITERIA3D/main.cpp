#include <QApplication>
#include <QtNetwork/QNetworkProxy>
#include <QMessageBox>

#include "mainwindow.h"
#include "crit3dProject.h"


Crit3DProject myProject;


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

    myProject.setApplicationPath(myApp.applicationDirPath() + "/");

    if (! myProject.loadProjectSettings(myProject.getApplicationPath() + "default.ini"))
        return -1;

    myProject.setDefaultPath(myProject.getProjectPath());

    if (! myProject.loadParameters("parameters.ini"))
        return -1;

    if (! myProject.readCriteria3DParameters())
        return -1;

    QNetworkProxyFactory::setUseSystemConfiguration(true);

    QApplication::setOverrideCursor(Qt::ArrowCursor);
    MainWindow w;
    w.show();

    return myApp.exec();
}






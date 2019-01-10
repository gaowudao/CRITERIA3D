#include <QtNetwork/QNetworkProxy>
#include <QMessageBox>
#include <QtGui>
#include <QApplication>
#include "commonConstants.h"
#include "vine3DProject.h"
#include "mainWindow.h"


Vine3DProject myProject;

//QMessageBox msgBox;
//QString myMsg = "No GUI available.\n\n";
//myMsg += "USAGE:\n";
//myMsg += "vintage.exe projectXmlFile [nrDays] [nrDaysForecast]\n";
//myMsg += "projectXmlName: XML project filename\n";
//myMsg += "nrDays: days from today where to start (default: 7)\n";
//myMsg += "nrDaysForecast: days since today where to finish (default: 9)\n";
//msgBox.setText(myMsg);
//msgBox.exec();

int main(int argc, char *argv[])
{
    QApplication myApp(argc, argv);

    QString currentPath = myApp.applicationDirPath() + "/";

    if (! myProject.loadVine3DSettings())
        return -1;

    if (argc == 1)
    {
        myProject.setEnvironment(gui);

        if (! myProject.loadDefaultSettings(currentPath))
            return -1;

        QNetworkProxyFactory::setUseSystemConfiguration(true);

        QApplication::setOverrideCursor(Qt::ArrowCursor);
        MainWindow w;
        w.show();

        return myApp.exec();
    }
    else
    {
        myProject.setEnvironment(batch);

        QDate today, firstDay;
        int nrDays, nrDaysForecast;

        QString settingsFileName = QString(argv[1]);
        if (argc > 3)
        {
            nrDaysForecast = QString(argv[3]).toInt();
            nrDays = QString(argv[2]).toInt();
        }
        else if (argc > 2)
        {
            nrDays = QString(argv[2]).toInt();
            nrDaysForecast = 9;
        }
        else
        {
            nrDays = 7;      //default: 1 week
            nrDaysForecast = 9;
        }

        if (!myProject.loadProject(settingsFileName))
        {
            myProject.logError("Open project failed:\n " + settingsFileName);
        }   

        today = QDate::currentDate();
        QDateTime lastDateTime = QDateTime(today);
        lastDateTime = lastDateTime.addDays(nrDaysForecast);
        lastDateTime.setTime(QTime(23,0,0,0));

        firstDay = today.addDays(-nrDays);
        QDateTime firstDateTime = QDateTime(firstDay);
        firstDateTime.setTime(QTime(1,0,0,0));

        myProject.runModels(firstDateTime, lastDateTime, true, myProject.idArea);

        myProject.closeProject();

        exit(true);
    }
}

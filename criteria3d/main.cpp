#include <QtGui>
#include <QApplication>
#include "commonConstants.h"
#include "project.h"
#include "mainwindow.h"

Crit3DProject myProject;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if (argc == 1)
    {
        myProject.setEnvironment(gui);
        MainWindow mainWindow;
        mainWindow.show();

        return a.exec();
    }
    else
    {
        QDate today, firstDay;
        int nrDays;

        QString xmlFileName = QString(argv[1]);
        if (argc > 2)
            nrDays = QString(argv[2]).toInt();
        else
            nrDays = 7;      //default: 1 week

        if (!myProject.loadProject(xmlFileName))
        {
            myProject.logError("Open project failed:\n " + xmlFileName);
        }
        myProject.setEnvironment(batch);

        today = QDate::currentDate();
        QDateTime lastDateTime = QDateTime(today);
        lastDateTime.setTime(QTime(0,0,0,0));

        firstDay = today.addDays(-nrDays);
        QDateTime firstDateTime = QDateTime(firstDay);
        firstDateTime.setTime(QTime(1,0,0,0));

        myProject.runModels(firstDateTime, lastDateTime, true, myProject.idArea);

        myProject.closeProject();

        exit(0);
    }

}








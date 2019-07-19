#include <QtNetwork/QNetworkProxy>
#include <QMessageBox>
#include <QtGui>
#include <QApplication>
#include "commonConstants.h"
#include "vine3DProject.h"
#include "vine3DShell.h"
#include "mainWindow.h"


Vine3DProject myProject;

//QMessageBox msgBox;
//QString myMsg = "No GUI available.\n\n";
//myMsg += "USAGE:\n";
//myMsg += "VINE3D projectXmlFile [nrDays] [nrDaysForecast]\n";
//myMsg += "projectXmlName: XML project filename\n";
//myMsg += "nrDays: days from today where to start (default: 7)\n";
//myMsg += "nrDaysForecast: days since today where to finish (default: 9)\n";
//msgBox.setText(myMsg);
//msgBox.exec();

int main(int argc, char *argv[])
{
    // set modality (default: GUI)
    if (argc > 1)
    {
        QString arg1 = QString::fromStdString(argv[1]);
        if (arg1.toUpper() == "CONSOLE")
        {
            myProject.modality = MODE_CONSOLE;
        }
        else
        {
            myProject.modality = MODE_BATCH;
        }
    }

    QApplication myApp(argc, argv);

    QString currentPath = myApp.applicationDirPath() + "/";

    if (! myProject.loadCommonSettings(currentPath + "default.ini"))
        return -1;

    if (! myProject.loadParameters(myProject.getPath() + "DATA/settings/parameters.ini"))
        return -1;

    if (! myProject.loadVine3DSettings())
        return -1;

    QNetworkProxyFactory::setUseSystemConfiguration(true);

    if (myProject.modality == MODE_GUI)
    {
        //myProject.setEnvironment(gui);
        QApplication::setOverrideCursor(Qt::ArrowCursor);
        MainWindow w;
        w.show();
        return myApp.exec();
    }
    else if (myProject.modality == MODE_CONSOLE)
    {
        return vine3dShell(&myProject);
    }
    else if (myProject.modality == MODE_BATCH)
    {
        return vine3dBatch(&myProject, argv[1]);
    }
}

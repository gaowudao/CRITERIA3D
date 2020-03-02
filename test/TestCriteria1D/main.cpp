#include <QCoreApplication>
#include <QDir>

#include "criteria1DProject.h"
#include "commonConstants.h"
#include "utilities.h"

#define TEST


int main(int argc, char *argv[])
{
    QCoreApplication myApp(argc, argv);

    QString appPath = myApp.applicationDirPath() + "/";
    QString settingsFileName;
    Criteria1DProject myProject;

    if (argc > 1)
        settingsFileName = argv[1];
    else
    {
        #ifdef TEST
                QString path;
                if (! searchDataPath(&path)) return -1;
                settingsFileName = path + "PROJECT/CLARA/CLARA.ini";
                //settingsFileName = path + "PROJECT/kiwifruit/kiwifruit.ini";
        #else
                myProject->logInfo("USAGE: CRITERIA1D settings.ini");
                return ERROR_SETTINGS_MISSING;
        #endif
    }

    if (settingsFileName.left(1) == ".")
        settingsFileName = appPath + settingsFileName;

    int myError = myProject.initializeProject(settingsFileName);
    if (myError != CRIT3D_OK)
    {
        myProject.logError();
        return myError;
    }

    // unit list
    if (! myProject.loadUnits())
    {
        myProject.logError();
        return ERROR_READ_UNITS;
    }

    myProject.logInfo("\nQuery result: " + QString::number(myProject.nrUnits) + " distinct units.\n");

    if (!myProject.initializeOutputFile())
        return ERROR_DBOUTPUT;

    int result = myProject.compute();

    myProject.logInfo("\nEND");
    return result;
}


#include <QCoreApplication>
#include <QDir>

#include "criteria1DProject.h"
#include "commonConstants.h"
#include "utilities.h"

//#define TEST


int main(int argc, char *argv[])
{
    QCoreApplication myApp(argc, argv);

    Criteria1DProject myProject;

    QString appPath = myApp.applicationDirPath() + "/";
    QString settingsFileName;

    if (argc > 1)
        settingsFileName = argv[1];
    else
    {
        #ifdef TEST
                QString path;
                //if (! searchDataPath(&path)) return -1;
                //settingsFileName = path + "PROJECT/kiwifruit/kiwifruit.ini";
                //settingsFileName = path + "PROJECT/Incolto/Incolto.ini";
                settingsFileName = "C:/CRITERIA1D/PROJECTS/CLARA/seasonalIrriClimate.ini";
        #else
                myProject.logInfo("USAGE: CRITERIA1D settings.ini");
                return ERROR_SETTINGS_MISSING;
        #endif
    }

    if (settingsFileName.left(1) == ".")
        settingsFileName = appPath + settingsFileName;

    // initialize project
    int myResult = myProject.initializeProject(settingsFileName);
    if (myResult != CRIT3D_OK)
    {
        myProject.logError();
        return myResult;
    }

    // read unit list
    if (! myProject.loadUnits())
    {
        myProject.logError();
        return ERROR_READ_UNITS;
    }
    myProject.logInfo("\nQuery result: " + QString::number(myProject.nrUnits) + " distinct units.\n");

    // initialize output
    if (!myProject.initializeCsvOutputFile())
        return ERROR_DBOUTPUT;

    // COMPUTE
    myResult = myProject.compute();
    myProject.logInfo("\nEND");

    return myResult;
}


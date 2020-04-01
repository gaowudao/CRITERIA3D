#include <QCoreApplication>
#include <QDir>

#include "commonConstants.h"
#include "csvToShapeProject.h"
#include "utilities.h"

#define TEST


int main(int argc, char *argv[])
{
    QCoreApplication myApp(argc, argv);
    CsvToShapeProject myProject;

    QString appPath = myApp.applicationDirPath() + "/";
    QString settingsFileName;

    if (argc > 1)
        settingsFileName = argv[1];
    else
    {
        #ifdef TEST
                if (! searchDataPath(&myProject.dataPath)) return -1;
                settingsFileName = myProject.dataPath + "PROJECT/CLARA/csvToShapeInputOutput/csvToShape.ini";
        #else
                logInfo("USAGE: CRITERIA1D settings.ini");
                return ERROR_SETTINGS_MISSING;
        #endif
    }

    if (settingsFileName.left(1) == ".")
        settingsFileName = appPath + settingsFileName;

    int myResult = myProject.initializeProject(settingsFileName);
    if (myResult != CRIT3D_OK)
        return myResult;

    myProject.createShape();

    return 0;
}


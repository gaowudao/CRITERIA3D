#include "csvToShapeProject.h"
#include "logger.h"

CsvToShapeProject::CsvToShapeProject()
{
    initialize();
}

void CsvToShapeProject::initialize()
{
    isProjectLoaded = false;

    UCM = "";
    csv_data = "";
    csv_format = "";
    output_shape = "";
    logFileName = "";

}

void CsvToShapeProject::closeProject()
{
    if (isProjectLoaded)
    {
        logger.writeInfo("Close Project...");
        isProjectLoaded = false;
    }
}

int CsvToShapeProject::initializeProject(QString settingsFileName)
{
    closeProject();
    initialize();

    if (settingsFileName == "")
    {
        logger.writeError("Missing settings File.");
        return ERROR_SETTINGS_MISSING;
    }
/*
    // Configuration file
    QFile myFile(settingsFileName);
    if (myFile.exists())
    {
        configFileName = QDir(myFile.fileName()).canonicalPath();
        configFileName = QDir().cleanPath(configFileName);
        //qDebug("Using config file %s", qPrintable(configFileName));

        QFileInfo fileInfo(configFileName);
        path = fileInfo.path() + "/";
    }
    else
    {
        projectError = "Cannot find settings file: " + settingsFileName;
        return ERROR_SETTINGS_WRONGFILENAME;
    }

    if (!readSettings())
        return ERROR_SETTINGS_MISSINGDATA;

    setLogFile();

    int myError = openAllDatabase();
    if (myError != CRIT3D_OK)
        return myError;

    if (! loadVanGenuchtenParameters(&(irrForecast.dbSoil), irrForecast.soilTexture, &(projectError)))
        return ERROR_SOIL_PARAMETERS;

    if (! loadDriessenParameters(&(irrForecast.dbSoil), irrForecast.soilTexture, &(projectError)))
        return ERROR_SOIL_PARAMETERS;

    isProjectLoaded = true;

    return CRIT3D_OK;
    */
}

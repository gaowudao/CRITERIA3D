#include "csvToShapeProject.h"
#include "logger.h"
#include "commonConstants.h"
#include "shapeHandler.h"
#include "shapeFromCSVForShell.h"
#include <QDir>
#include <QSettings>

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

    // Configuration file
    QFile myFile(settingsFileName);
    if (myFile.exists())
    {
        configFileName = QDir(myFile.fileName()).canonicalPath();
        configFileName = QDir().cleanPath(configFileName);

        QFileInfo fileInfo(configFileName);
        path = fileInfo.path() + "/";
    }
    else
    {
        projectError = "Cannot find settings file: " + settingsFileName;
        logger.writeError(projectError);
        return ERROR_SETTINGS_WRONGFILENAME;
    }

    if (!readSettings())
        return ERROR_SETTINGS_MISSINGDATA;

    logger.setLog(path,logFileName);

    Crit3DShapeHandler shapeHandler;
    Crit3DShapeHandler outputShape;

    std::string errorStr;
    shapeHandler.open(UCM.toStdString());
    logger.writeInfo(" Waiting...");
    if (shapeFromCSVForShell(&shapeHandler, &outputShape, csv_data, csv_format, output_shape, &errorStr))
    {
        logger.writeInfo(" DONE");
        isProjectLoaded = true;
        return CRIT3D_OK;
    }

}

bool CsvToShapeProject::readSettings()
{
    QSettings* projectSettings;
    projectSettings = new QSettings(configFileName, QSettings::IniFormat);
    projectSettings->beginGroup("project");

    projectName = projectSettings->value("name","").toString();

    UCM = projectSettings->value("UCM","").toString();
    if (UCM.left(1) == ".")
    {
        UCM = path + QDir::cleanPath(UCM);
    }
    else
    {
        UCM = dataPath + QDir::cleanPath(UCM);
    }

    csv_data = projectSettings->value("csv_data","").toString();
    if (csv_data.left(1) == ".")
    {
        csv_data = path + QDir::cleanPath(csv_data);
    }
    else
    {
        csv_data = dataPath + QDir::cleanPath(csv_data);
    }

    csv_format = projectSettings->value("csv_format","").toString();
    if (csv_format.left(1) == ".")
    {
        csv_format = path + QDir::cleanPath(csv_format);
    }
    else
    {
        csv_format = dataPath + QDir::cleanPath(csv_format);
    }

    output_shape = projectSettings->value("output_shape","").toString();
    if (output_shape.left(1) == ".")
    {
        output_shape = path + QDir::cleanPath(output_shape);
    }
    else
    {
        output_shape = dataPath + QDir::cleanPath(output_shape);
    }

    logFileName = projectSettings->value("logfile","").toString();
    logFileName = QDir::cleanPath(logFileName);

    projectSettings->endGroup();
    return true;
}

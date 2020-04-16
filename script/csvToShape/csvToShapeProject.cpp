#include "csvToShapeProject.h"
#include "logger.h"
#include "commonConstants.h"
#include "shapeHandler.h"
#include "shapeFromCSVForShell.h"
#include <QDir>
#include <QSettings>
#include <iostream>
#include "utilities.h"

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

    isProjectLoaded = true;

    return CRIT3D_OK;
}


bool CsvToShapeProject::createShape()
{
    Crit3DShapeHandler shapeHandler;
    Crit3DShapeHandler outputShape;

    shapeHandler.open(UCM.toStdString());
    std::cout << " Waiting...";

    std::string errorStr;
    if (! shapeFromCSVForShell(&shapeHandler, &outputShape, csv_data, csv_format, output_shape, &errorStr))
    {
        logger.writeError(QString::fromStdString(errorStr));
        return false;
    }

    std::cout << "Done!\n";
    return true;
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
    QString fileName = getFileName(csv_data);
    QString dateStr = QDate::currentDate().toString("yyyy-MM-dd");
    csv_data += "_" + dateStr + ".csv";
    if (csv_data.left(1) == ".")
    {
        csv_data = path + QDir::cleanPath(csv_data);
    }
    else
    {
        csv_data = dataPath + QDir::cleanPath(csv_data);
    }

    csv_format = projectSettings->value("csv_format", "").toString();
    if (csv_format.left(1) == ".")
    {
        csv_format = path + QDir::cleanPath(csv_format);
    }
    else
    {
        csv_format = dataPath + QDir::cleanPath(csv_format);
    }

    // output directory
    QString outputDir = projectSettings->value("output_dir", "").toString();
    outputDir += dateStr;
    if (outputDir.left(1) == ".")
    {
        outputDir = path + QDir::cleanPath(outputDir);
    }
    else
    {
        outputDir = dataPath + QDir::cleanPath(outputDir);
    }

    QDir myDir;
    if (! myDir.exists(outputDir)) myDir.mkdir(outputDir);

    output_shape += outputDir + "/" + fileName + "_" + dateStr + ".shp";

    projectSettings->endGroup();
    return true;
}

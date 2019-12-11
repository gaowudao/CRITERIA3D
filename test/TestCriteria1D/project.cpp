#include <QSqlQuery>
#include <QSqlError>
#include <QDir>
#include <QDateTime>
#include <QSettings>
#include <QDebug>
#include <iostream>

#include "commonConstants.h"
#include "utilities.h"
#include "soilDbTools.h"
#include "project.h"

using namespace std;

Criteria1DProject::Criteria1DProject()
{
    initialize();
}

void Criteria1DProject::initialize()
{
    isProjectLoaded = false;

    path = "";
    name = "";
    logFileName = "";
    irrigationFileName = "";
    irrigationPath = "";
    dbOutputPath = "";

    dbParametersName = "";
    dbSoilName = "";
    dbMeteoName = "";
    dbForecastName = "";
    dbOutputName = "";
    dbUnitsName = "";

    projectError = "";
    nrUnits = 0;
}

void Criteria1DProject::initializeUnit(int nr)
{
    nrUnits = nr;
    unit = new CriteriaUnit[unsigned(nr)];
}


void Criteria1DProject::closeProject()
{
    if (isProjectLoaded)
    {
        logInfo("Close Project...");
        closeAllDatabase();
        logFile.close();

        isProjectLoaded = false;
    }
}


int Criteria1DProject::initializeProject(QString settingsFileName)
{
    closeProject();
    initialize();

    if (settingsFileName == "")
    {
        logError("Missing settings File.");
        return ERROR_SETTINGS_MISSING;
    }

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

    if (! loadVanGenuchtenParameters(&(criteria.dbSoil), criteria.soilTexture, &(projectError)))
        return ERROR_SOIL_PARAMETERS;

    if (! loadDriessenParameters(&(criteria.dbSoil), criteria.soilTexture, &(projectError)))
        return ERROR_SOIL_PARAMETERS;

    isProjectLoaded = true;

    return CRIT3D_OK;
}


bool Criteria1DProject::readSettings()
{
    QSettings* projectSettings;
    projectSettings = new QSettings(configFileName, QSettings::IniFormat);
    projectSettings->beginGroup("project");

    path += projectSettings->value("path","").toString();
    name += projectSettings->value("name","").toString();

    dbParametersName = projectSettings->value("db_parameters","").toString();
    if (dbParametersName.left(1) == ".")
        dbParametersName = path + dbParametersName;

    dbSoilName = projectSettings->value("db_soil","").toString();
    if (dbSoilName == "") dbSoilName = projectSettings->value("soil_db","").toString();
    if (dbSoilName.left(1) == ".")
        dbSoilName = path + dbSoilName;

    dbMeteoName = projectSettings->value("db_meteo","").toString();
    if (dbMeteoName == "") dbMeteoName = projectSettings->value("meteo_points","").toString();
    if (dbMeteoName.left(1) == ".")
        dbMeteoName = path + dbMeteoName;

    dbForecastName = projectSettings->value("db_forecast","").toString();
    if (dbForecastName.left(1) == ".")
        dbForecastName = path + dbForecastName;

    // unit list
    dbUnitsName = projectSettings->value("db_units","").toString();
    if (dbUnitsName.left(1) == ".")
        dbUnitsName = path + dbUnitsName;

    if (dbUnitsName == "")
    {
        projectError = "Missing information on units";
        return false;
    }

    dbOutputName = projectSettings->value("db_output","").toString();
    if (dbOutputName.left(1) == ".")
        dbOutputName = path + dbOutputName;

    dbOutputPath = getFilePath(dbOutputName);

    // seasonal or short-term forecast
    projectSettings->endGroup();
    projectSettings->beginGroup("forecast");

    criteria.isSeasonalForecast = projectSettings->value("isSeasonalForecast",0).toBool();
    criteria.isShortTermForecast = projectSettings->value("isShortTermForecast",0).toBool();
    if ((criteria.isSeasonalForecast) || (criteria.isShortTermForecast))
    {
        irrigationFileName = projectSettings->value("irrigationOutput").toString();

        if (irrigationFileName.left(1) == ".")
            irrigationFileName = path + irrigationFileName;

        irrigationPath = getFilePath(irrigationFileName);

        criteria.firstSeasonMonth = projectSettings->value("firstMonth",0).toInt();
    }

    if (criteria.isShortTermForecast)
    {
        criteria.daysOfForecast = projectSettings->value("daysOfForecast",0).toInt();
    }

    projectSettings->endGroup();
    return true;
}

void Criteria1DProject::closeAllDatabase()
{
    criteria.dbParameters.close();
    criteria.dbSoil.close();
    criteria.dbMeteo.close();
    criteria.dbForecast.close();
    criteria.dbOutput.close();
}

int Criteria1DProject::openAllDatabase()
{
    closeAllDatabase();

    logInfo ("Model parameters: " + dbParametersName);
    if (! QFile(dbParametersName).exists())
    {
        projectError = "DBparameters file doesn't exist";
        closeAllDatabase();
        return ERROR_DBPARAMETERS;
    }

    criteria.dbParameters = QSqlDatabase::addDatabase("QSQLITE", "parameters");
    criteria.dbParameters.setDatabaseName(dbParametersName);
    if (! criteria.dbParameters.open())
    {
        projectError = "Open parameters DB failed: " + criteria.dbParameters.lastError().text();
        closeAllDatabase();
        return ERROR_DBPARAMETERS;
    }

    logInfo ("Soil DB: " + dbSoilName);
    if (! QFile(dbSoilName).exists())
    {
        projectError = "Soil DB file doesn't exist";
        closeAllDatabase();
        return ERROR_DBSOIL;
    }

    criteria.dbSoil = QSqlDatabase::addDatabase("QSQLITE", "soil");
    criteria.dbSoil.setDatabaseName(dbSoilName);
    if (! criteria.dbSoil.open())
    {
        projectError = "Open soil DB failed: " + criteria.dbSoil.lastError().text();
        closeAllDatabase();
        return ERROR_DBSOIL;
    }

    logInfo ("Meteo DB: " + dbMeteoName);
    if (! QFile(dbMeteoName).exists())
    {
        projectError = "Meteo points DB file doesn't exist";
        closeAllDatabase();
        return ERROR_DBMETEO_OBSERVED;
    }

    criteria.dbMeteo = QSqlDatabase::addDatabase("QSQLITE", "meteo");
    criteria.dbMeteo.setDatabaseName(dbMeteoName);
    if (! criteria.dbMeteo.open())
    {
        projectError = "Open meteo DB failed: " + criteria.dbMeteo.lastError().text();
        closeAllDatabase();
        return ERROR_DBMETEO_OBSERVED;
    }

    // meteo forecast
    if (criteria.isShortTermForecast)
    {
        logInfo ("Forecast DB: " + dbForecastName);
        if (! QFile(dbForecastName).exists())
        {
            projectError = "DBforecast file doesn't exist";
            closeAllDatabase();
            return ERROR_DBMETEO_FORECAST;
        }
        criteria.dbForecast = QSqlDatabase::addDatabase("QSQLITE", "forecast");
        criteria.dbForecast.setDatabaseName(dbForecastName);
        if (! criteria.dbForecast.open())
        {
            projectError = "Open forecast DB failed: " + criteria.dbForecast.lastError().text();
            closeAllDatabase();
            return ERROR_DBMETEO_FORECAST;
        }
    }

    // output DB (not used in seasonal forecast)
    if (! criteria.isSeasonalForecast)
    {
        QFile::remove(dbOutputName);
        logInfo ("Output DB: " + dbOutputName);
        criteria.dbOutput = QSqlDatabase::addDatabase("QSQLITE", "output");
        criteria.dbOutput.setDatabaseName(dbOutputName);

        if (!QDir(dbOutputPath).exists())
             QDir().mkdir(dbOutputPath);


        if (! criteria.dbOutput.open())
        {
            projectError = "Open output DB failed: " + criteria.dbOutput.lastError().text();
            closeAllDatabase();
            return ERROR_DBOUTPUT;
        }
    }

    // units list
    logInfo ("Units DB: " + dbUnitsName);
    dbUnits = QSqlDatabase::addDatabase("QSQLITE", "units");
    dbUnits.setDatabaseName(dbUnitsName);
    if (! dbUnits.open())
    {
        projectError = "Open Units DB failed: " + dbUnits.lastError().text();
        closeAllDatabase();
        return ERROR_DBUNITS;
    }

    return CRIT3D_OK;
}


bool Criteria1DProject::loadUnits()
{
    QString queryString = "SELECT DISTINCT ID_CASE, ID_CROP, ID_SOIL, ID_METEO FROM units";
    queryString += " ORDER BY ID_CROP, ID_SOIL, ID_METEO";

    QSqlQuery query = dbUnits.exec(queryString);
    query.last();
    if (! query.isValid())
    {
        if (query.lastError().nativeErrorCode() != "")
            projectError = "dbUnits error: " + query.lastError().nativeErrorCode() + " - " + query.lastError().text();
        else
            projectError = "Missing units";
        return false;
    }

    int nr = query.at() + 1;     // SQLITE doesn't support SIZE
    initializeUnit(nr);

    int i = 0;
    query.first();
    do
    {
        unit[i].idCase = query.value("ID_CASE").toString();
        unit[i].idCropClass = query.value("ID_CROP").toString();
        unit[i].idMeteo = query.value("ID_METEO").toString();
        unit[i].idForecast = query.value("ID_METEO").toString();
        unit[i].idSoilNumber = query.value("ID_SOIL").toInt();

        i++;
    } while(query.next());

    return true;
}


//-------------------
//
//   LOG functions
//
//-------------------

bool Criteria1DProject::setLogFile()
{
    if (!QDir(path + "log").exists())
         QDir().mkdir(path + "log");

    QString myDate = QDateTime().currentDateTime().toString("yyyy-MM-dd hh.mm");
    QString fileName = name + "_" + myDate + ".txt";

    logFileName = path + "log/" + fileName;
    std::cout << "SWB PROCESSOR - log file created:\n" << logFileName.toStdString() << std::endl;

    logFile.open(logFileName.toStdString().c_str());
    return (logFile.is_open());
}


void Criteria1DProject::logInfo(QString logStr)
{
    if (logFile.is_open())
        logFile << logStr.toStdString() << std::endl;

    std::cout << logStr.toStdString() << std::endl;
}


void Criteria1DProject::logError()
{
    if (logFile.is_open())
        logFile << "----ERROR!----\n" << projectError.toStdString() << std::endl;

    std::cout << "----ERROR!----\n" << projectError.toStdString() << std::endl << std::endl;
}


void Criteria1DProject::logError(QString myErrorStr)
{
    projectError = myErrorStr;
    logError();
}


#include <QSqlQuery>
#include <QSqlError>
#include <QDir>
#include <QDateTime>
#include <QSettings>
#include <QDebug>
#include <iostream>

#include "commonConstants.h"
#include "utilities.h"
#include "dbTools.h"
#include "soilDbTools.h"
#include "project.h"

using namespace std;

Criteria1DProject::Criteria1DProject()
{
    this->initialize();
}

void Criteria1DProject::initialize()
{
    this->isProjectLoaded = false;

    this->path = "";
    this->name = "";
    this->logFileName = "";
    this->irrigationFileName = "";
    this->irrigationPath = "";
    this->dbOutputPath = "";

    this->dbParametersName = "";
    this->dbSoilName = "";
    this->dbMeteoName = "";
    this->dbForecastName = "";
    this->dbOutputName = "";
    this->dbUnitsName = "";

    this->projectError = "";
    this->nrUnits = 0;
}

void Criteria1DProject::initializeUnit(int nr)
{
    this->nrUnits = nr;
    this->unit = new Criteria1DUnit[unsigned(nr)];
}


void Criteria1DProject::closeProject()
{
    if (this->isProjectLoaded)
    {
        this->logInfo("Close Project...");
        this->closeAllDatabase();
        this->logFile.close();

        this->isProjectLoaded = false;
    }
}


int Criteria1DProject::initializeProject(QString settingsFileName)
{
    this->closeProject();
    this->initialize();

    if (settingsFileName == "")
    {
        this->logError("Missing settings File.");
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
        this->projectError = "Cannot find settings file: " + settingsFileName;
        return ERROR_SETTINGS_WRONGFILENAME;
    }

    if (!readSettings())
        return ERROR_SETTINGS_MISSINGDATA;

    this->setLogFile();

    int myError = this->openAllDatabase();
    if (myError != CRIT3D_OK)
        return myError;

    if (! loadVanGenuchtenParameters(&(criteria.soilTexture[0]), &(criteria.dbParameters), &(this->projectError)))
        return ERROR_SOIL_PARAMETERS;

    if (! loadDriessenParameters(&(criteria.soilTexture[0]), &(criteria.dbParameters), &(this->projectError)))
        return ERROR_SOIL_PARAMETERS;

    isProjectLoaded = true;

    return CRIT3D_OK;
}


bool Criteria1DProject::readSettings()
{
    QSettings* projectSettings;
    projectSettings = new QSettings(this->configFileName, QSettings::IniFormat);
    projectSettings->beginGroup("project");

    this->path += projectSettings->value("path","").toString();
    this->name += projectSettings->value("name","").toString();

    this->dbParametersName = projectSettings->value("DBparameters","").toString();
    if (this->dbParametersName.left(1) == ".")
        this->dbParametersName = this->path + this->dbParametersName;

    this->dbSoilName = projectSettings->value("DBsoil","").toString();
    if (this->dbSoilName.left(1) == ".")
        this->dbSoilName = this->path + this->dbSoilName;

    this->dbMeteoName = projectSettings->value("DBmeteo","").toString();
    if (this->dbMeteoName.left(1) == ".")
        this->dbMeteoName = this->path + this->dbMeteoName;

    this->dbForecastName = projectSettings->value("DBforecast","").toString();
    if (this->dbForecastName.left(1) == ".")
        this->dbForecastName = this->path + this->dbForecastName;

    // unit list
    this->dbUnitsName = projectSettings->value("DBunits","").toString();
    if (this->dbUnitsName.left(1) == ".")
        this->dbUnitsName = this->path + this->dbUnitsName;

    if (this->dbUnitsName == "")
    {
        this->projectError = "Missing information on units";
        return false;
    }

    this->dbOutputName = projectSettings->value("DBoutput","").toString();
    if (this->dbOutputName.left(1) == ".")
        this->dbOutputName = this->path + this->dbOutputName;

    this->dbOutputPath = getFilePath(this->dbOutputName);

    // seasonal or short-term forecast
    projectSettings->endGroup();
    projectSettings->beginGroup("forecast");

    this->criteria.isSeasonalForecast = projectSettings->value("isSeasonalForecast",0).toBool();
    this->criteria.isShortTermForecast = projectSettings->value("isShortTermForecast",0).toBool();
    if ((this->criteria.isSeasonalForecast) || (this->criteria.isShortTermForecast))
    {
        this->irrigationFileName = projectSettings->value("irrigationOutput").toString();

        if (this->irrigationFileName.left(1) == ".")
            this->irrigationFileName = this->path + this->irrigationFileName;

        this->irrigationPath = getFilePath(this->irrigationFileName);

        this->criteria.firstSeasonMonth = projectSettings->value("firstMonth",0).toInt();
    }

    if (this->criteria.isShortTermForecast)
    {
        this->criteria.daysOfForecast = projectSettings->value("daysOfForecast",0).toInt();
    }

    projectSettings->endGroup();
    return true;
}

void Criteria1DProject::closeAllDatabase()
{
    this->criteria.dbParameters.close();
    this->criteria.dbSoil.close();
    this->criteria.dbMeteo.close();
    this->criteria.dbForecast.close();
    this->criteria.dbOutput.close();
}

int Criteria1DProject::openAllDatabase()
{
    this->closeAllDatabase();

    this->logInfo ("Model parameters: " + this->dbParametersName);
    if (! QFile(dbParametersName).exists())
    {
        this->projectError = "DBparameters file doesn't exist";
        this->closeAllDatabase();
        return ERROR_DBPARAMETERS;
    }

    this->criteria.dbParameters = QSqlDatabase::addDatabase("QSQLITE", "parameters");
    this->criteria.dbParameters.setDatabaseName(this->dbParametersName);
    if (! this->criteria.dbParameters.open())
    {
        this->projectError = "Open parameters DB failed: " + this->criteria.dbParameters.lastError().text();
        this->closeAllDatabase();
        return ERROR_DBPARAMETERS;
    }

    this->logInfo ("Soil DB: " + this->dbSoilName);
    if (! QFile(dbSoilName).exists())
    {
        this->projectError = "DBsoil file doesn't exist";
        this->closeAllDatabase();
        return ERROR_DBSOIL;
    }

    this->criteria.dbSoil = QSqlDatabase::addDatabase("QSQLITE", "soil");
    this->criteria.dbSoil.setDatabaseName(this->dbSoilName);
    if (! this->criteria.dbSoil.open())
    {
        this->projectError = "Open soil DB failed: " + this->criteria.dbSoil.lastError().text();
        this->closeAllDatabase();
        return ERROR_DBSOIL;
    }

    this->logInfo ("Meteo DB: " + this->dbMeteoName);
    if (! QFile(dbMeteoName).exists())
    {
        this->projectError = "DBmeteo file doesn't exist";
        this->closeAllDatabase();
        return ERROR_DBMETEO_OBSERVED;
    }

    this->criteria.dbMeteo = QSqlDatabase::addDatabase("QSQLITE", "meteo");
    this->criteria.dbMeteo.setDatabaseName(this->dbMeteoName);
    if (! this->criteria.dbMeteo.open())
    {
        this->projectError = "Open meteo DB failed: " + this->criteria.dbMeteo.lastError().text();
        this->closeAllDatabase();
        return ERROR_DBMETEO_OBSERVED;
    }

    // meteo forecast
    if (this->criteria.isShortTermForecast)
    {
        this->logInfo ("Forecast DB: " + this->dbForecastName);
        if (! QFile(dbForecastName).exists())
        {
            this->projectError = "DBforecast file doesn't exist";
            this->closeAllDatabase();
            return ERROR_DBMETEO_FORECAST;
        }
        this->criteria.dbForecast = QSqlDatabase::addDatabase("QSQLITE", "forecast");
        this->criteria.dbForecast.setDatabaseName(this->dbForecastName);
        if (! this->criteria.dbForecast.open())
        {
            this->projectError = "Open forecast DB failed: " + this->criteria.dbForecast.lastError().text();
            this->closeAllDatabase();
            return ERROR_DBMETEO_FORECAST;
        }
    }

    // output DB (not used in seasonal forecast)
    if (! this->criteria.isSeasonalForecast)
    {
        QFile::remove(this->dbOutputName);
        this->logInfo ("Output DB: " + this->dbOutputName);
        this->criteria.dbOutput = QSqlDatabase::addDatabase("QSQLITE", "output");
        this->criteria.dbOutput.setDatabaseName(this->dbOutputName);

        if (!QDir(this->dbOutputPath).exists())
             QDir().mkdir(this->dbOutputPath);


        if (! this->criteria.dbOutput.open())
        {
            this->projectError = "Open output DB failed: " + this->criteria.dbOutput.lastError().text();
            this->closeAllDatabase();
            return ERROR_DBOUTPUT;
        }
    }

    // units list
    this->logInfo ("Units DB: " + this->dbUnitsName);
    this->dbUnits = QSqlDatabase::addDatabase("QSQLITE", "units");
    this->dbUnits.setDatabaseName(this->dbUnitsName);
    if (! this->dbUnits.open())
    {
        this->projectError = "Open Units DB failed: " + this->dbUnits.lastError().text();
        this->closeAllDatabase();
        return ERROR_DBUNITS;
    }

    return CRIT3D_OK;
}


bool Criteria1DProject::loadUnits()
{
    QString queryString = "SELECT DISTINCT ID_CASE, ID_CROP, ID_SOIL, ID_METEO FROM units";
    queryString += " ORDER BY ID_CROP, ID_SOIL, ID_METEO";

    QSqlQuery query = this->dbUnits.exec(queryString);
    query.last();
    if (! query.isValid())
    {
        if (query.lastError().nativeErrorCode() != "")
            this->projectError = "dbUnits error: " + query.lastError().nativeErrorCode() + " - " + query.lastError().text();
        else
            this->projectError = "Missing units";
        return false;
    }

    int nr = query.at() + 1;     // SQLITE doesn't support SIZE
    this->initializeUnit(nr);

    int i = 0;
    query.first();
    do
    {
        this->unit[i].idCase = query.value("ID_CASE").toString();
        this->unit[i].idCropClass = query.value("ID_CROP").toString();
        this->unit[i].idMeteo = query.value("ID_METEO").toString();
        this->unit[i].idForecast = query.value("ID_METEO").toString();
        this->unit[i].idSoilNumber = query.value("ID_SOIL").toInt();

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
    if (!QDir(this->path + "log").exists())
         QDir().mkdir(this->path + "log");

    QString myDate = QDateTime().currentDateTime().toString("yyyy-MM-dd hh.mm");
    QString fileName = this->name + "_" + myDate + ".txt";

    this->logFileName = this->path + "log/" + fileName;
    std::cout << "SWB PROCESSOR - log file created:\n" << this->logFileName.toStdString() << std::endl;

    this->logFile.open(this->logFileName.toStdString().c_str());
    return (this->logFile.is_open());
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
        logFile << "----ERROR!----\n" << this->projectError.toStdString() << std::endl;

    std::cout << "----ERROR!----\n" << this->projectError.toStdString() << std::endl << std::endl;
}


void Criteria1DProject::logError(QString myErrorStr)
{
    this->projectError = myErrorStr;
    logError();
}


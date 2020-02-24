#include <QSqlQuery>
#include <QSqlError>
#include <QDir>
#include <QDateTime>
#include <QSettings>
#include <QDebug>
#include <iostream>

#include "commonConstants.h"
#include "basicMath.h"
#include "utilities.h"
#include "cropDbTools.h"
#include "soilDbTools.h"
#include "criteria1DProject.h"
#include "modelCore.h"

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

    dbCropName = "";
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

    if (! loadVanGenuchtenParameters(&(criteriaCase.dbSoil), criteriaCase.soilTexture, &(projectError)))
        return ERROR_SOIL_PARAMETERS;

    if (! loadDriessenParameters(&(criteriaCase.dbSoil), criteriaCase.soilTexture, &(projectError)))
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

    dbCropName = projectSettings->value("db_crop","").toString();
    if (dbCropName.left(1) == ".")
        dbCropName = path + dbCropName;

    dbSoilName = projectSettings->value("db_soil","").toString();
    if (dbSoilName.left(1) == ".")
        dbSoilName = path + dbSoilName;

    dbMeteoName = projectSettings->value("db_meteo","").toString();
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

    criteriaCase.isSeasonalForecast = projectSettings->value("isSeasonalForecast",0).toBool();
    criteriaCase.isShortTermForecast = projectSettings->value("isShortTermForecast",0).toBool();
    if ((criteriaCase.isSeasonalForecast) || (criteriaCase.isShortTermForecast))
    {
        irrigationFileName = projectSettings->value("irrigationOutput").toString();

        if (irrigationFileName.left(1) == ".")
            irrigationFileName = path + irrigationFileName;

        irrigationPath = getFilePath(irrigationFileName);

        criteriaCase.firstSeasonMonth = projectSettings->value("firstMonth",0).toInt();
    }

    if (criteriaCase.isShortTermForecast)
    {
        criteriaCase.daysOfForecast = projectSettings->value("daysOfForecast",0).toInt();
    }

    projectSettings->endGroup();
    return true;
}

void Criteria1DProject::closeAllDatabase()
{
    criteriaCase.dbCrop.close();
    criteriaCase.dbSoil.close();
    criteriaCase.dbMeteo.close();
    criteriaCase.dbForecast.close();
    criteriaCase.dbOutput.close();
}

int Criteria1DProject::openAllDatabase()
{
    closeAllDatabase();

    logInfo ("Model parameters: " + dbCropName);
    if (! QFile(dbCropName).exists())
    {
        projectError = "DBparameters file doesn't exist";
        closeAllDatabase();
        return ERROR_DBPARAMETERS;
    }

    criteriaCase.dbCrop = QSqlDatabase::addDatabase("QSQLITE", "parameters");
    criteriaCase.dbCrop.setDatabaseName(dbCropName);
    if (! criteriaCase.dbCrop.open())
    {
        projectError = "Open parameters DB failed: " + criteriaCase.dbCrop.lastError().text();
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

    criteriaCase.dbSoil = QSqlDatabase::addDatabase("QSQLITE", "soil");
    criteriaCase.dbSoil.setDatabaseName(dbSoilName);
    if (! criteriaCase.dbSoil.open())
    {
        projectError = "Open soil DB failed: " + criteriaCase.dbSoil.lastError().text();
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

    criteriaCase.dbMeteo = QSqlDatabase::addDatabase("QSQLITE", "meteo");
    criteriaCase.dbMeteo.setDatabaseName(dbMeteoName);
    if (! criteriaCase.dbMeteo.open())
    {
        projectError = "Open meteo DB failed: " + criteriaCase.dbMeteo.lastError().text();
        closeAllDatabase();
        return ERROR_DBMETEO_OBSERVED;
    }

    // meteo forecast
    if (criteriaCase.isShortTermForecast)
    {
        logInfo ("Forecast DB: " + dbForecastName);
        if (! QFile(dbForecastName).exists())
        {
            projectError = "DBforecast file doesn't exist";
            closeAllDatabase();
            return ERROR_DBMETEO_FORECAST;
        }
        criteriaCase.dbForecast = QSqlDatabase::addDatabase("QSQLITE", "forecast");
        criteriaCase.dbForecast.setDatabaseName(dbForecastName);
        if (! criteriaCase.dbForecast.open())
        {
            projectError = "Open forecast DB failed: " + criteriaCase.dbForecast.lastError().text();
            closeAllDatabase();
            return ERROR_DBMETEO_FORECAST;
        }
    }

    // output DB (not used in seasonal forecast)
    if (! criteriaCase.isSeasonalForecast)
    {
        QFile::remove(dbOutputName);
        logInfo ("Output DB: " + dbOutputName);
        criteriaCase.dbOutput = QSqlDatabase::addDatabase("QSQLITE", "output");
        criteriaCase.dbOutput.setDatabaseName(dbOutputName);

        if (!QDir(dbOutputPath).exists())
             QDir().mkdir(dbOutputPath);


        if (! criteriaCase.dbOutput.open())
        {
            projectError = "Open output DB failed: " + criteriaCase.dbOutput.lastError().text();
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


bool Criteria1DProject::initializeOutputFile()
{
    if ((criteriaCase.isSeasonalForecast) || (criteriaCase.isShortTermForecast))
    {
        if (!QDir(irrigationPath).exists())
            QDir().mkdir(irrigationPath);

        // add date to filename (only for ShortTermForecast)
        if (criteriaCase.isShortTermForecast)
        {
            irrigationFileName = irrigationFileName.left(irrigationFileName.length()-4);
            irrigationFileName += "_" + QDate::currentDate().toString("yyyyMMdd") + ".csv";
        }

        outputFile.open(irrigationFileName.toStdString().c_str(), std::ios::out | std::ios::trunc);
        if ( outputFile.fail())
        {
            logError("open failure: " + QString(strerror(errno)) + '\n');
            return false;
        }
        else
        {
            logInfo("Output file: " + irrigationFileName);
        }

        if (criteriaCase.isSeasonalForecast)
            outputFile << "ID_CASE,CROP,SOIL,METEO,p5,p25,p50,p75,p95\n";

        else if(criteriaCase.isShortTermForecast)
            outputFile << "dateForecast,ID_CASE,CROP,SOIL,METEO,readilyAvailableWater,rootDepth,"
                           "forecast7daysPrec,forecast7daysETc,forecast7daysIRR,previousAllSeasonIRR\n";
    }
    return true;
}



int Criteria1DProject::compute()
{
    bool isErrorMeteo = false;
    bool isErrorSoil = false;
    bool isErrorCrop = false;
    long nrUnitsComputed = 0;

    try
    {
        for (int i = 0; i < nrUnits; i++)
        {
            //CROP
            unit[i].idCrop = getCropFromClass(&(criteriaCase.dbCrop), "crop_class", "id_class",
                                                         unit[i].idCropClass, &(projectError)).toUpper();
            if (unit[i].idCrop == "")
            {
                logInfo("Unit " + unit[i].idCase + " " + unit[i].idCropClass + " ***** missing CROP *****");
                isErrorCrop = true;
            }
            else
            {
                //IRRI_RATIO
                double irriRatio = double(getIrriRatioFromClass(&(criteriaCase.dbCrop), "crop_class", "id_class",
                                                                unit[i].idCropClass, &(projectError)));
                if (int(irriRatio) == int(NODATA))
                    logInfo("Unit " + unit[i].idCase + " " + unit[i].idCropClass + " ***** missing IRRIGATION RATIO *****");
                else
                {
                    //SOIL
                    unit[i].idSoil = getIdSoilString(&(criteriaCase.dbSoil), unit[i].idSoilNumber, &(projectError));
                    if (unit[i].idSoil == "")
                    {
                        logInfo("Unit " + unit[i].idCase + " Soil nr." + QString::number(unit[i].idSoilNumber) + " ***** missing SOIL *****");
                        isErrorSoil = true;
                    }
                    else
                    {
                        // LOG
                        logInfo("Unit " + unit[i].idCase +" "+ unit[i].idCrop +" "+ unit[i].idSoil +" "+ unit[i].idMeteo);

                        // SEASONAL
                        if (criteriaCase.isSeasonalForecast)
                        {
                            if (irriRatio < EPSILON)
                            {
                                outputFile << unit[i].idCase.toStdString() << "," << unit[i].idCrop.toStdString() << ",";
                                outputFile << unit[i].idSoil.toStdString() << "," << unit[i].idMeteo.toStdString();
                                outputFile << ",0,0,0,0,0\n";
                            }
                            else
                            {
                                if (! runModel(&criteriaCase, &(unit[i]), &(projectError)))
                                {
                                    logError();
                                    // TODO Improve
                                    isErrorMeteo = true;
                                }
                                else
                                {
                                    outputFile << unit[i].idCase.toStdString() << "," << unit[i].idCrop.toStdString() << ",";
                                    outputFile << unit[i].idSoil.toStdString() << "," << unit[i].idMeteo.toStdString();
                                    // percentiles
                                    double percentile = sorting::percentile(criteriaCase.seasonalForecasts, &(criteriaCase.nrSeasonalForecasts), 5, true);
                                    outputFile << "," << percentile * irriRatio;
                                    percentile = sorting::percentile(criteriaCase.seasonalForecasts, &(criteriaCase.nrSeasonalForecasts), 25, false);
                                    outputFile << "," << percentile * irriRatio;
                                    percentile = sorting::percentile(criteriaCase.seasonalForecasts, &(criteriaCase.nrSeasonalForecasts), 50, false);
                                    outputFile << "," << percentile * irriRatio;
                                    percentile = sorting::percentile(criteriaCase.seasonalForecasts, &(criteriaCase.nrSeasonalForecasts), 75, false);
                                    outputFile << "," << percentile * irriRatio;
                                    percentile = sorting::percentile(criteriaCase.seasonalForecasts, &(criteriaCase.nrSeasonalForecasts), 95, false);
                                    outputFile << "," << percentile * irriRatio << "\n";
                                    outputFile.flush();

                                    nrUnitsComputed++;
                                }
                            }
                        }
                        else
                        {
                            if (runModel(&criteriaCase, &(unit[i]), &(projectError)))
                            {
                                nrUnitsComputed++;

                                // SHORT TERM FORECAST
                                if(criteriaCase.isShortTermForecast)
                                {
                                    int indexOfForecast = criteriaCase.meteoPoint.nrObsDataDaysD - criteriaCase.daysOfForecast - 1;
                                    Crit3DDate dateOfForecast = criteriaCase.meteoPoint.obsDataD[indexOfForecast].date;
                                    std::string dateOfForecastStr = dateOfForecast.toStdString();
                                    std::string IrrPreviousDateStr = dateOfForecast.addDays(-13).toStdString();

                                    // first date for annual irrigation
                                    Crit3DDate firstDateAllSeason;
                                    if (criteriaCase.firstSeasonMonth <= dateOfForecast.month)
                                        firstDateAllSeason = Crit3DDate(1, criteriaCase.firstSeasonMonth, dateOfForecast.year);
                                    else
                                        firstDateAllSeason = Crit3DDate(1, criteriaCase.firstSeasonMonth, dateOfForecast.year - 1);

                                    std::string firstDateAllSeasonStr = firstDateAllSeason.toStdString();

                                    double prec = NODATA;
                                    double maxTranspiration = NODATA;
                                    double forecastIrrigation = NODATA;
                                    double previousIrrigation = NODATA;
                                    double readilyAvailWater = NODATA;
                                    double rootDepth = NODATA;

                                    std::string mySQL = "SELECT SUM(PREC) AS prec,"
                                                        " SUM(TRANSP_MAX) AS maxTransp, SUM(IRRIGATION) AS irr"
                                                        " FROM '" + unit[i].idCase.toStdString() + "'"
                                                                                                    " WHERE DATE > '" + dateOfForecastStr + "'";

                                    QSqlQuery myQuery = criteriaCase.dbOutput.exec(QString::fromStdString(mySQL));

                                    if (myQuery.lastError().type() != QSqlError::NoError)
                                        logError("SELECT SUM(PREC)\n" + myQuery.lastError().text());
                                    else
                                    {
                                        myQuery.last();
                                        prec = myQuery.value("prec").toDouble();
                                        maxTranspiration = myQuery.value("maxTransp").toDouble();
                                        forecastIrrigation = myQuery.value("irr").toDouble();
                                    }

                                    mySQL = "SELECT RAW, DEFICIT, ROOTDEPTH FROM '"
                                            + unit[i].idCase.toStdString() + "'"
                                                                                        " WHERE DATE = '" + dateOfForecastStr + "'";

                                    myQuery = criteriaCase.dbOutput.exec(QString::fromStdString(mySQL));

                                    if (myQuery.lastError().type() != QSqlError::NoError)
                                        logError("SELECT RAW, DEFICIT, ROOTDEPTH\n" + myQuery.lastError().text());
                                    else
                                    {
                                        myQuery.last();
                                        readilyAvailWater = myQuery.value("RAW").toDouble();
                                        rootDepth = myQuery.value("ROOTDEPTH").toDouble();
                                    }

                                    mySQL = "SELECT SUM(IRRIGATION) AS previousIrrigation FROM '"
                                            + unit[i].idCase.toStdString() + "'"
                                                                                        " WHERE DATE <= '" + dateOfForecastStr + "'"
                                                                  " AND DATE >= '" + firstDateAllSeasonStr + "'";

                                    myQuery = criteriaCase.dbOutput.exec(QString::fromStdString(mySQL));

                                    if (myQuery.lastError().type() != QSqlError::NoError)
                                        logError("SELECT SUM(IRRIGATION) (all season) \n" + myQuery.lastError().text());
                                    else
                                    {
                                        myQuery.last();
                                        previousIrrigation = myQuery.value("previousIrrigation").toDouble();
                                    }

                                    outputFile << dateOfForecast.toStdString();
                                    outputFile << "," << unit[i].idCase.toStdString();
                                    outputFile << "," << unit[i].idCrop.toStdString();
                                    outputFile << "," << unit[i].idSoil.toStdString();
                                    outputFile << "," << unit[i].idMeteo.toStdString();
                                    outputFile << "," << QString::number(readilyAvailWater,'f',1).toStdString();
                                    outputFile << "," << QString::number(rootDepth,'f',2).toStdString();
                                    outputFile << "," << QString::number(prec,'f',1).toStdString();
                                    outputFile << "," << QString::number(maxTranspiration,'f',1).toStdString();
                                    outputFile << "," << forecastIrrigation * irriRatio;
                                    outputFile << "," << previousIrrigation * irriRatio << "\n";
                                    outputFile.flush();
                                }
                            }
                            else
                            {
                                logError();
                                // TODO Improve
                                isErrorMeteo = true;
                            }
                        }
                    }
                }
            }
        }

        outputFile.close();

    } catch (std::exception &e)
    {
        qFatal("Error %s ", e.what());

    } catch (...)
    {
        qFatal("Error <unknown>");
        return ERROR_UNKNOWN;
    }

    // error check
    if (nrUnitsComputed == 0)
    {
        if (isErrorMeteo)
            return ERROR_METEO;
        else if (isErrorSoil)
            return ERROR_SOIL_MISSING;
        else if (isErrorCrop)
            return ERROR_CROP_MISSING;
        else
            return ERROR_UNKNOWN;
    }
    else if (nrUnitsComputed < nrUnits)
    {
        if (isErrorMeteo)
            return WARNING_METEO;
        else if (isErrorSoil)
            return WARNING_SOIL;
        else if (isErrorCrop)
            return WARNING_CROP;
    }

    return CRIT3D_OK;
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


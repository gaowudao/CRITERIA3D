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
    outputCsvFileName = "";
    outputCsvPath = "";

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
    unit = new Crit1DUnit[unsigned(nr)];
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

    if (! loadVanGenuchtenParameters(&(irrForecast.dbSoil), irrForecast.soilTexture, &(projectError)))
        return ERROR_SOIL_PARAMETERS;

    if (! loadDriessenParameters(&(irrForecast.dbSoil), irrForecast.soilTexture, &(projectError)))
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

    // seasonal or short-term forecast
    projectSettings->endGroup();
    projectSettings->beginGroup("forecast");

    irrForecast.isSeasonalForecast = projectSettings->value("isSeasonalForecast",0).toBool();
    irrForecast.isShortTermForecast = projectSettings->value("isShortTermForecast",0).toBool();
    if ((irrForecast.isSeasonalForecast) || (irrForecast.isShortTermForecast))
    {
        outputCsvFileName = projectSettings->value("output").toString();

        if (outputCsvFileName.left(1) == ".")
            outputCsvFileName = path + outputCsvFileName;

        irrForecast.firstSeasonMonth = projectSettings->value("firstMonth",0).toInt();
    }

    if (irrForecast.isShortTermForecast)
    {
        irrForecast.daysOfForecast = projectSettings->value("daysOfForecast",0).toInt();
    }

    projectSettings->endGroup();
    return true;
}


void Criteria1DProject::closeAllDatabase()
{
    irrForecast.dbCrop.close();
    irrForecast.dbSoil.close();
    irrForecast.dbMeteo.close();
    irrForecast.dbForecast.close();
    irrForecast.dbOutput.close();
}


int Criteria1DProject::openAllDatabase()
{
    closeAllDatabase();

    logInfo ("Crop DB: " + dbCropName);
    if (! QFile(dbCropName).exists())
    {
        projectError = "DBparameters file doesn't exist";
        closeAllDatabase();
        return ERROR_DBPARAMETERS;
    }

    irrForecast.dbCrop = QSqlDatabase::addDatabase("QSQLITE", "parameters");
    irrForecast.dbCrop.setDatabaseName(dbCropName);
    if (! irrForecast.dbCrop.open())
    {
        projectError = "Open parameters DB failed: " + irrForecast.dbCrop.lastError().text();
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

    irrForecast.dbSoil = QSqlDatabase::addDatabase("QSQLITE", "soil");
    irrForecast.dbSoil.setDatabaseName(dbSoilName);
    if (! irrForecast.dbSoil.open())
    {
        projectError = "Open soil DB failed: " + irrForecast.dbSoil.lastError().text();
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

    irrForecast.dbMeteo = QSqlDatabase::addDatabase("QSQLITE", "meteo");
    irrForecast.dbMeteo.setDatabaseName(dbMeteoName);
    if (! irrForecast.dbMeteo.open())
    {
        projectError = "Open meteo DB failed: " + irrForecast.dbMeteo.lastError().text();
        closeAllDatabase();
        return ERROR_DBMETEO_OBSERVED;
    }

    // meteo forecast
    if (irrForecast.isShortTermForecast)
    {
        logInfo ("Forecast DB: " + dbForecastName);
        if (! QFile(dbForecastName).exists())
        {
            projectError = "DBforecast file doesn't exist";
            closeAllDatabase();
            return ERROR_DBMETEO_FORECAST;
        }
        irrForecast.dbForecast = QSqlDatabase::addDatabase("QSQLITE", "forecast");
        irrForecast.dbForecast.setDatabaseName(dbForecastName);
        if (! irrForecast.dbForecast.open())
        {
            projectError = "Open forecast DB failed: " + irrForecast.dbForecast.lastError().text();
            closeAllDatabase();
            return ERROR_DBMETEO_FORECAST;
        }
    }

    // output DB (not used in seasonal forecast)
    if (! irrForecast.isSeasonalForecast)
    {
        QFile::remove(dbOutputName);
        logInfo ("Output DB: " + dbOutputName);
        irrForecast.dbOutput = QSqlDatabase::addDatabase("QSQLITE", "output");
        irrForecast.dbOutput.setDatabaseName(dbOutputName);

        QString outputDbPath = getFilePath(outputCsvFileName);
        if (!QDir(outputDbPath).exists())
             QDir().mkdir(outputDbPath);


        if (! irrForecast.dbOutput.open())
        {
            projectError = "Open output DB failed: " + irrForecast.dbOutput.lastError().text();
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


bool Criteria1DProject::initializeCsvOutputFile(QString dateOfForecast)
{
    if ((irrForecast.isSeasonalForecast) || (irrForecast.isShortTermForecast))
    {
        if (!QDir(outputCsvPath).exists())
            QDir().mkdir(outputCsvPath);

        // add date to filename (only for ShortTermForecast)
        if (irrForecast.isShortTermForecast)
        {
            outputCsvFileName = outputCsvFileName.left(outputCsvFileName.length()-4);
            outputCsvFileName += "_" + dateOfForecast + ".csv";
        }

        outputFile.open(outputCsvFileName.toStdString().c_str(), std::ios::out | std::ios::trunc);
        if ( outputFile.fail())
        {
            logError("open failure: " + QString(strerror(errno)) + '\n');
            return false;
        }
        else
        {
            logInfo("Output file: " + outputCsvFileName + "\n");
        }

        if (irrForecast.isSeasonalForecast)
            outputFile << "ID_CASE,CROP,SOIL,METEO,p5,p25,p50,p75,p95\n";

        else if(irrForecast.isShortTermForecast)
            outputFile << "dateForecast,ID_CASE,CROP,SOIL,METEO,deficit,readilyAvailableWater,"
                           "forecast7daysPrec,forecast7daysETc,forecast7daysIRR,previousAllSeasonIRR\n";
    }

    return true;
}


bool Criteria1DProject::runSeasonalForecast(unsigned int index, double irriRatio)
{
    if (irriRatio < EPSILON)
    {
        // No irrigation: nothing to do
        outputFile << unit[index].idCase.toStdString() << "," << unit[index].idCrop.toStdString() << ",";
        outputFile << unit[index].idSoil.toStdString() << "," << unit[index].idMeteo.toStdString();
        outputFile << ",0,0,0,0,0\n";
        return true;
    }

    if (! irrForecast.runModel(unit[index], projectError))
    {
        logError();
        return false;
    }

    outputFile << unit[index].idCase.toStdString() << "," << unit[index].idCrop.toStdString() << ",";
    outputFile << unit[index].idSoil.toStdString() << "," << unit[index].idMeteo.toStdString();
    // percentiles
    double percentile = sorting::percentile(irrForecast.seasonalForecasts, &(irrForecast.nrSeasonalForecasts), 5, true);
    outputFile << "," << percentile * irriRatio;
    percentile = sorting::percentile(irrForecast.seasonalForecasts, &(irrForecast.nrSeasonalForecasts), 25, false);
    outputFile << "," << percentile * irriRatio;
    percentile = sorting::percentile(irrForecast.seasonalForecasts, &(irrForecast.nrSeasonalForecasts), 50, false);
    outputFile << "," << percentile * irriRatio;
    percentile = sorting::percentile(irrForecast.seasonalForecasts, &(irrForecast.nrSeasonalForecasts), 75, false);
    outputFile << "," << percentile * irriRatio;
    percentile = sorting::percentile(irrForecast.seasonalForecasts, &(irrForecast.nrSeasonalForecasts), 95, false);
    outputFile << "," << percentile * irriRatio << "\n";
    outputFile.flush();

    return true;
}


bool Criteria1DProject::runShortTermForecast(QString dateForecastStr, unsigned int index, double irriRatio)
{
    if (! irrForecast.runModel(unit[index], projectError))
    {
        logError();
        return false;
    }

    std::string idCaseStr = unit[index].idCase.toStdString();

    // last Observed day: day before
    Crit3DDate lastObservedDate = Crit3DDate(dateForecastStr.toStdString()).addDays(-1);
    int lastIndex = irrForecast.myCase.meteoPoint.nrObsDataDaysD - irrForecast.daysOfForecast - 1;
    Crit3DDate firstDate = irrForecast.myCase.meteoPoint.obsDataD[0].date;
    Crit3DDate lastDate = irrForecast.myCase.meteoPoint.obsDataD[lastIndex].date;
    if (lastObservedDate < firstDate || lastObservedDate > lastDate)
    {
        logError(" wrong date.");
        return false;
    }

    std::string lastObservedDateStr = lastObservedDate.toStdString();
    std::string lastDateOfForecast = lastObservedDate.addDays(irrForecast.daysOfForecast).toStdString();

    // first date for annual irrigation
    Crit3DDate firstDateAllSeason;
    if (irrForecast.firstSeasonMonth <= lastObservedDate.month)
    {
        firstDateAllSeason = Crit3DDate(1, irrForecast.firstSeasonMonth, lastObservedDate.year);
    }
    else
    {
        firstDateAllSeason = Crit3DDate(1, irrForecast.firstSeasonMonth, lastObservedDate.year - 1);
    }

    std::string firstDateAllSeasonStr = firstDateAllSeason.toStdString();

    double prec = NODATA;
    double maxTranspiration = NODATA;
    double forecastIrrigation = NODATA;
    double previousIrrigation = NODATA;
    double readilyAvailWater = NODATA;
    double deficit = NODATA;

    std::string mySQLstr = "SELECT SUM(PREC) AS prec,"
                        " SUM(TRANSP_MAX) AS maxTransp, SUM(IRRIGATION) AS irr"
                        " FROM '" + idCaseStr + "'"
                        " WHERE DATE > '" + lastObservedDateStr + "'"
                        " AND DATE <= '" + lastDateOfForecast + "'";

    QString mySQL = QString::fromStdString(mySQLstr);

    QSqlQuery myQuery = irrForecast.dbOutput.exec(mySQL);

    if (myQuery.lastError().type() != QSqlError::NoError)
    {
        logError(mySQL + "\n" + myQuery.lastError().text());
    }
    else
    {
        myQuery.last();
        prec = myQuery.value("prec").toDouble();
        maxTranspiration = myQuery.value("maxTransp").toDouble();
        forecastIrrigation = myQuery.value("irr").toDouble();
    }

    mySQLstr = "SELECT RAW, DEFICIT FROM '"
            + idCaseStr + "'"
            " WHERE DATE = '" + lastObservedDateStr + "'";

    mySQL = QString::fromStdString(mySQLstr);

    myQuery = irrForecast.dbOutput.exec(mySQL);

    if (myQuery.lastError().type() != QSqlError::NoError)
        logError(mySQL + "\n" + myQuery.lastError().text());
    else
    {
        myQuery.last();
        readilyAvailWater = myQuery.value("RAW").toDouble();
        deficit = myQuery.value("DEFICIT").toDouble();
    }

    mySQLstr = "SELECT SUM(IRRIGATION) AS previousIrrigation FROM '"
            + idCaseStr + "'"
            " WHERE DATE <= '" + lastObservedDateStr + "'"
            " AND DATE >= '" + firstDateAllSeasonStr + "'";

    mySQL = QString::fromStdString(mySQLstr);

    myQuery = irrForecast.dbOutput.exec(mySQL);

    if (myQuery.lastError().type() != QSqlError::NoError)
        logError(mySQL + "\n" + myQuery.lastError().text());
    else
    {
        myQuery.last();
        previousIrrigation = myQuery.value("previousIrrigation").toDouble();
    }

    outputFile << dateForecastStr.toStdString();
    outputFile << "," << unit[index].idCase.toStdString();
    outputFile << "," << unit[index].idCrop.toStdString();
    outputFile << "," << unit[index].idSoil.toStdString();
    outputFile << "," << unit[index].idMeteo.toStdString();
    outputFile << "," << QString::number(deficit,'f',2).toStdString();
    outputFile << "," << QString::number(readilyAvailWater,'f',1).toStdString();
    outputFile << "," << QString::number(prec,'f',1).toStdString();
    outputFile << "," << QString::number(maxTranspiration,'f',1).toStdString();
    outputFile << "," << forecastIrrigation * irriRatio;
    outputFile << "," << previousIrrigation * irriRatio << "\n";
    outputFile.flush();

    return true;
}


int Criteria1DProject::compute(QString dateOfForecast)
{
    bool isErrorModel = false;
    bool isErrorSoil = false;
    bool isErrorCrop = false;
    long nrUnitsComputed = 0;

    try
    {
        for (int i = 0; i < nrUnits; i++)
        {
            // CROP
            unit[i].idCrop = getCropFromClass(&(irrForecast.dbCrop), "crop_class", "id_class",
                                                         unit[i].idCropClass, &(projectError)).toUpper();
            if (unit[i].idCrop == "")
            {
                logInfo("Unit " + unit[i].idCase + " " + unit[i].idCropClass + " ***** missing CROP *****");
                isErrorCrop = true;
                continue;
            }

            // IRRI_RATIO
            double irriRatio = double(getIrriRatioFromClass(&(irrForecast.dbCrop), "crop_class", "id_class",
                                                            unit[i].idCropClass, &(projectError)));
            if ((irrForecast.isSeasonalForecast || irrForecast.isShortTermForecast) && (int(irriRatio) == int(NODATA)))
            {
                logInfo("Unit " + unit[i].idCase + " " + unit[i].idCropClass + " ***** missing IRRIGATION RATIO *****");
                continue;
            }

            // SOIL
            unit[i].idSoil = getIdSoilString(&(irrForecast.dbSoil), unit[i].idSoilNumber, &(projectError));
            if (unit[i].idSoil == "")
            {
                logInfo("Unit " + unit[i].idCase + " Soil nr." + QString::number(unit[i].idSoilNumber) + " ***** missing SOIL *****");
                isErrorSoil = true;
                continue;
            }

            logInfo("Unit " + unit[i].idCase +" "+ unit[i].idCrop +" "+ unit[i].idSoil +" "+ unit[i].idMeteo);

            if (irrForecast.isSeasonalForecast)
            {
                if (runSeasonalForecast(i, irriRatio))
                    nrUnitsComputed++;
                else
                    isErrorModel = true;

                continue;
            }

            if(irrForecast.isShortTermForecast)
            {
                if (runShortTermForecast(dateOfForecast, i, irriRatio))
                    nrUnitsComputed++;
                else
                    isErrorModel = true;

                continue;
            }

            if (irrForecast.runModel(unit[i], projectError))
                nrUnitsComputed++;
            else
            {
                logError();
                isErrorModel = true;
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
        if (isErrorModel)
            return ERROR_METEO_OR_MODEL;
        else if (isErrorSoil)
            return ERROR_SOIL_MISSING;
        else if (isErrorCrop)
            return ERROR_CROP_MISSING;
        else
            return ERROR_UNKNOWN;
    }
    else if (nrUnitsComputed < nrUnits)
    {
        if (isErrorModel)
            return WARNING_METEO_OR_MODEL;
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


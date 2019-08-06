#include <QCoreApplication>
#include <QVariant>
#include <QSqlError>
#include <QSqlQuery>
#include <QDir>
#include <QDate>

#include "basicMath.h"
#include "project.h"
#include "modelCore.h"
#include "cropDbTools.h"
#include "soilDbTools.h"
#include "commonConstants.h"


int main(int argc, char *argv[])
{
    QCoreApplication myApp(argc, argv);

    Criteria1DProject myProject;
    QString settingsFileName;
    int indexOfForecast;
    Crit3DDate dateOfForecast, firstDateAllSeason;
    QString dateOfForecastStr, IrrPreviousDateStr, firstDateAllSeasonStr, mySQL;
    QSqlQuery myQuery;
    float RAW, rootDepth, prec, maxTranspiration;
    float forecastIrrigation, previousAllIrrigation;
    double irriRatio;
    double percentile;

    if (argc > 1)
        settingsFileName = argv[1];
    else
    {
        settingsFileName = "../../../DATA/PROJECT/swamp/swamp.ini";

        //myProject.logError("USAGE: CRITERIA1D filename.ini");
        //return ERROR_SETTINGS_MISSING;
    }

    if (settingsFileName.left(1) == ".")
        settingsFileName = myApp.applicationDirPath() + "/" + settingsFileName;

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

    // Initialize irrigation output file
    if ((myProject.criteria.isSeasonalForecast) || (myProject.criteria.isShortTermForecast))
    {
        if (!QDir(myProject.irrigationPath).exists())
             QDir().mkdir(myProject.irrigationPath);

        // Add date to filename (only for ShortTermForecast)
        if (myProject.criteria.isShortTermForecast)
        {
            myProject.irrigationFileName = myProject.irrigationFileName.left(myProject.irrigationFileName.length()-4);
            myProject.irrigationFileName += "_" + QDate::currentDate().toString("yyyyMMdd") + ".csv";
        }

        myProject.outputFile.open(myProject.irrigationFileName.toStdString().c_str(), std::ios::out | std::ios::trunc);
        if ( myProject.outputFile.fail())
        {
            myProject.logError("open failure: " + QString(strerror(errno)) + '\n');
            return ERROR_DBOUTPUT;
        }
        else
        {
           myProject.logInfo("Output file: " + myProject.irrigationFileName);
        }

        if (myProject.criteria.isSeasonalForecast)
            myProject.outputFile << "ID_CASE,CROP,SOIL,METEO,p5,p25,p50,p75,p95\n";

        else if(myProject.criteria.isShortTermForecast)
            myProject.outputFile << "dateForecast,ID_CASE,CROP,SOIL,METEO,"
                                    "readilyAvailableWater,rootDepth,"
                                    "forecast7daysPrec,forecast7daysETc,forecast7daysIRR,"
                                    "previousAllSeasonIRR\n";
    }

    bool isErrorMeteo = false;
    bool isErrorSoil = false;
    bool isErrorCrop = false;
    long nrUnitsComputed = 0;

    try
    {
        for (int i = 0; i < myProject.nrUnits; i++)
        {
            //CROP
            myProject.unit[i].idCrop = getCropFromClass(&(myProject.criteria.dbParameters), "crop_class", "id_class",
                                                        myProject.unit[i].idCropClass, &(myProject.projectError)).toUpper();
            if (myProject.unit[i].idCrop == "")
            {
                myProject.logInfo("Unit " + myProject.unit[i].idCase + " " + myProject.unit[i].idCropClass + " ***** missing CROP *****");
                isErrorCrop = true;
            }
            else
            {
                //IRRI_RATIO
                irriRatio = double(getIrriRatioFromClass(&(myProject.criteria.dbParameters), "crop_class", "id_class",
                                                  myProject.unit[i].idCropClass, &(myProject.projectError)));
                if (int(irriRatio) == int(NODATA))
                    myProject.logInfo("Unit " + myProject.unit[i].idCase + " " + myProject.unit[i].idCropClass + " ***** missing IRRIGATION RATIO *****");
                else
                {
                    //SOIL
                    myProject.unit[i].idSoil = getIdSoilString(&(myProject.criteria.dbSoil), myProject.unit[i].idSoilNumber, &(myProject.projectError));
                    if (myProject.unit[i].idSoil == "")
                    {
                        myProject.logInfo("Unit " + myProject.unit[i].idCase + " Soil nr." + QString::number(myProject.unit[i].idSoilNumber) + " ***** missing SOIL *****");
                        isErrorSoil = true;
                    }
                    else
                    {
                        // LOG
                        myProject.logInfo("Unit " + myProject.unit[i].idCase +" "+ myProject.unit[i].idCrop +" "+ myProject.unit[i].idSoil +" "+ myProject.unit[i].idMeteo);

                        // SEASONAL
                        if (myProject.criteria.isSeasonalForecast)
                        {
                            if (irriRatio < EPSILON)
                            {
                                myProject.outputFile << myProject.unit[i].idCase.toStdString() << "," << myProject.unit[i].idCrop.toStdString() << ",";
                                myProject.outputFile << myProject.unit[i].idSoil.toStdString() << "," << myProject.unit[i].idMeteo.toStdString();
                                myProject.outputFile << ",0,0,0,0,0\n";
                            }
                            else
                            {
                                if (! runModel(&(myProject.criteria), &(myProject.unit[i]), &(myProject.projectError)))
                                {
                                    myProject.logError();
                                    // TODO Improve
                                    isErrorMeteo = true;
                                }
                                else
                                {
                                    myProject.outputFile << myProject.unit[i].idCase.toStdString() << "," << myProject.unit[i].idCrop.toStdString() << ",";
                                    myProject.outputFile << myProject.unit[i].idSoil.toStdString() << "," << myProject.unit[i].idMeteo.toStdString();
                                    // percentiles
                                    percentile = sorting::percentile(myProject.criteria.seasonalForecasts, &(myProject.criteria.nrSeasonalForecasts), 5, true);
                                    myProject.outputFile << "," << percentile * irriRatio;
                                    percentile = sorting::percentile(myProject.criteria.seasonalForecasts, &(myProject.criteria.nrSeasonalForecasts), 25, false);
                                    myProject.outputFile << "," << percentile * irriRatio;
                                    percentile = sorting::percentile(myProject.criteria.seasonalForecasts, &(myProject.criteria.nrSeasonalForecasts), 50, false);
                                    myProject.outputFile << "," << percentile * irriRatio;
                                    percentile = sorting::percentile(myProject.criteria.seasonalForecasts, &(myProject.criteria.nrSeasonalForecasts), 75, false);
                                    myProject.outputFile << "," << percentile * irriRatio;
                                    percentile = sorting::percentile(myProject.criteria.seasonalForecasts, &(myProject.criteria.nrSeasonalForecasts), 95, false);
                                    myProject.outputFile << "," << percentile * irriRatio << "\n";
                                    myProject.outputFile.flush();

                                    nrUnitsComputed++;
                                }
                            }
                        }
                        else
                        {
                            if (runModel(&(myProject.criteria), &(myProject.unit[i]), &(myProject.projectError)))
                            {
                                nrUnitsComputed++;

                                // SHORT TERM FORECAST
                                if(myProject.criteria.isShortTermForecast)
                                {
                                    indexOfForecast = myProject.criteria.meteoPoint.nrObsDataDaysD - myProject.criteria.daysOfForecast - 1;
                                    dateOfForecast = myProject.criteria.meteoPoint.obsDataD[indexOfForecast].date;
                                    dateOfForecastStr = QString::fromStdString(dateOfForecast.toStdString());
                                    IrrPreviousDateStr = QString::fromStdString(dateOfForecast.addDays(-13).toStdString());

                                    // first date for annual irrigation
                                    if (myProject.criteria.firstSeasonMonth <= dateOfForecast.month)
                                        firstDateAllSeason = Crit3DDate(1, myProject.criteria.firstSeasonMonth, dateOfForecast.year);
                                    else
                                        firstDateAllSeason = Crit3DDate(1, myProject.criteria.firstSeasonMonth, dateOfForecast.year - 1);

                                    firstDateAllSeasonStr = QString::fromStdString(firstDateAllSeason.toStdString());

                                    prec = NODATA;
                                    maxTranspiration = NODATA;
                                    forecastIrrigation = NODATA;
                                    previousAllIrrigation = NODATA;
                                    RAW = NODATA;
                                    rootDepth = NODATA;

                                    mySQL = "SELECT SUM(PREC) AS prec,"
                                            " SUM(TRANSP_MAX) AS maxTransp, SUM(IRRIGATION) AS irr"
                                            " FROM '" + myProject.unit[i].idCase + "'"
                                            " WHERE DATE > '" + dateOfForecastStr + "'";

                                    myQuery = myProject.criteria.dbOutput.exec(mySQL);

                                    if (myQuery.lastError().type() != QSqlError::NoError)
                                        myProject.logError("SELECT SUM(PREC)\n" + myQuery.lastError().text());
                                    else
                                    {
                                        myQuery.last();
                                        prec = myQuery.value("prec").toFloat();
                                        maxTranspiration = myQuery.value("maxTransp").toFloat();
                                        forecastIrrigation = myQuery.value("irr").toFloat();
                                    }

                                    mySQL = "SELECT RAW, DEFICIT, ROOTDEPTH FROM '" + myProject.unit[i].idCase + "'"
                                            " WHERE DATE = '" + dateOfForecastStr + "'";
                                    myQuery = myProject.criteria.dbOutput.exec(mySQL);

                                    if (myQuery.lastError().type() != QSqlError::NoError)
                                        myProject.logError("SELECT RAW, DEFICIT, ROOTDEPTH\n" + myQuery.lastError().text());
                                    else
                                    {
                                        myQuery.last();
                                        RAW = myQuery.value("RAW").toFloat();
                                        rootDepth = myQuery.value("ROOTDEPTH").toFloat();
                                    }


                                    mySQL = "SELECT SUM(IRRIGATION) AS prevAllIrr FROM '" + myProject.unit[i].idCase + "'"
                                            " WHERE DATE <= '" + dateOfForecastStr + "'"
                                            " AND DATE >= '" + firstDateAllSeasonStr + "'";
                                    myQuery = myProject.criteria.dbOutput.exec(mySQL);

                                    if (myQuery.lastError().type() != QSqlError::NoError)
                                        myProject.logError("SELECT SUM(IRRIGATION) (all season) \n" + myQuery.lastError().text());
                                    else
                                    {
                                        myQuery.last();
                                        previousAllIrrigation = myQuery.value("prevAllIrr").toFloat();
                                    }

                                    myProject.outputFile << dateOfForecast.toStdString();
                                    myProject.outputFile << "," << myProject.unit[i].idCase.toStdString();
                                    myProject.outputFile << "," << myProject.unit[i].idCrop.toStdString();
                                    myProject.outputFile << "," << myProject.unit[i].idSoil.toStdString();
                                    myProject.outputFile << "," << myProject.unit[i].idMeteo.toStdString();
                                    myProject.outputFile << "," << QString::number(double(RAW),'f',1).toStdString();
                                    myProject.outputFile << "," << QString::number(double(rootDepth),'f',2).toStdString();
                                    myProject.outputFile << "," << QString::number(double(prec),'f',1).toStdString();
                                    myProject.outputFile << "," << QString::number(double(maxTranspiration),'f',1).toStdString();
                                    myProject.outputFile << "," << double(forecastIrrigation) * irriRatio;
                                    myProject.outputFile << "," << double(previousAllIrrigation) * irriRatio << "\n";
                                    myProject.outputFile.flush();
                                }
                            }
                            else
                            {
                                myProject.logError();
                                // TODO Improve
                                isErrorMeteo = true;
                            }   
                        }
                    }
                }
            }
        }

        myProject.outputFile.close();

    } catch (std::exception &e)
    {
        qFatal("Error %s ", e.what());

    } catch (...)
    {
        qFatal("Error <unknown>");
        return ERROR_UNKNOWN;
    }

    myProject.logInfo("\nEND");

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
    else if (nrUnitsComputed < myProject.nrUnits)
    {
        if (isErrorMeteo)
            return WARNING_METEO;
        else if (isErrorSoil)
            return WARNING_SOIL;
        else if (isErrorCrop)
            return WARNING_CROP;
    }

    else return CRIT3D_OK;
}

#include <QSqlQuery>
#include <QSqlError>
#include <QDate>
#include <QVariant>
#include <QDebug>
#include <iostream>

#include "commonConstants.h"
#include "crit3dDate.h"
#include "Criteria1D.h"
#include "utilities.h"
#include "dbTools.h"
#include "dbToolsMOSES.h"
#include "meteo.h"
#include "root.h"


Criteria1DUnit::Criteria1DUnit()
{
    this->idCase = "";
    this->idCrop = "";
    this->idSoil = "";
    this->idMeteo = "";

    this->idForecast = "";
    this->idSoilNumber = NODATA;
    this->idCropMoses = "";
}


Criteria1D::Criteria1D()
{
    this->idCase = "";
    this->outputString = "";

    this->layer = NULL;
    this->nrLayers = 0;
    this->layerThickness = 0.02;          // [m] thickness = 2cm

    this->maxSimulationDepth = 2.0;       // [m]

    this->isGeometricLayer = false;

    this->depthPloughedSoil = 0.5;        // [m]
    this->initialAW[0] = 0.85;            // [-] of available Water (ploughed soil)
    this->initialAW[1] = 0.8;             // [-] of available Water (deep soil)

    this->optimizeIrrigation = false;

    this->isSeasonalForecast = false;
    this->firstSeasonMonth = NODATA;
    this->nrSeasonalForecasts = 0;
    this->seasonalForecasts = NULL;

    this->isShortTermForecast = false;
    this->daysOfForecast = NODATA;
}


void Criteria1DOutput::initializeDaily()
{
    this->dailyPrec = 0.0;
    this->dailyDrainage = 0.0;
    this->dailySurfaceRunoff = 0.0;
    this->dailyLateralDrainage = 0.0;
    this->dailyIrrigation = 0.0;
    this->dailyEt0 = 0.0;
    this->dailyEvaporation = 0.0;
    this->dailyMaxTranspiration = 0.0;
    this->dailyMaxEvaporation = 0.0;
    this->dailyTranspiration = 0.0;
    this->dailyCropAvailableWater = 0.0;
    this->dailySoilWaterDeficit = 0.0;
    this->dailyCapillaryRise = 0.0;
    this->dailyWaterTable = NODATA;
    this->dailyKc = 0.0;
}


Criteria1DOutput::Criteria1DOutput()
{
    this->initializeDaily();
}


bool Criteria1D::setSoil(QString idSoil, std::string *myError)
{
    // load Soil
    if (! loadSoil (&dbSoil, idSoil, &mySoil, &(soilTexture[0]), myError))
        return false;

    // nr of layers
    this->nrLayers = ceil(mySoil.totalDepth / layerThickness) + 1;

    // alloc memory
    if (this->layer != NULL)
        free(this->layer);
    this->layer = (soil::Crit3DLayer *) calloc(nrLayers, sizeof(soil::Crit3DLayer));

    double soilFraction, hygroscopicHumidity;
    int horizonIndex;
    double depth = this->layerThickness / 2.0;

    // initialize layers
    layer[0].depth = 0.0;
    layer[0].thickness = 0.0;
    for (int i = 1; i < this->nrLayers; i++)
    {
        horizonIndex = soil::getHorizonIndex(&(mySoil), depth);
        layer[i].horizon = &(mySoil.horizon[horizonIndex]);

        soilFraction = (1.0 - layer[i].horizon->coarseFragments);
        layer[i].soilFraction = soilFraction;       // [-]

        // TODO geometric layer
        layer[i].depth = depth;                     // [m]
        layer[i].thickness = this->layerThickness;  // [m]

        //[mm]
        layer[i].SAT = mySoil.horizon[horizonIndex].vanGenuchten.thetaS * soilFraction * layer[i].thickness * 1000.0;

        //[mm]
        layer[i].FC = mySoil.horizon[horizonIndex].waterContentFC * soilFraction * layer[i].thickness * 1000.0;
        layer[i].critical = layer[i].FC;

        //[mm]
        layer[i].WP = mySoil.horizon[horizonIndex].waterContentWP * soilFraction * layer[i].thickness * 1000.0;

        // hygroscopic humidity: -2000 kPa
        hygroscopicHumidity = soil::thetaFromSignPsi(-2000, &(mySoil.horizon[horizonIndex]));

        //[mm]
        layer[i].HH = hygroscopicHumidity * soilFraction * layer[i].thickness * 1000.0;

        depth += layer[i].thickness;              //[m]
    }

    return(true);
}


bool Criteria1D::loadMOSESMeteo(QString idMeteo, QString idForecast, std::string *myError)
{
    QString queryString = "SELECT * FROM meteo_locations";
    queryString += " WHERE id_meteo='" + idMeteo + "'";

    QSqlQuery query = dbMeteo.exec(queryString);
    query.last();
    if (! query.isValid())
    {
        if (query.lastError().number() > 0)
            *myError = "dbMeteo error: " + query.lastError().text().toStdString();
        else
            *myError = "Missing meteo location:" + idMeteo.toStdString();
        return(false);
    }

    QString tableName = query.value("table_name").toString();

    double myLat, myLon;
    if (getValue(query.value(("latitude")), &myLat))
        this->meteoPoint.latitude = myLat;
    else
    {
        *myError = "Missing latitude in idMeteo: " + idMeteo.toStdString();
        return false;
    }

    if (getValue(query.value(("longitude")), &myLon))
        this->meteoPoint.longitude = myLon;

    queryString = "SELECT * FROM " + tableName + " ORDER BY [date]";
    query = this->dbMeteo.exec(queryString);
    query.last();

    if (! query.isValid())
    {
        if (query.lastError().number() > 0)
            *myError = "dbMeteo error: " + query.lastError().text().toStdString();
        else
            *myError = "Missing meteo table:" + tableName.toStdString();
        return false;
    }

    query.first();
    QDate firstObsDate = query.value("date").toDate();
    query.last();
    QDate lastObsDate = query.value("date").toDate();

    int nrDays = firstObsDate.daysTo(lastObsDate) + 1;

    // Is Forecast: increase nr of days
    if (this->isShortTermForecast)
        nrDays += this->daysOfForecast;

    // Initialize data
    this->meteoPoint.initializeObsDataD(nrDays, getCrit3DDate(firstObsDate));

    // Read observed data
    if (! readMOSESDailyData(&query, &meteoPoint, myError)) return false;

    // Add Short-Term forecast
    if (this->isShortTermForecast)
    {
        QString queryString = "SELECT * FROM meteo_locations";
        queryString += " WHERE id_meteo='" + idForecast + "'";
        QSqlQuery query = this->dbForecast.exec(queryString);
        query.last();
        if (! query.isValid())
        {
            if (query.lastError().number() > 0)
                *myError = "dbForecast error: " + query.lastError().text().toStdString();
            else
                *myError = "Missing forecast location:" + idForecast.toStdString();
            return false;
        }
        QString tableName = query.value("table_name").toString();

        query.clear();
        queryString = "SELECT * FROM " + tableName + " ORDER BY [date]";
        query = this->dbForecast.exec(queryString);
        query.last();

        //check query
        if (! query.isValid())
        {
            if (query.lastError().number() > 0)
                *myError = "dbForecast error: " + query.lastError().text().toStdString();
            else
                *myError = "Missing forecast table:" + tableName.toStdString();
            return false;
        }

        // check date
        query.first();
        QDate myDate = query.value("date").toDate();
        if (myDate != lastObsDate.addDays(1))
        {
            *myError = "The forecast date doesn't match with observed data.";
            return false;
        }

        // Read forecast data
        if (! readMOSESDailyData(&query, &meteoPoint, myError)) return false;

        // TODO Watertable data
        // check last watertable data (last 15 days)
        // se presente: estendi il dato sino ultimo giorno
    }

    return true;
}


// alloc memory for annual values of irrigation
void Criteria1D::initializeSeasonalForecast(const Crit3DDate& firstDate, const Crit3DDate& lastDate)
{
    if (isSeasonalForecast)
    {
        if (seasonalForecasts != NULL) free(seasonalForecasts);

        nrSeasonalForecasts = lastDate.year - firstDate.year +1;
        seasonalForecasts = (double*) calloc(nrSeasonalForecasts, sizeof(double));
        for (int i = 0; i < nrSeasonalForecasts; i++)
            seasonalForecasts[i] = NODATA;
    }
}


bool Criteria1D::createOutputTable(std::string* myError)
{
    QString queryString = "DROP TABLE '" + this->idCase + "'";
    QSqlQuery myQuery = this->dbOutput.exec(queryString);

    if (myQuery.lastError().number() > 0)
    {
        *myError = "Error in dropping table: " + this->idCase.toStdString() + "\n" + myQuery.lastError().text().toStdString();
    }

    queryString = "CREATE TABLE '" + this->idCase + "'"
            + " ( DATE TEXT,"
            + " PREC REAL, IRRIGATION REAL, RAW REAL, DEFICIT REAL, DRAINAGE REAL, RUNOFF REAL, ET0 REAL,"
            + " EVAP_MAX REAL, TRANSP_MAX, EVAP REAL, TRANSP REAL, LAI REAL, KC REAL, ROOTDEPTH REAL )";
    myQuery = this->dbOutput.exec(queryString);

    if (myQuery.lastError().number() > 0)
    {
        *myError = "Error in creating table: " + this->idCase.toStdString() + "\n" + myQuery.lastError().text().toStdString();
        return false;
    }

    return true;
}


void Criteria1D::prepareOutput(Crit3DDate myDate, bool isFirst)
{
    if (isFirst)
        this->outputString = "INSERT INTO '" + this->idCase + "'"
            + " (DATE, PREC, IRRIGATION, RAW, DEFICIT, DRAINAGE, RUNOFF, ET0,"
            + " EVAP_MAX, TRANSP_MAX, EVAP, TRANSP, LAI, KC, ROOTDEPTH) "
            + " VALUES ";
    else
        this->outputString += ",";

    this->outputString
            += "('" + QString::fromStdString(myDate.toStdString()) + "'"
            + "," + QString::number(this->output.dailyPrec)
            + "," + QString::number(this->output.dailyIrrigation)
            + "," + QString::number(this->output.dailyCropAvailableWater)
            + "," + QString::number(this->output.dailySoilWaterDeficit)
            + "," + QString::number(this->output.dailyDrainage)
            + "," + QString::number(this->output.dailySurfaceRunoff)
            + "," + QString::number(this->output.dailyEt0)
            + "," + QString::number(this->output.dailyMaxEvaporation)
            + "," + QString::number(this->output.dailyMaxTranspiration)
            + "," + QString::number(this->output.dailyEvaporation)
            + "," + QString::number(this->output.dailyTranspiration)
            + "," + QString::number(this->myCrop.LAI)
            + "," + QString::number(this->output.dailyKc)
            + "," + QString::number(this->myCrop.roots.rootDepth)
            + ")";
}


bool Criteria1D::saveOutput(std::string* myError)
{
    QSqlQuery myQuery = this->dbOutput.exec(this->outputString);

    if (myQuery.lastError().type() != QSqlError::NoError)
    {
        *myError = "Error in saving output:\n" + myQuery.lastError().text().toStdString();
        return false;
    }

    return true;
}

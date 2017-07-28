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
    this->dailySubsurfaceRunoff = 0.0;
    this->dailyIrrigation = 0.0;
    this->dailyEt0 = 0.0;
    this->dailyEvaporation = 0.0;
    this->dailyMaxTranspiration = 0.0;
    this->dailyMaxEvaporation = 0.0;
    this->dailyTranspiration = 0.0;
    this->dailyCropAvailableWater = 0.0;
    this->dailySoilWaterDeficit = 0.0;
}

Criteria1DOutput::Criteria1DOutput()
{
    this->initializeDaily();
}



bool Criteria1D::loadSoil(std::string idSoilStr, std::string *myError)
{
    QString soilCode = QString::fromStdString(idSoilStr);

    QString queryString = "SELECT * FROM horizons ";
    queryString += "WHERE soil_code='" + soilCode + "' ORDER BY horizon_nr";

    QSqlQuery query = this->dbSoil.exec(queryString);
    query.last();

    if (! query.isValid())
    {
        if (query.lastError().number() > 0)
            *myError = "dbSoil error: " + query.lastError().text().toStdString();
        else
            *myError = "Missing soil:" + idSoilStr;
        return(false);
    }

    int nrHorizons = query.at() + 1;     //SQLITE doesn't support SIZE
    mySoil.cleanSoil();
    mySoil = soil::Crit3DSoil(1, nrHorizons);

    int idTextureUSDA, idTextureNL, idHorizon;
    double sand, silt, clay, organicMatter, coarseFragments;
    double lowerDepth, upperDepth, bulkDensity, theta_sat, ksat;
    int i = 0;
    query.first();
    do
    {
        // horizon depth
        idHorizon = query.value("horizon_nr").toInt();
        getValue(query.value("upper_depth"), &upperDepth);
        getValue(query.value("lower_depth"), &lowerDepth);

        // [cm]->[m]
        mySoil.horizon[i].upperDepth = upperDepth / 100.;
        mySoil.horizon[i].lowerDepth = lowerDepth / 100.;

        if ((mySoil.horizon[i].upperDepth == NODATA) || (mySoil.horizon[i].lowerDepth == NODATA)
            || (mySoil.horizon[i].lowerDepth < mySoil.horizon[i].upperDepth)
            || ((idHorizon == 1) && (mySoil.horizon[i].upperDepth > 0))
            || ((idHorizon > 1) && (mySoil.horizon[i].upperDepth > mySoil.horizon[i-1].lowerDepth)))
        {
            *myError = "Wrong depth in soil:" + idSoilStr + " horizon nr:" + QString::number(idHorizon).toStdString();
            return(false);
        }

        // coarse fragments %
        getValue(query.value("coarse_fragment"), &coarseFragments);
        if (coarseFragments == NODATA)
            mySoil.horizon[i].coarseFragments = 0.;    //default = no coarse fragment
        else
            //[0,1]
            mySoil.horizon[i].coarseFragments = coarseFragments / 100.0;

        // sand silt clay [-]
        getValue(query.value("sand"), &sand);
        if (sand < 1) sand *= 100.0;
        getValue(query.value("silt"), &silt);
        if (silt < 1) silt *= 100.0;
        getValue(query.value("clay"), &clay);
        if (clay < 1) clay *= 100.0;

        // texture
        idTextureUSDA = soil::getUSDATextureClass(sand, silt, clay);
        if (idTextureUSDA == NODATA)
        {
                *myError = "Texture wrong! sand+silt+clay <> 1 in soil:"
                        + idSoilStr + " horizon nr:" + QString::number(idHorizon).toStdString();
                return (false);
        }
        idTextureNL =  soil::getNLTextureClass(sand, silt, clay);

        mySoil.horizon[i].texture.sand = sand;
        mySoil.horizon[i].texture.silt = silt;
        mySoil.horizon[i].texture.clay = clay;
        mySoil.horizon[i].texture.classUSDA = idTextureUSDA;
        mySoil.horizon[i].texture.classNL = idTextureNL;
        mySoil.horizon[i].vanGenuchten = this->soilTexture[idTextureUSDA].vanGenuchten;
        mySoil.horizon[i].waterConductivity = this->soilTexture[idTextureUSDA].waterConductivity;
        mySoil.horizon[i].Driessen = this->soilTexture[idTextureNL].Driessen;

        // organic matter (%)
        getValue(query.value("organic_matter"), &organicMatter);
        if ((organicMatter == NODATA) || (organicMatter == 0.0))
            organicMatter = 0.005;       //default: 0.5%
        else
            organicMatter /= 100.0;     //[-]
        mySoil.horizon[i].organicMatter = organicMatter;

        // bulk density and porosity
        getValue(query.value("bulk_density"), &bulkDensity);
        if (bulkDensity <= 0) bulkDensity = NODATA;
        getValue(query.value("theta_sat"), &theta_sat);
        if (theta_sat <= 0) theta_sat = NODATA;

        if ((theta_sat == NODATA) && (bulkDensity != NODATA))
            theta_sat = soil::estimateTotalPorosity(&(mySoil.horizon[i]), bulkDensity);

        if (theta_sat != NODATA)
            mySoil.horizon[i].vanGenuchten.thetaS = theta_sat;

        if (bulkDensity == NODATA)
            bulkDensity = soil::estimateBulkDensity(&(mySoil.horizon[i]), theta_sat);

        mySoil.horizon[i].bulkDensity = bulkDensity;

        // SATURATED CONDUCTIVITY (cm/day)
        getValue(query.value("ksat"), &ksat);
        if ((ksat == NODATA) || (ksat <= 0))
            ksat = soil::estimateSaturatedConductivity(&(mySoil.horizon[i]), bulkDensity);
        /*else
        {
            if ((ksat < (mySoil.horizon[i].waterConductivity.kSat / 10.)) || (ksat > (mySoil.horizon[i].waterConductivity.kSat * 10.)))
                logInfo("WARNING: Ksat out of class limit, in soil " + idSoil + " horizon " + QString::number(i));
        }*/

        mySoil.horizon[i].waterConductivity.kSat = ksat;

        mySoil.horizon[i].fieldCapacity = soil::getFieldCapacity(&(mySoil.horizon[i]), soil::KPA);
        mySoil.horizon[i].wiltingPoint = soil::getWiltingPoint(soil::KPA);
        mySoil.horizon[i].waterContentFC = soil::getThetaFC(&(mySoil.horizon[i]));
        mySoil.horizon[i].waterContentWP = soil::getThetaWP(&(mySoil.horizon[i]));

        i++;
    } while(query.next());

    mySoil.totalDepth = mySoil.horizon[nrHorizons-1].lowerDepth;
    if (mySoil.totalDepth < 0.5)
    {
            *myError = "Texture wrong! soil depth < 50cm:" + idSoilStr;
            return (false);
    }

    // set layers
    this->nrLayers = ceil(mySoil.totalDepth / layerThickness) + 1;
    if (this->layer != NULL) free(this->layer);
    this->layer = (soil::Crit3DLayer *) calloc(nrLayers, sizeof(soil::Crit3DLayer));

    // initialize layers
    layer[0].depth = 0.0;
    layer[0].thickness = 0.0;
    double soilFraction, hygroscopicHumidity;
    int horizonIndex;
    double depth = this->layerThickness / 2.0;
    for (i = 1; i < this->nrLayers; i++)
    {
        horizonIndex = soil::getHorizonIndex(&(mySoil), depth);
        layer[i].horizon = &(mySoil.horizon[horizonIndex]);
        soilFraction = (1.0 - layer[i].horizon->coarseFragments);
        layer[i].soilFraction = soilFraction;       // [-]
        layer[i].depth = depth;                     // [m]
        layer[i].thickness = this->layerThickness;  // [m]

        //[mm]
        layer[i].SAT = mySoil.horizon[horizonIndex].vanGenuchten.thetaS * soilFraction * this->layerThickness * 1000.0;

        layer[i].FC = mySoil.horizon[horizonIndex].waterContentFC * soilFraction * this->layerThickness * 1000.0;
        layer[i].critical = layer[i].FC;

        layer[i].WP = mySoil.horizon[horizonIndex].waterContentWP * soilFraction * this->layerThickness * 1000.0;

        // hygroscopic humidity: -2000 kPa
        hygroscopicHumidity = soil::thetaFromSignPsi(-2000, &(mySoil.horizon[horizonIndex]));
        layer[i].HH = hygroscopicHumidity * soilFraction * this->layerThickness * 1000.0;

        depth += this->layerThickness;
    }

    return(true);
}


bool Criteria1D::loadMeteo(QString idMeteo, QString idForecast, std::string *myError)
{
    QString queryString = "SELECT * FROM meteo_locations";
    queryString += " WHERE id_meteo='" + idMeteo + "'";
    QSqlQuery query = this->dbMeteo.exec(queryString);
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

    // increase nr of days (forecast)
    if (this->isShortTermForecast)
        nrDays += this->daysOfForecast;

    this->meteoPoint.initializeObsDataD(nrDays, getCrit3DDate(firstObsDate));

    if (! readMeteoData(&query, myError)) return false;

    // SHORT TERM FORECAST
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

        //check date
        query.first();
        QDate myDate = query.value("date").toDate();
        if (myDate != lastObsDate.addDays(1))
        {
            *myError = "The forecast date doesn't match with observed data.";
            return false;
        }

        if (! readMeteoData(&query, myError)) return false;
    }

    return true;
}


bool Criteria1D::readMeteoData(QSqlQuery * query, std::string *myError)
{
    const int MAX_MISSING_DAYS = 3;
    float tmed, prec, et0;
    QDate myDate, expectedDate, previousDate;
    Crit3DDate date;

    float tmin = NODATA;
    float tmax = NODATA;
    float prevTmin = NODATA;
    float prevTmax = NODATA;
    int nrMissingTemp = 0;
    int nrMissingPrec = 0;

    query->first();
    myDate = query->value("date").toDate();
    expectedDate = myDate;
    previousDate = myDate.addDays(-1);

    do
    {
        myDate = query->value("date").toDate();

        if (! myDate.isValid())
        {
            *myError = "Wrong date format: " + query->value("date").toString().toStdString();
            return false;
        }

        if (myDate != previousDate)
        {
            if (myDate != expectedDate)
            {
                if (expectedDate.daysTo(myDate) > MAX_MISSING_DAYS)
                {
                    *myError = "Wrong METEO: too many missing data." + expectedDate.toString().toStdString();
                    return false;
                }
                else
                {
                    // fill missing data
                    while (myDate != expectedDate)
                    {
                        tmin = prevTmin;
                        tmax = prevTmax;
                        tmed = (tmin + tmax) * 0.5;
                        prec = 0;
                        et0 = NODATA;

                        date = getCrit3DDate(expectedDate);
                        this->meteoPoint.setMeteoPointValueD(date, dailyAirTemperatureMin, tmin);
                        this->meteoPoint.setMeteoPointValueD(date, dailyAirTemperatureMax, tmax);
                        this->meteoPoint.setMeteoPointValueD(date, dailyAirTemperatureAvg, tmed);
                        this->meteoPoint.setMeteoPointValueD(date, dailyPrecipitation, prec);
                        this->meteoPoint.setMeteoPointValueD(date, dailyPotentialEvapotranspiration, et0);

                        expectedDate = expectedDate.addDays(1);
                    }
                }
            }

            prevTmax = tmax;
            prevTmin = tmin;

            // mandatory
            getValue(query->value("tmin"), &tmin);
            getValue(query->value("tmax"), &tmax);
            if ((tmin == NODATA) || (tmax == NODATA))
            {
                if (nrMissingTemp < MAX_MISSING_DAYS)
                {
                    nrMissingTemp++;
                    if (tmin == NODATA) tmin = prevTmin;
                    if (tmax == NODATA) tmax = prevTmax;
                }
                else
                {
                    *myError = "Wrong METEO: too many missing data " + myDate.toString().toStdString();
                    return false;
                }
            }
            else nrMissingTemp = 0;

            getValue(query->value("prec"), &prec);
            if (prec == NODATA)
            {
                if (nrMissingPrec < MAX_MISSING_DAYS)
                {
                    nrMissingPrec++;
                    prec = 0;
                }
                else
                {
                    *myError = "Wrong METEO: too many missing data " + myDate.toString().toStdString();
                    return false;
                }
            }
            else nrMissingPrec = 0;

            // not mandatory
            getValue(query->value("tavg"), &tmed);
            getValue(query->value("etp"), &et0);
            if (tmed == NODATA) tmed = (tmin + tmax) * 0.5;

            date = getCrit3DDate(myDate);
            if (this->meteoPoint.obsDataD[0].date.daysTo(date) < this->meteoPoint.nrObsDataDaysD)
            {
                this->meteoPoint.setMeteoPointValueD(date, dailyAirTemperatureMin, (float)tmin);
                this->meteoPoint.setMeteoPointValueD(date, dailyAirTemperatureMax, (float)tmax);
                this->meteoPoint.setMeteoPointValueD(date, dailyAirTemperatureAvg, (float)tmed);
                this->meteoPoint.setMeteoPointValueD(date, dailyPrecipitation, (float)prec);
                this->meteoPoint.setMeteoPointValueD(date, dailyPotentialEvapotranspiration, (float)et0);
            }
            else
            {
                *myError = "Wrong METEO: index out of range.";
                return false;
            }

            previousDate = myDate;
            expectedDate = myDate.addDays(1);
        }

    } while(query->next());

    return true;
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
            + " EVAP_MAX REAL, TRANSP_MAX, EVAP REAL, TRANSP REAL, LAI REAL, ROOTDEPTH REAL )";
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
            + " EVAP_MAX, TRANSP_MAX, EVAP, TRANSP, LAI, ROOTDEPTH) "
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





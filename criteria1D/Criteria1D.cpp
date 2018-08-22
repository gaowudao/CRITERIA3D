/*!
    \copyright 2018 Fausto Tomei, Gabriele Antolini,
    Alberto Pistocchi, Marco Bittelli, Antonio Volta, Laura Costantini

    This file is part of CRITERIA3D.
    CRITERIA3D has been developed under contract issued by ARPAE Emilia-Romagna

    CRITERIA3D is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    CRITERIA3D is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with CRITERIA3D.  If not, see <http://www.gnu.org/licenses/>.

    contacts:
    fausto.tomei@gmail.com
    ftomei@arpae.it
    gantolini@arpae.it
*/

#include <QSqlQuery>
#include <QSqlError>
#include <QDate>
#include <QVariant>
#include <iostream>
#include <math.h>

#include "commonConstants.h"
#include "Criteria1D.h"
#include "crit3dDate.h"
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

    this->idCropClass = "";
    this->idForecast = "";
    this->idSoilNumber = NODATA;
    this->idICM = NODATA;
}


Criteria1D::Criteria1D()
{
    this->idCase = "";
    this->outputString = "";

    this->layer = NULL;
    this->nrLayers = 0;
    this->layerThickness = 0.02;          /*!<  [m] default thickness = 2 cm  */

    this->maxSimulationDepth = 2.0;       /*!<  [m] default simulation depth = 2 meters  */

    this->isGeometricLayer = false;

    this->depthPloughedSoil = 0.5;        /*!<  [m] depth of ploughed soil  */
    this->initialAW[0] = 0.85;            /*!<  [-] fraction of available Water (ploughed soil)  */
    this->initialAW[1] = 0.8;             /*!<  [-] fraction of available Water (deep soil)  */

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
    this->dailySoilWaterContent = 0.0;
    this->dailySurfaceWaterContent = 0.0;
    this->dailyEt0 = 0.0;
    this->dailyEvaporation = 0.0;
    this->dailyMaxTranspiration = 0.0;
    this->dailyMaxEvaporation = 0.0;
    this->dailyTranspiration = 0.0;
    this->dailyCropAvailableWater = 0.0;
    this->dailyCropWaterDeficit = 0.0;
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
    double currentDepth;

    // initialize layers
    layer[0].depth = 0.0;
    layer[0].thickness = 0.0;

    currentDepth = this->layerThickness / 2.0;
    for (int i = 1; i < this->nrLayers; i++)
    {
        horizonIndex = soil::getHorizonIndex(&(mySoil), currentDepth);
        layer[i].horizon = &(mySoil.horizon[horizonIndex]);

        soilFraction = (1.0 - layer[i].horizon->coarseFragments);
        layer[i].soilFraction = soilFraction;                       // [-]

        // TODO geometric layer
        layer[i].depth = currentDepth;                              // [m]
        layer[i].thickness = this->layerThickness;                  // [m]

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

        currentDepth += layer[i].thickness;              //[m]
    }

    return(true);
}


QString getId5Char(QString id)
{
    if (id.length() < 5)
        id = "0" + id;

    return id;
}


bool Criteria1D::loadMeteo(QString idMeteo, QString idForecast, std::string *myError)
{
    QString queryString = "SELECT * FROM meteo_locations WHERE id_meteo='" + idMeteo + "'";
    QSqlQuery query = dbMeteo.exec(queryString);
    query.last();

    if (! query.isValid())
    {
        QString idMeteo5char = getId5Char(idMeteo);
        queryString = "SELECT * FROM meteo_locations WHERE id_meteo='" + idMeteo5char + "'";
        query = dbMeteo.exec(queryString);
        query.last();

        if (! query.isValid())
        {
            if (query.lastError().number() > 0)
                *myError = "dbMeteo error: " + query.lastError().text().toStdString();
            else
                *myError = "Missing meteo location:" + idMeteo.toStdString();
            return(false);
        }
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
        queryString = "SELECT * FROM meteo_locations WHERE id_meteo='" + idForecast + "'";
        query = this->dbForecast.exec(queryString);
        query.last();

        if (! query.isValid())
        {
            QString idForecast5char = getId5Char(idForecast);
            queryString = "SELECT * FROM meteo_locations WHERE id_meteo='" + idForecast5char + "'";
            query = dbForecast.exec(queryString);
            query.last();

            if (! query.isValid())
            {
                if (query.lastError().number() > 0)
                    *myError = "dbForecast error: " + query.lastError().text().toStdString();
                else
                    *myError = "Missing forecast location:" + idForecast.toStdString();
                return false;
            }
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
        // caso normale
        if (myDate != lastObsDate.addDays(1))
        {
            // previsioni indietro di un giorno: accettato ma tolgo un giorno
            if (myDate == lastObsDate)
            {
                meteoPoint.nrObsDataDaysD--;
            }
            else
            {
                *myError = "The forecast date doesn't match with observed data.";
                return false;
            }
        }

        // Read forecast data
        if (! readMOSESDailyData(&query, &meteoPoint, myError)) return false;

        // Fill watertable data
        // estende il dato precedente se mancante
        float previousData = NODATA;
        for (long i = 0; i < meteoPoint.nrObsDataDaysD; i++)
        {
            if (meteoPoint.obsDataD[i].waterTable != NODATA)
                previousData = meteoPoint.obsDataD[i].waterTable;
            else if (previousData != NODATA)
                meteoPoint.obsDataD[i].waterTable = previousData;
        }
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
            + " ( DATE TEXT, PREC REAL, IRRIGATION REAL, WATER_CONTENT REAL, SURFACE_WC REAL, "
            + " RAW REAL, DEFICIT REAL, DRAINAGE REAL, RUNOFF REAL, ET0 REAL, "
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
            + " (DATE, PREC, IRRIGATION, WATER_CONTENT, SURFACE_WC, RAW, DEFICIT, DRAINAGE, RUNOFF, ET0,"
            + " EVAP_MAX, TRANSP_MAX, EVAP, TRANSP, LAI, KC, ROOTDEPTH) "
            + " VALUES ";
    else
        this->outputString += ",";

    this->outputString
            += "('" + QString::fromStdString(myDate.toStdString()) + "'"
            + "," + QString::number(this->output.dailyPrec)
            + "," + QString::number(this->output.dailyIrrigation)
            + "," + QString::number(this->output.dailySoilWaterContent)
            + "," + QString::number(this->output.dailySurfaceWaterContent)
            + "," + QString::number(this->output.dailyCropAvailableWater)
            + "," + QString::number(this->output.dailyCropWaterDeficit)
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

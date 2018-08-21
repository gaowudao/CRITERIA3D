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

#include <iostream>
#include <QSqlQuery>
#include <QSqlError>
#include <math.h>

#include "commonConstants.h"
#include "Criteria1D.h"
#include "croppingSystem.h"
#include "water1D.h"
#include "modelCore.h"
#include "dbTools.h"


bool runModel(Criteria1D* myCase, Criteria1DUnit *myUnit, std::string* myError)
{
    myCase->idCase = myUnit->idCase;

    if (! myCase->setSoil(myUnit->idSoil, myError))
        return false;

    if (! myCase->loadMeteo(myUnit->idMeteo, myUnit->idForecast, myError))
        return false;

    if (! loadCropParameters(myUnit->idCrop, &(myCase->myCrop), &(myCase->dbParameters), myError))
        return false;

    if (! myCase->isSeasonalForecast)
        if (! myCase->createOutputTable(myError))
            return false;

    // set computation period (all meteo data)
    Crit3DDate firstDate, lastDate;
    long lastIndex = myCase->meteoPoint.nrObsDataDaysD-1;
    firstDate = myCase->meteoPoint.obsDataD[0].date;
    lastDate = myCase->meteoPoint.obsDataD[lastIndex].date;

    if (myCase->isSeasonalForecast)
        myCase->initializeSeasonalForecast(firstDate, lastDate);

    return computeModel(myCase, firstDate, lastDate, myError);
}


bool computeModel(Criteria1D* myCase, const Crit3DDate& firstDate, const Crit3DDate& lastDate, std::string* myError)
{
    Crit3DDate myDate;
    long myIndex;
    int doy;
    float tmin, tmax;                               // [°C]
    float prec, et0, tomorrowPrec;                  // [mm]
    float irrigation, irrigationPrec;               // [mm]
    float waterTableDepth;                          // [m]
    bool isFirstDay = true;
    int indexSeasonalForecast = NODATA;
    bool isInsideSeason;

    if (myCase->meteoPoint.latitude == NODATA)
    {
        *myError = "Latitude is missing";
        return false;
    }

    initializeWater(myCase);

    initializeCrop(myCase, getDoyFromDate(firstDate));

    for (myDate = firstDate; myDate <= lastDate; ++myDate)
    {
        // Initialize
        myCase->output.initializeDaily();
        doy = getDoyFromDate(myDate);

        // daily meteo
        myIndex = myCase->meteoPoint.obsDataD[0].date.daysTo(myDate);
        if ((myIndex < 0) || (myIndex >= myCase->meteoPoint.nrObsDataDaysD))
        {
            *myError = "Missing weather data: " + myDate.toStdString();
            return false;
        }

        prec = myCase->meteoPoint.getMeteoPointValueD(myDate, dailyPrecipitation);
        tmin = myCase->meteoPoint.getMeteoPointValueD(myDate, dailyAirTemperatureMin);
        tmax = myCase->meteoPoint.getMeteoPointValueD(myDate, dailyAirTemperatureMax);

        if ((prec == NODATA) || (tmin == NODATA) || (tmax == NODATA))
        {
            *myError = "Missing weather data: " + myDate.toStdString();
            return false;
        }

        // check on wrong data
        if (prec < 0.0) prec = 0.0;
        myCase->output.dailyPrec = prec;

        // WATERTABLE
        waterTableDepth = myCase->meteoPoint.getMeteoPointValueD(myDate, dailyWaterTableDepth);

        myCase->output.dailyWaterTable = waterTableDepth;
        if (myDate < lastDate)
            tomorrowPrec = myCase->meteoPoint.getMeteoPointValueD(myDate.addDays(1), dailyPrecipitation);
        else
            tomorrowPrec = 0;

        // ET0
        et0 = myCase->meteoPoint.getMeteoPointValueD(myDate, dailyReferenceEvapotranspirationHS);
        if ((et0 == NODATA || et0 <= 0))
            et0 = ET0_Hargreaves(0.17, myCase->meteoPoint.latitude, doy, tmax, tmin);

        myCase->output.dailyEt0 = et0;

        // CROP
        if (! updateCrop(myCase, myError, myDate, tmin, tmax, waterTableDepth))
            return false;

        // ETcrop
        if (! cropWaterDemand(myCase))
            return false;

        // WATERTABLE (if available)
        computeCapillaryRise(myCase, waterTableDepth);

        // IRRIGATION
        irrigation = cropIrrigationDemand(myCase, doy, prec, tomorrowPrec);
        myCase->output.dailyIrrigation = irrigation;

        irrigationPrec = 0.0;
        if (irrigation > 0)
        {
            if (myCase->optimizeIrrigation)
                irrigateCrop(myCase, irrigation);
            else
                irrigationPrec = irrigation;
        }

        // INFILTRATION
        if (! computeInfiltration(myCase, prec, irrigationPrec))
            return false;

        // RUNOFF
        if (! computeSurfaceRunoff(myCase))
            return false;

        // Adjust irrigation losses
        if (! myCase->optimizeIrrigation)
        {
            if ((myCase->output.dailySurfaceRunoff > 5) && (myCase->output.dailyIrrigation > 0))
                {
                    myCase->output.dailyIrrigation -= floor(myCase->output.dailySurfaceRunoff);
                    myCase->output.dailySurfaceRunoff -= floor(myCase->output.dailySurfaceRunoff);
                }
        }

        // LATERAL DRAINAGE
        if (! computeLateralDrainage(myCase))
            return false;

        // EVAPORATION
        if (! evaporation(myCase))
            return false;

        // TRANSPIRATION
        myCase->output.dailyTranspiration = cropTranspiration(myCase, false);

        // RAW and Water Deficit
        myCase->output.dailyCropAvailableWater = getReadilyAvailableWater(myCase);
        myCase->output.dailySoilWaterDeficit = getSoilWaterDeficit(myCase);

        if (! myCase->isSeasonalForecast)
        {
            myCase->prepareOutput(myDate, isFirstDay);
            isFirstDay = false;
        }

        // seasonal forecast: update values of annual irrigation
        if (myCase->isSeasonalForecast)
        {
            isInsideSeason = false;
            // normal seasons
            if (myCase->firstSeasonMonth < 11)
            {
                if (myDate.month >= myCase->firstSeasonMonth && myDate.month <= myCase->firstSeasonMonth+2)
                    isInsideSeason = true;
            }
            // NDJ or DJF
            else
            {
                int lastMonth = (myCase->firstSeasonMonth + 2) % 12;
                if (myDate.month >= myCase->firstSeasonMonth || myDate.month <= lastMonth)
                   isInsideSeason = true;
            }

            if (isInsideSeason)
            {
                // first date of season
                if (myDate.day == 1 && myDate.month == myCase->firstSeasonMonth)
                {
                    if (indexSeasonalForecast == NODATA)
                        indexSeasonalForecast = 0;
                    else
                        indexSeasonalForecast++;
                }

                // sum of irrigations
                if (indexSeasonalForecast != NODATA)
                {
                    if (myCase->seasonalForecasts[indexSeasonalForecast] == NODATA)
                        myCase->seasonalForecasts[indexSeasonalForecast] = myCase->output.dailyIrrigation;
                    else
                        myCase->seasonalForecasts[indexSeasonalForecast] += myCase->output.dailyIrrigation;
                }
            }
        }
    }

    if (myCase->isSeasonalForecast)
        return true;
    else
        return myCase->saveOutput(myError);
}







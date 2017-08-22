#include <iostream>
#include <QSqlQuery>
#include <QSqlError>

#include "commonConstants.h"
#include "Criteria1D.h"
#include "croppingSystem.h"
#include "water1D.h"
#include "modelCore.h"
#include "dbTools.h"


bool runModel(Criteria1D* myCase, std::string* myError, Criteria1DUnit *myUnit)
{
    myCase->idCase = myUnit->idCase;

    if (! myCase->setSoil(myUnit->idSoil, myError))
        return false;

    if (! myCase->loadMOSESMeteo(myUnit->idMeteo, myUnit->idForecast, myError))
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

    myCase->initializeSeasonalForecast(firstDate, lastDate);

    return computeModel(myCase, myError, firstDate, lastDate);
}


bool computeModel(Criteria1D* myCase, std::string* myError, const Crit3DDate& firstDate, const Crit3DDate& lastDate)
{
    Crit3DDate myDate;
    long myIndex;
    int doy;
    float tmin, tmax;                               // [°C]
    float prec, et0, tomorrowPrec, irrigation;      // [mm]
    float waterTableDepth;                          // [m]
    bool isFirstDay = true;
    int indexSeasonalForecast = NODATA;

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
        irrigation = 0.0;

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
        waterTableDepth = myCase->meteoPoint.getMeteoPointValueD(myDate, dailyWaterTableDepth);

        // check for DA-RO
        if (waterTableDepth >= 0.f && waterTableDepth < 0.1f)
            waterTableDepth = 0.1f;
        myCase->output.dailyWaterTable = waterTableDepth;

        if ((prec == NODATA) || (tmin == NODATA) || (tmax == NODATA))
        {
            *myError = "Missing weather data: " + myDate.toStdString();
            return false;
        }

        myCase->output.dailyPrec = prec;
        if (myDate < lastDate)
            tomorrowPrec = myCase->meteoPoint.getMeteoPointValueD(myDate.addDays(1), dailyPrecipitation);
        else
            tomorrowPrec = 0;

        // ET0
        et0 = myCase->meteoPoint.getMeteoPointValueD(myDate, dailyPotentialEvapotranspiration);
        if ((et0 == NODATA || et0 <= 0))
            et0 = ET0_Hargreaves(0.17, myCase->meteoPoint.latitude, doy, tmax, tmin);

        myCase->output.dailyEt0 = et0;

        // CROP
        if (! updateCrop(myCase, myError, myDate, tmin, tmax, waterTableDepth))
            return false;

        // WATERTABLE (not mandatory)
        computeCapillaryRise(myCase, waterTableDepth);

        // IRRIGATION
        if (myCase->myCrop.isLiving)
        {
            irrigation = cropIrrigationDemand(myCase, prec, tomorrowPrec);
            if (irrigation > 0 && myCase->optimizeIrrigation)
            {
                irrigateCrop(myCase, irrigation);
                irrigation = 0.0;
            }
            else
                myCase->output.dailyIrrigation = irrigation;
        }

        // INFILTRATION
        if (! computeInfiltration(myCase, prec, irrigation))
            return false;

        // RUNOFF
        if (! computeSurfaceRunoff(myCase))
            return false;

        // LATERAL DRAINAGE
        if (! computeLateralDrainage(myCase))
            return false;

        // Check irrigation lost
        if (! myCase->optimizeIrrigation)
        {
            if ((myCase->output.dailySurfaceRunoff > 5) && (myCase->output.dailyIrrigation > 0))
                {
                    myCase->output.dailyIrrigation -= floor(myCase->output.dailySurfaceRunoff);
                    myCase->output.dailySurfaceRunoff -= floor(myCase->output.dailySurfaceRunoff);
                }
        }

        if (! cropWaterDemand(myCase))
            return false;

        if (! evaporation(myCase))
            return false;

        if (! cropTranspiration(myCase))
            return false;

        myCase->output.dailyCropAvailableWater = getReadilyAvailableWater(myCase);
        myCase->output.dailySoilWaterDeficit = getSoilWaterDeficit(myCase);

        if (! myCase->isSeasonalForecast)
        {
            myCase->prepareOutput(myDate, isFirstDay);
            isFirstDay = false;
        }

        // seasonal forecast: update values of annual irrigation
        if (myCase->isSeasonalForecast)
            if (myDate.month >= myCase->firstSeasonMonth && myDate.month <= myCase->firstSeasonMonth+2)
            {
                if (indexSeasonalForecast == NODATA)
                    indexSeasonalForecast = 0;
                // first date of season
                else if (myDate.month == myCase->firstSeasonMonth && myDate.day == 1)
                    indexSeasonalForecast++;

                myCase->seasonalForecasts[indexSeasonalForecast] += myCase->output.dailyIrrigation;
            }
    }

    if (myCase->isSeasonalForecast)
        return true;
    else
        return myCase->saveOutput(myError);
}







#include <iostream>
#include <QSqlQuery>
#include <QSqlError>

#include "commonConstants.h"
#include "Criteria1D.h"
#include "croppingSystem.h"
#include "datahandler.h"
#include "water1D.h"
#include "modelcore.h"


bool runModel(Criteria1D* myCase, QString* myError, Criteria1DUnit *myUnit)
{
    myCase->idCase = myUnit->idCase;

    if (! myCase->loadSoil(myUnit->idSoil, myError))
        return false;

    if (! myCase->loadMeteo(myUnit->idMeteo, myUnit->idForecast, myError))
        return false;

    if (! myCase->loadCropParameters(myUnit->idCrop, myError))
        return false;

    if (! myCase->isSeasonalForecast)
        if (! myCase->createOutputTable(myError))
            return false;


    // set computation period (tutti i dati meteo)
    Crit3DDate firstDate, lastDate;
    long lastIndex = myCase->meteoPoint.nrObsDataDaysD-1;
    firstDate = myCase->meteoPoint.obsDataD[0].date;
    lastDate = myCase->meteoPoint.obsDataD[lastIndex].date;

    return computeModel(myCase, myError, firstDate, lastDate);
}


bool computeModel(Criteria1D* myCase, QString* myError, const Crit3DDate& firstDate, const Crit3DDate& lastDate)
{
    Crit3DDate myDate;
    long myIndex;
    int doy;
    double tmin, tmax, prec, et0, tomorrowPrec, irrigation;
    bool isFirstDay = true;

    if (myCase->meteoPoint.latitude == NODATA)
    {
        *myError = "Latitude is missing";
        return false;
    }

    int indexSeasonalForecast;
    if (myCase->isSeasonalForecast)
    {
        if (myCase->seasonalForecasts != NULL) free(myCase->seasonalForecasts);

        myCase->nrSeasonalForecasts = lastDate.year - firstDate.year +1;
        myCase->seasonalForecasts = (double*) calloc(myCase->nrSeasonalForecasts, sizeof(double));
        for (int i=0; i<myCase->nrSeasonalForecasts; i++)
            myCase->seasonalForecasts[i] = 0.0;
        indexSeasonalForecast = NODATA;
    }

    initializeWater(myCase);

    if (! initializeCrop(myCase)) return false;

    for (myDate = firstDate; myDate <= lastDate; ++myDate)
    {
        // INITIALIZE
        myCase->output.initializeDaily();
        doy = getDoyFromDate(myDate);
        irrigation = 0.0;

        // READ METEO
        myIndex = myCase->meteoPoint.obsDataD[0].date.daysTo(myDate);
        if ((myIndex < 0) || (myIndex >= myCase->meteoPoint.nrObsDataDaysD))
        {
            *myError = "Missing weather data: " + QString::fromStdString(myDate.toStdString());
            return false;
        }

        tmin = myCase->meteoPoint.getMeteoPointValueD(myDate, dailyAirTemperatureMin);
        tmax = myCase->meteoPoint.getMeteoPointValueD(myDate, dailyAirTemperatureMax);
        prec = myCase->meteoPoint.getMeteoPointValueD(myDate, precipitation);
        et0 = myCase->meteoPoint.getMeteoPointValueD(myDate, dailyPotentialEvapotranspiration);
        if (myDate < lastDate)
            tomorrowPrec = myCase->meteoPoint.getMeteoPointValueD(myDate.addDays(1), precipitation);
        else
            tomorrowPrec = 0;

        myCase->output.dailyPrec = prec;

        // CROP
        if (! updateCrop(myCase, myError, myDate, isFirstDay, tmin, tmax))
            return false;

        // ***** SUB_SOIL_Risalita_da_falda (TODO)

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
        if (! empiricalInfiltration(myCase, myError, prec, irrigation))
            return false;

        if (! computeRunoff( myCase))
            return false;

        // Check irrigation lost
        if (! myCase->optimizeIrrigation)
            if ((myCase->output.dailySurfaceRunoff > 5) && (myCase->output.dailyIrrigation > 0))
                {
                    myCase->output.dailyIrrigation -= floor(myCase->output.dailySurfaceRunoff);
                    myCase->output.dailySurfaceRunoff -= floor(myCase->output.dailySurfaceRunoff);
                }

        if (! cropWaterDemand(myCase, doy, tmax, tmin, et0))
            return false;

        if (! evaporation(myCase))
            return false;

        if (! cropTranspiration(myCase))
            return false;

        myCase->output.dailyCropAvailableWater = getReadilyAvailableWater(myCase);

        if (! myCase->isSeasonalForecast)
        {
            myCase->prepareOutput(myDate, isFirstDay);
            isFirstDay = false;
        }

        if (myCase->isSeasonalForecast)
            if (myDate.month >= myCase->firstSeasonMonth && myDate.month <= myCase->firstSeasonMonth+2)
            {
                if (indexSeasonalForecast == NODATA)
                    indexSeasonalForecast = 0;
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







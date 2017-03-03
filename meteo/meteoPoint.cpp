/*!
    \copyright 2016 Fausto Tomei, Gabriele Antolini,
    Alberto Pistocchi, Marco Bittelli, Antonio Volta, Laura Costantini

    This file is part of CRITERIA3D.
    CRITERIA3D has been developed under contract issued by A.R.P.A. Emilia-Romagna

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
*/


#include <malloc.h>

#include "commonConstants.h"
#include "meteoPoint.h"
#include "meteo.h"


Crit3DMeteoPoint::Crit3DMeteoPoint()
{
        name = "";
        id = "";
        isUTC = true;
        nrObsDataDaysH = 0;
        nrObsDataDaysD = 0;
        hourlyFraction = 1;
        myQuality = quality::missing;
        residual = NODATA;
        latitude = NODATA;
        longitude = NODATA;
        obsDataH = NULL;
        obsDataD = NULL;
}

Crit3DMeteoPoint::Crit3DMeteoPoint(std::string myName, std::string myId, bool _isUTC)
{
        name = myName;
        id = myId;
        isUTC = _isUTC;
        nrObsDataDaysD = 0;
        nrObsDataDaysH = 0;
        hourlyFraction = 1;
        myQuality = quality::missing;
        residual = NODATA;
        latitude = NODATA;
        longitude = NODATA;
        obsDataH = NULL;
        obsDataD = NULL;
}

void Crit3DMeteoPoint::initializeObsDataH(int myHourlyFraction, int numberOfDays, const Crit3DDate& firstDate)
{
    this->cleanObsDataH();

    nrObsDataDaysH = numberOfDays;
    hourlyFraction = myHourlyFraction;
    int nrDayValues = hourlyFraction * 24 +1;
    obsDataH = (TObsDataH *) calloc(numberOfDays, sizeof(TObsDataH));
    myQuality = quality::missing;
    residual = NODATA;

    for (int i = 0; i < numberOfDays; i++)
    {
        obsDataH[i].date = firstDate.addDays(i);
        obsDataH[i].tAir = (float *) calloc(nrDayValues, sizeof(float));
        obsDataH[i].prec = (float *) calloc(nrDayValues, sizeof(float));
        obsDataH[i].rhAir = (float *) calloc(nrDayValues, sizeof(float));
        obsDataH[i].tDew = (float *) calloc(nrDayValues, sizeof(float));
        obsDataH[i].irradiance = (float *) calloc(nrDayValues, sizeof(float));
        obsDataH[i].windInt = (float *) calloc(nrDayValues, sizeof(float));
        obsDataH[i].pressure = (float *) calloc(nrDayValues, sizeof(float));
        obsDataH[i].wetDuration = (int *) calloc(nrDayValues, sizeof(int));
        obsDataH[i].transmissivity = (float *) calloc(nrDayValues, sizeof(float));
        for (int j = 0; j < nrDayValues; j++)
        {
            obsDataH[i].tAir[j] = NODATA;
            obsDataH[i].prec[j] = NODATA;
            obsDataH[i].rhAir[j] = NODATA;
            obsDataH[i].tDew[j] = NODATA;
            obsDataH[i].irradiance[j] = NODATA;
            obsDataH[i].windInt[j] = NODATA;
            obsDataH[i].pressure[j] = NODATA;
            obsDataH[i].wetDuration[j] = NODATA;
            obsDataH[i].transmissivity[j] = NODATA;
        }
    }
}

void Crit3DMeteoPoint::initializeObsDataD(int numberOfDays, const Crit3DDate& firstDate)
{
    this->cleanObsDataD();

    nrObsDataDaysD = numberOfDays;
    obsDataD = (TObsDataD *) calloc(numberOfDays, sizeof(TObsDataD));
    myQuality = quality::missing;
    residual = NODATA;

    for (int i = 0; i < numberOfDays; i++)
    {
        obsDataD[i].date = firstDate.addDays(i);
        obsDataD[i].tMax = NODATA;
        obsDataD[i].tMin = NODATA;
        obsDataD[i].tAvg = NODATA;
        obsDataD[i].prec = NODATA;
        obsDataD[i].rhMax = NODATA;
        obsDataD[i].rhMin = NODATA;
        obsDataD[i].rhAvg = NODATA;
        obsDataD[i].globRad = NODATA;
        obsDataD[i].windIntAvg = NODATA;
        obsDataD[i].windDirPrev = NODATA;
        obsDataD[i].waterTableDepth = NODATA;
    }
}

void Crit3DMeteoPoint::emptyVarObsDataH(meteoVariable myVar, const Crit3DDate& myDate)
{
    if (! isDateLoadedH(myDate)) return;

    int nrDayValues = hourlyFraction * 24 + 1;
    int i = obsDataH[0].date.daysTo(myDate);
    residual = NODATA;

    if (i>=0 && i<nrObsDataDaysH)
        if (obsDataH[i].date == myDate)
            for (int j = 0; j < nrDayValues; j++)
            {
                if (myVar == airTemperature)
                    obsDataH[i].tAir[j] = NODATA;
                else if (myVar == precipitation)
                    obsDataH[i].prec[j] = NODATA;
                else if (myVar == airHumidity)
                    obsDataH[i].rhAir[j] = NODATA;
                else if (myVar == airDewTemperature)
                    obsDataH[i].tDew[j] = NODATA;
                else if (myVar == globalIrradiance)
                    obsDataH[i].irradiance[j] = NODATA;
                else if (myVar == windIntensity)
                    obsDataH[i].windInt[j] = NODATA;
                else if (myVar == atmPressure)
                    obsDataH[i].pressure[j] = NODATA;
                else if (myVar == wetnessDuration)
                    obsDataH[i].wetDuration[j] = NODATA;
                else if (myVar == atmTransmissivity)
                    obsDataH[i].transmissivity[j] = NODATA;
            }
}

void Crit3DMeteoPoint::emptyVarObsDataH(meteoVariable myVar, const Crit3DDate& myDateIni, const Crit3DDate& myDateFin)
{
    if (! isDateIntervalLoadedH(myDateIni, myDateFin)) return;

    int nrDayValues = hourlyFraction * 24 + 1;
    int indexIni = obsDataH[0].date.daysTo(myDateIni);
    int indexFin = obsDataH[0].date.daysTo(myDateFin);
    residual = NODATA;

    for (int i = indexIni; i <= indexFin; i++)
        for (int j = 0; j < nrDayValues; j++)
        {
            if (myVar == airTemperature)
                obsDataH[i].tAir[j] = NODATA;
            else if (myVar == precipitation)
                obsDataH[i].prec[j] = NODATA;
            else if (myVar == airHumidity)
                obsDataH[i].rhAir[j] = NODATA;
            else if (myVar == airDewTemperature)
                obsDataH[i].tDew[j] = NODATA;
            else if (myVar == globalIrradiance)
                obsDataH[i].irradiance[j] = NODATA;
            else if (myVar == windIntensity)
                obsDataH[i].windInt[j] = NODATA;
            else if (myVar == atmPressure)
                obsDataH[i].pressure[j] = NODATA;
            else if (myVar == wetnessDuration)
                obsDataH[i].wetDuration[j] = NODATA;
            else if (myVar == atmTransmissivity)
                obsDataH[i].transmissivity[j] = NODATA;
        }
}

void Crit3DMeteoPoint::emptyVarObsDataD(meteoVariable myVar, const Crit3DDate& myDateIni, const Crit3DDate& myDateFin)
{
    if (! isDateIntervalLoadedH(myDateIni, myDateFin)) return;

    int indexIni = obsDataD[0].date.daysTo(myDateIni);
    int indexFin = obsDataD[0].date.daysTo(myDateFin);
    residual = NODATA;

    for (int i = indexIni; i <= indexFin; i++)
        if (myVar == dailyAirTemperatureMax)
            obsDataD[i].tMax = NODATA;
        else if (myVar == dailyAirTemperatureMin)
            obsDataD[i].tMin = NODATA;
        else if (myVar == dailyAirTemperatureMin)
            obsDataD[i].tAvg = NODATA;
        else if (myVar == precipitation)
            obsDataD[i].prec = NODATA;
        else if (myVar == dailyAirHumidityMax)
            obsDataD[i].rhMax = NODATA;
        else if (myVar == dailyAirHumidityMin)
            obsDataD[i].rhMin = NODATA;
        else if (myVar == dailyAirHumidityAvg)
            obsDataD[i].rhAvg = NODATA;
        else if (myVar == dailyGlobalRadiation)
            obsDataD[i].globRad = NODATA;
        else if (myVar == dailyWindIntensityAvg)
            obsDataD[i].windIntAvg = NODATA;
        else if (myVar == dailyAirHumidityAvg)
            obsDataD[i].rhAvg = NODATA;
}

bool Crit3DMeteoPoint::isDateLoadedH(const Crit3DDate& myDate)
{
    if (nrObsDataDaysH == 0)
        return (false);
    else if (myDate < obsDataH->date || myDate > (obsDataH->date.addDays(nrObsDataDaysH - 1)))
        return (false);
    else
        return (true);
}

bool Crit3DMeteoPoint::isDateIntervalLoadedH(const Crit3DDate& myDateIni, const Crit3DDate& myDateFin)
{
    if (nrObsDataDaysH == 0)
        return false;
    else if (myDateIni > myDateFin)
        return false;
    else if (myDateIni < obsDataH->date || myDateFin > (obsDataH->date.addDays(nrObsDataDaysH - 1)))
        return (false);
    else
        return (true);
}

bool Crit3DMeteoPoint::isDateLoadedD(const Crit3DDate& myDate)
{
    if (nrObsDataDaysD == 0)
        return (false);
    else if (myDate < obsDataD->date || myDate > (obsDataD->date.addDays(nrObsDataDaysD - 1)))
        return (false);
    else
        return (true);
}

bool Crit3DMeteoPoint::isDateIntervalLoadedD(const Crit3DDate& myDateIni, const Crit3DDate& myDateFin)
{
    if (nrObsDataDaysD == 0)
        return false;
    else if (myDateIni > myDateFin)
        return false;
    else if (myDateIni < obsDataD->date || myDateFin > (obsDataD->date.addDays(nrObsDataDaysD - 1)))
        return (false);
    else
        return (true);
}

bool Crit3DMeteoPoint::isDateIntervalLoadedH(const Crit3DTime& timeIni, const Crit3DTime& timeFin)
{
    if (nrObsDataDaysH == 0)
        return false;
    else if (timeIni > timeFin)
        return false;
    else if (obsDataH == NULL)
        return false;
    else if (timeIni.date < obsDataH[0].date || timeFin.date > (obsDataH[0].date.addDays(nrObsDataDaysH - 1)))
        return (false);
    else
        return (true);
}

float Crit3DMeteoPoint::obsDataConsistencyH(meteoVariable myVar, const Crit3DTime& timeIni, const Crit3DTime& timeFin)
{
    if (nrObsDataDaysH == 0)
        return 0.0;
    else if (timeIni > timeFin)
        return 0.0;
    else if (obsDataH == NULL)
        return 0.0;
    else if (timeIni.date < obsDataH[0].date || timeFin.date > (obsDataH[0].date.addDays(nrObsDataDaysH - 1)))
        return 0.0;
    else
    {
        Crit3DTime myTime = timeIni;
        float myValue;
        float myDeltaSeconds = (float)3600.0 / hourlyFraction;
        int counter=0, counterAll=0;
        while (myTime <= timeFin)
        {
            myValue = getMeteoPointValueH(myTime.date, myTime.getHour(), myTime.getMinutes(), myVar);
            if (myValue != NODATA) counter++;
            counterAll++;
            myTime = myTime.addSeconds(myDeltaSeconds);
        }
        return (float(counter)/float(counterAll));
    }

}

void Crit3DMeteoPoint::cleanObsDataH()
{
    myQuality = quality::missing;

    if (nrObsDataDaysH > 0)
    {
        for (int i = 0; i < nrObsDataDaysH; i++)
        {
            free(obsDataH[i].tAir);
            free(obsDataH[i].prec);
            free(obsDataH[i].rhAir);
            free(obsDataH[i].tDew);
            free(obsDataH[i].irradiance);
            free(obsDataH[i].windInt);
            free(obsDataH[i].pressure);
            free(obsDataH[i].wetDuration);
        }
        free (obsDataH);
    }
}

void Crit3DMeteoPoint::cleanObsDataD()
{
    myQuality = quality::missing;

    if (nrObsDataDaysD > 0)
        free (obsDataD);
}

bool Crit3DMeteoPoint::setMeteoPointValueH(const Crit3DDate& myDate, int myHour, int myMinutes, meteoVariable myVar, float myValue)
{
    int myHourlyIndex = (hourlyFraction * myHour) + myMinutes % (60 / hourlyFraction);
    long i = obsDataH[0].date.daysTo(myDate);

    if (i < 0) return false;

    if (myVar == airTemperature)
        obsDataH[i].tAir[myHourlyIndex] = myValue;
    else if (myVar == precipitation)
        obsDataH[i].prec[myHourlyIndex] = myValue;
    else if (myVar == airHumidity)
        obsDataH[i].rhAir[myHourlyIndex] = myValue;
    else if (myVar == airDewTemperature)
        obsDataH[i].tDew[myHourlyIndex] = myValue;
    else if (myVar == globalIrradiance)
        obsDataH[i].irradiance[myHourlyIndex] = myValue;
    else if (myVar == windIntensity)
        obsDataH[i].windInt[myHourlyIndex] = myValue;
    else if (myVar == wetnessDuration)
        obsDataH[i].wetDuration[myHourlyIndex] = (int)myValue;
    else if (myVar == atmPressure)
        obsDataH[i].pressure[myHourlyIndex] = myValue;
    else if (myVar == atmTransmissivity)
        obsDataH[i].transmissivity[myHourlyIndex] = myValue;
    else
        return false;

    return true;
}


bool Crit3DMeteoPoint::setMeteoPointValueD(const Crit3DDate& myDate, meteoVariable myVar, float myValue)
{
    long i = obsDataD[0].date.daysTo(myDate);
    if (i < 0) return false;

    if (myVar == dailyAirTemperatureMax)
        obsDataD[i].tMax = myValue;
    else if (myVar == dailyAirTemperatureMin)
        obsDataD[i].tMin = myValue;
    else if (myVar == dailyAirTemperatureAvg)
        obsDataD[i].tAvg = myValue;
    else if (myVar == dailyPrecipitation)
        obsDataD[i].prec = myValue;
    else if (myVar == dailyAirHumidityMax)
        obsDataD[i].rhMax = myValue;
    else if (myVar == dailyAirHumidityMin)
        obsDataD[i].rhMin = myValue;
    else if (myVar == dailyAirHumidityAvg)
        obsDataD[i].rhAvg = myValue;
    else if (myVar == dailyGlobalRadiation)
        obsDataD[i].globRad = myValue;
    else if (myVar == dailyWindIntensityAvg)
        obsDataD[i].windIntAvg = myValue;
    else if (myVar == windDirectionPrevailing)
        obsDataD[i].windDirPrev = myValue;
    else if (myVar == waterTableDepth)
        obsDataD[i].waterTableDepth = myValue;
    else
        return false;

    return true;
}

float Crit3DMeteoPoint::getMeteoPointValueH(const Crit3DDate& myDate, int myHour, int myMinutes, meteoVariable myVar)
{
    if (myVar == noMeteoVar) return NODATA;

    int h = (hourlyFraction * myHour) + myMinutes % (60 / hourlyFraction);
    int dateIndex = obsDataH[0].date.daysTo(myDate);

    if ((dateIndex < 0) || (dateIndex >= nrObsDataDaysH)) return NODATA;

    if (myVar == airTemperature)
        return (obsDataH[dateIndex].tAir[h]);
    else if (myVar == precipitation)
        return (obsDataH[dateIndex].prec[h]);
    else if (myVar == airHumidity)
        return (obsDataH[dateIndex].rhAir[h]);
    else if (myVar == airDewTemperature)
    {
        if (obsDataH[dateIndex].tDew[h] != NODATA)
            return obsDataH[dateIndex].tDew[h];
        else
            return tDewFromRelHum(obsDataH[dateIndex].rhAir[h], obsDataH[dateIndex].tAir[h]);
    }
    else if (myVar == globalIrradiance)
        return (obsDataH[dateIndex].irradiance[h]);
    else if (myVar == windIntensity)
        return (obsDataH[dateIndex].windInt[h]);
    else if (myVar == wetnessDuration)
        return (float)(obsDataH[dateIndex].wetDuration[h]);
    else if (myVar == atmPressure)
        return (obsDataH[dateIndex].pressure[h]);
    else if (myVar == atmTransmissivity)
        return (obsDataH[dateIndex].transmissivity[h]);
    else
        return (NODATA);

}

float Crit3DMeteoPoint::getMeteoPointValueD(const Crit3DDate& myDate, meteoVariable myVar)
{
    if (myVar == noMeteoVar) return NODATA;

    int dateIndex = obsDataD[0].date.daysTo(myDate);
    if ((dateIndex < 0) || (dateIndex >= nrObsDataDaysD)) return NODATA;

    if (myVar == dailyAirTemperatureMax)
        return (obsDataD[dateIndex].tMax);
    else if (myVar == dailyAirTemperatureMin)
        return (obsDataD[dateIndex].tMin);
    else if (myVar == dailyAirTemperatureAvg)
        return (obsDataD[dateIndex].tAvg);
    else if (myVar == precipitation)
        return (obsDataD[dateIndex].prec);
    else if (myVar == dailyAirHumidityMax)
        return (obsDataD[dateIndex].rhMax);
    else if (myVar == dailyAirHumidityMin)
        return (float)(obsDataD[dateIndex].rhMin);
    else if (myVar == dailyAirHumidityAvg)
        return (obsDataD[dateIndex].rhAvg);
    else if (myVar == dailyGlobalRadiation)
        return (obsDataD[dateIndex].globRad);
    else if (myVar == dailyWindIntensityAvg)
        return (obsDataD[dateIndex].windIntAvg);
    else if (myVar == windDirectionPrevailing)
        return (obsDataD[dateIndex].windDirPrev);
    else if (myVar == waterTableDepth)
        return (obsDataD[dateIndex].waterTableDepth);
    else
        return (NODATA);

}

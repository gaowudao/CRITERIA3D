#include "commonConstants.h"
#include "quality.h"
#include "meteoPoint.h"

namespace quality
{
    Range::Range()
    {
        min = NODATA;
        max = NODATA;
    }

    Range::Range(float myMin, float myMax)
    {
        min = myMin;
        max = myMax;
    }

    float Range::getMin() { return min; }

    float Range::getMax() { return max; }
}


float Crit3DQuality::getReferenceHeight() const
{
    return referenceHeight;
}

void Crit3DQuality::setReferenceHeight(float value)
{
    referenceHeight = value;
}

float Crit3DQuality::getDeltaTSuspect() const
{
    return deltaTSuspect;
}

void Crit3DQuality::setDeltaTSuspect(float value)
{
    deltaTSuspect = value;
}

float Crit3DQuality::getDeltaTWrong() const
{
    return deltaTWrong;
}

void Crit3DQuality::setDeltaTWrong(float value)
{
    deltaTWrong = value;
}

float Crit3DQuality::getRelHumTolerance() const
{
    return relHumTolerance;
}

void Crit3DQuality::setRelHumTolerance(float value)
{
    relHumTolerance = value;
}

Crit3DQuality::Crit3DQuality()
{
    qualityHourlyT = new quality::Range(-60, 60);
    qualityHourlyTd = new quality::Range(-60, 50);
    qualityHourlyP = new quality::Range(0, 300);
    qualityHourlyRH = new quality::Range(0, 104);
    qualityHourlyWInt = new quality::Range(0, 150);
    qualityHourlyWDir = new quality::Range(0, 360);
    qualityHourlyGIrr = new quality::Range(-20, 1353);
    qualityTransmissivity = new quality::Range(0, 1);

    qualityDailyT = new quality::Range(-60, 60);
    qualityDailyP = new quality::Range(0, 800);
    qualityDailyRH = new quality::Range(0, 102);
    qualityDailyWInt = new quality::Range(0, 150);
    qualityDailyWDir = new quality::Range(0, 360);
    qualityDailyGRad = new quality::Range(-20, 120);

    referenceHeight = NODATA;
    deltaTSuspect = NODATA;
    deltaTWrong = NODATA;
    relHumTolerance = NODATA;

}


quality::Range* Crit3DQuality::getQualityRange(meteoVariable myVar)
{
    // hourly
    if (myVar == airTemperature)
        return qualityHourlyT;
    else if (myVar == precipitation)
        return qualityHourlyP;
    else if (myVar == globalIrradiance)
        return qualityHourlyGIrr;
    else if (myVar == atmTransmissivity)
        return qualityTransmissivity;
    else if (myVar == airRelHumidity)
        return qualityHourlyRH;
    else if (myVar == windIntensity)
        return qualityHourlyWInt;
    else if (myVar == windDirection)
        return qualityHourlyWDir;
    else if (myVar == airDewTemperature)
        return qualityHourlyTd;

    // daily
    else if (myVar == dailyAirTemperatureMax
          || myVar == dailyAirTemperatureMin
          || myVar == dailyAirTemperatureAvg)
        return qualityDailyT;

    else if (myVar == dailyPrecipitation)
        return qualityDailyP;

    else if (myVar == dailyAirRelHumidityMax
          || myVar == dailyAirRelHumidityMin
          || myVar == dailyAirRelHumidityAvg)
        return qualityDailyRH;

    else if (myVar == dailyGlobalRadiation)
        return qualityDailyGRad;

    else if (myVar == dailyWindIntensityAvg || myVar == dailyWindIntensityMax)
        return qualityDailyWInt;

    else
        return NULL;
}


void Crit3DQuality::syntacticQualityControl(meteoVariable myVar, Crit3DMeteoPoint* meteoPoints, int nrMeteoPoints)
{
    float qualityMin, qualityMax;

    quality::Range* myRange = this->getQualityRange(myVar);
    if (myRange != NULL)
    {
        qualityMin = myRange->getMin();
        qualityMax = myRange->getMax();
    }

    for (int i = 0; i < nrMeteoPoints; i++)
    {
        if (meteoPoints[i].currentValue == NODATA)
            meteoPoints[i].quality = quality::missing_data;
        else
        {
            if (myRange == NULL)
                meteoPoints[i].quality = quality::accepted;

            else if (meteoPoints[i].currentValue < qualityMin || meteoPoints[i].currentValue > qualityMax)
                meteoPoints[i].quality = quality::wrong_syntactic;

            else
                meteoPoints[i].quality = quality::accepted;
        }
    }
}


quality::qualityType Crit3DQuality::syntacticQualitySingleValue(meteoVariable myVar, float myValue)
{
    float qualityMin, qualityMax;

    quality::Range* myRange = this->getQualityRange(myVar);
    if (myRange != NULL)
    {
        qualityMin = myRange->getMin();
        qualityMax = myRange->getMax();
    }

    if (myValue == NODATA)
        return quality::missing_data;
    else
    {
        if (myRange == NULL)
            return quality::accepted;

        else if (myValue < qualityMin || myValue > qualityMax)
            return quality::wrong_syntactic;

        else
            return quality::accepted;
    }
}

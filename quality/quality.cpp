

#include <stdio.h>
#include <cmath>

#include "commonConstants.h"
#include "quality.h"
#include "interpolation.h"


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

    spatialControlActive = true;
}


void Crit3DQuality::setSpatialControl(bool isActive)
{
    spatialControlActive = isActive;
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


// check quality and pass good data to interpolation
bool Crit3DQuality::checkData(meteoVariable myVar, frequencyType myFrequency, Crit3DMeteoPoint* meteoPoints, int nrMeteoPoints, Crit3DTime myTime)
{
    if (nrMeteoPoints == 0)
        return false;

    // assign data
    for (int i = 0; i < nrMeteoPoints; i++)
        meteoPoints[i].currentValue = meteoPoints[i].getMeteoPointValue(myTime, myVar, myFrequency);

    // quality control - synthctic
    syntacticQualityControl(myVar, meteoPoints, nrMeteoPoints);

    // quality control - spatial
    if (this->spatialControlActive
        && myVar != precipitation && myVar != dailyPrecipitation
        && myVar != globalIrradiance && myVar != dailyGlobalRadiation)
    {
        spatialQualityControl(myVar, meteoPoints, nrMeteoPoints);
    }

    // return true if at least one valid data
    return passDataToInterpolation(meteoPoints, nrMeteoPoints);
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
            meteoPoints[i].myQuality = quality::missing_data;
        else
        {
            if (myRange == NULL)
                meteoPoints[i].myQuality = quality::accepted;

            else if (meteoPoints[i].currentValue < qualityMin || meteoPoints[i].currentValue > qualityMax)
                meteoPoints[i].myQuality = quality::wrong_syntactic;

            else
                meteoPoints[i].myQuality = quality::accepted;
        }
    }
}

quality::type Crit3DQuality::syntacticQualityControlSingleVal(meteoVariable myVar, float myValue)
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


float findThreshold(meteoVariable myVar, float value, float stdDev, float nrStdDev, float stdDevZ, float minDistance);
bool computeResiduals(meteoVariable myVar, Crit3DMeteoPoint* meteoPoints, int nrMeteoPoints);
void setSpatialQualityControlSettings(Crit3DInterpolationSettings* mySettings, meteoVariable myVar);


void spatialQualityControl(meteoVariable myVar, Crit3DMeteoPoint* meteoPoints, int nrMeteoPoints)
{
    int i;
    float stdDev, stdDevZ, minDist, myValue, myResidual;
    std::vector <int> listIndex;
    std::vector <float> listResiduals;

    Crit3DInterpolationSettings mySettings;

    setSpatialQualityControlSettings(&mySettings, myVar);
    setInterpolationSettings(&mySettings);

    if (passDataToInterpolation(meteoPoints, nrMeteoPoints))
    {
        // detrend
        if (! preInterpolation(myVar))
            return;

        // compute residuals
        if (! computeResiduals(myVar, meteoPoints, nrMeteoPoints))
            return;

        // re-load data
        passDataToInterpolation(meteoPoints, nrMeteoPoints);

        for (i = 0; i < nrMeteoPoints; i++)
            if (meteoPoints[i].myQuality == quality::accepted)
            {
                if (neighbourhoodVariability(float(meteoPoints[i].point.utm.x),
                         float(meteoPoints[i].point.utm.y),float(meteoPoints[i].point.z),
                         10, &stdDev, &stdDevZ, &minDist))
                {
                    myValue = meteoPoints[i].currentValue;
                    myResidual = meteoPoints[i].residual;
                    stdDev = maxValue(stdDev, myValue/100.f);
                    if (fabs(myResidual) > findThreshold(myVar, myValue, stdDev, 2, stdDevZ, minDist))
                    {
                        listIndex.push_back(i);
                        meteoPoints[i].myQuality = quality::wrong_spatial;
                    }
                }
            }

        if (listIndex.size() > 0)
        {
            if (passDataToInterpolation(meteoPoints, nrMeteoPoints))
            {
                preInterpolation(myVar);

                float interpolatedValue;
                for (i=0; i < int(listIndex.size()); i++)
                {
                    interpolatedValue = interpolate(myVar,
                                            float(meteoPoints[listIndex[i]].point.utm.x),
                                            float(meteoPoints[listIndex[i]].point.utm.y),
                                            float(meteoPoints[listIndex[i]].point.z),
                                            NODATA, NODATA, NODATA, NODATA);

                    myValue = meteoPoints[listIndex[i]].currentValue;

                    listResiduals.push_back(interpolatedValue - myValue);
                }

                passDataToInterpolation(meteoPoints, nrMeteoPoints);

                for (i=0; i < int(listIndex.size()); i++)
                {
                    if (neighbourhoodVariability(float(meteoPoints[listIndex[i]].point.utm.x),
                             float(meteoPoints[listIndex[i]].point.utm.y),
                             float(meteoPoints[listIndex[i]].point.z),
                             10, &stdDev, &stdDevZ, &minDist))
                    {
                        myResidual = listResiduals[i];

                        myValue = meteoPoints[listIndex[i]].currentValue;

                        if (fabs(myResidual) > findThreshold(myVar, myValue, stdDev, 3, stdDevZ, minDist))
                            meteoPoints[listIndex[i]].myQuality = quality::wrong_spatial;
                        else
                            meteoPoints[listIndex[i]].myQuality = quality::accepted;
                    }
                    else
                        meteoPoints[listIndex[i]].myQuality = quality::accepted;
                }
            }
        }
    }
}


void setSpatialQualityControlSettings(Crit3DInterpolationSettings* mySettings, meteoVariable myVar)
{
    mySettings->setUseOrogIndex(false);
    mySettings->setUseAspect(false);
    mySettings->setUseTAD(false);
    mySettings->setUseDewPoint(false);
    mySettings->setUseGenericProxy(false);
    mySettings->setUseSeaDistance(false);

    mySettings->setInterpolationMethod(interpolationMethod::idw);

    if (   myVar == airTemperature
        || myVar == dailyAirTemperatureMax
        || myVar == dailyAirTemperatureMin
        || myVar == dailyAirTemperatureAvg )
    {
        mySettings->setUseHeight(true);
        mySettings->setUseThermalInversion(true);
    }
}


bool computeResiduals(meteoVariable myVar, Crit3DMeteoPoint* meteoPoints, int nrMeteoPoints)
{

    if (myVar == noMeteoVar) return false;

    float myValue, interpolatedValue;
    interpolatedValue = NODATA;
    myValue = NODATA;

    for (int i = 0; i < nrMeteoPoints; i++)
    {
        meteoPoints[i].residual = NODATA;

        if (meteoPoints[i].myQuality == quality::accepted)
        {
            myValue = meteoPoints[i].currentValue;

            interpolatedValue = interpolate(myVar, float(meteoPoints[i].point.utm.x),
                                            float(meteoPoints[i].point.utm.y),
                                            float(meteoPoints[i].point.z),
                                            NODATA, NODATA, NODATA, NODATA);

            if (  myVar == precipitation
               || myVar == dailyPrecipitation)
            {
                if (myValue != NODATA)
                    if (myValue < PREC_THRESHOLD) myValue=0.;

                if (interpolatedValue != NODATA)
                    if (interpolatedValue < PREC_THRESHOLD) interpolatedValue=0.;
            }

            // TODO derived var

            if ((interpolatedValue != NODATA) && (myValue != NODATA))
                meteoPoints[i].residual = interpolatedValue - myValue;
        }
    }

    return true;
}


float findThreshold(meteoVariable myVar, float value, float stdDev, float nrStdDev, float stdDevZ, float minDistance)
{
    float zWeight, distWeight, threshold;

    if (   myVar == precipitation
        || myVar == dailyPrecipitation)
    {
        distWeight = maxValue(1.f, minDistance / 2000.f);
        if (value < PREC_THRESHOLD)
            threshold = maxValue(5.f, distWeight + stdDev * (nrStdDev + 1));
        else
            return 800.f;
    }
    else if (   myVar == airTemperature
             || myVar == airDewTemperature
             || myVar == dailyAirTemperatureMax
             || myVar == dailyAirTemperatureMin
             || myVar == dailyAirTemperatureAvg )
    {
        threshold = 1.f;
        zWeight = stdDevZ / 100.f;
        distWeight = minDistance / 5000.f;

        threshold = minValue(minValue(distWeight + threshold + zWeight, 12.f) + stdDev * nrStdDev, 15.f);
    }
    else if (   myVar == airRelHumidity
             || myVar == dailyAirRelHumidityMax
             || myVar == dailyAirRelHumidityMin
             || myVar == dailyAirRelHumidityAvg )
    {
        threshold = 8.f;
        zWeight = stdDevZ / 100.f;
        distWeight = minDistance / 1000.f;
        threshold += zWeight + distWeight + stdDev * nrStdDev;
    }
    else if (   myVar == windIntensity
             || myVar == dailyWindIntensityAvg
			 || myVar == dailyWindIntensityMax)
    {
        threshold = 1.f;
        zWeight = stdDevZ / 50.f;
        distWeight = minDistance / 2000.f;
        threshold += zWeight + distWeight + stdDev * nrStdDev;
    }
    else if (   myVar == globalIrradiance
             || myVar == dailyGlobalRadiation )
        threshold = maxValue(stdDev * (nrStdDev + 1.f), 5.f);

    else if (myVar == atmTransmissivity)
        threshold = maxValue(stdDev * nrStdDev, 0.5f);
    else
        threshold = stdDev * nrStdDev;

    return threshold;
}


bool passDataToInterpolation(Crit3DMeteoPoint* meteoPoints, int nrMeteoPoints)
{
    int myCounter = 0;
    float myValue, myX, myY, myZ;

    clearInterpolationPoints();

    for (int i = 0; i < nrMeteoPoints; i++)
    {
        if (meteoPoints[i].myQuality == quality::accepted)
        {
            myValue = meteoPoints[i].currentValue;
            myX = float(meteoPoints[i].point.utm.x);
            myY = float(meteoPoints[i].point.utm.y);
            myZ = float(meteoPoints[i].point.z);

            if (addInterpolationPoint(i, myValue, myX, myY, myZ, NODATA, NODATA, NODATA, NODATA, NODATA))
                myCounter++;
        }
    }

    return (myCounter > 0);
}


#include <stdlib.h>
#include <math.h>
#include <vector>
#include <string>
#include <QString>
#include <QFile>

#include "interpolation.h"
#include "solarRadiation.h"
#include "dataHandler.h"
#include "atmosphere.h"
#include "project.h"
#include "meteo.h"
#include "commonConstants.h"

//cout
#include <iostream>

void qualityControlSimple(const quality::qualityMeteo myQualityMeteo, meteoVariable myVar, const Crit3DTime& myCrit3DTime,
                            Crit3DMeteoPoint* myMeteoPoints, int nrMeteoPoints)
{
    float myValue;

    quality::qualityRange* myQualityRange = quality::getQualityRange(myVar, myQualityMeteo);

    for (int i = 0; i < nrMeteoPoints; i++)
    {
        myValue = myMeteoPoints[i].getMeteoPointValueH(myCrit3DTime.date, myCrit3DTime.getHour(), myCrit3DTime.getMinutes(), myVar);

        if (myQualityRange == NULL && myValue != NODATA)
            myMeteoPoints[i].myQuality = quality::accepted;
        else if (myValue == NODATA)
            myMeteoPoints[i].myQuality = quality::missing;
        else if (myValue < myQualityRange->getMin() || myValue > myQualityRange->getMax())
            myMeteoPoints[i].myQuality = quality::wrong;
        else
            myMeteoPoints[i].myQuality = quality::accepted;
    }
}

void setSpatialQualityControlSettings(Crit3DInterpolationSettings* mySettings, meteoVariable myVar)
{
    mySettings->setUseOrogIndex(false);
    mySettings->setUseAspect(false);
    mySettings->setUseTAD(false);
    mySettings->setUseOrogIndex(false);
    mySettings->setUseDewPoint(false);
    mySettings->setUseGenericProxy(false);
    mySettings->setUseSeaDistance(false);
    mySettings->setInterpolationMethod(geostatisticsMethods::idw);
    mySettings->setIsCrossValidation(true);

    if (myVar == airTemperature)
    {
        mySettings->setUseHeight(true);
        mySettings->setUseThermalInversion(true);
    }

}


bool isDataPresent(Crit3DProject* myProject, meteoVariable myVar, Crit3DTime myDateTime)
{
    float value;
    for (int i = 0; i < myProject->nrMeteoPoints; i++)
    {
         value = myProject->meteoPoints[i].getMeteoPointValueH(myDateTime.date, myDateTime.getHour(), myDateTime.getMinutes(), myVar);
         if (value != NODATA)
             return true;
    }
    return false;
}


/*!
 * \brief Try to cover missing data
 * \param myProject pointer to Crit3DProject
 * \param myVar
 * \param myDateTime
 * \param nrReplacedData
 * \return true if data has been replaced, false otherwise
 */
bool checkLackOfData(Crit3DProject* myProject, meteoVariable myVar, Crit3DTime myDateTime, long* nrReplacedData)
{
    if (isDataPresent(myProject, myVar, myDateTime))
        return true;

    int indexPoint = 0;
    Crit3DDate currentDate = myDateTime.date;
    int hour = myDateTime.getHour();
    int minutes = myDateTime.getMinutes();

    //--------------------------------------------------------
    // STEP 1: it uses daily data
    //--------------------------------------------------------
    // TODO

    //--------------------------------------------------------
    // STEP 2: it computes hourly mean
    // in the two months around the missing data
    // only for the station with the highest number of values
    //--------------------------------------------------------
    int i, day, nrValues;
    float myValue, avgValue;
    double sumValues;
    bool isReplacedData = false;

    for (i = 0; i < myProject->nrMeteoPoints; i++)
    {
        nrValues = 0;
        sumValues = 0;
        for (day = -30; day <= 30; day++)
        {
            myValue = myProject->meteoPoints[i].getMeteoPointValueH(currentDate.addDays(day), hour, minutes, myVar);
            if (myValue != NODATA)
            {
                nrValues++;
                sumValues += myValue;
            }
        }
        // need at least two week of data!
        if (nrValues >= 14)
        {
            avgValue = sumValues/nrValues;
            myProject->meteoPoints[indexPoint].setMeteoPointValueH(currentDate, hour, minutes, myVar, avgValue);
            isReplacedData = true;
        }
        if (isReplacedData)
        {
            (*nrReplacedData)++;
            return true;
        }
    }

    //------------------------------------------------------
    // STEP 3: use default values for some variables
    //------------------------------------------------------
    if (myVar == windIntensity)
    {
        myProject->meteoPoints[indexPoint].setMeteoPointValueH(currentDate, hour, minutes, myVar, myProject->windIntensityDefault);
        (*nrReplacedData)++;
        return true;
    }

    return false;
}


bool passDataToInterpolation(Crit3DProject* myProject, meteoVariable myVar, Crit3DTime myCrit3DTime, bool doQualityControl)
{
    int myCounter = 0;
    float myValue, myX, myY, myZ;
    int pointCode;

    if (doQualityControl) qualityControl(myProject, myVar, myCrit3DTime);

    clearInterpolationPoints();

    for (int i = 0; i < myProject->nrMeteoPoints; i++)
    {
        if (myProject->meteoPoints[i].myQuality == quality::accepted)
        {
            myValue = myProject->meteoPoints[i].getMeteoPointValueH(myCrit3DTime.date, myCrit3DTime.getHour(), myCrit3DTime.getMinutes(), myVar);
            myX = myProject->meteoPoints[i].point.utm.x;
            myY = myProject->meteoPoints[i].point.utm.y;
            myZ = myProject->meteoPoints[i].point.z;
            pointCode = i;
            if (addInterpolationPoint(pointCode, myValue, myX, myY, myZ, NODATA, NODATA, NODATA, NODATA, NODATA))
                myCounter++;
        }
    }

    return (myCounter > 0);
}


bool computeResiduals(Crit3DProject* myProject, meteoVariable myVar, Crit3DTime myCrit3DTime, bool derivedVar)
{

    float myValue, interpolatedValue;
    interpolatedValue = NODATA;
    myValue = NODATA;

    if (myVar == noMeteoVar) return false;

    for (int i = 0; i < myProject->nrMeteoPoints; i++)
    {
        myProject->meteoPoints[i].residual = NODATA;
        if (myProject->meteoPoints[i].myQuality == quality::accepted)
        {
            if (! derivedVar)
            {
                myValue = myProject->meteoPoints[i].getMeteoPointValueH(myCrit3DTime.date, myCrit3DTime.getHour(), myCrit3DTime.getMinutes(), myVar);
                setindexPointJacknife(i);
                interpolatedValue = interpolate(myVar, myProject->meteoPoints[i].point.utm.x,
                                                myProject->meteoPoints[i].point.utm.y,
                                                myProject->meteoPoints[i].point.z,
                                                NODATA, NODATA, NODATA, NODATA);

                setindexPointJacknife(NODATA);

                if (myVar == precipitation)
                {
                    if (myValue != NODATA)
                        if (myValue < PREC_THRESHOLD) myValue=0.;

                    if (interpolatedValue != NODATA)
                        if (interpolatedValue < PREC_THRESHOLD) interpolatedValue=0.;
                }
            }
            else
                if (myVar == airDewTemperature)
                {
                    float humid = myProject->meteoPoints[i].getMeteoPointValueH(myCrit3DTime.date, myCrit3DTime.getHour(), myCrit3DTime.getMinutes(), airHumidity);
                    float temp = myProject->meteoPoints[i].getMeteoPointValueH(myCrit3DTime.date, myCrit3DTime.getHour(), myCrit3DTime.getMinutes(), airTemperature);
                    float tdew = myProject->meteoPoints[i].getMeteoPointValueH(myCrit3DTime.date, myCrit3DTime.getHour(), myCrit3DTime.getMinutes(), airDewTemperature);
                    if (humid != NODATA && temp != NODATA && tdew != NODATA)
                        interpolatedValue = tDewFromRelHum(humid, temp);
                }

            if ((interpolatedValue != NODATA) && (myValue != NODATA))
                myProject->meteoPoints[i].residual = interpolatedValue - myValue;

        }
    }

    return true;
}

float findThreshold(float value, meteoVariable myVar, float dev, float nDev, float devZ, float dist, int myHour)
{
    float zWeight, distWeight, threshold;

    if (myVar == precipitation)
    {
        distWeight = maxValue(1., dist / 2000);
        if (value < PREC_THRESHOLD)
            threshold = maxValue(5., distWeight + dev * (nDev + 1));
        else
            return 900.;
    }
    else if (myVar == airTemperature || myVar == airDewTemperature)
    {
        threshold = 1.;
        if (myHour > 22 || myHour <= 10)
        {
            zWeight = devZ / 50.;
            distWeight = dist / 2500.;
        }
        else
        {
            zWeight = devZ / 100.;
            distWeight = dist / 5000.;
        }
        threshold = minValue(minValue(distWeight + threshold + zWeight, 12.) + dev * nDev, 15.);
    }
    else if (myVar == airHumidity)
    {
        threshold = 8.;
        zWeight = devZ / 100.;
        distWeight = dist / 1000.;
        threshold += zWeight + distWeight + dev * nDev;
    }
    else if (myVar == windIntensity)
    {
        threshold = 1;
        zWeight = devZ / 100.;
        distWeight = dist / 5000.;
        threshold += zWeight + distWeight + dev * nDev;
    }
    else if (myVar == globalIrradiance)
        threshold = maxValue(dev * (nDev + 1), 5.);
    else if (myVar == atmTransmissivity)
        threshold = maxValue(dev * nDev, 0.5);
    else
        threshold = dev * nDev;

    return threshold;
}

void qualityControlSpatial(Crit3DProject* myProject, meteoVariable myVar, const Crit3DTime& myCrit3DTime)
{
    int i;
    float stdDev, stdDevZ, minDist, myValue, myResidual;
    std::vector <int> listIndex;
    std::vector <float> listResiduals;

    int myHour = myCrit3DTime.getHour();

    Crit3DInterpolationSettings myTmpSettings;
    myTmpSettings = myProject->interpolationSettings;
    setSpatialQualityControlSettings(&(myProject->interpolationSettings), myVar);
    setInterpolationSettings(&(myProject->interpolationSettings));

    if (passDataToInterpolation(myProject, myVar, myCrit3DTime, false))
    {
        if (! preInterpolation(myVar)) return;
        if (! computeResiduals(myProject, myVar, myCrit3DTime, false)) return;
        if (passDataToInterpolation(myProject, myVar, myCrit3DTime, false) == 0) return;

        for (i = 0; i < myProject->nrMeteoPoints; i++)
            if (myProject->meteoPoints[i].myQuality == quality::accepted)
            {

                if (neighbourhoodVariability(myProject->meteoPoints[i].point.utm.x,
                         myProject->meteoPoints[i].point.utm.y,
                         myProject->meteoPoints[i].point.z,
                         10, &stdDev, &stdDevZ, &minDist))
                {
                    myValue = myProject->meteoPoints[i].getMeteoPointValueH(myCrit3DTime.date, myCrit3DTime.getHour(), myCrit3DTime.getMinutes(), myVar);
                    myResidual = myProject->meteoPoints[i].residual;
                    stdDev = maxValue(stdDev, myValue/100.);
                    if (fabs(myResidual) > findThreshold(myValue, myVar, stdDev, 2, stdDevZ, minDist, myHour))
                    {
                        listIndex.push_back(i);
                        myProject->meteoPoints[i].myQuality = quality::wrong;
                    }
                }
            }

        if (listIndex.size() > 0)
            if (passDataToInterpolation(myProject, myVar, myCrit3DTime, false))
            {
                preInterpolation(myVar);

                float interpolatedValue;
                for (i=0; i < int(listIndex.size()); i++)
                {
                    interpolatedValue = interpolate(myVar,
                                            myProject->meteoPoints[listIndex[i]].point.utm.x,
                                            myProject->meteoPoints[listIndex[i]].point.utm.y,
                                            myProject->meteoPoints[listIndex[i]].point.z,
                                            NODATA, NODATA, NODATA, NODATA);
                    myValue = myProject->meteoPoints[listIndex[i]].getMeteoPointValueH(myCrit3DTime.date, myCrit3DTime.getHour(), myCrit3DTime.getMinutes(), myVar);

                    listResiduals.push_back(interpolatedValue-myValue);
                }

                passDataToInterpolation(myProject, myVar, myCrit3DTime, false);

                for (i=0; i < int(listIndex.size()); i++)
                {
                    if (neighbourhoodVariability(myProject->meteoPoints[listIndex[i]].point.utm.x,
                             myProject->meteoPoints[listIndex[i]].point.utm.y,
                             myProject->meteoPoints[listIndex[i]].point.z,
                             10, &stdDev, &stdDevZ, &minDist))
                    {
                        myResidual = listResiduals[i];
                        myValue = myProject->meteoPoints[listIndex[i]].getMeteoPointValueH(myCrit3DTime.date, myCrit3DTime.getHour(), myCrit3DTime.getMinutes(), myVar);
                        if (fabs(myResidual) > findThreshold(myValue, myVar, stdDev, 3, stdDevZ, minDist, myHour))
                            myProject->meteoPoints[listIndex[i]].myQuality = quality::wrong;
                        else
                            myProject->meteoPoints[listIndex[i]].myQuality = quality::accepted;
                    }
                    else
                        myProject->meteoPoints[listIndex[i]].myQuality = quality::accepted;
                }
            }
    }

    myProject->interpolationSettings = myTmpSettings;
    setInterpolationSettings(&(myProject->interpolationSettings));
}

void qualityControl(Crit3DProject* myProject, meteoVariable myVar, const Crit3DTime& myCrit3DTime)
{
    if (myProject->nrMeteoPoints == 0) return;

    qualityControlSimple(myProject->qualityParameters, myVar, myCrit3DTime, myProject->meteoPoints, myProject->nrMeteoPoints);
    qualityControlSpatial(myProject, myVar, myCrit3DTime);
}

bool postInterpolation(meteoVariable myVar, gis::Crit3DRasterGrid* myGrid)
{
    if (myVar == airTemperature || myVar == airDewTemperature)
        setTemperatureScale(myGrid->colorScale);
    else if (myVar == precipitation)
        setPrecipitationScale(myGrid->colorScale);
    else if (myVar == airHumidity)
        setRelativeHumidityScale(myGrid->colorScale);
    else if (myVar == globalIrradiance)
        setRadiationScale(myGrid->colorScale);
    else if (myVar == windIntensity)
        setWindIntensityScale(myGrid->colorScale);
    else if (myVar == wetnessDuration)
        setLeafWetnessScale(myGrid->colorScale);

    return true;
}

int computeTransmissivity(Crit3DMeteoPoint* myMeteoPoints,
                          int nrPoints, int intervalWidth, int myHourInterval,
                          Crit3DTime myCentralTime,
                          const gis::Crit3DRasterGrid& myDtm)
{
    int mySemiInterval = (intervalWidth-1)/2;
    int myDeltaSeconds = 3600 / myHourInterval;
    int mySemiIntervalSeconds = myDeltaSeconds * mySemiInterval;
    float* myObsRad;
    int myIndex;
    Crit3DTime myTimeIni =  myCentralTime.addSeconds(-mySemiIntervalSeconds);
    Crit3DTime myTimeFin =  myCentralTime.addSeconds(mySemiIntervalSeconds);
    Crit3DTime myCurrentTime;
    int myCounter = 0;
    int indexDate;
    int indexSubDaily;

    gis::Crit3DPoint myPoint;

    for (int i = 0; i < nrPoints; i++)
        if (myMeteoPoints[i].getMeteoPointValueH(myCentralTime.date, myCentralTime.getHour(), myCentralTime.getMinutes(), globalIrradiance) != NODATA)
        {
            myIndex = 0;
            myObsRad = (float *) calloc(intervalWidth, sizeof(float));
            myCurrentTime = myTimeIni;
            while (myCurrentTime <= myTimeFin)
            {
                myObsRad[myIndex] = myMeteoPoints[i].getMeteoPointValueH(myCurrentTime.date, myCurrentTime.getHour(), myCurrentTime.getMinutes(), globalIrradiance);
                myCurrentTime = myCurrentTime.addSeconds(myDeltaSeconds);
                myIndex++;
            }

            myPoint.utm.x = myMeteoPoints[i].point.utm.x;
            myPoint.utm.y = myMeteoPoints[i].point.utm.y;
            myPoint.z = myMeteoPoints[i].point.z;

            indexDate = -myCentralTime.date.daysTo(myMeteoPoints[i].obsDataH->date);
            indexSubDaily = (myHourInterval * myCentralTime.getHour()) + myCentralTime.getMinutes() % (60 / myHourInterval);

            myMeteoPoints[i].obsDataH[indexDate].transmissivity[indexSubDaily] =
                    radiation::computePointTransmissivity(
                        myPoint, myCentralTime, myObsRad,
                        intervalWidth, myDeltaSeconds, myDtm);

            myCounter++;
        }

    return myCounter;
}

int computeTransmissivityFromTRange(Crit3DMeteoPoint* meteoPoints, int nrPoints,
                                     int hourInterval, Crit3DTime currentTime)
{
    int deltaSeconds = 3600 / hourInterval;
    int counter = 0;
    int indexDate;
    Crit3DTime timeTmp;
    float temp, tmin, tmax;
    float transmissivity;
    int i;

    for (i = 0; i < nrPoints; i++)
    {
        tmin = NODATA;
        tmax = NODATA;
        int mySeconds = deltaSeconds;
        timeTmp = currentTime;
        timeTmp.time = 0;
        while (mySeconds < DAY_SECONDS)
        {
            timeTmp = timeTmp.addSeconds(deltaSeconds);
            temp = meteoPoints[i].getMeteoPointValueH(timeTmp.date, timeTmp.getHour(), timeTmp.getMinutes(), airTemperature);
            if (temp != NODATA)
            {
                if (tmin == NODATA || temp < tmin)
                    tmin = temp;
                if (tmax == NODATA || temp > tmax)
                    tmax = temp;
            }
            mySeconds+=3600;
        }

        if (tmin != NODATA && tmax != NODATA)
        {
            transmissivity = radiation::computePointTransmissivitySamani(tmin, tmax, float(0.17));
            if (transmissivity != NODATA)
            {
                indexDate = -currentTime.date.daysTo(meteoPoints[i].obsDataH->date);
                int nrDayValues = hourInterval * 24 +1;
                for (int j = 0; j < nrDayValues; j++)
                    meteoPoints[i].obsDataH[indexDate].transmissivity[j] = transmissivity;
                //midnight
                meteoPoints[i].obsDataH[indexDate+1].transmissivity[0] = transmissivity;
                counter++;
            }
        }
    }

    return counter;
}


bool computeLeafWetnessMap(Crit3DProject* myProject)
{
    float relHumidity, precipitation, leafWetness;

    gis::Crit3DRasterGrid* myMap = myProject->meteoMaps->leafWetnessMap;

    for (long row = 0; row < myMap->header->nrRows; row++)
        for (long col = 0; col < myMap->header->nrCols; col++)
        {
            //initialize
            myMap->value[row][col] = myMap->header->flag;

            if (myProject->dtm.value[row][col] != myProject->dtm.header->flag)
            {
                relHumidity = myProject->meteoMaps->airHumidityMap->value[row][col];
                precipitation = myProject->meteoMaps->precipitationMap->value[row][col];

                if (relHumidity != myProject->meteoMaps->airHumidityMap->header->flag
                        && precipitation != myProject->meteoMaps->precipitationMap->header->flag)
                {
                    leafWetness = 0;
                    if (precipitation > 0.0)
                        leafWetness = 1;
                    else if (relHumidity > 92.0)
                        leafWetness = 1;
                    //TODO: ora precedente prec > 2mm ?

                    myMap->value[row][col] = leafWetness;
                }
            }
        }

    return gis::updateMinMaxRasterGrid(myMap);
}




bool computeET0Map(Crit3DProject* myProject)
{
    float myET0;
    float myGlobalRadiation, myTransmissivity, myClearSkyTransmissivity;
    float myTemperature, myRelHumidity, myWindSpeed;
    float myHeight;

    gis::Crit3DRasterGrid* myEt0Map = myProject->meteoMaps->ET0Map;

    for (long myRow = 0; myRow < myEt0Map->header->nrRows; myRow++)
        for (long myCol = 0; myCol < myEt0Map->header->nrCols; myCol++)
        {
            myEt0Map->value[myRow][myCol] = myEt0Map->header->flag;

            myHeight = myProject->dtm.value[myRow][myCol];

            if (myHeight != myProject->dtm.header->flag)
            {
                myGlobalRadiation = myProject->meteoMaps->radiationMaps->globalRadiationMap->value[myRow][myCol];

                //cercare soluzione migliore
                //if (myGlobalRadiation == myProject->meteoMaps->radiationMaps->globalRadiationMap->header->flag)
                //    myGlobalRadiation = myProject->meteoMaps->radiationMaps->globalRadiationMap->neighbourValue(myRow, myCol);

                myTransmissivity = myProject->meteoMaps->radiationMaps->transmissivityMap->value[myRow][myCol];
                myClearSkyTransmissivity = myProject->meteoMaps->radiationMaps->clearSkyTransmissivityMap->value[myRow][myCol];
                myTemperature = myProject->meteoMaps->airTemperatureMap->value[myRow][myCol];
                myRelHumidity = myProject->meteoMaps->airHumidityMap->value[myRow][myCol];
                myWindSpeed = myProject->meteoMaps->windIntensityMap->value[myRow][myCol];

                if (myGlobalRadiation != myProject->meteoMaps->radiationMaps->globalRadiationMap->header->flag
                        && myTransmissivity != myProject->meteoMaps->radiationMaps->transmissivityMap->header->flag
                        && myClearSkyTransmissivity != myProject->meteoMaps->radiationMaps->clearSkyTransmissivityMap->header->flag
                        && myTemperature != myProject->meteoMaps->airTemperatureMap->header->flag
                        && myRelHumidity != myProject->meteoMaps->airHumidityMap->header->flag
                        && myWindSpeed != myProject->meteoMaps->windIntensityMap->header->flag)
                {
                    myET0 = ET0_Penman_hourly(myHeight, myTransmissivity / myClearSkyTransmissivity,
                                      myGlobalRadiation, myTemperature, myRelHumidity, myWindSpeed);

                    myEt0Map->value[myRow][myCol] = myET0;
                }
            }
        }

    return gis::updateMinMaxRasterGrid(myEt0Map);
}

bool computeHumidityMap(const gis::Crit3DRasterGrid& myTemperatureMap,
                        const gis::Crit3DRasterGrid& myDewTemperatureMap,
                        gis::Crit3DRasterGrid* myHumidityMap)
{
    if (myHumidityMap == NULL) return false;
    if (! myHumidityMap->isLoaded) return false;

    if (! (*(myHumidityMap->header) == *(myTemperatureMap.header) && *(myHumidityMap->header) == *(myDewTemperatureMap.header)))
        return false;

    for (long myRow = 0; myRow < myHumidityMap->header->nrRows ; myRow++)
        for (long myCol = 0; myCol < myHumidityMap->header->nrCols; myCol++)
            if ((myTemperatureMap.value[myRow][myCol] != myTemperatureMap.header->flag) &&
                    (myDewTemperatureMap.value[myRow][myCol] != myDewTemperatureMap.header->flag))
                        myHumidityMap->value[myRow][myCol] = relHumFromTdew(myDewTemperatureMap.value[myRow][myCol], myTemperatureMap.value[myRow][myCol]);

    if (! gis::updateMinMaxRasterGrid(myHumidityMap))
        return (false);

    return postInterpolation(airDewTemperature, myHumidityMap);
}

bool interpolationProjectDtm(Crit3DProject* myProject, meteoVariable myVar,
                             const Crit3DTime& myCrit3DTime, bool isLoadData)
{
    myProject->interpolationSettings.setCurrentDate(myCrit3DTime.date);
    myProject->interpolationSettings.setCurrentHour(myCrit3DTime.getHour());
    setInterpolationSettings(&(myProject->interpolationSettings));

    bool dataAvailable = true;
    if (myProject->meteoDataConsistency(myVar, myCrit3DTime, myCrit3DTime) == 0.0)
    {
        QDate qDate = getQDate(myCrit3DTime.date);
        if (! isLoadData || ! myProject->loadObsDataAllPointsVar(myVar, qDate, qDate))
            dataAvailable = false;
    }

    if (! dataAvailable) return false;

    gis::Crit3DRasterGrid* myMap;
    myMap = myProject->meteoMaps->getMapFromVar(myVar);    

    bool isSuccessful = false;
    if (passDataToInterpolation(myProject, myVar, myCrit3DTime, true))
    {
        if (preInterpolation(myVar))
        {
            if (interpolateGridDtm(myMap, myProject->dtm, myVar))
                isSuccessful = true;
            else
                myProject->projectError = "Function interpolationProjectDtm: interpolateGridDtm";
        }
        else
            myProject->projectError = "Function interpolationProjectDtm: preparing interpolation";
    }
    else
        myProject->projectError = "Function interpolationProjectDtm: passing data to interpolation";

    if (! isSuccessful) return false;

    myMap->timeString = getQDateTime(myCrit3DTime).toString("yyyyMMdd:hhmm").toStdString();

    if (postInterpolation(myVar, myMap))
        return true;
    else
    {
        myProject->projectError = "Function interpolationProjectDtm: post interpolation";
        return false;
    }
}

bool computeRadiationProjectDtm(Crit3DProject* myProject, const Crit3DTime& myCrit3DTime, bool isLoadData)
{
    bool myResult = false;

    myProject->radiationSettings.setGisSettings(&(myProject->gisSettings));
    radiation::setRadiationSettings(&(myProject->radiationSettings));

    gis::Crit3DPoint myDtmCenter = myProject->dtm.mapCenter();
    int intervalWidth = radiation::estimateTransmissivityWindow(myProject->dtm, *(myProject->meteoMaps->radiationMaps), &myDtmCenter, myCrit3DTime, (int)(3600 / myProject->hourlyIntervals));
    int myTimeStep = getTimeStepFromHourlyInterval(myProject->hourlyIntervals);

    float myDeltaTime = (intervalWidth-1) * 0.5 * myTimeStep;
    Crit3DTime myTimeIni = myCrit3DTime.addSeconds(-myDeltaTime);
    Crit3DTime myTimeFin = myCrit3DTime.addSeconds(myDeltaTime);
    QDateTime myQTimeIni = getQDateTime(myTimeIni);
    QDateTime myQTimeFin = getQDateTime(myTimeFin);

    bool radAvailable;
    bool transComputed = false;

    if (isLoadData)
        myProject->loadObsDataAllPointsVar(globalIrradiance, myQTimeIni.date(), myQTimeFin.date());

    radAvailable = (myProject->meteoDataConsistency(globalIrradiance, myTimeIni, myTimeFin) > 0.5);

    if (radAvailable)
        if(computeTransmissivity(myProject->meteoPoints, myProject->nrMeteoPoints, intervalWidth, myProject->hourlyIntervals, myCrit3DTime, myProject->dtm) > 0)
            transComputed = true;

    if (! transComputed)
    {
        QDate transmissivityDate = getQDate(myCrit3DTime.date);
        QDate yesterday = transmissivityDate.addDays(-1);
        if ((transmissivityDate == myProject->lastDateTransmissivity)
            || ((yesterday == myProject->lastDateTransmissivity) && (myCrit3DTime.getHour() == 0))) //midnight
            transComputed = true;
        else
        {
            Crit3DTime timeIniTemp = myCrit3DTime;
            Crit3DTime timeFinTemp = myCrit3DTime;
            timeIniTemp.time = myTimeStep;
            timeFinTemp.time = DAY_SECONDS;
            bool tempLoaded = false;

            tempLoaded = (myProject->meteoDataConsistency(airTemperature, timeIniTemp, timeFinTemp) > 0.5);
            if (! tempLoaded && isLoadData) tempLoaded = (myProject->loadObsDataAllPointsVar(airTemperature, transmissivityDate, transmissivityDate));
            if (tempLoaded && isLoadData) tempLoaded = (myProject->meteoDataConsistency(airTemperature, timeIniTemp, timeFinTemp) > 0.5);
            if (tempLoaded)
                if (computeTransmissivityFromTRange(myProject->meteoPoints, myProject->nrMeteoPoints, myProject->hourlyIntervals, myCrit3DTime))
                {
                    transComputed= true;
                    myProject->lastDateTransmissivity = transmissivityDate;
                }
        }
    }

    if (! transComputed)
    {
        myProject->projectError = "Function computeRadiationProjectDtm: transmissivity data unavailable";
        return false;
    }

    if (! passDataToInterpolation(myProject, atmTransmissivity, myCrit3DTime, true))
    {
        myProject->projectError = "Function computeRadiationProjectDtm: no transmissivity data available";
        return false;
    }

    if (preInterpolation(atmTransmissivity))
        if (! interpolateGridDtm(myProject->meteoMaps->radiationMaps->transmissivityMap, myProject->dtm, atmTransmissivity))
        {
            myProject->projectError = "Function computeRadiationProjectDtm: error interpolating transmissivity";
            return false;
        }

    if (radiation::computeRadiationGridPresentTime(myProject->dtm, myProject->meteoMaps->radiationMaps, myCrit3DTime))
        myResult = setRadiationScale(myProject->meteoMaps->radiationMaps->globalRadiationMap->colorScale);
    else
        myProject->projectError = "Function computeRadiationProjectDtm: error computing irradiance";

    return myResult;
}


bool interpolationProjectDtmMain(Crit3DProject* myProject, meteoVariable myVar, const Crit3DTime& myCrit3DTime, bool isLoadData)
{
    bool myResult = false;

    if (myVar == globalIrradiance)
        myResult = (computeRadiationProjectDtm(myProject, myCrit3DTime, isLoadData));
    else if (myVar == airHumidity)
    {
        if (myProject->interpolationSettings.getUseDewPoint())
        {
            std::string myTime = getQDateTime(myCrit3DTime).toString("yyyyMMdd:hhmm").toStdString();
            if (myProject->meteoMaps->airTemperatureMap->timeString != myTime)
                if (! interpolationProjectDtm(myProject, airTemperature, myCrit3DTime, isLoadData))
                    return false;

            if (interpolationProjectDtm(myProject, airDewTemperature, myCrit3DTime, isLoadData))
                myResult = computeHumidityMap(*(myProject->meteoMaps->airTemperatureMap),
                                              *(myProject->meteoMaps->airDewTemperatureMap),
                                              myProject->meteoMaps->airHumidityMap);
        }
        else
            myResult = interpolationProjectDtm(myProject, airHumidity, myCrit3DTime, isLoadData);
    }
    else if (myVar == windIntensity)
    {
        myResult = interpolationProjectDtm(myProject, windIntensity, myCrit3DTime, isLoadData);
        if (myResult == false)
        {
            myProject->meteoMaps->windIntensityMap->setConstantValueWithBase(myProject->windIntensityDefault, myProject->dtm);
            myResult = postInterpolation(windIntensity, myProject->meteoMaps->windIntensityMap);
        }
    }
    else
    {
        //other variables
        myResult = interpolationProjectDtm(myProject, myVar, myCrit3DTime, isLoadData);
    }

    return myResult;
}

meteoVariable getMeteoVarFromAggregationType(meteoVariable myVar, aggregationType myAggregation)
{
    if (myVar == airTemperature)
    {
        if (myAggregation == aggregationMin)
            return dailyAirTemperatureMin;
        else if (myAggregation == aggregationMax)
            return dailyAirTemperatureMax;
        else if (myAggregation == aggregationMean)
            return dailyAirTemperatureAvg;
    }
    else if (myVar == airHumidity)
    {
        if (myAggregation == aggregationMin)
            return dailyAirHumidityMin;
        else if (myAggregation == aggregationMax)
            return dailyAirHumidityMax;
        else if (myAggregation == aggregationMean)
            return dailyAirHumidityAvg;
    }
    else if (myVar == windIntensity)
        return dailyWindIntensityAvg;
    else if (myVar == precipitation)
        return dailyPrecipitation;
    else if (myVar == potentialEvapotranspiration)
        return potentialEvapotranspiration;
    else if (myVar == realEvaporation)
        return realEvaporation;
    else if (myVar == globalIrradiance)
        return dailyGlobalRadiation;
    else if (myVar == directIrradiance)
        return dailyDirectRadiation;
    else if (myVar == diffuseIrradiance)
        return dailyDiffuseRadiation;
    else if (myVar == reflectedIrradiance)
        return dailyReflectedRadiation;
    else if (myVar == wetnessDuration)
        return wetnessDuration;

    return noMeteoVar;
}


bool aggregateAndSaveDailyMap(Crit3DProject* myProject, meteoVariable myVar,
                         aggregationType myAggregation, const Crit3DDate& myDate,
                         const QString& dailyPath, const QString& hourlyPath, const QString& myArea)
{
    std::string myError;
    int myTimeStep = 3600. / myProject->hourlyIntervals;
    Crit3DTime myTimeIni(myDate, myTimeStep);
    Crit3DTime myTimeFin(myDate.addDays(1), 0.);

    gis::Crit3DRasterGrid* myMap = new gis::Crit3DRasterGrid();
    myMap->initializeGrid(myProject->dtm);
    gis::Crit3DRasterGrid* myAggrMap = new gis::Crit3DRasterGrid();
    myAggrMap->initializeGrid(myProject->dtm);

    long myRow, myCol;
    int nrAggrMap = 0;

    for (Crit3DTime myTime = myTimeIni; myTime<=myTimeFin; myTime=myTime.addSeconds(myTimeStep))
    {
        if (gis::readEsriGrid((hourlyPath + getOutputNameHourly(myVar, myTime, myArea)).toStdString(), myMap, &myError))
        {
            if (myTime == myTimeIni)
            {
                for (myRow = 0; myRow < myAggrMap->header->nrRows; myRow++)
                    for (myCol = 0; myCol < myAggrMap->header->nrCols; myCol++)
                        myAggrMap->value[myRow][myCol] = myMap->value[myRow][myCol];

                nrAggrMap++;
            }
            else
            {
                if (myAggregation == aggregationMin)
                    gis::mapAlgebra(myAggrMap, myMap, myAggrMap, operationMin);
                else if (myAggregation == aggregationMax)
                    gis::mapAlgebra(myAggrMap, myMap, myAggrMap, operationMax);
                else if (myAggregation == aggregationSum || myAggregation == aggregationMean)
                    gis::mapAlgebra(myAggrMap, myMap, myAggrMap, operationSum);
                else if (myAggregation == aggregationIntegration)
                    gis::mapAlgebra(myAggrMap, myMap, myAggrMap, operationSum);
                else
                {
                    myProject->logError("wrong aggregation type in function 'aggregateAndSaveDailyMap'");
                    return(false);
                }
                nrAggrMap++;
            }
        }
    }

    if (myAggregation == aggregationMean)
        gis::mapAlgebra(myAggrMap, nrAggrMap, myAggrMap, operationDivide);
    else if (myAggregation == aggregationIntegration)
        if (myVar == globalIrradiance || myVar == directIrradiance || myVar == diffuseIrradiance || myVar == reflectedIrradiance)
            gis::mapAlgebra(myAggrMap, float(myTimeStep) / 1000000.0, myAggrMap, operationProduct);

    meteoVariable myAggrVar = getMeteoVarFromAggregationType(myVar, myAggregation);
    QString varName = getVarNameFromMeteoVariable(myAggrVar);
    QDate qDate = getQDate(myTimeIni.date);

    QString filename = getOutputNameDaily("ARPA", varName, myArea , "", qDate);

    //geoserver - no error check
    QString geoserverFileName = myProject->getGeoserverPath() + filename;
    gis::writeEsriGrid(geoserverFileName.toStdString(), myAggrMap, &myError);

    QString outputFileName = dailyPath + filename;
    bool isOk = gis::writeEsriGrid(outputFileName.toStdString(), myAggrMap, &myError);
    myMap->freeGrid();
    myAggrMap->freeGrid();

    if (! isOk)
    {
        myProject->logError("aggregateMapToDaily: " + QString::fromStdString(myError));
        return false;
    }

    return true;
}


bool loadDailyMeteoMap(Crit3DProject* myProject, meteoVariable myDailyVar, QDate myDate, const QString& myArea)
{
    QString myPath = myProject->path + myProject->dailyOutputPath + myDate.toString("yyyy/MM/dd/");
    QString varName = getVarNameFromMeteoVariable(myDailyVar);
    QString myFileName = myPath + getOutputNameDaily("ARPA", varName, myArea, "", myDate);
    std::string myError;

    QFile myFile;
    myFile.setFileName(myFileName + ".hdr");
    if (! myFile.exists()) return false;

    if (!gis::readEsriGrid(myFileName.toStdString(), myProject->meteoMaps->getMapFromVar(myDailyVar), &myError))
    {
        myProject->logError(QString::fromStdString(myError));
        return false;
    }

    return true;
}

bool saveMeteoHourlyOutput(Crit3DProject* myProject, meteoVariable myVar, const QString& myOutputPath,
                     Crit3DTime myCrit3DTime, const QString myArea)
{
    QString fileName = getOutputNameHourly(myVar, myCrit3DTime, myArea);
    QString outputFileName = myOutputPath + fileName;
    std::string myErrorString;
    gis::Crit3DRasterGrid* myMap;
    myMap = myProject->meteoMaps->getMapFromVar(myVar);

    if (! gis::writeEsriGrid(outputFileName.toStdString(), myMap, &myErrorString))
    {
        myProject->logError(QString::fromStdString(myErrorString));
        return false;
    }

    return true;
}

bool interpolateAndSaveHourlyMeteo(Crit3DProject* myProject, meteoVariable myVar,
                        const Crit3DTime& myCrit3DTime, const QString& myOutputPath,
                        bool isSave, const QString& myArea)
{
    if (! interpolationProjectDtmMain(myProject, myVar, myCrit3DTime, false))
    {
        myProject->logError("interpolateAndSave: interpolation of " + getVarNameFromMeteoVariable(myVar) + " at time: " + getQStringFromCrit3DTime(myCrit3DTime));
        return false;
    }

    if (isSave)
        return saveMeteoHourlyOutput(myProject, myVar, myOutputPath, myCrit3DTime, myArea);
    else
        return true;
}

#include <stdlib.h>
#include <math.h>
#include <vector>
#include <string>
#include <QString>
#include <QFile>

#include "interpolation.h"
#include "interpolationCmd.h"
#include "solarRadiation.h"
#include "dataHandler.h"
#include "atmosphere.h"
#include "vine3DProject.h"
#include "commonConstants.h"
#include "utilities.h"
#include "quality.h"
#include "transmissivity.h"
#include "spatialControl.h"

//cout
#include <iostream>


bool isDataPresent(Vine3DProject* myProject, meteoVariable myVar, Crit3DTime myDateTime)
{
    float value;
    for (int i = 0; i < myProject->nrMeteoPoints; i++)
    {
         value = myProject->meteoPoints[i].getMeteoPointValueH(myDateTime.date, myDateTime.getHour(), myDateTime.getMinutes(), myVar);
         if (int(value) != NODATA)
             return true;
    }
    return false;
}

// ---------------------------------------------------------------
// Try to cover missing data
// ---------------------------------------------------------------
bool checkLackOfData(Vine3DProject* myProject, meteoVariable myVar, Crit3DTime myDateTime, long* nrReplacedData)
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
    float sumValues;
    bool isReplacedData = false;

    for (i = 0; i < myProject->nrMeteoPoints; i++)
    {
        nrValues = 0;
        sumValues = 0;
        for (day = -30; day <= 30; day++)
        {
            myValue = myProject->meteoPoints[i].getMeteoPointValueH(currentDate.addDays(day), hour, minutes, myVar);
            if (int(myValue) != NODATA)
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
        myProject->meteoPoints[indexPoint].setMeteoPointValueH(currentDate, hour, minutes, myVar, myProject->meteoSettings->getWindIntensityDefault());
        (*nrReplacedData)++;
        return true;
    }

    return false;
}


bool postInterpolation(meteoVariable myVar, gis::Crit3DRasterGrid* myGrid)
{
    if (myVar == airTemperature || myVar == airDewTemperature)
        setTemperatureScale(myGrid->colorScale);
    else if (myVar == precipitation)
        setPrecipitationScale(myGrid->colorScale);
    else if (myVar == airRelHumidity)
        setRelativeHumidityScale(myGrid->colorScale);
    else if (myVar == globalIrradiance)
        setRadiationScale(myGrid->colorScale);
    else if (myVar == windIntensity)
        setWindIntensityScale(myGrid->colorScale);
    else if (myVar == leafWetness)
        setLeafWetnessScale(myGrid->colorScale);

    return true;
}


bool computeHumidityMap(const gis::Crit3DRasterGrid& myTemperatureMap,
                        const gis::Crit3DRasterGrid& myDewTemperatureMap,
                        gis::Crit3DRasterGrid* myHumidityMap)
{
    if (myHumidityMap == nullptr) return false;
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

bool vine3DInterpolationDem(Vine3DProject* myProject, meteoVariable myVar,
                             const Crit3DTime& myCrit3DTime, bool isLoadData)
{
    bool dataAvailable = true;
    if (myProject->meteoDataConsistency(myVar, myCrit3DTime, myCrit3DTime) == 0.0)
    {
        QDate qDate = getQDate(myCrit3DTime.date);
        if (! isLoadData || ! myProject->loadObsDataAllPointsVar(myVar, qDate, qDate))
            dataAvailable = false;
    }

    if (! dataAvailable) return false;

    gis::Crit3DRasterGrid* myMap = myProject->vine3DMapsH->getMapFromVar(myVar);
    if (myMap == nullptr)
    {
        myMap = &(myProject->dataRaster);
    }

    if (! myProject->interpolationDem(myVar, myCrit3DTime, myMap, false))
        return false;

    if (postInterpolation(myVar, myMap))
        return true;
    else
    {
        myProject->errorString = "Function vine3DInterpolationDem: post interpolation";
        return false;
    }
}


bool vine3DInterpolationDemRadiation(Vine3DProject* myProject, const Crit3DTime& myCrit3DTime, bool isLoadData)
{
    bool myResult = false;

    std::vector <Crit3DInterpolationDataPoint> interpolationPoints;

    myProject->radSettings.setGisSettings(&(myProject->gisSettings));

    gis::Crit3DPoint myDEMCenter = myProject->DEM.mapCenter();
    int intervalWidth = radiation::estimateTransmissivityWindow(&(myProject->radSettings), myProject->DEM, myDEMCenter, myCrit3DTime, (int)(3600 / myProject->meteoSettings->getHourlyIntervals()));
    int myTimeStep = getTimeStepFromHourlyInterval(myProject->meteoSettings->getHourlyIntervals());

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
        if(computeTransmissivity(&(myProject->radSettings), myProject->meteoPoints, myProject->nrMeteoPoints, intervalWidth, myCrit3DTime, myProject->DEM))
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
                if (computeTransmissivityFromTRange(myProject->meteoPoints, myProject->nrMeteoPoints, myCrit3DTime))
                {
                    transComputed= true;
                    myProject->lastDateTransmissivity = transmissivityDate;
                }
        }
    }

    if (! transComputed)
    {
        myProject->errorString = "Function vine3DInterpolationDemRadiation: transmissivity data unavailable";
        return false;
    }

    if (! checkAndPassDataToInterpolation(&(myProject->qualityParameters), atmTransmissivity, myProject->meteoPoints, myProject->nrMeteoPoints, myCrit3DTime,
                                          &(myProject->qualityInterpolationSettings), &(myProject->interpolationSettings), &(myProject->climateParameters),
                                          interpolationPoints, true))
    {
        myProject->errorString = "Function vine3DInterpolationDemRadiation: no transmissivity data available";
        return false;
    }

    if (preInterpolation(interpolationPoints, &(myProject->interpolationSettings), &(myProject->climateParameters), myProject->meteoPoints, myProject->nrMeteoPoints, atmTransmissivity, myCrit3DTime))
        if (! interpolationRaster(interpolationPoints, &(myProject->interpolationSettings), myProject->radiationMaps->transmissivityMap, myProject->DEM, atmTransmissivity, false))
        {
            myProject->errorString = "Function vine3DInterpolationDemRadiation: error interpolating transmissivity";
            return false;
        }

    if (radiation::computeRadiationGridPresentTime(&(myProject->radSettings), myProject->DEM, myProject->radiationMaps, myCrit3DTime))
        myResult = setRadiationScale(myProject->radiationMaps->globalRadiationMap->colorScale);
    else
        myProject->errorString = "Function vine3DInterpolationDemRadiation: error computing irradiance";

    return myResult;
}


bool interpolationProjectDemMain(Vine3DProject* myProject, meteoVariable myVar, const Crit3DTime& myCrit3DTime, bool isLoadData)
{
    bool myResult = false;

    if (myVar == globalIrradiance)
        myResult = (vine3DInterpolationDemRadiation(myProject, myCrit3DTime, isLoadData));
    else if (myVar == airRelHumidity)
    {
        if (myProject->interpolationSettings.getUseDewPoint())
        {
            if (myProject->vine3DMapsH->mapHourlyT->mapTime != myCrit3DTime)
            {
                if (! vine3DInterpolationDem(myProject, airTemperature, myCrit3DTime, isLoadData))
                    return false;
            }

            if (vine3DInterpolationDem(myProject, airDewTemperature, myCrit3DTime, isLoadData))
            {
                myResult = computeHumidityMap(*(myProject->vine3DMapsH->mapHourlyT),
                                              *(myProject->vine3DMapsH->mapHourlyTdew),
                                              myProject->vine3DMapsH->mapHourlyRelHum);
            }
        }
        else
            myResult = vine3DInterpolationDem(myProject, airRelHumidity, myCrit3DTime, isLoadData);
    }
    else if (myVar == windIntensity)
    {
        myResult = vine3DInterpolationDem(myProject, windIntensity, myCrit3DTime, isLoadData);
        if (myResult == false)
        {
            myProject->vine3DMapsH->mapHourlyWindInt->setConstantValueWithBase(myProject->meteoSettings->getWindIntensityDefault(), myProject->DEM);
            myResult = postInterpolation(windIntensity, myProject->vine3DMapsH->mapHourlyWindInt);
        }
    }
    else
    {
        //other variables
        myResult = vine3DInterpolationDem(myProject, myVar, myCrit3DTime, isLoadData);
    }

    return myResult;
}




bool loadDailyMeteoMap(Vine3DProject* myProject, meteoVariable myDailyVar, QDate myDate, const QString& myArea)
{
    QString myPath = myProject->getProjectPath() + myProject->dailyOutputPath + myDate.toString("yyyy/MM/dd/");
    QString varName = QString::fromStdString(MapDailyMeteoVarToString.at(myDailyVar));
    QString myFileName = myPath + getOutputNameDaily(varName, myArea, "", myDate);
    std::string myError;

    QFile myFile;
    myFile.setFileName(myFileName + ".hdr");
    if (! myFile.exists()) return false;

    if (!gis::readEsriGrid(myFileName.toStdString(), myProject->vine3DMapsD->getMapFromVar(myDailyVar), &myError))
    {
        myProject->logError(QString::fromStdString(myError));
        return false;
    }

    return true;
}


bool saveMeteoHourlyOutput(Vine3DProject* myProject, meteoVariable myVar, const QString& myOutputPath,
                     Crit3DTime myCrit3DTime, const QString myArea)
{
    QString fileName = getOutputNameHourly(myVar, getQDateTime(myCrit3DTime), myArea);
    QString outputFileName = myOutputPath + fileName;
    std::string myErrorString;
    gis::Crit3DRasterGrid* myMap;
    if (myVar == globalIrradiance)
    {
        myMap = myProject->radiationMaps->globalRadiationMap;
    }
    else
    {
        myMap = myProject->vine3DMapsH->getMapFromVar(myVar);
    }


    if (! gis::writeEsriGrid(outputFileName.toStdString(), myMap, &myErrorString))
    {
        myProject->logError(QString::fromStdString(myErrorString));
        return false;
    }

    return true;
}


bool interpolateAndSaveHourlyMeteo(Vine3DProject* myProject, meteoVariable myVar,
                        const Crit3DTime& myCrit3DTime, const QString& myOutputPath,
                        bool isSave, const QString& myArea)
{
    if (! interpolationProjectDemMain(myProject, myVar, myCrit3DTime, false))
    {
        Crit3DTime t = myCrit3DTime;
        QString myTimeStr = QString::fromStdString(t.toISOString());
        QString varStr = QString::fromStdString(MapHourlyMeteoVarToString.at(myVar));
        myProject->logError("interpolateAndSave: interpolation of " + varStr + " at time: " + myTimeStr);
        return false;
    }

    if (isSave)
        return saveMeteoHourlyOutput(myProject, myVar, myOutputPath, myCrit3DTime, myArea);
    else
        return true;
}

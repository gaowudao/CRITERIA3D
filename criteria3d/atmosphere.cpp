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
#include "commonConstants.h"
#include "utilities.h"
#include "quality.h"
#include "transmissivity.h"

//cout
#include <iostream>


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

    // quality control
    if (myProject->qualityParameters.checkData(myVar, hourly, myProject->meteoPoints, myProject->nrMeteoPoints, myCrit3DTime))
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

    Crit3DTime t = myCrit3DTime;
    myMap->timeString = t.toStdString();

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
        if(computeTransmissivity(myProject->meteoPoints, myProject->nrMeteoPoints, intervalWidth, myCrit3DTime, myProject->dtm) > 0)
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
        myProject->projectError = "Function computeRadiationProjectDtm: transmissivity data unavailable";
        return false;
    }

    if (! myProject->qualityParameters.checkData(atmTransmissivity, hourly, myProject->meteoPoints, myProject->nrMeteoPoints, myCrit3DTime))
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
    else if (myVar == actualEvaporation)
        return actualEvaporation;
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
        Crit3DTime t = myCrit3DTime;
        QString myTimeStr = QString::fromStdString(t.toStdString());
        myProject->logError("interpolateAndSave: interpolation of " + getVarNameFromMeteoVariable(myVar) + " at time: " + myTimeStr);
        return false;
    }

    if (isSave)
        return saveMeteoHourlyOutput(myProject, myVar, myOutputPath, myCrit3DTime, myArea);
    else
        return true;
}

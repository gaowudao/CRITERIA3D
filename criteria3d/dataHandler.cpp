#include <string>
#include "meteo.h"
#include "dataHandler.h"
#include "commonConstants.h"
#include"utilities.h"

QString getVarNameFromMeteoVariable(meteoVariable myVar)
{
    if (myVar == airTemperature)
        return "tair";
    else if (myVar == dailyAirTemperatureMin)
        return "tmin";
    else if (myVar == dailyAirTemperatureMax)
        return "tmax";
    else if (myVar == dailyAirTemperatureAvg)
        return "tavg";
    else if (myVar == precipitation)
        return "prec";
    else if (myVar == dailyPrecipitation)
        return "prec";
    else if (myVar == airHumidity)
        return "rhair";
    else if (myVar == dailyAirHumidityMin)
        return "rhmin";
    else if (myVar == dailyAirHumidityMax)
        return "rhmax";
    else if (myVar == dailyAirHumidityAvg)
        return "rhavg";
    else if (myVar == globalIrradiance)
        return "globrad";
    else if (myVar == directIrradiance)
        return "dirrad";
    else if (myVar == diffuseIrradiance)
        return "diffrad";
    else if (myVar == reflectedIrradiance)
        return "reflrad";
    else if (myVar == dailyGlobalRadiation)
        return "globrad";
    else if (myVar == dailyDirectRadiation)
        return "dirrad";
    else if (myVar == dailyDiffuseRadiation)
        return "diffrad";
    else if (myVar == dailyReflectedRadiation)
        return "reflrad";
    else if (myVar == windIntensity)
        return "wind";
    else if (myVar == dailyWindIntensityAvg)
        return "windavg";
    else if (myVar == leafWetness)
        return "leafWetness";
    else if (myVar == referenceEvapotranspiration)
        return "potentialET";
    else if (myVar == actualEvaporation)
        return "evaporation";
    else
        return "";
}


meteoVariable getMeteoVariableFromVarName(QString myVar)
{
    if (myVar == "tair")
        return airTemperature;
    if (myVar == "tmin")
        return dailyAirTemperatureMin;
    if (myVar == "tmax")
        return dailyAirTemperatureMax;
    else if (myVar == "prec")
        return precipitation;
    else if (myVar == "rhair")
        return airHumidity;
    else if (myVar == "rad")
        return globalIrradiance;
    else if (myVar == "dirrad")
        return directIrradiance;
    else if (myVar == "diffrad")
        return diffuseIrradiance;
    else if (myVar == "reflrad")
        return reflectedIrradiance;
    else if (myVar == "wint")
        return windIntensity;
    else if (myVar == "leafWetness")
        return leafWetness;
    else if (myVar == "potentialET")
        return referenceEvapotranspiration;
    else if (myVar == "evaporation")
        return actualEvaporation;
    else
        return noMeteoVar;
}

QString getVarNameFromPlantVariable(plantVariable myVar)
{
    if (myVar == tartaricAcidVar)
        return "tartaricAcid";
    else if (myVar == wineYieldVar)
        return "wineYield";
    else if (myVar == pHBerryVar)
        return "pHBerry";
    else if (myVar == brixBerryVar)
        return "brixBerry";
    else if (myVar == brixMaximumVar)
        return "brixMaximum";
    else if (myVar == deltaBrixVar)
        return "deltabrix";
    else if (myVar == daysAfterBloomVar)
        return "daysAfterBloom";
    else if (myVar == cumulatedBiomassVar)
        return "totalBiomass";
    else if (myVar == daysFromFloweringVar)
        return "daysFromFlowering";
    else if (myVar == isHarvestedVar)
        return "isHarvested";
    else if (myVar == fruitBiomassVar)
        return "fruitBiomass";
    else if (myVar == shootLeafNumberVar)
        return "shootLeafNumber";
    else if (myVar == meanTemperatureLastMonthVar)
        return "meanTLastMonth";
    else if (myVar == chillingUnitsVar)
        return "chillingUnits";
    else if (myVar == forceStateBudBurstVar)
        return "forceStBudBurst";
    else if (myVar == forceStateVegetativeSeasonVar)
        return "forceStVegSeason";
    else if (myVar == stageVar)
        return "phenoPhase";
    else if (myVar == cumRadFruitsetVerVar)
        return "cumRadFSVeraison";
    else if (myVar == leafAreaIndexVar)
        return "leafAreaIndex";
    else if (myVar == transpirationStressVar)
        return "vineStress";
    else if (myVar == transpirationVineyardVar)
        return "transpirationVine";
    else if (myVar == transpirationGrassVar)
        return "transpirationGrass";
    else if (myVar == degreeDaysFromFirstMarchVar)
        return "degreeDaysFromFirstMarch";
    else if (myVar == degreeDaysAtFruitSetVar)
        return "degreeDaysAtFruitSet";
    else if (myVar == fruitBiomassIndexVar)
        return "fruitBiomassIndex";
    else
        return "";
}


QString getOutputNameDaily(QString producer, QString varName, QString strArea, QDate myDate)
{
    if (notes != "")
        return producer + "_" + varName + "_" + strArea + "_" + notes + "_" + myDate.toString("yyyyMMdd");
    else
        return producer + "_" + varName + "_" + strArea + "_" + myDate.toString("yyyyMMdd");
}

QString getOutputNameHourly(meteoVariable myVar, Crit3DTime myTime, QString myArea)
{
    QDateTime myQDateTime = getQDateTime(myTime);
    QString varName = getVarNameFromMeteoVariable(myVar);
    return "ARPA_" + varName + "_" + myArea + "_" + myQDateTime.toString("yyyyMMddThhmm");
}

QString getOutputNameHourly(meteoVariable myVar, QDateTime myTime, QString myArea)
{
    QString varName = getVarNameFromMeteoVariable(myVar);
    return "ARPA_" + varName + "_" + myArea + "_" + myTime.toString("yyyyMMddThhmm");
}


bool readHourlyMap(meteoVariable myVar, QString hourlyPath, QDateTime myTime, QString myArea, gis::Crit3DRasterGrid* myGrid)
{
    QString fileName = hourlyPath + getOutputNameHourly(myVar, myTime, myArea);
    std::string error;

    if (gis::readEsriGrid(fileName.toStdString(), myGrid, &error))
        return true;
    else
        return false;
}


float readDataHourly(meteoVariable myVar, QString hourlyPath, QDateTime myTime, QString myArea, int row, int col)
{
    gis::Crit3DRasterGrid* myGrid = new gis::Crit3DRasterGrid();
    QString fileName = hourlyPath + getOutputNameHourly(myVar, myTime, myArea);
    std::string error;

    if (gis::readEsriGrid(fileName.toStdString(), myGrid, &error))
        if (myGrid->value[row][col] != myGrid->header->flag)
            return myGrid->value[row][col];

    return NODATA;
}


meteoVariable getMeteoVariable(int myVar)
{
    if (myVar == 100) return(airTemperature);
    else if (myVar == 101) return(precipitation);
    else if (myVar == 102) return(airHumidity);
    else if (myVar == 103) return(windIntensity);
    else if (myVar == 104) return(windDirection);
    else if (myVar == 105) return(globalIrradiance);
    else if (myVar == 106) return(leafWetness);
    else if (myVar == 107) return(atmPressure);

    else
        return(noMeteoVar);
}

int getMeteoVarIndex(meteoVariable myVar)
{
    if (myVar == airTemperature)
        return 100;
    else if (myVar == precipitation)
        return 101;
    else if (myVar == airHumidity)
        return 102;
    else if (myVar == windIntensity)
        return 103;
    else if (myVar == windDirection)
        return 104;
    else if (myVar == globalIrradiance)
        return 105;
    else if (myVar == leafWetness)
        return 106;
    else if (myVar == atmPressure)
        return 107;
    else
        return NODATA;
}

float getTimeStepFromHourlyInterval(int myHourlyIntervals)
{    return 3600. / ((float)myHourlyIntervals);}



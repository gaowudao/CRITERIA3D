#include <string>
#include "meteo.h"
#include "dataHandler.h"
#include "commonConstants.h"

QString getVarNameFromMeteoVariable(meteoVariable myVar)
{
    if (myVar == airTemperature)
        return "tair";
    else if (myVar == airTemperatureMin)
        return "tmin";
    else if (myVar == airTemperatureMax)
        return "tmax";
    else if (myVar == airTemperatureMean)
        return "tavg";
    else if (myVar == precipitation)
        return "prec";
    else if (myVar == precipitationSum)
        return "prec";
    else if (myVar == airHumidity)
        return "rhair";
    else if (myVar == airHumidityMin)
        return "rhmin";
    else if (myVar == airHumidityMax)
        return "rhmax";
    else if (myVar == airHumidityMean)
        return "rhavg";
    else if (myVar == globalIrradiance)
        return "globrad";
    else if (myVar == directIrradiance)
        return "dirrad";
    else if (myVar == diffuseIrradiance)
        return "diffrad";
    else if (myVar == reflectedIrradiance)
        return "reflrad";
    else if (myVar == dalilyGlobalRadiation)
        return "globrad";
    else if (myVar == directRadiation)
        return "dirrad";
    else if (myVar == diffuseRadiation)
        return "diffrad";
    else if (myVar == reflectedRadiation)
        return "reflrad";
    else if (myVar == windIntensity)
        return "wind";
    else if (myVar == windIntensityMean)
        return "windavg";
    else if (myVar == wetnessDuration)
        return "leafWetness";
    else if (myVar == potentialEvapotranspiration)
        return "potentialET";
    else if (myVar == realEvaporation)
        return "evaporation";
    else
        return "";
}


meteoVariable getMeteoVariableFromVarName(QString myVar)
{
    if (myVar == "tair")
        return airTemperature;
    if (myVar == "tmin")
        return airTemperatureMin;
    if (myVar == "tmax")
        return airTemperatureMax;
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
        return wetnessDuration;
    else if (myVar == "potentialET")
        return potentialEvapotranspiration;
    else if (myVar == "evaporation")
        return realEvaporation;
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


QString getOutputNameDaily(QString producer, QString varName, QString strArea, QString notes, QDate myDate)
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


QString getFileNameFromString(QString myString)
{
    int myPos;
    QString myChar, myFileName;
    bool isSlashFound = false;

    myFileName = "";
    myPos = myString.length();
    while ((!isSlashFound) && (myPos > 0))
    {
        myChar = myString.mid(myPos,1);
        if ((myChar != "\\") && (myChar != "/"))
        {
            myFileName = myChar + myFileName;
            myPos--;
        }
        else isSlashFound = true;
    }

    return myFileName;
}


QString getPathFromString(QString myString)
{
    QString myFileName, myPath;
    myFileName = getFileNameFromString(myString);
    myPath = myString.left(myString.length()-myFileName.length());
    return myPath;
}

Crit3DDate getCrit3DDate(const QDate& myDate)
{
    Crit3DDate date;

    date.day = myDate.day();
    date.month = myDate.month();
    date.year = myDate.year();
    return date;
}

Crit3DTime getCrit3DTime(const QDateTime& myQTime)
{
    Crit3DTime myCrit3DTime;

    myCrit3DTime.date.day = myQTime.date().day();
    myCrit3DTime.date.month = myQTime.date().month();
    myCrit3DTime.date.year = myQTime.date().year();
    myCrit3DTime.time = myQTime.time().hour()*3600 + myQTime.time().minute()*60 + myQTime.time().second();

    return myCrit3DTime;
}

QDate getQDate(Crit3DDate myDate)
{
    QDate date = QDate(myDate.year, myDate.month, myDate.day);
    return date;
}

QDateTime getQDateTime(Crit3DTime myCrit3DTime)
{
    QDateTime myQDateTime;
    QDate myQDate = QDate(myCrit3DTime.date.year, myCrit3DTime.date.month, myCrit3DTime.date.day);
    myQDateTime.setDate(myQDate);
    QTime myQTime = QTime(myCrit3DTime.getHour(), myCrit3DTime.getMinutes(), myCrit3DTime.getSeconds(), 0);
    myQDateTime.setTime(myQTime);
    return myQDateTime;
}

QString getQStringFromCrit3DTime(const Crit3DTime& myCrit3DTime)
{
    return getQDateTime(myCrit3DTime).toString();
}

bool getValue(QVariant myRs, float* myValue)
{
    if (myRs.isNull())
        *myValue = NODATA;
    else
        *myValue = myRs.toFloat();

    return (*myValue != NODATA);
}

bool getValue(QVariant myRs, double* myValue)
{
    if (myRs.isNull())
        *myValue = NODATA;
    else
        *myValue = myRs.toDouble();

    return (*myValue != NODATA);
}


bool getValue(QVariant myRs, int* myValue)
{
    if (myRs.isNull())
        *myValue = NODATA;
    else
        *myValue = myRs.toInt();

    return (*myValue != NODATA);
}

meteoVariable getMeteoVariable(int myVar)
{
    if (myVar == 100) return(airTemperature);
    else if (myVar == 101) return(precipitation);
    else if (myVar == 102) return(airHumidity);
    else if (myVar == 103) return(windIntensity);
    else if (myVar == 104) return(windDirection);
    else if (myVar == 105) return(globalIrradiance);
    else if (myVar == 106) return(wetnessDuration);
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
    else if (myVar == wetnessDuration)
        return 106;
    else if (myVar == atmPressure)
        return 107;
    else
        return NODATA;
}

float getTimeStepFromHourlyInterval(int myHourlyIntervals)
{    return 3600. / ((float)myHourlyIntervals);}



#include "climate.h"
#include "crit3dDate.h"
#include "utilities.h"
#include "quality.h"
#include "statistics.h"

#define THOMTHRESHOLD 24 // mettere nei settings quando ci saranno Environment.ThomThreshold
#define MINPERCENTAGE 80 // mettere nei settings quando ci saranno Environment.minPercentage

Crit3DDate firstDateDailyVar; //temporaneo

bool elaborationPointsCycle(std::string *myError, Crit3DMeteoPointsDbHandler* meteoPointsDbHandler, meteoVariable variable, int firstYear, int lastYear, QDate firstDate, QDate lastDate, int nYears,
    QString elab1, bool param1IsClimate, QString param1ClimateField, float param1, QString elab2, float param2, bool isAnomaly,
    int nYearsMin, int firstYearClimate, int lastYearClimate)
{


    bool isMeteoGrid = 0; // meteoPoint

//    Dim row As Long, col As Long, i As Long
//    Dim result As Single
//    Dim currentParameter1 As Single


    QDate startDate(firstYear, firstDate.month(), firstDate.day());
    QDate endDate(lastYear, lastDate.month(), lastDate.day());

    if (nYears > 0)
    {
        endDate.setDate(lastYear + nYears, endDate.month(), endDate.day());
    }
    else if (nYears < 0)
    {
        startDate.setDate(firstYear + nYears, startDate.month(), startDate.day());
    }

    if (param1IsClimate)
    {
//        Dim myPeriodType As Byte
//        Dim myClimateIndex As Integer

//        myPeriodType = Climate.parserElaborationOnlyPeriodType(parameter1ClimateElab)
//        myClimateIndex = Climate.getClimateIndexFromDate(currentDay, myPeriodType)
     }


//                For i = 0 To UBound(meteoPoint)

//                    If meteoPoint(i).Point.active And meteoPoint(i).Point.selected Then

//                        If param1IsClimate Then
//                            If Climate.ClimateReadPoint(PragaClimate.Point(i).TableName, _
//                                                        param1ClimateField, _
//                                                        myPeriodType, _
//                                                        myClimateIndex, _
//                                                        PragaClimate.Point(i)) Then

//                                currentParameter1 = passaggioDati.GetClimateData(myPeriodType, PragaClimate.Point(i), myClimateIndex)
//                            Else
//                                currentParameter1 = Definitions.NO_DATA
//                            End If
//                        Else
//                            currentParameter1 = param1
//                        End If

//                        If elab1 = Definitions.ELABORATION_PHENO Then currentPheno.setPhenoPoint i

//                        If ElaborationOnPoint(meteoPoint(i).Point, isMeteoGrid, variable, _
//                            elab1, currentParameter1, elab2, Parameter2, _
//                            startDate, endDate, nYears, firstYear, lastYear, _
//                            nYearsMin, _
//                            isAnomaly, True) Then ElaborationPointsCycle = True

//                    End If
//                Next i
    return true;

}




bool elaborationPointsCycleGrid(std::string *myError, Crit3DMeteoGridDbHandler* meteoGridDbHandler, meteoVariable variable, int firstYear, int lastYear, QDate firstDate, QDate lastDate, int nYears,
    QString elab1, bool param1IsClimate, QString param1ClimateField, float param1, QString elab2, float param2, bool isAnomaly,
    int nYearsMin, int firstYearClimate, int lastYearClimate)
{

    bool isMeteoGrid = 1; // grid
    float currentParameter1;

    std::string id;


    QDate startDate(firstYear, firstDate.month(), firstDate.day());
    QDate endDate(lastYear, lastDate.month(), lastDate.day());

    if (nYears > 0)
    {
        endDate.setDate(lastYear + nYears, endDate.month(), endDate.day());
    }
    else if (nYears < 0)
    {
        startDate.setDate(firstYear + nYears, startDate.month(), startDate.day());
    }


    if (param1IsClimate)
    {
//        Dim myPeriodType As Byte
//        Dim myClimateIndex As Integer

//        myPeriodType = Climate.parserElaborationOnlyPeriodType(parameter1ClimateElab)
//        myClimateIndex = Climate.getClimateIndexFromDate(currentDay, myPeriodType)
     }


     for (int row = 0; row < meteoGridDbHandler->gridStructure().header().nrRows; row++)
     {
         for (int col = 0; col < meteoGridDbHandler->gridStructure().header().nrCols; col++)
         {

            if (meteoGridDbHandler->meteoGrid()->getMeteoPointActiveId(row, col, &id))
            {

                if (param1IsClimate)
                {
//                    if (Climate.ClimateReadPoint(PragaClimate.Point(row, col).TableName, _
//                        param1ClimateField, myPeriodType, myClimateIndex,PragaClimate.Point(row, col)) )
//                    {

//                      currentParameter1 = passaggioDati.GetClimateData(myPeriodType, PragaClimate.Point(row, col), myClimateIndex)
//                    }
//                    else
//                    {
//                        currentParameter1 = NODATA;
//                    }
                }
                else
                {
                    currentParameter1 = param1;
                }

//                            if  ( !elaborationOnPoint(&(meteoGridDbHandler->meteoGrid()->meteoPoint(row,col)), isMeteoGrid, variable,
//                                elab1, currentParameter1, elab2, param2, startDate, endDate, nYears, firstYear, lastYear,
//                                nYearsMin, isAnomaly, true))
//                            {
//                                // LC se qualche problema su una cella, deve interrompere e ritornare false oppure no?
                                  // FT credo di no, ma lascio decidere a Gabri
                                  // GA  deve tornare false solo se nemmeno una cella ha dato valido (v. VB)
//                                return false;
//                            }

            }

        }
    }

    return true;

}




bool elaborationOnPoint(std::string *myError, Crit3DMeteoPointsDbHandler* meteoPointsDbHandler, Crit3DMeteoGridDbHandler* meteoGridDbHandler,
    Crit3DMeteoPoint* meteoPoint, bool isMeteoGrid, meteoVariable variable, QString elab1, float param1,
    QString elab2, float param2, QDate startDate, QDate endDate, int nYears, int firstYear, int lastYear,
    int nYearsMin, bool isAnomaly, bool loadData)
{


    float percValue;
    bool dataLoaded;
    float result;

    Crit3DMeteoPoint* meteoPointTemp = new Crit3DMeteoPoint;
    meteoPointTemp->id = meteoPoint->id;

    if (!isAnomaly || meteoPoint->anomaly != NODATA)
    {

            dataLoaded = false;

            if (loadData)
            {
                dataLoaded = preElaboration(myError, meteoPointsDbHandler, meteoGridDbHandler, meteoPointTemp, isMeteoGrid, variable, elab1, startDate, endDate, &percValue);
            }
            else
            {
                dataLoaded = true;
            }

            if (dataLoaded)
            {
                // check
                Crit3DDate startD(startDate.day(), startDate.month(), firstYear);
                Crit3DDate endD(endDate.day(), endDate.month(), firstYear);

                if (nYears < 0)
                {
                    startD.year = firstYear + nYears;
                }
                else if (nYears > 0)
                {
                    endD.year = firstYear + nYears;
                }

                if (difference (startD, endD) < 0)
                {
                    *myError = "Wrong dates!";
                    return false;
                }
                if (elab1 == "")
                {
                    *myError = "Missing elaboration";
                    return false;
                }
//                result = computeStatistic(dailyValues, firstYear, lastYear, startD, endD, nYears, firstDateDailyVar, elab1.toStdString(), param1, elab2.toStdString(), param2, myHeight);

                if (isAnomaly)
                {
                    return anomalyOnPoint(meteoPoint, result);
                }
                else
                {
                      meteoPoint->elaboration = result;
                      if (meteoPoint->elaboration != NODATA)
                      {
                          return true;
                      }
                      else
                      {
                          return false;
                      }
                }
            }
            else if (isAnomaly)
            {
                meteoPoint->anomaly = NODATA;
                meteoPoint->anomalyPercentage = NODATA;
            }

    }

    return false;

}

bool anomalyOnPoint(Crit3DMeteoPoint* meteoPoint, float refValue)
{

    bool anomalyOnPoint = (refValue != NODATA && meteoPoint->elaboration != NODATA);

    if (anomalyOnPoint)
    {
        meteoPoint->anomaly = meteoPoint->elaboration - refValue;
        if (refValue != 0)
        {
            meteoPoint->anomalyPercentage = (meteoPoint->elaboration - refValue) / abs(refValue) * 100;
        }
        else
        {
            meteoPoint->anomalyPercentage = NODATA;
        }
    }
    else
    {
        meteoPoint->anomaly = NODATA;
        meteoPoint->anomalyPercentage = NODATA;
    }
    return anomalyOnPoint;

}


bool loadDailyVarSeries(std::string *myError, Crit3DMeteoPointsDbHandler *meteoPointsDbHandler,
        Crit3DMeteoGridDbHandler *meteoGridDbHandler, Crit3DMeteoPoint* meteoPoint, bool isMeteoGrid,
        meteoVariable variable, QDate first, QDate last)
{
    std::vector<float> dailyValues;
    QDate firstDateDB;

    // meteoGrid
    if (isMeteoGrid)
    {
        if (meteoGridDbHandler->gridStructure().isFixedFields())
        {
            dailyValues = meteoGridDbHandler->loadGridDailyVarFixedFields(myError, QString::fromStdString(meteoPoint->id), variable, first, last, &firstDateDB);
        }
        else
        {
            dailyValues = meteoGridDbHandler->loadGridDailyVar(myError, QString::fromStdString(meteoPoint->id), variable, first, last, &firstDateDB);
        }

        int numberOfDays = firstDateDB.daysTo(last) + 1;
        meteoPoint->initializeObsDataD(numberOfDays, getCrit3DDate(firstDateDB));
    }
    // meteoPoint
    else
    {
        dailyValues = meteoPointsDbHandler->getDailyVar(myError, variable, getCrit3DDate(first), getCrit3DDate(last), &firstDateDB, meteoPoint );
    }

    firstDateDailyVar = getCrit3DDate(firstDateDB);

    if ( dailyValues.empty() )
    {
        return false;
    }
    else
    {
        if (meteoPoint->nrObsDataDaysD == 0)
        {
            meteoPoint->initializeObsDataD(dailyValues.size(), firstDateDailyVar);
        }

        for (unsigned int i = 0; i < dailyValues.size(); i++)
        {
            meteoPoint->setMeteoPointValueD(Crit3DDate(firstDateDB.day(), firstDateDB.month(), firstDateDB.year()), variable, dailyValues[i]);
            firstDateDB = firstDateDB.addDays(1);
        }
        return true;
    }


}

bool loadHourlyVarSeries(std::string *myError, Crit3DMeteoPointsDbHandler* meteoPointsDbHandler,
           Crit3DMeteoGridDbHandler* meteoGridDbHandler, Crit3DMeteoPoint* meteoPoint, bool isMeteoGrid,
           meteoVariable variable, QDateTime first, QDateTime last)
{
    std::vector<float> hourlyValues;
    QDateTime firstDateDB;

    // meteoGrid
    if (isMeteoGrid)
    {
        if (meteoGridDbHandler->gridStructure().isFixedFields())
        {
            hourlyValues = meteoGridDbHandler->loadGridHourlyVarFixedFields(myError, QString::fromStdString(meteoPoint->id), variable, first, last, &firstDateDB);
        }
        else
        {
            hourlyValues = meteoGridDbHandler->loadGridHourlyVar(myError, QString::fromStdString(meteoPoint->id), variable, first, last, &firstDateDB);
        }

        int numberOfDays = firstDateDB.date().daysTo(last.date());
        meteoPoint->initializeObsDataH(meteoPoint->hourlyFraction, numberOfDays, getCrit3DDate(firstDateDB.date()));

    }
    // meteoPoint
    else
    {
        hourlyValues = meteoPointsDbHandler->getHourlyVar(myError, variable, getCrit3DDate(first.date()), getCrit3DDate(last.date()), &firstDateDB, meteoPoint );
    }

    firstDateDailyVar = getCrit3DDate(firstDateDB.date());

    if ( hourlyValues.empty() )
    {
        return false;
    }
    else
    {
        if (meteoPoint->nrObsDataDaysH == 0)
        {
            meteoPoint->initializeObsDataH(meteoPoint->hourlyFraction, hourlyValues.size(), firstDateDailyVar);
        }

        for (unsigned int i = 0; i < hourlyValues.size(); i++)
        {
            meteoPoint->setMeteoPointValueH(Crit3DDate(firstDateDB.date().day(), firstDateDB.date().month(), firstDateDB.date().year()), firstDateDB.time().hour(), firstDateDB.time().minute(), variable, hourlyValues[i]);
            firstDateDB = firstDateDB.addSecs(3600);
        }
        return true;
    }

}


// compute daily Thom using Tmax e RHmin
float thomDayTime(float tempMax, float relHumMinAir)
{

    Crit3DQuality qualityCheck;
    quality::type qualityT = qualityCheck.syntacticQualityControlSingleVal(dailyAirTemperatureMax, tempMax);
    quality::type qualityRelHumMinAir = qualityCheck.syntacticQualityControlSingleVal(dailyAirRelHumidityMin, relHumMinAir);


    // TODO nella versione vb ammessi anche i qualitySuspectData, questo tipo per ora non è stato implementato
    if ( qualityT == quality::accepted && qualityRelHumMinAir == quality::accepted )
    {
            return computeThomIndex(tempMax, relHumMinAir);
    }
    else
        return NODATA;

}

// compute daily Thom using Tmin e RHmax
float thomNightTime(float tempMin, float relHumMaxAir)
{

    Crit3DQuality qualityCheck;
    quality::type qualityT = qualityCheck.syntacticQualityControlSingleVal(dailyAirTemperatureMin, tempMin);
    quality::type qualityRelHumMaxAir = qualityCheck.syntacticQualityControlSingleVal(dailyAirRelHumidityMax, relHumMaxAir);

    // TODO nella versione vb ammessi anche i qualitySuspectData, questo tipo per ora non è stato implementato
    if ( qualityT == quality::accepted && qualityRelHumMaxAir == quality::accepted )
    {
            return computeThomIndex(tempMin, relHumMaxAir);
    }
    else
        return NODATA;

}

// compuote hourly thom
float thomH(float tempAvg, float relHumAvgAir)
{

    Crit3DQuality qualityCheck;
    quality::type qualityT = qualityCheck.syntacticQualityControlSingleVal(dailyAirTemperatureAvg, tempAvg);
    quality::type qualityRelHumAvgAir = qualityCheck.syntacticQualityControlSingleVal(dailyAirRelHumidityAvg, relHumAvgAir);

    // TODO nella versione vb ammessi anche i qualitySuspectData, questo tipo per ora non è stato implementato
    if ( qualityT == quality::accepted && qualityRelHumAvgAir == quality::accepted )
    {
            return computeThomIndex(tempAvg, relHumAvgAir);
    }
    else
        return NODATA;
}


// compute # hours thom >  threshold per day
int thomDailyNHoursAbove(float* tempAvg, float* relHumAvgAir)
{

    int nData = 0;
    int thomDailyNHoursAbove = NODATA;
    for (int hour = 0; hour < 24; hour++)
    {
        float thom = thomH(tempAvg[hour], relHumAvgAir[hour]);
        if (thom != NODATA)
        {
            nData = nData + 1;
            if (thomDailyNHoursAbove == NODATA)
                thomDailyNHoursAbove = 0;
            if (thom > THOMTHRESHOLD)
                thomDailyNHoursAbove = thomDailyNHoursAbove + 1;
        }
    }
    if ( (nData / 24 * 100) < MINPERCENTAGE)
        thomDailyNHoursAbove = NODATA;

    return thomDailyNHoursAbove;


}

// compute daily max thom value
float thomDailyMax(float *tempAvg, float* relHumAvgAir)
{
    int nData = 0;
    int thomDailyMax = NODATA;
    for (int hour = 0; hour < 24; hour++)
    {
        float thom = thomH(tempAvg[hour], relHumAvgAir[hour]);
        if (thom != NODATA)
        {
            nData = nData + 1;
            if (thom > thomDailyMax)
                thomDailyMax = thom;
        }
    }
    if ( (nData / 24 * 100) < MINPERCENTAGE)
        thomDailyMax = NODATA;

    return thomDailyMax;
}

// compute daily avg thom value
float thomDailyMean(float *tempAvg, float* relHumAvgAir)
{

    int nData = 0;
    std::vector<float> thomValues;
    float thomDailyMean;

    for (int hour = 0; hour < 24; hour++)
    {
        float thom = thomH(tempAvg[hour], relHumAvgAir[hour]);
        if (thom != NODATA)
        {
            thomValues.push_back(thom);
            nData = nData + 1;
        }
    }
    if ( (nData / 24 * 100) < MINPERCENTAGE)
        thomDailyMean = NODATA;
    else
        thomDailyMean = statistics::mean(thomValues, nData);


    return thomDailyMean;

}

float dailyLeafWetnessComputation(int *leafW)
{

    int nData = 0;
    float dailyLeafWetnessRes = 0;

    for (int hour = 0; hour < 24; hour++)
    {
        if (leafW[hour] == 0 || leafW[hour] == 1)
        {
                dailyLeafWetnessRes = dailyLeafWetnessRes + leafW[hour];
                nData = nData + 1;
        }
    }
    if ( (nData / 24 * 100) < MINPERCENTAGE)
        dailyLeafWetnessRes = NODATA;

    return dailyLeafWetnessRes;

}

float computeDailyBIC(float prec, float etp)
{

    Crit3DQuality qualityCheck;

    // TODO nella versione vb ammessi anche i qualitySuspectData, questo tipo per ora non è stato implementato
    quality::type qualityPrec = qualityCheck.syntacticQualityControlSingleVal(dailyPrecipitation, prec);
    quality::type qualityETP = qualityCheck.syntacticQualityControlSingleVal(dailyReferenceEvapotranspiration, etp);
    if (qualityPrec == quality::accepted && qualityETP == quality::accepted)
    {
            return (prec - etp);
    }
    else
        return NODATA;

}

float dailyThermalRange(float Tmin, float Tmax)
{

    Crit3DQuality qualityCheck;

    // TODO nella versione vb ammessi anche i qualitySuspectData, questo tipo per ora non è stato implementato
    quality::type qualityTmin = qualityCheck.syntacticQualityControlSingleVal(dailyAirTemperatureMin, Tmin);
    quality::type qualityTmax = qualityCheck.syntacticQualityControlSingleVal(dailyAirTemperatureMax, Tmax);
    if (qualityTmin  == quality::accepted && qualityTmax == quality::accepted)
        return (Tmax - Tmin);
    else
        return NODATA;

}

float dailyAverageT(float Tmin, float Tmax)
{

        Crit3DQuality qualityCheck;

        // TODO nella versione vb ammessi anche i qualitySuspectData, questo tipo per ora non è stato implementato
        quality::type qualityTmin = qualityCheck.syntacticQualityControlSingleVal(dailyAirTemperatureMin, Tmin);
        quality::type qualityTmax = qualityCheck.syntacticQualityControlSingleVal(dailyAirTemperatureMax, Tmax);
        if (qualityTmin  == quality::accepted && qualityTmax == quality::accepted)
            return ( (Tmin + Tmax) / 2) ;
        else
            return NODATA;

}

float dailyEtpHargreaves(float Tmin, float Tmax, Crit3DDate date, double latitude)
{

    Crit3DQuality qualityCheck;

    // TODO nella versione vb ammessi anche i qualitySuspectData, questo tipo per ora non è stato implementato
    quality::type qualityTmin = qualityCheck.syntacticQualityControlSingleVal(dailyAirTemperatureMin, Tmin);
    quality::type qualityTmax = qualityCheck.syntacticQualityControlSingleVal(dailyAirTemperatureMax, Tmax);
    int dayOfYear = getDoyFromDate(date);
    if (qualityTmin  == quality::accepted && qualityTmax == quality::accepted)
        return ET0_Hargreaves(TRANSMISSIVITY_SAMANI_COEFF_DEFAULT, latitude, dayOfYear, Tmax, Tmin);
    else
        return NODATA;

}

float dewPoint(float relHumAir, float tempAir)
{

    if (relHumAir == NODATA || relHumAir == 0 || tempAir == NODATA)
        return NODATA;

    relHumAir = minValue(100, relHumAir);

    float saturatedVaporPres = exp((16.78 * tempAir - 116.9) / (tempAir + 237.3));
    float actualVaporPres = relHumAir / 100 * saturatedVaporPres;
    return (log(actualVaporPres) * 237.3 + 116.9) / (16.78 - log(actualVaporPres));

}

void extractValidValuesWithThreshold(std::vector<float> myValues, std::vector<float>* myValidValues, float myThreshold)
{

    for (unsigned int i = 0; i < myValues.size(); i++)
    {
        if (myValues[i] != NODATA)
        {
            if (myValues[i] > myThreshold)
            {
                myValidValues->push_back(myValues[i]);
            }
        }
    }

}

void extractValidValuesCC(std::vector<float> myValues, std::vector<float>* myValidValues)
{

    for (unsigned int i = 0; i < myValues.size(); i++)
    {
        if (myValues[i] != NODATA)
        {
            myValidValues->push_back(myValues[i]);
        }
    }

}

bool elaborateDailyAggregatedVar(meteoVariable myVar, Crit3DMeteoPoint meteoPoint, std::vector<float>* aggregatedValues, float* percValue)
{

    frequencyType aggregationFrequency = getAggregationFrequency(myVar);

    if (aggregationFrequency == hourly)
    {
            return elaborateDailyAggregatedVarFromHourly(myVar, meteoPoint, aggregatedValues);
    }
    else if (aggregationFrequency == daily)
    {
            return elaborateDailyAggregatedVarFromDaily(myVar, meteoPoint, aggregatedValues, percValue);
    }
    else
        return false;

}


frequencyType getAggregationFrequency(meteoVariable myVar)
{

    //if (elab == ELABORATION_THOM_DAILYHOURSABOVE || elab == ELABORATION_THOM_DAILYMEAN || elab == ELABORATION_THOM_DAILYMAX || elab == DAILY_LEAFWETNESS)
    if (myVar == dailyThomHoursAbove || myVar == dailyThomAvg || myVar == dailyThomMax || myVar == dailyLeafWetness)
    {
        return hourly;
    }
    else if (myVar != NODATA)
    {
        return daily;
    }
    else
    {
        return noFrequency;
    }

}

bool elaborateDailyAggregatedVarFromDaily(meteoVariable myVar, Crit3DMeteoPoint meteoPoint, std::vector<float>* aggregatedValues, float* percValue)
{

    float res;
    int nrValidi = 0;
    Crit3DDate date = meteoPoint.obsDataD[0].date;
    Crit3DQuality qualityCheck;

    for (unsigned int index = 0; index < meteoPoint.nrObsDataDaysD; index++)
    {
        switch(myVar)
        {
        case dailyThomDaytime:
                res = thomDayTime(meteoPoint.obsDataD[index].tMax, meteoPoint.obsDataD[index].rhMin);
            break;
        case dailyThomNighttime:
                res = thomNightTime(meteoPoint.obsDataD[index].tMin, meteoPoint.obsDataD[index].rhMax);
                break;
        case dailyBIC:
                res = computeDailyBIC(meteoPoint.obsDataD[index].prec, meteoPoint.obsDataD[index].et0);
                break;
        case dailyAirTemperatureRange:
                res = dailyThermalRange(meteoPoint.obsDataD[index].tMin, meteoPoint.obsDataD[index].tMax);
                break;
        case dailyAirTemperatureAvg:
                {
                    quality::type qualityTavg = qualityCheck.syntacticQualityControlSingleVal(dailyAirTemperatureAvg, meteoPoint.obsDataD[index].tAvg);
                    if (qualityTavg == quality::accepted)
                    {
                        res = meteoPoint.obsDataD[index].tAvg;
                    }
                    else
                    {
                        res = dailyAverageT(meteoPoint.obsDataD[index].tMin, meteoPoint.obsDataD[index].tMax);
                    }
                    break;
                }
        case dailyReferenceEvapotranspiration:
        {
            quality::type qualityEtp = qualityCheck.syntacticQualityControlSingleVal(dailyReferenceEvapotranspiration, meteoPoint.obsDataD[index].et0);
            if (qualityEtp == quality::accepted)
            {
                res = meteoPoint.obsDataD[index].et0;
            }
            else
            {
                res = dailyEtpHargreaves(meteoPoint.obsDataD[index].tMin, meteoPoint.obsDataD[index].tMax, date, meteoPoint.latitude);
            }
            break;
        }
        case dailyAirDewTemperatureAvg:
                res = dewPoint(meteoPoint.obsDataD[index].rhAvg, meteoPoint.obsDataD[index].tAvg); // RHavg, Tavg
                break;
        case dailyAirDewTemperatureMin:
                res = dewPoint(meteoPoint.obsDataD[index].rhMax, meteoPoint.obsDataD[index].tMin); // RHmax, Tmin
                break;
        case dailyAirDewTemperatureMax:
                res = dewPoint(meteoPoint.obsDataD[index].rhMin, meteoPoint.obsDataD[index].tMax); // RHmin, Tmax
                break;
        default:
                res = NODATA;
                break;
        }

        if (res != NODATA)
        {
            nrValidi = nrValidi + 1;
            firstDateDailyVar = date;
        }
        aggregatedValues->push_back(res);
        date = date.addDays(1);

    }

    *percValue = nrValidi/aggregatedValues->size();
    if (nrValidi > 0)
        return true;
    else
        return false;

}

bool elaborateDailyAggregatedVarFromHourly(meteoVariable myVar, Crit3DMeteoPoint meteoPoint, std::vector<float>* aggregatedValues)
{

    float res;
    int nrValidi = 0;
    Crit3DDate date = meteoPoint.obsDataH[0].date;

    for (unsigned int index = 0; index < meteoPoint.nrObsDataDaysH; index++)
    {

        switch(myVar)
        {
            case dailyThomHoursAbove:
                res = thomDailyNHoursAbove(meteoPoint.obsDataH[index].tAir, meteoPoint.obsDataH[index].rhAir);
                break;
            case dailyThomMax:
                res = thomDailyMax(meteoPoint.obsDataH[index].tAir, meteoPoint.obsDataH[index].rhAir);
                break;
            case dailyThomAvg:
                res = thomDailyMean(meteoPoint.obsDataH[index].tAir, meteoPoint.obsDataH[index].rhAir);
                break;
            case dailyLeafWetness:
                res = dailyLeafWetnessComputation(meteoPoint.obsDataH[index].leafW);
                break;
            default:
                res = NODATA;
                break;
        }

        if (res != NODATA)
        {
            nrValidi = nrValidi + 1;
            firstDateDailyVar = date;

        }
        aggregatedValues->push_back(res);
        date = date.addDays(1);
    }
    if (nrValidi > 0)
        return true;
    else
        return false;

}

bool preElaboration(std::string *myError, Crit3DMeteoPointsDbHandler* meteoPointsDbHandler, Crit3DMeteoGridDbHandler* meteoGridDbHandler, Crit3DMeteoPoint* meteoPoint, bool isMeteoGrid, meteoVariable variable, QString elab1,
    QDate startDate, QDate endDate, float* percValue)
{

    /*
    //// era in statistica
    todo: spostare in preElaboration (quindi eliminare myVar)
    If myElab <> Definitions.ELAB_GIORNI_CONSECUTIVI_SOPRA_SOGLIA And _
        myElab <> Definitions.ELAB_GIORNI_CONSECUTIVI_SOTTO_SOGLIA And _
        myElab <> Definitions.ELAB_TREND Then

        If myVar = Definitions.DAILY_PREC And myElab = Definitions.ELAB_PERCENTILE And Not isCumulated Then
            Elaboration.ExtractValidValuesWithThreshold myValues, myValidValues, Environment.RainfallThreshold
        Else
            Elaboration.ExtractValidValuesCC myValues, myValidValues
        End If
    End If
    //////////
    */

    std::vector<float> aggregatedValues;
    bool preElaboration = false;

     /*
    switch(variable)
    {

        case dailyLeafWetness:
        {
            if ( loadHourlyVarSeries(myError, meteoPointsDbHandler, meteoGridDbHandler, meteoPoint, isMeteoGrid, leafWetness, QDateTime(startDate,QTime(0,0,0)), QDateTime(endDate,QTime(0,0,0))) )
            {
                preElaboration = elaborateDailyAggregatedVar(dailyLeafWetness, *meteoPoint, &aggregatedValues, percValue);
            }
        }

        case dailyThomDaytime:
        {
            if ( loadDailyVarSeries(myError, meteoPointsDbHandler, meteoGridDbHandler, meteoPoint, isMeteoGrid, dailyAirRelHumidityMin, startDate, endDate))
            {
                    preElaboration = true;
            }
            if (preElaboration)
            {
                preElaboration = false;
                if (loadDailyVarSeries(myError, meteoPointsDbHandler, meteoGridDbHandler, meteoPoint, isMeteoGrid, dailyAirTemperatureMax, startDate, endDate))
                {
                        preElaboration = true;
                }
                if (preElaboration)
                {
                    preElaboration = elaborateDailyAggregatedVar(dailyThomDaytime, *meteoPoint, &aggregatedValues, percValue);
                }
            }
        }

        case dailyThomNighttime:
        {
            if (loadDailyVarSeries(myError, meteoPointsDbHandler, meteoGridDbHandler, meteoPoint, isMeteoGrid, dailyAirRelHumidityMax, startDate, endDate) )
            {
                preElaboration = true;
            }
            if (preElaboration)
            {
                preElaboration = false;
                if (loadDailyVarSeries(myError, meteoPointsDbHandler, meteoGridDbHandler, meteoPoint, isMeteoGrid, dailyAirTemperatureMin, startDate, endDate))
                {
                    preElaboration = true;
                }
                if (preElaboration)
                {
                    preElaboration = elaborateDailyAggregatedVar(dailyThomNighttime, *meteoPoint, &aggregatedValues, percValue);
                }
            }
        }
        case dailyThomAvg: case dailyThomMax: case dailyThomHoursAbove:
        {

            if (loadHourlyVarSeries(myError, meteoPointsDbHandler, meteoGridDbHandler, meteoPoint, isMeteoGrid, airTemperature, QDateTime(startDate,QTime(0,0,0)), QDateTime(endDate,QTime(0,0,0))) )
            {
                if (loadHourlyVarSeries(myError, meteoPointsDbHandler, meteoGridDbHandler, meteoPoint, isMeteoGrid, airRelHumidity, QDateTime(startDate,QTime(0,0,0)), QDateTime(endDate,QTime(0,0,0))) )
                {
                    preElaboration = elaborateDailyAggregatedVar(variable, *meteoPoint, &aggregatedValues, percValue);
                }
            }
        }
        case dailyBIC:
        {
            if (loadDailyVarSeries(myError, meteoPointsDbHandler, meteoGridDbHandler, meteoPoint, isMeteoGrid, dailyReferenceEvapotranspiration, startDate, endDate))
            {
                preElaboration = true;
            }
            else if (Environment.AutomaticETP)
            {
                if (loadDailyVarSeries(myError, meteoPointsDbHandler, meteoGridDbHandler, meteoPoint, isMeteoGrid, dailyAirTemperatureMin, startDate, endDate))
                {
                    preElaboration = true;
                }
                if (preElaboration)
                {
                    preElaboration = false;
                    if ( loadDailyVarSeries(myError, meteoPointsDbHandler, meteoGridDbHandler, meteoPoint, isMeteoGrid, dailyAirTemperatureMax, startDate, endDate))
                    {
                        preElaboration = true;
                    }
                    if (preElaboration)
                    {
                        preElaboration = elaborateDailyAggregatedVar(dailyReferenceEvapotranspiration, *meteoPoint, &aggregatedValues, percValue);
                    }
                }
            }
            if (preElaboration)
            {
                preElaboration = false;
                if ( loadDailyVarSeries(myError, meteoPointsDbHandler, meteoGridDbHandler, meteoPoint, isMeteoGrid, dailyPrecipitation, startDate, endDate) )
                {
                    preElaboration = true;
                }
                if (preElaboration)
                {
                    preElaboration = elaborateDailyAggregatedVar(dailyBIC, *meteoPoint, &aggregatedValues, percValue);
                }
            }
        }

        case dailyAirTemperatureRange:
        {
            if (loadDailyVarSeries(myError, meteoPointsDbHandler, meteoGridDbHandler, meteoPoint, isMeteoGrid, dailyAirTemperatureMin, startDate, endDate))
            {
                preElaboration = true;
            }
            if (preElaboration)
            {
                preElaboration = false;
                if (loadDailyVarSeries(myError, meteoPointsDbHandler, meteoGridDbHandler, meteoPoint, isMeteoGrid, dailyAirTemperatureMax, startDate, endDate) )
                {
                    preElaboration = true;
                }
                if (preElaboration)
                {
                    preElaboration = elaborateDailyAggregatedVar(dailyAirTemperatureRange, *meteoPoint, &aggregatedValues, percValue);
                }
            }
        }
        case dailyAirDewTemperatureMax:
        {
            if ( loadDailyVarSeries(myError, meteoPointsDbHandler, meteoGridDbHandler, meteoPoint, isMeteoGrid, dailyAirTemperatureMax, startDate, endDate) )
            {
                preElaboration = true;
            }
            if (preElaboration)
            {
                preElaboration = false;
                if ( loadDailyVarSeries(myError, meteoPointsDbHandler, meteoGridDbHandler, meteoPoint, isMeteoGrid, dailyAirRelHumidityMin, startDate, endDate) )
                {
                    preElaboration = true;
                }
                if (preElaboration)
                {
                    preElaboration = elaborateDailyAggregatedVar(dailyAirDewTemperatureMax, *meteoPoint, &aggregatedValues, percValue);
                }
            }
        }

        case dailyAirDewTemperatureMin:
        {
            if ( loadDailyVarSeries(myError, meteoPointsDbHandler, meteoGridDbHandler, meteoPoint, isMeteoGrid, dailyAirTemperatureMin, startDate, endDate) )
            {
                preElaboration = true;
            }
            if (preElaboration)
            {
                preElaboration = false;
                if ( loadDailyVarSeries(myError, meteoPointsDbHandler, meteoGridDbHandler, meteoPoint, isMeteoGrid, dailyAirRelHumidityMax, startDate, endDate) )
                {
                    preElaboration = true;
                }
                if (preElaboration)
                {
                    preElaboration = elaborateDailyAggregatedVar(dailyAirDewTemperatureMin, *meteoPoint, &aggregatedValues, percValue);
                }
            }
        }

        case dailyAirTemperatureAvg:
        {
            percValue = loadDailyVarSeries(myError, meteoPointsDbHandler, meteoGridDbHandler, meteoPoint, isMeteoGrid, dailyAirTemperatureAvg, startDate, endDate)
            If percValue > 0 Then
                    preElaboration = True

            ElseIf Environment.AutomaticTmed Then
                If loadDailyVarSeries(isMeteoGrid, dailyAirTemperatureMin, myPoint, startDate, endDate) > 0 Then
                        preElaboration = True
                End If
                If preElaboration Then
                    preElaboration = False
                    passaggioDati.InizializzaMbuto startDate, endDate
                    passaggioDati.MbutoInversoGiornaliero dailyAirTemperatureMin
                    If loadDailyVarSeries(isMeteoGrid, dailyAirTemperatureMax, myPoint, startDate, endDate) > 0 Then
                            preElaboration = True
                    End If
                    If preElaboration Then
                        passaggioDati.MbutoInversoGiornaliero dailyAirTemperatureMax
                        preElaboration = Elaboration.elaborateDailyAggregatedVar(Definitions.DAILY_TAVG, myPoint, percValue)
                    End If
                End If
            End If
        }

        case dailyReferenceEvapotranspiration:
            percValue = loadDailyVarSeries(myError, meteoPointsDbHandler, meteoGridDbHandler, meteoPoint, isMeteoGrid, dailyReferenceEvapotranspiration, startDate, endDate)
            If percValue > 0 The
                    preElaboration = True

            ElseIf Environment.AutomaticETP Then
                If loadDailyVarSeries(isMeteoGrid, dailyAirTemperatureMin, myPoint, startDate, endDate) > 0 Then
                        preElaboration = True
                End If
                If preElaboration Then
                    preElaboration = False
                    passaggioDati.InizializzaMbuto startDate, endDate
                    passaggioDati.MbutoInversoGiornaliero dailyAirTemperatureMin
                    If loadDailyVarSeries(isMeteoGrid, dailyAirTemperatureMax, myPoint, startDate, endDate) > 0 Then
                            preElaboration = True
                    End If
                    If preElaboration Then
                        passaggioDati.MbutoInversoGiornaliero dailyAirTemperatureMax
                        preElaboration = Elaboration.elaborateDailyAggregatedVar(Definitions.DAILY_ETP, myPoint, percValue)
                    End If
                End If
            End If
*/
    /*
        default:
        {

            switch(elab1.toInt())
            {

                case huglin:
                    If loadDailyVarSeries(isMeteoGrid, dailyAirTemperatureMin, myPoint, startDate, endDate) > 0 Then
                            preElaboration = True
                    End If
                    If preElaboration Then
                        preElaboration = False
                        passaggioDati.InizializzaMbuto startDate, endDate
                        passaggioDati.MbutoInversoGiornaliero dailyAirTemperatureMin
                        If loadDailyVarSeries(isMeteoGrid, dailyAirTemperatureMax, myPoint, startDate, endDate) > 0 Then
                                preElaboration = True
                        End If
                        If preElaboration Then
                            preElaboration = False
                            passaggioDati.MbutoInversoGiornaliero dailyAirTemperatureMax
                            If loadDailyVarSeries(isMeteoGrid, Definitions.DAILY_TAVG, myPoint, startDate, endDate) > 0 Then
                                preElaboration = True
                            ElseIf Environment.AutomaticTmed Then
                                preElaboration = Elaboration.elaborateDailyAggregatedVar(Definitions.DAILY_TAVG, myPoint, percValue)
                            End If
                        End If
                    End If

                case winkler: case correctedDegreeDaysSum: case fregoni

                    If loadDailyVarSeries(isMeteoGrid, dailyAirTemperatureMin, myPoint, startDate, endDate) > 0 Then
                            preElaboration = True
                    End If
                    If preElaboration Then
                        preElaboration = False
                        passaggioDati.InizializzaMbuto startDate, endDate
                        passaggioDati.MbutoInversoGiornaliero dailyAirTemperatureMin
                        If loadDailyVarSeries(isMeteoGrid, dailyAirTemperatureMax, myPoint, startDate, endDate) > 0 Then
                                preElaboration = True
                        End If
                        If preElaboration Then passaggioDati.MbutoInversoGiornaliero dailyAirTemperatureMax
                    End If

                case phenology:
                    If loadDailyVarSeries(isMeteoGrid, dailyAirTemperatureMin, myPoint, startDate, endDate) > 0 Then
                            preElaboration = True
                    End If
                    If preElaboration Then
                        preElaboration = False
                        passaggioDati.InizializzaMbuto startDate, endDate
                        passaggioDati.MbutoInversoGiornaliero dailyAirTemperatureMin
                        If loadDailyVarSeries(isMeteoGrid, dailyAirTemperatureMax, myPoint, startDate, endDate) > 0 Then
                                preElaboration = True
                        End If
                        If preElaboration Then
                            preElaboration = False
                            passaggioDati.MbutoInversoGiornaliero dailyAirTemperatureMax
                            If loadDailyVarSeries(isMeteoGrid, Definitions.DAILY_PREC, myPoint, startDate, endDate) > 0 Then
                                    preElaboration = True
                            End If
                        End If
                    End If

                default:
                {

                    percValue = loadDailyVarSeries(isMeteoGrid, variable, myPoint, startDate, endDate)

                    If percValue > 0 Then
                            preElaboration = True
                    End If
                }


            }
        }

    }
*/

    return true;
}



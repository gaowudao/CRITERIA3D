#include "climate.h"
#include "crit3dDate.h"
#include "utilities.h"
#include "quality.h"
#include "statistics.h"

#define THOMTHRESHOLD 24 // mettere nei settings quando ci saranno Environment.ThomThreshold
#define WINKLERTHRESHOLD 10 // LC setting?
#define MINPERCENTAGE 80 // mettere nei settings quando ci saranno Environment.minPercentage
#define AutomaticETP 1 // LC setting?
#define AutomaticTmed 1 // LC setting?
float RainfallThreshold = 0.2f; // LC mettere nei settings Environment.RainfallThreshold



bool elaborationPointsCycle(std::string *myError, Crit3DMeteoPointsDbHandler* meteoPointsDbHandler, Crit3DMeteoPoint* meteoPoints, int nrMeteoPoints, Crit3DClimate* referenceClima, Crit3DClimate* clima, QDate currentDay, bool isAnomaly)
{


    bool isMeteoGrid = 0; // meteoPoint
    float currentParameter1;
    int validCell = 0;
    int myClimateIndex;


    QDate startDate(clima->yearStart(), clima->genericPeriodDateStart().month(), clima->genericPeriodDateStart().day());
    QDate endDate(clima->yearEnd(), clima->genericPeriodDateEnd().month(), clima->genericPeriodDateEnd().day());

    if (clima->nYears() > 0)
    {
        endDate.setDate(clima->yearEnd() + clima->nYears(), clima->genericPeriodDateEnd().month(), clima->genericPeriodDateEnd().day());
    }
    else if (clima->nYears() < 0)
    {
        startDate.setDate(clima->yearStart() + clima->nYears(), clima->genericPeriodDateStart().month(), clima->genericPeriodDateStart().day());
    }

    if (clima->param1IsClimate())
    {
        parserElaboration(clima);
        myClimateIndex = getClimateIndexFromDate(currentDay, clima->periodType());
     }


    for ( unsigned int i = 0; i < nrMeteoPoints; i++)
    {

        if (clima->param1IsClimate())
        {
//            if ( ClimateReadPoint(PragaClimate.Point(i).TableName, climateElab, myPeriodType, myClimateIndex, PragaClimate.Point(i)))
//            {

//                currentParameter1 = passaggioDati.GetClimateData(myPeriodType, PragaClimate.Point(i), myClimateIndex);
//            }
//            else
//            {
//                currentParameter1 = NODATA;
//            }
             clima->setParam1(currentParameter1);
        }


        if (clima->elab1() == "phenology")
        {
            //Then currentPheno.setPhenoPoint i;  // TODO
        }

        if ( elaborationOnPoint(myError, meteoPointsDbHandler, NULL, &meteoPoints[i], clima, isMeteoGrid, startDate, endDate, isAnomaly, true))
        {
            validCell = validCell + 1;
        }


    }
    if (validCell == 0)
    {
        *myError = "no valid cells available";
        return false;
    }
    else
    {
        return true;
    }

}


bool elaborationPointsCycleGrid(std::string *myError, Crit3DMeteoGridDbHandler* meteoGridDbHandler, Crit3DClimate* referenceClima, Crit3DClimate* clima, QDate currentDay, bool isAnomaly)
{

    bool isMeteoGrid = 1; // grid
    float currentParameter1;
    int validCell = 0;
    int myClimateIndex;

    std::string id;


    QDate startDate(clima->yearStart(), clima->genericPeriodDateStart().month(), clima->genericPeriodDateStart().day());
    QDate endDate(clima->yearEnd(), clima->genericPeriodDateEnd().month(), clima->genericPeriodDateEnd().day());

    if (clima->nYears() > 0)
    {
        endDate.setDate(clima->yearEnd() + clima->nYears(), clima->genericPeriodDateEnd().month(), clima->genericPeriodDateEnd().day());
    }
    else if (clima->nYears() < 0)
    {
        startDate.setDate(clima->yearStart() + clima->nYears(), clima->genericPeriodDateStart().month(), clima->genericPeriodDateStart().day());
    }


    if (clima->param1IsClimate())
    {

        parserElaboration(clima);
        myClimateIndex = getClimateIndexFromDate(currentDay, clima->periodType());

     }


     for (int row = 0; row < meteoGridDbHandler->gridStructure().header().nrRows; row++)
     {
         for (int col = 0; col < meteoGridDbHandler->gridStructure().header().nrCols; col++)
         {

            if (meteoGridDbHandler->meteoGrid()->getMeteoPointActiveId(row, col, &id))
            {

                if (clima->param1IsClimate())
                {
//                    if (Climate.ClimateReadPoint(PragaClimate.Point(row, col).TableName, _
//                        climateElab, myPeriodType, myClimateIndex,PragaClimate.Point(row, col)) )
//                    {

//                      currentParameter1 = passaggioDati.GetClimateData(myPeriodType, PragaClimate.Point(row, col), myClimateIndex)
//                    }
//                    else
//                    {
//                        currentParameter1 = NODATA;
//                    }
                    clima->setParam1(currentParameter1);
                }


                Crit3DMeteoPoint meteoPoint = meteoGridDbHandler->meteoGrid()->meteoPoint(row,col);
                if  ( elaborationOnPoint(myError, NULL, meteoGridDbHandler, &meteoPoint, clima, isMeteoGrid, startDate, endDate, isAnomaly, true))
                {
                    validCell = validCell + 1;
                }

            }

        }
    }

    if (validCell == 0)
    {
        *myError = "no valid cells available";
        return false;
    }
    else
    {
        return true;
    }

}


bool elaborationOnPoint(std::string *myError, Crit3DMeteoPointsDbHandler* meteoPointsDbHandler, Crit3DMeteoGridDbHandler* meteoGridDbHandler,
    Crit3DMeteoPoint* meteoPoint, Crit3DClimate* clima, bool isMeteoGrid, QDate startDate, QDate endDate, bool isAnomaly, bool loadData)
{


    float percValue;
    bool dataLoaded;
    float result;

    Crit3DMeteoPoint* meteoPointTemp = new Crit3DMeteoPoint;
    std::vector<float> outputValues;

    meteoPointTemp->id = meteoPoint->id;
    meteoPointTemp->point.z = meteoPoint->point.z;
    meteoPointTemp->firstDateDailyVar = meteoPoint->firstDateDailyVar;

    if (!isAnomaly || meteoPoint->anomaly != NODATA)
    {

            dataLoaded = false;

            if (loadData)
            {
                dataLoaded = preElaboration(myError, meteoPointsDbHandler, meteoGridDbHandler, meteoPointTemp, isMeteoGrid, clima->variable(), clima->elab1(), startDate, endDate, outputValues, &percValue);
            }
            else
            {
                dataLoaded = true;
            }

            if (dataLoaded)
            {
                // check
                Crit3DDate startD(startDate.day(), startDate.month(), clima->yearStart());
                Crit3DDate endD(endDate.day(), endDate.month(), clima->yearStart());

                if ( clima->nYears() < 0)
                {
                    startD.year = clima->yearStart() + clima->nYears();
                }
                else if ( clima->nYears() > 0)
                {
                    endD.year = clima->yearStart() + clima->nYears();
                }

                if (difference (startD, endD) < 0)
                {
                    *myError = "Wrong dates!";
                    return false;
                }
                if (clima->elab1() == "")
                {
                    *myError = "Missing elaboration";
                    return false;
                }

                meteoComputation elab1MeteoComp;
                meteoComputation elab2MeteoComp;
                try
                {
                    elab1MeteoComp = MapMeteoComputation.at(clima->elab1().toStdString());
                }
                catch (const std::out_of_range& oor) {
                    elab1MeteoComp = noMeteoComp;
                  }

                try
                {
                    elab2MeteoComp = MapMeteoComputation.at(clima->elab2().toStdString());
                }
                catch (const std::out_of_range& oor) {
                    elab2MeteoComp = noMeteoComp;
                  }

                result = computeStatistic(outputValues, meteoPointTemp, clima->yearStart(), clima->yearEnd(), startD, endD, clima->nYears(), elab1MeteoComp, clima->param1(), elab2MeteoComp, clima->param2());

                if (isAnomaly)
                {
                    return anomalyOnPoint(meteoPoint, result);
                }
                else
                {

                      meteoPoint->elaboration = result;
                      if (meteoPoint->elaboration != NODATA)
                      {
                          delete meteoPointTemp;
                          return true;
                      }
                      else
                      {
                          delete meteoPointTemp;
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

    delete meteoPointTemp;
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


float loadDailyVarSeries(std::string *myError, Crit3DMeteoPointsDbHandler *meteoPointsDbHandler,
        Crit3DMeteoGridDbHandler *meteoGridDbHandler, Crit3DMeteoPoint* meteoPoint, bool isMeteoGrid,
        meteoVariable variable, QDate first, QDate last, std::vector<float> &outputValues)
{
    std::vector<float> dailyValues;
    QDate firstDateDB;
    Crit3DQuality qualityCheck;
    int nrValidValues = 0;
    float percValue = -1;

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
        if (meteoPoint->nrObsDataDaysD == 0)
        {
            meteoPoint->initializeObsDataD(numberOfDays, getCrit3DDate(firstDateDB));
        }

    }
    // meteoPoint
    else
    {
        dailyValues = meteoPointsDbHandler->getDailyVar(myError, variable, getCrit3DDate(first), getCrit3DDate(last), &firstDateDB, meteoPoint );
    }

    meteoPoint->firstDateDailyVar = getCrit3DDate(firstDateDB);

    if ( dailyValues.empty() )
    {
        return percValue;
    }
    else
    {
        if (meteoPoint->nrObsDataDaysD == 0)
        {
            meteoPoint->initializeObsDataD(dailyValues.size(), meteoPoint->firstDateDailyVar);
        }

        for (unsigned int i = 0; i < dailyValues.size(); i++)
        {
            quality::type qualityT = qualityCheck.syntacticQualityControlSingleVal(variable, dailyValues[i]);
            if (qualityT == quality::accepted)
            {
                nrValidValues = nrValidValues + 1;
            }
            meteoPoint->setMeteoPointValueD(Crit3DDate(firstDateDB.day(), firstDateDB.month(), firstDateDB.year()), variable, dailyValues[i]);

            outputValues.push_back(dailyValues[i]);
            firstDateDB = firstDateDB.addDays(1);

        }
        percValue = nrValidValues / dailyValues.size();
        return percValue;
    }


}

float loadHourlyVarSeries(std::string *myError, Crit3DMeteoPointsDbHandler* meteoPointsDbHandler,
           Crit3DMeteoGridDbHandler* meteoGridDbHandler, Crit3DMeteoPoint* meteoPoint, bool isMeteoGrid,
           meteoVariable variable, QDateTime first, QDateTime last, std::vector<float> &outputValues)
{
    std::vector<float> hourlyValues;
    QDateTime firstDateDB;
    Crit3DQuality qualityCheck;
    int nrValidValues = 0;
    float percValue = -1;

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
        if (meteoPoint->nrObsDataDaysH == 0)
        {
            meteoPoint->initializeObsDataH(meteoPoint->hourlyFraction, numberOfDays, getCrit3DDate(firstDateDB.date()));
        }


    }
    // meteoPoint
    else
    {
        hourlyValues = meteoPointsDbHandler->getHourlyVar(myError, variable, getCrit3DDate(first.date()), getCrit3DDate(last.date()), &firstDateDB, meteoPoint );
    }

    meteoPoint->firstDateDailyVar = getCrit3DDate(firstDateDB.date());

    if ( hourlyValues.empty() )
    {
        return percValue;
    }
    else
    {
        if (meteoPoint->nrObsDataDaysH == 0)
        {
            meteoPoint->initializeObsDataH(meteoPoint->hourlyFraction, hourlyValues.size(), meteoPoint->firstDateDailyVar);
        }

        for (unsigned int i = 0; i < hourlyValues.size(); i++)
        {
            quality::type qualityT = qualityCheck.syntacticQualityControlSingleVal(variable, hourlyValues[i]);
            if (qualityT == quality::accepted)
            {
                nrValidValues = nrValidValues + 1;
            }
            meteoPoint->setMeteoPointValueH(Crit3DDate(firstDateDB.date().day(), firstDateDB.date().month(), firstDateDB.date().year()), firstDateDB.time().hour(), firstDateDB.time().minute(), variable, hourlyValues[i]);
            outputValues.push_back(hourlyValues[i]);
            firstDateDB = firstDateDB.addSecs(3600);
        }
        percValue = nrValidValues / hourlyValues.size();
        return percValue;
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
    quality::type qualityETP = qualityCheck.syntacticQualityControlSingleVal(dailyReferenceEvapotranspirationHS, etp);
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


float computeWinkler(Crit3DMeteoPoint* meteoPoint, Crit3DDate firstDate, Crit3DDate finishDate)
{

    float computeWinkler = 0;

    Crit3DQuality qualityCheck;
    unsigned int index;
    int count;
    bool checkData;
    float Tavg;


    int numberOfDays = difference(firstDate, finishDate) +1;

    Crit3DDate presentDate = firstDate;
    for (int i = 0; i < numberOfDays; i++)
    {
        index = difference(meteoPoint->firstDateDailyVar, presentDate);
        checkData = false;
        if ( index >= 0 && index < meteoPoint->nrObsDataDaysD)
        {

            // TO DO nella versione vb il check prevede anche l'immissione del parametro height
            quality::type qualityTavg = qualityCheck.syntacticQualityControlSingleVal(dailyAirTemperatureAvg, meteoPoint->obsDataD[index].tAvg);
            if (qualityTavg == quality::accepted)
            {
                Tavg = meteoPoint->obsDataD[index].tAvg;
                checkData = true;
            }
            else
            {
                quality::type qualityTmin = qualityCheck.syntacticQualityControlSingleVal(dailyAirTemperatureMin, meteoPoint->obsDataD[index].tMin);
                quality::type qualityTmax = qualityCheck.syntacticQualityControlSingleVal(dailyAirTemperatureMax, meteoPoint->obsDataD[index].tMax);
                if (qualityTmin  == quality::accepted && qualityTmax == quality::accepted)
                {
                    Tavg = (meteoPoint->obsDataD[index].tMin + meteoPoint->obsDataD[index].tMax)/2;
                    checkData = true;
                }

            }

        }
        if (checkData)
        {
            if (Tavg > WINKLERTHRESHOLD)
            {
                Tavg = Tavg - WINKLERTHRESHOLD;
            }
            else
            {
                Tavg = 0;
            }
            computeWinkler = computeWinkler + Tavg;
            count = count + 1;
        }
        presentDate = presentDate.addDays(1);
    }
    if (numberOfDays != 0)
    {
        if ( (count / numberOfDays * 100) < MINPERCENTAGE )
        {
            computeWinkler = NODATA;
        }
    }

    return computeWinkler;

}

float computeLastDayBelowThreshold(std::vector<float> &inputValues, Crit3DDate firstDateDailyVar, Crit3DDate firstDate, Crit3DDate finishDate, float param1)
{
    unsigned int index;
    float lastDay = NODATA;

    int numberOfDays = difference(firstDate, finishDate) +1;
    Crit3DDate presentDate = finishDate;
    for (int i = 0; i < numberOfDays; i++)
    {
        index = difference(firstDateDailyVar, presentDate);
        if ( index >= 0 && index < inputValues.size())
        {
            if (inputValues.at(index) != NODATA && inputValues.at(index) < param1)
            {
                lastDay = getDoyFromDate(presentDate);
            }
        }
        presentDate = presentDate.addDays(-1);
    }


    return lastDay;
}

float computeHuglin(Crit3DMeteoPoint* meteoPoint, Crit3DDate firstDate, Crit3DDate finishDate)
{
    float computeHuglin = 0;

    const int threshold = 10;                   // LC vanno lasciate qui queste costanti?
    const float K = 1.04;                       //coeff. K di Huglin lunghezza giorno (=1.04 per ER)

    Crit3DQuality qualityCheck;
    unsigned int index;
    int count;
    bool checkData;
    float Tavg;
    float Tmax;


    int numberOfDays = difference(firstDate, finishDate) +1;

    Crit3DDate presentDate = firstDate;
    for (int i = 0; i < numberOfDays; i++)
    {
        index = difference(meteoPoint->firstDateDailyVar, presentDate);
        checkData = false;
        if ( index >= 0 && index < meteoPoint->nrObsDataDaysD)
        {

            // TO DO nella versione vb il check prevede anche l'immissione del parametro height
            quality::type qualityTavg = qualityCheck.syntacticQualityControlSingleVal(dailyAirTemperatureAvg, meteoPoint->obsDataD[index].tAvg);
            quality::type qualityTmax = qualityCheck.syntacticQualityControlSingleVal(dailyAirTemperatureMax, meteoPoint->obsDataD[index].tMax);
            if (qualityTavg == quality::accepted && qualityTmax == quality::accepted)
            {
                Tavg = meteoPoint->obsDataD[index].tAvg;
                checkData = true;
            }
            else
            {
                quality::type qualityTmin = qualityCheck.syntacticQualityControlSingleVal(dailyAirTemperatureMin, meteoPoint->obsDataD[index].tMin);
                if (qualityTmin  == quality::accepted && qualityTmax == quality::accepted)
                {
                    Tmax = meteoPoint->obsDataD[index].tMax;
                    Tavg = (meteoPoint->obsDataD[index].tMin + Tmax)/2;
                    checkData = true;
                }

            }

        }
        if (checkData)
        {
            computeHuglin = computeHuglin + K * ((Tavg - threshold) + (Tmax - threshold)) / 2;
            count = count + 1;
        }
        presentDate = presentDate.addDays(1);
    }
    if (numberOfDays != 0)
    {
        if ( (count / numberOfDays * 100) < MINPERCENTAGE )
        {
            computeHuglin = NODATA;
        }
    }

    return computeHuglin;
}

float computeFregoni(Crit3DMeteoPoint* meteoPoint, Crit3DDate firstDate, Crit3DDate finishDate)
{
    const int threshold = 10;
    float computeFregoni = 0;

    Crit3DQuality qualityCheck;
    unsigned int index;
    int count, myDaysBelow;
    bool checkData;
    float tMin, tMax;
    float tRange, sumTRange;


    int numberOfDays = difference(firstDate, finishDate) +1;

    Crit3DDate presentDate = firstDate;
    for (int i = 0; i < numberOfDays; i++)
    {
        index = difference(meteoPoint->firstDateDailyVar, presentDate);
        checkData = false;
        if ( index >= 0 && index < meteoPoint->nrObsDataDaysD)
        {

            // TO DO nella versione vb il check prevede anche l'immissione del parametro height
            quality::type qualityTmin = qualityCheck.syntacticQualityControlSingleVal(dailyAirTemperatureMin, meteoPoint->obsDataD[index].tMin);
            quality::type qualityTmax = qualityCheck.syntacticQualityControlSingleVal(dailyAirTemperatureMax, meteoPoint->obsDataD[index].tMax);
            if (qualityTmin == quality::accepted && qualityTmax == quality::accepted)
            {
                tMin = meteoPoint->obsDataD[index].tMin;
                tMax = meteoPoint->obsDataD[index].tMax;
                tRange = tMax - tMin;
                checkData = true;
            }

        }
        if (checkData)
        {
            sumTRange = sumTRange + tRange;
            if (tMin < threshold)
            {
                myDaysBelow = myDaysBelow + 1;
            }
            count = count + 1;
        }
        presentDate = presentDate.addDays(1);
    }
    if (numberOfDays != 0)
    {
        if ( (count / numberOfDays * 100) < MINPERCENTAGE )
        {
            computeFregoni = NODATA;
        }
        else
        {
            computeFregoni = sumTRange * myDaysBelow;
        }
    }

    return computeFregoni;
}

float computeCorrectedSum(Crit3DMeteoPoint* meteoPoint, Crit3DDate firstDate, Crit3DDate finishDate)
{
    // TO DO
    return NODATA;
}


bool elaborateDailyAggregatedVar(meteoVariable myVar, Crit3DMeteoPoint meteoPoint, std::vector<float> &outputValues, float* percValue)
{

    frequencyType aggregationFrequency = getAggregationFrequency(myVar);

    if (aggregationFrequency == hourly)
    {
            return elaborateDailyAggregatedVarFromHourly(myVar, meteoPoint, outputValues);
    }
    else if (aggregationFrequency == daily)
    {
            return elaborateDailyAggregatedVarFromDaily(myVar, meteoPoint, outputValues, percValue);
    }
    else
        return false;

}


frequencyType getAggregationFrequency(meteoVariable myVar)
{

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

bool elaborateDailyAggregatedVarFromDaily(meteoVariable myVar, Crit3DMeteoPoint meteoPoint, std::vector<float> &outputValues, float* percValue)
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
                res = computeDailyBIC(meteoPoint.obsDataD[index].prec, meteoPoint.obsDataD[index].et0_hs);
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
        case dailyReferenceEvapotranspirationHS:
        {
            quality::type qualityEtp = qualityCheck.syntacticQualityControlSingleVal(dailyReferenceEvapotranspirationHS, meteoPoint.obsDataD[index].et0_hs);
            if (qualityEtp == quality::accepted)
            {
                res = meteoPoint.obsDataD[index].et0_hs;
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
            meteoPoint.firstDateDailyVar = date;
        }
        outputValues.push_back(res);
        date = date.addDays(1);

    }

    *percValue = nrValidi/outputValues.size();
    if (nrValidi > 0)
        return true;
    else
        return false;

}

bool elaborateDailyAggregatedVarFromHourly(meteoVariable myVar, Crit3DMeteoPoint meteoPoint, std::vector<float> &outputValues)
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
            meteoPoint.firstDateDailyVar = date;

        }
        outputValues.push_back(res);
        date = date.addDays(1);
    }
    if (nrValidi > 0)
        return true;
    else
        return false;

}

bool preElaboration(std::string *myError, Crit3DMeteoPointsDbHandler* meteoPointsDbHandler, Crit3DMeteoGridDbHandler* meteoGridDbHandler, Crit3DMeteoPoint* meteoPoint, bool isMeteoGrid, meteoVariable variable, QString elab1,
    QDate startDate, QDate endDate, std::vector<float> &outputValues, float* percValue)
{

    bool preElaboration = false;


    switch(variable)
    {

        case dailyLeafWetness:
        {
            if ( loadHourlyVarSeries(myError, meteoPointsDbHandler, meteoGridDbHandler, meteoPoint, isMeteoGrid, leafWetness, QDateTime(startDate,QTime(0,0,0)), QDateTime(endDate,QTime(0,0,0)), outputValues) > 0)
            {
                preElaboration = elaborateDailyAggregatedVar(dailyLeafWetness, *meteoPoint, outputValues, percValue);
            }
            break;
        }

        case dailyThomDaytime:
        {
            if ( loadDailyVarSeries(myError, meteoPointsDbHandler, meteoGridDbHandler, meteoPoint, isMeteoGrid, dailyAirRelHumidityMin, startDate, endDate, outputValues) > 0)
            {
                    preElaboration = true;
            }
            if (preElaboration)
            {
                preElaboration = false;
                if (loadDailyVarSeries(myError, meteoPointsDbHandler, meteoGridDbHandler, meteoPoint, isMeteoGrid, dailyAirTemperatureMax, startDate, endDate, outputValues) > 0)
                {
                    preElaboration = true;
                }
                if (preElaboration)
                {
                    preElaboration = elaborateDailyAggregatedVar(dailyThomDaytime, *meteoPoint, outputValues, percValue);
                }
            }
            break;
        }

        case dailyThomNighttime:
        {
            if (loadDailyVarSeries(myError, meteoPointsDbHandler, meteoGridDbHandler, meteoPoint, isMeteoGrid, dailyAirRelHumidityMax, startDate, endDate, outputValues) > 0)
            {
                preElaboration = true;
            }
            if (preElaboration)
            {
                preElaboration = false;
                if (loadDailyVarSeries(myError, meteoPointsDbHandler, meteoGridDbHandler, meteoPoint, isMeteoGrid, dailyAirTemperatureMin, startDate, endDate, outputValues) > 0)
                {
                    preElaboration = true;
                }
                if (preElaboration)
                {
                    preElaboration = elaborateDailyAggregatedVar(dailyThomNighttime, *meteoPoint, outputValues, percValue);
                }
            }
            break;
        }
        case dailyThomAvg: case dailyThomMax: case dailyThomHoursAbove:
        {

            if (loadHourlyVarSeries(myError, meteoPointsDbHandler, meteoGridDbHandler, meteoPoint, isMeteoGrid, airTemperature, QDateTime(startDate,QTime(0,0,0)), QDateTime(endDate,QTime(0,0,0)), outputValues)  > 0)
            {
                if (loadHourlyVarSeries(myError, meteoPointsDbHandler, meteoGridDbHandler, meteoPoint, isMeteoGrid, airRelHumidity, QDateTime(startDate,QTime(0,0,0)), QDateTime(endDate,QTime(0,0,0)), outputValues)  > 0)
                {
                    preElaboration = elaborateDailyAggregatedVar(variable, *meteoPoint, outputValues, percValue);
                }
            }
            break;
        }
        case dailyBIC:
        {
            if (loadDailyVarSeries(myError, meteoPointsDbHandler, meteoGridDbHandler, meteoPoint, isMeteoGrid, dailyReferenceEvapotranspirationHS, startDate, endDate, outputValues) > 0)
            {
                preElaboration = true;
            }
            else if (AutomaticETP)
            {
                if (loadDailyVarSeries(myError, meteoPointsDbHandler, meteoGridDbHandler, meteoPoint, isMeteoGrid, dailyAirTemperatureMin, startDate, endDate, outputValues) > 0)
                {
                    preElaboration = true;
                }
                if (preElaboration)
                {
                    preElaboration = false;
                    if ( loadDailyVarSeries(myError, meteoPointsDbHandler, meteoGridDbHandler, meteoPoint, isMeteoGrid, dailyAirTemperatureMax, startDate, endDate, outputValues) > 0)
                    {
                        preElaboration = true;
                    }
                    if (preElaboration)
                    {
                        preElaboration = elaborateDailyAggregatedVar(dailyReferenceEvapotranspirationHS, *meteoPoint, outputValues, percValue);
                    }
                }
            }
            if (preElaboration)
            {
                preElaboration = false;
                if ( loadDailyVarSeries(myError, meteoPointsDbHandler, meteoGridDbHandler, meteoPoint, isMeteoGrid, dailyPrecipitation, startDate, endDate, outputValues) > 0)
                {
                    preElaboration = true;
                }
                if (preElaboration)
                {
                    preElaboration = elaborateDailyAggregatedVar(dailyBIC, *meteoPoint, outputValues, percValue);
                }
            }
            break;
        }

        case dailyAirTemperatureRange:
        {
            if (loadDailyVarSeries(myError, meteoPointsDbHandler, meteoGridDbHandler, meteoPoint, isMeteoGrid, dailyAirTemperatureMin, startDate, endDate, outputValues) > 0)
            {
                preElaboration = true;
            }
            if (preElaboration)
            {
                preElaboration = false;
                if (loadDailyVarSeries(myError, meteoPointsDbHandler, meteoGridDbHandler, meteoPoint, isMeteoGrid, dailyAirTemperatureMax, startDate, endDate, outputValues) > 0)
                {
                    preElaboration = true;
                }
                if (preElaboration)
                {
                    preElaboration = elaborateDailyAggregatedVar(dailyAirTemperatureRange, *meteoPoint, outputValues, percValue);
                }
            }
            break;
        }
        case dailyAirDewTemperatureMax:
        {
            if ( loadDailyVarSeries(myError, meteoPointsDbHandler, meteoGridDbHandler, meteoPoint, isMeteoGrid, dailyAirTemperatureMax, startDate, endDate, outputValues) > 0)
            {
                preElaboration = true;
            }
            if (preElaboration)
            {
                preElaboration = false;
                if ( loadDailyVarSeries(myError, meteoPointsDbHandler, meteoGridDbHandler, meteoPoint, isMeteoGrid, dailyAirRelHumidityMin, startDate, endDate, outputValues) > 0)
                {
                    preElaboration = true;
                }
                if (preElaboration)
                {
                    preElaboration = elaborateDailyAggregatedVar(dailyAirDewTemperatureMax, *meteoPoint, outputValues, percValue);
                }
            }
            break;
        }

        case dailyAirDewTemperatureMin:
        {
            if ( loadDailyVarSeries(myError, meteoPointsDbHandler, meteoGridDbHandler, meteoPoint, isMeteoGrid, dailyAirTemperatureMin, startDate, endDate, outputValues) > 0)
            {
                preElaboration = true;
            }
            if (preElaboration)
            {
                preElaboration = false;
                if ( loadDailyVarSeries(myError, meteoPointsDbHandler, meteoGridDbHandler, meteoPoint, isMeteoGrid, dailyAirRelHumidityMax, startDate, endDate, outputValues) > 0)
                {
                    preElaboration = true;
                }
                if (preElaboration)
                {
                    preElaboration = elaborateDailyAggregatedVar(dailyAirDewTemperatureMin, *meteoPoint, outputValues, percValue);
                }
            }
            break;
        }

        case dailyAirTemperatureAvg:
        {
            if ( loadDailyVarSeries(myError, meteoPointsDbHandler, meteoGridDbHandler, meteoPoint, isMeteoGrid, dailyAirTemperatureAvg, startDate, endDate, outputValues) > 0)
            {
                preElaboration = true;
            }
            else if (AutomaticTmed)
            {
                if ( loadDailyVarSeries(myError, meteoPointsDbHandler, meteoGridDbHandler, meteoPoint, isMeteoGrid, dailyAirTemperatureMin, startDate, endDate, outputValues) > 0 )
                {
                    preElaboration = true;
                }
                if (preElaboration)
                {
                    preElaboration = false;
                    if ( loadDailyVarSeries(myError, meteoPointsDbHandler, meteoGridDbHandler, meteoPoint, isMeteoGrid, dailyAirTemperatureMax, startDate, endDate, outputValues) > 0)
                    {
                        preElaboration = true;
                    }
                    if (preElaboration)
                    {
                        preElaboration = elaborateDailyAggregatedVar(dailyAirTemperatureAvg, *meteoPoint, outputValues, percValue);
                    }
                }
            }
            break;
        }

        case dailyReferenceEvapotranspirationHS:
        {
            if ( loadDailyVarSeries(myError, meteoPointsDbHandler, meteoGridDbHandler, meteoPoint, isMeteoGrid, dailyReferenceEvapotranspirationHS, startDate, endDate, outputValues) > 0)
            {
                preElaboration = true;
            }

            else if (AutomaticETP)
            {
                if ( loadDailyVarSeries(myError, meteoPointsDbHandler, meteoGridDbHandler, meteoPoint, isMeteoGrid, dailyAirTemperatureMin, startDate, endDate, outputValues) > 0)
                {
                    preElaboration = true;
                }
                if (preElaboration)
                {
                    preElaboration = false;
                    if ( loadDailyVarSeries(myError, meteoPointsDbHandler, meteoGridDbHandler, meteoPoint, isMeteoGrid, dailyAirTemperatureMax, startDate, endDate, outputValues) > 0)
                    {
                        preElaboration = true;
                    }
                    if (preElaboration)
                    {
                        preElaboration = elaborateDailyAggregatedVar(dailyReferenceEvapotranspirationHS, *meteoPoint, outputValues, percValue);
                    }

                }
            }
            break;
        }

        default:
        {

            switch(elab1.toInt())
            {

                case huglin:
                {
                    if ( loadDailyVarSeries(myError, meteoPointsDbHandler, meteoGridDbHandler, meteoPoint, isMeteoGrid, dailyAirTemperatureMin, startDate, endDate, outputValues) > 0 )
                    {
                        preElaboration = true;
                    }
                    if (preElaboration)
                    {
                        preElaboration = false;
                        if ( loadDailyVarSeries(myError, meteoPointsDbHandler, meteoGridDbHandler, meteoPoint, isMeteoGrid, dailyAirTemperatureMax, startDate, endDate, outputValues) > 0 )
                        {
                            preElaboration = true;
                        }
                        if (preElaboration)
                        {
                            preElaboration = false;
                            if ( loadDailyVarSeries(myError, meteoPointsDbHandler, meteoGridDbHandler, meteoPoint, isMeteoGrid, dailyAirTemperatureAvg, startDate, endDate, outputValues) > 0 )
                            {
                                preElaboration = true;
                            }
                            else if (AutomaticTmed)
                            {
                                preElaboration = elaborateDailyAggregatedVar(dailyAirTemperatureAvg, *meteoPoint, outputValues, percValue);
                            }
                        }
                    }
                    break;
                }

            case winkler: case correctedDegreeDaysSum: case fregoni:
            {

                if ( loadDailyVarSeries(myError, meteoPointsDbHandler, meteoGridDbHandler, meteoPoint, isMeteoGrid, dailyAirTemperatureMin, startDate, endDate, outputValues) > 0 )
                {
                    preElaboration = true;
                }
                if (preElaboration)
                {
                    preElaboration = false;
                    if ( loadDailyVarSeries(myError, meteoPointsDbHandler, meteoGridDbHandler, meteoPoint, isMeteoGrid, dailyAirTemperatureMax, startDate, endDate, outputValues) > 0 )
                    {
                        preElaboration = true;
                    }
                }
                break;
            }

            case phenology:
            {
                if ( loadDailyVarSeries(myError, meteoPointsDbHandler, meteoGridDbHandler, meteoPoint, isMeteoGrid, dailyAirTemperatureMin, startDate, endDate, outputValues) > 0 )
                {
                    preElaboration = true;
                }
                if (preElaboration)
                {
                    preElaboration = false;
                    if ( loadDailyVarSeries(myError, meteoPointsDbHandler, meteoGridDbHandler, meteoPoint, isMeteoGrid, dailyAirTemperatureMax, startDate, endDate, outputValues) > 0 )
                    {
                        preElaboration = true;
                    }
                    if (preElaboration)
                    {
                        preElaboration = false;
                        if (loadDailyVarSeries(myError, meteoPointsDbHandler, meteoGridDbHandler, meteoPoint, isMeteoGrid, dailyPrecipitation, startDate, endDate, outputValues) > 0 )
                        {
                            preElaboration = true;
                        }
                    }
                }
                break;
             }

            default:
            {

                *percValue = loadDailyVarSeries(myError, meteoPointsDbHandler, meteoGridDbHandler, meteoPoint, isMeteoGrid, variable, startDate, endDate, outputValues);

                if ( (*percValue) > 0)
                {
                    preElaboration = true;
                }
                else
                {
                    preElaboration = false;
                }
                break;
            }


            }
            break;
        }

    }

    if (elab1 != "consecutiveDaysAbove" && elab1 != "consecutiveDaysBelow" && elab1 != "trend")
    {
        // LC rimossa condizione And Not isCumulated
        if ( variable == dailyPrecipitation && elab1 == "percentile")
        {
            extractValidValuesWithThreshold(outputValues, RainfallThreshold);
        }
        else
        {
            extractValidValuesCC(outputValues);
        }
    }


    return preElaboration;
}


bool parserElaboration(Crit3DClimate* clima)
{

    int pos = 0;

    QString climateElab = clima->climateElab();
    QStringList words = climateElab.split('_');

    if (words.isEmpty())
    {
        return false;
    }

    QStringList myYearWords = words[pos].split('÷');

    foreach(QChar c, myYearWords[0])
    {
        if (!c.isDigit())
            return false;
    }
    foreach(QChar c, myYearWords[1])
    {
        if (!c.isDigit())
            return false;
    }
    clima->setYearStart(myYearWords[0].toInt());
    clima->setYearEnd(myYearWords[1].toInt());

    pos = pos + 1;

    if (words.size() == pos)
    {
        return false;
    }

    meteoVariable var;
    try {
      var = MapDailyMeteoVar.at(words[pos].toStdString());
    }
    catch (const std::out_of_range& oor) {
      var = noMeteoVar;
    }
    clima->setVariable(var);

    pos = pos + 1;

    if (words.size() == pos)
    {
        return false;
    }

    QString periodTypeStr = words[pos];

    clima->setPeriodType(getPeriodTypeFromString(periodTypeStr));

    pos = pos + 1; // pos = 3

    if (words.size() == pos)
    {
        return false;
    }

    if ( (clima->periodType() == genericPeriod) && ( (words[pos].at(0)).isDigit() ) )
    {
        clima->setPeriodStr(words[pos]);
        parserGenericPeriodString(clima);
    }

    pos = pos + 1; // pos = 4

    if (words.size() == pos)
    {
        return false;
    }

    QString elab = words[pos];

    meteoComputation elabMeteoComputation = MapMeteoComputation.at(elab.toStdString());

    float param = NODATA;
    int nrParam = nParameters(elabMeteoComputation);

    if (nrParam > 0)
    {
        pos = pos + 1;
        if ( words[pos].at(0) == "|" )
        {
            clima->setParam1IsClimate(true);
            QString param1ClimateField = words[pos];
            param1ClimateField.remove(0,1);

            pos = pos + 1;
            if ( words[pos].right(2) == "||" ) return false;

            while ( words[pos].right(2) != "||" )
            {
                param1ClimateField = param1ClimateField + "_" + words[pos];
                pos = pos + 1;
            }
            param1ClimateField = param1ClimateField + "_" + words[pos].left(words[pos].size() - 2);

            clima->setParam1ClimateField(param1ClimateField);
            param =  NODATA;
        }
        else
        {
            clima->setParam1IsClimate(false);
            clima->setParam1ClimateField("");
            param = words[pos].toFloat(); // GA controllare se funziona bene gestione separatore decimale
        }
    }

    if (words.size() > pos)
    {
        clima->setElab2(elab);
        clima->setParam2(param);
        pos = pos + 1;

        QString elab1 = words[pos];
        elabMeteoComputation = MapMeteoComputation.at(elab1.toStdString());
        nrParam = nParameters(elabMeteoComputation);

        if (nrParam > 0)
        {
            pos = pos + 1;
            if ( words[pos].at(0) == "|" )
            {
                clima->setParam1IsClimate(true);
                QString param1ClimateField = words[pos];
                param1ClimateField.remove(0,1);

                pos = pos + 1;
                if ( words[pos].right(2) == "||" ) return false;

                while ( words[pos].right(2) != "||" )
                {
                    pos = pos + 1;
                    param1ClimateField = param1ClimateField + "_" + words[pos];
                }
                pos = pos + 1;
                param1ClimateField = param1ClimateField + "_" + words[pos].left(words[pos].size() - 2);

                clima->setParam1(NODATA);
            }
            else
            {
                clima->setParam1IsClimate(false);
                clima->setParam1ClimateField("");
                clima->setParam1( words[pos].toFloat() );// GA controllare se funziona bene gestione separatore decimale
            }
        }

    }
    else
    {
        clima->setElab1(elab);
        clima->setParam1(param);
    }

    return true;

}


period getPeriodTypeFromString(QString periodStr)
{

    if (periodStr == "Daily")
        return dailyPeriod;
    if (periodStr == "Decadal")
        return decadalPeriod;
    if (periodStr == "Monthly")
        return monthlyPeriod;
    if (periodStr == "Seasonal")
        return seasonalPeriod;
    if (periodStr == "Annual")
        return annualPeriod;
    if (periodStr == "Generic") // era generic_period
        return genericPeriod;

    return noPeriodType;

}

bool parserGenericPeriodString(Crit3DClimate *clima)
{

    if ( clima->periodStr().isEmpty())
    {
        return false;
    }

    QString day = clima->periodStr().mid(0,2);
    QString month = clima->periodStr().mid(3,2);
    int year = 2000;
    clima->setGenericPeriodDateStart( QDate(year,  month.toInt(),  day.toInt()) );

    day = clima->periodStr().mid(6,2);
    month = clima->periodStr().mid(9,2);

    clima->setGenericPeriodDateEnd( QDate(year,  month.toInt(),  day.toInt()) );

    if ( clima->periodStr().size() > 11 )
    {
        clima->setNYears( (clima->periodStr().mid(13,2)).toInt() );
    }
    return true;

}

// LC fatta lista delle elab con parametri usata nell'interfaccia, probabilmente da rimuovere questa funzione
int nParameters(meteoComputation elab)
{
    switch(elab)
    {
    case average:
        return 0;
    case maxInList:
        return 0;
    case minInList:
        return 0;
    case sum:
        return 0;
    case avgAbove:
        return 1;
    case stdDevAbove:
        return 1;
    case sumAbove:
        return 1;
    case daysAbove:
        return 1;
    case daysBelow:
        return 1;
    case consecutiveDaysAbove:
        return 1;
    case consecutiveDaysBelow:
        return 1;
    case percentile:
        return 1;
    case prevailingWindDir:
        return 0;
    case correctedDegreeDaysSum:
        return 1;
    case trend:
        return 0;
    case mannKendall:
        return 0;
    case differenceWithThreshold:
        return 1;
    case lastDayBelowThreshold:
        return 1;
    default:
        return 0;
    }


}

void extractValidValuesCC(std::vector<float> &outputValues)
{

    for ( unsigned int i = 0;  i < outputValues.size(); i++)
    {
        if (outputValues.at(i) == NODATA)
        {
            outputValues.erase(outputValues.begin()+i);
        }
    }

}


void extractValidValuesWithThreshold(std::vector<float> &outputValues, float myThreshold)
{

    for ( unsigned int i = 0;  i < outputValues.size(); i++)
    {
        if (outputValues.at(i) == NODATA || outputValues.at(i) < myThreshold)
        {
            outputValues.erase(outputValues.begin()+i);
        }
    }

}


int getClimateIndexFromDate(QDate myDate, period periodType)
{

    switch(periodType)
    {
    case annualPeriod: genericPeriod:
            return 1;
    case decadalPeriod:
            return decadeFromDate(myDate);
    case monthlyPeriod:
            return myDate.month();
    case seasonalPeriod:
            return getSeasonFromDate(myDate);
    case dailyPeriod:
            return myDate.dayOfYear();
    default:
            return NODATA;
    }

}


//nYears   = 0         same year
//nYears   = 1,2,3...   betweend years 1,2,3...
float computeStatistic(std::vector<float> &inputValues, Crit3DMeteoPoint* meteoPoint, int firstYear, int lastYear, Crit3DDate firstDate, Crit3DDate lastDate, int nYears, meteoComputation elab1, float param1, meteoComputation elab2, float param2)
{

    std::vector<float> values;
    std::vector<float> valuesSecondElab;
    Crit3DDate presentDate;
    int numberOfDays;
    int nValidValues = 0;
    int nValues = 0;
    unsigned int index;

    float primary = NODATA;


    // no secondary elab
    if (elab2 == noMeteoComp)
    {
        switch(elab1)
        {
            case lastDayBelowThreshold:
            {
                return computeLastDayBelowThreshold(inputValues, meteoPoint->firstDateDailyVar ,firstDate, lastDate, param1);
            }
            case winkler:
            {
                return computeWinkler(meteoPoint, firstDate, lastDate);
            }
            case huglin:
            {
                return computeHuglin(meteoPoint, firstDate, lastDate);
            }
            case fregoni:
            {
                return computeFregoni(meteoPoint, firstDate, lastDate);
            }
            case correctedDegreeDaysSum:
            {
                return computeCorrectedSum(meteoPoint, firstDate, lastDate);
            }
            default:
            {
                for (int presentYear = firstYear; presentYear <= lastYear; presentYear++)
                {
                    firstDate.year = presentYear;
                    lastDate.year = presentYear;

                    if (nYears < 0)
                    {
                        firstDate.year = (presentYear + nYears);
                    }
                    else if (nYears > 0)
                    {
                        lastDate.year = (presentYear + nYears);
                    }

                    numberOfDays = difference(firstDate, lastDate) +1;
                    presentDate = firstDate;
                    for (int i = 0; i < numberOfDays; i++)
                    {

                        float value = NODATA;
                        index = difference(meteoPoint->firstDateDailyVar, presentDate);
                        if (index >= 0 && index < inputValues.size())
                        {
                            value = inputValues.at(index);
                        }
                        if (value != NODATA)
                        {
                            nValidValues = nValidValues + 1;
                        }

                        values.push_back(value);
                        nValues = nValues + 1;
                        presentDate = presentDate.addDays(1);

                    }
                }

                if (nValues > 0)
                {
                    if ( (nValidValues / nValues) * 100 >= MINPERCENTAGE)
                    {
                        return elaborations::statisticalElab(elab1, param1, values, nValues);
                    }
                    else
                    {
                        return NODATA;
                    }
                }

                break;
            }
        }
    }
    // secondary elab
    else
    {

        int nTotYears = 0;
        int nValidYears = 0;

        for (int presentYear = firstYear; presentYear <= lastYear; presentYear++)
        {
            firstDate.year = presentYear;
            lastDate.year = presentYear;

            if (nYears < 0)
            {
                firstDate.year = (presentYear + nYears);
            }
            else if (nYears > 0)
            {
                lastDate.year = (presentYear + nYears);
            }
            primary = NODATA;

            nValues = 0;
            nValidValues = 0;

            switch(elab1)
            {
                case lastDayBelowThreshold:
                {
                    primary = computeLastDayBelowThreshold(inputValues, meteoPoint->firstDateDailyVar ,firstDate, lastDate, param1);
                    break;
                }
                case winkler:
                {
                    primary = computeWinkler(meteoPoint, firstDate, lastDate);
                    break;
                }
                case huglin:
                {
                    primary = computeHuglin(meteoPoint, firstDate, lastDate);
                    break;
                }
                case fregoni:
                {
                    primary = computeFregoni(meteoPoint, firstDate, lastDate);
                    break;
                }
                case correctedDegreeDaysSum:
                {
                    primary = computeCorrectedSum(meteoPoint, firstDate, lastDate);
                    break;
                }
                default:
                {

                    numberOfDays = difference(firstDate, lastDate) +1;
                    presentDate = firstDate;
                    for (int i = 0; i < numberOfDays; i++)
                    {
                        float value = NODATA;
                        index = difference(meteoPoint->firstDateDailyVar, presentDate);
                        if (index >= 0 && index < inputValues.size())
                        {
                            value = inputValues.at(index);
                        }
                        if (value != NODATA)
                        {
                            nValidValues = nValidValues + 1;
                        }

                        values.push_back(value);
                        nValues = nValues + 1;
                        presentDate = presentDate.addDays(1);

                    }

                    if (nValues > 0)
                    {
                        if ( (nValidValues / nValues) * 100 >= MINPERCENTAGE)
                        {
                            primary = elaborations::statisticalElab(elab1, param1, values, nValues);
                        }
                    }

                    break;

                }
            }

            if (primary != NODATA)
            {
                nValidYears = nValidYears + 1;
            }

            valuesSecondElab.push_back(primary);
            nTotYears = nTotYears + 1;

        } // end for

        if (nTotYears > 0)
        {
            if ( (nValidYears / nTotYears) * 100 >= MINPERCENTAGE)
            {
                switch(elab2)
                {
                    case trend:
                        return elaborations::statisticalElab(elab2, firstYear, valuesSecondElab, nTotYears);
                    default:
                        return elaborations::statisticalElab(elab2, param2, valuesSecondElab, nTotYears);
                }
            }
        }

    }

}

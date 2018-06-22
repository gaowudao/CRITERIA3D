#include "climate.h"
#include "crit3dDate.h"
#include "utilities.h"
#include "quality.h"


bool elaborationPointsCycle(std::string *myError, Crit3DMeteoPointsDbHandler* meteoPointsDbHandler, meteoVariable variable, int firstYear, int lastYear, QDate firstDate, QDate lastDate, int nYears,
    QString elab1, bool param1IsClimate, QString param1ClimateField, float param1, QString elab2, float param2, bool isAnomaly,
    int nYearsMin, int firstYearClimate, int lastYearClimate)
{


    bool pointOrGrid = 0; // meteoPoint

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

//                        If ElaborationOnPoint(meteoPoint(i).Point, pointOrGrid, variable, _
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



    bool pointOrGrid = 1; // grid
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

//                            if  ( !elaborationOnPoint(&(meteoGridDbHandler->meteoGrid()->meteoPoint(row,col)), pointOrGrid, variable,
//                                elab1, currentParameter1, elab2, param2, startDate, endDate, nYears, firstYear, lastYear,
//                                nYearsMin, isAnomaly, true))
//                            {
//                                // LC se qualche problema su una cella, deve interrompere e ritornare false oppure no?
//                                return false;
//                            }

            }

        }
    }

    return true;

}




bool elaborationOnPoint(std::string *myError, Crit3DMeteoGridDbHandler* meteoGridDbHandler, Crit3DMeteoPointsDbHandler* meteoPointsDbHandler, Crit3DMeteoPoint* meteoPoint, bool pointOrGrid, meteoVariable variable, QString elab1, float param1,
    QString elab2, float param2, QDate startDate, QDate endDate, int nYears, int firstYear, int lastYear,
    int nYearsMin, bool isAnomaly, bool loadData)
{


//    Dim perc_data As Single
    bool dataLoaded;
    float result;

    if (!isAnomaly || meteoPoint->anomaly != NODATA)
    {

            dataLoaded = false;

            if (loadData)
            {
//                dataLoaded = Elaboration.preElaboration(pointOrGrid, variable, elab1, myPoint, _
//                   startDate, endDate, perc_data)
            }
            else
            {
                // ricordarsi di usare imbuto se loadData è falso
                dataLoaded = true;
            }

            if (dataLoaded)
            {
//                result = Elaboration.ComputeStatistic(myVar, firstYear, lastYear, startDate, endDate, nYears, elab1, param1, elab2, param2, .z)

                if (isAnomaly)
                {
//                    ElaborationOnPoint = AnomalyOnPoint(myPoint, result)
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


std::vector<float> loadDailyVarSeries(std::string *myError, Crit3DMeteoPointsDbHandler* meteoPointsDbHandler, Crit3DMeteoGridDbHandler meteoGridDbHandler, Crit3DMeteoPoint* meteoPoint, bool pointOrGrid, meteoVariable variable, QDate first, QDate last, bool saveValue)
{
    std::vector<float> dailyValues;
    QDate firstDateDB;
    //grid
    if (pointOrGrid)
    {
        if (meteoGridDbHandler.gridStructure().isFixedFields())
        {
            dailyValues = meteoGridDbHandler.loadGridDailyVarFixedFields(myError, QString::fromStdString(meteoPoint->id), variable, first, last, &firstDateDB);
        }
        else
        {
            dailyValues = meteoGridDbHandler.loadGridDailyVar(myError, QString::fromStdString(meteoPoint->id), variable, first, last, &firstDateDB);
        }
        if (saveValue)
        {
            // LC la serie deve iniziare con la prima data utile oppure con la data richeista e quindi NODATA se non c'è il valore corrispondente a tale dato=
            //int numberOfDays = first.daysTo(last) + 1;
            int numberOfDays = firstDateDB.daysTo(last) + 1;
            int row, col;
            int initialize = 1;
            meteoGridDbHandler.meteoGrid()->findMeteoPointFromId(&row, &col, meteoPoint->id);
            for (int i = 0; i < dailyValues.size(); i++)
            {
                meteoGridDbHandler.meteoGrid()->fillMeteoPointDailyValue(row, col, numberOfDays, initialize, Crit3DDate(firstDateDB.day(), firstDateDB.month(), firstDateDB.year()), variable, dailyValues[i]) ;
                initialize = 0;
                firstDateDB.addDays(1);
            }
        }
    }
    // meteoPoint
    else
    {
        dailyValues = meteoPointsDbHandler->getDailyVar(myError, variable, getCrit3DDate(first), getCrit3DDate(last), &firstDateDB, meteoPoint );
        if (saveValue)
        {
            for (int i = 0; i < dailyValues.size(); i++)
            {
                meteoPoint->setMeteoPointValueD(Crit3DDate(firstDateDB.day(), firstDateDB.month(), firstDateDB.year()), variable, dailyValues[i]);
                firstDateDB.addDays(1);
            }
        }
    }
    return dailyValues;

}

std::vector<float> loadHourlyVarSeries(std::string *myError, Crit3DMeteoPointsDbHandler* meteoPointsDbHandler, Crit3DMeteoGridDbHandler meteoGridDbHandler, Crit3DMeteoPoint* meteoPoint, bool pointOrGrid, meteoVariable variable, QDateTime first, QDateTime last, bool saveValue)
{
    std::vector<float> hourlyValues;
    QDateTime firstDateDB;
    //grid
    if (pointOrGrid)
    {
        if (meteoGridDbHandler.gridStructure().isFixedFields())
        {
            hourlyValues = meteoGridDbHandler.loadGridHourlyVarFixedFields(myError, QString::fromStdString(meteoPoint->id), variable, first, last, &firstDateDB);
        }
        else
        {
            hourlyValues = meteoGridDbHandler.loadGridHourlyVar(myError, QString::fromStdString(meteoPoint->id), variable, first, last, &firstDateDB);
        }
        if (saveValue)
        {
            // LC la serie deve iniziare con la prima data utile oppure con la data richeista e quindi NODATA se non c'è il valore corrispondente a tale dato=
            //int numberOfDays = first.date().daysTo(last.date());
            int numberOfDays = firstDateDB.date().daysTo(last.date());
            int row, col;
            int initialize = 1;
            meteoGridDbHandler.meteoGrid()->findMeteoPointFromId(&row, &col, meteoPoint->id);
            for (int i = 0; i < hourlyValues.size(); i++)
            {
                meteoGridDbHandler.meteoGrid()->fillMeteoPointHourlyValue(row, col, numberOfDays, initialize,  Crit3DDate(firstDateDB.date().day(), firstDateDB.date().month(), firstDateDB.date().year()), firstDateDB.time().hour(), firstDateDB.time().minute(), variable, hourlyValues[i]) ;
                initialize = 0;
                firstDateDB.addSecs(3600);
            }
        }
    }
    // meteoPoint
    else
    {
        hourlyValues = meteoPointsDbHandler->getHourlyVar(myError, variable, getCrit3DDate(first.date()), getCrit3DDate(last.date()), &firstDateDB, meteoPoint );
        if (saveValue)
        {
            for (int i = 0; i < hourlyValues.size(); i++)
            {
                meteoPoint->setMeteoPointValueH(Crit3DDate(firstDateDB.date().day(), firstDateDB.date().month(), firstDateDB.date().year()), firstDateDB.time().hour(), firstDateDB.time().minute(), variable, hourlyValues[i]);
                firstDateDB.addSecs(3600);
            }
        }
    }
    return hourlyValues;

}


// calcola Thom giornaliero usando Tmax e RHmin per un punto
float thomDayTime(float tempMax, float relHumMinAir)
{

    Crit3DQuality qualityCheck;
    quality::type qualityT = qualityCheck.syntacticQualityControlSingleVal(dailyAirTemperatureMax, tempMax);
    quality::type qualityRelHumMinAir = qualityCheck.syntacticQualityControlSingleVal(dailyAirHumidityMin, relHumMinAir);

    // LC WrongValueDaily non ho trovato corrispondente funzione in quality, obsoleto o ancora da implementare?
    if ( qualityT == quality::accepted && qualityRelHumMinAir == quality::accepted )
    {
            return thom(tempMax, relHumMinAir);
    }
    else
        return NODATA;

}

// calcola Thom giornaliero usando Tmin e RHmax per un punto
float thomNightTime(float tempMin, float relHumMaxAir)
{

    Crit3DQuality qualityCheck;
    quality::type qualityT = qualityCheck.syntacticQualityControlSingleVal(dailyAirTemperatureMin, tempMin);
    quality::type qualityRelHumMaxAir = qualityCheck.syntacticQualityControlSingleVal(dailyAirHumidityMax, relHumMaxAir);

    // LC WrongValueDaily non ho trovato corrispondente funzione in quality, obsoleto o ancora da implementare?
    if ( qualityT == quality::accepted && qualityRelHumMaxAir == quality::accepted )
    {
            return thom(tempMin, relHumMaxAir);
    }
    else
        return NODATA;

}
/*
bool elaborateDailyAggregatedVar(ByVal var As String, ByRef myPoint As Tpoint, ByRef perc_value As Single)
{

Dim aggregationFrequency As Byte

    elaborateDailyAggregatedVar = False

    aggregationFrequency = passaggioDati.getAggregationFrequency(var)

    Select Case aggregationFrequency
        Case Definitions.HOURLY
            elaborateDailyAggregatedVar = elaborateDailyAggregatedVarFromHourly(var, myPoint)
        Case Definitions.DAILY
            elaborateDailyAggregatedVar = elaborateDailyAggregatedVarFromDaily(var, myPoint, perc_value)
    End Select

}
*/

frequencyType getAggregationFrequency(QString elab)
{

    if (elab == ELABORATION_THOM_DAILYHOURSABOVE || elab == ELABORATION_THOM_DAILYMEAN || elab == ELABORATION_THOM_DAILYMAX || elab == DAILY_LEAFWETNESS)
    {
        return hourly;
    }
    else if (elab != "")
    {
        return daily;
    }
    else
    {
        return noFrequency;
    }

}


bool preElaboration(Crit3DMeteoGridDbHandler* meteoGridDbHandler, Crit3DMeteoPointsDbHandler* meteoPointsDbHandler, Crit3DMeteoPoint* meteoPoint, bool pointOrGrid, meteoVariable variable, QString elab1,
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
/*
    preElaboration = false;

    switch (variable) {
    case (variable = dailyLeafWetness) :

        break;
    default:
        break;
    }

    Case variable


        Case Definitions.DAILY_LEAFWETNESS
            If passaggioDati.LoadGenericHourlySeries(pointOrGrid, Definitions.HOURLY_LEAFWETNESS, myPoint, startDate, endDate) > 0 Then
                passaggioDati.InizializzaMbutoOrario startDate, endDate
                passaggioDati.MbutoInversoOrario Definitions.HOURLY_LEAFWETNESS
                preElaboration = Elaboration.elaborateDailyAggregatedVar(variable, myPoint, percValue)
            End If

            If passaggioDati.LoadGenericHourlySeries(pointOrGrid, Definitions.HOURLY_LEAFWETNESS, myPoint, startDate, endDate) > 0 Then
                passaggioDati.MbutoHourly Definitions.HOURLY_LEAFWETNESS, currentHourlySeries, myPoint.z
                preElaboration = Elaboration.elaborateDailyAggregatedVar(Definitions.DAILY_LEAFWETNESS, myPoint, percValue)
            End If

        Case Definitions.ELABORATION_THOM_DAYTIME
            If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_RHmin, myPoint, startDate, endDate) > 0 Then
                    preElaboration = True
            End If
            If preElaboration Then
                preElaboration = False
                passaggioDati.InizializzaMbuto startDate, endDate
                passaggioDati.MbutoInversoGiornaliero Definitions.DAILY_RHmin
                If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_TMAX, myPoint, startDate, endDate) > 0 Then
                        preElaboration = True
                End If
                If preElaboration Then
                    passaggioDati.MbutoInversoGiornaliero Definitions.DAILY_TMAX
                    preElaboration = Elaboration.elaborateDailyAggregatedVar(Definitions.ELABORATION_THOM_DAYTIME, myPoint, percValue)
                End If
            End If

        Case Definitions.ELABORATION_THOM_NIGHTTIME
            If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_RHmax, myPoint, startDate, endDate) > 0 Then
                    preElaboration = True
            End If
            If preElaboration Then
                preElaboration = False
                passaggioDati.InizializzaMbuto startDate, endDate
                passaggioDati.MbutoInversoGiornaliero Definitions.DAILY_RHmax
                If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_TMIN, myPoint, startDate, endDate) > 0 Then
                        preElaboration = True
                End If
                If preElaboration Then
                    passaggioDati.MbutoInversoGiornaliero Definitions.DAILY_TMIN
                    preElaboration = Elaboration.elaborateDailyAggregatedVar(Definitions.ELABORATION_THOM_NIGHTTIME, myPoint, percValue)
                End If
            End If

        Case Definitions.ELABORATION_THOM_DAILYMEAN, Definitions.ELABORATION_THOM_DAILYMAX, Definitions.ELABORATION_THOM_DAILYHOURSABOVE
            If passaggioDati.LoadGenericHourlySeries(pointOrGrid, Definitions.HOURLY_TAVG, myPoint, startDate, endDate) > 0 Then
                passaggioDati.InizializzaMbutoOrario startDate, endDate
                passaggioDati.MbutoInversoOrario Definitions.HOURLY_TAVG
                If passaggioDati.LoadGenericHourlySeries(pointOrGrid, Definitions.HOURLY_RHAVG, myPoint, startDate, endDate) > 0 Then
                    passaggioDati.MbutoInversoOrario Definitions.HOURLY_RHAVG
                    preElaboration = Elaboration.elaborateDailyAggregatedVar(variable, myPoint, percValue)
                End If
            End If

        Case Definitions.DAILY_BIC
            If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_ETP, myPoint, startDate, endDate) > 0 Then
                    preElaboration = True
            ElseIf Environment.AutomaticETP Then
                If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_TMIN, myPoint, startDate, endDate) > 0 Then
                        preElaboration = True
                End If
                If preElaboration Then
                    preElaboration = False
                    passaggioDati.InizializzaMbuto startDate, endDate
                    passaggioDati.MbutoInversoGiornaliero Definitions.DAILY_TMIN
                    If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_TMAX, myPoint, startDate, endDate) > 0 Then
                            preElaboration = True
                    End If
                    If preElaboration Then
                        passaggioDati.MbutoInversoGiornaliero Definitions.DAILY_TMAX
                        preElaboration = Elaboration.elaborateDailyAggregatedVar(Definitions.DAILY_ETP, myPoint, percValue)
                    End If
                End If
            End If
            If preElaboration Then
                preElaboration = False
                passaggioDati.InizializzaMbuto startDate, endDate
                passaggioDati.MbutoInversoGiornaliero Definitions.DAILY_ETP
                If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_PREC, myPoint, startDate, endDate) > 0 Then
                        preElaboration = True
                End If
                If preElaboration Then
                    passaggioDati.MbutoInversoGiornaliero Definitions.DAILY_PREC
                    preElaboration = Elaboration.elaborateDailyAggregatedVar(Definitions.DAILY_BIC, myPoint, percValue)
                End If
            End If

        Case Definitions.DAILY_TEMPERATURE_RANGE
            If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_TMIN, myPoint, startDate, endDate) > 0 Then
                    preElaboration = True
            End If
            If preElaboration Then
                preElaboration = False
                passaggioDati.InizializzaMbuto startDate, endDate
                passaggioDati.MbutoInversoGiornaliero Definitions.DAILY_TMIN
                If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_TMAX, myPoint, startDate, endDate) > 0 Then
                        preElaboration = True
                End If
                If preElaboration Then
                    passaggioDati.MbutoInversoGiornaliero Definitions.DAILY_TMAX
                    preElaboration = Elaboration.elaborateDailyAggregatedVar(Definitions.DAILY_TEMPERATURE_RANGE, myPoint, percValue)
                End If
            End If

        Case Definitions.DAILY_TDMAX
            If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_TMAX, myPoint, startDate, endDate) > 0 Then
                    preElaboration = True
            End If
            If preElaboration Then
                preElaboration = False
                passaggioDati.InizializzaMbuto startDate, endDate
                passaggioDati.MbutoInversoGiornaliero Definitions.DAILY_TMAX
                If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_RHmin, myPoint, startDate, endDate) > 0 Then
                        preElaboration = True
                End If
                If preElaboration Then
                    passaggioDati.MbutoInversoGiornaliero Definitions.DAILY_RHmin
                    preElaboration = Elaboration.elaborateDailyAggregatedVar(Definitions.DAILY_TDMAX, myPoint, percValue)
                End If
            End If

        Case Definitions.DAILY_TDMIN
            If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_TMIN, myPoint, startDate, endDate) > 0 Then
                    preElaboration = True
            End If
            If preElaboration Then
                preElaboration = False
                passaggioDati.InizializzaMbuto startDate, endDate
                passaggioDati.MbutoInversoGiornaliero Definitions.DAILY_TMIN
                If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_RHmax, myPoint, startDate, endDate) > 0 Then
                        preElaboration = True
                End If
                If preElaboration Then
                    passaggioDati.MbutoInversoGiornaliero Definitions.DAILY_RHmax
                    preElaboration = Elaboration.elaborateDailyAggregatedVar(Definitions.DAILY_TDMIN, myPoint, percValue)
                End If
            End If

        Case Definitions.DAILY_TAVG
            percValue = passaggioDati.loadDailyVarSeries(pointOrGrid, DAILY_TAVG, myPoint, startDate, endDate)
            If percValue > 0 Then
                    preElaboration = True

            ElseIf Environment.AutomaticTmed Then
                If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_TMIN, myPoint, startDate, endDate) > 0 Then
                        preElaboration = True
                End If
                If preElaboration Then
                    preElaboration = False
                    passaggioDati.InizializzaMbuto startDate, endDate
                    passaggioDati.MbutoInversoGiornaliero Definitions.DAILY_TMIN
                    If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_TMAX, myPoint, startDate, endDate) > 0 Then
                            preElaboration = True
                    End If
                    If preElaboration Then
                        passaggioDati.MbutoInversoGiornaliero Definitions.DAILY_TMAX
                        preElaboration = Elaboration.elaborateDailyAggregatedVar(Definitions.DAILY_TAVG, myPoint, percValue)
                    End If
                End If
            End If

        Case Definitions.DAILY_ETP
            percValue = passaggioDati.loadDailyVarSeries(pointOrGrid, DAILY_ETP, myPoint, startDate, endDate)
            If percValue > 0 The
                    preElaboration = True

            ElseIf Environment.AutomaticETP Then
                If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_TMIN, myPoint, startDate, endDate) > 0 Then
                        preElaboration = True
                End If
                If preElaboration Then
                    preElaboration = False
                    passaggioDati.InizializzaMbuto startDate, endDate
                    passaggioDati.MbutoInversoGiornaliero Definitions.DAILY_TMIN
                    If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_TMAX, myPoint, startDate, endDate) > 0 Then
                            preElaboration = True
                    End If
                    If preElaboration Then
                        passaggioDati.MbutoInversoGiornaliero Definitions.DAILY_TMAX
                        preElaboration = Elaboration.elaborateDailyAggregatedVar(Definitions.DAILY_ETP, myPoint, percValue)
                    End If
                End If
            End If

        Case Else

            Select Case elab1

                Case Definitions.ELABORATION_HUGLIN
                    If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_TMIN, myPoint, startDate, endDate) > 0 Then
                            preElaboration = True
                    End If
                    If preElaboration Then
                        preElaboration = False
                        passaggioDati.InizializzaMbuto startDate, endDate
                        passaggioDati.MbutoInversoGiornaliero Definitions.DAILY_TMIN
                        If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_TMAX, myPoint, startDate, endDate) > 0 Then
                                preElaboration = True
                        End If
                        If preElaboration Then
                            preElaboration = False
                            passaggioDati.MbutoInversoGiornaliero Definitions.DAILY_TMAX
                            If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_TAVG, myPoint, startDate, endDate) > 0 Then
                                preElaboration = True
                            ElseIf Environment.AutomaticTmed Then
                                preElaboration = Elaboration.elaborateDailyAggregatedVar(Definitions.DAILY_TAVG, myPoint, percValue)
                            End If
                        End If
                    End If

                Case Definitions.ELABORATION_WINKLER, Definitions.ELABORATION_CORRECTED_SUM, Definitions.ELABORATION_FREGONI

                    If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_TMIN, myPoint, startDate, endDate) > 0 Then
                            preElaboration = True
                    End If
                    If preElaboration Then
                        preElaboration = False
                        passaggioDati.InizializzaMbuto startDate, endDate
                        passaggioDati.MbutoInversoGiornaliero Definitions.DAILY_TMIN
                        If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_TMAX, myPoint, startDate, endDate) > 0 Then
                                preElaboration = True
                        End If
                        If preElaboration Then passaggioDati.MbutoInversoGiornaliero Definitions.DAILY_TMAX
                    End If

                Case Definitions.ELABORATION_PHENO
                    If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_TMIN, myPoint, startDate, endDate) > 0 Then
                            preElaboration = True
                    End If
                    If preElaboration Then
                        preElaboration = False
                        passaggioDati.InizializzaMbuto startDate, endDate
                        passaggioDati.MbutoInversoGiornaliero Definitions.DAILY_TMIN
                        If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_TMAX, myPoint, startDate, endDate) > 0 Then
                                preElaboration = True
                        End If
                        If preElaboration Then
                            preElaboration = False
                            passaggioDati.MbutoInversoGiornaliero Definitions.DAILY_TMAX
                            If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_PREC, myPoint, startDate, endDate) > 0 Then
                                    preElaboration = True
                            End If
                        End If
                    End If

                Case Else

                    percValue = passaggioDati.loadDailyVarSeries(pointOrGrid, variable, myPoint, startDate, endDate)

                    If percValue > 0 Then
                            preElaboration = True
                    End If

            End Select

    End Select
*/
}



#include "climate.h"


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




bool elaborationOnPoint(std::string *myError, Crit3DMeteoGridDbHandler* meteoGridDbHandler, Crit3DMeteoPoint* meteoPoint, bool pointOrGrid, meteoVariable variable, QString elab1, float param1,
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
//                dataLoaded = Elaboration.PreElaboration(pointOrGrid, variable, elab1, myPoint, _
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


std::vector<float> loadDailyVarSeries(std::string *myError, Crit3DMeteoPoint meteoPoint, Crit3DMeteoGridDbHandler meteoGridDbHandler, bool pointOrGrid, meteoVariable variable, QDate first, QDate last)
{
    std::vector<float> dailyValues;
    //grid
    if (pointOrGrid)
    {
        QDate firstDateDB;
        if (meteoGridDbHandler.gridStructure().isFixedFields())
        {
            dailyValues = meteoGridDbHandler.loadGridDailyVarFixedFields(myError, QString::fromStdString(meteoPoint.id), variable, first, last, &firstDateDB);
        }
        else
        {
            dailyValues = meteoGridDbHandler.loadGridDailyVar(myError, QString::fromStdString(meteoPoint.id), variable, first, last, &firstDateDB);
        }
    }
    // meteoPoint
    else
    {

    }
    return dailyValues;

}

std::vector<float> loadHourlyVarSeries(std::string *myError, Crit3DMeteoPoint meteoPoint, Crit3DMeteoGridDbHandler meteoGridDbHandler, bool pointOrGrid, meteoVariable variable, QDateTime first, QDateTime last)
{
    std::vector<float> hourlyValues;
    //grid
    if (pointOrGrid)
    {
        QDateTime firstDateDB;
        if (meteoGridDbHandler.gridStructure().isFixedFields())
        {
            hourlyValues = meteoGridDbHandler.loadGridHourlyVarFixedFields(myError, QString::fromStdString(meteoPoint.id), variable, first, last, &firstDateDB);
        }
        else
        {
            hourlyValues = meteoGridDbHandler.loadGridHourlyVar(myError, QString::fromStdString(meteoPoint.id), variable, first, last, &firstDateDB);
        }
    }
    // meteoPoint
    else
    {

    }
    return hourlyValues;

}


/*
Public Function PreElaboration(ByVal pointOrGrid As Byte, ByVal Variabile As String, ByVal elab1 As String, _
    ByRef myPoint As Tpoint, _
    ByVal startDate As Date, ByVal finishDate As Date, ByRef perc_value As Single) As Boolean

    //// era in statistica
    'todo: spostare in preElaboration (quindi eliminare myVar)
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

    PreElaboration = False

    Select Case Variabile

        Case Definitions.DAILY_LEAFWETNESS
            If passaggioDati.LoadGenericHourlySeries(pointOrGrid, Definitions.HOURLY_LEAFWETNESS, myPoint, startDate, finishDate) > 0 Then
                passaggioDati.InizializzaMbutoOrario startDate, finishDate
                passaggioDati.MbutoInversoOrario Definitions.HOURLY_LEAFWETNESS
                PreElaboration = Elaboration.ElaborateDailyAggregatedVar(Variabile, myPoint, perc_value)
            End If

            If passaggioDati.LoadGenericHourlySeries(pointOrGrid, Definitions.HOURLY_LEAFWETNESS, myPoint, startDate, finishDate) > 0 Then
                passaggioDati.MbutoHourly Definitions.HOURLY_LEAFWETNESS, currentHourlySeries, myPoint.z
                PreElaboration = Elaboration.ElaborateDailyAggregatedVar(Definitions.DAILY_LEAFWETNESS, myPoint, perc_value)
            End If

        Case Definitions.ELABORATION_THOM_DAYTIME
            If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_RHmin, myPoint, startDate, finishDate) > 0 Then
                    PreElaboration = True
            End If
            If PreElaboration Then
                PreElaboration = False
                passaggioDati.InizializzaMbuto startDate, finishDate
                passaggioDati.MbutoInversoGiornaliero Definitions.DAILY_RHmin
                If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_TMAX, myPoint, startDate, finishDate) > 0 Then
                        PreElaboration = True
                End If
                If PreElaboration Then
                    passaggioDati.MbutoInversoGiornaliero Definitions.DAILY_TMAX
                    PreElaboration = Elaboration.ElaborateDailyAggregatedVar(Definitions.ELABORATION_THOM_DAYTIME, myPoint, perc_value)
                End If
            End If

        Case Definitions.ELABORATION_THOM_NIGHTTIME
            If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_RHmax, myPoint, startDate, finishDate) > 0 Then
                    PreElaboration = True
            End If
            If PreElaboration Then
                PreElaboration = False
                passaggioDati.InizializzaMbuto startDate, finishDate
                passaggioDati.MbutoInversoGiornaliero Definitions.DAILY_RHmax
                If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_TMIN, myPoint, startDate, finishDate) > 0 Then
                        PreElaboration = True
                End If
                If PreElaboration Then
                    passaggioDati.MbutoInversoGiornaliero Definitions.DAILY_TMIN
                    PreElaboration = Elaboration.ElaborateDailyAggregatedVar(Definitions.ELABORATION_THOM_NIGHTTIME, myPoint, perc_value)
                End If
            End If

        Case Definitions.ELABORATION_THOM_DAILYMEAN, Definitions.ELABORATION_THOM_DAILYMAX, Definitions.ELABORATION_THOM_DAILYHOURSABOVE
            If passaggioDati.LoadGenericHourlySeries(pointOrGrid, Definitions.HOURLY_TAVG, myPoint, startDate, finishDate) > 0 Then
                passaggioDati.InizializzaMbutoOrario startDate, finishDate
                passaggioDati.MbutoInversoOrario Definitions.HOURLY_TAVG
                If passaggioDati.LoadGenericHourlySeries(pointOrGrid, Definitions.HOURLY_RHAVG, myPoint, startDate, finishDate) > 0 Then
                    passaggioDati.MbutoInversoOrario Definitions.HOURLY_RHAVG
                    PreElaboration = Elaboration.ElaborateDailyAggregatedVar(Variabile, myPoint, perc_value)
                End If
            End If

        Case Definitions.DAILY_BIC
            If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_ETP, myPoint, startDate, finishDate) > 0 Then
                    PreElaboration = True
            ElseIf Environment.AutomaticETP Then
                If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_TMIN, myPoint, startDate, finishDate) > 0 Then
                        PreElaboration = True
                End If
                If PreElaboration Then
                    PreElaboration = False
                    passaggioDati.InizializzaMbuto startDate, finishDate
                    passaggioDati.MbutoInversoGiornaliero Definitions.DAILY_TMIN
                    If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_TMAX, myPoint, startDate, finishDate) > 0 Then
                            PreElaboration = True
                    End If
                    If PreElaboration Then
                        passaggioDati.MbutoInversoGiornaliero Definitions.DAILY_TMAX
                        PreElaboration = Elaboration.ElaborateDailyAggregatedVar(Definitions.DAILY_ETP, myPoint, perc_value)
                    End If
                End If
            End If
            If PreElaboration Then
                PreElaboration = False
                passaggioDati.InizializzaMbuto startDate, finishDate
                passaggioDati.MbutoInversoGiornaliero Definitions.DAILY_ETP
                If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_PREC, myPoint, startDate, finishDate) > 0 Then
                        PreElaboration = True
                End If
                If PreElaboration Then
                    passaggioDati.MbutoInversoGiornaliero Definitions.DAILY_PREC
                    PreElaboration = Elaboration.ElaborateDailyAggregatedVar(Definitions.DAILY_BIC, myPoint, perc_value)
                End If
            End If

        Case Definitions.DAILY_TEMPERATURE_RANGE
            If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_TMIN, myPoint, startDate, finishDate) > 0 Then
                    PreElaboration = True
            End If
            If PreElaboration Then
                PreElaboration = False
                passaggioDati.InizializzaMbuto startDate, finishDate
                passaggioDati.MbutoInversoGiornaliero Definitions.DAILY_TMIN
                If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_TMAX, myPoint, startDate, finishDate) > 0 Then
                        PreElaboration = True
                End If
                If PreElaboration Then
                    passaggioDati.MbutoInversoGiornaliero Definitions.DAILY_TMAX
                    PreElaboration = Elaboration.ElaborateDailyAggregatedVar(Definitions.DAILY_TEMPERATURE_RANGE, myPoint, perc_value)
                End If
            End If

        Case Definitions.DAILY_TDMAX
            If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_TMAX, myPoint, startDate, finishDate) > 0 Then
                    PreElaboration = True
            End If
            If PreElaboration Then
                PreElaboration = False
                passaggioDati.InizializzaMbuto startDate, finishDate
                passaggioDati.MbutoInversoGiornaliero Definitions.DAILY_TMAX
                If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_RHmin, myPoint, startDate, finishDate) > 0 Then
                        PreElaboration = True
                End If
                If PreElaboration Then
                    passaggioDati.MbutoInversoGiornaliero Definitions.DAILY_RHmin
                    PreElaboration = Elaboration.ElaborateDailyAggregatedVar(Definitions.DAILY_TDMAX, myPoint, perc_value)
                End If
            End If

        Case Definitions.DAILY_TDMIN
            If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_TMIN, myPoint, startDate, finishDate) > 0 Then
                    PreElaboration = True
            End If
            If PreElaboration Then
                PreElaboration = False
                passaggioDati.InizializzaMbuto startDate, finishDate
                passaggioDati.MbutoInversoGiornaliero Definitions.DAILY_TMIN
                If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_RHmax, myPoint, startDate, finishDate) > 0 Then
                        PreElaboration = True
                End If
                If PreElaboration Then
                    passaggioDati.MbutoInversoGiornaliero Definitions.DAILY_RHmax
                    PreElaboration = Elaboration.ElaborateDailyAggregatedVar(Definitions.DAILY_TDMIN, myPoint, perc_value)
                End If
            End If

        Case Definitions.DAILY_TAVG
            perc_value = passaggioDati.loadDailyVarSeries(pointOrGrid, DAILY_TAVG, myPoint, startDate, finishDate)
            If perc_value > 0 Then
                    PreElaboration = True

            ElseIf Environment.AutomaticTmed Then
                If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_TMIN, myPoint, startDate, finishDate) > 0 Then
                        PreElaboration = True
                End If
                If PreElaboration Then
                    PreElaboration = False
                    passaggioDati.InizializzaMbuto startDate, finishDate
                    passaggioDati.MbutoInversoGiornaliero Definitions.DAILY_TMIN
                    If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_TMAX, myPoint, startDate, finishDate) > 0 Then
                            PreElaboration = True
                    End If
                    If PreElaboration Then
                        passaggioDati.MbutoInversoGiornaliero Definitions.DAILY_TMAX
                        PreElaboration = Elaboration.ElaborateDailyAggregatedVar(Definitions.DAILY_TAVG, myPoint, perc_value)
                    End If
                End If
            End If

        Case Definitions.DAILY_ETP
            perc_value = passaggioDati.loadDailyVarSeries(pointOrGrid, DAILY_ETP, myPoint, startDate, finishDate)
            If perc_value > 0 The
                    PreElaboration = True

            ElseIf Environment.AutomaticETP Then
                If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_TMIN, myPoint, startDate, finishDate) > 0 Then
                        PreElaboration = True
                End If
                If PreElaboration Then
                    PreElaboration = False
                    passaggioDati.InizializzaMbuto startDate, finishDate
                    passaggioDati.MbutoInversoGiornaliero Definitions.DAILY_TMIN
                    If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_TMAX, myPoint, startDate, finishDate) > 0 Then
                            PreElaboration = True
                    End If
                    If PreElaboration Then
                        passaggioDati.MbutoInversoGiornaliero Definitions.DAILY_TMAX
                        PreElaboration = Elaboration.ElaborateDailyAggregatedVar(Definitions.DAILY_ETP, myPoint, perc_value)
                    End If
                End If
            End If

        Case Else

            Select Case elab1

                Case Definitions.ELABORATION_HUGLIN
                    If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_TMIN, myPoint, startDate, finishDate) > 0 Then
                            PreElaboration = True
                    End If
                    If PreElaboration Then
                        PreElaboration = False
                        passaggioDati.InizializzaMbuto startDate, finishDate
                        passaggioDati.MbutoInversoGiornaliero Definitions.DAILY_TMIN
                        If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_TMAX, myPoint, startDate, finishDate) > 0 Then
                                PreElaboration = True
                        End If
                        If PreElaboration Then
                            PreElaboration = False
                            passaggioDati.MbutoInversoGiornaliero Definitions.DAILY_TMAX
                            If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_TAVG, myPoint, startDate, finishDate) > 0 Then
                                PreElaboration = True
                            ElseIf Environment.AutomaticTmed Then
                                PreElaboration = Elaboration.ElaborateDailyAggregatedVar(Definitions.DAILY_TAVG, myPoint, perc_value)
                            End If
                        End If
                    End If

                Case Definitions.ELABORATION_WINKLER, Definitions.ELABORATION_CORRECTED_SUM, Definitions.ELABORATION_FREGONI

                    If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_TMIN, myPoint, startDate, finishDate) > 0 Then
                            PreElaboration = True
                    End If
                    If PreElaboration Then
                        PreElaboration = False
                        passaggioDati.InizializzaMbuto startDate, finishDate
                        passaggioDati.MbutoInversoGiornaliero Definitions.DAILY_TMIN
                        If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_TMAX, myPoint, startDate, finishDate) > 0 Then
                                PreElaboration = True
                        End If
                        If PreElaboration Then passaggioDati.MbutoInversoGiornaliero Definitions.DAILY_TMAX
                    End If

                Case Definitions.ELABORATION_PHENO
                    If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_TMIN, myPoint, startDate, finishDate) > 0 Then
                            PreElaboration = True
                    End If
                    If PreElaboration Then
                        PreElaboration = False
                        passaggioDati.InizializzaMbuto startDate, finishDate
                        passaggioDati.MbutoInversoGiornaliero Definitions.DAILY_TMIN
                        If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_TMAX, myPoint, startDate, finishDate) > 0 Then
                                PreElaboration = True
                        End If
                        If PreElaboration Then
                            PreElaboration = False
                            passaggioDati.MbutoInversoGiornaliero Definitions.DAILY_TMAX
                            If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_PREC, myPoint, startDate, finishDate) > 0 Then
                                    PreElaboration = True
                            End If
                        End If
                    End If

                Case Else

                    perc_value = passaggioDati.loadDailyVarSeries(pointOrGrid, Variabile, myPoint, startDate, finishDate)

                    If perc_value > 0 Then
                            PreElaboration = True
                    End If

            End Select

    End Select

End Function

*/

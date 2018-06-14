#include "climate.h"


bool elaborationPointsCycle(std::string *myError, Crit3DMeteoPointsDbHandler* meteoPointsDbHandler, QString variable, int firstYear, int lastYear, QDate firstDate, QDate lastDate, int nYears,
    QString elab1, bool param1IsClimate, QString param1ClimateField, float param1, QString elab2, float param2, bool isAnomaly,
    int nYearsMin, int firstYearClimate, int lastYearClimate)
{



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

//                        If ElaborationOnPoint(meteoPoint(i).Point, pointOrGrid, Variabile, _
//                            elab1, currentParameter1, elab2, Parameter2, _
//                            startDate, endDate, nYears, firstYear, lastYear, _
//                            nYearsMin, _
//                            isAnomaly, True) Then ElaborationPointsCycle = True

//                    End If
//                Next i
    return true;

}




bool elaborationPointsCycleGrid(std::string *myError, Crit3DMeteoGridDbHandler* meteoGridDbHandler, QString variable, int firstYear, int lastYear, QDate firstDate, QDate lastDate, int nYears,
    QString elab1, bool param1IsClimate, QString param1ClimateField, float param1, QString elab2, float param2, bool isAnomaly,
    int nYearsMin, int firstYearClimate, int lastYearClimate)
{


//    Dim result As Single
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

//                            If elaborationOnPoint(MeteoGrid.Point(row, col), pointOrGrid, Variabile, _
//                                elab1, currentParameter1, elab2, Parameter2, _
//                                startDate, endDate, nYears, firstYear, lastYear, _
//                                nYearsMin, isAnomaly, True) Then ElaborationPointsCycle = True

            }

        }
    }

    return true;

}




bool elaborationOnPoint(Crit3DMeteoPoint* meteoPoint, bool pointOrGrid, QString variable, QString elab1, float param1,
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
//                dataLoaded = Elaboration.PreElaboration(pointOrGrid, myVar, elab1, myPoint, _
//                   startDate, endDate, _
//                   dateScenarioIni, dateScenarioFin, _
//                   isScenario, numAnniMin, perc_data)
            }
            else
            {
                // ricordarsi di usare imbuto se loadData è falso
                dataLoaded = true;
            }

            if (dataLoaded)
            {
//                result = Elaboration.ComputeStatistic(myVar, firstYear, lastYear, startDate, endDate, _
//                nYears, elab1, param1, elab2, param2, .z)

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


#include "climate.h"


bool elaborationPointsCycle(std::string *myError, bool pointOrGrid, QString variable, int firstYear, int lastYear, QDate firstDate, QDate lastDate, int nYears,
    QString elab1, bool param1IsClimate, QString param1ClimateField, float param1, QString elab2, float param2, bool isAnomaly,
    int nYearsMin, int firstYearClimate, int lastYearClimate)
{



//    Dim row As Long, col As Long, i As Long
//    Dim result As Single
//    Dim dateScenarioIni As Date, dateScenarioFin As Date
//    Dim currentParameter1 As Single
//    Dim myStep As Integer


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


//    If param1IsClimate Then
//        Dim myPeriodType As Byte
//        Dim myClimateIndex As Integer
//        Dim myParam1ClimateField As String

//        myParam1ClimateField = Climate.getClimateFieldName(parameter1ClimateElab)
//        myPeriodType = Climate.parserElaborationOnlyPeriodType(parameter1ClimateElab)
//        myClimateIndex = Climate.getClimateIndexFromDate(currentDay, myPeriodType)
//    End If

//        Select Case dataType

//            Case Definitions.DATATYPE_GRID
//                If Not isDBMeteoGridLoaded Then
//                    PragaShell.setErrorMsg InfoMsg.ErrDBMeteoGridNotLoaded
//                    Exit Function
//                End If

//                If Environment.Praga_Environment = Environment.ENVRNMT_PRAGA_GUI Then Cmd.SelectMeteoCellsGUI

//                For col = 0 To MeteoGrid.structure.header.nrCols - 1
//                    For row = 0 To MeteoGrid.structure.header.nrRows - 1

//                        If MeteoGrid.Point(row, col).active And MeteoGrid.Point(row, col).selected Then

//                            If param1IsClimate Then
//                                If Climate.ClimateReadPoint(PragaClimate.Point(row, col).TableName, _
//                                                            myParam1ClimateField, _
//                                                            myPeriodType, _
//                                                            myClimateIndex, _
//                                                            PragaClimate.Point(row, col)) Then

//                                    currentParameter1 = passaggioDati.GetClimateData(myPeriodType, PragaClimate.Point(row, col), myClimateIndex)
//                                Else
//                                    currentParameter1 = Definitions.NO_DATA
//                                End If
//                            Else
//                                currentParameter1 = Parameter1
//                            End If

//                            If ElaborationOnPoint(MeteoGrid.Point(row, col), dataType, Variabile, _
//                                elab1, currentParameter1, elab2, Parameter2, _
//                                startDate, endDate, nYears, firstYear, lastYear, _
//                                isScenario, dateScenarioIni, dateScenarioFin, numAnniMin, _
//                                isAnomaly, True) Then ElaborationPointsCycle = True

//                        End If

//                    Next row
//                Next col


//            Case Definitions.DATATYPE_POINT

//                If Not isDBMeteoPointsLoaded Then
//                    MsgBox InfoMsg.ErrDBPointsNotLoaded
//                    Exit Function
//                End If

//                If Environment.Praga_Environment = Environment.ENVRNMT_PRAGA_GUI Then Cmd.SelectMeteoPointsGUI

//                For i = 0 To UBound(meteoPoint)

//                    If meteoPoint(i).Point.active And meteoPoint(i).Point.selected Then

//                        If param1IsClimate Then
//                            If Climate.ClimateReadPoint(PragaClimate.Point(i).TableName, _
//                                                        myParam1ClimateField, _
//                                                        myPeriodType, _
//                                                        myClimateIndex, _
//                                                        PragaClimate.Point(i)) Then

//                                currentParameter1 = passaggioDati.GetClimateData(myPeriodType, PragaClimate.Point(i), myClimateIndex)
//                            Else
//                                currentParameter1 = Definitions.NO_DATA
//                            End If
//                        Else
//                            currentParameter1 = Parameter1
//                        End If

//                        If elab1 = Definitions.ELABORATION_PHENO Then currentPheno.setPhenoPoint i

//                        If ElaborationOnPoint(meteoPoint(i).Point, dataType, Variabile, _
//                            elab1, currentParameter1, elab2, Parameter2, _
//                            startDate, endDate, nYears, firstYear, lastYear, _
//                            numAnniMin, _
//                            isAnomaly, True) Then ElaborationPointsCycle = True

//                    End If
//                Next i
//        End Select
    return true;

}

#include "pragaProject.h"
#include "project.h"
#include "climate.h"
#include "formInfo.h"

#include <iostream> //debug

bool PragaProject::getIsElabMeteoPointsValue() const
{
    return isElabMeteoPointsValue;
}

void PragaProject::setIsElabMeteoPointsValue(bool value)
{
    isElabMeteoPointsValue = value;
}

PragaProject::PragaProject()
{
    clima = new Crit3DClimate();
    climaFromDb = NULL;
    referenceClima = NULL;
}

bool PragaProject::readPragaParameters()
{
    Q_FOREACH (QString group, settings->childGroups())
    {
        if (group == "elaboration")
        {
            settings->beginGroup(group);
            Crit3DElaborationSettings* elabSettings = clima->getElabSettings();


            if (settings->contains("anomaly_pts_max_distance") && !settings->value("anomaly_pts_max_distance").toString().isEmpty())
            {
                elabSettings->setAnomalyPtsMaxDistance(settings->value("anomaly_pts_max_distance").toFloat());
            }
            if (settings->contains("anomaly_pts_max_delta_z") && !settings->value("anomaly_pts_max_delta_z").toString().isEmpty())
            {
                elabSettings->setAnomalyPtsMaxDeltaZ(settings->value("anomaly_pts_max_delta_z").toFloat());
            }
            if (settings->contains("grid_min_coverage") && !settings->value("grid_min_coverage").toString().isEmpty())
            {
                elabSettings->setGridMinCoverage(settings->value("grid_min_coverage").toFloat());
            }
            if (settings->contains("compute_tmed") && !settings->value("compute_tmed").toString().isEmpty())
            {
                elabSettings->setAutomaticTmed(settings->value("compute_tmed").toBool());
            }
            if (settings->contains("compute_et0hs") && !settings->value("compute_et0hs").toString().isEmpty())
            {
                elabSettings->setAutomaticETP(settings->value("compute_et0hs").toBool());
            }
            if (settings->contains("merge_joint_stations") && !settings->value("merge_joint_stations").toString().isEmpty())
            {
                elabSettings->setMergeJointStations(settings->value("merge_joint_stations").toBool());
            }

            settings->endGroup();

        }
    }

    return true;
}

bool PragaProject::interpolationMeteoGrid(meteoVariable myVar, frequencyType myFrequency, const Crit3DTime& myTime,
                              gis::Crit3DRasterGrid *myRaster, bool showInfo)
{

    if (meteoGridDbHandler != NULL)
    {
        if (!interpolationDem(myVar, myTime, myRaster, showInfo))
        {
            return false;
        }
        meteoGridDbHandler->meteoGrid()->aggregateMeteoGrid(myVar, myFrequency, myTime.date, myTime.getHour(), myTime.getMinutes(), &DTM, dataRaster, interpolationSettings.getMeteoGridAggrMethod());
        meteoGridDbHandler->meteoGrid()->fillMeteoRaster();
    }
    else
    {
        errorString = "No grid";
        return false;
    }
    return true;
}

bool PragaProject::saveGrid(meteoVariable myVar, frequencyType myFrequency, const Crit3DTime& myTime, bool showInfo)
{
    std::string id;
    FormInfo myInfo;
    int infoStep;


    if (myFrequency == daily)
    {
        if (showInfo)
        {
            QString infoStr = "Save grid daily data";
            infoStep = myInfo.start(infoStr, this->meteoGridDbHandler->gridStructure().header().nrRows);
        }

        for (int row = 0; row < this->meteoGridDbHandler->gridStructure().header().nrRows; row++)
        {
            if (showInfo && (row % infoStep) == 0)
                myInfo.setValue(row);
            for (int col = 0; col < this->meteoGridDbHandler->gridStructure().header().nrCols; col++)
            {
                if (this->meteoGridDbHandler->meteoGrid()->getMeteoPointActiveId(row, col, &id))
                {
                    if (!this->meteoGridDbHandler->gridStructure().isFixedFields())
                    {
                        this->meteoGridDbHandler->saveCellCurrrentGridDaily(&errorString, QString::fromStdString(id), QDate(myTime.date.year, myTime.date.month, myTime.date.day), this->meteoGridDbHandler->getDailyVarCode(myVar), this->meteoGridDbHandler->meteoGrid()->meteoPoint(row,col).currentValue);
                    }
                    else
                    {
                        this->meteoGridDbHandler->saveCellCurrentGridDailyFF(&errorString, QString::fromStdString(id), QDate(myTime.date.year, myTime.date.month, myTime.date.day), QString::fromStdString(this->meteoGridDbHandler->getDailyPragaName(myVar)), this->meteoGridDbHandler->meteoGrid()->meteoPoint(row,col).currentValue);
                    }
                }
            }
        }
    }
    else if (myFrequency == hourly)
    {
        if (showInfo)
        {
            QString infoStr = "Save grid hourly data";
            infoStep = myInfo.start(infoStr, this->meteoGridDbHandler->gridStructure().header().nrRows);
        }

        for (int row = 0; row < this->meteoGridDbHandler->gridStructure().header().nrRows; row++)
        {
            if (showInfo && (row % infoStep) == 0)
                myInfo.setValue(row);
            for (int col = 0; col < this->meteoGridDbHandler->gridStructure().header().nrCols; col++)
            {
                if (this->meteoGridDbHandler->meteoGrid()->getMeteoPointActiveId(row, col, &id))
                {
                    if (!this->meteoGridDbHandler->gridStructure().isFixedFields())
                    {
                        this->meteoGridDbHandler->saveCellCurrentGridHourly(&errorString, QString::fromStdString(id), QDateTime(QDate(myTime.date.year, myTime.date.month, myTime.date.day), QTime(myTime.getHour(), myTime.getMinutes(), myTime.getSeconds())), this->meteoGridDbHandler->getHourlyVarCode(myVar), this->meteoGridDbHandler->meteoGrid()->meteoPoint(row,col).currentValue);
                    }
                    else
                    {
                        this->meteoGridDbHandler->saveCellCurrentGridHourlyFF(&errorString, QString::fromStdString(id), QDateTime(QDate(myTime.date.year, myTime.date.month, myTime.date.day), QTime(myTime.getHour(), myTime.getMinutes(), myTime.getSeconds())), QString::fromStdString(this->meteoGridDbHandler->getHourlyPragaName(myVar)), this->meteoGridDbHandler->meteoGrid()->meteoPoint(row,col).currentValue);
                    }
                }
            }
        }
    }

    if (showInfo) myInfo.close();

    return true;
}

bool PragaProject::elaborationCheck(bool isMeteoGrid, bool isAnomaly)
{

    if (isMeteoGrid)
    {
        if (this->meteoGridDbHandler == NULL)
        {
            errorString = "Load grid";
            return false;
        }
        else
        {
            if (this->clima == NULL)
            {
                this->clima = new Crit3DClimate(); 
            }
            clima->setDb(this->meteoGridDbHandler->db());
        }
    }
    else
    {
        if (this->meteoPointsDbHandler == NULL)
        {
            errorString = "Load meteo Points";
            return false;
        }
        else
        {
            if (this->clima == NULL)
            {
                this->clima = new Crit3DClimate();
            }
            clima->setDb(this->meteoPointsDbHandler->getDb());
        }
    }
    if (isAnomaly)
    {
        if (this->referenceClima == NULL)
        {
            this->referenceClima = new Crit3DClimate();
        }
    }

    return true;
}


bool PragaProject::elaboration(bool isMeteoGrid, bool isAnomaly, bool saveClima)
{

    if (isMeteoGrid)
    {
        if (saveClima)
        {
            if (!climatePointsCycleGrid(true))
            {
                return false;
            }
            else
            {
                return true;
            }
        }
        if (!isAnomaly)
        {
            if (!elaborationPointsCycleGrid(isAnomaly, true))
            {
                return false;
            }
            meteoGridDbHandler->meteoGrid()->fillMeteoRasterElabValue();
        }
        else
        {
            clima = referenceClima;
            if (!elaborationPointsCycleGrid(isAnomaly, true))
            {
                return false;
            }
            meteoGridDbHandler->meteoGrid()->fillMeteoRasterAnomalyValue();
        }
        meteoGridDbHandler->meteoGrid()->setIsElabValue(true);
    }
    else
    {
        if (saveClima)
        {
            if (!climatePointsCycle(true))
            {
                return false;
            }
            else
            {
                return true;
            }
        }
        if (!isAnomaly)
        {
            if (!elaborationPointsCycle(isAnomaly, true))
            {
                return false;
            }
        }
        else
        {
            clima = referenceClima;
            if (!elaborationPointsCycle(isAnomaly, true))
            {
                return false;
            }
        }

        setIsElabMeteoPointsValue(true);
    }

    return true;
}

bool PragaProject::elaborationPointsCycle(bool isAnomaly, bool showInfo)
{

    bool isMeteoGrid = 0; // meteoPoint
    int validCell = 0;

    FormInfo myInfo;
    int infoStep;
    QString infoStr;

    errorString.clear();

    if (showInfo)
    {
        if (isAnomaly)
        {
            infoStr = "Anomaly - Meteo Points";
        }
        else
        {
            infoStr = "Elaboration - Meteo Points";
        }

        infoStep = myInfo.start(infoStr, nrMeteoPoints);
    }


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

//    if (clima->elab1() == "phenology")
//    {
//        Then currentPheno.setPhenoPoint i;  // TODO
//    }

    Crit3DMeteoPoint* meteoPointTemp = new Crit3DMeteoPoint;

    for (int i = 0; i < nrMeteoPoints; i++)
    {

        if (meteoPoints[i].active)
        {

            // copy data to MPTemp
            meteoPointTemp->id = meteoPoints[i].id;
            meteoPointTemp->point.z = meteoPoints[i].point.z;
            meteoPointTemp->latitude = meteoPoints[i].latitude;
            meteoPointTemp->elaboration = meteoPoints[i].elaboration;

            // meteoPointTemp should be init
            meteoPointTemp->nrObsDataDaysH = 0;
            meteoPointTemp->nrObsDataDaysD = 0;

            if (showInfo && (i % infoStep) == 0)
                        myInfo.setValue(i);


            if ( elaborationOnPoint(&errorString, meteoPointsDbHandler, NULL, meteoPointTemp, clima, isMeteoGrid, startDate, endDate, isAnomaly, meteoSettings))
            {
                validCell = validCell + 1;
            }

            // save result to MP
            meteoPoints[i].elaboration = meteoPointTemp->elaboration;
            meteoPoints[i].anomaly = meteoPointTemp->anomaly;
            meteoPoints[i].anomalyPercentage = meteoPointTemp->anomalyPercentage;

        }

    } // end for
    if (showInfo) myInfo.close();

    if (validCell == 0)
    {
        if (errorString.empty())
        {
            errorString = "no valid cells available";
        }
        delete meteoPointTemp;
        return false;
    }
    else
    {
        delete meteoPointTemp;
        return true;
    }

}


bool PragaProject::elaborationPointsCycleGrid(bool isAnomaly, bool showInfo)
{

    bool isMeteoGrid = true; // grid
    int validCell = 0;

    std::string id;

    FormInfo myInfo;
    int infoStep;
    QString infoStr;

    errorString.clear();

    if (showInfo)
    {
        if (isAnomaly)
        {
            infoStr = "Anomaly - Meteo Grid";
        }
        else
        {
            infoStr = "Elaboration - Meteo Grid";
        }

        infoStep = myInfo.start(infoStr, this->meteoGridDbHandler->gridStructure().header().nrRows);
    }

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


     Crit3DMeteoPoint* meteoPointTemp = new Crit3DMeteoPoint;

     for (int row = 0; row < meteoGridDbHandler->gridStructure().header().nrRows; row++)
     {
         if (showInfo && (row % infoStep) == 0)
             myInfo.setValue(row);

         for (int col = 0; col < meteoGridDbHandler->gridStructure().header().nrCols; col++)
         {

            if (meteoGridDbHandler->meteoGrid()->getMeteoPointActiveId(row, col, &id))
            {

                Crit3DMeteoPoint* meteoPoint = meteoGridDbHandler->meteoGrid()->meteoPointPointer(row,col);

                // copy data to MPTemp
                meteoPointTemp->id = meteoPoint->id;
                meteoPointTemp->point.z = meteoPoint->point.z;
                meteoPointTemp->latitude = meteoPoint->latitude;
                meteoPointTemp->elaboration = meteoPoint->elaboration;

                // meteoPointTemp should be init
                meteoPointTemp->nrObsDataDaysH = 0;
                meteoPointTemp->nrObsDataDaysD = 0;


                if  ( elaborationOnPoint(&errorString, NULL, meteoGridDbHandler, meteoPointTemp, clima, isMeteoGrid, startDate, endDate, isAnomaly, meteoSettings))
                {
                    validCell = validCell + 1;
                }

                // save result to MP
                meteoPoint->elaboration = meteoPointTemp->elaboration;
                meteoPoint->anomaly = meteoPointTemp->anomaly;
                meteoPoint->anomalyPercentage = meteoPointTemp->anomalyPercentage;

            }

        }
    }

    if (showInfo) myInfo.close();

    if (validCell == 0)
    {
        if (errorString.empty())
        {
            errorString = "no valid cells available";
        }
        delete meteoPointTemp;
        return false;
    }
    else
    {
        delete meteoPointTemp;
        return true;
    }

}

bool PragaProject::climatePointsCycle(bool showInfo)
{
    bool isMeteoGrid = false;
    FormInfo myInfo;
    int infoStep;
    QString infoStr;

    int validCell = 0;
    QDate startDate;
    QDate endDate;
    bool changeDataSet = true;

    errorString.clear();

    if (showInfo)
    {
        infoStr = "Climate  - Meteo Points";
        infoStep = myInfo.start(infoStr, nrMeteoPoints);
    }

    // parser all the list
    Crit3DClimateList* climateList = clima->getListElab();
    climateList->parserElaboration();

    Crit3DMeteoPoint* meteoPointTemp = new Crit3DMeteoPoint;
    for (int i = 0; i < nrMeteoPoints; i++)
    {
        if (meteoPoints[i].active)
        {

            if (showInfo && (i % infoStep) == 0)
            {
                myInfo.setValue(i);
            }

            meteoPointTemp->id = meteoPoints[i].id;
            meteoPointTemp->point.z = meteoPoints[i].point.z;
            meteoPointTemp->latitude = meteoPoints[i].latitude;
            changeDataSet = true;

            std::vector<float> outputValues;

            for (int j = 0; j < climateList->listClimateElab().size(); j++)
            {

                clima->resetParam();
                clima->setClimateElab(climateList->listClimateElab().at(j));


                if (climateList->listClimateElab().at(j)!= NULL)
                {

                    // copy current elaboration to clima
                    clima->setYearStart(climateList->listYearStart().at(j));
                    clima->setYearEnd(climateList->listYearEnd().at(j));
                    clima->setPeriodType(climateList->listPeriodType().at(j));
                    clima->setPeriodStr(climateList->listPeriodStr().at(j));
                    clima->setGenericPeriodDateStart(climateList->listGenericPeriodDateStart().at(j));
                    clima->setGenericPeriodDateEnd(climateList->listGenericPeriodDateEnd().at(j));
                    clima->setNYears(climateList->listNYears().at(j));
                    clima->setVariable(climateList->listVariable().at(j));
                    clima->setElab1(climateList->listElab1().at(j));
                    clima->setElab2(climateList->listElab2().at(j));
                    clima->setParam1(climateList->listParam1().at(j));
                    clima->setParam2(climateList->listParam2().at(j));
                    clima->setParam1IsClimate(climateList->listParam1IsClimate().at(j));
                    clima->setParam1ClimateField(climateList->listParam1ClimateField().at(j));

                    if (clima->periodType() == genericPeriod)
                    {
                        startDate.setDate(clima->yearStart(), clima->genericPeriodDateStart().month(), clima->genericPeriodDateStart().day());
                        endDate.setDate(clima->yearEnd() + clima->nYears(), clima->genericPeriodDateEnd().month(), clima->genericPeriodDateEnd().day());
                    }
                    else if (clima->periodType() == seasonalPeriod)
                    {
                        startDate.setDate(clima->yearStart() -1, 12, 1);
                        endDate.setDate(clima->yearEnd(), 12, 31);
                    }
                    else
                    {
                        startDate.setDate(clima->yearStart(), 1, 1);
                        endDate.setDate(clima->yearEnd(), 12, 31);
                    }
                }
                else
                {
                    errorString = "parser elaboration error";
                    delete meteoPointTemp;
                    return false;
                }

                if (climateOnPoint(&errorString, meteoPointsDbHandler, NULL, clima, meteoPointTemp, outputValues, isMeteoGrid, startDate, endDate, changeDataSet, meteoSettings))
                {
                    validCell = validCell + 1;
                }
                changeDataSet = false;

            }

        }
    }
    if (showInfo) myInfo.close();

    if (validCell == 0)
    {
        if (errorString.empty())
        {
            errorString = "no valid cells available";
        }
        delete meteoPointTemp;
        return false;
    }
    else
    {
        delete meteoPointTemp;
        return true;
    }
}


bool PragaProject::climatePointsCycleGrid(bool showInfo)
{

    bool isMeteoGrid = true;
    FormInfo myInfo;
    int infoStep;
    QString infoStr;

    int validCell = 0;
    QDate startDate;
    QDate endDate;
    std::string id;
    bool changeDataSet = true;

    errorString.clear();

    if (showInfo)
    {
        infoStr = "Climate  - Meteo Grid";
        infoStep = myInfo.start(infoStr, this->meteoGridDbHandler->gridStructure().header().nrRows);
    }

    // parser all the list
    Crit3DClimateList* climateList = clima->getListElab();
    climateList->parserElaboration();

    Crit3DMeteoPoint* meteoPointTemp = new Crit3DMeteoPoint;
    for (int row = 0; row < meteoGridDbHandler->gridStructure().header().nrRows; row++)
    {
        if (showInfo && (row % infoStep) == 0)
            myInfo.setValue(row);

        for (int col = 0; col < meteoGridDbHandler->gridStructure().header().nrCols; col++)
        {

           if (meteoGridDbHandler->meteoGrid()->getMeteoPointActiveId(row, col, &id))
           {

               Crit3DMeteoPoint* meteoPoint = meteoGridDbHandler->meteoGrid()->meteoPointPointer(row,col);

               meteoPointTemp->id = meteoPoint->id;
               meteoPointTemp->point.z = meteoPoint->point.z;
               meteoPointTemp->latitude = meteoPoint->latitude;

               changeDataSet = true;
               std::vector<float> outputValues;

               for (int j = 0; j < climateList->listClimateElab().size(); j++)
               {

                   clima->resetParam();
                   clima->setClimateElab(climateList->listClimateElab().at(j));


                   if (climateList->listClimateElab().at(j)!= NULL)
                   {

                       // copy current elaboration to clima
                       clima->setYearStart(climateList->listYearStart().at(j));
                       clima->setYearEnd(climateList->listYearEnd().at(j));
                       clima->setPeriodType(climateList->listPeriodType().at(j));
                       clima->setPeriodStr(climateList->listPeriodStr().at(j));
                       clima->setGenericPeriodDateStart(climateList->listGenericPeriodDateStart().at(j));
                       clima->setGenericPeriodDateEnd(climateList->listGenericPeriodDateEnd().at(j));
                       clima->setNYears(climateList->listNYears().at(j));
                       clima->setVariable(climateList->listVariable().at(j));
                       clima->setElab1(climateList->listElab1().at(j));
                       clima->setElab2(climateList->listElab2().at(j));
                       clima->setParam1(climateList->listParam1().at(j));
                       clima->setParam2(climateList->listParam2().at(j));
                       clima->setParam1IsClimate(climateList->listParam1IsClimate().at(j));
                       clima->setParam1ClimateField(climateList->listParam1ClimateField().at(j));

                       if (clima->periodType() == genericPeriod)
                       {
                           startDate.setDate(clima->yearStart(), clima->genericPeriodDateStart().month(), clima->genericPeriodDateStart().day());
                           endDate.setDate(clima->yearEnd() + clima->nYears(), clima->genericPeriodDateEnd().month(), clima->genericPeriodDateEnd().day());
                       }
                       else if (clima->periodType() == seasonalPeriod)
                       {
                           startDate.setDate(clima->yearStart() -1, 12, 1);
                           endDate.setDate(clima->yearEnd(), 12, 31);
                       }
                       else
                       {
                           startDate.setDate(clima->yearStart(), 1, 1);
                           endDate.setDate(clima->yearEnd(), 12, 31);
                       }

                   }
                   else
                   {
                       errorString = "parser elaboration error";
                       delete meteoPointTemp;
                       return false;
                   }

                   if (climateOnPoint(&errorString, NULL, meteoGridDbHandler, clima, meteoPointTemp, outputValues, isMeteoGrid, startDate, endDate, changeDataSet, meteoSettings))
                   {
                       validCell = validCell + 1;
                   }
                   changeDataSet = false;

               }

           }
       }
   }

   if (showInfo) myInfo.close();

   if (validCell == 0)
   {
       if (errorString.empty())
       {
           errorString = "no valid cells available";
       }
       delete meteoPointTemp;
       return false;
    }
    else
    {
        delete meteoPointTemp;
        return true;
    }

}

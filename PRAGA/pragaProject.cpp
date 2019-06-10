#include "pragaProject.h"
#include "project.h"
#include "climate.h"
#include "dbClimate.h"
#include "download.h"
#include "furtherMathFunctions.h"
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
    climaFromDb = nullptr;
    referenceClima = nullptr;
}

bool PragaProject::loadPragaSettings()
{
    Q_FOREACH (QString group, parameters->childGroups())
    {
        if (group == "elaboration")
        {
            parameters->beginGroup(group);
            Crit3DElaborationSettings* elabSettings = clima->getElabSettings();

            if (parameters->contains("anomaly_pts_max_distance") && !parameters->value("anomaly_pts_max_distance").toString().isEmpty())
            {
                elabSettings->setAnomalyPtsMaxDistance(parameters->value("anomaly_pts_max_distance").toFloat());
            }
            if (parameters->contains("anomaly_pts_max_delta_z") && !parameters->value("anomaly_pts_max_delta_z").toString().isEmpty())
            {
                elabSettings->setAnomalyPtsMaxDeltaZ(parameters->value("anomaly_pts_max_delta_z").toFloat());
            }
            if (parameters->contains("grid_min_coverage") && !parameters->value("grid_min_coverage").toString().isEmpty())
            {
                elabSettings->setGridMinCoverage(parameters->value("grid_min_coverage").toFloat());
            }
            if (parameters->contains("compute_tmed") && !parameters->value("compute_tmed").toString().isEmpty())
            {
                elabSettings->setAutomaticTmed(parameters->value("compute_tmed").toBool());
            }
            if (parameters->contains("compute_et0hs") && !parameters->value("compute_et0hs").toString().isEmpty())
            {
                elabSettings->setAutomaticETP(parameters->value("compute_et0hs").toBool());
            }
            if (parameters->contains("merge_joint_stations") && !parameters->value("merge_joint_stations").toString().isEmpty())
            {
                elabSettings->setMergeJointStations(parameters->value("merge_joint_stations").toBool());
            }

            parameters->endGroup();

        }
    }

    return true;
}

bool PragaProject::interpolationMeteoGrid(meteoVariable myVar, frequencyType myFrequency, const Crit3DTime& myTime,
                              gis::Crit3DRasterGrid *myRaster, bool showInfo)
{

    if (meteoGridDbHandler != nullptr)
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
        if (this->meteoGridDbHandler == nullptr)
        {
            errorString = "Load grid";
            return false;
        }
        else
        {
            if (this->clima == nullptr)
            {
                this->clima = new Crit3DClimate(); 
            }
            clima->setDb(this->meteoGridDbHandler->db());
        }
    }
    else
    {
        if (this->meteoPointsDbHandler == nullptr)
        {
            errorString = "Load meteo Points";
            return false;
        }
        else
        {
            if (this->clima == nullptr)
            {
                this->clima = new Crit3DClimate();
            }
            clima->setDb(this->meteoPointsDbHandler->getDb());
        }
    }
    if (isAnomaly)
    {
        if (this->referenceClima == nullptr)
        {
            this->referenceClima = new Crit3DClimate();
        }
        if (isMeteoGrid)
        {
            referenceClima->setDb(this->meteoGridDbHandler->db());
        }
        else
        {
            referenceClima->setDb(this->meteoPointsDbHandler->getDb());
        }
    }

    return true;
}

bool PragaProject::showClimateFields(bool isMeteoGrid, QStringList* climateDbElab, QStringList* climateDbVarList)
{
    QSqlDatabase db;
    if (isMeteoGrid)
    {
        if (this->meteoGridDbHandler == nullptr)
        {
            errorString = "Load grid";
            return false;
        }
        db = this->meteoGridDbHandler->db();
    }
    else
    {
        if (this->meteoPointsDbHandler == nullptr)
        {
            errorString = "Load meteo Points";
            return false;
        }
        db = this->meteoPointsDbHandler->getDb();
    }
    QStringList climateTables;

    if ( !showClimateTables(db, &errorString, &climateTables) )
    {
        errorString = "No climate tables";
        return false;
    }
    else
    {
        for (int i=0; i < climateTables.size(); i++)
        {
            selectAllElab(db, &errorString, climateTables.at(i), climateDbElab);
        }
        if (climateDbElab->isEmpty())
        {
            errorString = "Empty climate tables";
            return false;
        }
    }
    for (int i=0; i < climateDbElab->size(); i++)
    {
        QString elab = climateDbElab->at(i);
        QStringList words = elab.split('_');
        QString var = words[1];
        if (!climateDbVarList->contains(var))
        {
            climateDbVarList->append(var);
        }
    }
    return true;

}

void PragaProject::saveClimateResult(bool isMeteoGrid, QString climaSelected, int climateIndex, bool showInfo)
{

    FormInfo myInfo;
    int infoStep;
    QString infoStr;

    QSqlDatabase db;
    QList<float> results;

    Crit3DClimateList climateList;
    QStringList climate;
    climate.push_back(climaSelected);

    climateList.setListClimateElab(climate);
    climateList.parserElaboration();

    // copy elaboration to clima
    clima->setYearStart(climateList.listYearStart().at(0));
    clima->setYearEnd(climateList.listYearEnd().at(0));
    clima->setPeriodType(climateList.listPeriodType().at(0));
    clima->setPeriodStr(climateList.listPeriodStr().at(0));
    clima->setGenericPeriodDateStart(climateList.listGenericPeriodDateStart().at(0));
    clima->setGenericPeriodDateEnd(climateList.listGenericPeriodDateEnd().at(0));
    clima->setNYears(climateList.listNYears().at(0));
    clima->setVariable(climateList.listVariable().at(0));
    clima->setElab1(climateList.listElab1().at(0));
    clima->setElab2(climateList.listElab2().at(0));
    clima->setParam1(climateList.listParam1().at(0));
    clima->setParam2(climateList.listParam2().at(0));
    clima->setParam1IsClimate(climateList.listParam1IsClimate().at(0));
    clima->setParam1ClimateField(climateList.listParam1ClimateField().at(0));

    QString table = "climate_" + climateList.listPeriodStr().at(0);

    if (isMeteoGrid)
    {
        if (showInfo)
        {
            infoStr = "Read Climate - Meteo Grid";
            infoStep = myInfo.start(infoStr, this->meteoGridDbHandler->gridStructure().header().nrRows);
        }
        std::string id;
        db = this->meteoGridDbHandler->db();
        for (int row = 0; row < meteoGridDbHandler->gridStructure().header().nrRows; row++)
        {
            if (showInfo && (row % infoStep) == 0 )
            {
                 myInfo.setValue(row);
            }
            for (int col = 0; col < meteoGridDbHandler->gridStructure().header().nrCols; col++)
            {
                if (meteoGridDbHandler->meteoGrid()->getMeteoPointActiveId(row, col, &id))
                {
                    Crit3DMeteoPoint* meteoPoint = meteoGridDbHandler->meteoGrid()->meteoPointPointer(row,col);
                    results = readElab(db, table.toLower(), &errorString, QString::fromStdString(meteoPoint->id), climaSelected);
                    if (results.size() < climateIndex)
                    {
                        errorString = "climate index error";
                        meteoPoint->climate = NODATA;
                    }
                    else
                    {
                        float value = results[climateIndex-1];
                        meteoPoint->climate = value;
                    }
                }
             }
        }
        //meteoGridDbHandler->meteoGrid()->fillMeteoRasterClimateValue();
        meteoGridDbHandler->meteoGrid()->setIsElabValue(true);
    }
    else
    {
        if (showInfo)
        {
            infoStr = "Read Climate - Meteo Points";
            infoStep = myInfo.start(infoStr, nrMeteoPoints);
        }
        db = this->meteoPointsDbHandler->getDb();
        for (int i = 0; i < nrMeteoPoints; i++)
        {
            if (meteoPoints[i].active)
            {
                if (showInfo && (i % infoStep) == 0)
                {
                    myInfo.setValue(i);
                }
                QString id = QString::fromStdString(meteoPoints[i].id);
                results = readElab(db, table.toLower(), &errorString, id, climaSelected);
                if (results.size() < climateIndex)
                {
                    errorString = "climate index error";
                    meteoPoints[i].climate = NODATA;
                }
                else
                {
                    float value = results[climateIndex-1];
                    meteoPoints[i].climate = value;
                }
            }
        }
        setIsElabMeteoPointsValue(true);

    }
    if (showInfo) myInfo.close();
}

bool PragaProject::deleteClima(bool isMeteoGrid, QString climaSelected)
{
    QSqlDatabase db;

    QStringList words = climaSelected.split('_');
    QString period = words[2];
    QString table = "climate_" + period;

    if (isMeteoGrid)
    {
        db = this->meteoGridDbHandler->db();
    }
    else
    {
        db = this->meteoPointsDbHandler->getDb();
    }

    return deleteElab(db, &errorString, table.toLower(), climaSelected);
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
            //meteoGridDbHandler->meteoGrid()->fillMeteoRasterElabValue();
        }
        else
        {
            if (!elaborationPointsCycleGrid(isAnomaly, true))
            {
                return false;
            }
            //meteoGridDbHandler->meteoGrid()->fillMeteoRasterAnomalyValue();
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

    Crit3DClimate* climaUsed = new Crit3DClimate();

    if (isAnomaly)
    {
        climaUsed->copyParam(referenceClima);
        if (showInfo)
        {
            infoStr = "Anomaly - Meteo Points";
            infoStep = myInfo.start(infoStr, nrMeteoPoints);
        }
    }
    else
    {
        climaUsed->copyParam(clima);
        if (showInfo)
        {
            infoStr = "Elaboration - Meteo Points";
            infoStep = myInfo.start(infoStr, nrMeteoPoints);
        }
    }


    QDate startDate(climaUsed->yearStart(), climaUsed->genericPeriodDateStart().month(), climaUsed->genericPeriodDateStart().day());
    QDate endDate(climaUsed->yearEnd(), climaUsed->genericPeriodDateEnd().month(), climaUsed->genericPeriodDateEnd().day());

    if (climaUsed->nYears() > 0)
    {
        endDate.setDate(climaUsed->yearEnd() + climaUsed->nYears(), climaUsed->genericPeriodDateEnd().month(), climaUsed->genericPeriodDateEnd().day());
    }
    else if (climaUsed->nYears() < 0)
    {
        startDate.setDate(climaUsed->yearStart() + climaUsed->nYears(), climaUsed->genericPeriodDateStart().month(), climaUsed->genericPeriodDateStart().day());
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
            meteoPointTemp->point.utm.x = meteoPoints[i].point.utm.x;  // LC to compute distance in passingClimateToAnomaly
            meteoPointTemp->point.utm.y = meteoPoints[i].point.utm.y;  // LC to compute distance in passingClimateToAnomaly
            meteoPointTemp->point.z = meteoPoints[i].point.z;
            meteoPointTemp->latitude = meteoPoints[i].latitude;
            meteoPointTemp->elaboration = meteoPoints[i].elaboration;

            // meteoPointTemp should be init
            meteoPointTemp->nrObsDataDaysH = 0;
            meteoPointTemp->nrObsDataDaysD = 0;

            if (showInfo && (i % infoStep) == 0)
                        myInfo.setValue(i);


            if (isAnomaly && climaUsed->getIsClimateAnomalyFromDb())
            {
                if ( passingClimateToAnomaly(&errorString, meteoPointTemp, climaUsed, meteoPoints, nrMeteoPoints, clima->getElabSettings()) )
                {
                    validCell = validCell + 1;
                }
            }
            else
            {
                if ( elaborationOnPoint(&errorString, meteoPointsDbHandler, nullptr, meteoPointTemp, climaUsed, isMeteoGrid, startDate, endDate, isAnomaly, meteoSettings))
                {
                    validCell = validCell + 1;
                }
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
        if (errorString.isEmpty())
        {
            errorString = "no valid cells available";
        }
        delete meteoPointTemp;
        delete climaUsed;
        return false;
    }
    else
    {
        delete meteoPointTemp;
        delete climaUsed;
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

    Crit3DClimate* climaUsed = new Crit3DClimate();

    if (isAnomaly)
    {
        climaUsed->copyParam(referenceClima);
        if (showInfo)
        {
            infoStr = "Anomaly - Meteo Grid";
            infoStep = myInfo.start(infoStr, this->meteoGridDbHandler->gridStructure().header().nrRows);
        }
    }
    else
    {
        climaUsed->copyParam(clima);
        if (showInfo)
        {
            infoStr = "Elaboration - Meteo Grid";
            infoStep = myInfo.start(infoStr, this->meteoGridDbHandler->gridStructure().header().nrRows);
        }
    }


    QDate startDate(climaUsed->yearStart(), climaUsed->genericPeriodDateStart().month(), climaUsed->genericPeriodDateStart().day());
    QDate endDate(climaUsed->yearEnd(), climaUsed->genericPeriodDateEnd().month(), climaUsed->genericPeriodDateEnd().day());

    if (climaUsed->nYears() > 0)
    {
        endDate.setDate(climaUsed->yearEnd() + climaUsed->nYears(), climaUsed->genericPeriodDateEnd().month(), climaUsed->genericPeriodDateEnd().day());
    }
    else if (climaUsed->nYears() < 0)
    {
        startDate.setDate(climaUsed->yearStart() + climaUsed->nYears(), climaUsed->genericPeriodDateStart().month(), climaUsed->genericPeriodDateStart().day());
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

                if (isAnomaly && climaUsed->getIsClimateAnomalyFromDb())
                {
                    if ( passingClimateToAnomalyGrid(&errorString, meteoPointTemp, climaUsed))
                    {
                        validCell = validCell + 1;
                    }
                }
                else
                {
                    if  ( elaborationOnPoint(&errorString, nullptr, meteoGridDbHandler, meteoPointTemp, climaUsed, isMeteoGrid, startDate, endDate, isAnomaly, meteoSettings))
                    {
                        validCell = validCell + 1;
                    }
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
        if (errorString.isEmpty())
        {
            errorString = "no valid cells available";
        }
        delete meteoPointTemp;
        delete climaUsed;
        return false;
    }
    else
    {
        delete meteoPointTemp;
        delete climaUsed;
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
    clima->resetCurrentValues();

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


                if (climateList->listClimateElab().at(j)!= nullptr)
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

                if (climateOnPoint(&errorString, meteoPointsDbHandler, nullptr, clima, meteoPointTemp, outputValues, isMeteoGrid, startDate, endDate, changeDataSet, meteoSettings))
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
        if (errorString.isEmpty())
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
    clima->resetCurrentValues();

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


                   if (climateList->listClimateElab().at(j)!= nullptr)
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

                   if (climateOnPoint(&errorString, nullptr, meteoGridDbHandler, clima, meteoPointTemp, outputValues, isMeteoGrid, startDate, endDate, changeDataSet, meteoSettings))
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
       if (errorString.isEmpty())
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

bool PragaProject::downloadDailyDataArkimet(QStringList variables, bool prec0024, QDate startDate, QDate endDate, bool showInfo)
{
    const int MAXDAYS = 30;

    QString id, dataset;
    QStringList datasetList;
    QList<QStringList> idList;

    QList<int> arkIdAirTemp;
    arkIdAirTemp << 231 << 232 << 233;
    int arkIdPrec = 250;
    QList<int> arkIdRH;
    arkIdRH << 240 << 241 << 242;
    int arkIdRadiation = 706;
    QList<int> arkIdWind;
    arkIdWind << 227 << 230;

    QList<int> arkIdVar;
    for( int i=0; i < variables.size(); i++ )
    {
        if (variables[i] == "Air Temperature")
            arkIdVar.append(arkIdAirTemp);
        if (variables[i] == "Precipitation")
            arkIdVar.append(arkIdPrec);
        if (variables[i] == "Air Humidity")
            arkIdVar.append(arkIdRH);
        if (variables[i] == "Radiation")
            arkIdVar.append(arkIdRadiation);
        if (variables[i] == "Wind")
            arkIdVar.append(arkIdWind);
    }

    Download* myDownload = new Download(meteoPointsDbHandler->getDbName());

    int index, nrPoints = 0;
    for( int i=0; i < nrMeteoPoints; i++ )
    {
        if (getMeteoPointSelected(i))
        {
            nrPoints ++;

            id = QString::fromStdString(meteoPoints[i].id);
            dataset = QString::fromStdString(meteoPoints[i].dataset);

            if (!datasetList.contains(dataset))
            {
                datasetList << dataset;
                QStringList myList;
                myList << id;
                idList.append(myList);
            }
            else
            {
                index = datasetList.indexOf(dataset);
                idList[index].append(id);
            }
        }
    }

    FormInfo myInfo;
    QString infoStr;

    int nrDays = startDate.daysTo(endDate) + 1;
    if (showInfo) myInfo.start(infoStr, nrPoints*nrDays);

    int currentPoints = 0.;
    for( int i=0; i < datasetList.size(); i++ )
    {
        QDate date1 = startDate;
        QDate date2 = std::min(date1.addDays(MAXDAYS-1), endDate);

        while (date1 <= endDate)
        {
            if (showInfo)
            {
                myInfo.setText("Download data from: " + date1.toString("yyyy-MM-dd") + " to: " + date2.toString("yyyy-MM-dd") + " dataset:" + datasetList[i]);
                currentPoints += idList[i].size() * (date1.daysTo(date2) + 1);
                myInfo.setValue(currentPoints);
            }

            myDownload->downloadDailyData(date1, date2, datasetList[i], idList[i], arkIdVar, prec0024);

            date1 = date2.addDays(1);
            date2 = std::min(date1.addDays(MAXDAYS-1), endDate);
        }
    }

    if (showInfo) myInfo.close();
    return true;
}


bool PragaProject::downloadHourlyDataArkimet(QStringList variables, QDate startDate, QDate endDate, bool showInfo)
{
    const int MAXDAYS = 7;

    QList<int> arkIdAirTemp;
    arkIdAirTemp << 78 << 158;
    QList<int> arkIdPrec;
    arkIdPrec << 160;
    QList<int> arkIdRH;
    arkIdRH << 139 << 140;
    QList<int> arkIdRadiation;
    arkIdRadiation << 164 << 409;
    QList<int> arkIdWind;
    arkIdWind << 69 << 165 << 166 << 431;

    QList<int> arkIdVar;
    for( int i=0; i < variables.size(); i++ )
    {
        if (variables[i] == "Air Temperature")
            arkIdVar.append(arkIdAirTemp);
        if (variables[i] == "Precipitation")
            arkIdVar.append(arkIdPrec);
        if (variables[i] == "Air Humidity")
            arkIdVar.append(arkIdRH);
        if (variables[i] == "Radiation")
            arkIdVar.append(arkIdRadiation);
        if (variables[i] == "Wind")
            arkIdVar.append(arkIdWind);
    }

    int index, nrPoints = 0;
    QString id, dataset;
    QStringList datasetList;
    QList<QStringList> idList;

    for( int i=0; i < nrMeteoPoints; i++ )
    {
        if (getMeteoPointSelected(i))
        {
            nrPoints ++;

            id = QString::fromStdString(meteoPoints[i].id);
            dataset = QString::fromStdString(meteoPoints[i].dataset);

            if (!datasetList.contains(dataset))
            {
                datasetList << dataset;
                QStringList myList;
                myList << id;
                idList.append(myList);
            }
            else
            {
                index = datasetList.indexOf(dataset);
                idList[index].append(id);
            }
        }
    }

    Download* myDownload = new Download(meteoPointsDbHandler->getDbName());

    FormInfo myInfo;
    QString infoStr;

    int nrDays = startDate.daysTo(endDate) + 1;
    if (showInfo) myInfo.start(infoStr, nrPoints*nrDays);

    int currentPoints = 0.;
    for( int i=0; i < datasetList.size(); i++ )
    {
        QDate date1 = startDate;
        QDate date2 = std::min(date1.addDays(MAXDAYS-1), endDate);

        while (date1 <= endDate)
        {
            if (showInfo)
            {
                myInfo.setText("Download data from: " + date1.toString("yyyy-MM-dd") + " to:" + date2.toString("yyyy-MM-dd") + " dataset:" + datasetList[i]);
                currentPoints += idList[i].size() * (date1.daysTo(date2) + 1);
                myInfo.setValue(currentPoints);
            }

            myDownload->downloadHourlyData(date1, date2, datasetList[i], idList[i], arkIdVar);

            date1 = date2.addDays(1);
            date2 = std::min(date1.addDays(MAXDAYS-1), endDate);
        }
    }

    if (showInfo) myInfo.close();
    return true;
}


void PragaProject::averageSeriesOnZonesMeteoGrid(meteoVariable variable, meteoComputation elab1MeteoComp, gridAggregationMethod spatialElab, float threshold, gis::Crit3DRasterGrid* zoneGrid, QDate startDate, QDate endDate, std::vector<float> &outputValues, bool showInfo)
{


    std::vector <std::vector<int> > meteoGridRow(zoneGrid->header->nrRows, std::vector<int>(zoneGrid->header->nrCols, NODATA));
    std::vector <std::vector<int> > meteoGridCol(zoneGrid->header->nrRows, std::vector<int>(zoneGrid->header->nrCols, NODATA));
    meteoGridDbHandler->meteoGrid()->saveRowColfromZone(zoneGrid, meteoGridRow, meteoGridCol);


    float percValue;
    bool isMeteoGrid = true;
    std::string id;
    unsigned int zoneIndex = 0;
    int indexSeries = 0;
    float value;
    std::vector<float> outputSeries;
    std::vector <std::vector<int>> indexRowCol(meteoGridDbHandler->gridStructure().header().nrRows, std::vector<int>(meteoGridDbHandler->gridStructure().header().nrCols, NODATA));

    gis::updateMinMaxRasterGrid(zoneGrid);
    std::vector <std::vector<float> > zoneVector(int(zoneGrid->maximum), std::vector<float>());
    for (int i = 0; i < int(zoneGrid->maximum); i++)
    {
        std::vector<float> tmp(0);
        zoneVector.push_back(tmp);
    }

    FormInfo myInfo;
    QString infoStr;
    int infoStep = myInfo.start(infoStr, this->meteoGridDbHandler->gridStructure().header().nrRows);

    Crit3DMeteoPoint* meteoPointTemp = new Crit3DMeteoPoint;

     for (int row = 0; row < meteoGridDbHandler->gridStructure().header().nrRows; row++)
     {
         if (showInfo && (row % infoStep) == 0)
             myInfo.setValue(row);

         std::vector<int> tmpRowVector;
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

                bool dataLoaded = preElaboration(&errorString, nullptr, meteoGridDbHandler, meteoPointTemp, isMeteoGrid, variable, elab1MeteoComp, startDate, endDate, outputValues, &percValue, meteoSettings, clima->getElabSettings());
                if (dataLoaded)
                {
                    indexSeries = indexSeries + 1;
                    outputSeries.insert(outputSeries.end(), outputValues.begin(), outputValues.end());
                    tmpRowVector.push_back(indexSeries);
                }
            }
        }
        indexRowCol.push_back(tmpRowVector);
    }

     int nrDays = int(startDate.daysTo(endDate) + 1);
     std::vector< std::vector<float> > dailyElabAggregation(nrDays, std::vector<float>(int(zoneGrid->maximum), NODATA));

     for (int day = 0; day < nrDays; day++)
     {
         std::vector<float> dailyElabRow;
         for (int zoneRow = 0; zoneRow < zoneGrid->header->nrRows; zoneRow++)
         {
             for (int zoneCol = 0; zoneCol < zoneGrid->header->nrRows; zoneCol++)
             {

                float zoneValue = zoneGrid->value[zoneRow][zoneCol];
                if ( zoneValue != zoneGrid->header->flag)
                {
                    zoneIndex = (unsigned int)(zoneValue);

                    if (meteoGridRow[zoneRow][zoneCol] != NODATA && meteoGridCol[zoneRow][zoneCol] != NODATA)
                    {
                        if (indexRowCol[meteoGridRow[zoneRow][zoneCol]][meteoGridCol[zoneRow][zoneCol]] != NODATA)
                        {
                            value = outputSeries.at(indexRowCol[meteoGridRow[zoneRow][zoneCol]][meteoGridCol[zoneRow][zoneCol]]+day);
                            if (value != meteoGridDbHandler->gridStructure().header().flag)
                            {
                                zoneVector[zoneIndex].push_back(value);
                            }
                        }
                    }
                }
             }
         }

         for (unsigned int zonePos = 0; zonePos < zoneVector.size(); zonePos++)
         {
            std::vector<float> validValues;
            validValues = zoneVector[zonePos];
            if (threshold != NODATA)
            {
                extractValidValuesWithThreshold(validValues, threshold);
            }
            // LC l'enum comprende solo questi valori a differenza di vb dove lo
            // switch p con mean, devStd, max e min
            float res = NODATA;
            int size = int(validValues.size());
            switch (spatialElab)
            {
                case aggrAvg:
                    {
                        res = statistics::mean(validValues, size);
                        break;
                    }
                case aggrMedian:
                    {

                        res = sorting::percentile(validValues, &size, 50.0, true);
                        break;
                    }
                case aggrStdDeviation:
                    {
                        res = statistics::standardDeviation(validValues, size);
                        break;
                    }
            }
            dailyElabRow.push_back(res);

         }
         dailyElabAggregation.push_back(dailyElabRow);
     }



}

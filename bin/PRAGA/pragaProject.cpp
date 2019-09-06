#include "basicMath.h"
#include "climate.h"
#include "dbClimate.h"
#include "download.h"
#include "dbAggregationsHandler.h"
#include "formInfo.h"
#include "utilities.h"
#include "project.h"
#include "interpolationCmd.h"
#include "pragaProject.h"


bool PragaProject::getIsElabMeteoPointsValue() const
{
    return isElabMeteoPointsValue;
}

void PragaProject::setIsElabMeteoPointsValue(bool value)
{
    isElabMeteoPointsValue = value;
}

void PragaProject::initializePragaProject()
{
    clima = new Crit3DClimate();
    climaFromDb = nullptr;
    referenceClima = nullptr;
    pragaDefaultSettings = nullptr;
}

void PragaProject::clearPragaProject()
{
    if (isProjectLoaded) clearProject();
    delete clima;
    delete pragaDailyMaps;
    delete pragaHourlyMaps;
}

void PragaProject::createPragaProject(QString path_, QString name_, QString description_)
{
    createProject(path_, name_, description_);
    savePragaParameters();
}

void PragaProject::savePragaProject()
{
    saveProject();
    savePragaParameters();
}

bool PragaProject::loadPragaProject(QString myFileName)
{
    clearPragaProject();
    initializeProject();
    initializePragaProject();

    if (myFileName == "") return(false);

    if (! loadProjectSettings(myFileName))
        return false;

    if (! loadProject())
        return false;

    if (! loadPragaSettings())
        return false;

    if (DEM.isLoaded)
    {
        pragaDailyMaps = new Crit3DDailyMeteoMaps(DEM);
        pragaHourlyMaps = new Crit3DHourlyMeteoMaps(DEM);
    }

    isProjectLoaded = true;

    if (projectName != "")
    {
        logInfo("Project " + projectName + " loaded");
    }
    return true;
}

PragaProject::PragaProject()
{
    initializePragaProject();
}

bool PragaProject::loadPragaSettings()
{
    pragaDefaultSettings = new QSettings(getDefaultPath() + PATH_SETTINGS + "pragaDefault.ini", QSettings::IniFormat);

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


bool PragaProject::saveGrid(meteoVariable myVar, frequencyType myFrequency, const Crit3DTime& myTime, bool showInfo)
{
    std::string id;
    FormInfo myInfo;
    int infoStep = 1;

    if (myFrequency == daily)
    {
        if (showInfo)
        {
            QString infoStr = "Save meteo grid daily data";
            infoStep = myInfo.start(infoStr, this->meteoGridDbHandler->gridStructure().header().nrRows);
        }

        for (int row = 0; row < this->meteoGridDbHandler->gridStructure().header().nrRows; row++)
        {
            if (showInfo)
            {
                if ((row % infoStep) == 0)
                    myInfo.setValue(row);
            }
            for (int col = 0; col < this->meteoGridDbHandler->gridStructure().header().nrCols; col++)
            {
                if (this->meteoGridDbHandler->meteoGrid()->getMeteoPointActiveId(row, col, &id))
                {
                    if (!this->meteoGridDbHandler->gridStructure().isFixedFields())
                    {
                        this->meteoGridDbHandler->saveCellCurrentGridDaily(&errorString, QString::fromStdString(id), QDate(myTime.date.year, myTime.date.month, myTime.date.day), this->meteoGridDbHandler->getDailyVarCode(myVar), this->meteoGridDbHandler->meteoGrid()->meteoPoint(row,col).currentValue);
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
            QString infoStr = "Save meteo grid hourly data";
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
            selectAllElab(db, &errorString, climateTables[i], climateDbElab);
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


bool PragaProject::averageSeriesOnZonesMeteoGrid(meteoVariable variable, meteoComputation elab1MeteoComp, aggregationMethod spatialElab, float threshold, gis::Crit3DRasterGrid* zoneGrid, QDate startDate, QDate endDate, QString periodType, std::vector<float> &outputValues, bool showInfo)
{


    QString aggregationString = QString::fromStdString(getKeyStringAggregationMethod(spatialElab));
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
    std::vector <std::vector<float> > zoneVector((unsigned int)(zoneGrid->maximum+1), std::vector<float>());

    FormInfo myInfo;
    QString infoStr;
    int infoStep = myInfo.start(infoStr, this->meteoGridDbHandler->gridStructure().header().nrRows);

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

                outputValues.clear();
                bool dataLoaded = preElaboration(&errorString, nullptr, meteoGridDbHandler, meteoPointTemp, isMeteoGrid, variable, elab1MeteoComp, startDate, endDate, outputValues, &percValue, meteoSettings, clima->getElabSettings());
                if (dataLoaded)
                {
                    outputSeries.insert(outputSeries.end(), outputValues.begin(), outputValues.end());
                    indexRowCol[row][col] = indexSeries;
                    indexSeries = indexSeries + 1;
                }
            }
        }
    }

     int nrDays = int(startDate.daysTo(endDate) + 1);
     std::vector< std::vector<float> > dailyElabAggregation(nrDays, std::vector<float>(int(zoneGrid->maximum+1), NODATA));

     for (int day = 0; day < nrDays; day++)
     {

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
                            value = outputSeries.at(indexRowCol[meteoGridRow[zoneRow][zoneCol]][meteoGridCol[zoneRow][zoneCol]]*outputValues.size()+day);
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

            float res = NODATA;
            int size = int(validValues.size());

            switch (spatialElab)
            {
                case aggrAverage:
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
            dailyElabAggregation[unsigned(day)][zonePos] = res;
         }
         // clear zoneVector
         for (unsigned int zonePos = 0; zonePos < zoneVector.size(); zonePos++)
         {
            zoneVector[zonePos].clear();
         }

     }
     // save dailyElabAggregation result into DB
     if (!aggregationDbHandler->saveAggrData(int(zoneGrid->maximum+1), aggregationString, periodType, QDateTime(startDate), QDateTime(endDate), variable, dailyElabAggregation))
     {
         errorString = aggregationDbHandler->error();
         return false;
     }
     return true;

}


void PragaProject::savePragaParameters()
{
    parameters->beginGroup("elaboration");
        parameters->setValue("anomaly_pts_max_distance", QString::number(double(clima->getElabSettings()->getAnomalyPtsMaxDistance())));
        parameters->setValue("anomaly_pts_max_delta_z", QString::number(double(clima->getElabSettings()->getAnomalyPtsMaxDeltaZ())));
        parameters->setValue("grid_min_coverage", QString::number(double(clima->getElabSettings()->getGridMinCoverage())));
        parameters->setValue("compute_tmed", clima->getElabSettings()->getAutomaticTmed());
        parameters->setValue("compute_et0hs", clima->getElabSettings()->getAutomaticETP());
        parameters->setValue("merge_joint_stations", clima->getElabSettings()->getMergeJointStations());
    parameters->endGroup();
}

void setHourlyMapVar(Crit3DHourlyMeteoMaps* hourlyMaps, gis::Crit3DRasterGrid myGrid, meteoVariable myVar)
{
    if (myVar == airTemperature)
        *(hourlyMaps->mapHourlyT) = myGrid;
    else if (myVar == precipitation)
        *(hourlyMaps->mapHourlyPrec) = myGrid;
    else if (myVar == airRelHumidity)
        *(hourlyMaps->mapHourlyRelHum) = myGrid;
    else if (myVar == airDewTemperature)
        *(hourlyMaps->mapHourlyTdew) = myGrid;
    else if (myVar == windIntensity)
        *(hourlyMaps->mapHourlyWindInt) = myGrid;
    else if (myVar == windDirection)
        *(hourlyMaps->mapHourlyWindDir) = myGrid;
    else if (myVar == referenceEvapotranspiration)
        *(hourlyMaps->mapHourlyET0) = myGrid;
    else if (myVar == leafWetness)
        *(hourlyMaps->mapHourlyLeafW) = myGrid;
}

bool PragaProject::interpolationMeteoGridPeriod(QDate dateIni, QDate dateFin, QList <meteoVariable> variables)
{
    // check meteo point
    if (! meteoPointsLoaded || nrMeteoPoints == 0)
    {
        logError("No meteo points");
        return false;
    }

    // check meteo grid
    if (! meteoGridLoaded)
    {
        logError("No meteo grid");
        return false;
    }

    // check dates
    if (dateIni.isNull() || dateFin.isNull() || dateIni > dateFin)
    {
        logError("Wrong period");
        return false;
    }

    if (interpolationSettings.getUseTAD())
        if (! loadTopographicDistanceMaps())
            return false;

    //order variables for derived computation

    std::string id;
    QString myError;
    int myHour;
    QDate myDate = dateIni;
    gis::Crit3DRasterGrid* myGrid = new gis::Crit3DRasterGrid();

    myGrid->initializeGrid(*DEM.header);

    unsigned currentYear = unsigned(NODATA);

    logInfo("Loading meteo points data...");
    if (! loadMeteoPointsData(dateIni, dateFin, false))
        return false;

    while (myDate <= dateFin)
    {
        // check proxy grid series
        if (currentYear != unsigned(myDate.year()))
        {
            logInfo("Interpolating proxy grid series...");
            if (checkProxyGridSeries(&interpolationSettings, DEM, proxyGridSeries, myDate))
                if (! readProxyValues()) return false;
        }

        for (myHour = 1; myHour <= 24; myHour++)
        {
            foreach (meteoVariable myVar, variables)
            {
                if (myVar == airRelHumidity && interpolationSettings.getUseDewPoint())
                {
                    if (! interpolationDemMain(airDewTemperature, getCrit3DTime(myDate, myHour), pragaHourlyMaps->mapHourlyTdew, false)) return false;
                    pragaHourlyMaps->computeRelativeHumidityMap(*pragaHourlyMaps->mapHourlyTdew);
                }

                if (! interpolationDemMain(myVar, getCrit3DTime(myDate, myHour), myGrid, false))
                    return false;

                setHourlyMapVar(pragaHourlyMaps, *myGrid, myVar);

                meteoGridDbHandler->meteoGrid()->aggregateMeteoGrid(myVar, hourly, getCrit3DDate(myDate), myHour, 0, &DEM, myGrid, interpolationSettings.getMeteoGridAggrMethod());

            }
        }

        myDate = myDate.addDays(1);
    }

    logInfo("Save meteo grid hourly data");
    meteoGridDbHandler->saveGridData(&myError, QDateTime(dateIni, QTime(0,0,0)), QDateTime(dateFin.addDays(1), QTime(0,0,0)), variables);

    //restore original proxy grids

    return true;

    /*

        computeLeafWetness = (computePrec And computeRelHum)
        computeEtpPenman = (computeTemp And computeRelHum And computeWind And computeRad)
        computeEtpHargreaves = computeTemp

        myYearUrban = Definitions.NO_DATA

        'log file
        Print #myHandleFile, "Interpolation started at " & format(Now, "YYYY-MM-DD HH:MM:SS") & vbCrLf
        Print #myHandleFile, "From " & format(myStartDate, "YYYY-MM-DD") & " to "; format(myEndDate, "YYYY-MM-DD")
        Print #myHandleFile, "Interpolation method: "; Interpolation.GetInterpolationMethod

        For myDate = myStartDate To myEndDate

            If DBPointsManagement.loadPointHourly_Date(myDate, True, Definitions.INFO_NOT_VISUALIZE) Then

                If computeRad Then
                    Radiation.InitializeRadiationMaps GIS.DEM
                    Radiation.InitializeTransmissivityMap GIS.DEM
                End If

                dbGridManagement.initializeMeteoGrid passaggioDati.currentDay

                infoString = format(myDate, "DD/MM/YYYY") & " H:" & CStr(myHour) & " "
                PragaShell.StartInfo "Interpolating data - " & infoString, 24

                'PREC HOUR 0
                If computePrec Then
                    PragaShell.SetInfoOnlyText infoString & Definitions.HOURLY_PREC
                    passaggioDati.setCurrentHour 0
                    Print #myHandleFile, "HOUR:00"
                    If InterpolationSingle(Definitions.HOURLY_PREC, False, False, False) Then
                        grdPrecPreviousHour = GIS.DEM_Var
                        Print #myHandleFile, vbCrLf & getInfoInterpolation(Definitions.HOURLY_PREC)
                    End If
                End If

                For myHour = 1 To 24
                    PragaShell.updateInfo myHour
                    Print #myHandleFile, "HOUR:" & format(myHour, "00")

                    myFilenamePre = Praga_Path & Definitions.PATH_OUTPUT
                    myFilenamePost = "_" & format(myHour, "00") & ".flt"

                    passaggioDati.setCurrentHour myHour

                    infoString = format(myDate, "DD/MM/YYYY") & " H:" & CStr(myHour) & " "

                    'Tavg
                    If computeTemp Then
                        PragaShell.SetInfoOnlyText infoString & Definitions.HOURLY_TAVG
                        If InterpolationSingle(Definitions.HOURLY_TAVG, False, False, False) Then
                            grdTemperature = GIS.DEM_Var
                            Print #myHandleFile, getInfoInterpolation(Definitions.HOURLY_TAVG)
                        End If
                    End If

                    'RHavg
                    If computeRelHum Then
                        If Interpolation.GetUseDewPoint Then
                            PragaShell.SetInfoOnlyText infoString & Definitions.HOURLY_TDEW
                            If InterpolationSingle(Definitions.HOURLY_TDEW, False, False, False) Then
                                grdDewTemperature = GIS.DEM_Var
                                computeGridUmedFromTdew grdTemperature, grdDewTemperature, grdHumidity
                                Print #myHandleFile, getInfoInterpolation(Definitions.HOURLY_TDEW)
                            End If
                        Else
                            PragaShell.SetInfoOnlyText infoString & Definitions.HOURLY_RHAVG
                            If InterpolationSingle(Definitions.HOURLY_RHAVG, False, False, False) Then
                                grdHumidity = GIS.DEM_Var
                                Print #myHandleFile, getInfoInterpolation(Definitions.HOURLY_RHAVG)
                            End If
                        End If
                    End If

                    'PREC
                    If computePrec Then
                        PragaShell.SetInfoOnlyText infoString & Definitions.HOURLY_PREC
                        If InterpolationSingle(Definitions.HOURLY_PREC, False, False, False) Then
                            grdPrecipitation = GIS.DEM_Var
                            Print #myHandleFile, getInfoInterpolation(Definitions.HOURLY_PREC)
                        End If
                    End If

                    'WIND
                    If computeWind Then
                        'interpolate vector wind because we need direction
                        PragaShell.SetInfoOnlyText infoString & Definitions.HOURLY_VMED_X
                        If InterpolationSingle(Definitions.HOURLY_VMED_X, False, False, False) Then
                            grdWindX = GIS.DEM_Var
                            Print #myHandleFile, getInfoInterpolation(Definitions.HOURLY_VMED_X)
                        End If
                        PragaShell.SetInfoOnlyText infoString & Definitions.HOURLY_VMED_Y
                        If InterpolationSingle(Definitions.HOURLY_VMED_Y, False, False, False) Then
                            grdWindY = GIS.DEM_Var
                            If grdWindX.isLoaded Then
                                computeGridWind grdWindX, grdWindY, grdWindInt, grdWindDir
                                Print #myHandleFile, getInfoInterpolation(Definitions.HOURLY_VMED_Y)
                            End If
                        End If
                        'interpolate wind intensity as scalar
                        PragaShell.SetInfoOnlyText infoString & Definitions.HOURLY_VMED_INT
                        If InterpolationSingle(Definitions.HOURLY_VMED_INT, False, False, False) Then
                            grdWindInt = GIS.DEM_Var
                            Print #myHandleFile, getInfoInterpolation(Definitions.HOURLY_VMED_INT)
                        End If
                    End If

                    'RAD
                    If computeRad Then
                        PragaShell.SetInfoOnlyText infoString & Definitions.HOURLY_RAD
                        If Radiation.ComputeRadiationCurrentTime_GRID(GIS.DEM, True, False) Then
                            grdRadiation = Radiation.GlobalRadiationMap
                            grdTransmissivity = Radiation.TransmissivityMap
                        End If
                    End If

                    'ETP PENMAN
                    If computeEtpPenman Then
                        If grdTemperature.isLoaded And grdHumidity.isLoaded And grdWindInt.isLoaded And grdRadiation.isLoaded Then
                            computeGridEtpPenman GIS.DEM, grdTemperature, grdHumidity, grdWindInt, grdRadiation, grdTransmissivity, grdEtpPenman
                        End If
                    End If

                    'LEAF WETNESS
                    If computeLeafWetness Then
                        If grdPrecipitation.isLoaded And grdPrecPreviousHour.isLoaded And grdHumidity.isLoaded Then
                            computeGridLeafWetness grdPrecipitation, grdPrecPreviousHour, grdHumidity, grdLeafWetness
                        End If
                    End If

                    'SAVE HOURLY DEM MAPS
                    If saveRasters Then
                        PragaShell.SetInfoOnlyText infoString & "Saving hourly maps..."
                        If grdTemperature.isLoaded Then Output.saveGridToExport grdTemperature, "flt", myFilenamePre & Definitions.HOURLY_TAVG & myFilenamePost
                        If grdPrecipitation.isLoaded Then Output.saveGridToExport grdPrecipitation, "flt", myFilenamePre & Definitions.HOURLY_PREC & myFilenamePost
                        If grdHumidity.isLoaded Then Output.saveGridToExport grdHumidity, "flt", myFilenamePre & Definitions.HOURLY_RHAVG & myFilenamePost
                        If grdWindInt.isLoaded Then Output.saveGridToExport grdWindInt, "flt", myFilenamePre & Definitions.HOURLY_VMED_INT & myFilenamePost
                        If grdRadiation.isLoaded Then Output.saveGridToExport grdRadiation, "flt", myFilenamePre & Definitions.HOURLY_RAD & myFilenamePost
                        If grdEtpPenman.isLoaded Then Output.saveGridToExport grdEtpPenman, "flt", myFilenamePre & Definitions.HOURLY_ET0_PM & myFilenamePost
                        If grdLeafWetness.isLoaded Then Output.saveGridToExport grdLeafWetness, "flt", myFilenamePre & Definitions.HOURLY_LEAFWETNESS & myFilenamePost
                    End If

                    'AGGREGATE HOURLY MAPS TO GRID
                    PragaShell.SetInfoOnlyText infoString & "Aggregating hourly maps..."
                    If grdTemperature.isLoaded Then InterpolationCmd.AggregateMeteoGrid Definitions.HOURLY_TAVG, grdTemperature, False
                    If grdPrecipitation.isLoaded Then InterpolationCmd.AggregateMeteoGrid Definitions.HOURLY_PREC, grdPrecipitation, False
                    If grdHumidity.isLoaded Then InterpolationCmd.AggregateMeteoGrid Definitions.HOURLY_RHAVG, grdHumidity, False
                    If grdWindX.isLoaded Then InterpolationCmd.AggregateMeteoGrid Definitions.HOURLY_VMED_X, grdWindX, False
                    If grdWindY.isLoaded Then InterpolationCmd.AggregateMeteoGrid Definitions.HOURLY_VMED_Y, grdWindY, False
                    If grdWindX.isLoaded And grdWindY.isLoaded Then dbGridManagement.gridComputePolarWind Definitions.HOURLY
                    If grdRadiation.isLoaded Then InterpolationCmd.AggregateMeteoGrid Definitions.HOURLY_RAD, grdRadiation, False
                    If grdEtpPenman.isLoaded Then InterpolationCmd.AggregateMeteoGrid Definitions.HOURLY_ET0_PM, grdEtpPenman, False
                    If grdLeafWetness.isLoaded Then InterpolationCmd.AggregateMeteoGrid Definitions.HOURLY_LEAFWETNESS, grdLeafWetness, False

                    grdPrecPreviousHour = grdPrecipitation

                    'FREE MEMORY
                    GIS.clearGrid grdTemperature
                    GIS.clearGrid grdDewTemperature
                    GIS.clearGrid grdHumidity
                    GIS.clearGrid grdPrecipitation
                    GIS.clearGrid grdRadiation
                    GIS.clearGrid grdTransmissivity
                    GIS.clearGrid grdWindX
                    GIS.clearGrid grdWindY
                    GIS.clearGrid grdWindInt
                    GIS.clearGrid grdWindDir
                    GIS.clearGrid grdEtpPenman
                    GIS.clearGrid grdLeafWetness

                Next myHour

                GIS.clearGrid grdPrecPreviousHour

                PragaShell.SetInfoOnlyText "Loading daily data - " & format(myDate, "DD/MM/YYYY")
                If DBPointsManagement.loadPointDaily_Date(myDate, True, Definitions.INFO_NOT_VISUALIZE) Then
                    infoString = format(myDate, "DD/MM/YYYY") & " "
                    Print #myHandleFile, "DAILY VARIABLES"

                    'interpolate daily minimum and maximum temperatures using consistent method
                    PragaShell.SetInfoOnlyText infoString & "Interpolating daily maps: Tmax"
                    If InterpolationSingle(Definitions.DAILY_TMAX, False, False, False) Then
                        grdTmax = GIS.DEM_Var
                        Print #myHandleFile, getInfoInterpolation(Definitions.DAILY_TMAX)
                        If InterpolationSingle(Definitions.DAILY_TMIN, False, False, False) Then
                            grdTmin = GIS.DEM_Var
                            Print #myHandleFile, getInfoInterpolation(Definitions.DAILY_TMIN)
                            If Quality.fixDailyThermalConsistencyRaster(grdTmax, grdTmin) Then
                                Print #myHandleFile, "Temperature range incoherence errors fixed"
                            End If
                        End If
                    End If

                    'interpolate daily precipitation
                    PragaShell.SetInfoOnlyText infoString & "Interpolating daily maps: Prec"
                    If InterpolationSingle(Definitions.DAILY_PREC, False, False, False) Then
                        grdPrecDaily = GIS.DEM_Var
                        Print #myHandleFile, getInfoInterpolation(Definitions.DAILY_PREC)
                    End If

                    'interpolate maximum wind intensity
                    PragaShell.SetInfoOnlyText infoString & "Interpolating daily maps: Max wind intensity"
                    If InterpolationSingle(Definitions.DAILY_VMAX, False, False, False) Then
                        grdWindMax = GIS.DEM_Var
                        Print #myHandleFile, getInfoInterpolation(Definitions.DAILY_VMAX)
                    End If

                    'compute et0 Hargreaves
                    If computeEtpHargreaves Then
                        PragaShell.SetInfoOnlyText infoString & " Computing daily maps: Et0 Hargreaves"
                        If grdTmin.isLoaded And grdTmax.isLoaded Then
                            computeGridEtpHargreaves TimeUtility.DateToDOY(myDate), grdTmin, grdTmax, grdEtpHargreaves
                        End If
                    End If

                    'save daily rasters
                    If saveRasters Then
                        PragaShell.SetInfoOnlyText infoString & "Saving hourly maps..."
                        If grdTmin.isLoaded Then Output.saveGridToExport grdTmin, "flt", myFilenamePre & Definitions.DAILY_TMIN & ".flt"
                        If grdTmax.isLoaded Then Output.saveGridToExport grdTmax, "flt", myFilenamePre & Definitions.DAILY_TMAX & ".flt"
                        If grdPrecDaily.isLoaded Then Output.saveGridToExport grdPrecDaily, "flt", myFilenamePre & Definitions.DAILY_PREC & ".flt"
                        If grdWindMax.isLoaded Then Output.saveGridToExport grdWindMax, "flt", myFilenamePre & Definitions.DAILY_VMAX & ".flt"
                        If grdEtpHargreaves.isLoaded Then Output.saveGridToExport grdEtpHargreaves, "flt", myFilenamePre & Definitions.DAILY_ET0_HS & ".flt"
                    End If

                    'aggregate daily values from dem to grid
                    PragaShell.SetInfoOnlyText infoString & "Aggregating daily maps..."
                    If grdTmin.isLoaded Then InterpolationCmd.AggregateMeteoGrid Definitions.DAILY_TMIN, grdTmin, False
                    If grdTmax.isLoaded Then InterpolationCmd.AggregateMeteoGrid Definitions.DAILY_TMAX, grdTmax, False
                    If grdPrecDaily.isLoaded Then InterpolationCmd.AggregateMeteoGrid Definitions.DAILY_PREC, grdPrecDaily, False
                    If grdWindMax.isLoaded Then InterpolationCmd.AggregateMeteoGrid Definitions.DAILY_VMAX, grdWindMax, False
                    If grdEtpHargreaves.isLoaded Then InterpolationCmd.AggregateMeteoGrid Definitions.DAILY_ET0_HS, grdEtpHargreaves, False
                End If

                dbGridManagement.saveHourly_5VarVM True, True, True, True, True, writeVM, True
                dbGridManagement.AggregateHourlyInDailyGridCurrentDay True, True, True, True, True, False, True, False
                dbGridManagement.saveDaily_5VarVM True, True, True, True, True, writeVM, True

            End If

        Next myDate

        If Interpolation.GetUseTAD Then Erase TadMaps

        PragaShell.StopInfo
        Print #myHandleFile, "Interpolation finished at " & format(Now, "YYYY-MM-DD HH:MM:SS")
        Close #myHandleFile

        InterpolationERG5v2 = True

    End Function
    */
}

bool PragaProject::interpolationMeteoGrid(meteoVariable myVar, frequencyType myFrequency, const Crit3DTime& myTime,
                              gis::Crit3DRasterGrid *myRaster, bool showInfo)
{
    if (meteoGridDbHandler != nullptr)
    {
        if (!interpolationDemMain(myVar, myTime, myRaster, showInfo))
        {
            return false;
        }

        meteoGridDbHandler->meteoGrid()->aggregateMeteoGrid(myVar, myFrequency, myTime.date, myTime.getHour(),
                            myTime.getMinutes(), &DEM, myRaster, interpolationSettings.getMeteoGridAggrMethod());
        meteoGridDbHandler->meteoGrid()->fillMeteoRaster();
    }
    else
    {
        errorString = "Open a Meteo Grid before.";
        return false;
    }

    return true;
}

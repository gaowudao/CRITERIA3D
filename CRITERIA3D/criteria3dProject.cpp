#include "elaborationSettings.h"
#include "formRunInfo.h"
#include "dbTools.h"
#include "utilities.h"
#include "criteria3dProject.h"


Criteria3DProject::Criteria3DProject()
{
    elaborationSettings = new Crit3DElaborationSettings();
    isParametersLoaded = false;
    nrSoils = 0;
    nrLayers = 0;
    nrVoxelsPerLayer = 0;
    nrVoxels = 0;
    soilDepth = 0.3f;            //[m] default: 30 cm
}


bool Criteria3DProject::readCriteria3DSettings()
{          
    Q_FOREACH (QString group, settings->childGroups())
    {
        if (group == "elaboration")
        {
            settings->beginGroup(group);

            if (settings->contains("min_percentage") && !settings->value("min_percentage").toString().isEmpty())
            {
                elaborationSettings->setMinimumPercentage(settings->value("min_percentage").toFloat());
            }
            if (settings->contains("prec_threshold") && !settings->value("prec_threshold").toString().isEmpty())
            {
                elaborationSettings->setRainfallThreshold(settings->value("prec_threshold").toFloat());
            }
            if (settings->contains("anomaly_pts_max_distance") && !settings->value("anomaly_pts_max_distance").toString().isEmpty())
            {
                elaborationSettings->setAnomalyPtsMaxDistance(settings->value("anomaly_pts_max_distance").toFloat());
            }
            if (settings->contains("anomaly_pts_max_delta_z") && !settings->value("anomaly_pts_max_delta_z").toString().isEmpty())
            {
                elaborationSettings->setAnomalyPtsMaxDeltaZ(settings->value("anomaly_pts_max_delta_z").toFloat());
            }
            if (settings->contains("thom_threshold") && !settings->value("thom_threshold").toString().isEmpty())
            {
                elaborationSettings->setThomThreshold(settings->value("thom_threshold").toFloat());
            }
            if (settings->contains("grid_min_coverage") && !settings->value("grid_min_coverage").toString().isEmpty())
            {
                elaborationSettings->setGridMinCoverage(settings->value("grid_min_coverage").toFloat());
            }
            if (settings->contains("samani_coefficient") && !settings->value("samani_coefficient").toString().isEmpty())
            {
                elaborationSettings->setTransSamaniCoefficient(settings->value("samani_coefficient").toFloat());
            }
            if (settings->contains("compute_tmed") && !settings->value("compute_tmed").toString().isEmpty())
            {
                elaborationSettings->setAutomaticTmed(settings->value("compute_tmed").toBool());
            }
            if (settings->contains("compute_et0hs") && !settings->value("compute_et0hs").toString().isEmpty())
            {
                elaborationSettings->setAutomaticETP(settings->value("compute_et0hs").toBool());
            }
            if (settings->contains("merge_joint_stations") && !settings->value("merge_joint_stations").toString().isEmpty())
            {
                elaborationSettings->setMergeJointStations(settings->value("merge_joint_stations").toBool());
            }

            settings->endGroup();
        }
    }
    return true;
}


bool Criteria3DProject::loadModelParameters(QString dbName)
{
    QSqlDatabase dbParameters;
    std::string myError;

    myProject.isParametersLoaded = false;

    dbParameters = QSqlDatabase::addDatabase("QSQLITE", QUuid::createUuid().toString());
    dbParameters.setDatabaseName(dbName);

    if (!dbParameters.open())
    {
       logError("Connection with database fail");
       return false;
    }

    // TODO Load crop parameters

    myProject.isParametersLoaded = true;
    return true;
}

bool Criteria3DProject::loadSoilData(QString dbName)
{
    myProject.soilList.clear();

    std::string myError;
    QSqlDatabase dbSoil;

    dbSoil = QSqlDatabase::addDatabase("QSQLITE", QUuid::createUuid().toString());
    dbSoil.setDatabaseName(dbName);

    if (!dbSoil.open())
    {
       logError("Connection with database fail");
       return false;
    }

    if (! loadVanGenuchtenParameters(myProject.soilClass, &dbSoil, &myError))
    {
        logError(QString::fromStdString(myError));
        return false;
    }

    // query soil list
    QString queryString = "SELECT id_soil, soil_code FROM soils";
    QSqlQuery query = dbSoil.exec(queryString);

    query.first();
    if (! query.isValid())
    {
        if (query.lastError().number() > 0)
            logError(query.lastError().text());
        else
            logError("Error in reading table soils");
        return false;
    }

    // load soil properties
    QString soilCode;
    int idSoil;
    QString wrongSoils = "";
    do
    {
        getValue(query.value("id_soil"), &idSoil);
        getValue(query.value("soil_code"), &soilCode);
        if (idSoil != NODATA && soilCode != "")
        {
            soil::Crit3DSoil *mySoil = new soil::Crit3DSoil;
            if (loadSoil(&dbSoil, soilCode, mySoil, myProject.soilClass, &myError))
            {
                mySoil->id = idSoil;
                myProject.soilList.push_back(*mySoil);
            }
            else
            {
                wrongSoils += " " + soilCode;
            }
        }
    } while(query.next());

    if (myProject.soilList.size() == 0)
    {
       logError("Missing soil properties.");
       return false;
    }
    else if (wrongSoils != "")
    {
        logError("Wrong soils:" + wrongSoils);
    }

    return true;
}


bool Criteria3DProject::loadSoilMap(QString myFileName)
{
    std::string myError;
    std::string fileName = myFileName.left(myFileName.length()-4).toStdString();

    if (! gis::readEsriGrid(fileName, &(myProject.soilMap), &myError))
    {
        logError("Load soil map failed!");
        return (false);
    }

    return (true);
}


bool Criteria3DProject::initializeCriteria3D()
{
    if (!myProject.initializeProject(&DTM, radiationMaps))
    {
        logError(QString::fromStdString(myProject.error));
        return false;
    }

    return true;
}


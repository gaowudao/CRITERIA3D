#include "elaborationSettings.h"
#include "formRunInfo.h"
#include "dbTools.h"
#include "utilities.h"
#include "Crit3DProject.h"
#include "waterBalance3D.h"


Crit3DProject::Crit3DProject()
{
    elaborationSettings = new Crit3DElaborationSettings();
    isParametersLoaded = false;
    nrSoils = 0;
    nrLayers = 0;
    nrVoxelsPerLayer = 0;
    nrVoxels = 0;
    soilDepth = 0.3f;            //[m] default: 30 cm
}


bool Crit3DProject::readCriteria3DSettings()
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


bool Crit3DProject::loadModelParameters(QString dbName)
{
    QSqlDatabase dbParameters;

    isParametersLoaded = false;

    dbParameters = QSqlDatabase::addDatabase("QSQLITE", QUuid::createUuid().toString());
    dbParameters.setDatabaseName(dbName);

    if (!dbParameters.open())
    {
       logError("Connection with database fail");
       return false;
    }

    // TODO Load crop parameters

    isParametersLoaded = true;
    return true;
}


bool Crit3DProject::loadSoilData(QString dbName)
{
    soilList.clear();

    std::string myError;
    QSqlDatabase dbSoil;

    dbSoil = QSqlDatabase::addDatabase("QSQLITE", QUuid::createUuid().toString());
    dbSoil.setDatabaseName(dbName);

    if (!dbSoil.open())
    {
       logError("Connection with database fail");
       return false;
    }

    if (! loadVanGenuchtenParameters(soilClass, &dbSoil, &myError))
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
            if (loadSoil(&dbSoil, soilCode, mySoil, soilClass, &myError))
            {
                mySoil->id = idSoil;
                soilList.push_back(*mySoil);
            }
            else
            {
                wrongSoils += " " + soilCode;
            }
        }
    } while(query.next());

    if (soilList.size() == 0)
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


bool Crit3DProject::loadSoilMap(QString myFileName)
{
    std::string myError;
    std::string fileName = myFileName.left(myFileName.length()-4).toStdString();

    if (! gis::readEsriGrid(fileName, &soilMap, &myError))
    {
        logError("Load soil map failed!");
        return (false);
    }

    return (true);
}


bool Crit3DProject::createIndexMap()
{
    // check
    if (! DTM.isLoaded)
    {
        logError("Missing DTM.");
        return false;
    }

    indexMap.initializeGrid(*(DTM.header));

    long index = 0;
    for (int row = 0; row < indexMap.header->nrRows; row++)
    {
        for (int col = 0; col < indexMap.header->nrCols; col++)
        {
            if (DTM.value[row][col] != DTM.header->flag)
            {
                indexMap.value[row][col] = float(index);
                index++;
            }
        }
    }

    indexMap.isLoaded = true;
    nrVoxelsPerLayer = index;
    return(nrVoxelsPerLayer > 0);
}


bool Crit3DProject::createBoundaryMap()
{
    // check
    if (! DTM.isLoaded)
    {
        logError("Missing DTM.");
        return false;
    }
    boundaryMap.initializeGrid(*(DTM.header));

    for (int row = 0; row < boundaryMap.header->nrRows; row++)
    {
        for (int col = 0; col < boundaryMap.header->nrCols; col++)
        {
            if (gis::isBoundary(DTM, row, col))
            {
                if (gis::isMinimum(DTM, row, col))
                    boundaryMap.value[row][col] = BOUNDARY_RUNOFF;
            }
        }
    }

    boundaryMap.isLoaded = true;
    return true;
}


double Crit3DProject::getSoilVar(int soilIndex, int layerIndex, soil::soilVariable myVar)
{
    int horizonIndex = soil::getHorizonIndex(&(soilList[soilIndex]), layerDepth[layerIndex]);
    if (myVar == soil::soilWaterPotentialWP)
        return soilList[soilIndex].horizon[horizonIndex].wiltingPoint;
    else if (myVar == soil::soilWaterPotentialFC)
        return soilList[soilIndex].horizon[horizonIndex].fieldCapacity;
    else if (myVar == soil::soilWaterContentFC)
        return soilList[soilIndex].horizon[horizonIndex].waterContentFC;
    else if (myVar == soil::soilWaterContentSat)
        return soilList[soilIndex].horizon[horizonIndex].vanGenuchten.thetaS;
    else if (myVar == soil::soilWaterContentWP)
    {
        double signPsiLeaf = -160;      //[m]
        return soil::thetaFromSignPsi(signPsiLeaf, &(soilList[soilIndex].horizon[horizonIndex]));
    }
    else
        return NODATA;
}


bool Crit3DProject::createSoilIndexMap()
{
    // check
    if (!DTM.isLoaded || !soilMap.isLoaded || soilList.size() == 0)
    {
        if (!DTM.isLoaded)
            logError("Missing DTM.");
        else if (!soilMap.isLoaded)
            logError("Missing soil map.");
        else if (soilList.size() == 0)
            logError("Missing soil properties.");
        return false;
    }

    int soilIndex;
    soilIndexMap.initializeGrid(*(DTM.header));
    for (int row = 0; row < DTM.header->nrRows; row++)
    {
        for (int col = 0; col < DTM.header->nrCols; col++)
        {
            if (DTM.value[row][col] != DTM.header->flag)
            {
                soilIndex = getSoilIndex(row, col);
                if (soilIndex != INDEX_ERROR)
                    soilIndexMap.value[row][col] = float(soilIndex);
            }
        }
    }

    soilIndexMap.isLoaded = true;
    return true;
}


void Crit3DProject::cleanProject()
{
    soilIndexMap.freeGrid();
    cropIndexMap.freeGrid();
    boundaryMap.freeGrid();
    indexMap.freeGrid();

    delete meteoMaps;

    cleanWaterBalanceMemory();
}


bool Crit3DProject::initializeCriteria3D()
{
    cleanProject();

    meteoMaps = new Crit3DMeteoMaps(DTM);

    if (!createSoilIndexMap())
        return false;

    // loadCropProperties()
    // load crop map

    if (! initializeWaterBalance(this))
    {
        cleanProject();
        return false;
    }

    //initialize root density
    //TO DO: andrebbe rifatto per ogni tipo di suolo
    //(ora considera solo suolo 0)
    /*
    int nrSoilLayersWithoutRoots = 2;
    int soilLayerWithRoot = this->nrSoilLayers - nrSoilLayersWithoutRoots;
    double depthModeRootDensity = 0.35*this->soilDepth;     //[m] depth of mode of root density
    double depthMeanRootDensity = 0.5*this->soilDepth;      //[m] depth of mean of root density
    initializeRootProperties(&(this->soilList[0]), this->nrSoilLayers, this->soilDepth,
                         this->layerDepth.data(), this->layerThickness.data(),
                         nrSoilLayersWithoutRoots, soilLayerWithRoot,
                         GAMMA_DISTRIBUTION, depthModeRootDensity, depthMeanRootDensity);*/

    log("Criteria3D Project initialized");

    return(true);
}


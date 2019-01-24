#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDir>
#include <QDateTime>

#include <iostream>

#include "formInfo.h"
#include "utilities.h"
#include "commonConstants.h"
#include "meteo.h"
#include "interpolation.h"
#include "solarRadiation.h"
#include "waterBalance.h"
#include "plant.h"
#include "dataHandler.h"
#include "modelCore.h"
#include "parserXML.h"
#include "atmosphere.h"
#include "disease.h"
#include "vine3DProject.h"


void Vine3DProject::initialize()
{
    idArea = "0000";
    dbProvider = "QPSQL";
    dbHostname = "127.0.0.1";
    dbName = "";
    dbPort = 5432;
    dbUsername = "postgres";
    dbPassword = "postgres";

    isObsDataLoaded = false;
    setFrequency(hourly);

    isProjectLoaded = false;

    dailyOutputPath = "daily_output/";
    demFileName = "";
    fieldMapName = "";
    logFileName = "";
    projectError = "";

    lastDateTransmissivity.setDate(1900,1,1);

    nrCultivar = 0;
    nrSoils = 0;
    nrSoilLayers = 0;
    nrLayerNodes = 0;
    nrVineFields = 0;
    nrNodes = 0;
    soilDepth = 1.0;       //[m]

    statePlant.stateGrowth.initialize();
    statePlant.statePheno.initialize();
}


Vine3DProject::Vine3DProject()
{
    initialize();
}

bool Vine3DProject::loadVine3DSettings()
{
    //eventually put Vine3D generic settings
    return true;
}

void Vine3DProject::deleteAllGrids()
{
    DTM.freeGrid();
    modelCaseMap.freeGrid();
    boundaryMap.freeGrid();
    indexMap.freeGrid();
    interpolatedDtm.freeGrid();

    delete meteoMaps;
}

void Vine3DProject::closeProject()
{
    if (this->isProjectLoaded)
    {
        this->logInfo("Close Project");
        this->dbConnection.close();
        this->logFile.close();
        this->initializeMeteoPoints();
        this->deleteAllGrids();

        cleanWaterBalanceMemory();

        this->isProjectLoaded = false;
    }
}

void Vine3DProject::setEnvironment(Tenvironment myEnvironment)
{
    environment = myEnvironment;
}

QString Vine3DProject::getGeoserverPath()
{
    return (path + "geoserver/");
}

bool Vine3DProject::loadVine3DProjectSettings(QString projectFile)
{
    if (! QFile(projectFile).exists())
    {
        logError("Project file not found: " + projectFile);
        return false;
    }

    initialize();

    if (! loadCommonSettings(projectFile))
    {
        logError("Error reading common settings in " + projectFile);
        return false;
    }

    projectSettings->beginGroup("location");
        int timeZone = projectSettings->value("timezone").toInt();
    projectSettings->endGroup();

    gisSettings.timeZone = timeZone;

    projectSettings->beginGroup("project");
        QString myId = projectSettings->value("id").toString();
        QString projectName = projectSettings->value("name").toString();
        QString demName = projectSettings->value("dem").toString();
        QString fieldName = projectSettings->value("modelCaseMap").toString();
    projectSettings->endGroup();

    idArea = myId;
    demFileName = demName;
    fieldMapName = fieldName;

    projectSettings->beginGroup("settings");
    QString paramFile = path + projectSettings->value("parameters_file").toString();
    float depth = projectSettings->value("soil_depth").toFloat();
    projectSettings->endGroup();

    soilDepth = depth;
    parametersFile = paramFile;

    return true;

}


bool Vine3DProject::loadProject(QString myFileName)
{
    closeProject();
    this->initialize();

    if (myFileName == "") return(false);
    if (! loadVine3DProjectSettings(myFileName))
        return false;

    if (! loadParameters(parametersFile))
        return false;

    if (this->setLogFile())
        this->logInfo("Set LogFile: " + this->logFileName);
    else
        this->logError("LogFile Wrong.");

    myFileName = path + demFileName;
    if (loadDEM(myFileName))
    {
        this->logInfo("Initialize DTM and project maps...");
        meteoMaps = new Crit3DMeteoMaps(DTM, this->gisSettings);
        statePlantMaps = new Crit3DStatePlantMaps(DTM);
    }
    else
        return(false);

    if (! openDBConnection()) return (false);

    if (!loadFieldShape())
    {
        myFileName = path + fieldMapName;
        if (!loadFieldMap(myFileName)) return false;
    }

    if (!loadVine3DProjectParameters() || !loadSoils() || !loadTrainingSystems()
        || !loadAggregatedMeteoVarCodes() || !loadDBPoints())
    {
        logError();
        dbConnection.close();
        return(false);
    }

    if (!loadFieldsProperties() || !loadFieldBook())
    {
        logError();
        dbConnection.close();
        return(false);
    }

    this->isProjectLoaded = true;

    if (! initializeWaterBalance(this))
    {
        this->logError();
        return(false);
    }

    this->currentProfile = (double *) calloc(this->nrSoilLayers, sizeof(double));

    outputPlantMaps = new Crit3DOutputPlantMaps(DTM, this->nrSoilLayers);

    //initialize root density
    //TO DO: andrebbe rifatto per ogni tipo di suolo
    //(ora considera solo suolo 0)
    int nrSoilLayersWithoutRoots = 2;
    int soilLayerWithRoot = this->nrSoilLayers - nrSoilLayersWithoutRoots;
    double depthModeRootDensity = 0.35*this->soilDepth;     //[m] depth of mode of root density
    double depthMeanRootDensity = 0.5*this->soilDepth;      //[m] depth of mean of root density
    this->grapevine.initializeRootProperties(&(this->soilList[0]), this->nrSoilLayers, this->soilDepth,
                         this->layerDepth.data(), this->layerThickness.data(),
                         nrSoilLayersWithoutRoots, soilLayerWithRoot,
                         GAMMA_DISTRIBUTION, depthModeRootDensity, depthMeanRootDensity);

    logInfo("Project loaded");
    return(true);
}


bool Vine3DProject::loadGrapevineParameters()
{
    logInfo ("Read Grapevine parameters->..");
    QString myQueryString =
            " SELECT id_cultivar, name,"
            " phenovitis_force_physiological_maturity, miglietta_radiation_use_efficiency,"
            " miglietta_d, miglietta_f, miglietta_fruit_biomass_offset,"
            " miglietta_fruit_biomass_slope,"
            " hydrall_psileaf, hydrall_stress_threshold,"
            " hydrall_vpd , hydrall_alpha_leuning,"
            " phenovitis_ecodormancy, phenovitis_critical_chilling,"
            " phenovitis_force_flowering, phenovitis_force_veraison,"
            " phenovitis_force_fruitset, degree_days_veraison, hydrall_carbox_rate"
            " FROM cultivar"
            " ORDER BY id_cultivar";

    QSqlQuery myQuery = dbConnection.exec(myQueryString);
    if (myQuery.size() == -1)
    {
        this->projectError = "wrong Grapevine parameters" + myQuery.lastError().text();
        return(false);
    }
    //initialize vines
    this->nrCultivar = myQuery.size();
    this->cultivar = (TVineCultivar *) calloc(this->nrCultivar, sizeof(TVineCultivar));

    //read values
    int i = 0;
    while (myQuery.next())
    {
        this->cultivar[i].id = myQuery.value(0).toInt();
        //strcpy(this->cultivar[i].name.cultivar, myQuery.value(1).toString().toStdString().c_str());
        this->cultivar[i].parameterPhenoVitis.criticalForceStatePhysiologicalMaturity = myQuery.value(2).toFloat();
        this->cultivar[i].parameterBindiMiglietta.radiationUseEfficiency = myQuery.value(3).toFloat();
        this->cultivar[i].parameterBindiMiglietta.d = myQuery.value(4).toFloat();
        this->cultivar[i].parameterBindiMiglietta.f = myQuery.value(5).toFloat();
        this->cultivar[i].parameterBindiMiglietta.fruitBiomassOffset = myQuery.value(6).toFloat();
        this->cultivar[i].parameterBindiMiglietta.fruitBiomassSlope = myQuery.value(7).toFloat();
        this->cultivar[i].parameterWangLeuning.psiLeaf = myQuery.value(8).toFloat();
        this->cultivar[i].parameterWangLeuning.waterStressThreshold = myQuery.value(9).toFloat();
        this->cultivar[i].parameterWangLeuning.sensitivityToVapourPressureDeficit = myQuery.value(10).toFloat();
        this->cultivar[i].parameterWangLeuning.alpha = myQuery.value(11).toFloat() * 1E5;
        this->cultivar[i].parameterPhenoVitis.co1 = myQuery.value(12).toFloat();
        this->cultivar[i].parameterPhenoVitis.criticalChilling = myQuery.value(13).toFloat();
        this->cultivar[i].parameterPhenoVitis.criticalForceStateFlowering = myQuery.value(14).toFloat();
        this->cultivar[i].parameterPhenoVitis.criticalForceStateVeraison = myQuery.value(15).toFloat();
        this->cultivar[i].parameterPhenoVitis.criticalForceStateFruitSet = myQuery.value(16).toFloat();
        this->cultivar[i].parameterPhenoVitis.degreeDaysAtVeraison = myQuery.value(17).toFloat();
        this->cultivar[i].parameterWangLeuning.maxCarboxRate = myQuery.value(18).toFloat();
        i++;
    }

    return(true);
}


bool Vine3DProject::loadTrainingSystems()
{
    logInfo ("Read training system...");
    QString myQueryString =
            " SELECT id_training_system, nr_shoots_plant, row_width, row_height,"
            " row_distance, plant_distance"
            " FROM training_system"
            " ORDER BY id_training_system";

    QSqlQuery myQuery = dbConnection.exec(myQueryString);

    if (myQuery.size() < 1)
    {
        this->projectError = "missing training system" + myQuery.lastError().text();
        return false;
    }
    //initialize training system
    this->nrTrainingSystems = myQuery.size();
    this->trainingSystems = (TtrainingSystem *) calloc(this->nrTrainingSystems, sizeof(TtrainingSystem));

    //read values
    int i = 0;
    while (myQuery.next())
    {
        this->trainingSystems[i].id = myQuery.value(0).toInt();
        this->trainingSystems[i].shootsPerPlant = myQuery.value(1).toFloat();
        this->trainingSystems[i].rowWidth = myQuery.value(2).toFloat();
        this->trainingSystems[i].rowHeight = myQuery.value(3).toFloat();
        this->trainingSystems[i].rowDistance = myQuery.value(4).toFloat();
        this->trainingSystems[i].plantDistance = myQuery.value(5).toFloat();
        i++;
    }

    return(true);
}


bool Vine3DProject::loadFieldBook()
{
    QDate myDate;
    int i, nrOperations, idField;

    logInfo ("Read field book table...");
    QString myQueryString =
            " SELECT date_, id_field, irrigated, grass, pinchout, leaf_removal,"
            " harvesting_performed, cluster_thinning, tartaric_acid, irrigation_hours, thinning_percentage"
            " FROM field_book"
            " ORDER BY date_, id_field";

    QSqlQuery myQuery = dbConnection.exec(myQueryString);

    if (myQuery.size() == -1)
    {
        this->projectError = "missing field_book\n" + myQuery.lastError().text();
        return false;
    }

    //count number of operations
    nrOperations = 0;
    while (myQuery.next())
    {
        for(i=2; i<=8; i++)
        {
            if (myQuery.value(i).toFloat() > 0.0)
                nrOperations++;
        }
    }
    this->nrFieldOperations = nrOperations;
    this->fieldBook = (TfieldBook *) calloc(this->nrFieldOperations, sizeof(TfieldBook));

    // read values
    myQuery.first();
    int idBook = 0;
    while (idBook < this->nrFieldOperations)
    {
        myDate = myQuery.value(0).toDate();
        idField = myQuery.value(1).toInt();

        nrOperations = 0;
        for(i=2; i<=8; i++)
        {
            if (myQuery.value(i).toFloat() > 0.0)
                nrOperations++;
        }
        i = 2;

        while (nrOperations > 0)
        {
            this->fieldBook[idBook].idField = idField;
            this->fieldBook[idBook].operationDate = myDate;
            if (myQuery.value(i).toFloat() > 0)
            {
                //irrigation
                if (i == 2)
                {
                    this->fieldBook[idBook].operation = irrigationOperation;
                    this->fieldBook[idBook].quantity = myQuery.value(9).toFloat();
                }
                //grass sowing/removal
                if (i == 3)
                {
                    if (myQuery.value(3).toInt() == 1)
                        this->fieldBook[idBook].operation = grassSowing;
                    if (myQuery.value(3).toInt() > 1)
                        this->fieldBook[idBook].operation = grassRemoving;
                    this->fieldBook[idBook].quantity = 0.0;
                }
                //pinchout == trimming
                if (i == 4)
                {
                    this->fieldBook[idBook].operation = trimming;
                    this->fieldBook[idBook].quantity = 2.5;
                }
                //leaf removal
                if (i == 5)
                {
                    this->fieldBook[idBook].operation = leafRemoval;
                    this->fieldBook[idBook].quantity = 3.0;
                }
                //harvesting
                if (i == 6)
                {
                    this->fieldBook[idBook].operation = harvesting;
                    this->fieldBook[idBook].quantity = 0.0;
                }
                //cluster thinning
                if (i == 7)
                {
                    this->fieldBook[idBook].operation = clusterThinning;
                    this->fieldBook[idBook].quantity = myQuery.value(10).toFloat();
                }
                //tartaric acid analysis
                if (i == 8)
                {
                    this->fieldBook[idBook].operation = tartaricAnalysis;
                    this->fieldBook[idBook].quantity = myQuery.value(i).toFloat();
                }

                nrOperations--;
                idBook++;
            }
            i++;
        }
        myQuery.next();
    }

    return(true);
}


int Vine3DProject::queryFieldPoint(double x, double y)
{
    QString UTMx = QString::number(x, 'f', 1);
    QString UTMy = QString::number(y, 'f', 1);
    QString UTMZone = QString::number(this->gisSettings.utmZone);

    QString myQueryString = "SELECT id_field FROM fields_shp";
    myQueryString += " WHERE ST_Contains(geom, ST_GeomFromText";
    myQueryString += "('POINT(" + UTMx + " " + UTMy + ")', 326" + UTMZone + ")) = true";
    QSqlQuery myQuery;

    myQuery = this->dbConnection.exec(myQueryString);

    if (myQuery.size() == -1)
    {
        this->projectError = myQuery.lastError().text();
        this->logError();
        return(NODATA);
    }

    if (myQuery.size() > 0)
    {
        myQuery.next();
        return myQuery.value(0).toInt();
    }
    else
        return NODATA;
}


bool Vine3DProject::loadFieldShape()
{
    this->logInfo ("Read Fields...");
    int dim = 1;
    int i, j, id;
    double x0, y0;
    std::vector <float> valuesList;

    QString myQueryString = "SELECT id_field FROM fields_shp";

    QSqlQuery myQuery;
    myQuery = this->dbConnection.exec(myQueryString);
    if (myQuery.size() == -1)
    {
        this->projectError = myQuery.lastError().text();
        return(false);
    }
    myQuery.clear();

    this->modelCaseMap.initializeGrid(this->DTM);

    double step = this->modelCaseMap.header->cellSize / (2*dim+1);

    for (long row = 0; row < this->modelCaseMap.header->nrRows ; row++)
        for (long col = 0; col < this->modelCaseMap.header->nrCols; col++)
            if (this->DTM.value[row][col] != this->DTM.header->flag)
            {
                //center
                gis::getUtmXYFromRowCol(this->modelCaseMap, row, col, &x0, &y0);
                id = queryFieldPoint(x0, y0);
                if (id != NODATA)
                    this->modelCaseMap.value[row][col] = id;
                else
                {
                    valuesList.resize(0);
                    for (i = -dim; i <= dim; i++)
                        for (j = -dim; j <= dim; j++)
                            if ((i != 0)|| (j != 0))
                            {
                                id = queryFieldPoint(x0+(i*step), y0+(j*step));
                                if (id != NODATA)
                                    valuesList.push_back(id);
                            }
                    if (valuesList.size() == 0)
                        this->modelCaseMap.value[row][col] = this->modelCaseMap.header->flag;
                    else
                        this->modelCaseMap.value[row][col] = gis::prevailingValue(valuesList);
                }
            }

    gis::updateMinMaxRasterGrid(&(this->modelCaseMap));
    this->nrVineFields = int(this->modelCaseMap.maximum);
    return true;
}


bool Vine3DProject::loadFieldMap(QString myFileName)
{
    this->logInfo ("Read field map " + myFileName);

    std::string fn = myFileName.left(myFileName.length()-4).toStdString();
    std::string* myError = new std::string();
    gis::Crit3DRasterGrid myGrid;

    if (! gis::readEsriGrid(fn, &(myGrid), myError))
    {
        this->projectError = "Load fields map failed:\n" + myFileName + "\n" + QString::fromStdString(*myError);
        logError();
        return (false);
    }
    else
    {
        // compute prevailing map
        modelCaseMap.initializeGrid(DTM);
        gis::prevailingMap(myGrid, &(modelCaseMap));

        gis::updateMinMaxRasterGrid(&(modelCaseMap));
        this->nrVineFields = modelCaseMap.maximum;

        return (true);
    }
}


bool Vine3DProject::setField(int fieldIndex, Crit3DLanduse landuse, int soilIndex, int vineIndex, int trainingIndex,
                             float maxLaiGrass, float maxIrrigationRate)
{
    if (fieldIndex >= nrVineFields) return false;

    int i = fieldIndex;
    modelCases[i].id = fieldIndex;

    modelCases[i].landuse = landuse;
    modelCases[i].soilIndex = soilIndex;

    modelCases[i].cultivar = &(this->cultivar[vineIndex]);
    modelCases[i].maxLAIGrass = maxLaiGrass;
    modelCases[i].maxIrrigationRate = maxIrrigationRate;

    float density = 1.0 / (this->trainingSystems[trainingIndex].rowDistance
                           * this->trainingSystems[trainingIndex].plantDistance);

    modelCases[i].trainingSystem = trainingIndex;
    modelCases[i].plantDensity = density;
    modelCases[i].shootsPerPlant = this->trainingSystems[trainingIndex].shootsPerPlant;

    return true;
}


bool Vine3DProject::readFieldQuery(QSqlQuery myQuery, int* idField, Crit3DLanduse* landuse, int* vineIndex, int* trainingIndex,
                                   int* soilIndex, float* maxLaiGrass, float* maxIrrigationRate)
{
    int i, idCultivar, idTraining, idSoil;

    *idField = myQuery.value("id_field").toInt();

    //LANDUSE
    std::string landuse_name = myQuery.value("landuse").toString().toStdString();
    if (landuseNames.find(landuse_name) == landuseNames.end())
    {
        this->projectError = "Unknown landuse for field " + *idField;
        return false;
    }
    else
        *landuse = landuseNames.at(landuse_name);

    //CULTIVAR
    idCultivar = myQuery.value("id_cultivar").toInt();
    i=0;
    while (i < this->nrCultivar && idCultivar != cultivar[i].id) i++;
    if (i == this->nrCultivar)
    {
        this->projectError = "cultivar " + QString::number(idCultivar) + " not found" + myQuery.lastError().text();
        return false;
    }
    *vineIndex = i;

    //TRAINING SYSTEM
    idTraining = myQuery.value("id_training_system").toInt();
    i=0;
    while (i < this->nrTrainingSystems && idTraining != this->trainingSystems[i].id) i++;
    if (i == this->nrTrainingSystems)
    {
        this->projectError = "training system nr." + QString::number(idTraining) + " not found" + myQuery.lastError().text();
        return false;
    }
    *trainingIndex = i;

    //SOIL
    idSoil = myQuery.value("id_soil").toInt();
    i=0;
    while (i < this->nrSoils && idSoil != soilList[i].id) i++;
    if (i == this->nrSoils)
    {
        this->projectError = "soil " + QString::number(idSoil) + " not found" + myQuery.lastError().text();
        return false;
    }
    *soilIndex = i;


    *maxLaiGrass = myQuery.value("max_lai_grass").toFloat();
    *maxIrrigationRate = myQuery.value("irrigation_max_rate").toFloat();

    return true;
}


bool Vine3DProject::loadFieldsProperties()
{
    logInfo ("Read fields properties...");

    // READ NUMBER OF FIELDS
    QString myQueryString = " SELECT max(id_field) FROM fields";
    QSqlQuery myQuery = dbConnection.exec(myQueryString);
    if (myQuery.size() == -1)
    {
        this->projectError = "NO DATA in the table 'fields'" + myQuery.lastError().text();
        return(false);
    }

    myQuery.next();
    int maxFieldIndex = myQuery.value(0).toInt();
    // check number of fields
    if (maxFieldIndex != this->nrVineFields)
    {
        this->logInfo("\nWarning! The number of field in the DB is different from the number in the fields map."
                      "\nSome fields will be set to default parameters->\n");
    }

    this->nrVineFields = maxValue(this->nrVineFields, maxFieldIndex) +1;

    //alloc memory for vines
    this->modelCases = (Crit3DModelCase *) calloc(this->nrVineFields, sizeof(Crit3DModelCase));

    // READ DEFAULT FIELD PROPERTIES
    int idField, vineIndex, trainingIndex, soilIndex;
    float maxLaiGrass, maxIrrigationRate;
    Crit3DLanduse landuse;

    myQueryString =
            " SELECT id_field, landuse, id_cultivar,"
            " id_training_system, id_soil, max_lai_grass, irrigation_max_rate"
            " FROM fields"
            " WHERE id_field=0";
    myQuery = dbConnection.exec(myQueryString);

    if (myQuery.size() == -1)
    {
        this->projectError = "Wrong structure in table 'fields'\n" + myQuery.lastError().text();
        return(false);
    }
    // missing default value
    if (myQuery.size() == 0)
    {
        this->projectError = "Default field (index = 0) is missing in the table 'fields'";
        return(false);
    }

    myQuery.next();
    if (!readFieldQuery(myQuery, &idField, &landuse, &vineIndex, &trainingIndex, &soilIndex, &maxLaiGrass, &maxIrrigationRate))
        return false;

    // INITIALIZE FIELDS
    for (int i = 0; i<nrVineFields; i++)
        setField(i, landuse, soilIndex, vineIndex, trainingIndex, maxLaiGrass, maxIrrigationRate);

   // READ PROPERTIES
    myQueryString =
            " SELECT id_field, id_cultivar,"
            " id_training_system, id_soil, max_lai_grass, irrigation_max_rate"
            " FROM fields"
            " ORDER BY id_field";
    myQuery = dbConnection.exec(myQueryString);

    if (myQuery.size() == -1)
    {
        this->projectError = "Wrong structure in table 'fields'\n" + myQuery.lastError().text();
        return(false);
    }

    // SET PROPERTIES
    while (myQuery.next())
    {
       if (readFieldQuery(myQuery, &idField, &landuse, &vineIndex, &trainingIndex, &soilIndex, &maxLaiGrass, &maxIrrigationRate))
            setField(idField, landuse, soilIndex, vineIndex, trainingIndex, maxLaiGrass, maxIrrigationRate);
    }

    return(true);
}


bool Vine3DProject::loadClimateParameters()
{
    logInfo ("Read climate parameters->..");
    QString myQueryString = "SELECT month, tmin_lapse_rate, tmax_lapse_rate, tdmin_lapse_rate, tdmax_lapse_rate";
    myQueryString += " FROM climate";
    myQueryString += " ORDER BY month";

    QSqlQuery myQuery = dbConnection.exec(myQueryString);
    if (myQuery.size() == -1)
    {
        this->projectError = myQuery.lastError().text();
        return(false);
    }
    else if (myQuery.size() != 12)
    {
        this->projectError = "wrong number of climate records (must be 12)";
        return(false);
    }

    //read values
    Crit3DClimateParameters myParameters;
    int i;
    while (myQuery.next())
    {
        i = myQuery.value(0).toInt();
        myParameters.tminLapseRate[i-1] = myQuery.value(1).toFloat();
        myParameters.tmaxLapseRate[i-1] = myQuery.value(2).toFloat();
        myParameters.tDewMinLapseRate[i-1] = myQuery.value(3).toFloat();
        myParameters.tDewMaxLapseRate[i-1] = myQuery.value(4).toFloat();
    }

    interpolationSettings.setClimateParameters(&myParameters);

    return(true);

}

bool Vine3DProject::loadVine3DProjectParameters()
{
    if (!loadClimateParameters()) return false;
    if (!loadVanGenuchtenParameters()) return false;
    if (!loadGrapevineParameters()) return false;

    return true;
}

bool Vine3DProject::loadAggregatedMeteoVarCodes()
{
    logInfo ("Reading aggregated variables codes...");
    QString myQueryString = "SELECT id_variable, aggregated_var_code";
    myQueryString += " FROM variables";
    myQueryString += " ORDER BY id_variable";

    QSqlQuery myQuery = dbConnection.exec(myQueryString);
    if (myQuery.size() == -1)
    {
        this->projectError = myQuery.lastError().text();
        return(false);
    }

    this->nrAggrVar = myQuery.size();
    this->varCodes = (int *) calloc(this->nrAggrVar, sizeof(int));
    this->aggrVarCodes = (int *) calloc(this->nrAggrVar, sizeof(int));

    int i=0;
    while (myQuery.next())
    {
        this->varCodes[i] = myQuery.value(0).toInt();
        this->aggrVarCodes[i] = NODATA;
        if (!myQuery.value(1).isNull())
            this->aggrVarCodes[i] = myQuery.value(1).toInt();
        i++;
    }

    return(true);
}

bool Vine3DProject::loadVanGenuchtenParameters()
{
    logInfo ("Read soil parameters->..");

    QString queryString = "SELECT id_texture, alpha, n, he, theta_r, theta_s, ksat, l";
    queryString += " FROM soil_van_genuchten";
    queryString += " ORDER BY id_texture";

    QSqlQuery query = dbConnection.exec(queryString);
    if (query.size() == -1)
    {
        this->projectError = "Table 'soil_van_genuchten': " + query.lastError().text();
        return(false);
    }
    else if (query.size() != 12)
    {
        this->projectError = "Table 'soil_van_genuchten' Wrong number of soil textures (must be 12)";
        return(false);
    }

    //read values
    int id, j;
    float myValue;
    while (query.next())
    {
        id = query.value(0).toInt();
        //check data
        for (j = 0; j <= 7; j++)
            if (! getValue(query.value(j), &myValue))
            {
                this->projectError = "Table 'soil_van_genuchten' Missing data in soil texture:" + QString::number(id);
                return(false);
            }
        soilClass[id].vanGenuchten.alpha = query.value(1).toDouble();
        soilClass[id].vanGenuchten.n = query.value(2).toDouble();
        soilClass[id].vanGenuchten.he = query.value(3).toDouble();
        soilClass[id].vanGenuchten.thetaR = query.value(4).toDouble();
        soilClass[id].vanGenuchten.thetaS = query.value(5).toDouble();
        soilClass[id].waterConductivity.kSat = query.value(6).toDouble();
        soilClass[id].waterConductivity.l = query.value(7).toDouble();

        soilClass[id].vanGenuchten.m = 1.0 - 1.0 / soilClass[id].vanGenuchten.n;
        soilClass[id].vanGenuchten.sc = pow(1.0 + pow(soilClass[id].vanGenuchten.alpha
                                        * soilClass[id].vanGenuchten.he, soilClass[id].vanGenuchten.n), -soilClass[id].vanGenuchten.m);
        soilClass[id].vanGenuchten.refThetaS = soilClass[id].vanGenuchten.thetaS;
    }

    return(true);
}


bool Vine3DProject::loadHorizons(soil::Crit3DSoil* outputSoil, int idSoil, QString soil_code)
{
    QString queryString = "SELECT soil_code, horizon_nr, upper_depth, lower_depth, coarse_fragment, ";
    queryString += "organic_matter, sand, silt, clay, bulk_density, theta_sat, ksat FROM horizons ";
    queryString += "WHERE soil_code='" + soil_code + "' ORDER BY horizon_nr";

    QSqlQuery query = dbConnection.exec(queryString);
    if (query.size() < 1)
    {
        if (query.size() == -1)
            this->projectError = "Table 'horizons' " + query.lastError().text();
        else if (query.size() == 0)
            this->projectError = "Missing horizons in soil nr:" + QString::number(idSoil);
        return(false);
    }

    int nrHorizons = query.size();
    soil::Crit3DSoil* mySoil = new soil::Crit3DSoil(idSoil, nrHorizons);

    int idHorizon;
    double sand, silt, clay, organicMatter, skeleton;
    double bulkDensity, porosity, ksat;
    int i = 0;
    while (query.next())
    {
        //check depth
        idHorizon = query.value("horizon_nr").toInt();
        getValue(query.value("upper_depth"), &(mySoil->horizon[i].upperDepth));
        getValue(query.value("lower_depth"), &(mySoil->horizon[i].lowerDepth));
        if ((mySoil->horizon[i].upperDepth == NODATA) || (mySoil->horizon[i].lowerDepth == NODATA)
                || (mySoil->horizon[i].lowerDepth < mySoil->horizon[i].upperDepth))
        {
            this->projectError = "Wrong depths in soil " + QString::number(idSoil)
                    + " horizon " + QString::number(idHorizon);
            return(false);
        }
        //[cm]->[m]
        mySoil->horizon[i].upperDepth /= 100.0;
        mySoil->horizon[i].lowerDepth /= 100.0;

        //check skeleton
        getValue(query.value("coarse_fragment"), &skeleton);
        if (skeleton == NODATA)
            mySoil->horizon[i].coarseFragments = 0.;
        else
            //[0,1]
            mySoil->horizon[i].coarseFragments = skeleton / 100.0;

        //texture
        getValue(query.value("sand"), &sand);
        getValue(query.value("silt"), &silt);
        getValue(query.value("clay"), &clay);

        if ((sand == NODATA) || (silt == NODATA) || (clay == NODATA))
        {
            this->projectError = "Missing texture data in soil " + soil_code + " horizon " + QString::number(idHorizon);
            return (false);
        }

        //check Texture
        int idTexture = soil::getUSDATextureClass(sand*100, silt*100, clay*100);
        if (idTexture == NODATA)
        {
            this->projectError = "Wrong texture for soil " + soil_code + " horizon " + QString::number(idHorizon);
            return (false);
        }

        // TODO
        if (clay == NODATA) clay = 0.25;
        mySoil->horizon[i].texture.sand = sand;
        mySoil->horizon[i].texture.silt = silt;
        mySoil->horizon[i].texture.clay = clay;

        //default values
        mySoil->horizon[i].texture.classUSDA = idTexture;
        mySoil->horizon[i].vanGenuchten = soilClass[idTexture].vanGenuchten;
        mySoil->horizon[i].waterConductivity = soilClass[idTexture].waterConductivity;

        //organic matter
        getValue(query.value("organic_matter"), &organicMatter);
        if (organicMatter == NODATA)
            organicMatter = 0.005;      //minimum value: 0.5%
        else
            organicMatter /= 100.0;     //[0-1]
        mySoil->horizon[i].organicMatter = organicMatter;

        //bulk density and porosity
        getValue(query.value("bulk_density"), &bulkDensity);
        getValue(query.value("theta_sat"), &porosity);

        if (bulkDensity == NODATA)
            bulkDensity = soil::estimateBulkDensity(&(mySoil->horizon[i]), porosity);
        if (porosity == NODATA)
            porosity = soil::estimateTotalPorosity(&(mySoil->horizon[i]), bulkDensity);

        mySoil->horizon[i].bulkDensity = bulkDensity;

        //saturated conductivity
        getValue(query.value("ksat"), &ksat);
        if (ksat != NODATA)
        {
            if ((ksat < (mySoil->horizon[i].waterConductivity.kSat / 10.)) || (ksat > (mySoil->horizon[i].waterConductivity.kSat * 10.)))
                logInfo("WARNING: Ksat out of class limit, in soil " + QString::number(idSoil) + " horizon " + QString::number(i));
        }
        else
        {
            ksat = soil::estimateSaturatedConductivity(&(mySoil->horizon[i]), bulkDensity);
        }
        mySoil->horizon[i].waterConductivity.kSat = ksat;

        //update with skeleton
        mySoil->horizon[i].vanGenuchten.thetaS = porosity * (1.0 - mySoil->horizon[i].coarseFragments);
        mySoil->horizon[i].vanGenuchten.thetaR = mySoil->horizon[i].vanGenuchten.thetaR * (1.0 - mySoil->horizon[i].coarseFragments);

        mySoil->horizon[i].CEC = 50.0;
        mySoil->horizon[i].PH = 7.7;

        mySoil->horizon[i].fieldCapacity = soil::getFieldCapacity(&(mySoil->horizon[i]), soil::KPA);
        mySoil->horizon[i].wiltingPoint = soil::getWiltingPoint(soil::KPA);
        mySoil->horizon[i].waterContentFC = soil::getThetaFC(&(mySoil->horizon[i]));
        mySoil->horizon[i].waterContentWP = soil::getThetaWP(&(mySoil->horizon[i]));

        i++;
    }
    mySoil->totalDepth = mySoil->horizon[nrHorizons-1].lowerDepth;
    *outputSoil = *mySoil;
    return(true);
}

bool Vine3DProject::loadSoils()
{
    logInfo ("Read soils...");

    QString queryString = "SELECT id_soil, soil_code FROM soils ORDER BY id_soil";

    QSqlQuery query = dbConnection.exec(queryString);
    if (query.size() == -1)
    {
        this->projectError = "Function 'loadSoils' - Table 'soils'\n" + query.lastError().text();
        return(false);
    }

    free(soilList);
    nrSoils = query.size();
    soilList = new soil::Crit3DSoil[nrSoils];

    int idSoil, index = 0;
    QString soilCode;
    float maxSoilDepth = 0;
    while (query.next())
    {
        idSoil = query.value("id_soil").toInt();
        soilCode = query.value("soil_code").toString();

        if (! loadHorizons(&(soilList[index]), idSoil, soilCode))
        {
             this->projectError = "Function 'loadHorizon' " + this->projectError;
             return(false);
        }
        maxSoilDepth = maxValue(maxSoilDepth, soilList[index].totalDepth);
        index++;
    }
    this->soilDepth = minValue(this->soilDepth, maxSoilDepth);

    return(true);
}

int Vine3DProject::getAggregatedVarCode(int rawVarCode)
{
    for (int i=0; i<nrAggrVar; i++)
        if (this->varCodes[i] == rawVarCode)
            return this->aggrVarCodes[i];

    return NODATA;
}

bool Vine3DProject::getMeteoVarIndexRaw(meteoVariable myVar, int* nrIndices, int** varIndices)
{
    int myAggrVarIndex = getMeteoVarIndex(myVar);

    if (myAggrVarIndex == NODATA) return false;

    *nrIndices = 0;
    int i;
    for (i=0; i<nrAggrVar; i++)
        if (myAggrVarIndex == this->aggrVarCodes[i])
            (*nrIndices)++;

    if (*nrIndices == 0) return false;

    *varIndices = (int *) calloc(*nrIndices, sizeof(int));

    int j=0;
    for (i=0; i<nrAggrVar; i++)
        if (myAggrVarIndex == this->aggrVarCodes[i])
        {
            (*varIndices)[j] = varCodes[i];
            j++;
        }

    return true;
}

void Vine3DProject::initializeMeteoPoints()
{
    if (nrMeteoPoints > 0)
    {
        if (meteoPoints != nullptr)
        {
            for (int i = 0; i < nrMeteoPoints; i++)
                meteoPoints[i].cleanObsDataH();
            delete [] meteoPoints;
        }
        nrMeteoPoints = 0;
    }
}

void Vine3DProject::initializeRadiationMaps()
{
    meteoMaps->radiationMaps->beamRadiationMap->emptyGrid();
    meteoMaps->radiationMaps->diffuseRadiationMap->emptyGrid();
    meteoMaps->radiationMaps->globalRadiationMap->emptyGrid();
    meteoMaps->radiationMaps->sunElevationMap->emptyGrid();
    meteoMaps->radiationMaps->transmissivityMap->emptyGrid();
}

void Vine3DProject::initializeMeteoMaps()
{
    meteoMaps->airTemperatureMap->emptyGrid();
    meteoMaps->precipitationMap->emptyGrid();
    meteoMaps->airRelHumidityMap->emptyGrid();
    meteoMaps->airDewTemperatureMap->emptyGrid();
    meteoMaps->leafWetnessMap->emptyGrid();
    meteoMaps->ET0Map->emptyGrid();
    meteoMaps->windIntensityMap->emptyGrid();
    meteoMaps->evaporationMap->emptyGrid();
    meteoMaps->irrigationMap->emptyGrid();
    initializeRadiationMaps();
}

bool Vine3DProject::loadDBPoints()
{
    logInfo ("Read points locations...");

    QString queryString = "SELECT id_point, name, utm_x, utm_y, altitude, is_utc, is_forecast FROM points_properties";
    queryString += " ORDER BY id_point";

    QSqlQuery query = dbConnection.exec(queryString);
    if (query.size() == -1)
    {
        this->projectError = "Query failed in Table 'points_properties'\n" + query.lastError().text();
        return(false);
    }

    initializeMeteoPoints();
    nrMeteoPoints = query.size();
    meteoPoints = new Crit3DMeteoPoint[nrMeteoPoints];

    //read values
    int i = 0;
    int id;
    while (query.next())
    {
        id = query.value("id_point").toInt();
        meteoPoints[i].id = std::to_string(id);
        meteoPoints[i].name = query.value("name").toString().toStdString();
        meteoPoints[i].point.utm.x = query.value("utm_x").toFloat();
        meteoPoints[i].point.utm.y = query.value("utm_y").toFloat();
        meteoPoints[i].point.z = query.value("altitude").toFloat();
        meteoPoints[i].isUTC = query.value("is_utc").toBool();
        meteoPoints[i].isForecast = query.value("is_forecast").toBool();

        //temporary
        meteoPoints[i].lapseRateCode = primary;

        gis::getLatLonFromUtm(gisSettings, meteoPoints[i].point.utm.x, meteoPoints[i].point.utm.y,
                                    &(meteoPoints[i].latitude), &(meteoPoints[i].longitude));

        i++;
    }

    findVine3DLastMeteoDate();

    // load proxy values for detrending
    if (! readProxyValues())
    {
        logError("Error reading proxy values");
        return false;
    }

    //position with respect to DEM
    if (DTM.isLoaded)
        checkMeteoPointsDEM();

    return(true);
}

void Vine3DProject::findVine3DLastMeteoDate()
{
    QSqlQuery qry(dbConnection);
    QStringList tables;
    QDateTime lastDate(QDate(1800, 1, 1), QTime(0, 0, 0));

    tables << "obs_values_boundary";
    tables << "obs_values_h";

    QDateTime date;
    QString dateStr, statement;
    foreach (QString table, tables)
    {
        statement = QString( "SELECT MAX(date_) FROM \"%1\" AS dateTime").arg(table);
        if(qry.exec(statement))
        {
            if (qry.next())
            {
                dateStr = qry.value(0).toString();
                if (!dateStr.isEmpty())
                {
                    date = QDateTime::fromString(dateStr,"yyyy-MM-dd");
                    if (date > lastDate) lastDate = date;
                }
            }
        }
    }

    setCurrentDate(lastDate.date());
    setCurrentHour(12);
}

float Vine3DProject::meteoDataConsistency(meteoVariable myVar, const Crit3DTime& myTimeIni, const Crit3DTime& myTimeFin)
{
    float dataConsistency = 0.0;
    for (int i = 0; i < nrMeteoPoints; i++)
        dataConsistency = maxValue(dataConsistency, meteoPoints[i].obsDataConsistencyH(myVar, myTimeIni, myTimeFin));

    return dataConsistency;
}

bool Vine3DProject::meteoDataLoaded(const Crit3DTime& myTimeIni, const Crit3DTime& myTimeFin)
{
    for (int i = 0; i < nrMeteoPoints; i++)
        if (meteoPoints[i].isDateIntervalLoadedH(myTimeIni, myTimeFin))
            return true;

    return false;
}

//observed data: 5 minutes
bool Vine3DProject::loadObsDataSubHourly(int indexPoint, meteoVariable myVar, QDateTime d1, QDateTime d2, QString tableName)
{
    QDateTime myDateTime;
    QString queryString;
    int i, j, myHour;
    float myValue;

    if (nrMeteoPoints <= indexPoint)
    {
        logError("Function loadObsData: wrong point index");
        return(false);
    }
    Crit3DMeteoPoint* myPoint = &(meteoPoints[indexPoint]);

    int nrDays = d1.daysTo(d2) + 1;

    //initialize data
    myPoint->initializeObsDataH(meteoSettings->getHourlyIntervals(), nrDays, getCrit3DDate(d1.date()));

    queryString = "SELECT date_time, id_variable, obs_value FROM " + tableName;
    queryString += " WHERE id_location = " + QString::fromStdString(myPoint->id);
    queryString += " AND id_variable = " + QString::number(getMeteoVarIndex(myVar));
    queryString += " AND date_time >= '" + d1.toString("yyyy-MM-dd hh:mm:ss") + "'";
    queryString += " AND date_time <= '" + d2.toString("yyyy-MM-dd hh:mm:ss") + "'";
    queryString += " ORDER BY date_time";

    QSqlQuery myQuery = dbConnection.exec(queryString);
    if (myQuery.size() == -1)
    {
        logError("Query failed in Table 'obs_values': " + myQuery.lastError().text());
        return(false);
    }
    else if (myQuery.size() == 0) return(false);

    //read values
    while (myQuery.next())
    {
        if (getValue(myQuery.value(2), &myValue))
        {
            //check value
            myDateTime = myQuery.value(0).toDateTime();
            myHour = myDateTime.time().hour();
            //if myHour != previuousHour  -> aggregazione

            i = d1.daysTo(myDateTime);
            j = myHour * meteoSettings->getHourlyIntervals();

        //check var
            if (myVar == airTemperature)
                myPoint->obsDataH[i].tAir[j] = myValue;
            else if (myVar == precipitation)
                myPoint->obsDataH[i].prec[j] = myValue;
            else if (myVar == airRelHumidity)
                myPoint->obsDataH[i].rhAir[j] = myValue;
            else if (myVar == globalIrradiance)
                myPoint->obsDataH[i].irradiance[j] = myValue;
            else if (myVar == windIntensity)
                myPoint->obsDataH[i].windInt[j] = myValue;
        }
    }

    myQuery.clear();
    return(true);
}


// observed data: aggregation hourly
bool Vine3DProject::loadObsDataHourly(int indexPoint, QDate d1, QDate d2, QString tableName, bool useAggrCodes)
{
    QDate myDate;
    QString queryString;
    int i, j, myHour;
    meteoVariable myVar;
    float myValue, myFlag;
    bool isValid;
    bool dataAvailable = false;

    if (nrMeteoPoints <= indexPoint)
    {
        logError("Function loadObsDataHourly: wrong point index");
        return(false);
    }
    Crit3DMeteoPoint* myPoint = &(meteoPoints[indexPoint]);

    int hourlyFraction = 1;

    if (useAggrCodes)
        queryString = "SELECT date_, hour_, id_variable, obs_value FROM " + tableName;
    else
        queryString = "SELECT date_, hour_, id_variable, obs_value, data_valid FROM " + tableName;

    queryString += " WHERE id_location = " + QString::fromStdString(myPoint->id);
    queryString += " AND date_ >= '" + d1.toString("yyyy-MM-dd") + "'";
    queryString += " AND date_ <= '" + d2.toString("yyyy-MM-dd") + "'";
    queryString += " ORDER BY date_, hour_";

    QSqlQuery myQuery = dbConnection.exec(queryString);

    if (myQuery.size() == -1)
    {
        logError("Query failed in Table " + tableName + "\n" + myQuery.lastError().text());
        return(false);
    }
    else if (myQuery.size() == 0) return(false);

    //read values
    while (myQuery.next())
    {
        myDate = myQuery.value(0).toDate();
        myHour = myQuery.value(1).toInt();
        //transform local time in UTC
        if (!myPoint->isUTC)
        {
            myHour -= this->gisSettings.timeZone;
            if (myHour < 0)
            {
                myDate = myDate.addDays(-1);
                myHour += 24;
            }
        }
        i = d1.daysTo(myDate);
        if (i >= 0)
        {
            j = myHour * hourlyFraction;

            if (useAggrCodes)
                isValid = true;
            else
            {
                isValid = false;
                if (getValue(myQuery.value(4), &myFlag))
                    if (myFlag >= 0.5) isValid = true;
            }

            if (isValid)
            {
                if (useAggrCodes)
                    myVar  = getMeteoVariable(myQuery.value(2).toInt());
                else
                    myVar = getMeteoVariable(this->getAggregatedVarCode(myQuery.value(2).toInt()));

                if (getValue(myQuery.value(3), &myValue))
                {
                    dataAvailable = true;

                    if (myVar == airTemperature)
                    {
                        if ((myValue > -40)&&(myValue <= 60))
                            myPoint->obsDataH[i].tAir[j] = myValue;
                    }
                    else if (myVar == precipitation)
                    {
                        if ((myValue >= 0)&&(myValue <= 500))
                            myPoint->obsDataH[i].prec[j] = myValue;
                    }
                    else if (myVar == airRelHumidity)
                    {
                        if ((myValue > 0)&&(myValue <= 100))
                            myPoint->obsDataH[i].rhAir[j] = myValue;
                    }
                    else if (myVar == globalIrradiance)
                    {
                        if ((myValue >= 0)&&(myValue < 1360))
                        myPoint->obsDataH[i].irradiance[j] = myValue;
                    }
                    else if (myVar == windIntensity)
                    {
                        if ((myValue >= 0)&&(myValue < 75))
                        myPoint->obsDataH[i].windInt[j] = myValue;
                    }
                    else if (myVar == leafWetness)
                    {
                        if ((myValue >= 0)&&(myValue <= 60))
                        myPoint->obsDataH[i].leafW[j] = myValue;
                    }
                }
            }
        }
    }

    myQuery.clear();
    return(dataAvailable);
}


bool Vine3DProject::loadObsDataHourlyVar(int indexPoint, meteoVariable myVar, QDate d1, QDate d2, QString tableName, bool useAggrCodes)
{
    QDate myDate;
    QString queryString;
    int i, j, myHour;
    float myValue, myFlag;
    bool isValid;
    int nrIndices;
    int* varIndices;
    bool dataAvailable=false;

    if (nrMeteoPoints <= indexPoint)
    {
        logError("Function loadObsDataBoundary: wrong point index");
        return(false);
    }
    Crit3DMeteoPoint* myPoint = &(meteoPoints[indexPoint]);

    if (useAggrCodes)
    {
        queryString = "SELECT date_, hour_, obs_value FROM " + tableName;
        queryString += " WHERE id_variable = " + QString::number(getMeteoVarIndex(myVar));
    }
    else
    {
        queryString = "SELECT date_, hour_, obs_value, data_valid FROM " + tableName;
        if (!this->getMeteoVarIndexRaw(myVar, &nrIndices, &varIndices)) return false;
        queryString += " WHERE id_variable IN (";
        for (int i=0; i<nrIndices-1; i++)
            queryString += QString::number(varIndices[i]) + ",";
        queryString += QString::number(varIndices[nrIndices-1]) + ")";
    }

    queryString += " AND id_location = " + QString::fromStdString(myPoint->id);
    queryString += " AND date_ >= '" + d1.toString("yyyy-MM-dd") + "'";
    queryString += " AND date_ <= '" + d2.toString("yyyy-MM-dd") + "'";
    queryString += " ORDER BY date_, hour_";

    QSqlQuery myQuery = dbConnection.exec(queryString);
    if (myQuery.size() == -1)
    {
        logError("Query failed in table: "+ tableName + "\n" + myQuery.lastError().text());
        return(false);
    }
    else if (myQuery.size() == 0) return(false);

    QDate pointFirstDate = getQDate(myPoint->obsDataH[0].date);

    //read values
    while (myQuery.next())
    {
        myDate = myQuery.value(0).toDate();
        myHour = myQuery.value(1).toInt();
        //transform local time in UTC
        if (!myPoint->isUTC)
        {
            myHour -= this->gisSettings.timeZone;
            if (myHour < 0)
            {
                myDate = myDate.addDays(-1);
                myHour += 24;
            }
        }
        i = pointFirstDate.daysTo(myDate);
        if (i>=0)
        {
            j = myHour * meteoSettings->getHourlyIntervals();

            if (useAggrCodes)
                isValid = true;
            else
            {
                isValid = false;
                if (getValue(myQuery.value(3), &myFlag))
                    if (myFlag >= 0.5) isValid = true;
            }

            if (isValid)
                if (getValue(myQuery.value(2), &myValue))
                {
                    dataAvailable = true;
                    if (myVar == airTemperature)
                        myPoint->obsDataH[i].tAir[j] = myValue;
                    else if (myVar == precipitation)
                        myPoint->obsDataH[i].prec[j] = myValue;
                    else if (myVar == airRelHumidity)
                        myPoint->obsDataH[i].rhAir[j] = myValue;
                    else if (myVar == globalIrradiance)
                        myPoint->obsDataH[i].irradiance[j] = myValue;
                    else if (myVar == windIntensity)
                        myPoint->obsDataH[i].windInt[j] = myValue;
                    else if (myVar == leafWetness)
                        myPoint->obsDataH[i].leafW[j] = myValue;
                }
        }
    }

    myQuery.clear();
    return(dataAvailable);
}


bool Vine3DProject::loadObsDataAllPoints(QDate d1, QDate d2, bool showInfo)
{
    isObsDataLoaded = false;

    logInfo("Load observed data:" + d1.toString() + " " + d2.toString());

    bool isObsDataBoundaryLoaded = false;
    bool isObsDataWMSLoaded = false;
    bool isForecast = false;
    int nrDays = d1.daysTo(d2) + 1;
    int hourlyFraction = 1;

    int step = 1;
    FormInfo myInfo;
    QString infoStr;

    if (showInfo)
    {
        infoStr = "Loading data from " + d1.toString() + " to " + d2.toString();
        step = myInfo.start(infoStr, nrMeteoPoints);
    }

    for (int i = 0; i < nrMeteoPoints; i++)
    {
        if (showInfo)
            if ((i % step) == 0) myInfo.setValue(i);

        meteoPoints[i].initializeObsDataH(hourlyFraction, nrDays, getCrit3DDate(d1));

        if (meteoPoints[i].isForecast)
        {
            if (loadObsDataHourly(i, d1, d2, "forecast", true))
                isForecast = true;
        }
        else
        {
            if (loadObsDataHourly(i, d1, d2, "obs_values_boundary", true))
                isObsDataBoundaryLoaded = true;

            if (loadObsDataHourly(i, d1, d2, "obs_values_h", false))
                isObsDataWMSLoaded = true;
        }

    }

    if (showInfo) myInfo.close();

    isObsDataLoaded = (isObsDataBoundaryLoaded || isObsDataWMSLoaded || isForecast);

    if (! isObsDataLoaded) this->projectError = "Missing observed data.";

    return(isObsDataLoaded);
}


bool Vine3DProject::loadObsDataAllPointsVar(meteoVariable myVar, QDate d1, QDate d2)
{
    isObsDataLoaded = false;

    bool isObsDataBoundaryLoaded = false;
    bool isObsDataWMSLoaded = false;
    bool isForecastLoaded=false;
    int nrDays = d1.daysTo(d2) + 1;
    int hourlyFraction = 1;
    Crit3DDate dateIni = getCrit3DDate(d1);
    Crit3DDate dateFin = getCrit3DDate(d2);

    for (int i = 0; i < nrMeteoPoints; i++)
    {
        if (! meteoPoints[i].isDateIntervalLoadedH(dateIni,dateFin))
            meteoPoints[i].initializeObsDataH(hourlyFraction, nrDays, dateIni);
        else
            meteoPoints[i].emptyVarObsDataH(myVar, dateIni, dateFin);

        if (meteoPoints[i].isForecast)
        {
            if (loadObsDataHourlyVar(i, myVar, d1, d2, "forecast", true))
                isForecastLoaded = true;
        }
        else
        {
            if (loadObsDataHourlyVar(i, myVar, d1, d2, "obs_values_boundary", true))
                isObsDataBoundaryLoaded = true;

            if (loadObsDataHourlyVar(i, myVar, d1, d2, "obs_values_h", false))
                isObsDataWMSLoaded = true;
        }

    }

    isObsDataLoaded = (isObsDataBoundaryLoaded || isObsDataWMSLoaded || isForecastLoaded);

    if (! isObsDataLoaded) this->projectError = "missing data";
    return(isObsDataLoaded);
}


int Vine3DProject::getIndexPointFromId(QString myId)
{
    for (int i = 0; i < nrMeteoPoints; i++)
        if (QString::fromStdString(meteoPoints[i].id) == myId)
            return(i);
    return(NODATA);
}


float Vine3DProject::getTimeStep()
{ return (3600. / meteoSettings->getHourlyIntervals());}


bool removeDirectory(QString myPath)
{
    QDir myDirectory(myPath);
    myDirectory.setNameFilters(QStringList() << "*.*");
    myDirectory.setFilter(QDir::Files);
    //remove all files
    foreach(QString myFile, myDirectory.entryList())
    {
        myDirectory.remove(myFile);
    }
    return myDirectory.rmdir(myPath);
}


bool Vine3DProject::LoadObsDataFilled(QDateTime firstTime, QDateTime lastTime)
{
    QDate d1 = firstTime.date().addDays(-30);
    QDate d2 = lastTime.date().addDays(30);
    //if (d2 > today) d2 = today;

    if (! this->loadObsDataAllPoints(d1, d2, false)) return(false);

    // Replace missing data
    long nrReplacedData = 0;
    Crit3DTime myTime = getCrit3DTime(firstTime);
    long nrHours = firstTime.secsTo(lastTime) / 3600;
    for (int i = 0; i <=nrHours; i++)
    {
        if (!checkLackOfData(this, airTemperature, myTime, &nrReplacedData)
            || !checkLackOfData(this, precipitation, myTime, &nrReplacedData)
            || !checkLackOfData(this, airRelHumidity, myTime, &nrReplacedData))
        {
            this->logError("Weather data missing: " + getQDateTime(myTime).toString("yyyyMMdd hh:mm"));
            return(false);
        }
        checkLackOfData(this, windIntensity, myTime, &nrReplacedData);
        myTime = myTime.addSeconds(3600);
    }

    if(nrReplacedData > 0)
    {
        this->logInfo("\nWarning! "+ QString::number(nrReplacedData)+ " hourly data are missing.");
        this->logInfo("They was replaced by mean values.\n");
    }

    return true;
}


bool Vine3DProject::runModels(QDateTime dateTime1, QDateTime dateTime2, bool isSaveOutput, const QString& myArea)
{
    if (! this->isProjectLoaded) return (false);

    if (!LoadObsDataFilled(dateTime1, dateTime2))
    {
        this->logError();
        return false;
    }

    QDir myDir;
    QString myOutputPathDaily, myOutputPathHourly;
    bool isInitialState;
    QDate firstDate = dateTime1.date();
    QDate lastDate = dateTime2.date();
    QDate previousDate = firstDate.addDays(-1);
    int hourTime2 = dateTime2.time().hour();
    int finalHour;

    this->logInfo("Run models from: " + firstDate.toString() + " to: " + lastDate.toString());

    for (QDate myDate = firstDate; myDate <= lastDate; myDate = myDate.addDays(1))
    {
        //load state
        isInitialState = false;
        if (myDate == firstDate)
        {
            if (loadStates(previousDate, myArea))
                isInitialState = false;
            else
            {
                this->logInfo("State not found.");
                isInitialState = true;
            }
        }

        if (myDate == lastDate)
            finalHour = hourTime2;
        else
            finalHour = 24;

        if (finalHour > 0)
        {
            if (isSaveOutput)
            {
                //create output directories
                myOutputPathDaily = path + dailyOutputPath + myDate.toString("yyyy/MM/dd/");
                myOutputPathHourly = path + "hourly_output/" + myDate.toString("yyyy/MM/dd/");

                if ((! myDir.mkpath(myOutputPathDaily)) || (! myDir.mkpath(myOutputPathHourly)))
                {
                    this->logError("Creation output directories failed." );
                    isSaveOutput = false;
                }
            }

            // load average air temperature map, if exists
            loadDailyMeteoMap(this, dailyAirTemperatureAvg, myDate.addDays(-1), myArea);

            if (! modelDailyCycle(isInitialState, getCrit3DDate(myDate), finalHour, this, myOutputPathHourly, isSaveOutput, myArea))
            {
                logError("Model cycle error.");
                return false;
            }
        }

        if ((finalHour == 24) || ((myDate == lastDate) && (finalHour == 23)))
        {
            if (isSaveOutput)
            {
                this->logInfo("Aggregate daily meteo data");
                aggregateAndSaveDailyMap(this, airTemperature, aggregationMin, getCrit3DDate(myDate), myOutputPathDaily, myOutputPathHourly, myArea);
                aggregateAndSaveDailyMap(this, airTemperature, aggregationMax, getCrit3DDate(myDate), myOutputPathDaily, myOutputPathHourly, myArea);
                aggregateAndSaveDailyMap(this, airTemperature, aggregationMean, getCrit3DDate(myDate), myOutputPathDaily,myOutputPathHourly, myArea);
                aggregateAndSaveDailyMap(this, precipitation, aggregationSum, getCrit3DDate(myDate), myOutputPathDaily, myOutputPathHourly, myArea);
                aggregateAndSaveDailyMap(this, airRelHumidity, aggregationMin, getCrit3DDate(myDate), myOutputPathDaily, myOutputPathHourly, myArea);
                aggregateAndSaveDailyMap(this, airRelHumidity, aggregationMax, getCrit3DDate(myDate), myOutputPathDaily, myOutputPathHourly, myArea);
                aggregateAndSaveDailyMap(this, airRelHumidity, aggregationMean, getCrit3DDate(myDate), myOutputPathDaily, myOutputPathHourly, myArea);
                aggregateAndSaveDailyMap(this, windIntensity, aggregationMean, getCrit3DDate(myDate), myOutputPathDaily, myOutputPathHourly, myArea);
                aggregateAndSaveDailyMap(this, globalIrradiance, aggregationIntegration, getCrit3DDate(myDate), myOutputPathDaily, myOutputPathHourly, myArea);
                aggregateAndSaveDailyMap(this, leafWetness, aggregationSum, getCrit3DDate(myDate), myOutputPathDaily, myOutputPathHourly, myArea);
                aggregateAndSaveDailyMap(this, referenceEvapotranspiration, aggregationSum, getCrit3DDate(myDate), myOutputPathDaily, myOutputPathHourly, myArea);
                aggregateAndSaveDailyMap(this, actualEvaporation, aggregationSum, getCrit3DDate(myDate), myOutputPathDaily, myOutputPathHourly, myArea);
                if (removeDirectory(myOutputPathHourly)) this->logInfo("Delete hourly files");
            }

            //load daily map (for desease)
            if (! loadDailyMeteoMap(this, dailyAirTemperatureAvg, myDate, myArea)) return false;
            if (! loadDailyMeteoMap(this, dailyAirRelHumidityAvg, myDate, myArea)) return false;
            if (! loadDailyMeteoMap(this, precipitation, myDate, myArea))  return false;
            if (! loadDailyMeteoMap(this, leafWetness, myDate, myArea)) return false;
            updateThermalSum(this, myDate);

            //powdery mildew
            computePowderyMildew(this);

            //state and output
            saveStateAndOutput(myDate, myArea);
        }
    }

    // Downy mildew (computation from 1 January)
    computeDownyMildew(this, firstDate, lastDate, hourTime2, myArea);

    logInfo("end of run");
    return true;
}


bool Vine3DProject::loadStates(QDate myDate, QString myArea)
{
    QString statePath = path + "states/" + myDate.toString("yyyy/MM/dd/");

    //if (!loadPlantState(this, tartaricAcidVar, myDate, myStatePath)) return(false);
    //if (!loadPlantState(this, pHBerryVar, myDate, myStatePath)) return(false);
    //if (!loadPlantState(this, fruitBiomassIndexVar, myDate, myStatePath)) return(false);
    if (!loadPlantState(this, daysAfterBloomVar, myDate, statePath, myArea)) return(false);
    if (!loadPlantState(this, cumulatedBiomassVar, myDate, statePath, myArea)) return(false);
    if (!loadPlantState(this, fruitBiomassVar, myDate, statePath, myArea)) return(false);
    if (!loadPlantState(this, shootLeafNumberVar, myDate, statePath, myArea)) return(false);
    if (!loadPlantState(this, meanTemperatureLastMonthVar, myDate, statePath, myArea)) return(false);
    if (!loadPlantState(this, chillingUnitsVar, myDate, statePath, myArea)) return(false);
    if (!loadPlantState(this, forceStateBudBurstVar, myDate, statePath, myArea)) return(false);
    if (!loadPlantState(this, forceStateVegetativeSeasonVar, myDate, statePath, myArea)) return(false);
    if (!loadPlantState(this, stageVar, myDate, statePath, myArea)) return(false);
    if (!loadPlantState(this, leafAreaIndexVar, myDate, statePath, myArea)) return(false);

    if (!loadPlantState(this, isHarvestedVar, myDate, statePath, myArea))
    {
        this->statePlantMaps->isHarvestedMap->setConstantValueWithBase(0, DTM);
    }
    if (!loadPlantState(this, fruitBiomassIndexVar, myDate, statePath, myArea))
    {
        //defualt= chardonnay
        this->statePlantMaps->fruitBiomassIndexMap->setConstantValueWithBase(this->modelCases[1].cultivar->parameterBindiMiglietta.fruitBiomassSlope, DTM);
    }

    //problema: mancano nei precedenti stati
    loadPlantState(this, cumRadFruitsetVerVar, myDate, statePath, myArea);
    loadPlantState(this, degreeDaysFromFirstMarchVar, myDate, statePath, myArea);
    loadPlantState(this, degreeDays10FromBudBurstVar, myDate, statePath, myArea);
    loadPlantState(this, degreeDaysAtFruitSetVar, myDate, statePath, myArea);
    loadPlantState(this, powderyAICVar, myDate, statePath, myArea);
    loadPlantState(this, powderyCurrentColoniesVar, myDate, statePath, myArea);
    loadPlantState(this, powderySporulatingColoniesVar, myDate, statePath, myArea);

    if (!loadWaterBalanceState(this, myDate, myArea, statePath, waterMatricPotential)) return false;

    this->logInfo("Load state: " + myDate.toString("yyyy-MM-dd"));
    return(true);
}

bool Vine3DProject::saveStateAndOutput(QDate myDate, QString myArea)
{
    QDir myDir;
    QString statePath = path + "states/" + myDate.toString("yyyy/MM/dd/");
    QString outputPath = path + this->dailyOutputPath + myDate.toString("yyyy/MM/dd/");
    if (! myDir.mkpath(statePath))
    {
        this->logError("Creation directory states failed." );
        return(false);
    }

    this->logInfo("Save state and output");

    if (!savePlantState(this, daysAfterBloomVar, myDate, statePath, myArea)) return(false);
    if (!savePlantState(this, cumulatedBiomassVar, myDate, statePath, myArea)) return(false);
    if (!savePlantState(this, fruitBiomassVar, myDate, statePath, myArea)) return(false);
    if (!savePlantState(this, isHarvestedVar, myDate, statePath, myArea)) return(false);
    if (!savePlantState(this, shootLeafNumberVar, myDate, statePath, myArea)) return(false);
    if (!savePlantState(this, meanTemperatureLastMonthVar, myDate, statePath, myArea)) return(false);
    if (!savePlantState(this, chillingUnitsVar, myDate, statePath, myArea)) return(false);
    if (!savePlantState(this, forceStateBudBurstVar, myDate, statePath, myArea)) return(false);
    if (!savePlantState(this, forceStateVegetativeSeasonVar, myDate, statePath, myArea)) return(false);
    if (!savePlantState(this, stageVar, myDate, statePath, myArea)) return(false);
    if (!savePlantState(this, cumRadFruitsetVerVar, myDate, statePath, myArea)) return(false);
    if (!savePlantState(this, leafAreaIndexVar, myDate, statePath, myArea)) return(false);
    if (!savePlantState(this, powderyAICVar, myDate, statePath, myArea)) return(false);
    if (!savePlantState(this, degreeDaysFromFirstMarchVar, myDate, statePath, myArea)) return(false);
    if (!savePlantState(this, degreeDays10FromBudBurstVar, myDate, statePath, myArea)) return(false);
    if (!savePlantState(this, degreeDaysAtFruitSetVar, myDate, statePath, myArea)) return(false);
    if (!savePlantState(this, powderyCurrentColoniesVar, myDate, statePath, myArea)) return(false);
    if (!savePlantState(this, powderySporulatingColoniesVar, myDate, statePath, myArea)) return(false);
    if (!savePlantState(this,fruitBiomassIndexVar,myDate,statePath, myArea)) return(false);

    if (!saveWaterBalanceState(this, myDate, myArea, statePath, waterMatricPotential)) return (false);

    QString notes = "";
    if (!savePlantOutput(this, wineYieldVar, myDate, outputPath, myArea, notes, false, true)) return(false);
    if (!savePlantOutput(this, stageVar, myDate, outputPath, myArea, notes, true, true)) return(false);
    if (!savePlantOutput(this, tartaricAcidVar, myDate, outputPath, myArea, notes, false, true)) return(false);
    if (!savePlantOutput(this, daysFromFloweringVar, myDate, outputPath, myArea, notes, false, true)) return(false);
    if (!savePlantOutput(this, brixMaximumVar, myDate, outputPath, myArea, notes, false, true)) return(false);
    if (!savePlantOutput(this, brixBerryVar, myDate, outputPath, myArea, notes, false, true)) return(false);
    if (!savePlantOutput(this, deltaBrixVar, myDate, outputPath, myArea, notes, false, true)) return(false);
    if (!savePlantOutput(this, cumulatedBiomassVar, myDate, outputPath, myArea, notes, true, true)) return(false);
    if (!savePlantOutput(this, fruitBiomassVar, myDate, outputPath, myArea, notes, true, true)) return(false);
    if (!savePlantOutput(this, shootLeafNumberVar, myDate, outputPath, myArea, notes, true, true)) return(false);
    if (!savePlantOutput(this, leafAreaIndexVar, myDate, outputPath, myArea, notes, true, true)) return(false);
    if (!savePlantOutput(this, transpirationStressVar, myDate, outputPath, myArea, notes, false, true)) return(false);
    if (!savePlantOutput(this, transpirationVineyardVar, myDate, outputPath, myArea, notes, false, true)) return(false);
    if (!savePlantOutput(this, transpirationGrassVar, myDate, outputPath, myArea, notes, false, false)) return(false);
    if (!savePlantOutput(this, powderyAICVar, myDate, outputPath, myArea, notes, true, true)) return(false);
    if (!savePlantOutput(this, powderySporulatingColoniesVar, myDate, outputPath, myArea, notes, true, true)) return(false);

    if (!savePlantOutput(this, powderyCOLVar, myDate, outputPath, myArea, notes, false, true)) return(false);
    if (!savePlantOutput(this, powderyINFRVar, myDate, outputPath, myArea, notes, false, true)) return(false);
    if (!savePlantOutput(this, powderyPrimaryInfectionRiskVar, myDate, outputPath, myArea, notes, false, true)) return(false);

    if (!saveWaterBalanceOutput(this, myDate, waterMatricPotential, "matricPotential10", "10cm", outputPath, myArea, 0.1, 0.1)) return false;
    if (!saveWaterBalanceOutput(this, myDate, waterMatricPotential, "matricPotential30", "30cm", outputPath, myArea, 0.3, 0.3)) return false;
    if (this->soilDepth >= 0.7f)
    {
        if (!saveWaterBalanceOutput(this, myDate, waterMatricPotential, "matricPotential70", "70cm", outputPath, myArea, 0.7, 0.7)) return false;
    }

    if (!saveWaterBalanceOutput(this, myDate, degreeOfSaturation, "degreeOfSaturation", "soilDepth", outputPath, myArea, 0.0, double(this->soilDepth) - 0.01)) return false;
    if (!saveWaterBalanceOutput(this, myDate, soilSurfaceMoisture, "SSM", "5cm", outputPath, myArea, 0.0, 0.05)) return false;
    if (!saveWaterBalanceOutput(this, myDate, availableWaterContent, "waterContent", "rootZone", outputPath, myArea, 0.0, double(this->soilDepth))) return false;

    return(true);
}

int Vine3DProject::getModelCase(long row, long col)
{
    int caseIndex = this->modelCaseMap.value[row][col];
    if (caseIndex == this->modelCaseMap.header->flag)
        //DEFAULT
        caseIndex = 0;
    return caseIndex;
}


bool Vine3DProject::isVineyard(long row, long col)
{
    int caseIndex = getModelCase(row, col);
    return (modelCases[caseIndex].landuse == landuse_vineyard);
}

int Vine3DProject::getSoilIndex(long row, long col)
{
    int fieldIndex = this->getModelCase(row, col);
    return this->modelCases[fieldIndex].soilIndex;
}

bool Vine3DProject::getFieldBookIndex(int firstIndex, QDate myDate, int fieldIndex, int* outputIndex)
{
    *outputIndex = NODATA;
    for (int i = firstIndex; i < this->nrFieldOperations; i++)
    {
        // order by date
        if (this->fieldBook[i].operationDate > myDate) return false;
        if (myDate == this->fieldBook[i].operationDate)
        {
            if (fieldIndex == this->fieldBook[i].idField)
            {
                *outputIndex = i;
                return true;
            }
        }
    }
    return false;
}


//-------- LOG functions -------------------------
bool Vine3DProject::setLogFile()
{
    QString fileName, myPath, myDate;
    QDir myDir;

    myPath = path + "log/";
    myDir.mkpath(myPath);
    //TODO check and error if directory not created

    myDate = QDateTime().currentDateTime().toString("yyyyMMddhhmm");
    fileName = "log_" + idArea + "_" + myDate + ".txt";

    this->logFileName = myPath + fileName;
    this->logFile.open(this->logFileName.toStdString().c_str());
    return (logFile.is_open());
}

void Vine3DProject::logInfo(QString myLog)
{
    if (logFile.is_open())
            logFile << myLog.toStdString() << std::endl;

    if (environment == gui)
        std::cout << myLog.toStdString() << std::endl;
}

void Vine3DProject::logError()
{
    this->projectError = "Error! " + this->projectError;
    if (environment == gui)
    {
        QMessageBox msgBox;
        msgBox.setText(this->projectError);
        msgBox.exec();
    }
    if (logFile.is_open())
        logFile << this->projectError.toStdString() << std::endl;
}

void Vine3DProject::logError(QString myError)
{
    this->projectError = myError;
    logError();
}

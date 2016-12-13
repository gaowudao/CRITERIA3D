#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>
#include <QDir>
#include <QDateTime>

#include <iostream>

#include "commonConstants.h"
#include "meteo.h"
#include "interpolation.h"
#include "waterBalance.h"
#include "plant.h"
#include "dataHandler.h"
#include "modelCore.h"
#include "parserXML.h"
#include "atmosphere.h"
#include "solarRadiation.h"
#include "project.h"


void Crit3DProject::initialize()
{
    environment = gui;

    idArea = "0000";
    sqlDriver = "QPSQL";
    hostName = "127.0.0.1";
    databaseName = "";
    connectionPort = 5432;
    userName = "postgres";
    password = "postgres";

    currentVar = noMeteoVar;

    nrMeteoPoints = 0;
    isObsDataLoaded = false;
    windIntensityDefault = 2.0;

    isProjectLoaded = false;

    path = "";
    dailyOutputPath = "daily_output/";
    dtmFileName = "";
    fieldMapName = "";
    logFileName = "";
    projectError = "";

    hourlyIntervals = 1;
    lastDateTransmissivity.setDate(1900,1,1);

    nrVines = 0;
    nrSoils = 0;
    nrSoilLayers = 0;
    nrLayerNodes = 0;
    nrVineFields = 0;
    nrNodes = 0;
    soilDepth = 1.0;       //[m]
}


Crit3DProject::Crit3DProject()
{
    initialize();
}

void Crit3DProject::deleteAllGrids()
{
    this->dtm.freeGrid();
    this->fieldMap.freeGrid();
    this->boundaryGrid.freeGrid();
    this->indexGrid.freeGrid();
    this->interpolatedDtm.freeGrid();

    delete this->meteoMaps;
}

void Crit3DProject::closeProject()
{
    if (this->isProjectLoaded)
    {
        this->logInfo("Close Project");
        this->db.close();
        this->logFile.close();
        this->initializeMeteoPoints();
        this->deleteAllGrids();

        cleanWaterBalanceMemory();

        this->isProjectLoaded = false;
    }
}

void Crit3DProject::setEnvironment(Tenvironment myEnvironment)
{
    environment = myEnvironment;
}

QString Crit3DProject::getGeoserverPath()
{
    return (this->path + "geoserver/");
}


bool Crit3DProject::loadProject(QString myFileName)
{
    closeProject();
    this->initialize();

    if (myFileName == "") return(false);
    if (! readXmlProject(this, myFileName))
    {
        this->logError("Wrong Project File.\n" + this->projectError);
        return (false);
    }

    if (this->setLogFile())
        this->logInfo("Set LogFile: " + this->logFileName);
    else
        this->logError("LogFile Wrong.");

    myFileName = this->path + this->dtmFileName;
    if (loadDTM(myFileName))
    {
        this->logInfo("Initialize DTM and project maps...");
        meteoMaps = new Crit3DMeteoMaps(dtm, this->gisSettings);
        statePlantMaps = new Crit3DStatePlantMaps(dtm);
    }
    else
        return(false);

    if (! openDB()) return (false);

    if (!loadFieldShape())
    {
        myFileName = this->path + this->fieldMapName;
        if (!loadFieldMap(myFileName)) return false;
    }

    if (!loadSettings() || !loadSoils() || !loadDBPoints())
    {
        logError();
        db.close();
        return(false);
    }

    if (!loadFieldsProperties())
    {
        logError();
        db.close();
        return(false);
    }

    this->isProjectLoaded = true;

    if (! initializeWaterBalance(this))
    {
        this->logError();
        return(false);
    }

    this->currentProfile = (double *) calloc(this->nrSoilLayers, sizeof(double));

    outputPlantMaps = new Crit3DOutputPlantMaps(dtm, this->nrSoilLayers);

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

    logInfo("Project loaded");
    return(true);
}


int Crit3DProject::queryFieldPoint(double x, double y)
{
    QString UTMx = QString::number(x, 'f', 1);
    QString UTMy = QString::number(y, 'f', 1);
    QString UTMZone = QString::number(this->gisSettings.utmZone);

    QString myQueryString = "SELECT id_field FROM fields_shp";
    myQueryString += " WHERE ST_Contains(geom, ST_GeomFromText";
    myQueryString += "('POINT(" + UTMx + " " + UTMy + ")', 326" + UTMZone + ")) = true";
    QSqlQuery myQuery;

    myQuery = this->db.exec(myQueryString);

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


bool Crit3DProject::loadFieldShape()
{
    this->logInfo ("Read Fields...");
    int dim = 1;
    int i, j, id;
    double x0, y0;
    std::vector <float> valuesList;

    QString myQueryString = "SELECT id_field FROM fields_shp";

    QSqlQuery myQuery;
    myQuery = this->db.exec(myQueryString);
    if (myQuery.size() == -1)
    {
        this->projectError = myQuery.lastError().text();
        return(false);
    }
    myQuery.clear();

    this->fieldMap.initializeGrid(this->dtm);

    double step = this->fieldMap.header->cellSize / (2*dim+1);

    for (long row = 0; row < this->fieldMap.header->nrRows ; row++)
        for (long col = 0; col < this->fieldMap.header->nrCols; col++)
            if (this->dtm.value[row][col] != this->dtm.header->flag)
            {
                //center
                gis::getUtmXYFromRowCol(this->fieldMap, row, col, &x0, &y0);
                id = queryFieldPoint(x0, y0);
                if (id != NODATA)
                    this->fieldMap.value[row][col] = id;
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
                        this->fieldMap.value[row][col] = this->fieldMap.header->flag;
                    else
                        this->fieldMap.value[row][col] = gis::prevailingValue(valuesList);
                }
            }

    gis::updateMinMaxRasterGrid(&(this->fieldMap));
    this->nrVineFields = this->fieldMap.maximum;
    return true;
}


bool Crit3DProject::loadFieldMap(QString myFileName)
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
        fieldMap.initializeGrid(dtm);
        gis::prevailingMap(myGrid, &(fieldMap));

        gis::updateMinMaxRasterGrid(&(fieldMap));
        this->nrVineFields = fieldMap.maximum;

        return (true);
    }
}



bool Crit3DProject::loadFieldsProperties()
{
    /*
    logInfo ("Read fields properties...");

    // READ NUMBER OF FIELDS
    QString myQueryString = " SELECT max(id_field) FROM fields";
    QSqlQuery myQuery = db.exec(myQueryString);
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
                      "\nSome fields will be set to default parameters.\n");
    }

    this->nrVineFields = maxValue(this->nrVineFields, maxFieldIndex) +1;

    //alloc memory for vines
    //this->vineFields = (TvineField *) calloc(this->nrVineFields, sizeof(TvineField));

    // READ DEFAULT FIELD PROPERTIES
    int idField, vineIndex, soilIndex;
    float maxLaiGrass, maxIrrigationRate;
    myQueryString =
            " SELECT id_field, id_cultivar,"
            " id_soil, max_lai_grass, irrigation_max_rate"
            " FROM fields"
            " WHERE id_field=0";
    myQuery = db.exec(myQueryString);

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
    if (!readFieldQuery(myQuery, &idField, &vineIndex, &soilIndex, &maxLaiGrass, &maxIrrigationRate))
        return false;

    // INITIALIZE FIELDS
    for (int i = 0; i<nrVineFields; i++)
        setField(i, soilIndex, vineIndex, maxLaiGrass, maxIrrigationRate);

   // READ PROPERTIES
    myQueryString =
            " SELECT id_field, id_cultivar,"
            " id_soil, max_lai_grass, irrigation_max_rate"
            " FROM fields"
            " ORDER BY id_field";
    myQuery = db.exec(myQueryString);

    if (myQuery.size() == -1)
    {
        this->projectError = "Wrong structure in table 'fields'\n" + myQuery.lastError().text();
        return(false);
    }

    // SET PROPERTIES
    while (myQuery.next())
    {
       if (readFieldQuery(myQuery, &idField, &vineIndex, &soilIndex, &maxLaiGrass, &maxIrrigationRate))
            setField(idField, soilIndex, vineIndex, maxLaiGrass, maxIrrigationRate);
    }
    */

    return(true);
}



bool Crit3DProject::loadClimateParameters()
{
    logInfo ("Read climate parameters...");
    QString myQueryString = "SELECT month, tmin_lapse_rate, tmax_lapse_rate, tdmin_lapse_rate, tdmax_lapse_rate";
    myQueryString += " FROM climate";
    myQueryString += " ORDER BY month";

    QSqlQuery myQuery = db.exec(myQueryString);
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

    /*! read values */
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

bool Crit3DProject::loadSettings()
{
    if (!loadClimateParameters()) return false;
    if (!loadVanGenuchtenParameters()) return false;

    /*! load crop parameters */
    return true;
}

bool Crit3DProject::loadDTM(QString myFileName)
{
    std::string fn = myFileName.left(myFileName.length()-4).toStdString();
    std::string* myError = new std::string();

    if (! gis::readEsriGrid(fn, &(dtm), myError))
    {
        this->projectError = "Load DTM failed.\n" + myFileName + "\n" + QString::fromStdString(*myError);
        logError();
        return (false);
    }
    else
    {
        logInfo ("Read DTM " + myFileName);
        return (true);
    }
}



bool Crit3DProject::openDB()
{
    db.close();

    logInfo ("Open DB  " + hostName + "//" + databaseName);
    qDebug() << QSqlDatabase::drivers();

    db = QSqlDatabase::addDatabase(sqlDriver);
    db.setHostName(hostName);
    db.setDatabaseName(databaseName);
    db.setPort(connectionPort);
    db.setUserName(userName);
    db.setPassword(password);
    if (! db.open())
    {
        logError("Open DB failed: " + hostName + "//" + databaseName +"\n" + db.lastError().text());
        db.close();
        return(false);
    }

    return (true);
}

bool Crit3DProject::loadVanGenuchtenParameters()
{
    logInfo ("Read soil parameters...");

    QString queryString = "SELECT id_texture, alpha, n, he, theta_r, theta_s, ksat, l";
    queryString += " FROM soil_van_genuchten";
    queryString += " ORDER BY id_texture";

    QSqlQuery query = db.exec(queryString);
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

    /*! read values */
    int id, j;
    float myValue;
    while (query.next())
    {
        id = query.value(0).toInt();
        /*! check data */
        for (j = 0; j <= 7; j++)
            if (! getValue(query.value(j), &myValue))
            {
                this->projectError = "Table 'soil_van_genuchten' Missing data in soil texture:" + QString::number(id);
                return(false);
            }
        soilClass[id].vanGenuchten.alpha = query.value(1).toFloat();
        soilClass[id].vanGenuchten.n = query.value(2).toFloat();
        soilClass[id].vanGenuchten.he = query.value(3).toFloat();
        soilClass[id].vanGenuchten.thetaR = query.value(4).toFloat();
        soilClass[id].vanGenuchten.thetaS = query.value(5).toFloat();
        soilClass[id].waterConductivity.kSat = query.value(6).toFloat();
        soilClass[id].waterConductivity.l = query.value(7).toFloat();

        soilClass[id].vanGenuchten.m = 1.0 - 1.0 / soilClass[id].vanGenuchten.n;
    }

    return(true);
}


bool Crit3DProject::loadHorizons(soil::Crit3DSoil* outputSoil, int idSoil)
{
    QString queryString = "SELECT id_soil, id_horizon, upper_depth, lower_depth, skeleton, ";
    queryString += "id_texture, sand, silt, clay, ";
    queryString += "organic_matter, bulk_density, theta_sat, ksat FROM horizons ";
    queryString += "WHERE id_soil=" + QString::number(idSoil) + " ORDER BY id_horizon";

    QSqlQuery query = db.exec(queryString);
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

    int idTexture, idHorizon;
    double sand, silt, clay, organicMatter, skeleton;
    double bulkDensity, porosity, ksat;
    int i = 0;
    while (query.next())
    {
        /*! check depth */
        idHorizon = query.value(1).toInt();
        getValue(query.value(2), &(mySoil->horizon[i].upperDepth));
        getValue(query.value(3), &(mySoil->horizon[i].lowerDepth));
        if ((mySoil->horizon[i].upperDepth == NODATA) || (mySoil->horizon[i].lowerDepth == NODATA)
                || (mySoil->horizon[i].lowerDepth < mySoil->horizon[i].upperDepth))
        {
            this->projectError = "Wrong depths in soil " + QString::number(idSoil)
                    + " horizon " + QString::number(idHorizon);
            return(false);
        }

        /*! [cm]->[m] */
        mySoil->horizon[i].upperDepth /= 100.0;
        mySoil->horizon[i].lowerDepth /= 100.0;

        /*! check skeleton*/
        getValue(query.value(4), &skeleton);
        if (skeleton == NODATA)
            mySoil->horizon[i].coarseFragments = 0.;
        else
            mySoil->horizon[i].coarseFragments = skeleton / 100.0; /*!< [0,1] */

        /*! texture */
        getValue(query.value(5), &idTexture);
        getValue(query.value(6), &sand);
        getValue(query.value(7), &silt);
        getValue(query.value(8), &clay);
        if (idTexture == NODATA)
        {
            if ((sand == NODATA) || (silt == NODATA) || (clay == NODATA))
            {
                    this->projectError = "Missing texture data in soil "
                            + QString::number(idSoil) + " horizon " + QString::number(idHorizon);
                    return (false);
            }

            /*! check Texture */
            idTexture = soil::getUSDATextureClass(sand, silt, clay);
            if (idTexture == NODATA)
            {
                    this->projectError = "Texture wrong (sand+silt+clay <> 100) in soil "
                            + QString::number(idSoil) + " horizon " + QString::number(idHorizon);
                    return (false);
            }
        }
        // TODO
        if (clay == NODATA) clay = 25;
        mySoil->horizon[i].texture.sand = sand;
        mySoil->horizon[i].texture.silt = silt;
        mySoil->horizon[i].texture.clay = clay;

        /*! default values */
        mySoil->horizon[i].texture.classUSDA = idTexture;
        mySoil->horizon[i].vanGenuchten = soilClass[idTexture].vanGenuchten;
        mySoil->horizon[i].waterConductivity = soilClass[idTexture].waterConductivity;

        /*! organic matter */
        getValue(query.value(9), &organicMatter);
        if (organicMatter == NODATA)
            organicMatter = 0.005;      //minimum value: 0.5%
        else
            organicMatter /= 100.0;     //[0-1]
        mySoil->horizon[i].organicMatter = organicMatter;

        /*! bulk density and porosity */
        getValue(query.value(10), &bulkDensity);
        getValue(query.value(11), &porosity);

        if (bulkDensity == NODATA)
            bulkDensity = soil::estimateBulkDensity(&(mySoil->horizon[i]), porosity);
        if (porosity == NODATA)
            porosity = soil::estimateTotalPorosity(&(mySoil->horizon[i]), bulkDensity);

        mySoil->horizon[i].bulkDensity = bulkDensity;

        /*! saturated conductivity */
        getValue(query.value(12), &ksat);
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

        /*! update with skeleton */
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

bool Crit3DProject::loadSoils()
{
    logInfo ("Read soils...");

    QString queryString = "SELECT id_soil FROM soils ORDER BY id_soil";

    QSqlQuery query = db.exec(queryString);
    if (query.size() == -1)
    {
        this->projectError = "Function 'loadSoils' - Table 'soils'\n" + query.lastError().text();
        return(false);
    }

    free(soilList);
    nrSoils = query.size();
    soilList = new soil::Crit3DSoil[nrSoils];

    int idSoil, index = 0;
    float maxSoilDepth = 0;
    while (query.next())
    {
        idSoil = query.value(0).toInt();
        if (! loadHorizons(&(soilList[index]), idSoil))
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


bool Crit3DProject::getMeteoVarIndexRaw(meteoVariable myVar, int* nrIndices, int** varIndices)
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

void Crit3DProject::initializeMeteoPoints()
{
    if (nrMeteoPoints > 0)
    {
        if (meteoPoints != NULL)
        {
            for (int i = 0; i < nrMeteoPoints; i++)
                meteoPoints[i].cleanObsDataH();
            delete [] meteoPoints;
        }
        nrMeteoPoints = 0;
    }
}

void Crit3DProject::initializeRadiationMaps()
{
    meteoMaps->radiationMaps->beamRadiationMap->emptyGrid();
    meteoMaps->radiationMaps->diffuseRadiationMap->emptyGrid();
    meteoMaps->radiationMaps->globalRadiationMap->emptyGrid();
    meteoMaps->radiationMaps->reflectedRadiationMap->emptyGrid();
    meteoMaps->radiationMaps->sunAzimuthMap->emptyGrid();
    meteoMaps->radiationMaps->sunElevationMap->emptyGrid();
    meteoMaps->radiationMaps->sunIncidenceMap->emptyGrid();
    meteoMaps->radiationMaps->sunShadowMap->emptyGrid();
    meteoMaps->radiationMaps->transmissivityMap->emptyGrid();
}

void Crit3DProject::initializeMeteoMaps()
{
    meteoMaps->airTemperatureMap->emptyGrid();
    meteoMaps->precipitationMap->emptyGrid();
    meteoMaps->airHumidityMap->emptyGrid();
    meteoMaps->airDewTemperatureMap->emptyGrid();
    meteoMaps->leafWetnessMap->emptyGrid();
    meteoMaps->ET0Map->emptyGrid();
    meteoMaps->windIntensityMap->emptyGrid();
    meteoMaps->evaporationMap->emptyGrid();
    meteoMaps->irrigationMap->emptyGrid();
    initializeRadiationMaps();
}

bool Crit3DProject::loadDBPoints()
{
    logInfo ("Read points locations...");

    QString queryString = "SELECT id_location, name, utm_x, utm_y, height, is_utc FROM locations";
    queryString += " ORDER BY id_location";

    QSqlQuery query = db.exec(queryString);
    if (query.size() == -1)
    {
        this->projectError = "Query failed in Table 'locations'\n" + query.lastError().text();
        return(false);
    }

    initializeMeteoPoints();
    nrMeteoPoints = query.size();
    meteoPoints = new Crit3DMeteoPoint[nrMeteoPoints];

    /*! read values */
    int i = 0;
    while (query.next())
    {
        meteoPoints[i].id = query.value(0).toString().toStdString();
        meteoPoints[i].name = query.value(1).toString().toStdString();
        meteoPoints[i].point.utm.x = query.value(2).toFloat();
        meteoPoints[i].point.utm.y = query.value(3).toFloat();
        meteoPoints[i].point.z = query.value(4).toFloat();
        meteoPoints[i].isUTC = query.value(5).toBool();
        i++;
    }

    return(true);
}

float Crit3DProject::meteoDataConsistency(meteoVariable myVar, const Crit3DTime& myTimeIni, const Crit3DTime& myTimeFin)
{
    float dataConsistency = 0.0;
    for (int i = 0; i < nrMeteoPoints; i++)
        dataConsistency = maxValue(dataConsistency, meteoPoints[i].obsDataConsistencyH(myVar, myTimeIni, myTimeFin));

    return dataConsistency;
}

bool Crit3DProject::meteoDataLoaded(const Crit3DTime& myTimeIni, const Crit3DTime& myTimeFin)
{
    for (int i = 0; i < nrMeteoPoints; i++)
        if (meteoPoints[i].isDateIntervalLoadedH(myTimeIni, myTimeFin))
            return true;

    return false;
}


/*!
 * \brief observed data: aggregation hourly
 * \param indexPoint
 * \param d1 date
 * \param d2 date
 * \param tableName
 * \return ture if data are available, false otherwise
 */
bool Crit3DProject::loadObsDataHourly(int indexPoint, QDate d1, QDate d2, QString tableName)
{
    QDate myDate;
    QString queryString;
    int i, j, myHour;
    meteoVariable myVar;
    float myValue;
    bool dataAvailable = false;

    if (nrMeteoPoints <= indexPoint)
    {
        logError("Function loadObsDataHourly: wrong point index");
        return(false);
    }
    Crit3DMeteoPoint* myPoint = &(meteoPoints[indexPoint]);

    int hourlyFraction = 1;

    queryString = "SELECT date_, hour_, id_variable, obs_value FROM " + tableName;
    queryString += " WHERE id_location = " + QString::fromStdString(myPoint->id);
    queryString += " AND date_ >= '" + d1.toString("yyyy-MM-dd") + "'";
    queryString += " AND date_ <= '" + d2.toString("yyyy-MM-dd") + "'";
    queryString += " ORDER BY date_, hour_";

    QSqlQuery myQuery = db.exec(queryString);

    if (myQuery.size() == -1)
    {
        logError("Query failed in Table " + tableName + "\n" + myQuery.lastError().text());
        return(false);
    }
    else if (myQuery.size() == 0) return(false);

    /*! read values */
    while (myQuery.next())
    {
        myDate = myQuery.value(0).toDate();
        myHour = myQuery.value(1).toInt();
        /*! transform local time in UTC */
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

                myVar = getMeteoVariable(myQuery.value(2).toInt());

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
                    else if (myVar == airHumidity)
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
                    else if (myVar == atmPressure)
                    {
                        if ((myValue >= 800)&&(myValue <= 1100))
                        myPoint->obsDataH[i].pressure[j] = myValue;
                    }
                    else if (myVar == wetnessDuration)
                    {
                        if ((myValue >= 0)&&(myValue <= 60))
                        myPoint->obsDataH[i].wetDuration[j] = myValue;
                    }
                }
            }
        }

    myQuery.clear();
    return(dataAvailable);
}


bool Crit3DProject::loadObsDataHourlyVar(int indexPoint, meteoVariable myVar, QDate d1, QDate d2, QString tableName)
{
    QDate myDate;
    QString queryString;
    int i, j, myHour;
    float myValue;
    int nrIndices;
    int* varIndices;
    bool dataAvailable=false;

    if (nrMeteoPoints <= indexPoint)
    {
        logError("Function loadObsDataBoundary: wrong point index");
        return(false);
    }
    Crit3DMeteoPoint* myPoint = &(meteoPoints[indexPoint]);

    queryString = "SELECT date_, hour_, obs_value FROM " + tableName;
    if (!this->getMeteoVarIndexRaw(myVar, &nrIndices, &varIndices)) return false;
    queryString += " WHERE id_variable IN (";
    for (int i=0; i<nrIndices-1; i++)
        queryString += QString::number(varIndices[i]) + ",";
    queryString += QString::number(varIndices[nrIndices-1]) + ")";

    queryString += " AND id_location = " + QString::fromStdString(myPoint->id);
    queryString += " AND date_ >= '" + d1.toString("yyyy-MM-dd") + "'";
    queryString += " AND date_ <= '" + d2.toString("yyyy-MM-dd") + "'";
    queryString += " ORDER BY date_, hour_";

    QSqlQuery myQuery = db.exec(queryString);
    if (myQuery.size() == -1)
    {
        logError("Query failed in table: "+ tableName + "\n" + myQuery.lastError().text());
        return(false);
    }
    else if (myQuery.size() == 0) return(false);

    QDate pointFirstDate = getQDate(myPoint->obsDataH[0].date);

    /*! read values */
    while (myQuery.next())
    {
        myDate = myQuery.value(0).toDate();
        myHour = myQuery.value(1).toInt();
        /*! transform local time in UTC */
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
            j = myHour * hourlyIntervals;

            if (getValue(myQuery.value(2), &myValue))
            {
                dataAvailable = true;
                if (myVar == airTemperature)
                    myPoint->obsDataH[i].tAir[j] = myValue;
                else if (myVar == precipitation)
                    myPoint->obsDataH[i].prec[j] = myValue;
                else if (myVar == airHumidity)
                    myPoint->obsDataH[i].rhAir[j] = myValue;
                else if (myVar == globalIrradiance)
                    myPoint->obsDataH[i].irradiance[j] = myValue;
                else if (myVar == windIntensity)
                    myPoint->obsDataH[i].windInt[j] = myValue;
                else if (myVar == atmPressure)
                    myPoint->obsDataH[i].pressure[j] = myValue;
                else if (myVar == wetnessDuration)
                    myPoint->obsDataH[i].wetDuration[j] = myValue;
            }
        }
    }

    myQuery.clear();
    return(dataAvailable);
}


bool Crit3DProject::loadObsDataAllPoints(QDate d1, QDate d2)
{
    isObsDataLoaded = false;

    logInfo("Load observed data:" + d1.toString() + " " + d2.toString());

    int nrDays = d1.daysTo(d2) + 1;
    int hourlyFraction = 1;

    for (int i = 0; i < nrMeteoPoints; i++)
    {
        meteoPoints[i].initializeObsDataH(hourlyFraction, nrDays, getCrit3DDate(d1));

        if (loadObsDataHourly(i, d1, d2, "obs_values_h"))
            isObsDataLoaded = true;
    }

    if (! isObsDataLoaded) this->projectError = "Missing observed data.";

    return(isObsDataLoaded);
}


bool Crit3DProject::loadObsDataAllPointsVar(meteoVariable myVar, QDate d1, QDate d2)
{
    isObsDataLoaded = false;

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

        if (loadObsDataHourlyVar(i, myVar, d1, d2, "obs_values_h"))
            isObsDataLoaded = true;
    }

    if (! isObsDataLoaded) this->projectError = "missing data";
    return(isObsDataLoaded);
}


int Crit3DProject::getIndexPointFromId(QString myId)
{
    for (int i = 0; i < nrMeteoPoints; i++)
        if (QString::fromStdString(meteoPoints[i].id) == myId)
            return(i);
    return(NODATA);
}


float Crit3DProject::getTimeStep()
{ return (3600. / this->hourlyIntervals);}


bool removeDirectory(QString myPath)
{
    QDir myDirectory(myPath);
    myDirectory.setNameFilters(QStringList() << "*.*");
    myDirectory.setFilter(QDir::Files);
    /*! remove all files */
    foreach(QString myFile, myDirectory.entryList())
    {
        myDirectory.remove(myFile);
    }
    return myDirectory.rmdir(myPath);
}


bool Crit3DProject::LoadObsDataFilled(QDateTime firstTime, QDateTime lastTime)
{
    QDate today = QDate::currentDate();
    QDate d1 = firstTime.date().addDays(-30);
    QDate d2 = lastTime.date().addDays(30);
    if (d2 > today) d2 = today;

    if (! this->loadObsDataAllPoints(d1, d2)) return(false);

    /*! Replace missing data */
    long nrReplacedData = 0;
    Crit3DTime myTime = getCrit3DTime(firstTime);
    long nrHours = firstTime.secsTo(lastTime) / 3600;
    for (int i = 0; i <=nrHours; i++)
    {
        if (!checkLackOfData(this, airTemperature, myTime, &nrReplacedData)
            || !checkLackOfData(this, precipitation, myTime, &nrReplacedData)
            || !checkLackOfData(this, airHumidity, myTime, &nrReplacedData))
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


bool Crit3DProject::runModels(QDateTime dateTime1, QDateTime dateTime2, bool isSaveOutput, const QString& myArea)
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
                /*! create output directories */
                myOutputPathDaily = this->path + this->dailyOutputPath + myDate.toString("yyyy/MM/dd/");
                myOutputPathHourly = this->path + "hourly_output/" + myDate.toString("yyyy/MM/dd/");
                this->logInfo("daily Output path: " + myOutputPathDaily);
                this->logInfo("hourly Output path: " + myOutputPathHourly);
                if ((! myDir.mkpath(myOutputPathDaily)) || (! myDir.mkpath(myOutputPathHourly)))
                {
                    this->logError("Creation output directories failed." );
                    isSaveOutput = false;
                }
            }

            /*! load average air temperature map, if exists*/
            loadDailyMeteoMap(this, airTemperatureMean, myDate.addDays(-1), myArea);

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
                aggregateAndSaveDailyMap(this, airHumidity, aggregationMin, getCrit3DDate(myDate), myOutputPathDaily, myOutputPathHourly, myArea);
                aggregateAndSaveDailyMap(this, airHumidity, aggregationMax, getCrit3DDate(myDate), myOutputPathDaily, myOutputPathHourly, myArea);
                aggregateAndSaveDailyMap(this, airHumidity, aggregationMean, getCrit3DDate(myDate), myOutputPathDaily, myOutputPathHourly, myArea);
                aggregateAndSaveDailyMap(this, windIntensity, aggregationMean, getCrit3DDate(myDate), myOutputPathDaily, myOutputPathHourly, myArea);
                aggregateAndSaveDailyMap(this, globalIrradiance, aggregationIntegration, getCrit3DDate(myDate), myOutputPathDaily, myOutputPathHourly, myArea);
                aggregateAndSaveDailyMap(this, wetnessDuration, aggregationSum, getCrit3DDate(myDate), myOutputPathDaily, myOutputPathHourly, myArea);
                aggregateAndSaveDailyMap(this, potentialEvapotranspiration, aggregationSum, getCrit3DDate(myDate), myOutputPathDaily, myOutputPathHourly, myArea);
                aggregateAndSaveDailyMap(this, realEvaporation, aggregationSum, getCrit3DDate(myDate), myOutputPathDaily, myOutputPathHourly, myArea);
                if (removeDirectory(myOutputPathHourly)) this->logInfo("Delete hourly files");
            }

            /*! load daily map (for desease) */
            if (! loadDailyMeteoMap(this, airTemperatureMean, myDate, myArea)) return false;
            if (! loadDailyMeteoMap(this, airHumidityMean, myDate, myArea)) return false;
            if (! loadDailyMeteoMap(this, precipitation, myDate, myArea))  return false;
            if (! loadDailyMeteoMap(this, wetnessDuration, myDate, myArea)) return false;
            updateThermalSum(this, myDate);

            /*! state and output */
            saveStateAndOutput(myDate, myArea);
        }
    }

    logInfo("end of run");
    return true;
}


bool Crit3DProject::loadStates(QDate myDate, QString myArea)
{
    QString statePath = this->path + "states/" + myDate.toString("yyyy/MM/dd/");

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
        this->statePlantMaps->isHarvestedMap->setConstantValueWithBase(0, this->dtm);
    }

    //problema: mancano nei precedenti stati
    loadPlantState(this, cumRadFruitsetVerVar, myDate, statePath, myArea);
    loadPlantState(this, degreeDaysFromFirstMarchVar, myDate, statePath, myArea);
    loadPlantState(this, degreeDaysAtFruitSetVar, myDate, statePath, myArea);

    if (!loadWaterBalanceState(this, myDate, myArea, statePath, waterMatricPotential)) return false;

    this->logInfo("Load state: " + myDate.toString("yyyy-MM-dd"));
    return(true);
}

bool Crit3DProject::saveStateAndOutput(QDate myDate, QString myArea)
{
    QDir myDir;
    QString statePath = this->path + "states/" + myDate.toString("yyyy/MM/dd/");
    QString outputPath = this->path + this->dailyOutputPath + myDate.toString("yyyy/MM/dd/");
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
    if (!savePlantState(this, degreeDaysFromFirstMarchVar, myDate, statePath, myArea)) return(false);
    if (!savePlantState(this, degreeDaysAtFruitSetVar, myDate, statePath, myArea)) return(false);
    if (!savePlantState(this,fruitBiomassIndexVar,myDate,statePath, myArea)) return(false);

    if (!saveWaterBalanceState(this, myDate, myArea, statePath, waterMatricPotential)) return (false);

    QString notes = "";
    if (!savePlantOutput(this, wineYieldVar, myDate, outputPath, myArea, notes, false, true)) return(false);
    if (!savePlantOutput(this, stageVar, myDate, outputPath, myArea, notes, true, true)) return(false);
    //if (!savePlantOutput(this, tartaricAcidVar, myDate, outputPath, myArea, notes, false, true)) return(false);
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

    if (!saveWaterBalanceOutput(this, myDate, waterMatricPotential, "matricPotential10", "10cm", outputPath, myArea, 0.1, 0.1)) return false;
    if (!saveWaterBalanceOutput(this, myDate, waterMatricPotential, "matricPotential30", "30cm", outputPath, myArea, 0.3, 0.3)) return false;
    if (this->soilDepth >= 0.7)
    {
        if (!saveWaterBalanceOutput(this, myDate, waterMatricPotential, "matricPotential70", "70cm", outputPath, myArea, 0.7, 0.7)) return false;
    }

    if (!saveWaterBalanceOutput(this, myDate, degreeOfSaturation, "degreeOfSaturation", "soilDepth", outputPath, myArea, 0.0, this->soilDepth-0.01)) return false;
    if (!saveWaterBalanceOutput(this, myDate, soilSurfaceMoisture, "SSM", "5cm", outputPath, myArea, 0.0, 0.05)) return false;
    if (!saveWaterBalanceOutput(this, myDate, availableWaterContent, "waterContent", "rootZone", outputPath, myArea, 0.0, this->soilDepth)) return false;

    return(true);
}

int Crit3DProject::getFieldIndex(long row, long col)
{
    int fieldIndex = this->fieldMap.value[row][col];
    if (fieldIndex == this->fieldMap.header->flag)
        //DEFAULT
        fieldIndex = 0;
    return fieldIndex;
}


bool Crit3DProject::isVineyard(long row, long col)
{
    int fieldIndex = getFieldIndex(row, col);
    if ((fieldIndex != NODATA) && (fieldIndex > 0))
        return true;
    else
        return false;
}

int Crit3DProject::getSoilIndex(long row, long col)
{
    int fieldIndex = this->getFieldIndex(row, col);
    //return this->vineFields[fieldIndex].soilIndex;
    return fieldIndex;
}

//-------- LOG functions -------------------------
bool Crit3DProject::setLogFile()
{
    QString fileName, myPath, myDate;
    QDir myDir;

    myPath = this->path + "log/";
    myDir.mkpath(myPath);
    //TODO check and error if directory not created

    myDate = QDateTime().currentDateTime().toString("yyyyMMdd_hhmm");
    fileName = "log_" + idArea + "_" + myDate + ".txt";

    this->logFileName = myPath + fileName;
    this->logFile.open(this->logFileName.toStdString().c_str());
    return (logFile.is_open());
}

void Crit3DProject::logInfo(QString myLog)
{
    if (logFile.is_open())
            logFile << myLog.toStdString() << std::endl;

    if (environment == gui)
        std::cout << myLog.toStdString() << std::endl;
}

void Crit3DProject::logError()
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

void Crit3DProject::logError(QString myError)
{
    this->projectError = myError;
    logError();
}

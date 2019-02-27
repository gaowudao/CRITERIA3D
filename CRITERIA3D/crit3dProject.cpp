/*!
    \copyright 2018 Fausto Tomei, Gabriele Antolini,
    Alberto Pistocchi, Marco Bittelli, Antonio Volta, Laura Costantini

    This file is part of CRITERIA3D.
    CRITERIA3D has been developed under contract issued by ARPAE Emilia-Romagna

    CRITERIA3D is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    CRITERIA3D is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with CRITERIA3D.  If not, see <http://www.gnu.org/licenses/>.

    contacts:
    fausto.tomei@gmail.com
    ftomei@arpae.it
    gantolini@arpae.it
*/


#include "commonConstants.h"
#include "formInfo.h"
#include "dbTools.h"
#include "utilities.h"
#include "crit3dProject.h"
#include "waterBalance3D.h"


Crit3DProject::Crit3DProject()
{
    meteoSettings = new Crit3DMeteoSettings();
    isParametersLoaded = false;
    isInitialized = false;
    nrSoils = 0;
    nrLayers = 0;
    nrVoxelsPerLayer = 0;
    nrVoxels = 0;
    soilDepth = 0.5;                    // [m]
}


bool Crit3DProject::readCriteria3DParameters()
{          
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

    QString myError;
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
        logError(myError);
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
            myError = "";
            if (loadSoil(&dbSoil, soilCode, mySoil, soilClass, &myError))
            {
                mySoil->id = idSoil;
                soilList.push_back(*mySoil);
            }
            else
            {
                wrongSoils += myError + "\n";
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
        logError(wrongSoils);
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
            if (int(DTM.value[row][col]) != int(DTM.header->flag))
            {
                indexMap.value[row][col] = float(index);
                index++;
            }
        }
    }

    gis::updateMinMaxRasterGrid(&indexMap);
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
            if (int(DTM.value[row][col]) != int(DTM.header->flag))
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


int Crit3DProject::getSoilIndex(int dtmRow, int dtmCol)
{
    double x, y;
    int idSoil;

    gis::getUtmXYFromRowCol(*(DTM.header), dtmRow, dtmCol, &x, &y);
    idSoil = int(gis::getValueFromXY(soilMap, x, y));

    if (idSoil == int(this->soilMap.header->flag))
    {
        return INDEX_ERROR;
    }

    // search id soil
    for (unsigned int i = 0; i < soilList.size(); i++)
    {
        if (soilList[i].id == idSoil) return(int(i));
    }

    // no soil data
    return INDEX_ERROR;
}


double Crit3DProject::getSoilVar(int soilIndex, int layerIndex, soil::soilVariable myVar)
{
    int horizonIndex = soil::getHorizonIndex(&(soilList[unsigned(soilIndex)]), layerDepth[unsigned(layerIndex)]);
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



void Crit3DProject::cleanProject()
{
    soilIndexMap.freeGrid();
    cropIndexMap.freeGrid();
    boundaryMap.freeGrid();
    indexMap.freeGrid();

    delete meteoMaps;

    cleanWaterBalanceMemory();
    isInitialized = false;
}


bool Crit3DProject::setDEM(QString myFileName)
{
    if (! this->loadDEM(myFileName))
        return false;

    this->meteoMaps = new Crit3DMeteoMaps(this->DTM);
    return true;
}


bool Crit3DProject::computeAllMeteoMaps(const Crit3DTime& myTime, bool showInfo)
{
    if (! DTM.isLoaded)
    {
        errorString = "Load a DEM before.";
        return false;
    }

    this->meteoMaps->isComputed = false;

    FormInfo myInfo;
    if (showInfo)
    {
        myInfo.start("Compute meteo maps...", 6);
    }

    if (! interpolationDemMain(airTemperature, myTime, this->meteoMaps->airTemperatureMap, false))
        return false;

    if (showInfo) myInfo.setValue(1);

    if (! interpolationDemMain(airRelHumidity, myTime, this->meteoMaps->airHumidityMap, false))
        return false;

    if (showInfo) myInfo.setValue(2);

    if (! interpolationDemMain(precipitation, myTime, this->meteoMaps->precipitationMap, false))
        return false;

    if (showInfo) myInfo.setValue(3);

    if (! interpolationDemMain(windIntensity, myTime, this->meteoMaps->windIntensityMap, false))
        return false;

    if (showInfo) myInfo.setValue(4);

    if (! interpolationDemMain(globalIrradiance, myTime, &(this->dataRaster), false))
        return false;

    if (showInfo) myInfo.setValue(5);

    if (! this->meteoMaps->computeET0Map(&(this->DTM), this->radiationMaps))
        return false;

    if (showInfo) myInfo.close();

    this->meteoMaps->isComputed = true;

    return true;
}


bool Crit3DProject::initializeCriteria3D()
{
    cleanProject();

    // check
    if (! this->DTM.isLoaded)
    {
        logError("Missing DTM.");
        return false;
    }
    else if (! this->soilMap.isLoaded)
    {
        logError("Missing soil map.");
        return false;
    }
    else if (this->soilList.size() == 0)
    {
        logError("Missing soil properties.");
        return false;
    }

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

    this->isInitialized = true;
    return true;
}


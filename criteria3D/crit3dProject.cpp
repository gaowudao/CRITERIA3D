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

#include "waterBalance.h"
#include "crit3dProject.h"
#include "commonConstants.h"


Crit3DProject::Crit3DProject()
{
    dtm = NULL;
    radiationMaps = NULL;

    error = "";
    log = "";
    nrSoils = 0;
    nrLayers = 0;
    nrVoxelsPerLayer = 0;
    nrVoxels = 0;
    soilDepth = 1.0;       //[m] default: 1 meter
    isParametersLoaded = false;
}


void Crit3DProject::closeProject()
{
    soilIndexMap.freeGrid();
    cropIndexMap.freeGrid();
    boundaryMap.freeGrid();
    indexMap.freeGrid();

    delete meteoMaps;

    cleanWaterBalanceMemory();
}


bool Crit3DProject::initializeProject(gis::Crit3DRasterGrid* myDTM, Crit3DRadiationMaps* myRadiationMaps)
{
    closeProject();

    // DTM and soil properties
    dtm = myDTM;
    if (!createSoilIndexMap())
        return false;

    // meteo maps
    radiationMaps = myRadiationMaps;
    meteoMaps = new Crit3DMeteoMaps(*dtm);

    // loadCropProperties()
    // crop map

    if (! initializeWaterBalance(this))
        return false;

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

    setLog("Criteria3D Project initialized");
    return(true);
}


int Crit3DProject::getSoilIndex(int dtmRow, int dtmCol)
{
    double x, y;
    int idSoil;

    gis::getUtmXYFromRowCol(*(dtm->header), dtmRow, dtmCol, &x, &y);
    idSoil = int(gis::getValueFromXY(soilMap, x, y));

    if (idSoil == int(this->soilMap.header->flag))
    {
        return INDEX_ERROR;
    }

    // search id soil
    for (int i = 0; i < soilList.size(); i++)
    {
        if (soilList[i].id == idSoil) return(i);
    }

    // no soil data
    return INDEX_ERROR;
}


bool Crit3DProject::createSoilIndexMap()
{
    // check
    if (dtm == NULL || !dtm->isLoaded || !soilMap.isLoaded || soilList.size() == 0)
    {
        if (dtm == NULL || !dtm->isLoaded)
            setError("Missing DTM.");
        else if (!soilMap.isLoaded)
            setError("Missing soil map.");
        else if (soilList.size() == 0)
            setError("Missing soil properties.");
        return false;
    }

    int soilIndex;
    soilIndexMap.initializeGrid(*(dtm->header));
    for (int row = 0; row < dtm->header->nrRows; row++)
    {
        for (int col = 0; col < dtm->header->nrCols; col++)
        {
            if (dtm->value[row][col] != dtm->header->flag)
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


bool Crit3DProject::createIndexMap()
{
    // check
    if (dtm == NULL || !dtm->isLoaded)
    {
        setError("Missing DTM.");
        return false;
    }

    indexMap.initializeGrid(*(dtm->header));

    long index = 0;
    for (int row = 0; row < indexMap.header->nrRows; row++)
    {
        for (int col = 0; col < indexMap.header->nrCols; col++)
        {
            if (dtm->value[row][col] != dtm->header->flag)
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
    if (dtm == NULL || !dtm->isLoaded)
    {
        setError("Missing DTM.");
        return false;
    }
    boundaryMap.initializeGrid(*(dtm->header));

    for (int row = 0; row < boundaryMap.header->nrRows; row++)
    {
        for (int col = 0; col < boundaryMap.header->nrCols; col++)
        {
            if (gis::isBoundary(*(dtm), row, col))
            {
                if (gis::isMinimum(*(dtm), row, col))
                    boundaryMap.value[row][col] = BOUNDARY_RUNOFF;
            }
        }
    }

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


double* Crit3DProject::getSoilVarProfile(int row, int col, soil::soilVariable myVar)
{
    double* myProfile = (double *) calloc(nrLayers, sizeof(double));
    for (int layerIndex = 0; layerIndex < nrLayers; layerIndex++)
        myProfile[layerIndex] = NODATA;

    long firstLayerIndex = long(indexMap.value[row][col]);
    int soilIndex = int(soilIndexMap.value[row][col]);

    if (firstLayerIndex != indexMap.header->flag && soilIndex != soilIndexMap.header->flag)
        for (int layerIndex = 0; layerIndex < nrLayers; layerIndex++)
            if ((myVar == soil::soilWaterPotentialWP) || (myVar == soil::soilWaterPotentialFC)
                    || (myVar == soil::soilWaterContentFC) || (myVar == soil::soilWaterContentWP))
                    myProfile[layerIndex] = getSoilVar(soilIndex, layerIndex, myVar);

    return myProfile;
}


bool Crit3DProject::setSoilProfileCrop(int row, int col)
{
    double* soilWPProfile = getSoilVarProfile(row, col, soil::soilWaterPotentialWP);
    double* soilFCProfile = getSoilVarProfile(row, col, soil::soilWaterPotentialFC) ;
    double* matricPotentialProfile = getCriteria3DVarProfile(this, row, col, waterMatricPotential);
    double* waterContentProfile = getCriteria3DVarProfile(this, row, col, waterContent);
    double* waterContentProfileWP = getSoilVarProfile(row, col, soil::soilWaterContentWP);
    double* waterContentProfileFC = getSoilVarProfile(row, col, soil::soilWaterContentFC);

    /*
     * passare all'algoritmo della pianta il profilo
    if (! setSoilProfile(soilWPProfile, soilFCProfile,
            matricPotentialProfile, waterContentProfile, waterContentProfileFC,
            waterContentProfileWP)) return false;
    */

    free(soilWPProfile);
    free(soilFCProfile);
    free(matricPotentialProfile);
    free(waterContentProfile);
    free(waterContentProfileWP);
    free(waterContentProfileFC);

    return true;
}


bool Crit3DProject::computeET0Map()
{
    float globalRadiation, transmissivity, clearSkyTransmissivity;
    float temperature, relHumidity, windSpeed, height;

    gis::Crit3DRasterGrid* map = meteoMaps->ET0Map;

    for (long row = 0; row < map->header->nrRows; row++)
        for (long col = 0; col < map->header->nrCols; col++)
        {
            map->value[row][col] = map->header->flag;

            height = dtm->value[row][col];

            if (height != dtm->header->flag)
            {
                globalRadiation = radiationMaps->globalRadiationMap->value[row][col];
                transmissivity = radiationMaps->transmissivityMap->value[row][col];
                clearSkyTransmissivity = radiationMaps->clearSkyTransmissivityMap->value[row][col];
                temperature = meteoMaps->airTemperatureMap->value[row][col];
                relHumidity = meteoMaps->airHumidityMap->value[row][col];
                windSpeed = meteoMaps->windIntensityMap->value[row][col];

                if (globalRadiation != radiationMaps->globalRadiationMap->header->flag
                        && transmissivity != radiationMaps->transmissivityMap->header->flag
                        && clearSkyTransmissivity != radiationMaps->clearSkyTransmissivityMap->header->flag
                        && temperature != meteoMaps->airTemperatureMap->header->flag
                        && relHumidity != meteoMaps->airHumidityMap->header->flag
                        && windSpeed != meteoMaps->windIntensityMap->header->flag)
                {
                    map->value[row][col] = float(ET0_Penman_hourly(height, transmissivity / clearSkyTransmissivity,
                                      globalRadiation, temperature, relHumidity, windSpeed));
                }
            }
        }

    return gis::updateMinMaxRasterGrid(map);
}


//---------------- LOG ------------------------------------------------------------------------------

void Crit3DProject::setLog(std::string myLog)
{
    this->log += myLog + "\n";
}

void Crit3DProject::setError(std::string myError)
{
    this->error = myError;
}


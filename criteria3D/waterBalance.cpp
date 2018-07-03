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

#include <math.h>

#include "commonConstants.h"
#include "waterBalance.h"
#include "soilFluxes3D.h"
#include "crit3dProject.h"


std::vector <double> waterSinkSource;     //[m^3/sec]


void cleanWaterBalanceMemory()
{
    waterSinkSource.resize(0);
    soilFluxes3D::cleanMemory();
}

bool isCrit3dError(int myResult, std::string* myError)
{
    if (myResult == CRIT3D_OK) return(false);

    if (myResult == INDEX_ERROR)
        *myError = "Index error";
    else if (myResult == MEMORY_ERROR)
        *myError = "Memory error";
    else if (myResult == TOPOGRAPHY_ERROR)
        *myError = "Topography error";
    else if (myResult == BOUNDARY_ERROR)
        *myError = "Boundary error";
    else if (myResult == PARAMETER_ERROR)
        *myError = "Parameter error";
    else
        *myError = "Undefined error";

    return (true);
}


bool setCrit3DSoils(Crit3DProject* myProject)
{
    soil::Crit3DHorizon* myHorizon;
    std::string myError;
    int myResult;

    for (int soilIndex = 0; soilIndex < myProject->nrSoils; soilIndex++)
    {
        for (int horizIndex = 0; horizIndex < myProject->soilList[soilIndex].nrHorizons; horizIndex++)
        {
            // TODO cec
            myHorizon = &(myProject->soilList[soilIndex].horizon[horizIndex]);
            if ((myHorizon->texture.classUSDA > 0) && (myHorizon->texture.classUSDA <= 12))
            {
                myResult = soilFluxes3D::setSoilProperties(soilIndex, horizIndex,
                     myHorizon->vanGenuchten.alpha,
                     myHorizon->vanGenuchten.n,
                     myHorizon->vanGenuchten.m,
                     myHorizon->vanGenuchten.he,
                     myHorizon->vanGenuchten.thetaR,
                     myHorizon->vanGenuchten.thetaS,
                     myHorizon->waterConductivity.kSat / 360000.0,
                     myHorizon->waterConductivity.l,
                     myHorizon->organicMatter,
                     myHorizon->texture.clay);

                 if (isCrit3dError(myResult, &myError))
                 {
                     myProject->error = "setCrit3DSoils:" + myError
                         + " in soil nr:" + std::to_string(myProject->soilList[soilIndex].id)
                         + " horizon nr: " + std::to_string(horizIndex);
                     return(false);
                 }
            }
        }
    }
    return(true);
}


bool setCrit3DSurfaces(Crit3DProject* myProject)
{
    std::string myError;
    int myResult;

    // TODO
    int nrSurfaces = 1;
    float ManningRoughness = (float)0.24;         //[s m^-1/3]
    float surfacePond = (float)0.002;             //[m]

    for (int surfaceIndex = 0; surfaceIndex < nrSurfaces; surfaceIndex++)
    {
        myResult = soilFluxes3D::setSurfaceProperties(surfaceIndex, ManningRoughness, surfacePond);
        if (isCrit3dError(myResult, &myError))
        {
            myProject->error = "setCrit3DSurfaces:" + myError
                + " in surface nr:" + std::to_string(surfaceIndex);
            return(false);
        }
    }
    return(true);
}


int computeNrLayers(float totalDepth, double minThickness, double maxThickness, double factor)
 {
    int nrLayers = 1;
    double nextThickness, prevThickness = minThickness;
    double depth = minThickness * 0.5f;
    while (depth < totalDepth)
    {
        nextThickness = minValue(maxThickness, prevThickness * factor);
        depth = depth + (prevThickness + nextThickness) * 0.5f;
        prevThickness = nextThickness;
        nrLayers++;
    }
    return(nrLayers);
}


// set thickness and depth (center) of layers [m]
bool setLayersDepth(Crit3DProject* myProject, double minThickness, double maxThickness, double factor)
{
    int lastLayer = myProject->nrLayers-1;
    myProject->layerDepth.resize(myProject->nrLayers);
    myProject->layerThickness.resize(myProject->nrLayers);

    myProject->layerDepth[0] = 0.0;
    myProject->layerThickness[0] = 0.0;
    myProject->layerThickness[1] = minThickness;
    myProject->layerDepth[1] = minThickness * 0.5;
    for (int i = 2; i < myProject->nrLayers; i++)
    {
        if (i == lastLayer)
            myProject->layerThickness[i] = myProject->soilDepth - (myProject->layerDepth[i-1]
                    + myProject->layerThickness[i-1] / 2.0);
        else
            myProject->layerThickness[i] = minValue(maxThickness, myProject->layerThickness[i-1] * factor);
        myProject->layerDepth[i] = myProject->layerDepth[i-1] +
                (myProject->layerThickness[i-1] + myProject->layerThickness[i]) * 0.5;
    }
    return(true);
}


bool setCrit3DTopography(Crit3DProject* myProject)
{
    double x, y;
    double z, area, lateralArea, volume, slope;
    long index, surfaceIndex, linkIndex;
    int myResult;
    std::string myError;

    for (int row = 0; row < myProject->indexMap.header->nrRows; row++)
        for (int col = 0; col < myProject->indexMap.header->nrCols; col++)
        {
            surfaceIndex = int(myProject->indexMap.value[row][col]);
            if (surfaceIndex != myProject->indexMap.header->flag)
            {
                gis::getUtmXYFromRowCol(*(myProject->dtm->header), row, col, &x, &y);
                area = myProject->dtm->header->cellSize * myProject->dtm->header->cellSize;
                slope = myProject->radiationMaps->slopeMap->value[row][col] / 100.0;

                for (int layer = 0; layer < myProject->nrLayers; layer++)
                {
                    index = layer * myProject->nrVoxelsPerLayer + surfaceIndex;
                    z = myProject->dtm->value[row][col] - myProject->layerDepth[layer];
                    volume = area * myProject->layerThickness[layer];

                    //surface
                    if (layer == 0)
                    {
                        lateralArea = myProject->dtm->header->cellSize;
                        if (myProject->boundaryMap.value[row][col] == BOUNDARY_RUNOFF)
                        {
                            myResult = soilFluxes3D::setNode(index, float(x), float(y), float(z), area,
                                             true, true, BOUNDARY_RUNOFF, float(slope));
                        }
                        else
                        {
                            myResult = soilFluxes3D::setNode(index, float(x), float(y), float(z), area,
                                             true, false, BOUNDARY_NONE, 0.f);
                        }
                    }
                    //sub-surface
                    else
                    {
                        lateralArea = myProject->dtm->header->cellSize * myProject->layerThickness[layer];
                        //last layer
                        if (layer == (myProject->nrLayers - 1))
                        {
                            myResult = soilFluxes3D::setNode(index, float(x), float(y), float(z), volume,
                                             false, true, BOUNDARY_FREEDRAINAGE, 0.f);
                        }
                        else
                        {
                            if (myProject->boundaryMap.value[row][col] == BOUNDARY_RUNOFF)
                            {
                                myResult = soilFluxes3D::setNode(index, float(x), float(y), float(z), volume,
                                                 false, true, BOUNDARY_FREELATERALDRAINAGE, float(slope));
                            }
                            else
                            {
                                myResult = soilFluxes3D::setNode(index, float(x), float(y), float(z), volume,
                                             false, false, BOUNDARY_NONE, 0.f);
                            }
                        }
                    }
                    //check error
                    if (isCrit3dError(myResult, &myError))
                    {
                        myProject->error = "setCrit3DTopography:" + myError
                                    + " in layer nr:" + std::to_string(layer);
                        return(false);
                    }
                    //up link
                    if (layer > 0)
                    {
                        linkIndex = index - myProject->nrVoxelsPerLayer;
                        myResult = soilFluxes3D::setNodeLink(index, linkIndex, UP, float(area));
                        if (isCrit3dError(myResult, &myError))
                        {
                            myProject->error = "setNodeLink:" + myError
                                    + " in layer nr:" + std::to_string(layer);
                            return(false);
                        }
                    }
                    //down link
                    if (layer < (myProject->nrLayers - 1))
                    {
                        linkIndex = index + myProject->nrVoxelsPerLayer;
                        myResult = soilFluxes3D::setNodeLink(index, linkIndex, DOWN, float(area));
                        if (isCrit3dError(myResult, &myError))
                        {
                            myProject->error = "setNodeLink:" + myError
                                    + " in layer nr:" + std::to_string(layer);
                            return(false);
                        }
                    }
                    //lateral links
                    for (int i=-1; i <= 1; i++)
                    {
                        for (int j=-1; j <= 1; j++)
                        {
                            if ((i != 0) || (j != 0))
                            {
                                if (! gis::isOutOfGridRowCol(row+i, col+j, myProject->indexMap))
                                {
                                    linkIndex = long(myProject->indexMap.value[row+i][col+j]);
                                    if (linkIndex != myProject->indexMap.header->flag)
                                    {
                                        linkIndex += layer * myProject->nrVoxelsPerLayer;
                                        myResult = soilFluxes3D::setNodeLink(index, linkIndex, LATERAL, float(lateralArea / 2.));
                                        if (isCrit3dError(myResult, &myError))
                                        {
                                            myProject->error = "setNodeLink:" + myError
                                                    + " in layer nr:" + std::to_string(layer);
                                            return(false);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
         }

   return (true);
}


bool setCrit3DNodeSoil(Crit3DProject* myProject)
{
    long index, surfaceIndex;
    int soilIndex, horizonIndex;
    int myResult;
    std::string myError;

    for (int row = 0; row < myProject->indexMap.header->nrRows; row++)
        for (int col = 0; col < myProject->indexMap.header->nrCols; col++)
        {
            surfaceIndex = long(myProject->indexMap.value[row][col]);
            if (surfaceIndex != long(myProject->indexMap.header->flag))
            {
                soilIndex = int(myProject->soilIndexMap.value[row][col]);
                if (soilIndex != int(myProject->soilIndexMap.header->flag))
                {
                    for (int layer = 0; layer < myProject->nrLayers; layer++)
                    {
                        index = layer * myProject->nrVoxelsPerLayer + surfaceIndex;
                        //surface
                        if (layer == 0)
                        {
                            myResult = soilFluxes3D::setNodeSurface(index, 0);
                        }
                        //sub-surface
                        else
                        {
                            horizonIndex = soil::getHorizonIndex(&(myProject->soilList[soilIndex]), myProject->layerDepth[layer]);
                            if (horizonIndex == NODATA)
                            {
                                myProject->error = "function setCrit3DNodeSoil: \nno horizon definition in soil nr: "
                                        + std::to_string(soilIndex) + " depth: " + std::to_string(myProject->layerDepth[layer])
                                        +"\nCheck soil totalDepth.";
                                return(false);
                            }

                            myResult = soilFluxes3D::setNodeSoil(index, soilIndex, horizonIndex);

                            //check error
                            if (isCrit3dError(myResult, &myError))
                            {
                                myProject->error = "setCrit3DNodeSoil:" + myError + " in soil nr: " + std::to_string(soilIndex)
                                        + " horizon nr:" + std::to_string(horizonIndex);
                                return(false);
                            }
                        }
                    }
                }
            }
        }
    return(true);
}


bool initializeSoilMoisture(Crit3DProject* myProject, int month)
{
    int myResult;
    std::string myError;
    long index, surfaceIndex;
    int soilIndex, horizonIndex;
    double moistureIndex, waterPotential;
    double fieldCapacity;                    //[m]
    double  wiltingPoint = -160.0;           //[m]
    double dry = wiltingPoint / 3.0;         //[m] dry potential

    //[0-1] min: july  max: january
    moistureIndex = fabs(1.0 - (month - 1) / 6.0);
    moistureIndex = maxValue(moistureIndex, 0.001);
    moistureIndex = log(moistureIndex) / log(0.001);

    myProject->setLog("Initialize soil moisture");

    for (int row = 0; row < myProject->indexMap.header->nrRows; row++)
    {
        for (int col = 0; col < myProject->indexMap.header->nrCols; col++)
        {
            surfaceIndex = long(myProject->indexMap.value[row][col]);
            if (surfaceIndex != long(myProject->indexMap.header->flag))
            {
                //surface
                soilFluxes3D::setWaterContent(surfaceIndex, 0.0);

                soilIndex = int(myProject->soilIndexMap.value[row][col]);
                if (soilIndex != int(myProject->soilIndexMap.header->flag))
                {
                    for (int layer = 1; layer < myProject->nrLayers; layer++)
                    {
                        index = layer * myProject->nrVoxelsPerLayer + surfaceIndex;
                        horizonIndex = soil::getHorizonIndex(&(myProject->soilList[soilIndex]), myProject->layerDepth[layer]);

                        fieldCapacity = myProject->soilList[soilIndex].horizon[horizonIndex].fieldCapacity;
                        waterPotential = fieldCapacity - moistureIndex * (fieldCapacity-dry);

                        myResult = soilFluxes3D::setMatricPotential(index, waterPotential);

                        if (isCrit3dError(myResult, &myError))
                        {
                            myProject->error = "initializeSoilMoisture:" + myError;
                            return(false);
                        }
                    }
                }
            }
        }
    }
    return true;
}


double getMaxEvaporation(double ET0, double LAI)
{
    const double ke = 0.6;   //[-] light extinction factor
    const double maxEvaporationRatio = 0.66;

    double Kc = exp(-ke * LAI);
    return(ET0 * Kc * maxEvaporationRatio);
}


double evaporation(Crit3DProject* myProject, int row, int col, long surfaceIndex)
{
    double depthCoeff, thickCoeff, layerCoeff;
    double realEvap, residualEvap, layerEvap, availableWater, E0, ET0, flow;
    double laiGrass, laiVine, laiTot;
    long nodeIndex;

    double const MAX_PROF_EVAPORATION = 0.15;           //[m]
    int lastEvapLayer = getSoilLayerIndex(myProject, MAX_PROF_EVAPORATION);
    double area = myProject->dtm->header->cellSize * myProject->dtm->header->cellSize;
    //int idField = myProject->getFieldIndex(row, col);

    //LAI
    laiGrass = 1;   // myProject->vineFields[idField].maxLAIGrass;
    laiVine = 1;    // myProject->statePlantMaps->leafAreaIndexMap->value[row][col];
    laiTot = laiVine + laiGrass;

    //E0 [mm]
    ET0 = myProject->meteoMaps->ET0Map->value[row][col];
    E0 = getMaxEvaporation(ET0, laiTot);
    realEvap = 0;

    for (int layer=0; layer <= lastEvapLayer; layer++)
    {
        nodeIndex = layer * myProject->nrVoxelsPerLayer + surfaceIndex;

        //[m]
        availableWater = getCriteria3DVar(availableWaterContent, nodeIndex);
        if (layer > 0) availableWater *= myProject->layerThickness[layer];
        //->[mm]
        availableWater *= 1000.0;

        //layer coefficient
        if (layer == 0)
        {
            //surface
            layerCoeff = 1.0;
        }
        else
        {
            depthCoeff = myProject->layerDepth[layer] / MAX_PROF_EVAPORATION;
            thickCoeff = myProject->layerThickness[layer] / 0.04;
            layerCoeff = exp(-EULER * depthCoeff) * thickCoeff;
        }

        residualEvap = E0 - realEvap;
        layerEvap = minValue(E0 * layerCoeff, residualEvap);
        layerEvap = minValue(layerEvap, availableWater);

        if (layerEvap > 0.0)
        {
            realEvap += layerEvap;
            flow = area * (layerEvap / 1000.0);                  //[m^3/h]
            waterSinkSource[nodeIndex] -= (flow / 3600.0);        //[m^3/s]
        }
    }
    return 0;
}


bool setWaterSinkSource(Crit3DProject* myProject, double* totalPrecipitation,
                        double* totalEvaporation, double *totalTranspiration)
{
    long surfaceIndex, layerIndex;
    double prec, irr, totalWater;
    double transp, flow, realEvap;
    int myResult;
    std::string myError;

    // initialize
    for (long i = 0; i < myProject->nrVoxels; i++)
        waterSinkSource[i] = 0.0;

    double area = myProject->dtm->header->cellSize * myProject->dtm->header->cellSize;

    // precipitation or irrigation
    *totalPrecipitation = 0.0;
    for (long row = 0; row < myProject->indexMap.header->nrRows; row++)
        for (long col = 0; col < myProject->indexMap.header->nrCols; col++)
        {
            surfaceIndex = long(myProject->indexMap.value[row][col]);
            if (surfaceIndex != long(myProject->indexMap.header->flag))
            {
                totalWater = 0.0;
                prec = myProject->meteoMaps->precipitationMap->value[row][col];
                if (prec != myProject->meteoMaps->precipitationMap->header->flag) totalWater += prec;

                irr = myProject->meteoMaps->irrigationMap->value[row][col];
                if (irr != myProject->meteoMaps->irrigationMap->header->flag) totalWater += irr;

                if (totalWater > 0.0)
                {
                    flow = area * (totalWater / 1000.0);                //[m^3/h]
                    *totalPrecipitation += flow;
                    waterSinkSource[surfaceIndex] += flow / 3600.0;     //[m^3/s]
                }
            }
        }

    // evaporation
    *totalEvaporation = 0.0;
    for (long row = 0; row < myProject->indexMap.header->nrRows; row++)
        for (long col = 0; col < myProject->indexMap.header->nrCols; col++)
        {
            surfaceIndex = long(myProject->indexMap.value[row][col]);
            if (surfaceIndex != long(myProject->indexMap.header->flag))
            {
                realEvap = 0.; //evaporation(myProject, row, col, surfaceIndex);

                flow = area * (realEvap / 1000.0);                  //[m^3/h]
                *totalEvaporation += flow;
            }
        }

    // crop transpiration
    *totalTranspiration = 0.0;
    for (long row = 0; row < myProject->indexMap.header->nrRows; row++)
        for (long col = 0; col < myProject->indexMap.header->nrCols; col++)
        {    
            surfaceIndex = long(myProject->indexMap.value[row][col]);
            if (surfaceIndex != long(myProject->indexMap.header->flag))
            {
                for (layerIndex=1; layerIndex < myProject->nrLayers; layerIndex++)
                {
                    transp = 0.; //myProject->outputPlantMaps->transpirationLayerMaps[layerIndex]->value[row][col];

                    /*if (transp != myProject->outputPlantMaps->transpirationLayerMaps[layerIndex]->header->flag)
                    {
                        flow = area * (transp / 1000.0);                    //[m^3/h]
                        *totalTranspiration += flow;
                        nodeIndex = layerIndex * myProject->nrVoxelsPerLayer + surfaceIndex;
                        waterSinkSource[nodeIndex] -= flow / 3600.0;        //[m^3/s]
                    }*/
                }

            }
        }

    for (long i = 0; i < myProject->nrVoxels; i++)
    {
        myResult = soilFluxes3D::setWaterSinkSource(i, waterSinkSource[i]);
        if (isCrit3dError(myResult, &myError))
        {
            myProject->error = "initializeSoilMoisture:" + myError;
            return(false);
        }
    }

    return(true);
}



double* getCriteria3DVarProfile(Crit3DProject* myProject, int myRow, int myCol, criteria3DVariable myVar)
{
    double* myProfile = (double *) calloc(myProject->nrLayers, sizeof(double));
    long firstLayerIndex = long(myProject->indexMap.value[myRow][myCol]);
    long nodeIndex, layerIndex;

    for (layerIndex = 0; layerIndex < myProject->nrLayers; layerIndex++)
        myProfile[layerIndex] = NODATA;

    if (firstLayerIndex != myProject->indexMap.header->flag)
        for (layerIndex = 0; layerIndex < myProject->nrLayers; layerIndex++)
        {
            nodeIndex = layerIndex * myProject->nrVoxelsPerLayer + firstLayerIndex;
            myProfile[layerIndex] = getCriteria3DVar(myVar, nodeIndex);
        }

    return myProfile;
}


double getCriteria3DVar(criteria3DVariable myVar, long nodeIndex)
{
    double myCrit3dVar;

    if (myVar == waterContent)
        myCrit3dVar = soilFluxes3D::getWaterContent(nodeIndex);
    else if (myVar == availableWaterContent)
        myCrit3dVar = soilFluxes3D::getAvailableWaterContent(nodeIndex);
    else if (myVar == waterDeficit)
        //TODO chiamare soil per field capacity
        myCrit3dVar = soilFluxes3D::getWaterDeficit(nodeIndex, 3.0);
    else if (myVar == waterTotalPotential)
        myCrit3dVar = soilFluxes3D::getTotalPotential(nodeIndex);
    else if (myVar == waterMatricPotential)
        myCrit3dVar = soilFluxes3D::getMatricPotential(nodeIndex);
    else if (myVar == degreeOfSaturation)
        myCrit3dVar = soilFluxes3D::getDegreeOfSaturation(nodeIndex);
    else
        myCrit3dVar = MISSING_DATA_ERROR;

    if (myCrit3dVar != INDEX_ERROR && myCrit3dVar != MEMORY_ERROR &&
            myCrit3dVar != MISSING_DATA_ERROR && myCrit3dVar != TOPOGRAPHY_ERROR)
        return myCrit3dVar;
    else
        return NODATA;
}

bool setCriteria3DVar(criteria3DVariable myVar, long nodeIndex, double myValue)
{
    int myResult;

    if (myVar == waterContent)
        myResult = soilFluxes3D::setWaterContent(nodeIndex, myValue);
    else if (myVar == waterMatricPotential)
        myResult = soilFluxes3D::setMatricPotential(nodeIndex, myValue);
    else
        myResult = MISSING_DATA_ERROR;

    return (myResult != INDEX_ERROR && myResult != MEMORY_ERROR && myResult != MISSING_DATA_ERROR &&
            myResult != TOPOGRAPHY_ERROR);
}


bool setCriteria3DVarMap(int myLayerIndex, Crit3DProject* myProject, criteria3DVariable myVar,
                        gis::Crit3DRasterGrid* myCriteria3DMap)
{
    long surfaceIndex, nodeIndex;

    for (int row = 0; row < myProject->indexMap.header->nrRows; row++)
        for (int col = 0; col < myProject->indexMap.header->nrCols; col++)
        {
            surfaceIndex = long(myProject->indexMap.value[row][col]);
            if (surfaceIndex != long(myProject->indexMap.header->flag))
            {
                nodeIndex = myLayerIndex * myProject->nrVoxelsPerLayer + surfaceIndex;
                if (! setCriteria3DVar(myVar, nodeIndex, myCriteria3DMap->value[row][col])) return false;
            }
        }

    return true;
}


bool getCriteria3DVarMap(Crit3DProject* myProject, criteria3DVariable myVar,
                        int layerIndex, gis::Crit3DRasterGrid* criteria3DMap)
{
    long surfaceIndex, nodeIndex;
    double myValue;

    criteria3DMap->initializeGrid(myProject->indexMap);

    for (int row = 0; row < myProject->indexMap.header->nrRows; row++)
        for (int col = 0; col < myProject->indexMap.header->nrCols; col++)
        {
            surfaceIndex = long(myProject->indexMap.value[row][col]);
            if (surfaceIndex != long(myProject->indexMap.header->flag))
            {
                nodeIndex = layerIndex * myProject->nrVoxelsPerLayer + surfaceIndex;

                myValue = getCriteria3DVar(myVar, nodeIndex);
                if (myValue == NODATA)
                    criteria3DMap->value[row][col] = criteria3DMap->header->flag;
                else
                    criteria3DMap->value[row][col] = float(myValue);
            }
            else
                criteria3DMap->value[row][col] = criteria3DMap->header->flag;
        }

    return true;
}

// AWC in the first 4 cm of soil
bool getSoilSurfaceMoisture(Crit3DProject* myProject, gis::Crit3DRasterGrid* outputMap)
{
    int lastIndex = getSoilLayerIndex(myProject, 0.04);
    long surfaceIndex, nodeIndex;
    double waterContent, sumWater, wiltingPoint, minWater, saturation, maxWater;
    double soilSurfaceMoisture;     //[0-100]

    for (int row = 0; row < myProject->indexMap.header->nrRows; row++)
    {
        for (int col = 0; col < myProject->indexMap.header->nrCols; col++)
        {
            outputMap->value[row][col] = outputMap->header->flag;
            surfaceIndex = long(myProject->indexMap.value[row][col]);
            if (surfaceIndex != long(myProject->indexMap.header->flag))
            {
                sumWater = soilFluxes3D::getWaterContent(surfaceIndex);         //[m]
                minWater = 0.0;                                                 //[m]
                maxWater = 0.0;                                                 //[m]
                for (int layer = 1; layer <= lastIndex; layer++)
                {
                    nodeIndex = layer * myProject->nrVoxelsPerLayer + surfaceIndex;
                    waterContent = soilFluxes3D::getWaterContent(nodeIndex);                    //[m^3 m^-3]
                    sumWater += waterContent * myProject->layerThickness[layer];                //[m]
                    wiltingPoint = myProject->getSoilVar(0, layer, soil::soilWaterContentWP);   //[m^3 m^-3]
                    minWater += wiltingPoint * myProject->layerThickness[layer];                //[m]
                    saturation = myProject->getSoilVar(0, layer, soil::soilWaterContentSat);    //[m^3 m^-3]
                    maxWater += saturation * myProject->layerThickness[layer];                  //[m]
                }
                soilSurfaceMoisture = 100 * ((sumWater-minWater) / (maxWater-minWater));
                soilSurfaceMoisture = minValue(maxValue(soilSurfaceMoisture, 0), 100);
                outputMap->value[row][col] = float(soilSurfaceMoisture);
            }
         }
    }
    return true;
}

// return map of available water content in the root zone [mm]
bool getRootZoneAWCmap(Crit3DProject* myProject, gis::Crit3DRasterGrid* outputMap)
{
    long surfaceIndex, nodeIndex;
    double awc, thickness, sumAWC, skeleton;
    int soilIndex, horizonIndex;

    for (int row = 0; row < myProject->indexMap.header->nrRows; row++)
    {
        for (int col = 0; col < myProject->indexMap.header->nrCols; col++)
        {
            //initialize
            outputMap->value[row][col] = outputMap->header->flag;

            surfaceIndex = long(myProject->indexMap.value[row][col]);
            if (surfaceIndex != long(myProject->indexMap.header->flag))
            {
                sumAWC = 0.0;
                soilIndex = int(myProject->soilIndexMap.value[row][col]);
                if (soilIndex != int(myProject->soilIndexMap.header->flag))
                {
                    for (int layer = 1; layer < myProject->nrLayers; layer++)
                    {
                        // if getRootDensity(layer) > 0
                        {
                            nodeIndex = myProject->nrVoxelsPerLayer * layer + surfaceIndex;
                            awc = soilFluxes3D::getAvailableWaterContent(nodeIndex);  //[m3 m-3]
                            if (awc != NODATA)
                            {
                                thickness = myProject->layerThickness[layer] * 1000.0;  //[mm]
                                horizonIndex = soil::getHorizonIndex(&(myProject->soilList[soilIndex]),
                                                                     myProject->layerDepth[layer]);
                                skeleton = myProject->soilList[soilIndex].horizon[horizonIndex].coarseFragments;

                                sumAWC += (awc * thickness * (1.0 - skeleton));         //[mm]
                            }
                        }
                    }
                    outputMap->value[row][col] = float(sumAWC);
                }
            }
        }
    }

    return true;
}


bool getCriteria3DIntegrationMap(Crit3DProject* myProject, criteria3DVariable myVar,
                       double upperDepth, double lowerDepth, gis::Crit3DRasterGrid* criteria3DMap)
{
    if (upperDepth > myProject->soilDepth) return false;
    lowerDepth = minValue(lowerDepth, myProject->soilDepth);

    if (upperDepth == lowerDepth)
    {
        int layerIndex = getSoilLayerIndex(myProject, upperDepth);
        if (layerIndex == INDEX_ERROR) return false;
        return getCriteria3DVarMap(myProject, myVar, layerIndex, criteria3DMap);
    }

    int firstIndex, lastIndex;
    double firstThickness, lastThickness;
    firstIndex = getSoilLayerIndex(myProject, upperDepth);
    lastIndex = getSoilLayerIndex(myProject, lowerDepth);
    if ((firstIndex == INDEX_ERROR)||(lastIndex == INDEX_ERROR))
        return false;
    firstThickness = getSoilLayerBottom(myProject, firstIndex) - upperDepth;
    lastThickness = lowerDepth - getSoilLayerTop(myProject, lastIndex);

    long surfaceIndex, nodeIndex;
    double myValue, sumValues, skeleton;
    double thickCoeff, sumCoeff;
    int soilIndex, horizonIndex;

    for (int row = 0; row < myProject->indexMap.header->nrRows; row++)
    {
        for (int col = 0; col < myProject->indexMap.header->nrCols; col++)
        {
            surfaceIndex = long(myProject->indexMap.value[row][col]);
            criteria3DMap->value[row][col] = criteria3DMap->header->flag;
            sumValues = 0.0;
            sumCoeff = 0.0;

            if (surfaceIndex != myProject->indexMap.header->flag)
            {
                soilIndex = int(myProject->soilIndexMap.value[row][col]);
                if (soilIndex != int(myProject->soilIndexMap.header->flag))
                {
                    for (int i = firstIndex; i <= lastIndex; i++)
                    {
                        nodeIndex = i * myProject->nrVoxelsPerLayer + surfaceIndex;
                        myValue = getCriteria3DVar(myVar, nodeIndex);
                        if (myValue != NODATA)
                        {
                            horizonIndex = soil::getHorizonIndex(&(myProject->soilList[soilIndex]), myProject->layerDepth[i]);
                            skeleton = myProject->soilList[soilIndex].horizon[horizonIndex].coarseFragments;
                            if (i == firstIndex)
                                thickCoeff = firstThickness * (1.0 - skeleton);
                            else if (i == lastIndex)
                                thickCoeff = lastThickness * (1.0 - skeleton);
                            else
                                thickCoeff = myProject->layerThickness[i] * (1.0 - skeleton);

                            sumValues += (myValue * thickCoeff);
                            sumCoeff += thickCoeff;
                        }
                    }
                    criteria3DMap->value[row][col] = float(sumValues / sumCoeff);
                }
            }
        }
    }

    return true;
}


bool saveWaterBalanceOutput(Crit3DProject* myProject, Crit3DDate myDate, criteria3DVariable myVar,
                            std::string varName, std::string outputPath,
                            double upperDepth, double lowerDepth)
{
    gis::Crit3DRasterGrid* myMap = new gis::Crit3DRasterGrid();
    myMap->initializeGrid(myProject->indexMap);

    if (myVar == soilSurfaceMoisture)
    {
        if (! getSoilSurfaceMoisture(myProject, myMap))
            return false;
    }
    else if(myVar == availableWaterContent)
    {
        if (! getRootZoneAWCmap(myProject, myMap))
            return false;
    }
    else
    {
        if (! getCriteria3DIntegrationMap(myProject, myVar, upperDepth, lowerDepth, myMap))
            return false;
    }


    std::string filename = outputPath + varName + "_" + myDate.toStdString();
    std::string errorString;
    if (! gis::writeEsriGrid(filename, myMap, &errorString))
    {
         myProject->setError(errorString);
         return false;
    }

    myMap->freeGrid();

    return true;
}


std::string getPrefixFromVar(Crit3DDate myDate, criteria3DVariable myVar)
{
    std::string fileName = myDate.toStdString();

    if (myVar == waterContent)
        fileName += "_WaterContent_";
    if (myVar == availableWaterContent)
        fileName += "_availableWaterContent_";
    else if(myVar == waterMatricPotential)
        fileName += "_MP_";
    else if(myVar == degreeOfSaturation)
        fileName += "_degreeOfSaturation_";
    else
        return "";

    return fileName;
}

bool loadWaterBalanceState(Crit3DProject* myProject, Crit3DDate myDate, std::string statePath, criteria3DVariable myVar)
{
    std::string myErrorString;
    std::string myMapName;

    gis::Crit3DRasterGrid myMap;

    std::string myPrefix = getPrefixFromVar(myDate, myVar);

    for (int layerIndex = 0; layerIndex < myProject->nrLayers; layerIndex++)
    {
        myMapName = statePath + myPrefix + std::to_string(layerIndex);
        if (! gis::readEsriGrid(myMapName, &myMap, &myErrorString))
        {
            myProject->setError(myErrorString);
            return false;
        }
        else
            if (!setCriteria3DVarMap(layerIndex, myProject, myVar, &myMap))
                return false;
    }
    myMap.freeGrid();
    return true;
}


//[m] upper depth of soil layer
double getSoilLayerTop(Crit3DProject* myProject, int i)
{
    return myProject->layerDepth[i] - myProject->layerThickness[i] / 2.0;
}

//lower depth of soil layer [m]
double getSoilLayerBottom(Crit3DProject* myProject, int i)
{
    return myProject->layerDepth[i] + myProject->layerThickness[i] / 2.0;
}


// index of soil layer for the current depth
// depth [m]
int getSoilLayerIndex(Crit3DProject* myProject, double depth)
{
    int i= 0;
    while (depth > getSoilLayerBottom(myProject, i))
        if (++i == myProject->nrLayers)
        {
            myProject->setError("getSoilLayerIndex: wrong soil depth.");
            return INDEX_ERROR;
        }
    return i;
}


bool saveWaterBalanceState(Crit3DProject* myProject, Crit3DDate myDate, std::string statePath, criteria3DVariable myVar)
{
    std::string myErrorString;
    gis::Crit3DRasterGrid* myMap;
    myMap = new gis::Crit3DRasterGrid();
    myMap->initializeGrid(myProject->indexMap);

    std::string myPrefix = getPrefixFromVar(myDate, myVar);

    for (int layerIndex = 0; layerIndex < myProject->nrLayers; layerIndex++)
        if (getCriteria3DVarMap(myProject, myVar, layerIndex, myMap))
        {
            std::string myOutputMapName = statePath + myPrefix + std::to_string(layerIndex);
            if (! gis::writeEsriGrid(myOutputMapName, myMap, &myErrorString))
            {
                myProject->setError(myErrorString);
                return false;
            }
        }
    myMap->freeGrid();

    return true;
}


bool waterBalance(Crit3DProject* myProject)
{
    double totalPrecipitation = 0.0, totalEvaporation = 0.0, totalTranspiration = 0.0;
    double previousWaterContent = 0.0, currentWaterContent = 0.0;

    previousWaterContent = soilFluxes3D::getTotalWaterContent();
    myProject->setLog("total water [m^3]: " + std::to_string(previousWaterContent));

    if (! setWaterSinkSource(myProject, &totalPrecipitation,
                             &totalEvaporation, &totalTranspiration)) return(false);

    myProject->setLog("precipitation [m^3]: " + std::to_string(totalPrecipitation));
    myProject->setLog("evaporation [m^3]: " + std::to_string(-totalEvaporation));
    myProject->setLog("transpiration [m^3]: " + std::to_string(-totalTranspiration));

    myProject->setLog("Compute water flow");
    soilFluxes3D::initializeBalance();
    soilFluxes3D::computePeriod(3600.0);

    currentWaterContent = soilFluxes3D::getTotalWaterContent();
    double runoff = soilFluxes3D::getBoundaryWaterSumFlow(BOUNDARY_RUNOFF);
    myProject->setLog("runoff [m^3]: " + std::to_string(runoff));
    double freeDrainage = soilFluxes3D::getBoundaryWaterSumFlow(BOUNDARY_FREEDRAINAGE);
    myProject->setLog("free drainage [m^3]: " + std::to_string(freeDrainage));
    double lateralDrainage = soilFluxes3D::getBoundaryWaterSumFlow(BOUNDARY_FREELATERALDRAINAGE);
    myProject->setLog("lateral drainage [m^3]: " + std::to_string(lateralDrainage));

    double forecastWaterContent = previousWaterContent + runoff + freeDrainage + lateralDrainage
                        + totalPrecipitation - totalEvaporation - totalTranspiration;
    double massBalanceError = currentWaterContent - forecastWaterContent;
    myProject->setLog("Mass balance error [m^3]: " + std::to_string(massBalanceError));

    return(true);
}


bool initializeWaterBalance(Crit3DProject* myProject)
{
    myProject->setLog("\nInitialize Waterbalance...");

    std::string myError;

    double minThickness = 0.02;      //[m]
    double maxThickness = 0.1;       //[m]
    double thickFactor = 1.5;
    myProject->nrLayers = computeNrLayers(myProject->soilDepth, minThickness, maxThickness, thickFactor);
    setLayersDepth(myProject, minThickness, maxThickness, thickFactor);
    myProject->setLog("nr of layers: " + std::to_string(myProject->nrLayers));

    if (myProject->createIndexMap())
        myProject->setLog("nr of surface cells: " + std::to_string(myProject->nrVoxelsPerLayer));
    else
    {
        myProject->setError("initializeWaterBalance: missing data in DTM");
        return(false);
    }
    myProject->nrVoxels = myProject->nrVoxelsPerLayer * myProject->nrLayers;
    waterSinkSource.resize(myProject->nrVoxels);
    myProject->createBoundaryMap();

    int nrLateralLink = 8;
    int myResult = soilFluxes3D::initialize(myProject->nrVoxels, myProject->nrLayers,
                                            nrLateralLink, true, false, false);
    if (isCrit3dError(myResult, &myError))
    {
        myProject->error = "initializeCriteria3D:" + myError;
        return(false);
    }

    if (! setCrit3DSurfaces(myProject)) return(false);
    if (! setCrit3DSoils(myProject)) return(false);
    if (! setCrit3DTopography(myProject)) return(false);
    if (! setCrit3DNodeSoil(myProject)) return(false);


    //criteria3D::setNumericalParameters(6.0, 600.0, 200, 10, 12, 3);   // precision
    soilFluxes3D::setNumericalParameters(30.0, 1800.0, 100, 10, 12, 2);  // speedy
    //criteria3D::setNumericalParameters(300.0, 3600.0, 100, 10, 12, 1);   // very speedy (high error)
    soilFluxes3D::setHydraulicProperties(MODIFIEDVANGENUCHTEN, MEAN_LOGARITHMIC, 10.0);

    myProject->setLog("Waterbalance initialized");
    return(true);
}

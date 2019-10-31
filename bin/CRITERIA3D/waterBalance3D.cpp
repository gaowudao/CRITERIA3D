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
#include "basicMath.h"
#include "waterBalance3D.h"
#include "soilFluxes3D.h"
#include "crit3dProject.h"
#include "crit3dDate.h"


double getMaxEvaporation(double ET0, double LAI)
{
    const double ke = 0.6;   //[-] light extinction factor
    const double maxEvaporationRatio = 0.66;

    double Kc = exp(-ke * LAI);
    return(ET0 * Kc * maxEvaporationRatio);
}

/*
double evaporation(Crit3DProject* myProject, int row, int col, long surfaceIndex)
{
    double depthCoeff, thickCoeff, layerCoeff;
    double realEvap, residualEvap, layerEvap, availableWater, E0, ET0, flow;
    double laiGrass, laiVine, laiTot;
    long nodeIndex;

    double const MAX_PROF_EVAPORATION = 0.15;           //[m]
    int lastEvapLayer = getSoilLayerIndex(myProject, MAX_PROF_EVAPORATION);
    double area = myProject->DEM.header->cellSize * myProject->DEM.header->cellSize;
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
        nodeIndex = layer * myProject->nrNodesPerLayer + surfaceIndex;

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
        layerEvap = MINVALUE(E0 * layerCoeff, residualEvap);
        layerEvap = MINVALUE(layerEvap, availableWater);

        if (layerEvap > 0.0)
        {
            realEvap += layerEvap;
            flow = area * (layerEvap / 1000.0);                  //[m^3/h]
            myProject->waterSinkSource[nodeIndex] -= (flow / 3600.0);        //[m^3/s]
        }
    }
    return 0;
}
*/

bool setWaterSinkSource(Crit3DProject* myProject, double* totalPrecipitation,
                        double* totalEvaporation, double *totalTranspiration)
{
    long surfaceIndex;
    unsigned int layerIndex;
    float prec, totalWater;
    double transp, flow, realEvap;
    int myResult;
    QString myError;

    // initialize
    for (unsigned long i = 0; i < myProject->nrNodes; i++)
        myProject->waterSinkSource[i] = 0.0;

    double area = myProject->DEM.header->cellSize * myProject->DEM.header->cellSize;

    // precipitation or irrigation
    *totalPrecipitation = 0.0;
    for (long row = 0; row < myProject->indexMap[0].header->nrRows; row++)
        for (long col = 0; col < myProject->indexMap[0].header->nrCols; col++)
        {
            surfaceIndex = long(myProject->indexMap[0].value[row][col]);
            if (surfaceIndex != long(myProject->indexMap[0].header->flag))
            {
                totalWater = 0.0;
                prec = myProject->hourlyMeteoMaps->mapHourlyPrec->value[row][col];
                if (! isEqual(prec, myProject->hourlyMeteoMaps->mapHourlyPrec->header->flag)) totalWater += prec;

                //float irr = myProject->meteoMaps->irrigationMap->value[row][col];
                //if (irr != myProject->meteoMaps->irrigationMap->header->flag) totalWater += irr;

                if (totalWater > 0.f)
                {
                    flow = area * (totalWater / 1000.0);                //[m^3/h]
                    *totalPrecipitation += flow;
                    myProject->waterSinkSource[surfaceIndex] += flow / 3600.0;     //[m^3/s]
                }
            }
        }

    // evaporation
    *totalEvaporation = 0.0;
    for (long row = 0; row < myProject->indexMap[0].header->nrRows; row++)
        for (long col = 0; col < myProject->indexMap[0].header->nrCols; col++)
        {
            surfaceIndex = long(myProject->indexMap[0].value[row][col]);
            if (surfaceIndex != long(myProject->indexMap[0].header->flag))
            {
                realEvap = 0.; //evaporation(myProject, row, col, surfaceIndex);

                flow = area * (realEvap / 1000.0);                  //[m^3/h]
                *totalEvaporation += flow;
            }
        }

    // crop transpiration
    *totalTranspiration = 0.0;
    for (long row = 0; row < myProject->indexMap[0].header->nrRows; row++)
        for (long col = 0; col < myProject->indexMap[0].header->nrCols; col++)
        {    
            surfaceIndex = long(myProject->indexMap[0].value[row][col]);
            if (surfaceIndex != long(myProject->indexMap[0].header->flag))
            {
                for (layerIndex=1; layerIndex < myProject->nrLayers; layerIndex++)
                {
                    transp = 0.; //myProject->outputPlantMaps->transpirationLayerMaps[layerIndex]->value[row][col];

                    /*if (transp != myProject->outputPlantMaps->transpirationLayerMaps[layerIndex]->header->flag)
                    {
                        flow = area * (transp / 1000.0);                    //[m^3/h]
                        *totalTranspiration += flow;
                        nodeIndex = layerIndex * myProject->nrNodesPerLayer + surfaceIndex;
                        waterSinkSource[nodeIndex] -= flow / 3600.0;        //[m^3/s]
                    }*/
                }

            }
        }

    for (unsigned long i = 0; i < myProject->nrNodes; i++)
    {
        myResult = soilFluxes3D::setWaterSinkSource(signed(i), myProject->waterSinkSource[i]);
        if (isCrit3dError(myResult, &myError))
        {
            myProject->errorString = "initializeSoilMoisture:" + myError;
            return(false);
        }
    }

    return(true);
}


/*
double* getCriteria3DVarProfile(Crit3DProject* myProject, int myRow, int myCol, criteria3DVariable myVar)
{
    double* myProfile = (double *) calloc(myProject->nrLayers, sizeof(double));
    long firstLayerIndex = long(myProject->indexMap[0].value[myRow][myCol]);
    long nodeIndex, layerIndex;

    for (layerIndex = 0; layerIndex < myProject->nrLayers; layerIndex++)
        myProfile[layerIndex] = NODATA;

    if (firstLayerIndex != myProject->indexMap[0].header->flag)
        for (layerIndex = 0; layerIndex < myProject->nrLayers; layerIndex++)
        {
            nodeIndex = layerIndex * myProject->nrNodesPerLayer + firstLayerIndex;
            myProfile[layerIndex] = getCriteria3DVar(myVar, nodeIndex);
        }

    return myProfile;
}
*/

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

/*
bool setCriteria3DVarMap(int myLayerIndex, Crit3DProject* myProject, criteria3DVariable myVar,
                        gis::Crit3DRasterGrid* myCriteria3DMap)
{
    long surfaceIndex, nodeIndex;

    for (int row = 0; row < myProject->indexMap[0].header->nrRows; row++)
        for (int col = 0; col < myProject->indexMap[0].header->nrCols; col++)
        {
            surfaceIndex = long(myProject->indexMap[0].value[row][col]);
            if (surfaceIndex != long(myProject->indexMap[0].header->flag))
            {
                nodeIndex = myLayerIndex * myProject->nrNodesPerLayer + surfaceIndex;
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

    criteria3DMap->initializeGrid(myProject->indexMap[0]);

    for (int row = 0; row < myProject->indexMap[0].header->nrRows; row++)
        for (int col = 0; col < myProject->indexMap[0].header->nrCols; col++)
        {
            surfaceIndex = long(myProject->indexMap[0].value[row][col]);
            if (surfaceIndex != long(myProject->indexMap[0].header->flag))
            {
                nodeIndex = layerIndex * myProject->nrNodesPerLayer + surfaceIndex;

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

    for (int row = 0; row < myProject->indexMap[0].header->nrRows; row++)
    {
        for (int col = 0; col < myProject->indexMap[0].header->nrCols; col++)
        {
            outputMap->value[row][col] = outputMap->header->flag;
            surfaceIndex = long(myProject->indexMap[0].value[row][col]);
            if (surfaceIndex != long(myProject->indexMap[0].header->flag))
            {
                sumWater = soilFluxes3D::getWaterContent(surfaceIndex);         //[m]
                minWater = 0.0;                                                 //[m]
                maxWater = 0.0;                                                 //[m]
                for (int layer = 1; layer <= lastIndex; layer++)
                {
                    nodeIndex = layer * myProject->nrNodesPerLayer + surfaceIndex;
                    waterContent = soilFluxes3D::getWaterContent(nodeIndex);                    //[m^3 m^-3]
                    sumWater += waterContent * myProject->layerThickness[layer];                //[m]
                    wiltingPoint = myProject->getSoilVar(0, layer, soil::soilWaterContentWP);   //[m^3 m^-3]
                    minWater += wiltingPoint * myProject->layerThickness[layer];                //[m]
                    saturation = myProject->getSoilVar(0, layer, soil::soilWaterContentSat);    //[m^3 m^-3]
                    maxWater += saturation * myProject->layerThickness[layer];                  //[m]
                }
                soilSurfaceMoisture = 100 * ((sumWater-minWater) / (maxWater-minWater));
                soilSurfaceMoisture = MINVALUE(MAXVALUE(soilSurfaceMoisture, 0), 100);
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

    for (int row = 0; row < myProject->indexMap[0].header->nrRows; row++)
    {
        for (int col = 0; col < myProject->indexMap[0].header->nrCols; col++)
        {
            //initialize
            outputMap->value[row][col] = outputMap->header->flag;

            surfaceIndex = long(myProject->indexMap[0].value[row][col]);
            if (surfaceIndex != long(myProject->indexMap[0].header->flag))
            {
                sumAWC = 0.0;
                soilIndex = int(myProject->soilIndexMap.value[row][col]);
                if (soilIndex != int(myProject->soilIndexMap.header->flag))
                {
                    for (int layer = 1; layer < myProject->nrLayers; layer++)
                    {
                        // if getRootDensity(layer) > 0
                        {
                            nodeIndex = myProject->nrNodesPerLayer * layer + surfaceIndex;
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
    lowerDepth = MINVALUE(lowerDepth, myProject->soilDepth);

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

    for (int row = 0; row < myProject->indexMap[0].header->nrRows; row++)
    {
        for (int col = 0; col < myProject->indexMap[0].header->nrCols; col++)
        {
            surfaceIndex = long(myProject->indexMap[0].value[row][col]);
            criteria3DMap->value[row][col] = criteria3DMap->header->flag;
            sumValues = 0.0;
            sumCoeff = 0.0;

            if (surfaceIndex != myProject->indexMap[0].header->flag)
            {
                soilIndex = int(myProject->soilIndexMap.value[row][col]);
                if (soilIndex != int(myProject->soilIndexMap.header->flag))
                {
                    for (int i = firstIndex; i <= lastIndex; i++)
                    {
                        nodeIndex = i * myProject->nrNodesPerLayer + surfaceIndex;
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
    myMap->initializeGrid(myProject->indexMap[0]);

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
         myProject->logError(QString::fromStdString(errorString));
         return false;
    }

    myMap->clear();

    return true;
}
*/

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

/*
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
            myProject->logError(QString::fromStdString(myErrorString));
            return false;
        }
        else
            if (!setCriteria3DVarMap(layerIndex, myProject, myVar, &myMap))
                return false;
    }
    myMap.clear();
    return true;
}
*/

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
    {
        if (i == signed(myProject->nrLayers-1))
        {
            myProject->logError("getSoilLayerIndex: wrong soil depth.");
            return INDEX_ERROR;
        }
        i++;
    }
    return i;
}


/*
bool saveWaterBalanceState(Crit3DProject* myProject, Crit3DDate myDate, std::string statePath, criteria3DVariable myVar)
{
    std::string myErrorString;
    gis::Crit3DRasterGrid* myMap;
    myMap = new gis::Crit3DRasterGrid();
    myMap->initializeGrid(myProject->indexMap[0]);

    std::string myPrefix = getPrefixFromVar(myDate, myVar);

    for (int layerIndex = 0; layerIndex < myProject->nrLayers; layerIndex++)
        if (getCriteria3DVarMap(myProject, myVar, layerIndex, myMap))
        {
            std::string myOutputMapName = statePath + myPrefix + std::to_string(layerIndex);
            if (! gis::writeEsriGrid(myOutputMapName, myMap, &myErrorString))
            {
                myProject->logError(QString::fromStdString(myErrorString));
                return false;
            }
        }
    myMap->clear();

    return true;
}
*/


bool waterBalance(Crit3DProject* myProject)
{
    double totalPrecipitation = 0.0, totalEvaporation = 0.0, totalTranspiration = 0.0;
    double previousWaterContent = 0.0, currentWaterContent = 0.0;

    previousWaterContent = soilFluxes3D::getTotalWaterContent();
    myProject->logInfo("total water [m^3]: " + QString::number(previousWaterContent));

    if (! setWaterSinkSource(myProject, &totalPrecipitation,
                             &totalEvaporation, &totalTranspiration)) return(false);

    myProject->logInfo("precipitation [m^3]: " + QString::number(totalPrecipitation));
    myProject->logInfo("evaporation [m^3]: " + QString::number(-totalEvaporation));
    myProject->logInfo("transpiration [m^3]: " + QString::number(-totalTranspiration));

    myProject->logInfo("Compute water flow");
    soilFluxes3D::initializeBalance();
    soilFluxes3D::computePeriod(3600.0);

    currentWaterContent = soilFluxes3D::getTotalWaterContent();
    double runoff = soilFluxes3D::getBoundaryWaterSumFlow(BOUNDARY_RUNOFF);
    myProject->logInfo("runoff [m^3]: " + QString::number(runoff));
    double freeDrainage = soilFluxes3D::getBoundaryWaterSumFlow(BOUNDARY_FREEDRAINAGE);
    myProject->logInfo("free drainage [m^3]: " + QString::number(freeDrainage));
    double lateralDrainage = soilFluxes3D::getBoundaryWaterSumFlow(BOUNDARY_FREELATERALDRAINAGE);
    myProject->logInfo("lateral drainage [m^3]: " + QString::number(lateralDrainage));

    double forecastWaterContent = previousWaterContent + runoff + freeDrainage + lateralDrainage
                        + totalPrecipitation - totalEvaporation - totalTranspiration;
    double massBalanceError = currentWaterContent - forecastWaterContent;
    myProject->logInfo("Mass balance error [m^3]: " + QString::number(massBalanceError));

    return(true);
}


bool initializeWaterBalance3D(Crit3DProject* myProject)
{
    myProject->logInfo("\nInitialize Waterbalance...");

    QString myError;

    // Layers depth
    myProject->computeNrLayers();
    myProject->setLayersDepth();
    myProject->logInfo("nr of layers: " + QString::number(myProject->nrLayers));

    // Index map
    if (! myProject->setIndexMaps()) return false;
    myProject->logInfo("nr of nodes: " + QString::number(myProject->nrNodes));

    myProject->waterSinkSource.resize(unsigned(myProject->nrNodes));

    // Boundary
    if (!myProject->setBoundary()) return false;
    myProject->logInfo("Boundary computed");

    // Initiale soil fluxes
    int myResult = soilFluxes3D::initialize(myProject->nrNodes, myProject->nrLayers,
                                            myProject->nrLateralLink, true, false, false);
    if (isCrit3dError(myResult, &myError))
    {
        myProject->logError("initializeWaterBalance:" + myError);
        return false;
    }
    myProject->logInfo("Memory initialized");

    // Set properties for all voxels
    if (! myProject->setCrit3DSurfaces()) return false;

    if (! myProject->setCrit3DSoils()) return false;
    myProject->logInfo("Soils initialized");

    if (! myProject->setCrit3DTopography()) return false;
    myProject->logInfo("Topology initialized");

    if (! myProject->setCrit3DNodeSoil()) return false;
    myProject->logInfo("Soils initialized");

    //criteria3D::setNumericalParameters(6.0, 600.0, 200, 10, 12, 3);   // precision
    soilFluxes3D::setNumericalParameters(30.0, 1800.0, 100, 10, 12, 2);  // speedy
    //criteria3D::setNumericalParameters(300.0, 3600.0, 100, 10, 12, 1);   // very speedy (high error)
    soilFluxes3D::setHydraulicProperties(MODIFIEDVANGENUCHTEN, MEAN_LOGARITHMIC, 10.0);

    myProject->logInfo("3D water balance initialized");
    return true;
}

#include <math.h>
#include "waterBalance.h"
#include "dataHandler.h"
#include "vine3DProject.h"
#include "soilFluxes3D.h"
#include "commonConstants.h"

#include <vector>

std::vector <double> waterSinkSource;     //[m^3/sec]


void cleanWaterBalanceMemory()
{
    soilFluxes3D::cleanMemory();
    waterSinkSource.resize(0);
}

bool isCrit3dError(int myResult, QString* myError)
{
    if (myResult == CRIT3D_OK) return(false);

    if (myResult == INDEX_ERROR)
        *myError = " Index error";
    else if (myResult == MEMORY_ERROR)
        *myError = "Memory error";
    else if (myResult == TOPOGRAPHY_ERROR)
        *myError = "Topography error";
    else if (myResult == BOUNDARY_ERROR)
        *myError = "Boundary error";
    else if (myResult == PARAMETER_ERROR)
        *myError = "Parameter error";

    return (true);
}


bool setCrit3DSoils(Vine3DProject* myProject)
{
    soil::Crit3DHorizon* myHorizon;
    QString myError;
    int myResult;

    for (int soilIndex = 0; soilIndex < myProject->nrSoils; soilIndex++)
    {
        for (int horizIndex = 0; horizIndex < myProject->soilList[soilIndex].nrHorizons; horizIndex++)
        {
            myHorizon = &(myProject->soilList[soilIndex].horizon[horizIndex]);
            if ((myHorizon->texture.classUSDA > 0) && (myHorizon->texture.classUSDA <= 12))
            {
                myResult = soilFluxes3D::setSoilProperties(soilIndex, horizIndex,
                     myHorizon->vanGenuchten.alpha * GRAVITY,   // kPa-1 --> m-1
                     myHorizon->vanGenuchten.n,
                     myHorizon->vanGenuchten.m,
                     myHorizon->vanGenuchten.he / GRAVITY,  // kPa --> m
                     myHorizon->vanGenuchten.thetaR,
                     myHorizon->vanGenuchten.thetaS,
                     myHorizon->waterConductivity.kSat / DAY_SECONDS / 100, // cm d-1 --> m s-1
                     myHorizon->waterConductivity.l,
                     myHorizon->organicMatter,
                     myHorizon->texture.clay);

                 if (isCrit3dError(myResult, &myError))
                 {
                     myProject->projectError = "setCrit3DSoils:" + myError
                         + " in soil nr:" + QString::number(myProject->soilList[soilIndex].id)
                         + " horizon nr: " + QString::number(horizIndex);
                     return(false);
                 }
            }
        }
    }
    return(true);
}


bool setCrit3DSurfaces(Vine3DProject* myProject)
{
    QString myError;
    int myResult;

    int nrSurfaces = 1;
    float ManningRoughness = (float)0.24;         //[s m^-1/3]
    float surfacePond = (float)0.002;             //[m]

    for (int surfaceIndex = 0; surfaceIndex < nrSurfaces; surfaceIndex++)
    {
        myResult = soilFluxes3D::setSurfaceProperties(surfaceIndex, ManningRoughness, surfacePond);
        if (isCrit3dError(myResult, &myError))
        {
            myProject->projectError = "setCrit3DSurfaces:" + myError
                + " in surface nr:" + QString::number(surfaceIndex);
            return(false);
        }
    }
    return(true);
}


int computeNrLayers(float totalDepth, float minThickness, float maxThickness, float factor)
 {
    int nrLayers = 1;
    float nextThickness, prevThickness = minThickness;
    float depth = minThickness * 0.5;
    while (depth < totalDepth)
    {
        nextThickness = minValue(maxThickness, prevThickness * factor);
        depth = depth + (prevThickness + nextThickness) * 0.5;
        prevThickness = nextThickness;
        nrLayers++;
    }
    return(nrLayers);
}


// set thickness and depth (center) of layers [m]
bool setLayersDepth(Vine3DProject* myProject, double minThickness, double maxThickness, double factor)
{
    int lastLayer = myProject->nrSoilLayers-1;
    myProject->layerDepth.resize(myProject->nrSoilLayers);
    myProject->layerThickness.resize(myProject->nrSoilLayers);

    myProject->layerDepth[0] = 0.0;
    myProject->layerThickness[0] = 0.0;
    myProject->layerThickness[1] = minThickness;
    myProject->layerDepth[1] = minThickness * 0.5;
    for (int i = 2; i < myProject->nrSoilLayers; i++)
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


bool setindexMap(Vine3DProject* myProject)
{
    long index = 0;
    myProject->indexMap.initializeGrid(myProject->DTM);
    for (int row = 0; row < myProject->indexMap.header->nrRows; row++)
        for (int col = 0; col < myProject->indexMap.header->nrCols; col++)
            if (myProject->DTM.value[row][col] != myProject->DTM.header->flag)
            {
                myProject->indexMap.value[row][col] = index;
                index++;
            }

    myProject->nrLayerNodes = index;
    return(myProject->nrLayerNodes > 0);
}


bool setBoundary(Vine3DProject* myProject)
{
    myProject->boundaryMap.initializeGrid(myProject->DTM);
    for (int row = 0; row < myProject->boundaryMap.header->nrRows; row++)
        for (int col = 0; col < myProject->boundaryMap.header->nrCols; col++)
            if (gis::isBoundary(myProject->DTM, row, col))
                if (! gis::isStrictMaximum(myProject->DTM, row, col))
                    myProject->boundaryMap.value[row][col] = BOUNDARY_RUNOFF;
    return true;
}


bool setCrit3DTopography(Vine3DProject* myProject)
{
    double x, y;
    float z, area, lateralArea, volume, slope;
    long index, surfaceIndex, linkIndex;
    int myResult;
    QString myError;

    for (int row = 0; row < myProject->indexMap.header->nrRows; row++)
        for (int col = 0; col < myProject->indexMap.header->nrCols; col++)
        {
            surfaceIndex = myProject->indexMap.value[row][col];
            if (surfaceIndex != myProject->indexMap.header->flag)
            {
                gis::getUtmXYFromRowCol(myProject->DTM, row, col, &x, &y);
                area = myProject->DTM.header->cellSize * myProject->DTM.header->cellSize;
                slope = myProject->meteoMaps->radiationMaps->slopeMap->value[row][col] / 100.0;

                for (int layer = 0; layer < myProject->nrSoilLayers; layer++)
                {
                    index = layer * myProject->nrLayerNodes + surfaceIndex;
                    z = myProject->DTM.value[row][col] - myProject->layerDepth[layer];
                    volume = area * myProject->layerThickness[layer];

                    //surface
                    if (layer == 0)
                    {
                        lateralArea = myProject->DTM.header->cellSize;
                        if (myProject->boundaryMap.value[row][col] == BOUNDARY_RUNOFF)
                        {
                            myResult = soilFluxes3D::setNode(index, x, y, z, area,
                                             true, true, BOUNDARY_RUNOFF, slope);
                        }
                        else
                        {
                            myResult = soilFluxes3D::setNode(index, x, y, z, area,
                                             true, false, BOUNDARY_NONE, 0.0);
                        }
                    }
                    //sub-surface
                    else
                    {
                        lateralArea = myProject->DTM.header->cellSize * myProject->layerThickness[layer];
                        //last layer
                        if (layer == (myProject->nrSoilLayers - 1))
                        {
                            myResult = soilFluxes3D::setNode(index, x, y, z, volume,
                                             false, true, BOUNDARY_FREEDRAINAGE, 0.0);
                        }
                        else
                        {
                            if (myProject->boundaryMap.value[row][col] == BOUNDARY_RUNOFF)
                            {
                                myResult = soilFluxes3D::setNode(index, x, y, z, volume,
                                                 false, true, BOUNDARY_FREELATERALDRAINAGE, slope);
                            }
                            else
                            {
                                myResult = soilFluxes3D::setNode(index, x, y, z, volume,
                                             false, false, BOUNDARY_NONE, 0.0);
                            }
                        }
                    }
                    //check error
                    if (isCrit3dError(myResult, &myError))
                    {
                        myProject->projectError = "setCrit3DTopography:" + myError
                                    + " in layer nr:" + QString::number(layer);
                        return(false);
                    }
                    //up link
                    if (layer > 0)
                    {
                        linkIndex = index - myProject->nrLayerNodes;
                        myResult = soilFluxes3D::setNodeLink(index, linkIndex, UP, area);
                        if (isCrit3dError(myResult, &myError))
                        {
                            myProject->projectError = "setNodeLink:" + myError
                                    + " in layer nr:" + QString::number(layer);
                            return(false);
                        }
                    }
                    //down link
                    if (layer < (myProject->nrSoilLayers - 1))
                    {
                        linkIndex = index + myProject->nrLayerNodes;
                        myResult = soilFluxes3D::setNodeLink(index, linkIndex, DOWN, area);
                        if (isCrit3dError(myResult, &myError))
                        {
                            myProject->projectError = "setNodeLink:" + myError
                                    + " in layer nr:" + QString::number(layer);
                            return(false);
                        }
                    }
                    //lateral links
                    for (int i=-1; i <= 1; i++)
                        for (int j=-1; j <= 1; j++)
                            if ((i != 0)||(j != 0))
                                if (! gis::isOutOfGridRowCol(row+i, col+j, myProject->indexMap))
                                {
                                    linkIndex = myProject->indexMap.value[row+i][col+j];
                                    if (linkIndex != myProject->indexMap.header->flag)
                                    {
                                        linkIndex += layer * myProject->nrLayerNodes;
                                        myResult = soilFluxes3D::setNodeLink(index, linkIndex, LATERAL, lateralArea / 2.0);
                                        if (isCrit3dError(myResult, &myError))
                                        {
                                            myProject->projectError = "setNodeLink:" + myError
                                                    + " in layer nr:" + QString::number(layer);
                                            return(false);
                                        }
                                    }
                                }
                }
            }
         }

   return (true);
}


bool setCrit3DNodeSoil(Vine3DProject* myProject)
{
    long index, surfaceIndex;
    long soilIndex, horizonIndex;
    int myResult;
    QString myError;

    for (int row = 0; row < myProject->indexMap.header->nrRows; row++)
        for (int col = 0; col < myProject->indexMap.header->nrCols; col++)
        {
            surfaceIndex = myProject->indexMap.value[row][col];
            if (surfaceIndex != myProject->indexMap.header->flag)
            {
                soilIndex = myProject->getSoilIndex(row, col);
                for (int layer = 0; layer < myProject->nrSoilLayers; layer++)
                {
                    index = layer * myProject->nrLayerNodes + surfaceIndex;
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
                            myProject->projectError = "function setCrit3DNodeSoil: \nno horizon definition in soil nr: "
                                    + QString::number(soilIndex) + " depth: " + QString::number(myProject->layerDepth[layer])
                                    +"\nCheck soil totalDepth in .xml file.";
                            return(false);
                        }

                        myResult = soilFluxes3D::setNodeSoil(index, soilIndex, horizonIndex);

                        //check error
                        if (isCrit3dError(myResult, &myError))
                        {
                            myProject->projectError = "setCrit3DNodeSoil:" + myError + " in soil nr: " + QString::number(soilIndex)
                                    + " horizon nr:" + QString::number(horizonIndex);
                            return(false);
                        }
                    }
                }
            }
        }
    return(true);
}


bool initializeSoilMoisture(Vine3DProject* myProject, int month)
{
    int myResult;
    QString myError;
    long index, surfaceIndex;
    long soilIndex, horizonIndex;
    double moistureIndex, waterPotential;
    double fieldCapacity;                    //[m]
    double  wiltingPoint = -160.0;           //[m]
    double dry = wiltingPoint / 3.0;         //[m] dry potential

    //[0-1] min: july  max: january
    moistureIndex = fabs(1.0 - (month - 1) / 6.0);
    moistureIndex = maxValue(moistureIndex, 0.001);
    moistureIndex = log(moistureIndex) / log(0.001);

    myProject->logInfo("Initialize soil moisture");

    for (int row = 0; row < myProject->indexMap.header->nrRows; row++)
        for (int col = 0; col < myProject->indexMap.header->nrCols; col++)
        {
            surfaceIndex = myProject->indexMap.value[row][col];
            if (surfaceIndex != myProject->indexMap.header->flag)
            {
                //surface
                soilFluxes3D::setWaterContent(surfaceIndex, 0.0);

                soilIndex = myProject->getSoilIndex(row, col);
                for (int layer = 1; layer < myProject->nrSoilLayers; layer++)
                {
                    index = layer * myProject->nrLayerNodes + surfaceIndex;
                    horizonIndex = soil::getHorizonIndex(&(myProject->soilList[soilIndex]), myProject->layerDepth[layer]);

                    fieldCapacity = myProject->soilList[soilIndex].horizon[horizonIndex].fieldCapacity;
                    waterPotential = fieldCapacity - moistureIndex * (fieldCapacity-dry);

                    myResult = soilFluxes3D::setMatricPotential(index, waterPotential);

                    if (isCrit3dError(myResult, &myError))
                    {
                        myProject->projectError = "initializeSoilMoisture:" + myError;
                        return(false);
                    }
                }
            }
        }
    return true;
}


double getMaxEvaporation(float ET0, float LAI)
{
    const double ke = 0.6;   //[-] light extinction factor
    const double maxEvaporationRatio = 0.66;

    double Kc = exp(-ke * LAI);
    return(maxEvaporationRatio * ET0 * Kc);
}


double evaporation(Vine3DProject* myProject, int row, int col, long surfaceIndex)
{
    double depthCoeff, thickCoeff, layerCoeff;
    double realEvap, residualEvap, layerEvap, availableWater, E0, ET0, flow;
    double laiGrass, laiVine, laiTot;
    long nodeIndex;

    double const MAX_PROF_EVAPORATION = 0.15;           //[m]
    int lastEvapLayer = getSoilLayerIndex(myProject, MAX_PROF_EVAPORATION);
    double area = myProject->DTM.header->cellSize * myProject->DTM.header->cellSize;
    int idField = myProject->getFieldIndex(row, col);

    //LAI
    laiGrass = myProject->vineFields[idField].maxLAIGrass;
    laiVine = myProject->statePlantMaps->leafAreaIndexMap->value[row][col];
    laiTot = laiVine + laiGrass;

    //E0 [mm]
    ET0 = myProject->meteoMaps->ET0Map->value[row][col];
    E0 = getMaxEvaporation(ET0, laiTot);
    realEvap = 0;

    for (int layer=0; layer <= lastEvapLayer; layer++)
    {
        nodeIndex = layer * myProject->nrLayerNodes + surfaceIndex;

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
    return realEvap;
}


bool setWaterSinkSource(Vine3DProject* myProject, double* totalPrecipitation,
                        double* totalEvaporation, double *totalTranspiration)
{
    long surfaceIndex, nodeIndex, layerIndex;
    double prec, irr, totalWater;
    double transp, flow, realEvap;
    int myResult;
    QString myError;

    //initialize
    for (long i = 0; i < myProject->nrNodes; i++)
        waterSinkSource[i] = 0.0;

    double area = myProject->DTM.header->cellSize * myProject->DTM.header->cellSize;

    //precipitation - irrigation
    *totalPrecipitation = 0.0;
    for (long row = 0; row < myProject->indexMap.header->nrRows; row++)
        for (long col = 0; col < myProject->indexMap.header->nrCols; col++)
        {
            surfaceIndex = myProject->indexMap.value[row][col];
            if (surfaceIndex != myProject->indexMap.header->flag)
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

    //Evaporation
    *totalEvaporation = 0.0;
    for (long row = 0; row < myProject->indexMap.header->nrRows; row++)
        for (long col = 0; col < myProject->indexMap.header->nrCols; col++)
        {
            surfaceIndex = myProject->indexMap.value[row][col];
            if (surfaceIndex != myProject->indexMap.header->flag)
            {
                realEvap = evaporation(myProject, row, col, surfaceIndex);
                myProject->meteoMaps->evaporationMap->value[row][col] = realEvap;

                flow = area * (realEvap / 1000.0);                  //[m^3/h]
                *totalEvaporation += flow;
            }
        }

    //crop transpiration
    *totalTranspiration = 0.0;
    for (long row = 0; row < myProject->indexMap.header->nrRows; row++)
        for (long col = 0; col < myProject->indexMap.header->nrCols; col++)
        {    
            surfaceIndex = myProject->indexMap.value[row][col];
            if (surfaceIndex != myProject->indexMap.header->flag)
            {
                for (layerIndex=1; layerIndex < myProject->nrSoilLayers; layerIndex++)
                {
                    transp = myProject->outputPlantMaps->transpirationLayerMaps[layerIndex]->value[row][col];

                    if (transp != myProject->outputPlantMaps->transpirationLayerMaps[layerIndex]->header->flag)
                    {
                        flow = area * (transp / 1000.0);                    //[m^3/h]
                        *totalTranspiration += flow;
                        nodeIndex = layerIndex * myProject->nrLayerNodes + surfaceIndex;
                        waterSinkSource[nodeIndex] -= flow / 3600.0;        //[m^3/s]
                    }
                }

            }
        }

    for (long i = 0; i < myProject->nrNodes; i++)
    {
        myResult = soilFluxes3D::setWaterSinkSource(i, waterSinkSource[i]);
        if (isCrit3dError(myResult, &myError))
        {
            myProject->projectError = "initializeSoilMoisture:" + myError;
            return(false);
        }
    }

    return(true);
}

double getSoilVar(Vine3DProject* myProject, int soilIndex, int myLayerIndex, soilVariable myVar)
{
    int horizonIndex = soil::getHorizonIndex(&(myProject->soilList[soilIndex]), myProject->layerDepth[myLayerIndex]);
    if (myVar == soilWiltingPointPotential)
        return myProject->soilList[soilIndex].horizon[horizonIndex].wiltingPoint;
    else if (myVar == soilFieldCapacityPotential)
        return myProject->soilList[soilIndex].horizon[horizonIndex].fieldCapacity;
    else if (myVar == soilWaterContentFC)
        return myProject->soilList[soilIndex].horizon[horizonIndex].waterContentFC;
    else if (myVar == soilSaturation)
        return myProject->soilList[soilIndex].horizon[horizonIndex].vanGenuchten.thetaS;
    else if (myVar == soilWaterContentWP)
    {
        double signPsiLeaf = - myProject->cultivar->parameterWangLeuning.psiLeaf; //kPa
        return soil::thetaFromSignPsi(signPsiLeaf, &(myProject->soilList[soilIndex].horizon[horizonIndex]));
    }
    else
        return NODATA;
}

double* getSoilVarProfile(Vine3DProject* myProject, int row, int col, soilVariable myVar)
{
    double* myProfile = (double *) calloc(myProject->nrSoilLayers, sizeof(double));
    long surfaceIndex = myProject->indexMap.value[row][col];
    int soilIndex = myProject->getSoilIndex(row, col);
    int layerIndex;

    for (layerIndex = 0; layerIndex < myProject->nrSoilLayers; layerIndex++)
        myProfile[layerIndex] = NODATA;

    if (surfaceIndex != myProject->indexMap.header->flag)
        for (layerIndex = 0; layerIndex < myProject->nrSoilLayers; layerIndex++)
            if ((myVar == soilWiltingPointPotential) || (myVar == soilFieldCapacityPotential)
                    || (myVar == soilWaterContentFC) || (myVar == soilWaterContentWP))
                    myProfile[layerIndex] = getSoilVar(myProject, soilIndex, layerIndex, myVar);
    return myProfile;
}

double* getCriteria3DVarProfile(Vine3DProject* myProject, int myRow, int myCol, criteria3DVariable myVar)
{
    double* myProfile = (double *) calloc(myProject->nrSoilLayers, sizeof(double));
    long surfaceIndex = myProject->indexMap.value[myRow][myCol];
    long nodeIndex, layerIndex;

    for (layerIndex = 0; layerIndex < myProject->nrSoilLayers; layerIndex++)
        myProfile[layerIndex] = NODATA;

    if (surfaceIndex != myProject->indexMap.header->flag)
        for (layerIndex = 0; layerIndex < myProject->nrSoilLayers; layerIndex++)
        {
            nodeIndex = layerIndex * myProject->nrLayerNodes + surfaceIndex;
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


bool setCriteria3DVarMap(int myLayerIndex, Vine3DProject* myProject, criteria3DVariable myVar,
                        gis::Crit3DRasterGrid* myCriteria3DMap)
{
    long surfaceIndex, nodeIndex;

    for (int row = 0; row < myProject->indexMap.header->nrRows; row++)
        for (int col = 0; col < myProject->indexMap.header->nrCols; col++)
        {
            surfaceIndex = myProject->indexMap.value[row][col];
            if (surfaceIndex != myProject->indexMap.header->flag)
            {
                nodeIndex = myLayerIndex * myProject->nrLayerNodes + surfaceIndex;
                if (! setCriteria3DVar(myVar, nodeIndex, myCriteria3DMap->value[row][col])) return false;
            }
        }

    return true;
}


bool getCriteria3DVarMap(Vine3DProject* myProject, criteria3DVariable myVar,
                        int layerIndex, gis::Crit3DRasterGrid* criteria3DMap)
{
    long surfaceIndex, nodeIndex;
    double myValue;

    criteria3DMap->initializeGrid(myProject->indexMap);

    for (int row = 0; row < myProject->indexMap.header->nrRows; row++)
        for (int col = 0; col < myProject->indexMap.header->nrCols; col++)
        {
            surfaceIndex = myProject->indexMap.value[row][col];
            if (surfaceIndex != myProject->indexMap.header->flag)
            {
                nodeIndex = layerIndex * myProject->nrLayerNodes + surfaceIndex;

                myValue = getCriteria3DVar(myVar, nodeIndex);
                if (myValue == NODATA)
                    criteria3DMap->value[row][col] = criteria3DMap->header->flag;
                else
                    criteria3DMap->value[row][col] = myValue;
            }
            else
                criteria3DMap->value[row][col] = criteria3DMap->header->flag;
        }

    return true;
}

// AWC in the first 4 cm of soil
bool getSoilSurfaceMoisture(Vine3DProject* myProject, gis::Crit3DRasterGrid* outputMap)
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
            surfaceIndex = myProject->indexMap.value[row][col];
            if (surfaceIndex != myProject->indexMap.header->flag)
            {
                sumWater = soilFluxes3D::getWaterContent(surfaceIndex);           //[m]
                minWater = 0.0;                                                 //[m]
                maxWater = 0.0;                                                 //[m]
                for (int layer = 1; layer <= lastIndex; layer++)
                {
                    nodeIndex = layer * myProject->nrLayerNodes + surfaceIndex;
                    waterContent = soilFluxes3D::getWaterContent(nodeIndex);              //[m^3 m^-3]
                    sumWater += waterContent * myProject->layerThickness[layer];        //[m]
                    wiltingPoint = getSoilVar(myProject, 0, layer, soilWaterContentWP); //[m^3 m^-3]
                    minWater += wiltingPoint * myProject->layerThickness[layer];        //[m]
                    saturation = getSoilVar(myProject, 0, layer, soilSaturation);       //[m^3 m^-3]
                    maxWater += saturation * myProject->layerThickness[layer];          //[m]
                }
                soilSurfaceMoisture = 100 * ((sumWater-minWater) / (maxWater-minWater));
                soilSurfaceMoisture = minValue(maxValue(soilSurfaceMoisture, 0), 100);
                outputMap->value[row][col] = soilSurfaceMoisture;
            }
         }
    }
    return true;
}

// return map of available water content in the root zone [mm]
bool getRootZoneAWCmap(Vine3DProject* myProject, gis::Crit3DRasterGrid* outputMap)
{
    long surfaceIndex, nodeIndex;
    double awc, thickness, sumAWC, skeleton;
    int soilIndex, horizonIndex;

    for (int row = 0; row < myProject->indexMap.header->nrRows; row++)
        for (int col = 0; col < myProject->indexMap.header->nrCols; col++)
        {
            //initialize
            outputMap->value[row][col] = outputMap->header->flag;

            surfaceIndex = myProject->indexMap.value[row][col];
            if (surfaceIndex != myProject->indexMap.header->flag)
            {
                sumAWC = 0.0;
                soilIndex = myProject->getSoilIndex(row, col);
                for (int layer = 1; layer < myProject->nrSoilLayers; layer++)
                {
                    if (myProject->grapevine.getRootDensity(layer) > 0.0)
                    {
                        nodeIndex = myProject->nrLayerNodes * layer + surfaceIndex;
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
                outputMap->value[row][col] = sumAWC;
            }
        }

    return true;
}

bool getCriteria3DIntegrationMap(Vine3DProject* myProject, criteria3DVariable myVar,
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
        for (int col = 0; col < myProject->indexMap.header->nrCols; col++)
        {
            surfaceIndex = myProject->indexMap.value[row][col];
            criteria3DMap->value[row][col] = criteria3DMap->header->flag;
            sumValues = 0.0;
            sumCoeff = 0.0;

            if (surfaceIndex != myProject->indexMap.header->flag)
            {
                soilIndex = myProject->getSoilIndex(row, col);
                for (int i = firstIndex; i <= lastIndex; i++)
                {
                    nodeIndex = i * myProject->nrLayerNodes + surfaceIndex;
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
                criteria3DMap->value[row][col] = sumValues / sumCoeff;
            }
        }

    return true;
}


bool saveWaterBalanceOutput(Vine3DProject* myProject, QDate myDate, criteria3DVariable myVar,
                            QString varName, QString notes, QString outputPath, QString myArea,
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

    QString producer = "ARPA";
    QString filename = getOutputNameDaily(producer, varName, myArea, notes, myDate);
    QString outputFilename = outputPath + getOutputNameDaily(producer, varName, myArea, notes, myDate);
    std::string myErrorString;
    if (! gis::writeEsriGrid(outputFilename.toStdString(), myMap, &myErrorString))
    {
         myProject->logError(QString::fromStdString(myErrorString));
         return false;
    }

    //geoserver - no error check
    QString geoserverFileName = myProject->getGeoserverPath() + filename;
    gis::writeEsriGrid(geoserverFileName.toStdString(), myMap, &myErrorString);

    myMap->freeGrid();

    return true;
}


QString getPrefixFromVar(QDate myDate, QString myArea, criteria3DVariable myVar)
{
    QString fileName = myDate.toString("yyyyMMdd") + "_" + myArea;

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

bool loadWaterBalanceState(Vine3DProject* myProject, QDate myDate, QString myArea, QString statePath, criteria3DVariable myVar)
{
    std::string myErrorString;
    QString myMapName;

    gis::Crit3DRasterGrid myMap;

    QString myPrefix = getPrefixFromVar(myDate, myArea, myVar);

    for (int layerIndex = 0; layerIndex < myProject->nrSoilLayers; layerIndex++)
    {
        myMapName = statePath + myPrefix + QString::number(layerIndex);
        if (! gis::readEsriGrid(myMapName.toStdString(), &myMap, &myErrorString))
        {
            myProject->logError(QString::fromStdString(myErrorString));
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
double getSoilLayerTop(Vine3DProject* myProject, int i)
{
    return myProject->layerDepth[i] - myProject->layerThickness[i] / 2.0;
}

//lower depth of soil layer [m]
double getSoilLayerBottom(Vine3DProject* myProject, int i)
{
    return myProject->layerDepth[i] + myProject->layerThickness[i] / 2.0;
}


// index of soil layer for the current depth
// depth [m]
int getSoilLayerIndex(Vine3DProject* myProject, double depth)
{
    int i= 0;
    while (depth > getSoilLayerBottom(myProject, i))
        if (++i == myProject->nrSoilLayers)
        {
            myProject->logError("getSoilLayerIndex: wrong soil depth.");
            return INDEX_ERROR;
        }
    return i;
}


bool saveWaterBalanceState(Vine3DProject* myProject, QDate myDate, QString myArea, QString statePath, criteria3DVariable myVar)
{
    std::string myErrorString;
    gis::Crit3DRasterGrid* myMap;
    myMap = new gis::Crit3DRasterGrid();
    myMap->initializeGrid(myProject->indexMap);

    QString myPrefix = getPrefixFromVar(myDate, myArea, myVar);

    for (int layerIndex = 0; layerIndex < myProject->nrSoilLayers; layerIndex++)
        if (getCriteria3DVarMap(myProject, myVar, layerIndex, myMap))
        {
            QString myOutputMapName = statePath + myPrefix + QString::number(layerIndex);
            if (! gis::writeEsriGrid(myOutputMapName.toStdString(), myMap, &myErrorString))
            {
                myProject->logError(QString::fromStdString(myErrorString));
                return false;
            }
        }
    myMap->freeGrid();

    return true;
}


bool waterBalance(Vine3DProject* myProject)
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


bool initializeWaterBalance(Vine3DProject* myProject)
{
    myProject->logInfo("\nInitialize Waterbalance...");

    QString myError;
    if (! myProject->isProjectLoaded)
    {
        myProject->projectError = "initializeWaterBalance: project not loaded";
        return(false);
    }

    double minThickness = 0.02;      //[m]
    double maxThickness = 0.1;       //[m]
    double thickFactor = 1.5;
    myProject->nrSoilLayers = computeNrLayers(myProject->soilDepth, minThickness, maxThickness, thickFactor);
    setLayersDepth(myProject, minThickness, maxThickness, thickFactor);
    myProject->logInfo("nr of layers: " + QString::number(myProject->nrSoilLayers));

    if (setindexMap(myProject))
        myProject->logInfo("nr of surface cells: " + QString::number(myProject->nrLayerNodes));
    else
    {
        myProject->logError("initializeWaterBalance: missing data in DTM");
        return(false);
    }
    myProject->nrNodes = myProject->nrLayerNodes * myProject->nrSoilLayers;
    waterSinkSource.resize(myProject->nrNodes);
    setBoundary(myProject);

    int nrLateralLink = 8;
    int myResult = soilFluxes3D::initialize(myProject->nrNodes, myProject->nrSoilLayers, nrLateralLink, true, false, false);
    if (isCrit3dError(myResult, &myError))
    {
        myProject->projectError = "initializeCriteria3D:" + myError;
        return(false);
    }

    if (! setCrit3DSurfaces(myProject)) return(false);
    if (! setCrit3DSoils(myProject)) return(false);
    if (! setCrit3DTopography(myProject)) return(false);
    if (! setCrit3DNodeSoil(myProject)) return(false);


    //soilFluxes3D::setNumericalParameters(6.0, 600.0, 200, 10, 12, 3);   // precision
    soilFluxes3D::setNumericalParameters(30.0, 1800.0, 100, 10, 12, 2);  // speedy
    //soilFluxes3D::setNumericalParameters(300.0, 3600.0, 100, 10, 12, 1);   // very speedy (high error)
    soilFluxes3D::setHydraulicProperties(MODIFIEDVANGENUCHTEN, MEAN_LOGARITHMIC, 10.0);

    myProject->logInfo("Waterbalance initialized");
    return(true);
}






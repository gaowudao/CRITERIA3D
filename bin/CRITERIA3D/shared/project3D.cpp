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
    ftomei@arpae.it
    gantolini@arpae.it
*/

#include "project3D.h"
#include "soilFluxes3D.h"


Project3D::Project3D() : Project()
{
    initializeProject3D();
}

void Project3D::initializeProject3D()
{
    nrSoils = 0;
    soilDepth = 1.0;            // [m]

    minThickness = 0.02;        // [m]
    maxThickness = 0.1;         // [m]
    thickFactor = 1.5;

    nrLayers = 0;
    nrNodes = 0;
    nrLateralLink = 8;

    meteoMaps = nullptr;
}

void Project3D::clearProject3D()
{
    clearWaterBalance3D();

    for (unsigned int i = 0; i < soilList.size(); i++)
    {
        soilList[i].cleanSoil();
    }

    delete meteoMaps;

    clearProject();
}


void Project3D::clearWaterBalance3D()
{
    soilFluxes3D::cleanMemory();
    waterSinkSource.clear();

    layerThickness.clear();
    layerDepth.clear();

    for (unsigned int i = 0; i < indexMap.size(); i++)
    {
        indexMap[i].clear();
    }
    indexMap.clear();

    boundaryMap.clear();
    soilIndexMap.clear();
}


void Project3D::computeNrLayers()
 {
    double nextThickness;
    double prevThickness = minThickness;
    double depth = minThickness * 0.5;

    nrLayers = 1;
    while (depth < soilDepth)
    {
        nextThickness = minValue(maxThickness, prevThickness * thickFactor);
        depth = depth + (prevThickness + nextThickness) * 0.5;
        prevThickness = nextThickness;
        nrLayers++;
    }
}


// set thickness and depth (center) of layers [m]
void Project3D::setLayersDepth()
{
    unsigned int lastLayer = nrLayers-1;
    layerDepth.resize(nrLayers);
    layerThickness.resize(nrLayers);

    layerDepth[0] = 0.0;
    layerThickness[0] = 0.0;
    layerThickness[1] = minThickness;
    layerDepth[1] = minThickness * 0.5;

    for (unsigned int i = 2; i < nrLayers; i++)
    {
        if (i == lastLayer)
        {
            layerThickness[i] = soilDepth - (layerDepth[i-1] + layerThickness[i-1] / 2.0);
        }
        else
        {
            layerThickness[i] = minValue(maxThickness, layerThickness[i-1] * thickFactor);
        }

        layerDepth[i] = layerDepth[i-1] + (layerThickness[i-1] + layerThickness[i]) * 0.5;
    }
}


bool Project3D::setIndexMaps()
{
    // check
    if (!DEM.isLoaded || !soilIndexMap.isLoaded || nrSoils == 0)
    {
        if (!DEM.isLoaded)
            logError("Missing Digital Elevation Model.");
        else if (!soilIndexMap.isLoaded)
            logError("Missing soil map.");
        else if (nrSoils == 0)
            logError("Missing soil properties.");
        return false;
    }

    indexMap.resize(nrLayers);

    long currentIndex = 0;
    for (unsigned int i = 0; i < nrLayers; i++)
    {
        indexMap.at(i).initializeGrid(DEM);

        for (int row = 0; row < indexMap.at(i).header->nrRows; row++)
        {
            for (int col = 0; col < indexMap.at(i).header->nrCols; col++)
            {
                if (int(DEM.value[row][col]) != int(DEM.header->flag))
                {
                    int soilIndex = getSoilIndex(row, col);
                    if (isWithinSoil(soilIndex, layerDepth.at(i)))
                    {
                        indexMap.at(i).value[row][col] = currentIndex;
                        currentIndex++;
                    }
                }
            }
        }
    }

    nrNodes = currentIndex;
    return (currentIndex > 0);
}


bool Project3D::setBoundary()
{
    if (! this->DEM.isLoaded)
    {
        logError("Missing Digital Elevation Model.");
        return false;
    }

    boundaryMap.initializeGrid(this->DEM);
    for (int row = 0; row < boundaryMap.header->nrRows; row++)
    {
        for (int col = 0; col < boundaryMap.header->nrCols; col++)
        {
            if (gis::isBoundary(this->DEM, row, col))
            {
                // or: if(isMinimum)
                if (! gis::isStrictMaximum(this->DEM, row, col))
                {
                    boundaryMap.value[row][col] = BOUNDARY_RUNOFF;
                }
            }
        }
    }

    return true;
}


bool Project3D::setCrit3DSurfaces()
{
    QString myError;
    int result;

    // TODO: read soilUse parameters
    int nrSurfaces = 1;
    double ManningRoughness = 0.24;         // [s m^-1/3]
    double surfacePond = 0.002;             // [m]

    for (int surfaceIndex = 0; surfaceIndex < nrSurfaces; surfaceIndex++)
    {
        result = soilFluxes3D::setSurfaceProperties(surfaceIndex, ManningRoughness, surfacePond);
        if (isCrit3dError(result, &myError))
        {
            errorString = "setCrit3DSurfaces: " + myError
                           + "\n surface nr:" + QString::number(surfaceIndex);
            return false;
        }
    }

    return true;
}


// ThetaS and ThetaR already corrected for coarse fragments
bool Project3D::setCrit3DSoils()
{
    soil::Crit3DHorizon* myHorizon;
    QString myError;
    int result;

    for (unsigned int soilIndex = 0; soilIndex < nrSoils; soilIndex++)
    {
        for (int horizIndex = 0; horizIndex < soilList[soilIndex].nrHorizons; horizIndex++)
        {
            myHorizon = &(soilList[soilIndex].horizon[horizIndex]);
            if ((myHorizon->texture.classUSDA > 0) && (myHorizon->texture.classUSDA <= 12))
            {
                result = soilFluxes3D::setSoilProperties(int(soilIndex), horizIndex,
                     myHorizon->vanGenuchten.alpha * GRAVITY,               // [kPa-1] -> [m-1]
                     myHorizon->vanGenuchten.n,
                     myHorizon->vanGenuchten.m,
                     myHorizon->vanGenuchten.he / GRAVITY,                  // [kPa] -> [m]
                     myHorizon->vanGenuchten.thetaR,
                     myHorizon->vanGenuchten.thetaS,
                     myHorizon->waterConductivity.kSat / DAY_SECONDS / 100, // [cm/d] -> [m/s]
                     myHorizon->waterConductivity.l,
                     myHorizon->organicMatter,
                     double(myHorizon->texture.clay));

                 if (isCrit3dError(result, &myError))
                 {
                     errorString = "setCrit3DSoils: " + myError
                                + "\n soil nr: " + QString::number(soilList[unsigned(soilIndex)].id)
                                + " horizon nr: " + QString::number(horizIndex);
                     return false;
                 }
            }
        }
    }

    return true;
}


bool Project3D::setCrit3DTopography()
{
    double x, y;
    float z, lateralArea, slope;
    double area, volume;
    long index, linkIndex, soilIndex;
    int myResult;
    QString myError;

    for (size_t layer = 0; layer < nrLayers; layer++)
    {
        for (int row = 0; row < indexMap.at(layer).header->nrRows; row++)
        {
            for (int col = 0; col < indexMap.at(layer).header->nrCols; col++)
            {
                index = long(indexMap.at(layer).value[row][col]);

                if (index != long(indexMap.at(layer).header->flag))
                {
                    gis::getUtmXYFromRowCol(DEM, row, col, &x, &y);
                    area = DEM.header->cellSize * DEM.header->cellSize;
                    slope = radiationMaps->slopeMap->value[row][col] / 100;
                    z = DEM.value[row][col] - float(layerDepth[layer]);
                    volume = area * layerThickness[layer];

                    soilIndex = getSoilIndex(row, col);

                    // surface
                    if (layer == 0)
                    {
                        lateralArea = float(DEM.header->cellSize);

                        if (int(boundaryMap.value[row][col]) == BOUNDARY_RUNOFF)
                            myResult = soilFluxes3D::setNode(index, float(x), float(y), float(z), area, true, true, BOUNDARY_RUNOFF, float(slope));
                        else
                            myResult = soilFluxes3D::setNode(index, float(x), float(y), z, area, true, false, BOUNDARY_NONE, 0.0);
                    }
                    // sub-surface
                    else
                    {
                        lateralArea = float(DEM.header->cellSize * layerThickness[layer]);

                        // last project layer or last soil layer
                        if (int(layer) == nrLayers - 1 || ! isWithinSoil(soilIndex, layerDepth.at(size_t(layer+1))))
                            myResult = soilFluxes3D::setNode(index, float(x), float(y), z, volume, false, true, BOUNDARY_FREEDRAINAGE, 0.0);
                        else
                        {
                            if (int(boundaryMap.value[row][col]) == BOUNDARY_RUNOFF)
                                myResult = soilFluxes3D::setNode(index, float(x), float(y), z, volume, false, true, BOUNDARY_FREELATERALDRAINAGE, slope);
                            else
                                myResult = soilFluxes3D::setNode(index, float(x), float(y), z, volume, false, false, BOUNDARY_NONE, 0.0);
                        }
                    }

                    // check error
                    if (isCrit3dError(myResult, &myError))
                    {
                        errorString = "setTopography:" + myError + " in layer nr:" + QString::number(layer);
                        return(false);
                    }

                    // up link
                    if (layer > 0)
                    {
                        linkIndex = long(indexMap.at(layer - 1).value[row][col]);

                        if (linkIndex != long(indexMap.at(layer - 1).header->flag))
                        {
                            myResult = soilFluxes3D::setNodeLink(index, linkIndex, UP, float(area));
                            if (isCrit3dError(myResult, &myError))
                            {
                                errorString = "setNodeLink:" + myError + " in layer nr:" + QString::number(layer);
                                return(false);
                            }
                        }
                    }

                    // down link
                    if (int(layer) < (nrLayers - 1) && isWithinSoil(soilIndex, layerDepth.at(size_t(layer + 1))))
                    {
                        linkIndex = long(indexMap.at(layer + 1).value[row][col]);

                        if (linkIndex != long(indexMap.at(layer + 1).header->flag))
                        {
                            myResult = soilFluxes3D::setNodeLink(index, linkIndex, DOWN, float(area));

                            if (isCrit3dError(myResult, &myError))
                            {
                                errorString = "setNodeLink:" + myError + " in layer nr:" + QString::number(layer);
                                return(false);
                            }
                        }
                    }

                    // lateral links
                    for (int i=-1; i <= 1; i++)
                    {
                        for (int j=-1; j <= 1; j++)
                        {
                            if ((i != 0)||(j != 0))
                            {
                                if (! gis::isOutOfGridRowCol(row+i, col+j, indexMap.at(layer)))
                                {
                                    linkIndex = long(indexMap.at(layer).value[row+i][col+j]);
                                    if (linkIndex != long(indexMap.at(layer).header->flag))
                                    {
                                        myResult = soilFluxes3D::setNodeLink(index, linkIndex, LATERAL, float(lateralArea / 2));
                                        if (isCrit3dError(myResult, &myError))
                                        {
                                            errorString = "setNodeLink:" + myError + " in layer nr:" + QString::number(layer);
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
    }

   return true;
}


bool Project3D::setCrit3DNodeSoil()
{
    long index;
    int soilIndex, horizonIndex, myResult;
    QString myError;

    for (unsigned int layer = 0; layer < nrLayers; layer ++)
    {
        for (int row = 0; row < indexMap.at(layer).header->nrRows; row++)
        {
            for (int col = 0; col < indexMap.at(layer).header->nrCols; col++)
            {
                index = long(indexMap.at(layer).value[row][col]);
                if (index != long(indexMap.at(layer).header->flag))
                {
                    soilIndex = getSoilIndex(row, col);

                    if (layer == 0)
                    {
                        // surface
                        myResult = soilFluxes3D::setNodeSurface(index, 0);
                    }
                    else
                    {
                        // sub-surface
                        horizonIndex = soil::getHorizonIndex(&(soilList[unsigned(soilIndex)]), layerDepth[layer]);
                        if (horizonIndex == NODATA)
                        {
                            logError("function setCrit3DNodeSoil: \nno horizon definition in soil "
                                    + QString::number(soilList[unsigned(soilIndex)].id) + " depth: " + QString::number(layerDepth[layer])
                                    +"\nCheck soil totalDepth");
                            return false;
                        }

                        myResult = soilFluxes3D::setNodeSoil(index, soilIndex, horizonIndex);

                        // check error
                        if (isCrit3dError(myResult, &myError))
                        {
                            logError("setCrit3DNodeSoil:" + myError + " in soil nr: " + QString::number(soilIndex)
                                    + " horizon nr:" + QString::number(horizonIndex));
                            return false;
                        }
                    }
                }
            }
        }
    }
    return true;
}


bool Project3D::initializeSoilMoisture(int month)
{
    int crit3dResult = CRIT3D_OK;
    QString error;
    long index, soilIndex, horizonIndex;
    double moistureIndex, waterPotential;
    double fieldCapacity;                    // [m]
    double  wiltingPoint = -160.0;           // [m]
    double dry = wiltingPoint / 3.0;         // [m] dry potential

    // [0-1] min: august  max: february
    moistureIndex = fabs(1 - (month - 2) / 6);
    moistureIndex = maxValue(moistureIndex, 0.001);
    moistureIndex = log(moistureIndex) / log(0.001);

    logInfo("Initialize soil moisture");

    for (unsigned int layer = 0; layer < nrLayers; layer++)
    {
        for (int row = 0; row < indexMap.at(size_t(layer)).header->nrRows; row++)
        {
            for (int col = 0; col < indexMap.at(size_t(layer)).header->nrCols; col++)
            {
                index = long(indexMap.at(size_t(layer)).value[row][col]);
                if (index != long(indexMap.at(size_t(layer)).header->flag))
                {
                    if (layer == 0)
                    {
                        // surface
                        soilFluxes3D::setWaterContent(index, 0.0);
                    }
                    else
                    {
                        soilIndex = getSoilIndex(row, col);
                        horizonIndex = soil::getHorizonIndex(&(soilList[soilIndex]), layerDepth[size_t(layer)]);
                        fieldCapacity = soilList[soilIndex].horizon[horizonIndex].fieldCapacity;
                        waterPotential = fieldCapacity - moistureIndex * (fieldCapacity-dry);
                        crit3dResult = soilFluxes3D::setMatricPotential(index, waterPotential);
                    }

                    if (isCrit3dError(crit3dResult, &error))
                    {
                        logError("initializeSoilMoisture:" + error);
                        return false;
                    }
                }
            }
        }
    }

    return true;
}


int Project3D::getSoilIndex(long row, long col)
{
    if ( !soilIndexMap.isLoaded) return NODATA;

    int soilIndex = int(soilIndexMap.getValueFromRowCol(row, col));
    if (soilIndex == int(soilIndexMap.header->flag)) return NODATA;

    return soilIndex;
}


bool Project3D::isWithinSoil(int soilIndex, double depth)
{
    if (soilIndex == int(NODATA) || soilIndex >= int(soilList.size())) return false;

    // check if depth is lower than lowerDepth of last horizon
    int lastHorizon = soilList[unsigned(soilIndex)].nrHorizons -1;
    double lowerDepth = soilList[unsigned(soilIndex)].horizon[lastHorizon].lowerDepth;

    return (depth <= lowerDepth);
}


// ------------------------- other functions -------------------------

bool isCrit3dError(int result, QString* error)
{
    if (result == CRIT3D_OK) return false;

    switch (result)
    {
    case INDEX_ERROR:
        *error = "index error";
        break;
    case MEMORY_ERROR:
        *error = "memory error";
        break;
    case TOPOGRAPHY_ERROR:
        *error = "topography error";
        break;
    case BOUNDARY_ERROR:
        *error = "boundary error";
        break;
    case PARAMETER_ERROR:
        *error = "parameter error";
        break;
    default:
        *error = "parameter error";
    }

    return true;
}


double getCriteria3DVar(criteria3DVariable myVar, long nodeIndex)
{
    double crit3dVar;

    if (myVar == waterContent)
    {
        crit3dVar = soilFluxes3D::getWaterContent(nodeIndex);
    }
    else if (myVar == availableWaterContent)
    {
        crit3dVar = soilFluxes3D::getAvailableWaterContent(nodeIndex);
    }
    else if (myVar == waterTotalPotential)
    {
        crit3dVar = soilFluxes3D::getTotalPotential(nodeIndex);
    }
    else if (myVar == waterMatricPotential)
    {
        crit3dVar = soilFluxes3D::getMatricPotential(nodeIndex);
    }
    else if (myVar == degreeOfSaturation)
    {
        crit3dVar = soilFluxes3D::getDegreeOfSaturation(nodeIndex);
    }
    else if (myVar == waterInflow)
    {
        crit3dVar = soilFluxes3D::getSumLateralWaterFlowIn(nodeIndex) * 1000;
    }
    else if (myVar == waterOutflow)
    {
        crit3dVar = soilFluxes3D::getSumLateralWaterFlowOut(nodeIndex) * 1000;
    }
    else if (myVar == waterDeficit)
    {
        // TODO chiamare soil per field capacity
        double fieldCapacity = 3.0;
        crit3dVar = soilFluxes3D::getWaterDeficit(nodeIndex, fieldCapacity);
    }
    else
    {
        crit3dVar = MISSING_DATA_ERROR;
    }

    if (crit3dVar == INDEX_ERROR || crit3dVar == MEMORY_ERROR
        || crit3dVar == MISSING_DATA_ERROR || crit3dVar == TOPOGRAPHY_ERROR) {
        return NODATA;
    }
    else {
        return crit3dVar;
    }

}



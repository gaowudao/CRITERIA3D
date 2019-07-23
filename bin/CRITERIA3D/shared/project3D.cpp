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
    soilDepth = 0.4;            // [m]

    minThickness = 0.02;        // [m]
    maxThickness = 0.1;         // [m]
    thickFactor = 1.5;

    nrLayers = 0;
    nrNodes = 0;
    nrNodesPerLayer = 0;
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
    int lastLayer = nrLayers-1;
    layerDepth.resize(unsigned(nrLayers));
    layerThickness.resize(unsigned(nrLayers));

    layerDepth[0] = 0.0;
    layerThickness[0] = 0.0;
    layerThickness[1] = minThickness;
    layerDepth[1] = minThickness * 0.5;

    for (unsigned int i = 2; i < unsigned(nrLayers); i++)
    {
        if (i == unsigned(lastLayer))
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

    for (int soilIndex = 0; soilIndex < nrSoils; soilIndex++)
    {
        for (int horizIndex = 0; horizIndex < soilList[unsigned(soilIndex)].nrHorizons; horizIndex++)
        {
            myHorizon = &(soilList[unsigned(soilIndex)].horizon[horizIndex]);
            if ((myHorizon->texture.classUSDA > 0) && (myHorizon->texture.classUSDA <= 12))
            {
                result = soilFluxes3D::setSoilProperties(soilIndex, horizIndex,
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



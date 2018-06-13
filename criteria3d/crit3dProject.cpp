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
    projectError = "";
    projectLog = "";
    nrSoils = 0;
    nrLayers = 0;
    nrVoxelsPerLayer = 0;
    nrVoxels = 0;
    soilDepth = 1.0;       //[m] default: 1 meter
}


void Crit3DProject::closeProject()
{
    soilMap.freeGrid();
    cropMap.freeGrid();
    boundaryMap.freeGrid();
    indexMap.freeGrid();

    delete meteoMaps;

    cleanWaterBalanceMemory();
}


bool Crit3DProject::initializeProject(gis::Crit3DRasterGrid* myDTM, Crit3DRadiationMaps* myRadiationMaps)
{
    closeProject();

    dtm = myDTM;
    radiationMaps = myRadiationMaps;
    meteoMaps = new Crit3DMeteoMaps(*dtm);

    // passa soil settings
    // loadSoils()
    // soil map
    // loadCropProperties()
    // crop map

    // maps computed here
    // compute indexMap;
    // compute boundaryMap;

    if (! initializeWaterBalance(this))
        return(false);

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


int Crit3DProject::getSoilIndex(long row, long col)
{
    if (this->soilMap.value[row][col] == this->soilMap.header->flag)
    {
        //DEFAULT
        return 0;
    }
    else return int(this->soilMap.value[row][col]);
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
    long firstLayerIndex = long(indexMap.value[row][col]);
    int soilIndex = getSoilIndex(row, col);

    for (int layerIndex = 0; layerIndex < nrLayers; layerIndex++)
        myProfile[layerIndex] = NODATA;

    if (firstLayerIndex != indexMap.header->flag)
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

void Crit3DProject::log(std::string myLog)
{
    projectLog += myLog + "\n";
}

void Crit3DProject::setError(std::string myError)
{
    this->projectError = myError;
}


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
#include "utilities.h"
#include "crit3dProject.h"
#include "waterBalance3D.h"
#include "soilDbTools.h"

#include <QtSql>


Crit3DProject::Crit3DProject() : Project3D()
{
    isParametersLoaded = false;
    isCriteria3DInitialized = false;
}


bool Crit3DProject::loadCriteria3DProject(QString myFileName)
{
    if (myFileName == "") return(false);

    clearCriteria3DProject();
    if (isProjectLoaded) clearProject();
    initializeProject();

    if (! loadProjectSettings(myFileName))
        return false;

    if (! loadProject())
        return false;

    if (! loadCriteria3DSettings())
        return false;

    // initialize meteo maps
    meteoMaps = new Crit3DMeteoMaps(DEM);

    if (projectName != "")
    {
        logInfo("Project " + projectName + " loaded");
    }

    isProjectLoaded = true;
    return true;
}


bool Crit3DProject::loadCriteria3DSettings()
{          
    // TODO
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

    // TODO: Load crop parameters

    isParametersLoaded = true;
    return true;
}


bool Crit3DProject::loadSoilMap(QString myFileName)
{
    std::string myError;
    std::string fileName = myFileName.left(myFileName.length()-4).toStdString();

    logInfo("Read soil map...");

    if (! gis::readEsriGrid(fileName, &soilMap, &myError))
    {
        logError("Load soil map failed: " + myFileName);
        return false;
    }

    logInfo("Soil map = " + myFileName);
    return true;
}


bool Crit3DProject::setSoilIndexMap()
{
    // check
    if (!DEM.isLoaded || !soilMap.isLoaded || soilList.size() == 0)
    {
        if (!DEM.isLoaded)
            logError("Missing Digital Elevation Model.");
        else if (!soilMap.isLoaded)
            logError("Missing soil map.");
        else if (soilList.size() == 0)
            logError("Missing soil properties.");
        return false;
    }

    int soilIndex;
    soilIndexMap.initializeGrid(*(DEM.header));
    for (int row = 0; row < DEM.header->nrRows; row++)
    {
        for (int col = 0; col < DEM.header->nrCols; col++)
        {
            if (int(DEM.value[row][col]) != int(DEM.header->flag))
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


int Crit3DProject::getCrit3DSoilIndex(double x, double y)
{
    if (! soilMap.isLoaded)
        return INDEX_ERROR;

    int idSoil = int(gis::getValueFromXY(soilMap, x, y));

    if (idSoil == int(soilMap.header->flag))
    {
        return INDEX_ERROR;
    }
    else
    {
        return idSoil;
    }
}


QString Crit3DProject::getCrit3DSoilCode(double x, double y)
{
    QString soilCode = "";

    int idSoil = getCrit3DSoilIndex(x, y);

    if (idSoil != INDEX_ERROR)
    {
        for (unsigned int i = 0; i < soilList.size(); i++)
        {
            if (soilList[i].id == idSoil)
            {
                soilCode = QString::fromStdString(soilList[i].code);
                break;
            }
        }
    }

    return soilCode;
}


double Crit3DProject::getSoilVar(int soilIndex, int layerIndex, soil::soilVariable myVar)
{
    int horizonIndex = soil::getHorizonIndex(&(soilList[unsigned(soilIndex)]),
                                               layerDepth[unsigned(layerIndex)]);
    if (myVar == soil::soilWaterPotentialWP)
        return soilList[unsigned(soilIndex)].horizon[horizonIndex].wiltingPoint;
    else if (myVar == soil::soilWaterPotentialFC)
        return soilList[unsigned(soilIndex)].horizon[horizonIndex].fieldCapacity;
    else if (myVar == soil::soilWaterContentFC)
        return soilList[unsigned(soilIndex)].horizon[horizonIndex].waterContentFC;
    else if (myVar == soil::soilWaterContentSat)
        return soilList[unsigned(soilIndex)].horizon[horizonIndex].vanGenuchten.thetaS;
    else if (myVar == soil::soilWaterContentWP)
    {
        double signPsiLeaf = -160;      //[m]
        return soil::thetaFromSignPsi(signPsiLeaf, &(soilList[unsigned(soilIndex)].horizon[horizonIndex]));
    }
    else
        return NODATA;
}


void Crit3DProject::clearCriteria3DProject()
{
    clearWaterBalance3D();
    cropIndexMap.clear();

    isCriteria3DInitialized = false;
}


bool Crit3DProject::interpolationRelHumidity(const Crit3DTime& myTime, gis::Crit3DRasterGrid *myRaster, bool showInfo)
{
    if (! this->DEM.isLoaded)
    {
        errorString = "Load a Digital Elevation Model before.";
        return false;
    }

    if (nrMeteoPoints == 0)
    {
        errorString = "Open a meteo points DB before.";
        return false;
    }

    if (interpolationSettings.getUseDewPoint())
    {
        // TODO check on airTemperatureMap

        gis::Crit3DRasterGrid *dewT_Map = new(gis::Crit3DRasterGrid);
        dewT_Map->initializeGrid(this->DEM);

        if (! interpolationDem(airDewTemperature, myTime, dewT_Map, showInfo))
            return false;

        if (! this->meteoMaps->computeRelativeHumidityMap(*dewT_Map))
            return false;

        *myRaster = *(this->meteoMaps->airRelHumidityMap);

        return true;
    }
    else
    {
        return interpolationDem(airRelHumidity, myTime, myRaster, showInfo);
    }
}


bool Crit3DProject::computeAllMeteoMaps(const Crit3DTime& myTime, bool showInfo)
{
    if (! this->DEM.isLoaded)
    {
        errorString = "Load a Digital Elevation Model before.";
        return false;
    }
    if (this->meteoMaps == nullptr)
    {
        errorString = "Meteo maps not initialized.";
        return false;
    }

    this->meteoMaps->isComputed = false;

    FormInfo myInfo;
    if (showInfo)
    {
        myInfo.start("Compute air temperature...", 6);
    }

    if (! interpolationDemMain(airTemperature, myTime, this->meteoMaps->airTemperatureMap, false))
        return false;

    if (showInfo)
    {
        myInfo.setText("Compute relative humidity...");
        myInfo.setValue(1);
    }

    if (! interpolationRelHumidity(myTime, this->meteoMaps->airRelHumidityMap, false))
        return false;

    if (showInfo)
    {
        myInfo.setText("Compute relative precipitation...");
        myInfo.setValue(2);
    }

    if (! interpolationDemMain(precipitation, myTime, this->meteoMaps->precipitationMap, false))
        return false;

    if (showInfo)
    {
        myInfo.setText("Compute wind intensity...");
        myInfo.setValue(3);
    }

    if (! interpolationDemMain(windIntensity, myTime, this->meteoMaps->windIntensityMap, false))
        return false;

    if (showInfo)
    {
        myInfo.setText("Compute solar radiation...");
        myInfo.setValue(4);
    }

    if (! interpolationDemMain(globalIrradiance, myTime, this->radiationMaps->globalRadiationMap, false))
        return false;

    if (showInfo)
    {
        myInfo.setText("Compute ET0...");
        myInfo.setValue(5);
    }

    if (! this->meteoMaps->computeET0Map(&(this->DEM), this->radiationMaps))
        return false;

    if (showInfo) myInfo.close();

    this->meteoMaps->isComputed = true;

    return true;
}


bool Crit3DProject::initializeCriteria3D()
{
    // check data
    if (! this->DEM.isLoaded)
    {
        logError("Missing Digital Elevation Model.");
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

    this->clearCriteria3DProject();

    if (!setSoilIndexMap()) return false;

    // TODO loadCropProperties()

    // TODO load crop map

    /* TODO initialize root density
    // andrebbe rifatto per ogni tipo di suolo (ora considera solo suolo 0)
    int nrSoilLayersWithoutRoots = 2;
    int soilLayerWithRoot = this->nrSoilLayers - nrSoilLayersWithoutRoots;
    double depthModeRootDensity = 0.35*this->soilDepth;     //[m] depth of mode of root density
    double depthMeanRootDensity = 0.5*this->soilDepth;      //[m] depth of mean of root density
    initializeRootProperties(&(this->soilList[0]), this->nrSoilLayers, this->soilDepth,
                         this->layerDepth.data(), this->layerThickness.data(),
                         nrSoilLayersWithoutRoots, soilLayerWithRoot,
                         GAMMA_DISTRIBUTION, depthModeRootDensity, depthMeanRootDensity);
    */

    if (! initializeWaterBalance3D(this))
    {
        clearCriteria3DProject();
        return false;
    }

    isCriteria3DInitialized = true;
    logInfoGUI("Criteria3D model initialized");

    return true;
}


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
#include "gis.h"

#include <QtSql>


Crit3DProject::Crit3DProject() : Project3D()
{
    isParametersLoaded = false;
    isCriteria3DInitialized = false;

    hourlyMeteoMaps = nullptr;
}


bool Crit3DProject::loadCriteria3DProject(QString myFileName)
{
    if (myFileName == "") return(false);

    clearCriteria3DProject();
    if (isProjectLoaded) clearProject();

    initializeProject();
    initializeProject3D();

    if (! loadProjectSettings(myFileName))
        return false;

    if (! loadCriteria3DSettings())
        return false;

    if (! loadProject())
        return false;

    // soil map and data
    if (soilMapFileName != "") loadSoilMap(soilMapFileName);
    if (soilDbFileName != "") loadSoilDatabase(soilDbFileName);

    // initialize meteo maps
    hourlyMeteoMaps = new Crit3DHourlyMeteoMaps(DEM);

    if (projectName != "")
    {
        logInfo("Project " + projectName + " loaded");
    }

    isProjectLoaded = true;
    return true;
}


bool Crit3DProject::loadCriteria3DSettings()
{
    projectSettings->beginGroup("project");
        soilDbFileName = projectSettings->value("soil_db").toString();
        soilMapFileName = projectSettings->value("soil_map").toString();
    projectSettings->endGroup();

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


bool Crit3DProject::loadSoilMap(QString fileName)
{
    if (fileName == "")
    {
        logError("Missing soil map filename");
        return false;
    }

    soilMapFileName = fileName;
    fileName = getCompleteFileName(fileName, PATH_GEO);

    std::string myError;
    std::string myFileName = fileName.left(fileName.length()-4).toStdString();

    if (! gis::readEsriGrid(myFileName, &soilMap, &myError))
    {
        logError("Load soil map failed: " + fileName);
        return false;
    }

    logInfo("Soil map = " + fileName);
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
    double x, y;
    soilIndexMap.initializeGrid(*(DEM.header));
    for (int row = 0; row < DEM.header->nrRows; row++)
    {
        for (int col = 0; col < DEM.header->nrCols; col++)
        {
            if (int(DEM.value[row][col]) != int(DEM.header->flag))
            {
                gis::getUtmXYFromRowCol(DEM, row, col, &x, &y);
                soilIndex = getCrit3DSoilIndex(x, y);
                if (soilIndex != NODATA)
                    soilIndexMap.value[row][col] = float(soilIndex);
            }
        }
    }

    soilIndexMap.isLoaded = true;
    return true;
}


int Crit3DProject::getCrit3DSoilId(double x, double y)
{
    if (! soilMap.isLoaded)
        return NODATA;

    int idSoil = int(gis::getValueFromXY(soilMap, x, y));

    if (idSoil == int(soilMap.header->flag))
    {
        return NODATA;
    }
    else
    {
        return idSoil;
    }
}


int Crit3DProject::getCrit3DSoilIndex(double x, double y)
{
    int idSoil = getCrit3DSoilId(x, y);
    if (idSoil == NODATA) return NODATA;

    for (unsigned int index = 0; index < soilList.size(); index++)
    {
        if (soilList[index].id == idSoil)
        {
            return signed(index);
        }
    }

    return NODATA;
}


QString Crit3DProject::getCrit3DSoilCode(double x, double y)
{
    int idSoil = getCrit3DSoilId(x, y);
    if (idSoil == NODATA) return "";

    for (unsigned int i = 0; i < soilList.size(); i++)
    {
        if (soilList[i].id == idSoil)
        {
            return QString::fromStdString(soilList[i].code);
        }
    }

    return "NOT FOUND";
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

        if (! interpolationDem(airDewTemperature, myTime, hourlyMeteoMaps->mapHourlyTdew, showInfo))
            return false;

        if (! this->hourlyMeteoMaps->computeRelativeHumidityMap())
            return false;

        *myRaster = *(hourlyMeteoMaps->mapHourlyRelHum);

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
    if (this->hourlyMeteoMaps == nullptr)
    {
        errorString = "Meteo maps not initialized.";
        return false;
    }

    this->hourlyMeteoMaps->setComputed(false);

    FormInfo myInfo;
    if (showInfo)
    {
        myInfo.start("Computing air temperature...", 6);
    }

    if (! interpolationDemMain(airTemperature, myTime, this->hourlyMeteoMaps->mapHourlyT, false))
        return false;

    if (showInfo)
    {
        myInfo.setText("Computing air relative humidity...");
        myInfo.setValue(1);
    }

    if (! interpolationRelHumidity(myTime, this->hourlyMeteoMaps->mapHourlyRelHum, false))
        return false;

    if (showInfo)
    {
        myInfo.setText("Computing precipitation...");
        myInfo.setValue(2);
    }

    if (! interpolationDemMain(precipitation, myTime, this->hourlyMeteoMaps->mapHourlyPrec, false))
        return false;

    if (showInfo)
    {
        myInfo.setText("Computing wind intensity...");
        myInfo.setValue(3);
    }

    if (! interpolationDemMain(windIntensity, myTime, this->hourlyMeteoMaps->mapHourlyWindInt, false))
        return false;

    if (showInfo)
    {
        myInfo.setText("Computing global irradiance...");
        myInfo.setValue(4);
    }

    if (! interpolationDemMain(globalIrradiance, myTime, this->radiationMaps->globalRadiationMap, false))
        return false;

    if (showInfo)
    {
        myInfo.setText("Computing ET0...");
        myInfo.setValue(5);
    }

    if (! this->hourlyMeteoMaps->computeET0PMMap(this->DEM, this->radiationMaps))
        return false;

    if (showInfo) myInfo.close();

    this->hourlyMeteoMaps->setComputed(true);

    return true;
}


bool Crit3DProject::initializeCriteria3DModel()
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
        logError("Criteria3D model not initialized.");
        return false;
    }

    isCriteria3DInitialized = true;
    logInfoGUI("Criteria3D model initialized");

    return true;
}


void Crit3DProject::setMapsComputed(bool value)
{
    if (radiationMaps != nullptr)
        radiationMaps->isComputed = value;

    if (hourlyMeteoMaps != nullptr)
        hourlyMeteoMaps->setComputed(value);
}


QString Crit3DProject::getOutputNameHourly(meteoVariable myVar, QDateTime myTime)
{
    std::string varName = MapHourlyMeteoVarToString.at(myVar);
    QString timeStr = myTime.toString("yyyyMMdd_hhmm");
    return QString::fromStdString(varName) + "_" + timeStr;
}


gis::Crit3DRasterGrid* Crit3DProject::getHourlyMeteoRaster(meteoVariable myVar)
{
    if (myVar == globalIrradiance)
    {
        return radiationMaps->globalRadiationMap;
    }
    else
    {
        return hourlyMeteoMaps->getMapFromVar(myVar);
    }
}


bool Crit3DProject::saveHourlyMeteoOutput(meteoVariable myVar, const QString& myOutputPath, QDateTime myTime)
{
    gis::Crit3DRasterGrid* myRaster = getHourlyMeteoRaster(myVar);
    if (myRaster == nullptr) return false;

    QString fileName = getOutputNameHourly(myVar, myTime);
    QString outputFileName = myOutputPath + fileName;

    std::string errStr;
    if (! gis::writeEsriGrid(outputFileName.toStdString(), myRaster, &errStr))
    {
        logError(QString::fromStdString(errStr));
        return false;
    }
    else
        return true;
}


bool Crit3DProject::interpolateAndSaveHourlyMeteo(meteoVariable myVar, const QDateTime& myTime,
                                                  const QString& outputPath, bool saveOutput)
{
    gis::Crit3DRasterGrid* myRaster = getHourlyMeteoRaster(myVar);
    if (myRaster == nullptr) return false;

    if (! interpolationDemMain(myVar, getCrit3DTime(myTime), myRaster, false))
    {
        QString timeStr = myTime.toString("yyyy-MM-dd hh:mm");
        QString varStr = QString::fromStdString(MapHourlyMeteoVarToString.at(myVar));
        errorString = "Error in interpolation of " + varStr + " at time: " + timeStr;
        return false;
    }

    if (saveOutput)
        return saveHourlyMeteoOutput(myVar, outputPath, myTime);
    else
        return true;
}


bool Crit3DProject::modelDailyCycle(bool isInitialState, QDate myDate, int firstHour, int lastHour,
                                    const QString& outputPath, bool saveOutput)
{
    setCurrentDate(myDate);

    for (int hour = firstHour; hour <= lastHour; hour++)
    {
        setCurrentHour(hour);

        QDateTime myTime = QDateTime(myDate, QTime(hour, 0, 0));
        logInfo("Compute " + myTime.toString("yyyy-MM-dd hh:mm"));

        hourlyMeteoMaps->setComputed(false);

        // meteo interpolation
        if (! interpolateAndSaveHourlyMeteo(airTemperature, myTime, outputPath, saveOutput)) return false;
        if (! interpolateAndSaveHourlyMeteo(precipitation, myTime, outputPath, saveOutput)) return false;
        if (! interpolateAndSaveHourlyMeteo(airRelHumidity, myTime, outputPath, saveOutput)) return false;
        if (! interpolateAndSaveHourlyMeteo(windIntensity, myTime, outputPath, saveOutput)) return false;

        // radiation model
        if (! interpolateAndSaveHourlyMeteo(globalIrradiance, myTime, outputPath, saveOutput)) return false;

        // ET0
        if (! hourlyMeteoMaps->computeET0PMMap(DEM, radiationMaps)) return false;
        if (saveOutput)
        {
            saveHourlyMeteoOutput(referenceEvapotranspiration, outputPath, myTime);
        }
        hourlyMeteoMaps->setComputed(true);

    }

    return true;
}


bool Crit3DProject::runModels(QDateTime firstTime, QDateTime lastTime, bool saveOutput)
{
    if (! isCriteria3DInitialized)
    {
        logError("Initialize 3d model before.");
        return false;
    }

    if (lastTime < firstTime)
    {
        logError("Wrong date");
        return false;
    }

    logInfo("\nRun models from: " + firstTime.toString() + " to: " + lastTime.toString());

    QDate firstDate = firstTime.date();
    QDate lastDate = lastTime.date();
    logInfo("Load meteo data");
    if (! loadMeteoPointsData(firstDate.addDays(-1), lastDate.addDays(+1), false))
    {
        logError();
        return false;
    }

    int hour1 = firstTime.time().hour();
    int hour2 = lastTime.time().hour();

    bool isInitialState;
    int firstHour, lastHour;
    QDir myDir;
    QString outputPathHourly, outputPathDaily;

    for (QDate myDate = firstDate; myDate <= lastDate; myDate = myDate.addDays(1))
    {
        // load state
        isInitialState = false;
        if (myDate == firstDate)
        {
            firstHour = hour1;
            /*if (loadStates(previousDate))
            {
                isInitialState = false;
            }
            else
            {
                logInfo("State not found.");
                isInitialState = true;
            }*/
            isInitialState = true;
        }
        else firstHour = 0;

        if (myDate == lastDate)
            lastHour = hour2;
        else
            lastHour = 23;

        if (saveOutput)
        {
            //create output directories
            outputPathDaily = getProjectPath() + "output/daily/" + myDate.toString("yyyy/MM/dd/");
            outputPathHourly = getProjectPath() + "output/hourly/" + myDate.toString("yyyy/MM/dd/");

            if ((! myDir.mkpath(outputPathDaily)) || (! myDir.mkpath(outputPathHourly)))
            {
                this->logError("Creation output directories failed." );
                saveOutput = false;
            }
        }

        if (! modelDailyCycle(isInitialState, myDate, firstHour, lastHour, outputPathHourly, saveOutput))
        {
            logError(errorString);
            return false;
        }

        if (lastHour >= 23)
        {
            /*if (saveOutput)
            {
                this->logInfo("Aggregate daily meteo data");
                aggregateAndSaveDailyMap(this, airTemperature, aggregationMin, getCrit3DDate(myDate), myOutputPathDaily, myOutputPathHourly, myArea);
                aggregateAndSaveDailyMap(this, airTemperature, aggregationMax, getCrit3DDate(myDate), myOutputPathDaily, myOutputPathHourly, myArea);
                aggregateAndSaveDailyMap(this, airTemperature, aggregationMean, getCrit3DDate(myDate), myOutputPathDaily,myOutputPathHourly, myArea);
                aggregateAndSaveDailyMap(this, precipitation, aggregationSum, getCrit3DDate(myDate), myOutputPathDaily, myOutputPathHourly, myArea);
                aggregateAndSaveDailyMap(this, referenceEvapotranspiration, aggregationSum, getCrit3DDate(myDate), myOutputPathDaily, myOutputPathHourly, myArea);
                aggregateAndSaveDailyMap(this, airRelHumidity, aggregationMin, getCrit3DDate(myDate), myOutputPathDaily, myOutputPathHourly, myArea);
                aggregateAndSaveDailyMap(this, airRelHumidity, aggregationMax, getCrit3DDate(myDate), myOutputPathDaily, myOutputPathHourly, myArea);
                aggregateAndSaveDailyMap(this, airRelHumidity, aggregationMean, getCrit3DDate(myDate), myOutputPathDaily, myOutputPathHourly, myArea);
                aggregateAndSaveDailyMap(this, globalIrradiance, aggregationIntegration, getCrit3DDate(myDate), myOutputPathDaily, myOutputPathHourly, myArea);

                if (removeDirectory(outputPathHourly)) this->logInfo("Delete hourly files");
            }*/

            // save state and output
            //if (! saveStateAndOutput(myDate)) return false;
        }
    }

    logInfo("End of run.");
    return true;
}


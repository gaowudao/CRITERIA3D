#include "meteoMaps.h"

void Crit3DMeteoMaps::initializeMaps()
{
    airTemperatureMap = new gis::Crit3DRasterGrid;
    precipitationMap = new gis::Crit3DRasterGrid;
    airHumidityMap = new gis::Crit3DRasterGrid;
    airDewTemperatureMap = new gis::Crit3DRasterGrid;
    windIntensityMap = new gis::Crit3DRasterGrid;
    avgDailyTemperature = new gis::Crit3DRasterGrid;
    leafWetnessMap = new gis::Crit3DRasterGrid;
    ET0Map = new gis::Crit3DRasterGrid;
    evaporationMap = new gis::Crit3DRasterGrid;
    irrigationMap = new gis::Crit3DRasterGrid;
}

Crit3DMeteoMaps::Crit3DMeteoMaps()
{
    this->initializeMaps();
    radiationMaps = new Crit3DRadiationMaps();

    isLoaded = false;
}

Crit3DMeteoMaps::Crit3DMeteoMaps(const gis::Crit3DRasterGrid& dtmGrid, const gis::Crit3DGisSettings& gisSettings)
{
    this->initializeMaps();

    airTemperatureMap->initializeGrid(dtmGrid);
    precipitationMap->initializeGrid(dtmGrid);
    airHumidityMap->initializeGrid(dtmGrid);
    airDewTemperatureMap->initializeGrid(dtmGrid);
    windIntensityMap->initializeGrid(dtmGrid);
    avgDailyTemperature->initializeGrid(dtmGrid);
    leafWetnessMap->initializeGrid(dtmGrid);
    ET0Map->initializeGrid(dtmGrid);
    evaporationMap->initializeGrid(dtmGrid);
    irrigationMap->initializeGrid(dtmGrid);

    radiationMaps = new Crit3DRadiationMaps(dtmGrid, gisSettings);

    isLoaded = true;
}

Crit3DMeteoMaps::~Crit3DMeteoMaps()
{
    if (isLoaded)
    {      
        airTemperatureMap->freeGrid();
        precipitationMap->freeGrid();
        airHumidityMap->freeGrid();
        airDewTemperatureMap->freeGrid();
        windIntensityMap->freeGrid();
        avgDailyTemperature->freeGrid();
        leafWetnessMap->freeGrid();
        ET0Map->freeGrid();
        evaporationMap->freeGrid();
        irrigationMap->freeGrid();

        delete airTemperatureMap;
        delete precipitationMap;
        delete airHumidityMap;
        delete airDewTemperatureMap;
        delete windIntensityMap;
        delete leafWetnessMap;
        delete ET0Map;
        delete evaporationMap;
        delete avgDailyTemperature;
        delete irrigationMap;

        delete radiationMaps;

        isLoaded = false;
    }
}

gis::Crit3DRasterGrid* Crit3DMeteoMaps::getMapFromVar(meteoVariable myVar)
{
    if (myVar == airTemperature)
        return airTemperatureMap;
    else if (myVar == dailyAirTemperatureAvg)
        return avgDailyTemperature;
    else if (myVar == precipitation)
        return precipitationMap;
    else if (myVar == airHumidity)
        return airHumidityMap;
    else if (myVar == dailyAirHumidityAvg)
        //todo: aggiungere mappa daily
        return airHumidityMap;
    else if (myVar == airDewTemperature)
        return airDewTemperatureMap;
    else if (myVar == windIntensity)
        return windIntensityMap;
    else if (myVar == globalIrradiance)
        return this->radiationMaps->globalRadiationMap;
    else if (myVar == directIrradiance)
        return this->radiationMaps->beamRadiationMap;
    else if (myVar == diffuseIrradiance)
        return this->radiationMaps->diffuseRadiationMap;
    else if (myVar == reflectedIrradiance)
        return this->radiationMaps->reflectedRadiationMap;
    else if (myVar == potentialEvapotranspiration)
        return ET0Map;
    else if (myVar == realEvaporation)
        return evaporationMap;
    else if (myVar == wetnessDuration)
        return leafWetnessMap;
    else
        return NULL;
}

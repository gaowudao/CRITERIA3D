#include "meteoMaps.h"

void Crit3DMeteoMaps::initializeMaps()
{
    airTemperatureMap = new gis::Crit3DRasterGrid;
    precipitationMap = new gis::Crit3DRasterGrid;
    airRelHumidityMap = new gis::Crit3DRasterGrid;
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

    isLoaded = false;
}

Crit3DMeteoMaps::Crit3DMeteoMaps(const gis::Crit3DRasterGrid& dtmGrid, const gis::Crit3DGisSettings& gisSettings)
{
    this->initializeMaps();

    airTemperatureMap->initializeGrid(dtmGrid);
    precipitationMap->initializeGrid(dtmGrid);
    airRelHumidityMap->initializeGrid(dtmGrid);
    airDewTemperatureMap->initializeGrid(dtmGrid);
    windIntensityMap->initializeGrid(dtmGrid);
    avgDailyTemperature->initializeGrid(dtmGrid);
    leafWetnessMap->initializeGrid(dtmGrid);
    ET0Map->initializeGrid(dtmGrid);
    evaporationMap->initializeGrid(dtmGrid);
    irrigationMap->initializeGrid(dtmGrid);

    isLoaded = true;
}

Crit3DMeteoMaps::~Crit3DMeteoMaps()
{
    if (isLoaded)
    {      
        airTemperatureMap->freeGrid();
        precipitationMap->freeGrid();
        airRelHumidityMap->freeGrid();
        airDewTemperatureMap->freeGrid();
        windIntensityMap->freeGrid();
        avgDailyTemperature->freeGrid();
        leafWetnessMap->freeGrid();
        ET0Map->freeGrid();
        evaporationMap->freeGrid();
        irrigationMap->freeGrid();

        delete airTemperatureMap;
        delete precipitationMap;
        delete airRelHumidityMap;
        delete airDewTemperatureMap;
        delete windIntensityMap;
        delete leafWetnessMap;
        delete ET0Map;
        delete evaporationMap;
        delete avgDailyTemperature;
        delete irrigationMap;

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
    else if (myVar == airRelHumidity)
        return airRelHumidityMap;
    else if (myVar == dailyAirRelHumidityAvg)
        //todo: aggiungere mappa daily
        return airRelHumidityMap;
    else if (myVar == airDewTemperature)
        return airDewTemperatureMap;
    else if (myVar == windIntensity)
        return windIntensityMap;
    else if (myVar == referenceEvapotranspiration)
        return ET0Map;
    else if (myVar == actualEvaporation)
        return evaporationMap;
    else if (myVar == leafWetness)
        return leafWetnessMap;
    else
        return nullptr;
}

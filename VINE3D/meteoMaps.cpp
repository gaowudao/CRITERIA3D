#include "meteoMaps.h"


void Crit3DMeteoMaps::initialize()
{
    airTemperatureMap = new gis::Crit3DRasterGrid;
    precipitationMap = new gis::Crit3DRasterGrid;
    airRelHumidityMap = new gis::Crit3DRasterGrid;
    airDewTemperatureMap = new gis::Crit3DRasterGrid;
    windIntensityMap = new gis::Crit3DRasterGrid;
    leafWetnessMap = new gis::Crit3DRasterGrid;
    ET0Map = new gis::Crit3DRasterGrid;
    irrigationMap = new gis::Crit3DRasterGrid;
    avgDailyTemperatureMap = new gis::Crit3DRasterGrid;
}


Crit3DMeteoMaps::Crit3DMeteoMaps()
{
    this->initialize();

    isInitialized = false;
    isComputed = false;
}


Crit3DMeteoMaps::Crit3DMeteoMaps(const gis::Crit3DRasterGrid& dtmGrid)
{
    this->initialize();

    airTemperatureMap->initializeGrid(dtmGrid);
    precipitationMap->initializeGrid(dtmGrid);
    airRelHumidityMap->initializeGrid(dtmGrid);
    airDewTemperatureMap->initializeGrid(dtmGrid);
    windIntensityMap->initializeGrid(dtmGrid);
    avgDailyTemperatureMap->initializeGrid(dtmGrid);
    leafWetnessMap->initializeGrid(dtmGrid);
    ET0Map->initializeGrid(dtmGrid);
    irrigationMap->initializeGrid(dtmGrid);

    isInitialized = true;
    isComputed = false;
}


void Crit3DMeteoMaps::clean()
{
    airTemperatureMap->emptyGrid();
    precipitationMap->emptyGrid();
    airRelHumidityMap->emptyGrid();
    airDewTemperatureMap->emptyGrid();
    leafWetnessMap->emptyGrid();
    ET0Map->emptyGrid();
    windIntensityMap->emptyGrid();
    irrigationMap->emptyGrid();

    isComputed = false;
}


Crit3DMeteoMaps::~Crit3DMeteoMaps()
{
    if (isInitialized)
    {      
        airTemperatureMap->freeGrid();
        precipitationMap->freeGrid();
        airRelHumidityMap->freeGrid();
        airDewTemperatureMap->freeGrid();
        windIntensityMap->freeGrid();
        leafWetnessMap->freeGrid();
        ET0Map->freeGrid();
        irrigationMap->freeGrid();
        avgDailyTemperatureMap->freeGrid();

        isInitialized = false;
        isComputed = false;
    }
}


gis::Crit3DRasterGrid* Crit3DMeteoMaps::getMapFromVar(meteoVariable myVar)
{
    if (myVar == airTemperature)
        return airTemperatureMap;
    else if (myVar == dailyAirTemperatureAvg)
        return avgDailyTemperatureMap;
    else if (myVar == precipitation)
        return precipitationMap;
    else if (myVar == airRelHumidity)
        return airRelHumidityMap;
    else if (myVar == dailyAirRelHumidityAvg)
        return airRelHumidityMap;
    else if (myVar == airDewTemperature)
        return airDewTemperatureMap;
    else if (myVar == windIntensity)
        return windIntensityMap;
    else if (myVar == referenceEvapotranspiration)
        return ET0Map;
    else if (myVar == leafWetness)
        return leafWetnessMap;
    else
        return nullptr;
}

#include "meteoMaps.h"


Crit3DMeteoMaps::Crit3DMeteoMaps(const gis::Crit3DRasterGrid& dtmGrid)
{
    airTemperatureMap = new gis::Crit3DRasterGrid;
    precipitationMap = new gis::Crit3DRasterGrid;
    airRelHumidityMap = new gis::Crit3DRasterGrid;
    windIntensityMap = new gis::Crit3DRasterGrid;
    leafWetnessMap = new gis::Crit3DRasterGrid;
    ET0Map = new gis::Crit3DRasterGrid;
    irrigationMap = new gis::Crit3DRasterGrid;
    avgDailyTemperatureMap = new gis::Crit3DRasterGrid;

    airTemperatureMap->initializeGrid(dtmGrid);
    precipitationMap->initializeGrid(dtmGrid);
    airRelHumidityMap->initializeGrid(dtmGrid);
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
        windIntensityMap->freeGrid();
        leafWetnessMap->freeGrid();
        ET0Map->freeGrid();
        irrigationMap->freeGrid();
        avgDailyTemperatureMap->freeGrid();
    }
}


gis::Crit3DRasterGrid* Crit3DMeteoMaps::getMapFromVar(meteoVariable myVar)
{
    if (myVar == airTemperature)
        return airTemperatureMap;
    else if (myVar == precipitation)
        return precipitationMap;
    else if (myVar == airRelHumidity)
        return airRelHumidityMap;
    else if (myVar == windIntensity)
        return windIntensityMap;
    else if (myVar == referenceEvapotranspiration)
        return ET0Map;
    else if (myVar == leafWetness)
        return leafWetnessMap;
    else if (myVar == dailyAirTemperatureAvg)
        return avgDailyTemperatureMap;
    else
        return nullptr;
}


bool Crit3DMeteoMaps::computeET0Map(gis::Crit3DRasterGrid* DTM, Crit3DRadiationMaps *radMaps)
{
    float globalRadiation, transmissivity, clearSkyTransmissivity;
    float temperature, relHumidity, windSpeed, height;

    for (long row = 0; row < this->ET0Map->header->nrRows; row++)
        for (long col = 0; col < this->ET0Map->header->nrCols; col++)
        {
            this->ET0Map->value[row][col] = this->ET0Map->header->flag;

            height = DTM->value[row][col];

            if (int(height) != int(DTM->header->flag))
            {
                clearSkyTransmissivity = CLEAR_SKY_TRANSMISSIVITY_DEFAULT;
                globalRadiation = radMaps->globalRadiationMap->value[row][col];
                transmissivity = radMaps->transmissivityMap->value[row][col];
                temperature = this->airTemperatureMap->value[row][col];
                relHumidity = this->airRelHumidityMap->value[row][col];
                windSpeed = this->windIntensityMap->value[row][col];

                if (globalRadiation != radMaps->globalRadiationMap->header->flag
                        && transmissivity != radMaps->transmissivityMap->header->flag
                        && temperature != this->airTemperatureMap->header->flag
                        && relHumidity != this->airRelHumidityMap->header->flag
                        && windSpeed != this->windIntensityMap->header->flag)
                {
                    this->ET0Map->value[row][col] = ET0_Penman_hourly(height, transmissivity / clearSkyTransmissivity,
                                      globalRadiation, temperature, relHumidity, windSpeed);
                }
            }
        }

    return gis::updateMinMaxRasterGrid(this->ET0Map);
}


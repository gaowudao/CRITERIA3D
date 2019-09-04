#include "basicMath.h"
#include "meteoMaps.h"


Crit3DHourlyMeteoMaps::Crit3DHourlyMeteoMaps(const gis::Crit3DRasterGrid& DEM)
{
    airTemperatureMap = new gis::Crit3DRasterGrid;
    precipitationMap = new gis::Crit3DRasterGrid;
    airRelHumidityMap = new gis::Crit3DRasterGrid;
    windIntensityMap = new gis::Crit3DRasterGrid;
    leafWetnessMap = new gis::Crit3DRasterGrid;
    ET0Map = new gis::Crit3DRasterGrid;
    //irrigationMap = new gis::Crit3DRasterGrid;
    //avgDailyTemperatureMap = new gis::Crit3DRasterGrid;

    airTemperatureMap->initializeGrid(DEM);
    precipitationMap->initializeGrid(DEM);
    airRelHumidityMap->initializeGrid(DEM);
    windIntensityMap->initializeGrid(DEM);
    //avgDailyTemperatureMap->initializeGrid(DEM);
    leafWetnessMap->initializeGrid(DEM);
    ET0Map->initializeGrid(DEM);
    //irrigationMap->initializeGrid(DEM);

    isInitialized = true;
    isComputed = false;
}


void Crit3DHourlyMeteoMaps::clean()
{
    airTemperatureMap->emptyGrid();
    precipitationMap->emptyGrid();
    airRelHumidityMap->emptyGrid();
    leafWetnessMap->emptyGrid();
    ET0Map->emptyGrid();
    windIntensityMap->emptyGrid();
    //irrigationMap->emptyGrid();

    isComputed = false;
}


Crit3DHourlyMeteoMaps::~Crit3DHourlyMeteoMaps()
{
    if (isInitialized)
    {
        airTemperatureMap->clear();
        precipitationMap->clear();
        airRelHumidityMap->clear();
        windIntensityMap->clear();
        leafWetnessMap->clear();
        ET0Map->clear();
        //irrigationMap->clear();
        //avgDailyTemperatureMap->clear();
    }
}


gis::Crit3DRasterGrid* Crit3DHourlyMeteoMaps::getMapFromVar(meteoVariable myVar)
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
    //else if (myVar == dailyAirTemperatureAvg)
        //return avgDailyTemperatureMap;
    else if (myVar == dailyAirRelHumidityAvg)
        return airRelHumidityMap;
    else
        return nullptr;
}


bool Crit3DHourlyMeteoMaps::computeET0Map(gis::Crit3DRasterGrid* myDEM, Crit3DRadiationMaps *radMaps)
{
    float globalRadiation, transmissivity, clearSkyTransmissivity;
    float temperature, relHumidity, windSpeed, height;

    for (long row = 0; row < this->ET0Map->header->nrRows; row++)
        for (long col = 0; col < this->ET0Map->header->nrCols; col++)
        {
            this->ET0Map->value[row][col] = this->ET0Map->header->flag;

            height = myDEM->value[row][col];
            if (int(height) != int(myDEM->header->flag))
            {
                clearSkyTransmissivity = CLEAR_SKY_TRANSMISSIVITY_DEFAULT;
                globalRadiation = radMaps->globalRadiationMap->value[row][col];
                transmissivity = radMaps->transmissivityMap->value[row][col];
                temperature = this->airTemperatureMap->value[row][col];
                relHumidity = this->airRelHumidityMap->value[row][col];
                windSpeed = this->windIntensityMap->value[row][col];

                if (! isEqual(globalRadiation, radMaps->globalRadiationMap->header->flag)
                        && ! isEqual(transmissivity, radMaps->transmissivityMap->header->flag)
                        && ! isEqual(temperature, airTemperatureMap->header->flag)
                        && ! isEqual(relHumidity, airRelHumidityMap->header->flag)
                        && ! isEqual(windSpeed, windIntensityMap->header->flag))
                {
                    this->ET0Map->value[row][col] = float(ET0_Penman_hourly(double(height), double(transmissivity / clearSkyTransmissivity),
                                      double(globalRadiation), double(temperature), double(relHumidity), double(windSpeed)));
                }
            }
        }

    return gis::updateMinMaxRasterGrid(this->ET0Map);
}


bool Crit3DHourlyMeteoMaps::computeLeafWetnessMap(gis::Crit3DRasterGrid* myDEM)
{
    float relHumidity, precipitation, leafWetness;

    for (long row = 0; row < leafWetnessMap->header->nrRows; row++)
        for (long col = 0; col < leafWetnessMap->header->nrCols; col++)
        {
            //initialize
            leafWetnessMap->value[row][col] = leafWetnessMap->header->flag;

            if (! isEqual(myDEM->value[row][col], myDEM->header->flag))
            {
                relHumidity = airRelHumidityMap->value[row][col];
                precipitation = precipitationMap->value[row][col];

                if (! isEqual(relHumidity, airRelHumidityMap->header->flag)
                        && ! isEqual(precipitation, precipitationMap->header->flag))
                {
                    leafWetness = 0;
                    if (precipitation > 0 || relHumidity > 92)
                    {
                        leafWetness = 1;
                    }
                    //TODO: ora precedente prec > 2mm ?

                    leafWetnessMap->value[row][col] = leafWetness;
                }
            }
        }

    return gis::updateMinMaxRasterGrid(leafWetnessMap);
}



bool Crit3DHourlyMeteoMaps::computeRelativeHumidityMap(const gis::Crit3DRasterGrid& dewTemperatureMap)
{
    float airT, dewT;
    airRelHumidityMap->emptyGrid();

    if (! dewTemperatureMap.header->isEqualTo(*(airTemperatureMap->header)))
        return false;

    for (long row = 0; row < airRelHumidityMap->header->nrRows ; row++)
    {
        for (long col = 0; col < airRelHumidityMap->header->nrCols; col++)
        {
            airT = airTemperatureMap->value[row][col];
            dewT = dewTemperatureMap.value[row][col];
            if (! isEqual(airT, airTemperatureMap->header->flag)
                 && ! isEqual(dewT, dewTemperatureMap.header->flag))
            {
                    airRelHumidityMap->value[row][col] = relHumFromTdew(dewT, airT);
            }
        }
    }

    return gis::updateMinMaxRasterGrid(airRelHumidityMap);
}




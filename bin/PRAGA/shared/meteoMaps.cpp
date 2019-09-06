#include "basicMath.h"
#include "meteoMaps.h"


bool Crit3DDailyMeteoMaps::getIsInitialized() const
{
    return isInitialized;
}

void Crit3DDailyMeteoMaps::setIsInitialized(bool value)
{
    isInitialized = value;
}

Crit3DDailyMeteoMaps::Crit3DDailyMeteoMaps(const gis::Crit3DRasterGrid& DEM)
{
    mapDailyTAvg = new gis::Crit3DRasterGrid;
    mapDailyTMax = new gis::Crit3DRasterGrid;
    mapDailyTMin = new gis::Crit3DRasterGrid;
    mapDailyPrec = new gis::Crit3DRasterGrid;
    mapDailyRHAvg = new gis::Crit3DRasterGrid;
    mapDailyRHMin = new gis::Crit3DRasterGrid;
    mapDailyRHMax = new gis::Crit3DRasterGrid;
    mapDailyLeafW = new gis::Crit3DRasterGrid;

    mapDailyTAvg->initializeGrid(DEM);
    mapDailyTMax->initializeGrid(DEM);
    mapDailyTMin->initializeGrid(DEM);
    mapDailyPrec->initializeGrid(DEM);
    mapDailyRHAvg->initializeGrid(DEM);
    mapDailyRHMin->initializeGrid(DEM);
    mapDailyRHMax->initializeGrid(DEM);
    mapDailyLeafW->initializeGrid(DEM);

    isInitialized = true;
}

void Crit3DDailyMeteoMaps::clean()
{
    mapDailyTAvg->emptyGrid();
    mapDailyTMax->emptyGrid();
    mapDailyTMin->emptyGrid();
    mapDailyPrec->emptyGrid();
    mapDailyRHAvg->emptyGrid();
    mapDailyRHMin->emptyGrid();
    mapDailyRHMax->emptyGrid();
    mapDailyLeafW->emptyGrid();
}

Crit3DDailyMeteoMaps::~Crit3DDailyMeteoMaps()
{
    if (isInitialized)
    {
        mapDailyTAvg->clear();
        mapDailyTMax->clear();
        mapDailyTMin->clear();
        mapDailyPrec->clear();
        mapDailyRHAvg->clear();
        mapDailyRHMin->clear();
        mapDailyRHMax->clear();
        mapDailyLeafW->clear();
    }
}

bool Crit3DHourlyMeteoMaps::getIsInitialized() const
{
    return isInitialized;
}

void Crit3DHourlyMeteoMaps::setIsInitialized(bool value)
{
    isInitialized = value;
}

gis::Crit3DRasterGrid* Crit3DDailyMeteoMaps::getMapFromVar(meteoVariable myVar)
{
    if (myVar == dailyAirTemperatureAvg)
        return mapDailyTAvg;
    else if (myVar == dailyAirTemperatureMax)
        return mapDailyTMax;
    else if (myVar == dailyAirTemperatureMin)
        return mapDailyTMin;
    else if (myVar == dailyPrecipitation)
        return mapDailyPrec;
    else if (myVar == dailyAirRelHumidityAvg)
        return mapDailyRHAvg;
    else if (myVar == dailyAirRelHumidityMax)
        return mapDailyRHMax;
    else if (myVar == dailyAirRelHumidityMin)
        return mapDailyRHMin;
    else if (myVar == dailyLeafWetness)
        return mapDailyLeafW;
    else
        return nullptr;
}

Crit3DHourlyMeteoMaps::Crit3DHourlyMeteoMaps(const gis::Crit3DRasterGrid& DEM)
{
    mapHourlyT = new gis::Crit3DRasterGrid;
    mapHourlyPrec = new gis::Crit3DRasterGrid;
    mapHourlyRelHum = new gis::Crit3DRasterGrid;
    mapHourlyWindInt = new gis::Crit3DRasterGrid;
    mapHourlyLeafW = new gis::Crit3DRasterGrid;
    mapHourlyET0 = new gis::Crit3DRasterGrid;
    mapHourlyTdew = new gis::Crit3DRasterGrid;
    mapHourlyWindDir = new gis::Crit3DRasterGrid;

    mapHourlyT->initializeGrid(DEM);
    mapHourlyPrec->initializeGrid(DEM);
    mapHourlyRelHum->initializeGrid(DEM);
    mapHourlyWindInt->initializeGrid(DEM);
    mapHourlyLeafW->initializeGrid(DEM);
    mapHourlyET0->initializeGrid(DEM);
    mapHourlyTdew->initializeGrid(DEM);
    mapHourlyWindDir->initializeGrid(DEM);

    isInitialized = true;
    isComputed = false;
}


void Crit3DHourlyMeteoMaps::clean()
{
    mapHourlyT->emptyGrid();
    mapHourlyPrec->emptyGrid();
    mapHourlyRelHum->emptyGrid();
    mapHourlyWindInt->emptyGrid();
    mapHourlyET0->emptyGrid();
    mapHourlyWindInt->emptyGrid();
    mapHourlyWindDir->emptyGrid();
    mapHourlyLeafW->emptyGrid();
    //irrigationMap->emptyGrid();

    isComputed = false;
}


Crit3DHourlyMeteoMaps::~Crit3DHourlyMeteoMaps()
{
    if (isInitialized)
    {
        mapHourlyT->clear();
        mapHourlyPrec->clear();
        mapHourlyRelHum->clear();
        mapHourlyWindInt->clear();
        mapHourlyLeafW->clear();
        mapHourlyWindInt->clear();
        mapHourlyWindDir->clear();
        mapHourlyET0->clear();
        //irrigationMap->clear();
        //avgDailyTemperatureMap->clear();
    }
}


gis::Crit3DRasterGrid* Crit3DHourlyMeteoMaps::getMapFromVar(meteoVariable myVar)
{
    if (myVar == airTemperature)
        return mapHourlyT;
    else if (myVar == precipitation)
        return mapHourlyPrec;
    else if (myVar == airRelHumidity)
        return mapHourlyRelHum;
    else if (myVar == windIntensity)
        return mapHourlyWindInt;
    else if (myVar == referenceEvapotranspiration)
        return mapHourlyET0;
    else if (myVar == leafWetness)
        return mapHourlyLeafW;
    //else if (myVar == dailyAirTemperatureAvg)
        //return avgDailyTemperatureMap;
    else if (myVar == windDirection)
        return mapHourlyWindDir;
    else if (myVar == airDewTemperature)
        return mapHourlyTdew;
    else
        return nullptr;
}


bool Crit3DHourlyMeteoMaps::computeET0Map(gis::Crit3DRasterGrid* myDEM, Crit3DRadiationMaps *radMaps)
{
    float globalRadiation, transmissivity, clearSkyTransmissivity;
    float temperature, relHumidity, windSpeed, height;

    for (long row = 0; row < this->mapHourlyET0->header->nrRows; row++)
        for (long col = 0; col < this->mapHourlyET0->header->nrCols; col++)
        {
            this->mapHourlyET0->value[row][col] = this->mapHourlyET0->header->flag;

            height = myDEM->value[row][col];
            if (int(height) != int(myDEM->header->flag))
            {
                clearSkyTransmissivity = CLEAR_SKY_TRANSMISSIVITY_DEFAULT;
                globalRadiation = radMaps->globalRadiationMap->value[row][col];
                transmissivity = radMaps->transmissivityMap->value[row][col];
                temperature = this->mapHourlyT->value[row][col];
                relHumidity = this->mapHourlyRelHum->value[row][col];
                windSpeed = this->mapHourlyWindInt->value[row][col];

                if (! isEqual(globalRadiation, radMaps->globalRadiationMap->header->flag)
                        && ! isEqual(transmissivity, radMaps->transmissivityMap->header->flag)
                        && ! isEqual(temperature, mapHourlyT->header->flag)
                        && ! isEqual(relHumidity, mapHourlyRelHum->header->flag)
                        && ! isEqual(windSpeed, mapHourlyWindInt->header->flag))
                {
                    this->mapHourlyET0->value[row][col] = float(ET0_Penman_hourly(double(height), double(transmissivity / clearSkyTransmissivity),
                                      double(globalRadiation), double(temperature), double(relHumidity), double(windSpeed)));
                }
            }
        }

    return gis::updateMinMaxRasterGrid(this->mapHourlyET0);
}


bool Crit3DHourlyMeteoMaps::computeLeafWetnessMap(gis::Crit3DRasterGrid* myDEM)
{
    float relHumidity, precipitation, leafWetness;

    for (long row = 0; row < mapHourlyLeafW->header->nrRows; row++)
        for (long col = 0; col < mapHourlyLeafW->header->nrCols; col++)
        {
            //initialize
            mapHourlyLeafW->value[row][col] = mapHourlyLeafW->header->flag;

            if (! isEqual(myDEM->value[row][col], myDEM->header->flag))
            {
                relHumidity = mapHourlyRelHum->value[row][col];
                precipitation = mapHourlyPrec->value[row][col];

                if (! isEqual(relHumidity, mapHourlyRelHum->header->flag)
                        && ! isEqual(precipitation, mapHourlyPrec->header->flag))
                {
                    leafWetness = 0;
                    if (precipitation > 0 || relHumidity > 92)
                    {
                        leafWetness = 1;
                    }
                    //TODO: ora precedente prec > 2mm ?

                    mapHourlyLeafW->value[row][col] = leafWetness;
                }
            }
        }

    return gis::updateMinMaxRasterGrid(mapHourlyLeafW);
}



bool Crit3DHourlyMeteoMaps::computeRelativeHumidityMap()
{
    float airT, dewT;
    mapHourlyRelHum->emptyGrid();

    for (long row = 0; row < mapHourlyRelHum->header->nrRows ; row++)
    {
        for (long col = 0; col < mapHourlyRelHum->header->nrCols; col++)
        {
            airT = mapHourlyT->value[row][col];
            dewT = mapHourlyTdew->value[row][col];
            if (! isEqual(airT, mapHourlyT->header->flag)
                 && ! isEqual(dewT, mapHourlyTdew->header->flag))
            {
                    mapHourlyRelHum->value[row][col] = relHumFromTdew(dewT, airT);
            }
        }
    }

    return gis::updateMinMaxRasterGrid(mapHourlyRelHum);
}




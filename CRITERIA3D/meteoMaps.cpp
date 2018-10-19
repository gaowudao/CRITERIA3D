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

#include "meteoMaps.h"


Crit3DMeteoMaps::Crit3DMeteoMaps()
{
    airTemperatureMap = new gis::Crit3DRasterGrid;
    precipitationMap = new gis::Crit3DRasterGrid;
    airHumidityMap = new gis::Crit3DRasterGrid;
    windIntensityMap = new gis::Crit3DRasterGrid;
    ET0Map = new gis::Crit3DRasterGrid;

    isInitialized = false;
}


Crit3DMeteoMaps::Crit3DMeteoMaps(const gis::Crit3DRasterGrid& rasterGrid)
{
    airTemperatureMap = new gis::Crit3DRasterGrid;
    precipitationMap = new gis::Crit3DRasterGrid;
    airHumidityMap = new gis::Crit3DRasterGrid;
    windIntensityMap = new gis::Crit3DRasterGrid;
    ET0Map = new gis::Crit3DRasterGrid;

    airTemperatureMap->initializeGrid(rasterGrid);
    precipitationMap->initializeGrid(rasterGrid);
    airHumidityMap->initializeGrid(rasterGrid);
    windIntensityMap->initializeGrid(rasterGrid);
    ET0Map->initializeGrid(rasterGrid);

    isInitialized = true;
}


Crit3DMeteoMaps::~Crit3DMeteoMaps()
{
    if (isInitialized)
    {      
        airTemperatureMap->freeGrid();
        precipitationMap->freeGrid();
        airHumidityMap->freeGrid();
        windIntensityMap->freeGrid();
        ET0Map->freeGrid();

        isInitialized = false;
    }
}


void Crit3DMeteoMaps::emptyMeteoMaps()
{
    airTemperatureMap->emptyGrid();
    precipitationMap->emptyGrid();
    airHumidityMap->emptyGrid();
    windIntensityMap->emptyGrid();
    ET0Map->emptyGrid();
}


gis::Crit3DRasterGrid* Crit3DMeteoMaps::getMapFromVar(meteoVariable myVar)
{
    if (myVar == airTemperature)
        return airTemperatureMap;
    else if (myVar == precipitation)
        return precipitationMap;
    else if (myVar == airRelHumidity)
        return airHumidityMap;
    else if (myVar == windIntensity)
        return windIntensityMap;
    else if (myVar == referenceEvapotranspiration)
        return ET0Map;
    else
        return NULL;
}


bool Crit3DMeteoMaps::computeET0Map(gis::Crit3DRasterGrid* dtm, Crit3DRadiationMaps *radMaps)
{
    float globalRadiation, transmissivity, clearSkyTransmissivity;
    float temperature, relHumidity, windSpeed, height;

    gis::Crit3DRasterGrid* map = ET0Map;

    for (long row = 0; row < map->header->nrRows; row++)
        for (long col = 0; col < map->header->nrCols; col++)
        {
            map->value[row][col] = map->header->flag;

            height = dtm->value[row][col];

            if (height != dtm->header->flag)
            {
                globalRadiation = radMaps->globalRadiationMap->value[row][col];
                transmissivity = radMaps->transmissivityMap->value[row][col];
                clearSkyTransmissivity = radMaps->clearSkyTransmissivityMap->value[row][col];
                temperature = airTemperatureMap->value[row][col];
                relHumidity = airHumidityMap->value[row][col];
                windSpeed = windIntensityMap->value[row][col];

                if (globalRadiation != radMaps->globalRadiationMap->header->flag
                        && transmissivity != radMaps->transmissivityMap->header->flag
                        && clearSkyTransmissivity != radMaps->clearSkyTransmissivityMap->header->flag
                        && temperature != airTemperatureMap->header->flag
                        && relHumidity != airHumidityMap->header->flag
                        && windSpeed != windIntensityMap->header->flag)
                {
                    map->value[row][col] = float(ET0_Penman_hourly(height, transmissivity / clearSkyTransmissivity,
                                      globalRadiation, temperature, relHumidity, windSpeed));
                }
            }
        }

    return gis::updateMinMaxRasterGrid(map);
}

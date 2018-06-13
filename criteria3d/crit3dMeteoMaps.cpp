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

#include "crit3dMeteoMaps.h"


Crit3DMeteoMaps::Crit3DMeteoMaps()
{
    airTemperatureMap = new gis::Crit3DRasterGrid;
    precipitationMap = new gis::Crit3DRasterGrid;
    airHumidityMap = new gis::Crit3DRasterGrid;
    windIntensityMap = new gis::Crit3DRasterGrid;
    ET0Map = new gis::Crit3DRasterGrid;
    irrigationMap = new gis::Crit3DRasterGrid;

    isInitialized = false;
}


Crit3DMeteoMaps::Crit3DMeteoMaps(const gis::Crit3DRasterGrid& rasterGrid)
{
    airTemperatureMap->initializeGrid(rasterGrid);
    precipitationMap->initializeGrid(rasterGrid);
    airHumidityMap->initializeGrid(rasterGrid);
    windIntensityMap->initializeGrid(rasterGrid);
    ET0Map->initializeGrid(rasterGrid);
    irrigationMap->initializeGrid(rasterGrid);

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
        irrigationMap->freeGrid();

        delete airTemperatureMap;
        delete precipitationMap;
        delete airHumidityMap;
        delete windIntensityMap;
        delete ET0Map;
        delete irrigationMap;

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
    irrigationMap->emptyGrid();
}

gis::Crit3DRasterGrid* Crit3DMeteoMaps::getMapFromVar(meteoVariable myVar)
{
    if (myVar == airTemperature)
        return airTemperatureMap;
    else if (myVar == precipitation)
        return precipitationMap;
    else if (myVar == airHumidity)
        return airHumidityMap;
    else if (myVar == windIntensity)
        return windIntensityMap;
    else if (myVar == referenceEvapotranspiration)
        return ET0Map;
    else
        return NULL;
}

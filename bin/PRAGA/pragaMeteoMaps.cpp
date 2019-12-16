#include "gis.h"
#include "pragaMeteoMaps.h"


PragaHourlyMeteoMaps::PragaHourlyMeteoMaps(const gis::Crit3DRasterGrid& DEM)
{
    mapHourlyWindVectorInt = new gis::Crit3DRasterGrid;
    mapHourlyWindVectorDir = new gis::Crit3DRasterGrid;
    mapHourlyWindVectorX = new gis::Crit3DRasterGrid;
    mapHourlyWindVectorY = new gis::Crit3DRasterGrid;

    mapHourlyWindVectorInt->initializeGrid(DEM);
    mapHourlyWindVectorDir->initializeGrid(DEM);
    mapHourlyWindVectorX->initializeGrid(DEM);
    mapHourlyWindVectorY->initializeGrid(DEM);
}


PragaHourlyMeteoMaps::~PragaHourlyMeteoMaps()
{
    this->clear();
}


void PragaHourlyMeteoMaps::clear()
{
    clear();

    mapHourlyWindVectorInt->clear();
    mapHourlyWindVectorDir->clear();
    mapHourlyWindVectorX->clear();
    mapHourlyWindVectorY->clear();

    delete mapHourlyWindVectorInt;
    delete mapHourlyWindVectorDir;
    delete mapHourlyWindVectorX;
    delete mapHourlyWindVectorY;
}


gis::Crit3DRasterGrid* PragaHourlyMeteoMaps::getMapFromVar(meteoVariable myVar)
{
    if (myVar == windVectorIntensity)
        return mapHourlyWindVectorInt;
    else if (myVar == windVectorDirection)
        return mapHourlyWindVectorDir;
    else if (myVar == windVectorX)
        return mapHourlyWindVectorX;
    else if (myVar == windVectorY)
        return mapHourlyWindVectorY;
    else
        return nullptr;
}

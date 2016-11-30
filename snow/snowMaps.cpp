#include <cmath>
#include "commonConstants.h"
#include "snowMaps.h"
#include "snowparam.h"
#include "snowPoint.h"


/*
Public Sub clearAllSnowMaps()

    GIS.clearGrid Snow.SnowFallMap
    GIS.clearGrid Snow.SnowMeltMap
    GIS.clearGrid Snow.SnowWaterEquivalentMap
    GIS.clearGrid Snow.IceContentMap
    GIS.clearGrid Snow.LWContentMap
    GIS.clearGrid Snow.InternalEnergyMap
    GIS.clearGrid Snow.SnowSurfaceTempMap
    GIS.clearGrid Snow.SurfaceInternalEnergyMap
    GIS.clearGrid Snow.AgeOfSnowMap

End Sub


Public Sub ActiveAllSnowModelMap()

    Snow.SnowMeltMap.isLoaded = True
    Snow.SnowWaterEquivalentMap.isLoaded = True
    Snow.IceContentMap.isLoaded = True
    Snow.LWContentMap.isLoaded = True
    Snow.InternalEnergyMap.isLoaded = True
    Snow.SnowSurfaceTempMap.isLoaded = True
    Snow.SurfaceInternalEnergyMap.isLoaded = True
    Snow.AgeOfSnowMap.isLoaded = True

End Sub


Public Sub ResetAllSnowModelMap(ByRef grd As GIS.grid)

    GIS.GridInitFrom grd, Radiation.TransmissivityMap
    GIS.GridInitFrom grd, Snow.SnowMeltMap
    GIS.GridInitFrom grd, Snow.IceContentMap
    GIS.GridInitFrom grd, Snow.LWContentMap
    GIS.GridInitFrom grd, Snow.InternalEnergyMap
    GIS.GridInitFrom grd, Snow.SurfaceInternalEnergyMap
    GIS.GridInitFrom grd, Snow.SnowSurfaceTempMap
    GIS.GridInitFrom grd, Snow.AgeOfSnowMap
    GIS.GridInitFrom grd, WaterBalance.TempH_Map
    GIS.GridInitFrom grd, WaterBalance.HumidH_Map
    GIS.GridInitFrom grd, WaterBalance.RainH_Map
    GIS.GridInitFrom grd, WaterBalance.WindH_Map
    GIS.GridInitFrom grd, WaterBalance.EvapH_Map

    'occorre passare una mappa di stato
    GIS.GridInitFromWithValue grd, Snow.SnowFallMap, 0
    GIS.GridInitFromWithValue grd, Snow.SnowWaterEquivalentMap, 0

End Sub
*/


Crit3DSnowMaps::Crit3DSnowMaps()
{
    this->initializeMaps();
    _isLoaded = false;
}

Crit3DSnowMaps::Crit3DSnowMaps(const gis::Crit3DRasterGrid& dtmGrid, const gis::Crit3DGisSettings& gisSettings)
{
    this->initializeMaps();

    _snowFallMap->initializeGrid(dtmGrid);
    _snowMeltMap->initializeGrid(dtmGrid);
    _snowWaterEquivalentMap->initializeGrid(dtmGrid);
    _iceContentMap->initializeGrid(dtmGrid);
    _lWContentMap->initializeGrid(dtmGrid);
    _internalEnergyMap->initializeGrid(dtmGrid);
    _surfaceInternalEnergyMap->initializeGrid(dtmGrid);
    _snowSurfaceTempMap->initializeGrid(dtmGrid);
    _ageOfSnowMap->initializeGrid(dtmGrid);

    _initSoilPackTemp = 3.4;
    _initSnowSurfaceTemp = 5;

}

/*---------------------------------
' Initialize all maps
'---------------------------------
*/

void Crit3DSnowMaps::initializeMaps()
{
    _snowFallMap = new gis::Crit3DRasterGrid;
    _snowMeltMap = new gis::Crit3DRasterGrid;
    _snowWaterEquivalentMap = new gis::Crit3DRasterGrid;
    _iceContentMap = new gis::Crit3DRasterGrid;
    _lWContentMap = new gis::Crit3DRasterGrid;
    _internalEnergyMap = new gis::Crit3DRasterGrid;
    _surfaceInternalEnergyMap = new gis::Crit3DRasterGrid;
    _snowSurfaceTempMap = new gis::Crit3DRasterGrid;
    _ageOfSnowMap = new gis::Crit3DRasterGrid;
}

void Crit3DSnowMaps::updateMap(Crit3DSnowPoint* snowPoint, int row, int col)
{

    _snowFallMap->value[row][col] = snowPoint->getSnowFall();
    _snowMeltMap->value[row][col] = snowPoint->getSnowMelt();
    _snowWaterEquivalentMap->value[row][col] = snowPoint->getSnowWaterEquivalent();
    _iceContentMap->value[row][col] = snowPoint->getIceContent();
    _lWContentMap->value[row][col] = snowPoint->getLWContent();
    _internalEnergyMap->value[row][col] = snowPoint->getInternalEnergy();
    _surfaceInternalEnergyMap->value[row][col] = snowPoint->getSurfaceInternalEnergy();
    _snowSurfaceTempMap->value[row][col] = snowPoint->getSnowSurfaceTemp();
    _ageOfSnowMap->value[row][col] = snowPoint->getAgeOfSnow();

}

void Crit3DSnowMaps::resetSnowModel(gis::Crit3DRasterGrid* myGrd)
{

    float initSWE;     // [mm]
    int bulkDensity;   // [kg/m^3]

    double x, y;
    //Dim index_soil As Integer, nrHorizon As Integer
    //Dim myHorizon As MSoil.Thorizon

    for (long row = 0; row < myGrd->header->nrRows; row++)
    {
        for (long col = 0; col < myGrd->header->nrCols; col++)
        {

            initSWE = myGrd->value[row][col];

            if (initSWE != myGrd->header->flag)
            {

                //bulk density superficiale (1 cm)
                bulkDensity = DEFAULT_BULK_DENSITY;
/*
                if (_soil.isLoaded)
                {
                    gis::getUtmXYFromRowCol(myGrd, row, col, &x, &y);
//                   da eperire dato reale di bulk_density, ora messo a valore di default
                    bulkDensity = DEFAULT_BULK_DENSITY;

'                     If MSoil.readPointSoilHorizon(x, y, 1, index_soil, nrHorizon, myHorizon) Then
'                         If myHorizon.bulkDensity <> Definitions.NO_DATA Then
'                             bulk_density = myHorizon.bulkDensity * 1000         //[kg/m^3]
'                         End If
'                     End If

                }
                else
                {
                    qDebug() << "Missing soil map init";
                }
*/
                _snowWaterEquivalentMap->value[row][col] = initSWE;

                //from [mm] to [m]
                // SnowWaterEquivalent
                initSWE = initSWE / 1000;

                _snowMeltMap->value[row][col] = 0;
                _iceContentMap->value[row][col] = 0;
                _lWContentMap->value[row][col] = 0;
                _ageOfSnowMap->value[row][col] = 0;

                _snowSurfaceTempMap->value[row][col] = _initSnowSurfaceTemp;

                _surfaceInternalEnergyMap->value[row][col] = Crit3DSnowMaps::computeSurfaceInternalEnergy(_initSnowSurfaceTemp, bulkDensity, initSWE);


        // tesi pag. 54
                _internalEnergyMap->value[row][col] = Crit3DSnowMaps::computeInternalEnergyMap(_initSnowSurfaceTemp, bulkDensity, initSWE);
            }
        }
    }

}

gis::Crit3DRasterGrid* Crit3DSnowMaps::getSnowFallMap()
{
    return _snowFallMap;
}

gis::Crit3DRasterGrid* Crit3DSnowMaps::getSnowMeltMap()
{
    return _snowMeltMap;
}

gis::Crit3DRasterGrid* Crit3DSnowMaps::getSnowWaterEquivalentMap()
{
    return _snowWaterEquivalentMap;
}

gis::Crit3DRasterGrid* Crit3DSnowMaps::getIceContentMap()
{
    return _iceContentMap;
}

gis::Crit3DRasterGrid* Crit3DSnowMaps::getLWContentMap()
{
    return _lWContentMap;
}

gis::Crit3DRasterGrid* Crit3DSnowMaps::getInternalEnergyMap()
{
    return _internalEnergyMap;
}

gis::Crit3DRasterGrid* Crit3DSnowMaps::getSurfaceInternalEnergyMap()
{
    return _surfaceInternalEnergyMap;
}

gis::Crit3DRasterGrid* Crit3DSnowMaps::getSnowSurfaceTempMap()
{
    return _snowSurfaceTempMap;
}

gis::Crit3DRasterGrid* Crit3DSnowMaps::getAgeOfSnowMap()
{
    return _ageOfSnowMap;
}

// è la forumula 3.27 a pag. 54 in cui ha diviso la surface come la somma dei contributi della parte "water" e di quella "soil"
float computeSurfaceInternalEnergy(float initSnowSurfaceTemp,int bulkDensity, float initSWE)
{
   return (initSnowSurfaceTemp * (WATER_DENSITY * HEAT_CAPACITY_SNOW * std::min(initSWE, static_cast<float>(SNOW_SKIN_THICKNESS)) + SOIL_SPECIFIC_HEAT * std::max(0.0f, static_cast<float>(SNOW_SKIN_THICKNESS) - initSWE) * bulkDensity) );
}


//InternalEnergyMap pag. 54 formula 3.29  initSoilPackTemp sarebbe da chiamare initSnowPackTemp ????
float computeInternalEnergyMap(float initSoilPackTemp,int bulkDensity, float initSWE)
{
    return ( initSoilPackTemp * (WATER_DENSITY * HEAT_CAPACITY_SNOW * initSWE + bulkDensity * SNOW_DAMPING_DEPTH * SOIL_SPECIFIC_HEAT) );
}


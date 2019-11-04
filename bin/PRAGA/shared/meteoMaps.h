#ifndef METEOMAPS_H
#define METEOMAPS_H

#ifndef GIS_H
    #include "gis.h"
#endif

#ifndef METEO_H
    #include "meteo.h"
#endif

#ifndef SOLARRADIATION_H
    #include "solarRadiation.h"
#endif

class Crit3DDailyMeteoMaps
{
private:

public:
    gis::Crit3DRasterGrid* mapDailyTAvg;
    gis::Crit3DRasterGrid* mapDailyTMin;
    gis::Crit3DRasterGrid* mapDailyTMax;
    gis::Crit3DRasterGrid* mapDailyPrec;
    gis::Crit3DRasterGrid* mapDailyRHAvg;
    gis::Crit3DRasterGrid* mapDailyRHMin;
    gis::Crit3DRasterGrid* mapDailyRHMax;
    gis::Crit3DRasterGrid* mapDailyET0HS;
    gis::Crit3DRasterGrid* mapDailyLeafW;

    Crit3DDailyMeteoMaps(const gis::Crit3DRasterGrid& DEM);
    ~Crit3DDailyMeteoMaps();

    gis::Crit3DRasterGrid* getMapFromVar(meteoVariable myVar);
    bool computeHSET0Map(gis::Crit3DGisSettings *gisSettings, Crit3DDate myDate);
    bool fixDailyThermalConsistency();
};


class Crit3DHourlyMeteoMaps
{

public:
    gis::Crit3DRasterGrid* mapHourlyT;
    gis::Crit3DRasterGrid* mapHourlyTdew;
    gis::Crit3DRasterGrid* mapHourlyPrec;
    gis::Crit3DRasterGrid* mapHourlyRelHum;
    gis::Crit3DRasterGrid* mapHourlyWindInt;
    gis::Crit3DRasterGrid* mapHourlyWindDir;
    gis::Crit3DRasterGrid* mapHourlyET0;
    gis::Crit3DRasterGrid* mapHourlyLeafW;

    bool isComputed;

    Crit3DHourlyMeteoMaps(const gis::Crit3DRasterGrid& DEM);
    ~Crit3DHourlyMeteoMaps();

    gis::Crit3DRasterGrid* getMapFromVar(meteoVariable myVar);
    bool computeET0PMMap(const gis::Crit3DRasterGrid &DEM, Crit3DRadiationMaps *radMaps);
    bool computeRelativeHumidityMap();
    bool computeLeafWetnessMap();
};


#endif // METEOMAPS_H

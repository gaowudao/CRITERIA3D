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
public:
    gis::Crit3DRasterGrid* avgTemperatureMap;
    gis::Crit3DRasterGrid* minTemperatureMap;
    gis::Crit3DRasterGrid* maxTemperatureMap;
    gis::Crit3DRasterGrid* precipitationMap;
};

class Crit3DHourlyMeteoMaps
{
private:
    bool isInitialized;

public:
    gis::Crit3DRasterGrid* airTemperatureMap;
    gis::Crit3DRasterGrid* airDewTemperatureMap;
    gis::Crit3DRasterGrid* precipitationMap;
    gis::Crit3DRasterGrid* airRelHumidityMap;
    gis::Crit3DRasterGrid* windIntensityMap;
    gis::Crit3DRasterGrid* windDirectionMap;
    gis::Crit3DRasterGrid* ET0Map;
    gis::Crit3DRasterGrid* leafWetnessMap;

    bool isComputed;

    Crit3DHourlyMeteoMaps(const gis::Crit3DRasterGrid& DEM);
    ~Crit3DHourlyMeteoMaps();

    void clean();

    gis::Crit3DRasterGrid* getMapFromVar(meteoVariable myVar);
    bool computeET0Map(gis::Crit3DRasterGrid* DEM, Crit3DRadiationMaps *radMaps);
    bool computeRelativeHumidityMap(const gis::Crit3DRasterGrid& dewTemperatureMap);
    bool computeLeafWetnessMap(gis::Crit3DRasterGrid* myDEM);
};


#endif // METEOMAPS_H

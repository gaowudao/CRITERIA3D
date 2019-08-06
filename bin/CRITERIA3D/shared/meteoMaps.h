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

    class Crit3DMeteoMaps
    {
    private:
        bool isInitialized;

    public:
        bool isComputed;

        gis::Crit3DRasterGrid* airTemperatureMap;
        gis::Crit3DRasterGrid* precipitationMap;
        gis::Crit3DRasterGrid* airRelHumidityMap;
        gis::Crit3DRasterGrid* windIntensityMap;
        gis::Crit3DRasterGrid* ET0Map;

        gis::Crit3DRasterGrid* leafWetnessMap;
        gis::Crit3DRasterGrid* irrigationMap;

        gis::Crit3DRasterGrid* avgDailyTemperatureMap;

        Crit3DMeteoMaps(const gis::Crit3DRasterGrid& DEM);
        ~Crit3DMeteoMaps();

        void clean();

        gis::Crit3DRasterGrid* getMapFromVar(meteoVariable myVar);
        bool computeET0Map(gis::Crit3DRasterGrid* DEM, Crit3DRadiationMaps *radMaps);
        bool computeRelativeHumidityMap(const gis::Crit3DRasterGrid& dewTemperatureMap);
        bool computeLeafWetnessMap(gis::Crit3DRasterGrid* myDEM);
    };


#endif // METEOMAPS_H

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
    public:
        gis::Crit3DRasterGrid* airTemperatureMap;
        gis::Crit3DRasterGrid* precipitationMap;
        gis::Crit3DRasterGrid* airRelHumidityMap;
        gis::Crit3DRasterGrid* windIntensityMap;
        gis::Crit3DRasterGrid* airDewTemperatureMap;
        gis::Crit3DRasterGrid* leafWetnessMap;
        gis::Crit3DRasterGrid* ET0Map;

        gis::Crit3DRasterGrid* avgDailyTemperature;

        gis::Crit3DRasterGrid* evaporationMap;
        gis::Crit3DRasterGrid* irrigationMap;

        Crit3DMeteoMaps();
        Crit3DMeteoMaps(const gis::Crit3DRasterGrid& myDtm);
        ~Crit3DMeteoMaps();

        void initializeMaps();

        gis::Crit3DRasterGrid* getMapFromVar(meteoVariable myVar);

        bool isLoaded;
    };


#endif // METEOMAPS_H

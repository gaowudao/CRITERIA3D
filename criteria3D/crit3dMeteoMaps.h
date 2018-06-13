#ifndef CRIT3DMETEOMAPS_H
#define CRIT3DMETEOMAPS_H

    #ifndef GIS_H
        #include "gis.h"
    #endif
    #ifndef METEO_H
        #include "meteo.h"
    #endif

    class Crit3DMeteoMaps
    {
    public:
        gis::Crit3DRasterGrid* airTemperatureMap;           //[°C]
        gis::Crit3DRasterGrid* precipitationMap;            //[mm]
        gis::Crit3DRasterGrid* airHumidityMap;              //[%]
        gis::Crit3DRasterGrid* windIntensityMap;            //[m s-1]
        gis::Crit3DRasterGrid* ET0Map;                      //[mm]
        gis::Crit3DRasterGrid* irrigationMap;               //[mm]

        bool isInitialized;

        Crit3DMeteoMaps();
        Crit3DMeteoMaps(const gis::Crit3DRasterGrid& rasterGrid);
        ~Crit3DMeteoMaps();

        void emptyMeteoMaps();

        gis::Crit3DRasterGrid* getMapFromVar(meteoVariable myVar);
    };

#endif // CRIT3DMETEOMAPS_H

#ifndef RASTER
#define RASTER

    #ifndef MAP_H
        #include "map.h"
    #endif
//    #ifndef POINTMAP_H
//        #include "pointmap.h"
//    #endif

#include "MapGraphicsView.h"

    class QPainter;
    class QString;

    void initializeDTM();
    bool loadRaster(QString fileName, gis::Crit3DRasterGrid *raster);
    bool drawRaster(gis::Crit3DRasterGrid* myRaster, gis::Crit3DGeoMap* myMap, QPainter* myPainter);
    //void drawObject(MapGraphicsView* pointView);

    bool setMapResolution(MapGraphicsView* view);

#endif

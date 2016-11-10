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

    void initializeMap();
    bool loadRaster(QString fileName, gis::Crit3DRasterGrid *raster);
    bool drawRaster(gis::Crit3DRasterGrid* myRaster, gis::Crit3DMapArea* myMap, QPainter* myPainter);
    //void drawObject(MapGraphicsView* pointView);

    bool setMapCenter();
    bool setMapResolution(QPainter *myPainter, MapGraphicsView* view);

    //qreal degreesLatPerMeter(const qreal latitude);
    //qreal degreesLonPerMeter(const qreal latitude);

#endif

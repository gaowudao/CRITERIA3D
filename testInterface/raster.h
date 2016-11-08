#ifndef RASTER
#define RASTER

    #ifndef MAP_H
        #include "map.h"
    #endif
    #ifndef MAPGRAPHICSVIEW_H
        #include "MapGraphicsView.h"
    #endif

    class QPainter;
    class QString;

    void initializeMap();
    bool loadRaster(QString fileName, gis::Crit3DRasterGrid *raster);
    bool drawRaster(gis::Crit3DRasterGrid* myRaster, gis::Crit3DMapArea* myMap, QPainter* myPainter);
    void drawObject(MapGraphicsView* pointView);

    bool setMapCenter();
    bool setMapResolution(QPainter *myPainter);

#endif


#ifndef RASTER
#define RASTER

    #ifndef MAP_H
        #include "map.h"
    #endif

    #include "MapGraphicsView.h"

    class QPainter;
    class QString;

    extern bool isDrawing;

    void initializeDTM();

    /*!
     * \brief loadRaster
     * \param fileName the name of the file
     * \param raster a Crit3DRasterGrid pointer
     * \return true if everything is ok, false otherwise
     */
    bool loadRaster(QString fileName, gis::Crit3DRasterGrid *raster);

    /*!
     * \brief drawRaster
     * \param myRaster a Crit3DRasterGrid pointer
     * \param myMap a Crit3DGeoMap pointer
     * \param myPainter a QPainter pointer
     * \return true if everything is ok, false otherwise
     */
    bool drawRaster(gis::Crit3DRasterGrid* myRaster, gis::Crit3DGeoMap* myMap, QPainter* myPainter);

    //void drawObject(MapGraphicsView* pointView);

    bool setMapResolution(MapGraphicsView* map);

#endif

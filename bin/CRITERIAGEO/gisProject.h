#ifndef GIS_PROJECT_H
#define GIS_PROJECT_H

    #ifndef GIS_H
        #include "gis.h"
    #endif
    #ifndef SHAPEHANDLER_H
        #include "shapeHandler.h"
    #endif
    #ifndef GISOBJECT_H
        #include "gisObject.h"
    #endif

    #include <QString>
    #include <vector>

    class GisProject
    {
    private:

    public:
        gis::Crit3DGisSettings gisSettings;
        std::vector<GisObject*> objectList;

        GisProject();

        bool loadRaster(QString fileName);
        bool loadShapefile(QString fileName);
        void addRaster(gis::Crit3DRasterGrid *myRaster, QString fileName);
        void addShapeFile(Crit3DShapeHandler *myShape, QString fileName);
        void getRasterFromShape(Crit3DShapeHandler *shape, QString field, QString outputName, double cellSize, bool showInfo);
        bool addUnitCropMap(Crit3DShapeHandler *crop, Crit3DShapeHandler *soil, Crit3DShapeHandler *meteo, std::string idSoil, std::string idMeteo, QString fileName, double cellSize);
    };


#endif // GIS_PROJECT_H

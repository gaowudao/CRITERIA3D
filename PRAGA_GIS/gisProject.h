#ifndef GIS_PROJECT_H
#define GIS_PROJECT_H

    #ifndef GIS_H
        #include "gis.h"
    #endif
    #ifndef SHAPEHANDLER_H
        #include "shapeHandler.h"
    #endif

    #include <QString>
    #include <vector>

#define CELLSIZE 500

    enum gisObjectType{gisObjectNone, gisObjectRaster, gisObjectShape};

    class GisObject
    {
    private:

    public:
        gisObjectType type;
        QString fileNameWithPath;
        QString fileName;
        bool isSelected;

        gis::Crit3DRasterGrid * rasterPtr;
        Crit3DShapeHandler * shapePtr;

        GisObject();
        void setRaster(QString filename, gis::Crit3DRasterGrid* rasterPtr);
        void setShapeFile(QString filename, Crit3DShapeHandler* shapePtr);
    };


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
        void getRasterFromShape(Crit3DShapeHandler *shape, QString field, QString outputName);
        bool addUnitCropMap(Crit3DShapeHandler *crop, Crit3DShapeHandler *soil, Crit3DShapeHandler *meteo, std::string idSoil, std::string idMeteo, QString fileName);
    };

    QString getFileName(QString fileNameComplete);


#endif // GIS_PROJECT_H

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
        void setShape(QString filename, Crit3DShapeHandler* shapePtr);
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
        void setObjectRaster(gis::Crit3DRasterGrid *myRaster, QString fileName);
    };

    QString getFileName(QString fileNameComplete);


#endif // GIS_PROJECT_H

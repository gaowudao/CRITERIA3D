#ifndef GISOBJECT_H
#define GISOBJECT_H

    #ifndef GIS_H
        #include "gis.h"
    #endif

    #ifndef SHAPEHANDLER_H
        #include "shapeHandler.h"
    #endif

    #include <QString>

    enum gisObjectType{gisObjectNone, gisObjectRaster, gisObjectShape};

    class GisObject
    {
    private:
        QString fileNameWithPath;

        gis::Crit3DRasterGrid* rasterPtr;
        Crit3DShapeHandler* shapePtr;

        void initialize();

    public:
        gisObjectType type;
        QString fileName;
        bool isSelected;

        GisObject();

        gis::Crit3DRasterGrid* getRaster();
        Crit3DShapeHandler* getShapeHandler();
        void setRaster(QString filename, gis::Crit3DRasterGrid* rasterPtr);
        void setShapeFile(QString filename, Crit3DShapeHandler* shapePtr);
        void close();
    };

    QString getFileName(QString fileNameComplete);


#endif // GISOBJECT_H

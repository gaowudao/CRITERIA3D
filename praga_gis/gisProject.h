#ifndef GIS_PROJECT_H
#define GIS_PROJECT_H

    #ifndef GIS_H
        #include "gis.h"
    #endif

    #include <QString>
    #include <vector>

    class GisProject
    {
    private:

    public:
        gis::Crit3DGisSettings gisSettings;
        std::vector<gis::Crit3DRasterGrid *> rasterList;

        GisProject();

        bool loadRaster(QString myFileName);
    };

    QString getFileName(QString fileNameComplete);


#endif // GIS_PROJECT_H

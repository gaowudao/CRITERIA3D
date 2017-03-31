#ifndef PROJECT_H
#define PROJECT_H

    #ifndef METEOPOINT_H
        #include "meteoPoint.h"
    #endif
    #ifndef QSTRING_H
        #include <QString>
    #endif

    class Project {
    public:
        int nrMeteoPoints;
        Crit3DMeteoPoint *meteoPoints;

        gis::Crit3DRasterGrid DTM;
        gis::Crit3DRasterGrid rowMatrix;
        gis::Crit3DRasterGrid colMatrix;
        gis::Crit3DGisSettings gisSettings;

        Project();

        bool loadRaster(QString myFileName);
    };

#endif // PROJECT_H

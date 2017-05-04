#ifndef PROJECT_H
#define PROJECT_H

    #ifndef GIS_H
        #include "gis.h"
    #endif

    #ifndef METEOPOINT_H
        #include "meteoPoint.h"
    #endif

    #ifndef QSTRING_H
        #include <QString>
    #endif

    #ifndef DOWNLOAD_H
        #include <download.h>
    #endif

    #include <QList>
    #include <QDate>


    class Project {
    public:
        QList<Crit3DMeteoPoint> meteoPoints;
        QList<gis::Crit3DGeoPoint> meteoPointsSelected;
        QDate startDate;
        QDate endDate;
        Download* pointProperties;

        gis::Crit3DRasterGrid DTM;
        gis::Crit3DRasterGrid rowMatrix;
        gis::Crit3DRasterGrid colMatrix;
        gis::Crit3DGisSettings gisSettings;

        Project();

        bool loadRaster(QString myFileName);
        void downloadArkimetDailyVar(QStringList variables, bool precSelection);
        void downloadArkimetHourlyVar(QStringList variables);
    };

#endif // PROJECT_H

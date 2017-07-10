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

    const QDate NULLDATE = QDate(1,1,1800);

    class Project {
    public:
        QList<Crit3DMeteoPoint> meteoPoints;
        QList<gis::Crit3DGeoPoint> meteoPointsSelected;
        QDate startDate, endDate;
        DbMeteoPoints* dbMeteoPoints;

        gis::Crit3DRasterGrid DTM;
        gis::Crit3DRasterGrid rowMatrix;
        gis::Crit3DRasterGrid colMatrix;
        gis::Crit3DGisSettings gisSettings;

        meteoVariable currentVariable;
        frequencyType currentFrequency;

        Project();

        bool loadRaster(QString myFileName);
        bool downloadArkimetDailyVar(QStringList variables, bool precSelection);
        bool downloadArkimetHourlyVar(QStringList variables);
    };

#endif // PROJECT_H

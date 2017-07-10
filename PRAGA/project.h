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
        DbMeteoPoints* dbMeteoPoints;

        gis::Crit3DRasterGrid DTM;
        gis::Crit3DRasterGrid rowMatrix;
        gis::Crit3DRasterGrid colMatrix;
        gis::Crit3DGisSettings gisSettings;

        meteoVariable currentVariable;
        frequencyType currentFrequency;
        QDate currentDate;
        short currentHour;

        Project();

        bool loadRaster(QString myFileName);
        bool downloadArkimetDailyVar(QStringList variables, bool precSelection, Crit3DDate dateStart, Crit3DDate dateEnd);
        bool downloadArkimetHourlyVar(QStringList variables, Crit3DDate dateStart, Crit3DDate dateEnd);
    };

    Crit3DDate getCrit3DDate(const QDate& myDate);

#endif // PROJECT_H

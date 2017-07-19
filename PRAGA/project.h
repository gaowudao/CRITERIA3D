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
    private:

        frequencyType currentFrequency;
        QDate previousDate, currentDate;
        short currentHour;

    public:
        QList<Crit3DMeteoPoint> meteoPoints;
        QList<gis::Crit3DGeoPoint> meteoPointsSelected;
        DbMeteoPoints* dbMeteoPoints;

        gis::Crit3DRasterGrid DTM;
        gis::Crit3DRasterGrid rowMatrix;
        gis::Crit3DRasterGrid colMatrix;
        gis::Crit3DGisSettings gisSettings;
        Crit3DColorScale *colorScalePoints;

        meteoVariable currentVariable;

        Project();

        void setCurrentDate(QDate myDate);
        void setCurrentHour(short myHour);
        void setFrequency(frequencyType myFrequency);
        QDate getCurrentDate();
        short getCurrentHour();
        frequencyType getFrequency();

        bool loadRaster(QString myFileName);
        bool downloadDailyDataArkimet(QStringList variables, bool precSelection, QDate startDate, QDate endDate, bool showInfo);
        bool downloadHourlyDataArkimet(QStringList variables, Crit3DDate dateStart, Crit3DDate dateEnd);
        bool loadMeteoPointsData(QDate firstDate, QDate lastDate, bool showInfo);
        void getMeteoPointsRange(float *minimum, float *maximum);

        bool loadlastMeteoData();
        void closeMeteoPointsDB();
        bool loadMeteoPointsDB(QString dbName, bool showInfo);
        bool getMeteoPointSelected(int i);
        bool updateMeteoPointsData();
    };


#endif // PROJECT_H

#ifndef PROJECT_H
#define PROJECT_H

    #ifndef GIS_H
        #include "gis.h"
    #endif

    #ifndef QUALITY_H
        #include "quality.h"
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
        Crit3DMeteoPoint* meteoPoints;
        int nrMeteoPoints;

        QList<gis::Crit3DGeoPoint> meteoPointsSelected;
        DbMeteoPoints* dbMeteoPoints;

        Crit3DQuality* quality;

        gis::Crit3DRasterGrid DTM;
        gis::Crit3DRasterGrid dataRaster;
        gis::Crit3DRasterGrid *currentRaster;

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
        Crit3DTime Project::getCurrentTime();
        frequencyType getFrequency();
        meteoVariable getCurrentVariable();

        bool loadRaster(QString myFileName);
        bool downloadDailyDataArkimet(QStringList variables, bool prec0024, QDate startDate, QDate endDate, bool showInfo);
        bool downloadHourlyDataArkimet(QStringList variables, QDate startDate, QDate endDate, bool showInfo);
        bool loadMeteoPointsData(QDate firstDate, QDate lastDate, bool showInfo);
        void getMeteoPointsRange(float *minimum, float *maximum);

        bool loadlastMeteoData();
        void closeMeteoPointsDB();
        bool loadMeteoPointsDB(QString dbName);
        bool getMeteoPointSelected(int i);
        bool updateMeteoPointsData();
    };


#endif // PROJECT_H

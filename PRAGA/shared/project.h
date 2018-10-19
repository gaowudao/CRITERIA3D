#ifndef PROJECT_H
#define PROJECT_H

#include <fstream>

#ifndef QLIST_H
    #include <QList>
#endif

#ifndef QDATETIME_H
    #include <QDate>
#endif

#ifndef QUALITY_H
    #include "quality.h"
#endif

#ifndef GIS_H
    #include "gis.h"
#endif

#ifdef NETCDF
    #include "netcdfHandler.h"
#endif

#ifndef DBMETEOPOINTS_H
    #include "dbMeteoPoints.h"
#endif

#ifndef DBMETEOGRID_H
    #include "dbMeteoGrid.h"
#endif

#ifndef SOLARRADIATION_H
    #include "solarRadiation.h"
#endif


    class Project {
    private:

        frequencyType currentFrequency;
        QDate previousDate, currentDate;
        int currentHour;

    public:
        QSettings* settings;
        QSettings* pathSetting;
        QString path;
        QString logFileName;
        std::ofstream logFile;
        std::string errorString;

        int nrMeteoPoints;
        Crit3DMeteoPoint* meteoPoints;
        Crit3DMeteoPointsDbHandler* meteoPointsDbHandler;
        QList<gis::Crit3DGeoPoint> meteoPointsSelected;
        Crit3DMeteoGridDbHandler* meteoGridDbHandler;

        Crit3DColorScale *meteoPointsColorScale;

        Crit3DQuality* quality;
        bool checkSpatialQuality;

        meteoVariable currentVariable;

        gis::Crit3DGisSettings gisSettings;
        Crit3DRadiationSettings radSettings;
        Crit3DRadiationMaps *radiationMaps;

        gis::Crit3DRasterGrid DTM;
        gis::Crit3DRasterGrid dataRaster;

        Crit3DInterpolationSettings interpolationSettings;
        Crit3DInterpolationSettings qualityInterpolationSettings;

        #ifdef NETCDF
            NetCDFHandler netCDF;
        #endif

        Project();

        bool initializeSettings(QString currentPath);
        bool readSettings();
        bool checkProxySetting(QString group, std::string* name, std::string* grdName, std::string* table, std::string* field, bool *isActive);
        bool readProxies();
        void copyInterpolationSettingsToQuality();

        void setCurrentDate(QDate myDate);
        void setCurrentHour(int myHour);
        void setFrequency(frequencyType myFrequency);
        QDate getCurrentDate();
        int getCurrentHour();
        //Crit3DTime Project::getCurrentTime();
        Crit3DTime getCurrentTime();
        frequencyType getFrequency();
        meteoVariable getCurrentVariable();

        bool setLogFile();
        void logError(QString myStr);
        void log(std::string myStr);
        void logError();

        bool loadDEM(QString myFileName);
        bool downloadDailyDataArkimet(QStringList variables, bool prec0024, QDate startDate, QDate endDate, bool showInfo);
        bool downloadHourlyDataArkimet(QStringList variables, QDate startDate, QDate endDate, bool showInfo);
        bool loadMeteoPointsData(QDate firstDate, QDate lastDate, bool showInfo);
        void getMeteoPointsRange(float *minimum, float *maximum);

        bool loadlastMeteoData();
        void closeMeteoPointsDB();
        void closeMeteoGridDB();
        bool loadMeteoPointsDB(QString dbName);
        bool loadMeteoGridDB(QString xmlName);
        bool loadMeteoGridDailyData(QDate firstDate, QDate lastDate, bool showInfo);
        bool loadMeteoGridHourlyData(QDateTime firstDate, QDateTime lastDate, bool showInfo);
        void loadMeteoGridData(QDate firstDate, QDate lastDate, bool showInfo);

        bool getMeteoPointSelected(int i);
        bool updateMeteoPointsData();

        float meteoDataConsistency(meteoVariable myVar, const Crit3DTime& timeIni, const Crit3DTime& timeFin);

        void readProxyValues();
        void checkMeteoPointsDEM();
        bool writeTopographicDistanceMaps();
        bool loadTopographicDistanceMaps();
        bool interpolationDemMain(meteoVariable myVar, const Crit3DTime& myTime, gis::Crit3DRasterGrid *myRaster, bool showInfo);
        bool interpolationDem(meteoVariable myVar, const Crit3DTime& myTime, gis::Crit3DRasterGrid *myRaster, bool showInfo);
        bool interpolateDemRadiation(const Crit3DTime& myTime, gis::Crit3DRasterGrid *myRaster, bool showInfo);
    };


#endif // PROJECT_H

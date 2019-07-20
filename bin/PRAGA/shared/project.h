#ifndef PROJECT_H
#define PROJECT_H

    #ifndef QUALITY_H
        #include "quality.h"
    #endif
    #ifndef GIS_H
        #include "gis.h"
    #endif
    #ifndef DBMETEOPOINTS_H
        #include "dbMeteoPointsHandler.h"
    #endif
    #ifndef DBAGGREGATIONSHANDLER_H
        #include "dbAggregationsHandler.h"
    #endif
    #ifndef DBMETEOGRID_H
        #include "dbMeteoGrid.h"
    #endif
    #ifndef SOLARRADIATION_H
        #include "solarRadiation.h"
    #endif

    #ifndef QSETTINGS_H
        #include <QSettings>
    #endif
    #ifndef _FSTREAM_
        #include <fstream>
    #endif

    #ifdef NETCDF
        #include "netcdfHandler.h"
    #endif


    class Project {
    private:


    protected:
        QString path;

        // database connection
        QSqlDatabase dbConnection;
        QString dbProvider;
        QString dbHostname;
        QString dbName;
        int dbPort;
        QString dbUsername;
        QString dbPassword;

        frequencyType currentFrequency;
        meteoVariable currentVariable;
        QDate previousDate, currentDate;
        int currentHour;

        void inizializeDBConnection();

    public:
        int modality;
        bool requestedExit;
        QString errorString;
        QString logFileName;
        std::ofstream logFile;

        QSettings* parameters;
        QSettings* projectSettings;

        int nrMeteoPoints;
        Crit3DMeteoPoint* meteoPoints;
        Crit3DMeteoPointsDbHandler* meteoPointsDbHandler;
        Crit3DAggregationsDbHandler* aggregationDbHandler;
        QList<gis::Crit3DGeoPoint> meteoPointsSelected;
        Crit3DMeteoGridDbHandler* meteoGridDbHandler;

        Crit3DColorScale *meteoPointsColorScale;

        Crit3DQuality* quality;
        bool checkSpatialQuality;

        Crit3DMeteoSettings* meteoSettings;

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

        bool openDBConnection();
        bool loadCommonSettings(QString settingsFileName);
        bool loadParameters(QString parametersFileName);
        void setProxyDEM();
        bool checkProxy(std::string name_, std::string gridName_, std::string table_, std::string field_, QString *error);
        void addProxy(std::string name_, std::string gridName_, std::string table_, std::string field_, bool isForQuality_, bool isActive_);

        void setCurrentDate(QDate myDate);
        void setCurrentHour(int myHour);
        void setFrequency(frequencyType frequency);
        void setCurrentVariable(meteoVariable variable);
        int getCurrentHour();
        QDate getCurrentDate();
        Crit3DTime getCurrentTime();
        frequencyType getFrequency();
        meteoVariable getCurrentVariable();

        void setPath(QString myPath);
        QString getPath();

        bool setLogFile(QString fileNameWithPath);
        void logError(QString myStr);
        void logInfo(QString myStr);
        void logInfoGUI(QString myStr);
        void logError();

        void closeMeteoPointsDB();
        void closeMeteoGridDB();

        bool loadDEM(QString myFileName);
        bool loadMeteoPointsData(QDate firstDate, QDate lastDate, bool showInfo);
        bool loadMeteoPointsDB(QString dbName);
        bool loadMeteoGridDB(QString xmlName);
        bool loadAggregationdDB(QString dbName);
        bool loadMeteoGridDailyData(QDate firstDate, QDate lastDate, bool showInfo);
        bool loadMeteoGridHourlyData(QDateTime firstDate, QDateTime lastDate, bool showInfo);
        void loadMeteoGridData(QDate firstDate, QDate lastDate, bool showInfo);
        void findLastMeteoDate();

        void getMeteoPointsRange(float *minimum, float *maximum);
        bool getMeteoPointSelected(int i);

        float meteoDataConsistency(meteoVariable myVar, const Crit3DTime& timeIni, const Crit3DTime& timeFin);

        bool loadProxyGrids();
        bool readPointProxyValues(Crit3DMeteoPoint* myPoint, QSqlDatabase* myDb);
        bool readProxyValues();
        bool updateProxy();
        void checkMeteoPointsDEM();
        bool writeTopographicDistanceMaps();
        bool loadTopographicDistanceMaps();
        bool interpolationDemMain(meteoVariable myVar, const Crit3DTime& myTime, gis::Crit3DRasterGrid *myRaster, bool showInfo);
        bool interpolationDem(meteoVariable myVar, const Crit3DTime& myTime, gis::Crit3DRasterGrid *myRaster, bool showInfo);
        bool interpolateDemRadiation(const Crit3DTime& myTime, gis::Crit3DRasterGrid *myRaster, bool showInfo);

        QString getCompleteFileName(QString fileName, QString secondaryPath);
        bool executeSharedCommand(QStringList argumentList, bool *isCommandFound);
    };


#endif // PROJECT_H

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
        QString appPath;
        QString defaultPath;
        QString projectPath;

        bool createDefaultSettings(QString fileName);
        bool searchDefaultPath(QString* path);

    protected:
        frequencyType currentFrequency;
        meteoVariable currentVariable;
        QDate previousDate, currentDate;
        int currentHour;

    public:
        QString projectName = "";
        bool isProjectLoaded;
        int modality;

        bool requestedExit;
        QString errorString;

        QString logFileName;
        QString demFileName;
        QString dbPointsFileName;
        QString dbGridXMLFileName;
        QString parametersFileName;

        std::ofstream logFile;

        QSettings* parameters;
        QSettings* projectSettings;

        int nrMeteoPoints;
        Crit3DMeteoPoint* meteoPoints;
        Crit3DMeteoPointsDbHandler* meteoPointsDbHandler;
        Crit3DAggregationsDbHandler* aggregationDbHandler;
        QList<gis::Crit3DGeoPoint> meteoPointsSelected;
        Crit3DMeteoGridDbHandler* meteoGridDbHandler;
        Crit3DColorScale* meteoPointsColorScale;

        Crit3DQuality* quality;
        bool checkSpatialQuality;

        Crit3DMeteoSettings* meteoSettings;

        gis::Crit3DGisSettings gisSettings;
        Crit3DRadiationSettings radSettings;
        Crit3DRadiationMaps *radiationMaps;

        gis::Crit3DRasterGrid DEM;
        gis::Crit3DRasterGrid dataRaster;

        Crit3DInterpolationSettings interpolationSettings;
        Crit3DInterpolationSettings qualityInterpolationSettings;

        #ifdef NETCDF
            NetCDFHandler netCDF;
        #endif

        Project();

        void initializeProject();
        void clearProject();
        bool start(QString appPath);
        bool loadProject();
        bool loadProjectSettings(QString settingsFileName);
        bool createProject(QString path_, QString name_, QString filename_);
        void saveSettings();
        bool loadParameters(QString parametersFileName);
        void saveParameters();
        void setProxyDEM();
        void clearProxyDEM();
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

        void setApplicationPath(QString myPath);
        QString getApplicationPath();
        void setDefaultPath(QString myPath);
        QString getDefaultPath();
        void setProjectPath(QString myPath);
        QString getProjectPath();

        bool setLogFile(QString fileNameWithPath);
        void logError(QString myStr);
        void logInfo(QString myStr);
        void logInfoGUI(QString myStr);
        void logError();

        void closeMeteoPointsDB();
        void closeMeteoGridDB();

        bool loadDEM(QString myFileName);
        void closeDEM();
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
    };


#endif // PROJECT_H

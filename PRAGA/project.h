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
        #include "download.h"
    #endif

    #ifndef DBMETEOGRID_H
        #include "dbMeteoGrid.h"
    #endif

    #ifndef CRIT3DPROJECT_H
        #include "crit3dProject.h"
    #endif

    #ifndef NETCDFHANDLER_H
        #include "netcdfHandler.h"
    #endif

    #ifndef DBCLIMATE_H
        #include "dbClimate.h"
    #endif

    #ifndef INTERPOLATIONSETTINGS_H
        #include "interpolationSettings.h"
    #endif

    #ifndef INTERPOLATIONPOINT_H
        #include "interpolationPoint.h"
    #endif

    #include <QList>
    #include <QDate>
    #include <fstream>

    class Project {
    private:

        frequencyType currentFrequency;
        QDate previousDate, currentDate;
        int currentHour;

    public:
        QSettings* settings;
        QString path;
        QString logFileName;
        std::ofstream logFile;
        std::string errorString;

        Crit3DMeteoPoint* meteoPoints;
        Crit3DMeteoPointsDbHandler* meteoPointsDbHandler;
        Crit3DMeteoGridDbHandler* meteoGridDbHandler;
        Crit3DClimate* clima;
        Crit3DClimate* referenceClima;

        gis::Crit3DGeoPoint startLocation;

        int nrMeteoPoints;
        bool elabMeteoPointsValue;

        QList<gis::Crit3DGeoPoint> meteoPointsSelected;

        Crit3DQuality* quality;
        bool checkSpatialQuality;

        gis::Crit3DRasterGrid DTM;
        gis::Crit3DRasterGrid dataRaster;

        Crit3DRadiationMaps *radiationMaps;

        Crit3DProject Criteria3Dproject;

        NetCDFHandler netCDF;

        gis::Crit3DGisSettings gisSettings;

        Crit3DColorScale *meteoPointsColorScale;

        meteoVariable currentVariable;

        gridAggregationMethod grdAggrMethod;

        std::vector <Crit3DInterpolationDataPoint> interpolationPointList;
        Crit3DInterpolationSettings interpolationSettings;
        Crit3DInterpolationSettings qualityInterpolationSettings;

        Project();

        bool initializeSettings(QString currentPath);
        bool readSettings();
        bool checkProxySetting(QString group, std::string* name, std::string* grdName, std::string* table, std::string* field);
        bool readProxies();

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

        bool readProxyValues();
        bool interpolateRaster(meteoVariable myVar, frequencyType myFrequency, const Crit3DTime& myTime, gis::Crit3DRasterGrid *myRaster);
        bool interpolateRasterRadiation(const Crit3DTime& myTime, gis::Crit3DRasterGrid *myRaster, std::string *myError);
        bool interpolateGrid(meteoVariable myVar, frequencyType myFrequency, const Crit3DTime& myTime, gis::Crit3DRasterGrid *myRaster);
        bool saveGrid(meteoVariable myVar, frequencyType myFrequency, const Crit3DTime& myTime, bool showInfo);

        bool elaborationCheck(bool isMeteoGrid);
        bool elaboration(bool isMeteoGrid, bool isAnomaly);
        void fillAnomaly(bool isMeteoGrid);

        bool loadModelParameters(QString dbName);
        bool loadSoilData(QString dbName);
        bool loadSoilMap(QString fileName);
        bool initializeCriteria3D();

        bool getElabMeteoPointsValue() const;
        void setElabMeteoPointsValue(bool value);
    };


#endif // PROJECT_H

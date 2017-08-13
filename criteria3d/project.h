#ifndef PROJECT_H
#define PROJECT_H

    #include <fstream>

    #ifndef QSQLDATABASE_H
        #include <QSqlDatabase>
    #endif
    #ifndef QSTRING_H
        #include <QString>
    #endif
    #ifndef QDATETIME_H
        #include <QDateTime>
    #endif
    #ifndef CRIT3DDATE_H
        #include "crit3dDate.h"
    #endif
    #ifndef METEOPOINT_H
        #include "meteoPoint.h"
    #endif
    #ifndef SOIL_H
        #include "soil.h"
    #endif
    #ifndef RADIATIONSETTINGS_H
        #include "radiationSettings.h"
    #endif
    #ifndef INTERPOLATIONSETTINGS_H
        #include "interpolationSettings.h"
    #endif
    #ifndef QUALITY_H
        #include "quality.h"
    #endif
    #ifndef METEOMAPS_H
        #include "meteoMaps.h"
    #endif
    #ifndef PLANT_H
        #include "plant.h"
    #endif
    #ifndef VECTOR_H
        #include <vector>
    #endif

    enum Tenvironment {gui, batch};

    class Crit3DProject
    {
    private:
        Tenvironment environment;

    public:
        Crit3DTime currentTime;
        meteoVariable currentVar;

        QString sqlDriver;
        QString hostName;
        QString databaseName;
        int connectionPort;
        QString userName;
        QString password;

        QString idArea;
        QString path;
        QString dailyOutputPath;
        QString dtmFileName;
        QString fieldMapName;
        QString logFileName;
        std::ofstream logFile;

        QSqlDatabase db;
        Crit3DMeteoPoint* meteoPoints;

        gis::Crit3DRasterGrid dtm;
        gis::Crit3DRasterGrid interpolatedDtm;
        gis::Crit3DRasterGrid indexGrid;
        gis::Crit3DRasterGrid boundaryGrid;
        gis::Crit3DRasterGrid fieldMap;

        soil::Crit3DSoilClass soilClass[13];

        //waterbalance
        int nrSoils;
        int nrSoilLayers;
        long nrNodes;
        long nrLayerNodes;
        float soilDepth;                                    //[m]
        double* currentProfile;
        std::vector <double> layerDepth;                     //[m]
        std::vector <double> layerThickness;                 //[m]
        soil::Crit3DSoil* soilList;
        gis::Crit3DGisSettings gisSettings;

        //Tvine* vine;
        int nrVines;

        //TvineField* vineFields;
        int nrVineFields;
        long nrFieldOperations;

        int nrMeteoPoints;
        bool isObsDataLoaded;
        int* varCodes;
        int* aggrVarCodes;
        int nrAggrVar;
        float windIntensityDefault;

        bool isProjectLoaded;

        QString projectError;

        Crit3DQuality qualityParameters;

        Crit3DMeteoMaps* meteoMaps;
        Crit3DStatePlantMaps* statePlantMaps;
        Crit3DOutputPlantMaps* outputPlantMaps;

        //TstatePlant statePlant;

        Crit3DInterpolationSettings interpolationSettings;
        Crit3DRadiationSettings radiationSettings;

        int hourlyIntervals;
        QDate lastDateTransmissivity;

        Crit3DProject();

        void deleteAllGrids();
        void initializeMeteoPoints();
        void initialize();
        void initializeMeteoMaps();
        void initializeRadiationMaps();

        void closeProject();
        bool loadProject(QString myFileName);
        bool openDB();

        bool loadDTM(QString myFileName);
        int queryFieldPoint(double x, double y);
        bool loadFieldShape();
        bool loadFieldMap(QString myFileName);

        bool loadFieldsProperties();
        bool loadDBPoints();
        bool loadVanGenuchtenParameters();

        bool loadSoils();
        bool loadHorizons(soil::Crit3DSoil* mySoil, int idSoil);
        bool getMeteoVarIndexRaw(meteoVariable myVar, int *nrVarIndices, int **varIndices);
        bool loadObsDataHourly(int indexPoint, QDate d1, QDate d2, QString tableName);
        bool loadObsDataHourlyVar(int indexPoint, meteoVariable myVar, QDate d1, QDate d2, QString tableName);
        bool loadObsDataAllPoints(QDate d1, QDate d2);
        bool loadObsDataAllPointsVar(meteoVariable myVar, QDate d1, QDate d2);
        bool meteoDataLoaded(const Crit3DTime& myTimeIni, const Crit3DTime& myTimeFin);
        float meteoDataConsistency(meteoVariable myVar, const Crit3DTime& myTimeIni, const Crit3DTime& myTimeFin);
        bool loadSettings();
        bool loadClimateParameters();
        bool loadStates(QDate myDate, QString myArea);
        bool saveStateAndOutput(QDate myDate, QString myArea);

        QString getGeoserverPath();
        int getIndexPointFromId(QString myId);

        bool setLogFile();
        void logError();
        void logError(QString myError);
        void logInfo(QString myLog);
        void setEnvironment(Tenvironment myEnv);
        float getTimeStep();
        int getFieldIndex(long row, long col);
        bool isVineyard(long row, long col);
        int getSoilIndex(long row, long col);

        bool LoadObsDataFilled(QDateTime firstTime, QDateTime lastTime);
        bool runModels(QDateTime myTime1, QDateTime myTime2, bool isSaveOutput, const QString& myArea);
    };

#endif // PROJECT_H

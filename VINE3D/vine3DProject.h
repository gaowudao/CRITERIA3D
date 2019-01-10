#ifndef VINE3DPROJECT_H
#define VINE3DPROJECT_H

#ifndef QSTRING_H
    #include <QString>
#endif

#ifndef QSQLDATABASE_H
    #include <QSqlDatabase>
#endif

#ifndef GRAPEVINE_H
    #include "grapevine.h"
#endif

#ifndef METEOMAPS_H
    #include "meteoMaps.h"
#endif

#ifndef PLANT_H
    #include "plant.h"
#endif

#ifndef PROJECT_H
    #include "project.h"
#endif

    enum Tenvironment {gui, batch};

    struct TfieldBook {
        QDate operationDate;
        TfieldOperation operation;
        int idField;
        float quantity;         //irrigation hours, thinning percentage, tartaric acid value
    };


    class Vine3DProject : public Project
    {
    private:
        Tenvironment environment;

    public:
        Crit3DTime currentTime;

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

        gis::Crit3DRasterGrid interpolatedDtm;
        gis::Crit3DRasterGrid indexMap;
        gis::Crit3DRasterGrid boundaryMap;
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

        Tvine* vine;
        int nrVines;

        TvineField* vineFields;
        TtrainingSystem* trainingSystems;
        TfieldBook* fieldBook;
        int nrTrainingSystems;
        int nrVineFields;
        long nrFieldOperations;

        bool isObsDataLoaded;
        int* varCodes;
        int* aggrVarCodes;
        int nrAggrVar;

        bool isProjectLoaded;

        QString projectError;

        Crit3DQuality qualityParameters;

        Crit3DMeteoMaps* meteoMaps;
        Crit3DStatePlantMaps* statePlantMaps;
        Crit3DOutputPlantMaps* outputPlantMaps;

        TstatePlant statePlant;
        Vine3D_Grapevine grapevine;

        QDate lastDateTransmissivity;

        Vine3DProject();

        bool loadVine3DSettings();
        bool loadVine3DProjectSettings(QString projectFile);
        bool loadVine3DProjectParameters();
        bool loadClimateParameters();
        bool loadAggregatedMeteoVarCodes();
        bool loadFieldsProperties();
        bool loadDBPoints();
        bool loadVanGenuchtenParameters();
        bool loadGrapevineParameters();
        bool loadTrainingSystems();
        bool loadSoils();
        bool loadFieldBook();
        bool loadHorizons(soil::Crit3DSoil* mySoil, int idSoil);

        void deleteAllGrids();
        void initializeMeteoPoints();
        void initialize();
        void initializeMeteoMaps();
        void initializeRadiationMaps();

        void closeProject();
        bool loadProject(QString myFileName);
        bool openDB();

        int queryFieldPoint(double x, double y);
        bool loadFieldShape();
        bool loadFieldMap(QString myFileName);

        bool readFieldQuery(QSqlQuery myQuery, int* idField, int* vineIndex, int* trainingIndex,
                            int* soilIndex, float* maxLaiGrass,  float* maxIrrigationRate);
        bool setField(int fieldIndex, int soilIndex, int vineIndex, int trainingIndex,
                            float maxLaiGrass,  float maxIrrigationRate);
        bool getFieldBookIndex(int firstIndex, QDate myQDate, int fieldIndex, int* outputIndex);

        int getAggregatedVarCode(int rawVarCode);
        bool getMeteoVarIndexRaw(meteoVariable myVar, int *nrVarIndices, int **varIndices);
        bool loadObsDataSubHourly(int indexPoint, meteoVariable myVar, QDateTime d1, QDateTime d2, QString tableName);
        bool loadObsDataHourly(int indexPoint, QDate d1, QDate d2, QString tableName, bool useAggrCodes);
        bool loadObsDataHourlyVar(int indexPoint, meteoVariable myVar, QDate d1, QDate d2, QString tableName, bool useAggrCodes);
        bool loadObsDataAllPoints(QDate d1, QDate d2);
        bool loadObsDataAllPointsVar(meteoVariable myVar, QDate d1, QDate d2);
        bool meteoDataLoaded(const Crit3DTime& myTimeIni, const Crit3DTime& myTimeFin);
        float meteoDataConsistency(meteoVariable myVar, const Crit3DTime& myTimeIni, const Crit3DTime& myTimeFin);
        bool LoadObsDataFilled(QDateTime firstTime, QDateTime lastTime);

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

        bool runModels(QDateTime myTime1, QDateTime myTime2, bool isSaveOutput, const QString& myArea);
    };

#endif // PROJECT_H

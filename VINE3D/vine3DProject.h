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

#ifndef WATERBALANCE_H
    #include "waterBalance.h"
#endif

#ifndef PROJECT_H
    #include "project.h"
#endif

#ifndef SOILFLUXES3DSETTINGS_H
    #include "soil3D.h"
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

        QString parametersFile;

        QString idArea;
        QString dailyOutputPath;
        QString demFileName;
        QString fieldMapName;

        Crit3DSoilFluxesSettings* WBSettings;
        Crit3DSoilFluxesMaps* WBMaps;

        gis::Crit3DRasterGrid interpolatedDtm;

        gis::Crit3DRasterGrid boundaryMap;
        gis::Crit3DRasterGrid modelCaseIndexMap;

        soil::Crit3DSoilClass soilClass[13];

        TVineCultivar* cultivar;
        int nrCultivar;

        Crit3DModelCase* modelCases;
        TtrainingSystem* trainingSystems;
        TfieldBook* fieldBook;
        int nrTrainingSystems;
        int nrModelCases;
        long nrFieldOperations;

        bool isObsDataLoaded;
        int* varCodes;
        int* aggrVarCodes;
        int nrAggrVar;

        bool isProjectLoaded;

        Crit3DQuality qualityParameters;

        Crit3DMeteoMaps* meteoMaps;
        Crit3DWaterBalanceMaps* outputWaterBalanceMaps;
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
        bool loadHorizons(soil::Crit3DSoil* mySoil, int idSoil, QString soil_code);

        void deleteAllGrids();
        void initializeMeteoPoints();
        void initialize();
        void initializeMeteoMaps();
        void initializeRadiationMaps();

        void closeProject();
        bool loadProject(QString myFileName);

        int queryFieldPoint(double x, double y);
        bool loadFieldShape();
        bool loadFieldMap(QString myFileName);

        bool readFieldQuery(QSqlQuery myQuery, int* idField, Crit3DLanduse* landuse, int* vineIndex, int* trainingIndex,
                            int* soilIndex, float* maxLaiGrass,  float* maxIrrigationRate);
        bool setField(int fieldIndex, int fieldId, Crit3DLanduse landuse, int soilIndex, int vineIndex, int trainingIndex,
                            float maxLaiGrass,  float maxIrrigationRate);
        bool getFieldBookIndex(int firstIndex, QDate myQDate, int fieldIndex, int* outputIndex);


        int getAggregatedVarCode(int rawVarCode);
        bool getMeteoVarIndexRaw(meteoVariable myVar, int *nrVarIndices, int **varIndices);
        bool loadObsDataSubHourly(int indexPoint, meteoVariable myVar, QDateTime d1, QDateTime d2, QString tableName);
        bool loadObsDataHourly(int indexPoint, QDate d1, QDate d2, QString tableName, bool useAggrCodes);
        bool loadObsDataHourlyVar(int indexPoint, meteoVariable myVar, QDate d1, QDate d2, QString tableName, bool useAggrCodes);
        bool loadObsDataAllPoints(QDate d1, QDate d2, bool showInfo);
        bool loadObsDataAllPointsVar(meteoVariable myVar, QDate d1, QDate d2);
        bool meteoDataLoaded(const Crit3DTime& myTimeIni, const Crit3DTime& myTimeFin);
        float meteoDataConsistency(meteoVariable myVar, const Crit3DTime& myTimeIni, const Crit3DTime& myTimeFin);
        bool LoadObsDataFilled(QDateTime firstTime, QDateTime lastTime);
        void findVine3DLastMeteoDate();

        bool loadStates(QDate myDate, QString myArea);
        bool saveStateAndOutput(QDate myDate, QString myArea);

        int getIndexPointFromId(QString myId);

        bool setLogFile();
        void logError();
        void logError(QString myError);
        void logInfo(QString myLog);
        void setEnvironment(Tenvironment myEnv);
        float getTimeStep();
        int getModelCaseIndex(long row, long col);
        bool isVineyard(long row, long col);
        int getSoilIndex(long row, long col);

        bool runModels(QDateTime myTime1, QDateTime myTime2, bool isSaveOutput, const QString& myArea);
    };

#endif // PROJECT_H

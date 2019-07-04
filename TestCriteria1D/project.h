#ifndef PROJECT
#define PROJECT

    #ifndef CRITERIA1D_H
        #include "criteria1D.h"
    #endif

    #include <fstream>

    #define ERROR_SETTINGS_MISSING -1
    #define ERROR_SETTINGS_WRONGFILENAME -2
    #define ERROR_SETTINGS_MISSINGDATA -3
    #define ERROR_SETTINGS_PROXY -4
    #define ERROR_DBPARAMETERS -5
    #define ERROR_DBSOIL -6
    #define ERROR_DBMETEO_OBSERVED -7
    #define ERROR_DBMETEO_FORECAST -8
    #define ERROR_DBUNITS -9
    #define ERROR_DBOUTPUT -10

    #define ERROR_READ_UNITS -15

    #define ERROR_SOIL_PARAMETERS -20
    #define ERROR_SOIL_MISSING -21
    #define WARNING_SOIL 20

    #define ERROR_CROP_MISSING -30
    #define WARNING_CROP 30

    #define ERROR_METEO -40
    #define WARNING_METEO 40

    #define ERROR_UNKNOWN -99


    class Criteria1DProject
    {
        public:
            QString path;
            QString name;
            QString configFileName;

            QString dbParametersName;
            QString dbSoilName;
            QString dbMeteoName;
            QString dbForecastName;
            QString dbOutputName;
            QString dbUnitsName;

            QSqlDatabase dbUnits;

            QString logFileName;
            std::ofstream logFile;
            std::ofstream outputFile;

            bool isProjectLoaded;
            QString irrigationFileName;
            QString irrigationPath;
            QString dbOutputPath;

            QString projectError;

            Criteria1D criteria;

            int nrUnits;
            Criteria1DUnit *unit;

            Criteria1DProject();

            void initialize();
            void initializeUnit(int nr);
            void closeProject();
            int initializeProject(QString myFileName);
            bool readSettings();
            bool setLogFile();

            void closeAllDatabase();
            int openAllDatabase();
            bool loadUnits();

            void logInfo(QString logStr);
            void logError();
            void logError(QString myErrorStr);
    };

#endif // PROJECT


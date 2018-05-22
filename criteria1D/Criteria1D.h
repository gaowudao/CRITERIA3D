#ifndef CRITERIA1D_H
#define CRITERIA1D_H

    #include <QSqlDatabase>
    #include "soil.h"
    #include "crop.h"
    #include "meteoPoint.h"

    class Criteria1DUnit
    {
        public:
        QString idCase;
        QString idCrop;
        QString idSoil;
        QString idMeteo;
        QString idForecast;
        QString idCropClass;
        int idSoilNumber;
        int idICM;

        Criteria1DUnit();
    };

    /*!
     * \brief The Criteria1DOutput class
     * \note all variables are in [mm]
     */
    class Criteria1DOutput
    {
    public:
        double dailyPrec;
        double dailySurfaceRunoff;
        double dailyLateralDrainage;
        double dailyDrainage;
        double dailyIrrigation;
        double dailyEt0;
        double dailyMaxEvaporation;
        double dailyEvaporation;
        double dailyMaxTranspiration;
        double dailyKc;
        double dailyTranspiration;
        double dailyCropAvailableWater;
        double dailySoilWaterDeficit;
        double dailyWaterTable;
        double dailyCapillaryRise;

        Criteria1DOutput();
        void initializeDaily();
    };

    class Criteria1D
    {
    public:
        QString idCase;

        // DATABASE
        QSqlDatabase dbParameters;
        QSqlDatabase dbSoil;
        QSqlDatabase dbMeteo;
        QSqlDatabase dbForecast;
        QSqlDatabase dbOutput;

        // IRRIGATION seasonal forecast
        bool isSeasonalForecast;
        int firstSeasonMonth;
        double* seasonalForecasts;
        int nrSeasonalForecasts;

        // IRRIGATION short term forecast
        bool isShortTermForecast;
        int daysOfForecast;

        // SOIL
        soil::Crit3DSoil mySoil;
        soil::Crit3DSoilClass soilTexture[13];
        soil::Crit3DLayer* layer;
        int nrLayers;
        double layerThickness;                  // [m]
        double maxSimulationDepth;              // [m]
        bool isGeometricLayer;

        // CROP
        Crit3DCrop myCrop;
        bool optimizeIrrigation;

        // WHEATER
        Crit3DMeteoPoint meteoPoint;

        // FIELD
        double depthPloughedSoil;               // [m] depth of ploughed soil (working layer)
        double initialAW[2];                    // [-] fraction of available water (between wilting point and field capacity)

        // OUTPUT
        Criteria1DOutput output;
        QString outputString;

        Criteria1D();

        bool loadMeteo(QString idMeteo, QString idForecast, std::string *myError);
        bool setSoil(QString idSoil, std::string *myError);
        bool createOutputTable(std::string* myError);
        void prepareOutput(Crit3DDate myDate, bool isFirst);
        bool saveOutput(std::string* myError);
        void initializeSeasonalForecast(const Crit3DDate& firstDate, const Crit3DDate& lastDate);
    };


#endif // CRITERIA1D_H

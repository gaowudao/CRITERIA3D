#ifndef PROJECT3D_H
#define PROJECT3D_H

    #ifndef PROJECT_H
        #include "project.h"
    #endif
    #ifndef METEOMAPS_H
        #include "meteoMaps.h"
    #endif
    #ifndef SOIL_H
        #include "soil.h"
    #endif
    class QString;


    enum criteria3DVariable {waterContent, waterTotalPotential, waterMatricPotential,
                        availableWaterContent, degreeOfSaturation, soilTemperature,
                        soilSurfaceMoisture, bottomDrainage, waterDeficit, waterInflow, waterOutflow};


    class Project3D : public Project
    {
    private:


    public:
        QString soilDbFileName;
        QString soilMapFileName;

        unsigned long nrNodes;
        unsigned int nrLayers;
        int nrLateralLink;

        // 3D soil fluxes maps
        gis::Crit3DRasterGrid soilIndexMap;
        gis::Crit3DRasterGrid boundaryMap;
        std::vector <gis::Crit3DRasterGrid> indexMap;

        // soil properties
        unsigned int nrSoils;
        double soilDepth;                       // [m]

        std::vector <soil::Crit3DSoil> soilList;
        soil::Crit3DTextureClass texturalClassList[13];
        soil::Crit3DFittingOptions fittingOptions;

        // layers
        double minThickness;                    // [m]
        double maxThickness;                    // [m]
        double thickFactor;                     // [m]

        std::vector <double> layerDepth;        // [m]
        std::vector <double> layerThickness;    // [m]

        // sink/source
        std::vector <double> waterSinkSource;   // [m^3/sec]


        Project3D();

        void initializeProject3D();
        void clearProject3D();

        void clearWaterBalance3D();

        bool loadSoilDatabase(QString fileName);

        void computeNrLayers();
        void setLayersDepth();
        bool setIndexMaps();
        bool setBoundary();
        bool setCrit3DSurfaces();
        bool setCrit3DSoils();
        bool setCrit3DTopography();
        bool setCrit3DNodeSoil();

        bool initializeSoilMoisture(int month);

        int getSoilIndex(long row, long col);
        bool isWithinSoil(int soilIndex, double depth);

        bool aggregateAndSaveDailyMap(meteoVariable myVar, aggregationMethod myAggregation, const Crit3DDate& myDate,
                                      const QString& dailyPath, const QString& hourlyPath, const QString& myArea);
    };


    bool isCrit3dError(int result, QString* error);
    double getCriteria3DVar(criteria3DVariable myVar, long nodeIndex);

    QString getOutputNameDaily(QString varName, QString strArea, QString notes, QDate myDate);
    QString getOutputNameHourly(meteoVariable myVar, QDateTime myTime, QString myArea);

    float readDataHourly(meteoVariable myVar, QString hourlyPath, QDateTime myTime, QString myArea, int row, int col);
    bool readHourlyMap(meteoVariable myVar, QString hourlyPath, QDateTime myTime, QString myArea, gis::Crit3DRasterGrid* myGrid);


#endif // PROJECT3D_H

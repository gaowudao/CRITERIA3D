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


    enum criteria3DVariable {waterContent, waterTotalPotential, waterMatricPotential,
                        availableWaterContent, degreeOfSaturation, soilTemperature,
                        soilSurfaceMoisture, bottomDrainage, waterDeficit, waterInflow, waterOutflow};


    class Project3D : public Project
    {

    public:
        long nrNodes;
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

        // layers
        double minThickness;                    // [m]
        double maxThickness;                    // [m]
        double thickFactor;                     // [m]

        std::vector <double> layerDepth;        // [m]
        std::vector <double> layerThickness;    // [m]

        // sink/source
        std::vector <double> waterSinkSource;   // [m^3/sec]

        // meteo maps (move to Project ?)
        Crit3DMeteoMaps* meteoMaps;


        Project3D();

        void initializeProject3D();
        void clearProject3D();

        void clearWaterBalance3D();

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
    };


    bool isCrit3dError(int result, QString* error);
    double getCriteria3DVar(criteria3DVariable myVar, long nodeIndex);


#endif // PROJECT3D_H

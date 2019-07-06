#ifndef CRITERIA3DPROJECT_H
#define CRITERIA3DPROJECT_H

    #define CRITERIA3D

    #ifndef SOIL_H
        #include "soil.h"
    #endif
    #ifndef PROJECT_H
        #include "project.h"
    #endif
    #ifndef METEOMAPS_H
        #include "meteoMaps.h"
    #endif

    #include <QString>

    class Crit3DProject : public Project
    {
    private:

    public:
        Crit3DMeteoMaps* meteoMaps;
        gis::Crit3DRasterGrid soilMap;

        // same header of DTM
        gis::Crit3DRasterGrid soilIndexMap;
        gis::Crit3DRasterGrid cropIndexMap;
        gis::Crit3DRasterGrid indexMap;
        gis::Crit3DRasterGrid boundaryMap;

        int nrSoils;
        int nrLayers;
        long nrVoxels;
        long nrVoxelsPerLayer;
        bool isParametersLoaded;
        bool isInitialized;

        double soilDepth;                                //[m]
        std::vector <double> layerDepth;                 //[m]
        std::vector <double> layerThickness;             //[m]
        std::vector <soil::Crit3DSoil> soilList;
        soil::Crit3DSoilClass soilClass[13];

        Crit3DProject();

        bool readCriteria3DParameters();

        bool loadModelParameters(QString dbName);
        bool loadSoilData(QString dbName);
        bool loadSoilMap(QString fileName);

        void cleanProject();
        bool initializeCriteria3D();

        bool createSoilIndexMap();
        bool createIndexMap();
        bool createBoundaryMap();

        double getSoilVar(int soilIndex, int layerIndex, soil::soilVariable myVar);
        double* getSoilVarProfile(int row, int col, soil::soilVariable myVar);
        bool setSoilProfileCrop(int row, int col);

        int getSoilIndex(int dtmRow, int dtmCol);

        bool setDEM(QString myFileName);

        bool interpolationRelHumidity(const Crit3DTime& myTime, gis::Crit3DRasterGrid *myRaster, bool showInfo);
        bool computeAllMeteoMaps(const Crit3DTime &myTime, bool showInfo);
    };


#endif // CRITERIA3DPROJECT_H

#ifndef CRIT3DPROJECT_H
#define CRIT3DPROJECT_H

    #ifndef SOIL_H
        #include "soil.h"
    #endif
    #ifndef GIS_H
        #include "gis.h"
    #endif
    #ifndef METEOMAPS_H
        #include "crit3dMeteoMaps.h"
    #endif
    #ifndef SOLARRADIATION_H
        #include "solarRadiation.h"
    #endif
    #ifndef VECTOR_H
        #include <vector>
    #endif

    class Crit3DProject
    {
    public:
        // extern map
        gis::Crit3DRasterGrid* dtm;
        Crit3DRadiationMaps* radiationMaps;

        // maps interpolated by the caller
        Crit3DMeteoMaps* meteoMaps;

        // maps loaded by the caller
        gis::Crit3DRasterGrid soilMap;

        // maps computed here
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

        float soilDepth;                                 //[m]
        std::vector <double> layerDepth;                 //[m]
        std::vector <double> layerThickness;             //[m]
        std::vector <soil::Crit3DSoil> soilList;
        soil::Crit3DSoilClass soilClass[13];

        std::string error;
        std::string log;

        Crit3DProject();

        void closeProject();

        bool initializeProject(gis::Crit3DRasterGrid* myDTM, Crit3DRadiationMaps *myRadiationMaps);

        bool createSoilIndexMap();
        bool createIndexMap();
        bool createBoundaryMap();

        double getSoilVar(int soilIndex, int layerIndex, soil::soilVariable myVar);
        double* getSoilVarProfile(int row, int col, soil::soilVariable myVar);
        bool setSoilProfileCrop(int row, int col);

        int getSoilIndex(int dtmRow, int dtmCol);
        bool computeET0Map();

        void setError(std::string myError);
        void setLog(std::string myLog);
    };

#endif // CRIT3DPROJECT_H

#ifndef CRITERIA3DPROJECT_H
#define CRITERIA3DPROJECT_H

    #define CRITERIA3D

    #ifndef CRIT3DPROJECT_H
        #include "crit3dProject.h"
    #endif

    #ifndef PROJECT_H
        #include "project.h"
    #endif

    #include "elaborationSettings.h"

    #include <QString>

    class Criteria3DProject : public Project
    {
    private:

    public:
        Crit3DElaborationSettings* elaborationSettings;
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

        float soilDepth;                                 //[m]
        std::vector <double> layerDepth;                 //[m]
        std::vector <double> layerThickness;             //[m]
        std::vector <soil::Crit3DSoil> soilList;
        soil::Crit3DSoilClass soilClass[13];

        Criteria3DProject();

        bool readCriteria3DSettings();

        bool loadModelParameters(QString dbName);
        bool loadSoilData(QString dbName);
        bool loadSoilMap(QString fileName);
        bool initializeCriteria3D();

        bool createSoilIndexMap();
        bool createIndexMap();
        bool createBoundaryMap();

        double getSoilVar(int soilIndex, int layerIndex, soil::soilVariable myVar);
        double* getSoilVarProfile(int row, int col, soil::soilVariable myVar);
        bool setSoilProfileCrop(int row, int col);

        int getSoilIndex(int dtmRow, int dtmCol);
        bool computeET0Map();

        void closeCriteria3DProject();
    };


#endif // CRITERIA3DPROJECT_H

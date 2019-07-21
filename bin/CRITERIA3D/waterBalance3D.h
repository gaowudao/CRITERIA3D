#ifndef WATERBALANCE3D_H
#define WATERBALANCE3D_H

    #ifndef GIS_H
        #include "gis.h"
    #endif
    #ifndef PROJECT3D_H
        #include "project3D.h"
    #endif

    class Crit3DProject;
    class Crit3DDate;


    int getSoilLayerIndex(Crit3DProject* myProject, double depth);
    double getSoilLayerTop(Crit3DProject* myProject, int i);
    double getSoilLayerBottom(Crit3DProject* myProject, int i);
    bool initializeWaterBalance3D(Crit3DProject* myProject);
    bool initializeSoilMoisture(Crit3DProject* myProject, int month);

    bool saveWaterBalanceState(Crit3DProject* myProject, Crit3DDate myDate, std::string myStatePath, criteria3DVariable myVar);

    bool loadWaterBalanceState(Crit3DProject* myProject, Crit3DDate myDate, std::string myStatePath, criteria3DVariable myVar);

    bool getCriteria3DVarMap(Crit3DProject* myProject, criteria3DVariable myVar, int layerIndex,
                             gis::Crit3DRasterGrid* criteria3DMap);

    bool getRootZoneAWCmap(Crit3DProject* myProject, gis::Crit3DRasterGrid* outputMap);

    bool getCriteria3DIntegrationMap(Crit3DProject* myProject, criteria3DVariable myVar,
                           double upperDepth, double lowerDepth, gis::Crit3DRasterGrid* criteria3DMap);

    bool saveWaterBalanceOutput(Crit3DProject* myProject, Crit3DDate myDate, criteria3DVariable myVar,
                                std::string varName, std::string outputPath, double upperDepth, double lowerDepth);

    double getMaxEvaporation(double ET0, double LAI);

    double* getCriteria3DVarProfile(Crit3DProject* myProject, int myRow, int myCol, criteria3DVariable myVar);

    double evaporation(Crit3DProject* myProject, int row, int col, long surfaceIndex);

    bool waterBalance(Crit3DProject* myProject);


#endif //WATERBALANCE3D_H

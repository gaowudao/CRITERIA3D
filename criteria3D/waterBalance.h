#ifndef WATERBALANCE_H
#define WATERBALANCE_H

    #ifndef GIS_H
        #include "gis.h"
    #endif
    #ifndef SOIL_H
        #include "soil.h"
    #endif
    #ifndef CRIT3DDATE_H
        #include "crit3dDate.h"
    #endif

    class Crit3DProject;

    enum criteria3DVariable {waterContent, waterTotalPotential, waterMatricPotential,
                            availableWaterContent, waterDeficit, degreeOfSaturation, soilTemperature,
                            soilSurfaceMoisture};

    void cleanWaterBalanceMemory();

    int computeNrLayers(float totalDepth, double minThickness, double maxThickness, double factor);
    int getSoilLayerIndex(Crit3DProject* myProject, double depth);
    double getSoilLayerTop(Crit3DProject* myProject, int i);
    double getSoilLayerBottom(Crit3DProject* myProject, int i);
    bool initializeWaterBalance(Crit3DProject* myProject);
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

    double getCriteria3DVar(criteria3DVariable myVar, long nodeIndex);

    double evaporation(Crit3DProject* myProject, int row, int col, long surfaceIndex);

    bool waterBalance(Crit3DProject* myProject);


#endif //WATERBALANCE_H

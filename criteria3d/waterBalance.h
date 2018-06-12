#ifndef WATERBALANCE_H
#define WATERBALANCE_H

#ifndef GIS_H
    #include "gis.h"
#endif

#ifndef CRIT3DDATE_H
    #include "crit3dDate.h"
#endif
#ifndef QSTRING_H
    #include <QString>
#endif
#ifndef QDATETIME_H
    #include <QDateTime>
#endif


    enum criteria3DVariable {waterContent, waterTotalPotential, waterMatricPotential,
                            availableWaterContent, waterDeficit, degreeOfSaturation, soilTemperature,
                            soilSurfaceMoisture};

    enum soilVariable {soilWaterContentFC, soilWaterContentWP, soilWaterPotentialWP,
                       soilWaterPotentialFC, soilWaterContentSat, soilWaterContentUI};

    class Crit3DProject;

    void cleanWaterBalanceMemory();

    int computeNrLayers(float totalDepth, float minThickness, float maxThickness, float factor);
    int getSoilLayerIndex(Crit3DProject* myProject, double depth);
    double getSoilLayerTop(Crit3DProject* myProject, int i);
    double getSoilLayerBottom(Crit3DProject* myProject, int i);
    bool initializeWaterBalance(Crit3DProject* myProject);
    bool initializeSoilMoisture(Crit3DProject* myProject, int month);

    bool saveWaterBalanceState(Crit3DProject* myProject, QDate myDate, QString myArea,
                               QString myStatePath, criteria3DVariable myVar);

    bool loadWaterBalanceState(Crit3DProject* myProject, QDate myDate, QString myArea,
                               QString myStatePath, criteria3DVariable myVar);

    bool waterBalance(Crit3DProject* myProject);

    bool getCriteria3DVarMap(Crit3DProject* myProject, criteria3DVariable myVar, int layerIndex,
                             gis::Crit3DRasterGrid* criteria3DMap);
    bool getRootZoneAWCmap(Crit3DProject* myProject, gis::Crit3DRasterGrid* outputMap);

    bool getCriteria3DIntegrationMap(Crit3DProject* myProject, criteria3DVariable myVar,
                           double upperDepth, double lowerDepth, gis::Crit3DRasterGrid* criteria3DMap);

    bool saveWaterBalanceOutput(Crit3DProject* myProject, QDate myDate, criteria3DVariable myVar,
                                QString varName, QString notes, QString outputPath, QString myArea,
                                double upperDepth, double lowerDepth);

    double getMaxEvaporation(float ET0, float LAI);

    double* getCriteria3DVarProfile(Crit3DProject* myProject, int myRow, int myCol, criteria3DVariable myVar);
    double* getSoilVarProfile(Crit3DProject* myProject, int myRow, int myCol, soilVariable myVar);
    double getCriteria3DVar(criteria3DVariable myVar, long nodeIndex);

    double evaporation(Crit3DProject* myProject, int row, int col, long surfaceIndex);


#endif // CRITERIA3DMODEL_H

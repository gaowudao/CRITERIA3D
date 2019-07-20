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
                            availableWaterContent, degreeOfSaturation, soilTemperature,
                            soilSurfaceMoisture, bottomDrainage, waterInflow, waterOutflow};

    enum soilVariable {soilWaterContentFC, soilWaterContentWP, soilWiltingPointPotential,
                       soilFieldCapacityPotential, soilSaturation, soilWaterContentUI};

    class Vine3DProject;


    class Crit3DWaterBalanceMaps
    {
        public:
            gis::Crit3DRasterGrid* bottomDrainageMap;
            gis::Crit3DRasterGrid* waterInflowMap;

            Crit3DWaterBalanceMaps();
            Crit3DWaterBalanceMaps(const gis::Crit3DRasterGrid &myDEM);

            void initialize();
            void initializeWithDEM(const gis::Crit3DRasterGrid &myDEM);
            gis::Crit3DRasterGrid* getMapFromVar(criteria3DVariable myVar);
    };

    void cleanWaterBalanceMemory();

    int computeNrLayers(float totalDepth, float minThickness, float maxThickness, float factor);
    int getLayerIndex(Vine3DProject* myProject, double depth);
    double getLayerTop(Vine3DProject* myProject, int i);
    double getLayerBottom(Vine3DProject* myProject, int i);
    bool isWithinSoil(Vine3DProject* myProject, long row, long col, double depth);

    bool initializeWaterBalance(Vine3DProject* myProject);
    bool initializeSoilMoisture(Vine3DProject* myProject, int month);

    bool saveWaterBalanceState(Vine3DProject* myProject, QDate myDate, QString myArea,
                               QString myStatePath, criteria3DVariable myVar);

    bool loadWaterBalanceState(Vine3DProject* myProject, QDate myDate, QString myArea,
                               QString myStatePath, criteria3DVariable myVar);

    bool waterBalance(Vine3DProject* myProject);

    bool getCriteria3DVarMap(Vine3DProject* myProject, criteria3DVariable myVar, int layerIndex,
                             gis::Crit3DRasterGrid* criteria3DMap);
    bool getRootZoneAWCmap(Vine3DProject* myProject, gis::Crit3DRasterGrid* outputMap);

    bool getCriteria3DIntegrationMap(Vine3DProject* myProject, criteria3DVariable myVar,
                           double upperDepth, double lowerDepth, gis::Crit3DRasterGrid* criteria3DMap);

    bool saveWaterBalanceOutput(Vine3DProject* myProject, QDate myDate, criteria3DVariable myVar,
                                QString varName, QString notes, QString outputPath, QString myArea,
                                double upperDepth, double lowerDepth);

    bool saveWaterBalanceCumulatedOutput(Vine3DProject* myProject, QDate myDate, criteria3DVariable myVar,
                                QString varName, QString notes, QString outputPath, QString myArea);

    double getMaxEvaporation(float ET0, float LAI);

    double* getCriteria3DVarProfile(Vine3DProject* myProject, int myRow, int myCol, criteria3DVariable myVar);
    double* getSoilVarProfile(Vine3DProject* myProject, int myRow, int myCol, soilVariable myVar);
    double getCriteria3DVar(criteria3DVariable myVar, long nodeIndex);

    double evaporation(Vine3DProject* myProject, int row, int col);

    void resetWaterBalanceMap(Vine3DProject* myProject);
    void updateWaterBalanceMaps(Vine3DProject* myProject);

#endif // WATERBALANCE_H

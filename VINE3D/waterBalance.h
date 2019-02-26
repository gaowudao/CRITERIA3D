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
                            soilSurfaceMoisture, bottomDrainage};

    enum soilVariable {soilWaterContentFC, soilWaterContentWP, soilWiltingPointPotential,
                       soilFieldCapacityPotential, soilSaturation, soilWaterContentUI};

    class Vine3DProject;


    class Crit3DWaterBalanceMaps
    {
        public:
            gis::Crit3DRasterGrid* bottomDrainageMap;

            Crit3DWaterBalanceMaps();
            Crit3DWaterBalanceMaps(const gis::Crit3DRasterGrid &myDtm);

            void initialize();
            void initializeWithDtm(const gis::Crit3DRasterGrid &dtm);
            void reset();
            void update(Vine3DProject *myProject);
            gis::Crit3DRasterGrid* getMapFromVar(criteria3DVariable myVar);
    };

    void cleanWaterBalanceMemory();

    int computeNrLayers(float totalDepth, float minThickness, float maxThickness, float factor);
    int getLayerIndex(Vine3DProject* myProject, double depth);
    double getLayerTop(Vine3DProject* myProject, int i);
    double getLayerBottom(Vine3DProject* myProject, int i);
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

    double getMaxEvaporation(float ET0, float LAI);

    double* getCriteria3DVarProfile(Vine3DProject* myProject, int myRow, int myCol, criteria3DVariable myVar);
    double* getSoilVarProfile(Vine3DProject* myProject, int myRow, int myCol, soilVariable myVar);
    double getCriteria3DVar(criteria3DVariable myVar, long nodeIndex);

    double evaporation(Vine3DProject* myProject, int row, int col);


#endif // WATERBALANCE_H

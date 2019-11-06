#ifndef CRITERIA3DPROJECT_H
#define CRITERIA3DPROJECT_H

    #define CRITERIA3D

    #ifndef SOIL_H
        #include "soil.h"
    #endif
    #ifndef PROJECT_H
        #include "project.h"
    #endif
    #ifndef PROJECT3D_H
        #include "project3D.h"
    #endif

    #include <QString>

    class Crit3DProject : public Project3D
    {
    private:
        void clearCriteria3DProject();

        bool interpolateAndSaveHourlyMeteo(meteoVariable myVar, const QDateTime& myTime,
                                           const QString& outputPath, bool saveOutput);


    public:
        // same header of DEM
        gis::Crit3DRasterGrid soilMap;
        gis::Crit3DRasterGrid cropIndexMap;

        bool isParametersLoaded;

        Crit3DProject();

        bool loadCriteria3DProject(QString myFileName);

        bool loadCriteria3DSettings();

        bool loadModelParameters(QString dbName);

        bool loadSoilMap(QString fileName);

        bool initializeCriteria3DModel();

        bool setSoilIndexMap();

        double getSoilVar(int soilIndex, int layerIndex, soil::soilVariable myVar);
        double* getSoilVarProfile(int row, int col, soil::soilVariable myVar);
        bool setSoilProfileCrop(int row, int col);

        int getCrit3DSoilId(double x, double y);
        QString getCrit3DSoilCode(double x, double y);
        int getCrit3DSoilIndex(double x, double y);

        bool interpolationRelHumidity(const Crit3DTime& myTime, gis::Crit3DRasterGrid *myRaster, bool showInfo);
        bool computeAllMeteoMaps(const Crit3DTime &myTime, bool showInfo);

        void setAllHourlyMeteoMapsComputed(bool value);

        bool saveStateAndOutput(QDate myDate, const QString& outputPathHourly, bool saveOutput);

        bool modelHourlyCycle(bool isInitialState, QDateTime myTime, const QString& outputPath, bool saveOutput);
    };


#endif // CRITERIA3DPROJECT_H

#ifndef PRAGAPROJECT_H
#define PRAGAPROJECT_H

#define PRAGA

#ifndef CRIT3DCLIMATE_H
    #include "crit3dClimate.h"
#endif

#ifndef PROJECT_H
    #include "project.h"
#endif

    class PragaProject : public Project
    {
    private:

    public:

        gridAggregationMethod grdAggrMethod;

        Crit3DClimate* clima;
        Crit3DClimate* climaFromDb;
        Crit3DClimate* referenceClima;

        bool isElabMeteoPointsValue;

        PragaProject();

        bool readPragaSettings();

        bool interpolationMeteoGrid(meteoVariable myVar, frequencyType myFrequency, const Crit3DTime& myTime, gis::Crit3DRasterGrid *myRaster, bool showInfo);
        bool saveGrid(meteoVariable myVar, frequencyType myFrequency, const Crit3DTime& myTime, bool showInfo);

        bool elaborationPointsCycle(bool isAnomaly, bool showInfo);
        bool elaborationPointsCycleGrid(bool isAnomaly, bool showInfo);
        bool elaborationCheck(bool isMeteoGrid, bool isAnomaly);
        bool elaboration(bool isMeteoGrid, bool isAnomaly, bool saveClima);
        bool climatePointsCycle(bool showInfo);
        bool climatePointsCycleGrid(bool showInfo);

        bool getIsElabMeteoPointsValue() const;
        void setIsElabMeteoPointsValue(bool value);
    };

#endif // PRAGAPROJECT_H

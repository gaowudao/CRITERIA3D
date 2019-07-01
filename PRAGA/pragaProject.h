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

        aggregationMethod grdAggrMethod;

        Crit3DClimate* clima;
        Crit3DClimate* climaFromDb;
        Crit3DClimate* referenceClima;

        bool isElabMeteoPointsValue;
        QString climateIndex;

        PragaProject();

        bool loadPragaSettings();

        bool downloadDailyDataArkimet(QStringList variables, bool prec0024, QDate startDate, QDate endDate, bool showInfo);
        bool downloadHourlyDataArkimet(QStringList variables, QDate startDate, QDate endDate, bool showInfo);

        bool interpolationMeteoGrid(meteoVariable myVar, frequencyType myFrequency, const Crit3DTime& myTime, gis::Crit3DRasterGrid *myRaster, bool showInfo);
        bool saveGrid(meteoVariable myVar, frequencyType myFrequency, const Crit3DTime& myTime, bool showInfo);

        bool elaborationPointsCycle(bool isAnomaly, bool showInfo);
        bool elaborationPointsCycleGrid(bool isAnomaly, bool showInfo);
        bool elaborationCheck(bool isMeteoGrid, bool isAnomaly);
        bool elaboration(bool isMeteoGrid, bool isAnomaly, bool saveClima);
        bool showClimateFields(bool isMeteoGrid, QStringList *climateDbElab, QStringList *climateDbVarList);
        void saveClimateResult(bool isMeteoGrid, QString climaSelected, int climateIndex, bool showInfo);
        bool deleteClima(bool isMeteoGrid, QString climaSelected);
        bool climatePointsCycle(bool showInfo);
        bool climatePointsCycleGrid(bool showInfo);
        std::vector<std::vector<float> > averageSeriesOnZonesMeteoGrid(meteoVariable variable, meteoComputation elab1MeteoComp, aggregationMethod spatialElab, float threshold, gis::Crit3DRasterGrid* zoneGrid, QDate startDate, QDate endDate, std::vector<float> &outputValues, bool showInfo);

        bool getIsElabMeteoPointsValue() const;
        void setIsElabMeteoPointsValue(bool value);
    };

#endif // PRAGAPROJECT_H

#ifndef ATMOSPHERE_H
#define ATMOSPHERE_H

    #ifndef QSTRING_H
        #include <QString>
    #endif
    #ifndef CRIT3DDATE_H
        #include "crit3dDate.h"
    #endif
    #ifndef METEO_H
        #include "meteo.h"
    #endif

    class Crit3DProject;

    enum interpolationType {interpolationTypeDtm, interpolationTypeMeteoPoint};
    enum aggregationType {aggregationMin, aggregationMax, aggregationMean, aggregationSum, aggregationIntegration};

    bool interpolationProjectDtm(Crit3DProject* myProject, meteoVariable myVar, const Crit3DTime& myTime, bool loadData);

    bool computeRadiationProjectDtm(Crit3DProject* myProject, const Crit3DTime& myTime, bool loadData);

    bool interpolationProjectDtmMain(Crit3DProject* myProject, meteoVariable myVar, const Crit3DTime& myTime, bool isLoadData);

    bool saveMeteoHourlyOutput(Crit3DProject* myProject, meteoVariable myVar,
                                const QString& myOutputPath, Crit3DTime myCrit3DTime,
                                const QString myArea);

    bool interpolateAndSaveHourlyMeteo(Crit3DProject* myProject, meteoVariable myVar,
                            const Crit3DTime& myCrit3DTime, const QString& myOutputPath,
                            bool isSave, const QString& myArea);

    bool aggregateAndSaveDailyMap(Crit3DProject* myProject, meteoVariable myVar,
                             aggregationType myAggregation, const Crit3DDate& myDate,
                             const QString& dailyPath, const QString& hourlyPath, const QString& myArea);

    bool loadDailyMeteoMap(Crit3DProject* myProject, meteoVariable myDailyVar, QDate myDate,
                           const QString& myArea);

    bool computeET0Map(Crit3DProject* myProject);
    bool computeLeafWetnessMap(Crit3DProject* myProject);

    bool checkLackOfData(Crit3DProject* myProject, meteoVariable myVar, Crit3DTime myDateTime, long* nrReplacedData);

#endif // ATMOSPHERE_H

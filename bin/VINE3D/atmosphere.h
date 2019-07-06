#ifndef ATMOSPHERE_H
#define ATMOSPHERE_H

    #ifndef QSTRING_H
        #include <QString>
    #endif
    #ifndef CRIT3DDATE_H
        #include "crit3dDate.h"
    #endif
    #ifndef METEOPOINT_H
        #include "meteoPoint.h"
    #endif

    class Vine3DProject;

    enum interpolationType {interpolationTypeDtm, interpolationTypeMeteoPoint};
    enum aggregationType {aggregationMin, aggregationMax, aggregationMean, aggregationSum, aggregationIntegration};

    bool vine3DInterpolationDem(Vine3DProject* myProject, meteoVariable myVar, const Crit3DTime& myTime, bool loadData);

    bool computeRadiationProjectDtm(Vine3DProject* myProject, const Crit3DTime& myTime, bool loadData);

    bool interpolationProjectDtmMain(Vine3DProject* myProject, meteoVariable myVar, const Crit3DTime& myTime, bool isLoadData);

    bool saveMeteoHourlyOutput(Vine3DProject* myProject, meteoVariable myVar,
                                const QString& myOutputPath, Crit3DTime myCrit3DTime,
                                const QString myArea);

    bool interpolateAndSaveHourlyMeteo(Vine3DProject* myProject, meteoVariable myVar,
                            const Crit3DTime& myCrit3DTime, const QString& myOutputPath,
                            bool isSave, const QString& myArea);

    bool aggregateAndSaveDailyMap(Vine3DProject* myProject, meteoVariable myVar,
                             aggregationType myAggregation, const Crit3DDate& myDate,
                             const QString& dailyPath, const QString& hourlyPath, const QString& myArea);

    bool loadDailyMeteoMap(Vine3DProject* myProject, meteoVariable myDailyVar, QDate myDate,
                           const QString& myArea);

    bool computeET0Map(Vine3DProject* myProject);
    bool computeLeafWetnessMap(Vine3DProject* myProject);
    void qualityControl(Vine3DProject* myProject, meteoVariable myVar, const Crit3DTime& myCrit3DTime);

    bool checkLackOfData(Vine3DProject* myProject, meteoVariable myVar, Crit3DTime myDateTime, long* nrReplacedData);

#endif // ATMOSPHERE_H

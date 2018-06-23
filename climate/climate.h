#ifndef CLIMATE_H
#define CLIMATE_H


#include <QString>
#include <QDate>

#ifndef METEO_H
    #include "meteo.h"
#endif

#ifndef DBMETEOGRID_H
    #include "dbMeteoPoints.h"
#endif

#ifndef DBMETEOGRID_H
    #include "dbMeteoGrid.h"
#endif

// LC dove mettere queste define? Teasformate in enum per fare switch case
enum elaboration { phenologyElab, winklerElab, huglinElab, fregoniElab, thomHourlyElab, thomDailyMaxElab, thomDailyMeanElab, thomDailyHoursAboveElab, thomDayTimeElab, thomNightTimeElab,
                   correctedDegreeDaysSumElab, lastDayBelowThresholdElab, erosivityFactorElab, rainIntensityElab, koppenElab, dailyLeafWetnessElab, dailyBICElab, dailyTemperatureRangeElab,
                   dailyTemperatureavgElab, dailyETPElab, dailyTdminElab, dailyTdmaxElab, dailyTdAvgElab
};

/*
#define ELABORATION_PHENOLOGY  "Phenology"
#define ELABORATION_WINKLER  "Winkler"
#define ELABORATION_HUGLIN  "Huglin"
#define ELABORATION_FREGONI  "Fregoni"
#define ELABORATION_THOM_HOURLY  "ThomH"
#define ELABORATION_THOM_DAILYMAX  "ThomMax"
#define ELABORATION_THOM_DAILYMEAN  "ThomMean"
#define ELABORATION_THOM_DAILYHOURSABOVE  "ThomHours"
#define ELABORATION_THOM_DAYTIME  "ThomDayTime"               // calcolato da RHmax e tmin
#define ELABORATION_THOM_NIGHTTIME  "ThomNightTime"           // calcolato da RHmax e tmin
#define ELABORATION_CORRECTED_SUM  "CorrectedDegreeDaysSum"
#define ELABORATION_LASTDAYBELOWTHRESHOLD  "LastDayBelowThreshold"
#define ELABORATION_EROSIVITY "ErosivityFactor"
#define ELABORATION_RAININTENSITY  "RainIntensity"
#define ELABORATION_KOPPEN  "Koppen"
#define ELABORATION_PHENO  "Phenology"
#define DAILY_LEAFWETNESS "LeafWetness"
#define DAILY_BIC "BIC"
#define DAILY_TEMPERATURE_RANGE "TRange"
#define DAILY_TAVG "Tmed"
#define DAILY_ETP "ETP"
#define DAILY_TDMIN "Tdmin"
#define DAILY_TDMAX "Tdmax"
#define DAILY_TDMED "Tdmed"
*/


bool elaborationPointsCycle(std::string *myError, Crit3DMeteoPointsDbHandler *meteoPointsDbHandler, QString variable, int firstYear, int lastYear, QDate firstDate, QDate lastDate, int nYears,
    QString elab1, bool param1IsClimate, QString param1ClimateField, float param1, QString elab2, float param2, bool isAnomaly,
    int nYearsMin, int firstYearClimate, int lastYearClimate);

bool elaborationPointsCycleGrid(std::string *myError, Crit3DMeteoGridDbHandler* meteoGridDbHandler, QString variable, int firstYear, int lastYear, QDate firstDate, QDate lastDate, int nYears,
    QString elab1, bool param1IsClimate, QString param1ClimateField, float param1, QString elab2, float param2, bool isAnomaly,
    int nYearsMin, int firstYearClimate, int lastYearClimate);

bool elaborationOnPoint(std::string *myError, Crit3DMeteoGridDbHandler* meteoGridDbHandler, Crit3DMeteoPointsDbHandler *meteoPointsDbHandler, Crit3DMeteoPoint *meteoPoint, bool pointOrGrid, meteoVariable variable, QString elab1, float param1,
    QString elab2, float param2, QDate startDate, QDate endDate, int nYears, int firstYear, int lastYear,
    int nYearsMin, bool isAnomaly, bool loadData);

frequencyType getAggregationFrequency(QString elab);

bool elaborateDailyAggregatedVar(QString elab, Crit3DMeteoPoint* meteoPoint, std::vector<float>* dailyValues, float* percValue);

bool elaborateDailyAggregatedVarFromDaily(QString elab, Crit3DMeteoPoint* meteoPoint, std::vector<float>* dailyValues, float* percValue) ;

float thomDayTime(float tempMax, float relHumMinAir);

float thomNightTime(float tempMin, float relHumMaxAir);

bool preElaboration(Crit3DMeteoGridDbHandler* meteoGridDbHandler, Crit3DMeteoPointsDbHandler* meteoPointsDbHandler, Crit3DMeteoPoint* meteoPoint, bool pointOrGrid, meteoVariable variable, QString elab1,
    QDate startDate, QDate endDate, float* percValue);

std::vector<float> loadDailyVarSeries(std::string *myError, Crit3DMeteoPointsDbHandler *meteoPointsDbHandler, Crit3DMeteoGridDbHandler meteoGridDbHandler, Crit3DMeteoPoint* meteoPoint, bool pointOrGrid, meteoVariable variable, QDate first, QDate last, bool saveValue);

std::vector<float> loadDailyVarSeries(std::string *myError, Crit3DMeteoPointsDbHandler *meteoPointsDbHandler, Crit3DMeteoGridDbHandler meteoGridDbHandler, Crit3DMeteoPoint* meteoPoint, bool pointOrGrid, meteoVariable variable, QDate first, QDate last, bool saveValue);

#endif // CLIMATE_H

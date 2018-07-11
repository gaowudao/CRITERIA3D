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

#ifndef DBCLIMATE_H
    #include "dbClimate.h"
#endif

enum meteoComputation { average, stdDev, sum, maxInList, minInList,
                        differenceWithThreshold, lastDayBelowThreshold,
                        sumAbove, avgAbove, stdDevAbove,
                        percentile, median, freqPositive,
                        daysAbove, daysBelow, consecutiveDaysAbove, consecutiveDaysBelow,
                        prevailingWindDir,
                        trend, mannKendall,
                        phenology,
                        winkler, huglin, fregoni,
                        correctedDegreeDaysSum, erosivityFactorElab, rainIntensityElab};

const std::map<std::string, meteoComputation> MapMeteoComputation = {
  { "average", average },
  { "stdDev", stdDev },
  { "sum", sum },
  { "maxInList", maxInList },
  { "minInList", minInList },
  { "differenceWithThreshold", differenceWithThreshold },
  { "lastDayBelowThreshold", lastDayBelowThreshold },
  { "sumAbove", sumAbove },
  { "avgAbove", avgAbove },
  { "stdDevAbove", stdDevAbove },
  { "percentile", percentile },
  { "median", median },
  { "freqPositive", freqPositive },
  { "daysAbove", daysAbove },
  { "daysBelow", daysBelow },
  { "consecutiveDaysAbove", consecutiveDaysAbove },
  { "consecutiveDaysBelow", consecutiveDaysBelow },
  { "prevailingWindDir", prevailingWindDir },
  { "trend", trend },
  { "mannKendall", mannKendall },
  { "phenology", phenology },
  { "winkler", winkler },
  { "huglin", huglin },
  { "fregoni", fregoni },
  { "correctedDegreeDaysSum", correctedDegreeDaysSum },
  { "erosivityFactorElab", erosivityFactorElab },
  { "rainIntensityElab", rainIntensityElab }
};


bool elaborationPointsCycle(std::string *myError, Crit3DMeteoPointsDbHandler *meteoPointsDbHandler, Crit3DMeteoPoint* meteoPoints,
    int nrMeteoPoints, Crit3DClimate* clima, QString variable, int firstYear, int lastYear, QDate firstDate, QDate lastDate, int nYears,
    QString elab1, bool param1IsClimate, QString param1ClimateField, float param1, QString elab2,
    float param2, bool isAnomaly, int nYearsMin, int firstYearClimate, int lastYearClimate);

bool elaborationPointsCycleGrid(std::string *myError, Crit3DMeteoGridDbHandler* meteoGridDbHandler, Crit3DClimate* clima,
    QString variable, int firstYear, int lastYear, QDate firstDate, QDate lastDate, int nYears,
    QString elab1, bool param1IsClimate, QString param1ClimateField, float param1, QString elab2,
    float param2, bool isAnomaly, int nYearsMin, int firstYearClimate, int lastYearClimate);

bool elaborationOnPoint(std::string *myError, Crit3DMeteoPointsDbHandler *meteoPointsDbHandler, Crit3DMeteoGridDbHandler *meteoGridDbHandler, Crit3DMeteoPoint *meteoPoint,
    bool isMeteoGrid, meteoVariable variable, QString elab1, float param1,
    QString elab2, float param2, QDate startDate, QDate endDate, int nYears, int firstYear, int lastYear,
    int nYearsMin, bool isAnomaly, bool loadData);

frequencyType getAggregationFrequency(meteoVariable myVar);

bool elaborateDailyAggregatedVar(meteoVariable myVar, Crit3DMeteoPoint meteoPoint, std::vector<float> *outputValues, float* percValue);

bool elaborateDailyAggregatedVarFromDaily(meteoVariable myVar, Crit3DMeteoPoint meteoPoint, std::vector<float> *outputValues, float* percValue);

bool elaborateDailyAggregatedVarFromHourly(meteoVariable myVar, Crit3DMeteoPoint meteoPoint, std::vector<float>* outputValues);

bool anomalyOnPoint(Crit3DMeteoPoint* meteoPoint, float refValue);

float thomDayTime(float tempMax, float relHumMinAir);

float thomNightTime(float tempMin, float relHumMaxAir);

float thomH(float tempAvg, float relHumAvgAir);

int thomDailyNHoursAbove(float *tempAvg, float *relHumAvgAir);

float thomDailyMax(float *tempAvg, float* relHumAvgAir);

float thomDailyMean(float *tempAvg, float* relHumAvgAir);

float dailyLeafWetnessComputation(int *leafW);

float computeDailyBIC(float prec, float etp);

float dailyThermalRange(float Tmin, float Tmax);

float dailyAverageT(float Tmin, float Tmax);

float dailyEtpHargreaves(float Tmin, float Tmax, Crit3DDate date, double latitude);

float dewPoint(float relHumAir, float tempAir);

bool preElaboration(std::string *myError, Crit3DMeteoPointsDbHandler* meteoPointsDbHandler, Crit3DMeteoGridDbHandler* meteoGridDbHandler, Crit3DMeteoPoint* meteoPoint, bool isMeteoGrid, meteoVariable variable, QString elab1,
    QDate startDate, QDate endDate, std::vector<float> *outputValues, float* percValue);

float loadDailyVarSeries(std::string *myError, Crit3DMeteoPointsDbHandler *meteoPointsDbHandler, Crit3DMeteoGridDbHandler *meteoGridDbHandler, Crit3DMeteoPoint* meteoPoint, bool isMeteoGrid, meteoVariable variable, QDate first, QDate last, std::vector<float>* outputValues);

float loadHourlyVarSeries(std::string *myError, Crit3DMeteoPointsDbHandler *meteoPointsDbHandler, Crit3DMeteoGridDbHandler *meteoGridDbHandler, Crit3DMeteoPoint* meteoPoint, bool isMeteoGrid, meteoVariable variable, QDateTime first, QDateTime last, std::vector<float>* outputValues);

period getPeriodTypeFromString(QString periodStr);

bool parserGenericPeriodString(Crit3DClimate* clima);

bool parserElaboration(Crit3DClimate* clima);

int nParameters(meteoComputation elab);

void extractValidValuesCC(std::vector<float>* outputValues);

void extractValidValuesWithThreshold(std::vector<float>* outputValues, float myThreshold);

int getClimateIndexFromDate(QDate myDate, period periodType);


#endif // CLIMATE_H

#ifndef STATISTICS_H
#define STATISTICS_H

#include <vector>
#include <string>
#ifndef CRIT3DDATE_H
    #include "crit3dDate.h"
#endif

enum statisticalElaborations {ELAB_MEAN, ELAB_STDDEVIATION, ELAB_SUM, ELAB_SUM_WITH_THRESHOLD, ELAB_MEAN_ABOVE_THRESHOLD,
                              ELAB_STDDEV_ABOVE_THRESHOLD, ELAB_MAX, ELAB_MIN, ELAB_PERCENTILE, ELAB_MEDIAN,
                              ELAB_FREQUENCY_POSITIVE, ELAB_DAYS_ABOVE_THRESHOLD, ELAB_DAYS_UNDER_THRESHOLD,
                              ELAB_CONSECUTIVE_DAYS_ABOVE_THRESHOLD, ELAB_CONSECUTIVE_DAYS_UNDER_THRESHOLD,
                              ELAB_PREVAILING_DIR, ELAB_TREND, ELAB_MANNKENDALL, ELAB_THRESHOLD_DIFFERENCE,
                              ELAB_WHEIGTHED_AVERAGE, ELAB_PREVAILING_X, ELAB_PREVAILING_Y, ELAB_PREVAILINGVALUE,
                              ELAB_PREVAILINGVALUE_CONSERVATIVE, ELAB_CENTERVALUE, ELAB_RENAME, ELAB_EROSIVITY, ELAB_RAININTENSITY};

namespace elaborations
{
    float statisticalElab(std::string elab, float param, std::vector<float> values, int nValues);
    float computeStatistic(std::string variable, int firstYear, int lastYear, Crit3DDate firstDate, Crit3DDate lastDate, int nYears, std::string elab1, float param1, std::string elab2, float param2, float myHeight);
}
namespace statistics
{
    double rootMeanSquareError(double *measured , double *simulated , int nrData);
    float rootMeanSquareError(float *measured , float *simulated , int nrData);
    float coefficientOfVariation(float *measured , float *simulated , int nrData);
    float weighedMean(float *data , float *weights, int nrData);
    void linearRegression(float* x, float* y, long nrItems, bool zeroIntercept, float* y_intercept, float* mySlope, float* r2);
    void linearRegression( std::vector<float> x,  std::vector<float> y, long nrItems, bool zeroIntercept, float* y_intercept, float* mySlope, float* r2);
    float standardDeviation(float *myList, int nrList);
    float standardDeviation(std::vector<float> myList, int nrList);
    double standardDeviation(double *myList, int nrList);
    float variance(float *myList, int nrList);
    float variance(std::vector<float> myList, int nrList);
    double variance(double *myList, int nrList);
    float mean(float *myList, int nrList);
    float mean(std::vector<float> myList, int nrList);
    double mean(double *myList, int nrList);
    float covariance(float *myList1, int nrList1,float *myList2, int nrList2);
    double covariance(double *myList1, int nrList1,double *myList2, int nrList2);
    float coefficientPearson(float *myList1, int nrList1,float *myList2, int nrList2);
    float** covariancesMatrix(int nrRowCol, float**myLists,int nrLists);
    void correlationsMatrix(int nrRowCol, double**myLists,int nrLists, double** c);
    double ERF(double x, double accuracy);
    double ERFC(double x, double accuracy);
    double inverseERFC(double value, double accuracy);
    double inverseERF(double value, double accuracy);
    float maxList(std::vector<float> values, int nValue);
    float minList(std::vector<float> values, int nValue);
    float sumList(std::vector<float> values, int nValue);
    float sumListThreshold(std::vector<float> values, int nValue, float threshold);
    float diffListThreshold(std::vector<float> values, int nValue, float threshold);
    float countAbove(std::vector<float> values, int nValue, float threshold);
    float countBelow(std::vector<float> values, int nValue, float threshold);
    float countConsecutive(std::vector<float> values, int nValue, float threshold, bool isPositive);
    float frequencyPositive(std::vector<float> values, int nValue);
    float trend(std::vector<float> values, int nValues, float myFirstYear);
    float mannKendall(std::vector<float> values, int nValues);



}


#endif // STATISTICS_H

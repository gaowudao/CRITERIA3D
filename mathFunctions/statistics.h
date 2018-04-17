#ifndef STATISTICS_H
#define STATISTICS_H


namespace statistics
{
    double rootMeanSquareError(double *measured , double *simulated , int nrData);
    float rootMeanSquareError(float *measured , float *simulated , int nrData);
    float coefficientOfVariation(float *measured , float *simulated , int nrData);
    float weighedMean(float *data , float *weights, int nrData);
    void linearRegression(float* x, float* y, long nrItems, bool zeroIntercept, float* y_intercept, float* mySlope, float* r2);
    float standardDeviation(float *myList, int nrList);
    double standardDeviation(double *myList, int nrList);
    float variance(float *myList, int nrList);
    double variance(double *myList, int nrList);
    float mean(float *myList, int nrList);
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
}


#endif // STATISTICS_H

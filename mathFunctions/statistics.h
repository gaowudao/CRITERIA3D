#ifndef STATISTICS_H
#define STATISTICS_H


namespace statistics
{
    float rootMeanSquareError(float *measured , float *simulated , int nrData);
    float coefficientOfVariation(float *measured , float *simulated , int nrData);
    float weighedMean(float *data , float *weights, int nrData);
    void linearRegression(float* x, float* y, long nrItems, bool zeroIntercept, float* y_intercept, float* mySlope, float* r2);
    float standardDeviation(float *myList, int nrList);
    float variance(float *myList, int nrList);
    float mean(float *myList, int nrList);
    float covariance(float *myList1, int nrList1,float *myList2, int nrList2);
    float coefficientPearson(float *myList1, int nrList1,float *myList2, int nrList2);
    float** covariancesMatrix(int nrRowCol, float**myLists,int nrLists);
    float ERF(float x, float accuracy);
    float ERFC(float x, float accuracy);
    float inverseERFC(float value, float accuracy);
    float inverseERF(float value, float accuracy);
}


#endif // STATISTICS_H

#ifndef WEATHERGENERATOR2D_H
#define WEATHERGENERATOR2D_H



#endif // WEATHERGENERATOR2D_H



#define TOLERANCE_MULGETS 0.001
#define MAX_ITERATION_MULGETS 200
#include "meteoPoint.h"


/*
class precipitation
{
    void computeWG2DParameters(); // step 1


public:

    void compute();
    precipitation() {}

};


class temperature
{

public:
    void compute();
    temperature() {}

};
*/


struct TparametersModel{
    int yearOfSimulation;
    int distributionPrecipitation; //Select a distribution to generate daily precipitation amount,1: Multi-exponential or 2: Multi-gamma
    double precipitationThreshold;
};

class weatherGenerator2D
{
private:

    bool isPrecWG2D,isTempWG2D;
    int nrData;
    int nrDataWithout29February;
    int nrStations;
    TparametersModel parametersModel;
    int *month,*lengthMonth,*beginMonth;
    TObsDataD** obsDataD;

    //functions

    void precipitationOccurrence();
    void precipitationCompute();
    void precipitation29February(int idStation);
    void precipitationAmountsOccurences(int idStation, double* precipitationAmountsD,bool* precipitationOccurencesD);
    void precipitationP00P10(int idStation);

    void temperatureCompute();


public:
    // variables

    //functions
    weatherGenerator2D() {}
    bool initializeObservedData(int lengthDataSeries, int nrStations);
    void initializeParameters(double thresholdPrecipitation, int simulatedYears, int distributionType, bool computePrecWG2D, bool computeTempWG2D);
    void setObservedData(float*** weatherArray, int** dateArray);
    void computeWeatherGenerator2D();

};


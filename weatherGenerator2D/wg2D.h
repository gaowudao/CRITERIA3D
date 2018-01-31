#ifndef WEATHERGENERATOR2D_H
#define WEATHERGENERATOR2D_H



#endif // WEATHERGENERATOR2D_H



#define TOLERANCE_MODEL 0.001
#define MAX_ITERATION_MODEL 200
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

    int nrData;
    int nrStations;
    TparametersModel parametersModel;
    int *month,*lengthMonth,*beginMonth;
    TObsDataD** obsDataD;

    //functions
    void precipitationOccurrence();
    void precipitationCompute();


public:
    // variables

    //functions
    weatherGenerator2D() {}
    bool initializeObservedData(int lengthDataSeries, int nrStations);
    void initializeParameters(double thresholdPrecipitation, int simulatedYears, int distributionType);
    void setObservedData(float*** weatherArray, int** dateArray);
    void computeWeatherGenerator2D();

};


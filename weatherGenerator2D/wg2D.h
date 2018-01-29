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
    // classes
    //precipitation prec;
    //temperature temp;
    // variables
    double*** weatherObservations;
    int** dateObservations;
    int nrData;
    TparametersModel parametersModel;
    int *month,*lengthMonth,*beginMonth;
    TObsDataD** obsDataD;

    //functions


public:
    // variables

    //functions
    weatherGenerator2D() {}
    bool initializeObservedData(int lengthDataSeries, int nrStations, int nrVariables, int nrDate, double*** weatherArray, int** dateArray);
    void initializeParameters(double thresholdPrecipitation, int simulatedYears, int distributionType);

};




//bool setData(Tobservations observations, int nStations, TparametersModel parametersModel);


//void fillObservationArray(int nStations, int timeSeriesLength,double*** variables, int** date);

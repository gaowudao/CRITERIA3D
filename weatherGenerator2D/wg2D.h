#ifndef WEATHERGENERATOR2D_H
#define WEATHERGENERATOR2D_H

#define TOLERANCE_MODEL 0.001
#define MAX_ITERATION_MODEL 200

#endif // WEATHERGENERATOR2D_H
class precipitation
{

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



struct TparametersModel{
    int yearOfSimulation;
    int distributionPrecipitation; //Select a distribution to generate daily precipitation amount,1: Multi-exponential or 2: Multi-gamma
    double precipitationThreshold;
};

class weatherGenerator2D
{
private:
    // classes
    precipitation prec;
    temperature temp;
    // variables
    double*** weatherObservations;
    int** dateObservations;
    TparametersModel parametersModel;
    int *month,*lengthMonth,*beginMonth;

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

#ifndef WEATHERGENERATOR2D_H
#define WEATHERGENERATOR2D_H



#endif // WEATHERGENERATOR2D_H



#define TOLERANCE_MULGETS 0.001
#define MAX_ITERATION_MULGETS 200
#include "meteoPoint.h"

enum Tseason {DJF,MAM,JJA,SON};

struct ToccurrenceIndexSeasonal{
    double** meanP;
    double** stdDevP;
    double** meanFit;
    double** stdDevFit;
    double*** parMultiexp;
    double** binCenter;
    double** bin;
};


struct TprecOccurrence{
    float p00;
    float p10;
    int month;
};

struct TcorrelationVar{
    double meanValueMonthlyPrec1;
    double meanValueMonthlyPrec2;
    double covariance;
    double variance1, variance2;
};

struct TObsPrecDataD{
    Crit3DDate date;
    float prec;
    float amounts;
    float amountsLessThreshold;
    float occurrences;
};

struct TcorrelationMatrix{
    double** amount;
    double** occurrence;
    int month;
};

struct TseasonalCorrelationMatrix{
    double** amount;
    double** occurrence;
    int beginDoySeason;
    int lengthSeason;
    Tseason season;
};

struct TrandomMatrix{
    float** matrixM;
    float** matrixK;
    float** matrixOccurrences;
    int month;
};

struct TsimulatedPrecipitationAmounts{
    double** matrixM;
    double** matrixK;
    double** matrixAmounts;
    Tseason season;
};

struct TparametersModel{
    int yearOfSimulation;
    int distributionPrecipitation; //Select a distribution to generate daily precipitation amount,1: Multi-exponential or 2: Multi-gamma
    float precipitationThreshold;
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
    TObsPrecDataD** obsPrecDataD;
    TprecOccurrence** precOccurence;
    TcorrelationMatrix *correlationMatrix;
    TrandomMatrix *randomMatrix;
    ToccurrenceIndexSeasonal* occurrenceIndexSeasonal;
    TsimulatedPrecipitationAmounts *simulatedPrecipitationAmounts;

    //functions

    void precipitationCompute();
    void precipitation29February(int idStation);
    void precipitationAmountsOccurences(int idStation, double* precipitationAmountsD,bool* precipitationOccurencesD);
    void precipitationP00P10();
    void precipitationCorrelationMatrices();
    void precipitationMultisiteOccurrenceGeneration();
    void spatialIterationOccurrence(double ** M, double **K, double **occurrences, double** matrixOccurrence, double** normalizedMatrixRandom, double **transitionNormal, int lengthSeries);
    void precipitationMultiDistributionAmounts();
    void initializeOccurrenceIndex();
    void initializePrecipitationOutputs(int lengthSeason[]);
    void spatialIterationAmounts(double ** amountsCorrelationMatrix , double** randomMatrix, int length, double** occurrences, double** phatAlpha, double** phatBeta,double** simulatedPrecipitationAmounts);

    void temperatureCompute();


public:
    // variables

    //functions
    weatherGenerator2D() {}
    bool initializeData(int lengthDataSeries, int nrStations);
    void initializeParameters(float thresholdPrecipitation, int simulatedYears, int distributionType, bool computePrecWG2D, bool computeTempWG2D);
    void setObservedData(TObsDataD** observations);
    void computeWeatherGenerator2D();
    double inverseGammaFunction(double valueProbability, double alpha, double beta);
};


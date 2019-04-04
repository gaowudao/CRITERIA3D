#ifndef WEATHERGENERATOR2D_H
#define WEATHERGENERATOR2D_H

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
        double p00;
        double p10;
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
        double prec;
        double amounts;
        double amountsLessThreshold;
        double occurrences;
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
        double** matrixM;
        double** matrixK;
        double** matrixOccurrences;
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
        double precipitationThreshold;
    };

    struct TfourierParameters{
        double a0;
        double aSin1;
        double aCos1;
        double aSin2;
        double aCos2;
    };

    struct Tvariable{
        TfourierParameters averageFourierParameters;
        TfourierParameters standardDeviationFourierParameters;
        double estimation[365];
    };

    struct TtemperatureCoefficients{
        double A[2][2];
        double B[2][2];
        Tvariable minTWet;
        Tvariable minTDry;
        Tvariable maxTWet;
        Tvariable maxTDry;
    };


    void randomSet(double *arrayNormal,int dimArray);
    class weatherGenerator2D
    {
    private:

        bool isPrecWG2D,isTempWG2D;
        int nrData;
        int nrDataWithout29February;
        int nrStations;
        TparametersModel parametersModel;
        int *month,*lengthMonth,*beginMonth;
        int lengthSeason[4];
        int numberObservedDJF,numberObservedMAM,numberObservedJJA,numberObservedSON;
        int numberObservedMax;
        TObsDataD** obsDataD;
        TObsPrecDataD** obsPrecDataD;
        TprecOccurrence** precOccurence;
        TcorrelationMatrix *correlationMatrix;
        TrandomMatrix *randomMatrix;
        ToccurrenceIndexSeasonal* occurrenceIndexSeasonal;
        TsimulatedPrecipitationAmounts *simulatedPrecipitationAmounts;

        // create the seasonal correlation matrices
        double** occurrenceMatrixSeasonDJF;
        double** occurrenceMatrixSeasonMAM;
        double** occurrenceMatrixSeasonJJA;
        double** occurrenceMatrixSeasonSON;

        double** wDJF ;
        double** wMAM;
        double** wJJA;
        double** wSON;
        double** wSeason;

        // variables only for temperatures
        TtemperatureCoefficients* temperatureCoefficients;


        //functions

        void precipitationCompute();
        void precipitation29February(int idStation);
        void precipitationAmountsOccurences(int idStation, double* precipitationAmountsD,bool* precipitationOccurencesD);
        void precipitationP00P10();
        void precipitationCorrelationMatrices();
        void precipitationMultisiteOccurrenceGeneration();
        void spatialIterationOccurrence(double ** M, double **K, double **occurrences, double** matrixOccurrence, double** normalizedMatrixRandom, double **transitionNormal, int lengthSeries);
        void precipitationMultiDistributionParameterization();
        void precipitationMultisiteAmountsGeneration();
        void initializeOccurrenceIndex();
        void initializePrecipitationOutputs(int lengthSeason[]);
        void spatialIterationAmounts(double** correlationMatrixSimulatedData,double ** amountsCorrelationMatrix , double** randomMatrix, int length, double** occurrences, double** phatAlpha, double** phatBeta,double** simulatedPrecipitationAmounts);
        double bestFit(double *par, int nrPar, double*x, double *yObs, int nrX);
        int bestParametersNonLinearFit(double *par, int nrPar, double*x, double *yObs, int nrX);



        void temperatureCompute();
        void computeTemperatureParameters();
        void initializeTemperatureParameters();
        int  doyFromDate(int day,int month,int year);
        void harmonicsFourier(double* variable, double *par, int nrPar, double estimatedVariable[], int nrEstimatedVariable);

    public:
        // variables

        //functions
        weatherGenerator2D() {}
        bool initializeData(int lengthDataSeries, int nrStations);
        void initializeParameters(float thresholdPrecipitation, int simulatedYears, int distributionType, bool computePrecWG2D, bool computeTempWG2D);
        void setObservedData(TObsDataD** observations);
        void computeWeatherGenerator2D();
        double inverseGammaFunction(double valueProbability, double alpha, double beta, double accuracy);
        void pressEnterToContinue();
    };

#endif // WEATHERGENERATOR2D_H

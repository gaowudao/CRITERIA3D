#ifndef WEATHERGENERATOR2D_H
#define WEATHERGENERATOR2D_H

    #ifndef METEOPOINT_H
        #include "meteoPoint.h"
    #endif

    #define TOLERANCE_MULGETS 0.001
    #define MAX_ITERATION_MULGETS 200
    #define ONELESSEPSILON 0.999999


    enum Tseason {DJF,MAM,JJA,SON};

    struct TseasonPrec{
        double* DJF;
        double* MAM;
        double* JJA;
        double* SON;
    };

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
        double meanValue1;
        double meanValue2;
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

    struct TcorrelationMatrixTemperature{
        double** maxT;
        double** minT;
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
        double* averageEstimation;
        double* stdDevEstimation;
    };

    struct TtemperatureCoefficients{
        double A[2][2];
        double B[2][2];
        Tvariable minTWet;
        Tvariable minTDry;
        Tvariable maxTWet;
        Tvariable maxTDry;
    };

    struct TdailyResidual{
        double maxTDry;
        double minTDry;
        double maxTWet;
        double minTWet;
        double maxT;
        double minT;

    };

    struct TmultiOccurrenceTemperature
    {
        int year_simulated;
        int month_simulated;
        int day_simulated;
        double* occurrence_simulated;
    };

    struct ToutputWeatherData
    {
        int* yearSimulated;
        int* monthSimulated;
        int* daySimulated;
        int* doySimulated;
        double* maxT;
        double* minT;
        double* precipitation;

    };


    //void randomSet(double *arrayNormal,int dimArray);
    class weatherGenerator2D
    {
    private:

        bool isPrecWG2D,isTempWG2D;
        int nrData;
        int nrDataWithout29February;
        int nrStations;
        TparametersModel parametersModel;
        int *month,*lengthMonth;
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
        //double** tagDoyPrec;

        // create the seasonal correlation matrices
        double** occurrenceMatrixSeasonDJF;
        double** occurrenceMatrixSeasonMAM;
        double** occurrenceMatrixSeasonJJA;
        double** occurrenceMatrixSeasonSON;

        double** wDJF;
        double** wMAM;
        double** wJJA;
        double** wSON;
        double** wSeason;
        // new distribution for precipitation
        Tvariable* precipitationAmount;
        TseasonPrec* seasonPrec;
        double** precGenerated;
        void initializePrecipitationAmountParameters();
        void computeprecipitationAmountParameters();
        void getSeasonalMeanPrecipitation(int iStation, int iSeason, int length, double* meanPrec);
        void getPrecipitationAmount();
        void spatialIterationAmountsMonthly(int iMonth, double** correlationMatrixSimulatedData,double ** amountsCorrelationMatrix , double** randomMatrix, int lengthSeries, double** occurrences, double** simulatedPrecipitationAmountsSeasonal);


        // variables only for temperatures
        TtemperatureCoefficients* temperatureCoefficients;
        TdailyResidual* dailyResidual;
        TcorrelationMatrixTemperature correlationMatrixTemperature;
        double** normRandomMaxT;
        double** normRandomMinT;
        TmultiOccurrenceTemperature* multiOccurrenceTemperature;
        double** maxTGenerated;
        double** minTGenerated;
        double** occurrencePrecGenerated;
        double** amountsPrecGenerated;

        ToutputWeatherData *outputWeatherData;

        double* normalRandomNumbers;
        //int contatoreGammaUguale = 0;
        //int contatoreGammaUguale2 = 0;
        //int contatoreGammaDiverso = 0;
        //functions
        void deallocateMemoryPointers();
        void commonModuleCompute();
        void precipitationCompute();
        void precipitation29February(int idStation);
        void precipitationAmountsOccurences(int idStation, double* precipitationAmountsD,bool* precipitationOccurencesD);
        void precipitationP00P10();
        void precipitationCorrelationMatrices();
        void precipitationMultisiteOccurrenceGeneration();
        void spatialIterationOccurrence(double ** M, double **K, double **occurrences, double** matrixOccurrence, double** normalizedMatrixRandom, double **transitionNormal, int lengthSeries);
        void precipitationMultiDistributionParameterization();
        void precipitationMultisiteAmountsGeneration();
        void initializeBaseWeatherVariables();
        void initializeOccurrenceIndex();
        void initializePrecipitationOutputs(int lengthSeason[]);
        void initializePrecipitationInternalArrays();
        void spatialIterationAmounts(double** correlationMatrixSimulatedData,double ** amountsCorrelationMatrix , double** randomMatrix, int length, double** occurrences, double** phatAlpha, double** phatBeta,double** simulatedPrecipitationAmounts);
        double bestFit(double *par, double*x, double *yObs, int nrX);
        int bestParametersNonLinearFit(double *par, double*x, double *yObs, int nrX);
        double inverseGammaFunction(double valueProbability, double alpha, double beta, double accuracy);


        void temperatureCompute();
        void computeTemperatureParameters();
        void initializeTemperatureParameters();
        int  doyFromDate(int day,int month,int year);
        int  dateFromDoy(int doy, int year, int *day, int *month);
        void harmonicsFourier(double* variable, double *par, int nrPar, double* estimatedVariable, int nrEstimatedVariable);
        void computeResiduals(double* averageTMaxDry,double* averageTMaxWet,double* stdDevTMaxDry,double* stdDevTMaxWet,double* averageTMinDry,double* averageTMinWet,double* stdDevTMinDry,double* stdDevTMinWet,int lengthArray,int idStation);
        void temperaturesCorrelationMatrices();
        void covarianceOfResiduals(double** covarianceMatrix, int lag);
        void initializeTemperaturecorrelationMatrices();
        void multisiteRandomNumbersTemperature();
        void initializeNormalRandomMatricesTemperatures();
        void multisiteTemperatureGeneration();
        void initializeMultiOccurrenceTemperature(int length);
        void initializeTemperaturesOutput(int length);
        void createAmountOutputSerie();

        void getWeatherGeneratorOutput();
        void initializeOutputData(int* nrDays);
        void randomSet(double *arrayNormal,int dimArray);

    public:
        // variables

        //functions
        weatherGenerator2D() {}
        bool initializeData(int lengthDataSeries, int nrStations);
        void initializeParameters(float thresholdPrecipitation, int simulatedYears, int distributionType, bool computePrecWG2D, bool computeTempWG2D);
        void setObservedData(TObsDataD** observations);
        void computeWeatherGenerator2D();
        void pressEnterToContinue();
        void initializeRandomNumbers(double* vector);
    };

#endif // WEATHERGENERATOR2D_H

#ifndef WEATHERGENERATOR_H
#define WEATHERGENERATOR_H

    #ifndef CRIT3DDATE_H
        #include "crit3dDate.h"
    #endif

    #ifndef PARSERXML_H
        #include "parserXML.h"
    #endif

    struct TinputObsData
    {
        Crit3DDate inputFirstDate;
        Crit3DDate inputLastDate;
        float *inputTMin;
        float *inputTMax;
        float *inputPrecip;
        int dataLenght;
    };

    struct TmonthlyWheather
    {
        float monthlyTmin [12];           //[°C] monthly maximum temp.
        float monthlyTmax [12];           //[°C] monthly minimum temp.
        float sumPrec [12];               //[mm] total monthly precipitation
        float fractionWetDays [12];       //[-] fraction of wet days (must be >0)
        float dw_Tmax [12];               //[°C]difference between maximum temperatures on dry and wet days
        float probabilityWetWet[12];      //[-] probability of a wet day after a wet day
        float stDevTmin [12];             //[-] monthly minimum temperature standard deviation
        float stDevTmax [12];             //[-] monthly maximum temperature standard deviation
    };

    struct TdailyWheather
    {
        float pww [366];                  //[-] daily probability wet/wet
        float pwd [366];                  //[-] daily probability wet/dry
        float meanPrecip [366];           //[mm] average mm/wet day
        float meanDryTMax [366];          //[°C] daily maximum temperature on dry days
        float meanWetTMax [366];          //[°C] daily maximum temperature on wet days
        float meanTMin [366];             //[°C] daily minimum temperature
        float maxTempStd [366];           //[°C] daily maximum temperature standard deviation
        float minTempStd [366];           //[°C] daily minimum temperature standard deviation
    };

    struct TstateWheather
    {
        int currentDay;                     // [-] day we have just passed as argument to "newday" function
        float currentTmax;                  // [°C] maximum temperature of the day passed as argument to "newday" function
        float currentTmin;                  // [°C] minimum temperature of the day passed as argument to "newday" function
        float currentPrec;                  // [mm] precipitation of the day passed as argument to "newday" function
        float resTMaxPrev;                  // [-] residual of maximum temperature for previous day
        float resTMinPrev;                  // [-] residual of minimum temperature for previous day
        bool wetPreviousDay;                // [-] true if the previous day has been a wet day, false otherwise
    };

    struct TwheatherGenClimate
    {
        TmonthlyWheather monthly;
        TdailyWheather daily;
        TstateWheather state;
    };

    struct ToutputDailyMeteo
    {
        Crit3DDate date;
        float minTemp;
        float maxTemp;
        float prec;
        int dataLenght;
    };

    void initializeDailyDataBasic(ToutputDailyMeteo* mydailyData, Crit3DDate myDate);

    float getTMax(int dayOfYear, float precThreshold, TwheatherGenClimate* wGen);
    float getTMin(int dayOfYear, float precThreshold, TwheatherGenClimate* wGen);
    float getTAverage(int dayOfYear, float precThreshold, TwheatherGenClimate* wGen);
    float getPrecip(int dayOfYear, float precThreshold, TwheatherGenClimate* wGen);

    void newDay(int dayOfYear, float precThreshold, TwheatherGenClimate* wGen);

    void initializeWeather(TwheatherGenClimate* wGen);

    void normalRandom(float *rnd_1, float *rnd_2);

    bool markov(float pwd, float pww, bool isWetPreviousDay);
    float weibull (float mean, float precThreshold);

    void qSplineYearInterpolate(float *meanY, float *dayVal);

    void genTemps(float *tMax, float *tMin, float meanTMax, float meanTMin, float stdMax,
                  float stdMin, float *resTMaxPrev, float *resTMinPrev);

    bool isWGDate(Crit3DDate myDate, int wgDoy1, int wgDoy2);

    bool assignXMLAnomaly(TXMLSeasonalAnomaly* XMLAnomaly, int modelIndex, int anomalyMonth1,
                          int anomalyMonth2, TwheatherGenClimate* wGenNoAnomaly, TwheatherGenClimate* wGen);

    bool assignAnomalyNoPrec(float myAnomaly, int anomalyMonth1, int anomalyMonth2,
                             float* myWGMonthlyVarNoAnomaly, float* myWGMonthlyVar );

    bool assignAnomalyPrec(float myAnomaly, int anomalyMonth1, int anomalyMonth2,
                           float* myWGMonthlyVarNoAnomaly, float* myWGMonthlyVar);

    bool makeSeasonalForecast(QString outputFileName, char separator, TXMLSeasonalAnomaly* XMLAnomaly,
                            TwheatherGenClimate wGenClimate, TinputObsData* lastYearDailyObsData,
                            int numRepetitions, int myPredictionYear, int wgDoy1, int wgDoy2, float minPrec);

    bool computeSeasonalPredictions(TinputObsData *lastYearDailyObsData, int dataLenght,
                                    TwheatherGenClimate* wgClimate, int predictionYear, int firstYear,
                                    int nrRepetitions, int wgDoy1, int wgDoy2, float minPrec,
                                    ToutputDailyMeteo* outputDailyData, bool isLastMember);

    void clearInputData(TinputObsData* myData);

#endif // WEATHERGENERATOR_H


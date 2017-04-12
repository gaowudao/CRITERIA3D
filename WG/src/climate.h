#ifndef CLIMATE
#define CLIMATE

#include <vector>

#ifndef CRIT3DDATE_H
    #include "crit3dDate.h"
#endif

struct TinputObsData;
struct TwheatherGenClimate;


bool computeWGClimate(int nrDays, Crit3DDate firstDate, float *inputTMin, float *inputTMax, float *inputPrec, float precThreshold, float minPercData, TwheatherGenClimate* wGen);
bool climateGenerator(int nrDays, TinputObsData climateDailyObsData, Crit3DDate climateDateIni, Crit3DDate climateDateFin, float precThreshold, float minPercData, TwheatherGenClimate* wGen);
float sampleStdDeviation(float values[], int nElement);

/* versione per std dev

struct TseasonalStdDeviation
{
    float minTemp;
    float maxTemp;
    float precip;
    float wetDaysFrequency;
    float wetWetDaysFrequency;
    float deltaTmaxDryWet;
};

struct TperiodStatistics
{
    std::vector<float> periodMeanMinTemp;
    std::vector<float> periodMeanMaxTemp;
    std::vector<float> periodPrecip;
    std::vector<float> periodWetDaysFrequency;
    std::vector<float> periodWetWetDaysFrequency;
    std::vector<float> periodDeltaTmaxDryWet;
};
*/

//bool climateGenerator(int nrData, TinputObsData myDailyObsData, Crit3DDate climateDateIni, Crit3DDate climateDateFin , int wgDoy1, int wgDoy2, int predictionYear, float precThreshold, float minPercData, TwheatherGenClimate* wGen);
//void computePeriodStatistic(float* seasonObsInputTMin, float* seasonObsInputTMax, float* seasonObsInputPrecip, int seasonDataLength, TperiodStatistics* statistic);

#endif // CLIMATE


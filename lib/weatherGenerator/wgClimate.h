#ifndef WGCLIMATE_H
#define WGCLIMATE_H

    class Crit3DDate;
    struct TinputObsData;
    struct TweatherGenClimate;

    bool computeWGClimate(int nrDays, Crit3DDate firstDate, float *inputTMin, float *inputTMax,
                          float *inputPrec, float precThreshold, float minPrecData,
                          TweatherGenClimate* wGen, bool writeOutput);

    bool climateGenerator(int nrDays, TinputObsData climateDailyObsData, Crit3DDate climateDateIni,
                          Crit3DDate climateDateFin, float precThreshold, float minPrecData, TweatherGenClimate* wGen);

    float sampleStdDeviation(float values[], int nElement);


#endif // WGCLIMATE_H


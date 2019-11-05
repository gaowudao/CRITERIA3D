#ifndef DATAHANDLER_H
#define DATAHANDLER_H

    #ifndef METEO_H
        #include "meteo.h"
    #endif
    #ifndef PLANT_H
        #include "plant.h"
    #endif

    #define OUTPUT_WEATHER 1
    #define OUTPUT_PLANT 2
    #define OUTPUT_SOIL 3

    float getTimeStepFromHourlyInterval(int myHourlyIntervals);

    meteoVariable getMeteoVariable(int myVar);
    int getMeteoVarIndex(meteoVariable myVar);

    QString getVarNameFromPlantVariable(plantVariable myVar);

#endif // DATAHANDLER_H

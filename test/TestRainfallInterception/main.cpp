#include <QCoreApplication>
#include <stdio.h>

#include "rainfallInterception.h"


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    double storedWater = 0.0;
    double rainfall = 0.5;
    double waterFreeEvaporation = 0.3;
    double lai = 4;
    double lightExtinctionCoefficient = 0.6;
    double leafStorage = 0.3;
    double stemStorage = 0.105;
    double maxStemFlowRate = 0.15;
    double freeRainfall = 0;
    double soilWater = 0;
    double stemFlow = 0;
    double throughfall = 0;
    double drainage = 0;
    canopy::waterManagementCanopy(&storedWater,rainfall,waterFreeEvaporation,lai,0.5,lightExtinctionCoefficient,leafStorage, stemStorage,maxStemFlowRate,&freeRainfall,&drainage,&stemFlow,&throughfall,&soilWater);
    //canopy::waterManagementCanopy(&storedWater,rainfall,waterFreeEvaporation,lai,0.5,lightExtinctionCoefficient,leafStorage, stemStorage,maxStemFlowRate,&soilWater);
    printf("stored %f  soilWater %f\n",storedWater,soilWater);
    return 0;
}

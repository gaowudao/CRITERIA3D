#include <QCoreApplication>
#include <stdio.h>
#include <stdlib.h>

#include "rainfallInterception.h"
#include "readWeatherMonticolo.h"


int main(int argc, char *argv[])
{
    // read weather data
    int numberOfLines;
    FILE* fp;
    fp = fopen("weather_Monticolo_all_years_2019_04_21.csv","r");
    numberOfLines = readWeatherLineFileNumber(fp);
    fclose(fp);
    double* temp = (double*) calloc(numberOfLines,sizeof(double));
    double* prec = (double*) calloc(numberOfLines,sizeof(double));
    int* day = (int*) calloc(numberOfLines,sizeof(int));
    int* month = (int*) calloc(numberOfLines,sizeof(int));
    int* year = (int*) calloc(numberOfLines,sizeof(int));
    int* dayOfYear = (int*) calloc(numberOfLines,sizeof(int));
    int* hour = (int*) calloc(numberOfLines,sizeof(int));
    int* minute = (int*) calloc(numberOfLines,sizeof(int));
    FILE* fp1;
    fp1 = fopen("weather_Monticolo_all_years_2019_04_21.csv","r");
    readWeatherMonticoloHalfhourlyData(fp1,minute,hour,dayOfYear,day,month,year,temp, prec, numberOfLines);
    fclose(fp1);


    double storedWater = 0.0;

    for (int iLine=0;iLine<numberOfLines;iLine++)
    {
        //QCoreApplication a(argc, argv);

        double waterFreeEvaporation = 0.01;
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
        canopy::waterManagementCanopy(&storedWater,prec[iLine],waterFreeEvaporation,lai,0.5,lightExtinctionCoefficient,leafStorage, stemStorage,maxStemFlowRate,&freeRainfall,&drainage,&stemFlow,&throughfall,&soilWater);
        //canopy::waterManagementCanopy(&storedWater,prec[iLine],waterFreeEvaporation,lai,0.5,lightExtinctionCoefficient,leafStorage, stemStorage,maxStemFlowRate,&soilWater);
        printf("%.2d:%.2d, %.2d/%.2d/%.4d, %.2f , %.2f\n",hour[iLine],minute[iLine],day[iLine],month[iLine],year[iLine],temp[iLine],prec[iLine]);
        printf("soilWater %f storedWater%f",soilWater, storedWater);
        //printf("%f\n",canopy::canopyNoInterceptedRainfallHydrall(lai,0.,rainfall));
        //getchar();
    }
    return 0;
}

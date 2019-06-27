
#include <QCoreApplication>
//#include <stdio.h>
#include <malloc.h>
#include <math.h>
#include <time.h>
//#include <iostream>

#include "commonConstants.h"
#include "furtherMathFunctions.h"
#include "inputData.h"
#include "wg2D.h"


weatherGenerator2D WG2D;


void obsDataMeteoPointFormat(int nrStations, int nrData, float*** weatherArray, int** dateArray,TObsDataD** observedDataDaily)
{
    for(int i=0;i<nrStations;i++)
    {
        for (int j=0;j<nrData;j++)
        {
            observedDataDaily[i][j].date.day = dateArray[j][0];
            observedDataDaily[i][j].date.month = dateArray[j][1];
            observedDataDaily[i][j].date.year = dateArray[j][2];
            observedDataDaily[i][j].tMin = weatherArray[i][j][0];
            observedDataDaily[i][j].tMax = weatherArray[i][j][1];
            observedDataDaily[i][j].prec = weatherArray[i][j][2];
        }

    }
}



int main()
{
    int nrStations = 3;
    int lengthDataSeries = 1099;
    int nrVariables = 3;
    int nrDate = 3;
    float *** weatherArray = nullptr;
    int ** dateArray = nullptr;
    float *** weatherArrayFourTimes = nullptr;
    int ** dateArrayFourTimes = nullptr;

    weatherArray = (float ***)calloc(nrStations, sizeof(float**));
    for (int i=0;i<nrStations;i++)
    {
        weatherArray[i] = (float **)calloc(lengthDataSeries, sizeof(float*));
        for (int j=0;j<lengthDataSeries;j++)
        {
            weatherArray[i][j] = (float *)calloc(nrVariables, sizeof(float));
        }
    }
    dateArray = (int **)calloc(lengthDataSeries, sizeof(int*));
    for (int j=0;j<lengthDataSeries;j++)
    {
        dateArray[j] = (int *)calloc(nrDate, sizeof(int));
    }
    fillObservationArray(lengthDataSeries,weatherArray,dateArray);

    weatherArrayFourTimes = (float ***)calloc(nrStations, sizeof(float**));
    for (int i=0;i<nrStations;i++)
    {
        weatherArrayFourTimes[i] = (float **)calloc(1095*4, sizeof(float*));
        for (int j=0;j<1095*4;j++)
        {
            weatherArrayFourTimes[i][j] = (float *)calloc(nrVariables, sizeof(float));
        }
    }
    dateArrayFourTimes = (int **)calloc(1095*4, sizeof(int*));
    for (int j=0;j<1095*4;j++)
    {
        dateArrayFourTimes[j] = (int *)calloc(nrDate, sizeof(int));
    }

    for (int i=0;i<nrStations;i++)
    {
        for (int j=0;j<1095;j++)
        {
            for (int k=0;k<nrVariables;k++)
            {
                weatherArrayFourTimes[i][j][k] = weatherArray[i][j][k];
                weatherArrayFourTimes[i][j+1095][k] = weatherArray[i][j][k];
                weatherArrayFourTimes[i][j+1095*2][k] = weatherArray[i][j][k];
                weatherArrayFourTimes[i][j+1095*3][k] = weatherArray[i][j][k];
            }

        }
    }
    for (int j=0;j<1095;j++)
    {

        for (int k=0;k<nrDate;k++)
        {
            dateArrayFourTimes[j][k] = dateArray[j][k];
            if (k== 2)
            {
                dateArrayFourTimes[j+1095][k] = dateArray[j][k]+3;
                dateArrayFourTimes[j+1095*2][k] = dateArray[j][k]+6;
                dateArrayFourTimes[j+1095*3][k] = dateArray[j][k]+9;
            }
            else
            {
                dateArrayFourTimes[j+1095][k] = dateArray[j][k];
                dateArrayFourTimes[j+1095*2][k] = dateArray[j][k];
                dateArrayFourTimes[j+1095*3][k] = dateArray[j][k];
            }
        }
    }

    TObsDataD** observedDataDaily = (TObsDataD **)calloc(nrStations, sizeof(TObsDataD*));
    for (int i=0;i<nrStations;i++)
    {
        //observedDataDaily[i] = (TObsDataD *)calloc(lengthDataSeries, sizeof(TObsDataD));
        observedDataDaily[i] = (TObsDataD *)calloc(1095*4, sizeof(TObsDataD));
    }
    //obsDataMeteoPointFormat(nrStations,lengthDataSeries,weatherArray,dateArray,observedDataDaily);
    obsDataMeteoPointFormat(nrStations,1095*4,weatherArrayFourTimes,dateArrayFourTimes,observedDataDaily);
    //WG2D.initializeData(lengthDataSeries,nrStations);
    WG2D.initializeData(1095*4,nrStations);
    WG2D.setObservedData(observedDataDaily);


    bool computePrecipitation = true;
    bool computeTemperature = true;
    printf("weather generator\n");
    if (computePrecipitation) printf("compute precipitation\n");
    if (computeTemperature) printf("compute temperature\n");

    int distributionType = 2; // 1 multiexponential 2 multigamma
    int yearsOfSimulations = 3;
    WG2D.initializeParameters(NODATA, yearsOfSimulations, distributionType,
                              computePrecipitation, computeTemperature);
    WG2D.computeWeatherGenerator2D();

    for (int i=0;i<nrStations;i++)
    {
        free(observedDataDaily[i]);
        for (int j=0;j<lengthDataSeries;j++)
        {
            free(weatherArray[i][j]);
        }
        free(weatherArray[i]);
    }
    for (int j=0;j<lengthDataSeries;j++)
    {
        free(dateArray[j]);
    }
    free(observedDataDaily);
    free(weatherArray);
    free(dateArray);

    for (int i=0;i<nrStations;i++)
    {
        for (int j=0;j<1095*4;j++)
        {
            free(weatherArrayFourTimes[i][j]);
        }
        free(weatherArrayFourTimes[i]);
    }
    for (int j=0;j<1095*4;j++)
    {
        free(dateArrayFourTimes[j]);
    }

    free(weatherArrayFourTimes);
    free(dateArrayFourTimes);
    return 0;
}



#include <stdio.h>
//#include <stdlib.h>
//#include <math.h>
#include <malloc.h>
//#include <time.h>
#include <iostream>

#include "wg2D.h"
#include "commonConstants.h"
//#include "furtherMathFunctions.h"
//#include "statistics.h"
//#include "eispack.h"
//#include "gammaFunction.h"

#include "weatherGenerator.h"
#include "wgClimate.h"


void weatherGenerator2D::initializeOutputData(int* nrDays)
{
    int length = 365*parametersModel.yearOfSimulation;
    for (int i=1; i<= parametersModel.yearOfSimulation;i++)
    {
        if (isLeapYear(i)) length++;
    }
    *nrDays = length;
    outputWeatherData = (ToutputWeatherData*)calloc(nrStations, sizeof(ToutputWeatherData));
    for (int iStation=0;iStation<nrStations;iStation++)
    {
        outputWeatherData[iStation].yearSimulated = (int*)calloc(length, sizeof(int));
        outputWeatherData[iStation].monthSimulated = (int*)calloc(length, sizeof(int));
        outputWeatherData[iStation].daySimulated = (int*)calloc(length, sizeof(int));
        outputWeatherData[iStation].doySimulated = (int*)calloc(length, sizeof(int));
        //if (isTempWG2D)
        //{
            outputWeatherData[iStation].maxT = (double*)calloc(length, sizeof(double));
            outputWeatherData[iStation].minT = (double*)calloc(length, sizeof(double));
        //}
        //if (isPrecWG2D)
            outputWeatherData[iStation].precipitation = (double*)calloc(length, sizeof(double));
    }
    // = (double*)calloc(lengthOfRandomSeries, sizeof(double));
}

void weatherGenerator2D::getWeatherGeneratorOutput()
{
    int counter;
    int counterSeason[4];
    int day,month;
    int nrDays = NODATA;
    weatherGenerator2D::initializeOutputData(&nrDays);
    Crit3DDate inputFirstDate;
    TweatherGenClimate weatherGenClimate;
    QString outputFileName;

    float *inputTMin = nullptr;
    float *inputTMax = nullptr;
    float *inputPrec = nullptr;
    float precThreshold = parametersModel.precipitationThreshold;
    float minPrecData = NODATA;
    bool writeOutput = true;
    inputTMin = (float*)calloc(nrDays, sizeof(float));
    inputTMax = (float*)calloc(nrDays, sizeof(float));
    inputPrec = (float*)calloc(nrDays, sizeof(float));
    inputFirstDate.day = 1;
    inputFirstDate.month = 1;
    inputFirstDate.year = 1;

    for (int iStation=0;iStation<nrStations;iStation++)
    {
        outputFileName = "wgSimulation_station_" + QString::number(iStation) + ".txt";
        counter = 0;
        counterSeason[3] = counterSeason[2] = counterSeason[1] = counterSeason[0] = 0;
        for (int iYear=1;iYear<=parametersModel.yearOfSimulation;iYear++)
        {
            for (int iDoy=0; iDoy<365; iDoy++)
            {
                outputWeatherData[iStation].yearSimulated[counter] = iYear;
                outputWeatherData[iStation].doySimulated[counter] = iDoy+1;
                weatherGenerator2D::dateFromDoy(outputWeatherData[iStation].doySimulated[counter],1,&day,&month);
                outputWeatherData[iStation].daySimulated[counter] = day;
                outputWeatherData[iStation].monthSimulated[counter] = month;
                if (isTempWG2D)
                {
                    outputWeatherData[iStation].maxT[counter] = maxTGenerated[counter][iStation];
                    outputWeatherData[iStation].minT[counter] = minTGenerated[counter][iStation];
                }
                else
                {
                    outputWeatherData[iStation].maxT[counter] = NODATA;
                    outputWeatherData[iStation].minT[counter] = NODATA;

                }

                if (isPrecWG2D)
                {
                    //simulatedPrecipitationAmounts[iSeason].matrixAmounts[i][j]
                    int iSeason;
                    if (month == 1 || month == 2 || month == 12) iSeason = 0;
                    else if (month == 3 || month == 4 || month == 5) iSeason = 1;
                    else if (month == 6 || month == 7 || month == 8) iSeason = 2;
                    else iSeason = 3;
                    outputWeatherData[iStation].precipitation[counter] = simulatedPrecipitationAmounts[iSeason].matrixAmounts[iStation][counterSeason[iSeason]];
                    //printf("%.2f\n",outputWeatherData[iStation].precipitation[counter]);
                    (counterSeason[iSeason])++;
                }
                else outputWeatherData[iStation].precipitation[counter] = NODATA;
                counter++;
            }
        }

        for(int i=0;i<parametersModel.yearOfSimulation*365;i++)
        {
            //printf("%d %d %.1f %.1f %.1f\n",outputWeatherData[iStation].daySimulated[i],outputWeatherData[iStation].monthSimulated[i],outputWeatherData[iStation].minT[i],outputWeatherData[iStation].maxT[i],outputWeatherData[iStation].precipitation[i]);
            //printf("%d %d %.1f %.1f %.1f\n",outputWeatherData[iStation].daySimulated[i],outputWeatherData[iStation].monthSimulated[i],outputWeatherData[iStation].minT[i],outputWeatherData[iStation].maxT[i],outputWeatherData[iStation].precipitation[i]);
            //printf("%d %d %.1f\n",outputWeatherData[iStation].daySimulated[i],outputWeatherData[iStation].monthSimulated[i],outputWeatherData[iStation].precipitation[i]);

        }
        counter = 0;
        for (int i=0;i<nrDays;i++)
        {
            inputTMin[i]= (float)(outputWeatherData[iStation].minT[counter]);
            inputTMax[i]= (float)(outputWeatherData[iStation].maxT[counter]);
            inputPrec[i]= (float)(outputWeatherData[iStation].precipitation[counter]);
            if (isLeapYear(outputWeatherData[iStation].yearSimulated[counter]) && outputWeatherData[iStation].monthSimulated[counter] == 2 && outputWeatherData[iStation].daySimulated[counter] == 28)
            {
                inputTMin[++i]= (float)(outputWeatherData[iStation].minT[counter]);
                inputTMax[++i]= (float)(outputWeatherData[iStation].maxT[counter]);
                inputPrec[++i]= (float)(outputWeatherData[iStation].precipitation[counter]);
            }
            counter++;
        }
        computeWGClimate(nrDays,inputFirstDate,inputTMin,inputTMax,inputPrec,precThreshold,minPrecData,&weatherGenClimate,writeOutput,outputFileName);
        //pressEnterToContinue();
    }
    free(inputTMin);
    free(inputTMax);
    free(inputPrec);

    precThreshold = float(parametersModel.precipitationThreshold);
    minPrecData = NODATA;
    nrDays = nrData;
    inputTMin = (float*)calloc(nrDays, sizeof(float));
    inputTMax = (float*)calloc(nrDays, sizeof(float));
    inputPrec = (float*)calloc(nrDays, sizeof(float));
    for (int iStation=0;iStation<nrStations;iStation++)
    {
        outputFileName = "wgClimate_station_" + QString::number(iStation) + ".txt";
        inputFirstDate.day = obsDataD[iStation][0].date.day;
        inputFirstDate.month = obsDataD[iStation][0].date.month;
        inputFirstDate.year = obsDataD[iStation][0].date.year;
        nrDays = nrData;
        for (int i=0;i<nrDays;i++)
        {
            inputTMin[i] = obsDataD[iStation][i].tMin;
            inputTMax[i] = obsDataD[iStation][i].tMax;
            inputPrec[i] = obsDataD[iStation][i].prec;
        }
        computeWGClimate(nrDays,inputFirstDate,inputTMin,inputTMax,inputPrec,precThreshold,minPrecData,&weatherGenClimate,writeOutput,outputFileName);

    }

}


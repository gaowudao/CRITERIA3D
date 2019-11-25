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
    /*
    for (int i=1;i<=parametersModel.yearOfSimulation;i++)
    {
        if (isLeapYear(i)) nrDays++;
    }
    */

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
    float ** monthlySimulatedAveragePrecipitation = nullptr;
    float ** monthlyClimateAveragePrecipitation = nullptr;
    float** meanAmountsPrecGenerated = nullptr;
    float** cumulatedOccurrencePrecGenerated = nullptr;
    monthlySimulatedAveragePrecipitation = (float**)calloc(nrStations, sizeof(float*));
    monthlyClimateAveragePrecipitation = (float**)calloc(nrStations, sizeof(float*));
    meanAmountsPrecGenerated = (float**)calloc(nrStations, sizeof(float*));
    cumulatedOccurrencePrecGenerated = (float**)calloc(nrStations, sizeof(float*));
    for (int iStation=0;iStation<nrStations;iStation++)
    {
        meanAmountsPrecGenerated[iStation] = (float*)calloc(12, sizeof(float));
        cumulatedOccurrencePrecGenerated[iStation] = (float*)calloc(12, sizeof(float));
        monthlySimulatedAveragePrecipitation[iStation] = (float*)calloc(12, sizeof(float));
        monthlyClimateAveragePrecipitation[iStation] = (float*)calloc(12, sizeof(float));
        for (int j=0;j<12;j++)
        {
            meanAmountsPrecGenerated[iStation][j] = 0;
            cumulatedOccurrencePrecGenerated[iStation][j] = 0;
            monthlySimulatedAveragePrecipitation[iStation][j] = NODATA;
            monthlyClimateAveragePrecipitation[iStation][j] = NODATA;
        }
    }
    for (int iStation=0;iStation<nrStations;iStation++)
    {
        for (int iDay=0;iDay<nrData;iDay++)
        {
            if (obsDataD[iStation][iDay].prec > parametersModel.precipitationThreshold)
            {
                meanAmountsPrecGenerated[iStation][obsPrecDataD[iStation][iDay].date.month-1] += obsDataD[iStation][iDay].prec;
                ++cumulatedOccurrencePrecGenerated[iStation][obsPrecDataD[iStation][iDay].date.month-1];
            }
        }
        for (int jMonth=0;jMonth<12;jMonth++)
        {
            meanAmountsPrecGenerated[iStation][jMonth] /= cumulatedOccurrencePrecGenerated[iStation][jMonth];
            printf("%d  %f %f\n",jMonth,meanAmountsPrecGenerated[iStation][jMonth],cumulatedOccurrencePrecGenerated[iStation][jMonth]);
        }
        //pressEnterToContinue();
    }

    for (int iStation=0;iStation<nrStations;iStation++)
    {
        outputFileName = "wgSimulation_station_" + QString::number(iStation) + ".txt";
        counter = 0;
        counterSeason[3] = counterSeason[2] = counterSeason[1] = counterSeason[0] = 0;
        for (int iYear=1;iYear<=parametersModel.yearOfSimulation;iYear++)
        {
            //counterSeason[3] = counterSeason[2] = counterSeason[1] = counterSeason[0] = 0;
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
                    //printf("%.2f %.2f %.0f\n",outputWeatherData[0].maxT[counter],outputWeatherData[0].minT[counter],occurrencePrecGenerated[counter][0]);
                }
                else
                {
                    outputWeatherData[iStation].maxT[counter] = NODATA;
                    outputWeatherData[iStation].minT[counter] = NODATA;

                }

                if (isPrecWG2D)
                {

                    if (parametersModel.distributionPrecipitation == 3)
                    {
                        outputWeatherData[iStation].precipitation[counter] = precGenerated[iStation][counter];
                    }
                    else {outputWeatherData[iStation].precipitation[counter] = amountsPrecGenerated[counter][iStation];}
                    //meanAmountsPrecGenerated[iStation][month-1] += amountsPrecGenerated[counter][iStation];
                    //cumulatedOccurrencePrecGenerated[iStation][month-1] += occurrencePrecGenerated[counter][iStation];
                }
                else
                {
                    outputWeatherData[iStation].precipitation[counter] = occurrencePrecGenerated[counter][iStation] + 0.1;
                }
                counter++;
            }
        }
        //for (int jMonth=0;jMonth<12;jMonth++)
            //meanAmountsPrecGenerated[iStation][jMonth] /= cumulatedOccurrencePrecGenerated[iStation][jMonth];


        for(int i=0;i<parametersModel.yearOfSimulation*365;i++)
        {
            //printf("%d %d %.1f %.1f %.1f\n",outputWeatherData[iStation].daySimulated[i],outputWeatherData[iStation].monthSimulated[i],outputWeatherData[iStation].minT[i],outputWeatherData[iStation].maxT[i],outputWeatherData[iStation].precipitation[i]);
            //printf("%d %d %.1f %.1f %.1f\n",outputWeatherData[iStation].daySimulated[i],outputWeatherData[iStation].monthSimulated[i],outputWeatherData[iStation].minT[i],outputWeatherData[iStation].maxT[i],outputWeatherData[iStation].precipitation[i]);
            //printf("%d %d %.1f\n",outputWeatherData[iStation].daySimulated[i],outputWeatherData[iStation].monthSimulated[i],outputWeatherData[iStation].precipitation[i]);

        }
        //pressEnterToContinue();
        counter = 0;
        for (int i=0;i<nrDays;i++)
        {
            inputTMin[i]= (float)(outputWeatherData[iStation].minT[counter]);
            inputTMax[i]= (float)(outputWeatherData[iStation].maxT[counter]);
            //if (isPrecWG2D)
            inputPrec[i]= (float)(outputWeatherData[iStation].precipitation[counter]);
            //else inputPrec[i]= 0;
            if (isLeapYear(outputWeatherData[iStation].yearSimulated[counter]) && outputWeatherData[iStation].monthSimulated[counter] == 2 && outputWeatherData[iStation].daySimulated[counter] == 28)
            {
                ++i;
                inputTMin[i]= (float)(outputWeatherData[iStation].minT[counter]);
                inputTMax[i]= (float)(outputWeatherData[iStation].maxT[counter]);
                //if (isPrecWG2D)
                inputPrec[i]= (float)(outputWeatherData[iStation].precipitation[counter]);
                //else inputPrec[i]= 0;
            }
            counter++;
        }
        computeWG2DClimate(nrDays,inputFirstDate,inputTMin,inputTMax,inputPrec,precThreshold,minPrecData,&weatherGenClimate,writeOutput,outputFileName,monthlySimulatedAveragePrecipitation[iStation]);
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
    // compute climate statistics from observed data
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
        computeWG2DClimate(nrDays,inputFirstDate,inputTMin,inputTMax,inputPrec,precThreshold,minPrecData,&weatherGenClimate,writeOutput,outputFileName,monthlyClimateAveragePrecipitation[iStation]);
    }
    free(inputTMin);
    free(inputTMax);
    free(inputPrec);

    for (int iStation=0;iStation<nrStations;iStation++)
    {
        printf("stazione %d\n",iStation);
        for (int iMonth=0;iMonth<12;iMonth++)
        {
            printf("%f  %f \n",monthlySimulatedAveragePrecipitation[iStation][iMonth],monthlyClimateAveragePrecipitation[iStation][iMonth]);
        }
    }

    for (int iStation=0;iStation<nrStations;iStation++)
    {
        for (int iDate=0;iDate<parametersModel.yearOfSimulation*365;iDate++)
        {
            int doy,day,month;
            day = month = 0;
            doy = (iDate+1)%365;
            weatherGenerator2D::dateFromDoy(doy,2001,&day,&month);
            if (outputWeatherData[iStation].precipitation[iDate] > parametersModel.precipitationThreshold + EPSILON)
            {
                printf("confronto %d %f  ",month,outputWeatherData[iStation].precipitation[iDate]);
                outputWeatherData[iStation].precipitation[iDate] = MAXVALUE(parametersModel.precipitationThreshold + EPSILON,outputWeatherData[iStation].precipitation[iDate]* monthlyClimateAveragePrecipitation[iStation][month-1] / monthlySimulatedAveragePrecipitation[iStation][month-1]);
                printf("%f\n",outputWeatherData[iStation].precipitation[iDate]);
            }

        }
    }

    nrDays = 365*parametersModel.yearOfSimulation;
    for (int i=1;i<=parametersModel.yearOfSimulation;i++)
    {
        if (isLeapYear(i)) nrDays++;
    }
    inputTMin = (float*)calloc(nrDays, sizeof(float));
    inputTMax = (float*)calloc(nrDays, sizeof(float));
    inputPrec = (float*)calloc(nrDays, sizeof(float));
    inputFirstDate.day = 1;
    inputFirstDate.month = 1;
    inputFirstDate.year = 1;


    for (int iStation=0;iStation<nrStations;iStation++)
    {
        outputFileName = "wgSimulation_2_station_" + QString::number(iStation) + ".txt";
        counter = 0;
        for (int i=0;i<nrDays;i++)
        {
            inputTMin[i]= (float)(outputWeatherData[iStation].minT[counter]);
            inputTMax[i]= (float)(outputWeatherData[iStation].maxT[counter]);
            //if (isPrecWG2D)
            inputPrec[i]= (float)(outputWeatherData[iStation].precipitation[counter]);
            //else inputPrec[i]= 0;
            if (isLeapYear(outputWeatherData[iStation].yearSimulated[counter]) && outputWeatherData[iStation].monthSimulated[counter] == 2 && outputWeatherData[iStation].daySimulated[counter] == 28)
            {
                ++i;
                inputTMin[i]= (float)(outputWeatherData[iStation].minT[counter]);
                inputTMax[i]= (float)(outputWeatherData[iStation].maxT[counter]);
                //if (isPrecWG2D)
                inputPrec[i]= (float)(outputWeatherData[iStation].precipitation[counter]);
                //else inputPrec[i]= 0;
            }
            counter++;
        }
        computeWG2DClimate(nrDays,inputFirstDate,inputTMin,inputTMax,inputPrec,precThreshold,minPrecData,&weatherGenClimate,writeOutput,outputFileName,monthlySimulatedAveragePrecipitation[iStation]);
    }

    free(inputTMin);
    free(inputTMax);
    free(inputPrec);



}



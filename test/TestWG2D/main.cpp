
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "commonConstants.h"
#include "furtherMathFunctions.h"
#include "wg2D.h"
#include "readPragaFormatData.h"


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
    FILE *fp;
    fp = fopen("inputData/budrio_1961_2018.txt", "r");
    if (fp == nullptr)
    {
        printf("Error! File not found\n");
        return -1;
    }

    int numberMeteoLines;
    numberMeteoLines = readPragaLineFileNumber(fp);
    fclose(fp);
    int doy,day,month,year;
    double prec,minT,maxT,meanT;
    doy = day = month = year = NODATA;
    prec = minT = maxT = meanT = NODATA;
    bool firstDay = true;
    int nrStations = 10;
    int lengthDataSeries = numberMeteoLines;
    int nrVariables = 3;
    int nrDate = 3;
    float *** weatherArray = nullptr;
    int ** dateArray = nullptr;

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

    for (int i=0;i<nrStations;i++)
    {
        if (i==0)
        {
               fp = fopen("inputData/argelato_1961_2018.txt","r");
               if (fp == nullptr)
               {
                   printf("Error! File not found\n");
                   return -1;
               }
               firstDay = true;
               readPragaERACLITODailyData(fp,&firstDay,&doy,&day,&month,&year,&minT,&maxT,&meanT,&prec);
               for (int j=0;j<numberMeteoLines;j++)
               {
                   readPragaERACLITODailyData(fp,&firstDay,&doy,&day,&month,&year,&minT,&maxT,&meanT,&prec);
                   weatherArray[i][j][0] = minT;
                   weatherArray[i][j][1] = maxT;
                   weatherArray[i][j][2] = prec;
                   dateArray[j][0] = day;
                   dateArray[j][1] = month;
                   dateArray[j][2] = year;
               }
               fclose(fp);
        }
        else if (i==1)
        {
            fp = fopen("inputData/baricella_1961_2018.txt","r");
            if (fp == nullptr)
            {
                printf("Error! File not found\n");
                return -1;
            }
            firstDay = true;
            readPragaERACLITODailyData(fp,&firstDay,&doy,&day,&month,&year,&minT,&maxT,&meanT,&prec);
            for (int j=0;j<numberMeteoLines;j++)
            {
                readPragaERACLITODailyData(fp,&firstDay,&doy,&day,&month,&year,&minT,&maxT,&meanT,&prec);
                weatherArray[i][j][0] = minT;
                weatherArray[i][j][1] = maxT;
                weatherArray[i][j][2] = prec;
            }
            fclose(fp);
        }
        else if (i==2)
        {
            fp = fopen("inputData/bologna_1961_2018.txt","r");
            if (fp == nullptr)
            {
                printf("Error! File not found\n");
                return -1;
            }
            firstDay = true;
            readPragaERACLITODailyData(fp,&firstDay,&doy,&day,&month,&year,&minT,&maxT,&meanT,&prec);
            for (int j=0;j<numberMeteoLines;j++)
            {
                readPragaERACLITODailyData(fp,&firstDay,&doy,&day,&month,&year,&minT,&maxT,&meanT,&prec);
                weatherArray[i][j][0] = minT;
                weatherArray[i][j][1] = maxT;
                weatherArray[i][j][2] = prec;
            }
            fclose(fp);
        }
        else if (i==3)
        {
            fp = fopen("inputData/budrio_1961_2018.txt","r");
            if (fp == nullptr)
            {
                printf("Error! File not found\n");
                return -1;
            }
            firstDay = true;
            readPragaERACLITODailyData(fp,&firstDay,&doy,&day,&month,&year,&minT,&maxT,&meanT,&prec);
            for (int j=0;j<numberMeteoLines;j++)
            {
                readPragaERACLITODailyData(fp,&firstDay,&doy,&day,&month,&year,&minT,&maxT,&meanT,&prec);
                weatherArray[i][j][0] = minT;
                weatherArray[i][j][1] = maxT;
                weatherArray[i][j][2] = prec;
            }
            fclose(fp);
        }
        else if (i==4)
        {
            fp = fopen("inputData/casalfiumanese_1961_2018.txt","r");
            if (fp == nullptr)
            {
                printf("Error! File not found\n");
                return -1;
            }
            firstDay = true;
            readPragaERACLITODailyData(fp,&firstDay,&doy,&day,&month,&year,&minT,&maxT,&meanT,&prec);
            for (int j=0;j<numberMeteoLines;j++)
            {
                readPragaERACLITODailyData(fp,&firstDay,&doy,&day,&month,&year,&minT,&maxT,&meanT,&prec);
                weatherArray[i][j][0] = minT;
                weatherArray[i][j][1] = maxT;
                weatherArray[i][j][2] = prec;
            }
            fclose(fp);
        }
        else if (i==5)
        {
            fp = fopen("inputData/castenaso_1961_2018.txt","r");
            if (fp == nullptr)
            {
                printf("Error! File not found\n");
                return -1;
            }
            firstDay = true;
            readPragaERACLITODailyData(fp,&firstDay,&doy,&day,&month,&year,&minT,&maxT,&meanT,&prec);
            for (int j=0;j<numberMeteoLines;j++)
            {
                readPragaERACLITODailyData(fp,&firstDay,&doy,&day,&month,&year,&minT,&maxT,&meanT,&prec);
                weatherArray[i][j][0] = minT;
                weatherArray[i][j][1] = maxT;
                weatherArray[i][j][2] = prec;
            }
            fclose(fp);
        }
        else if (i==6)
        {
            fp = fopen("inputData/mezzolara_1961_2018.txt","r");
            if (fp == nullptr)
            {
                printf("Error! File not found\n");
                return -1;
            }
            firstDay = true;
            readPragaERACLITODailyData(fp,&firstDay,&doy,&day,&month,&year,&minT,&maxT,&meanT,&prec);
            for (int j=0;j<numberMeteoLines;j++)
            {
                readPragaERACLITODailyData(fp,&firstDay,&doy,&day,&month,&year,&minT,&maxT,&meanT,&prec);
                weatherArray[i][j][0] = minT;
                weatherArray[i][j][1] = maxT;
                weatherArray[i][j][2] = prec;
            }
            fclose(fp);
        }
        else if (i==7)
        {
            fp = fopen("inputData/montecalderaro_1961_2018.txt","r");
            if (fp == nullptr)
            {
                printf("Error! File not found\n");
                return -1;
            }
            firstDay = true;
            readPragaERACLITODailyData(fp,&firstDay,&doy,&day,&month,&year,&minT,&maxT,&meanT,&prec);
            for (int j=0;j<numberMeteoLines;j++)
            {
                readPragaERACLITODailyData(fp,&firstDay,&doy,&day,&month,&year,&minT,&maxT,&meanT,&prec);
                weatherArray[i][j][0] = minT;
                weatherArray[i][j][1] = maxT;
                weatherArray[i][j][2] = prec;
            }
            fclose(fp);
        }
        else if (i==8)
        {
            fp = fopen("inputData/pievedicento_1961_2018.txt","r");
            if (fp == nullptr)
            {
                printf("Error! File not found\n");
                return -1;
            }
            firstDay = true;
            readPragaERACLITODailyData(fp,&firstDay,&doy,&day,&month,&year,&minT,&maxT,&meanT,&prec);
            for (int j=0;j<numberMeteoLines;j++)
            {
                readPragaERACLITODailyData(fp,&firstDay,&doy,&day,&month,&year,&minT,&maxT,&meanT,&prec);
                weatherArray[i][j][0] = minT;
                weatherArray[i][j][1] = maxT;
                weatherArray[i][j][2] = prec;
            }
            fclose(fp);
        }
        else
        {
            fp = fopen("inputData/sanlazzaro_1961_2018.txt","r");
            firstDay = true;
            if (fp == nullptr)
            {
                printf("Error! File not found\n");
                return -1;
            }
            readPragaERACLITODailyData(fp,&firstDay,&doy,&day,&month,&year,&minT,&maxT,&meanT,&prec);
            for (int j=0;j<numberMeteoLines;j++)
            {
                readPragaERACLITODailyData(fp,&firstDay,&doy,&day,&month,&year,&minT,&maxT,&meanT,&prec);
                weatherArray[i][j][0] = minT;
                weatherArray[i][j][1] = maxT;
                weatherArray[i][j][2] = prec;
            }
            fclose(fp);
        }
    }


    TObsDataD** observedDataDaily = (TObsDataD **)calloc(nrStations, sizeof(TObsDataD*));
    for (int i=0;i<nrStations;i++)
    {
        observedDataDaily[i] = (TObsDataD *)calloc(lengthDataSeries, sizeof(TObsDataD));
    }
    obsDataMeteoPointFormat(nrStations,lengthDataSeries,weatherArray,dateArray,observedDataDaily);
    WG2D.initializeData(lengthDataSeries,nrStations);
    WG2D.setObservedData(observedDataDaily);


    bool computePrecipitation = false;
    bool computeTemperature = true;
    printf("weather generator\n");
    if (computePrecipitation) printf("compute precipitation\n");
    if (computeTemperature) printf("compute temperature\n");

    int distributionType = 1; // 1 multiexponential 2 multigamma
    int yearsOfSimulations = 500;
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
    return 0;
}




#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdio.h>

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
    // read uniform random numbers and generation of the array to be used for tests
    FILE* fp;
    fp = fopen("randomNumbers2.txt","r");
    char vectorDummy[20];
    double randomSeries[10000];

    int counter = 0;
    char dummy;

    for (int j=0;j<10000;j++)
    {
        counter = 0;
        for (int i=0;i<20;i++)
        {
            vectorDummy[i] = '\0';
        }
        do
        {
            dummy = getc(fp);
            if (dummy != '\n');
                vectorDummy[counter]= dummy;
            counter++;
        } while (dummy != '\n');
        randomSeries[j] = atof(vectorDummy);
    }
    fclose(fp);
    WG2D.initializeRandomNumbers(randomSeries);

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
    double precipitationThreshold = 0.25;
    doy = day = month = year = NODATA;
    prec = minT = maxT = meanT = NODATA;
    bool firstDay = true;
    int nrStations = 5; // !! da 1 a 10 stazioni
    int distributionType = 1; // 1 multiexponential 2 multigamma 3 Weibull
    int yearsOfSimulations = 1000; // numero anni
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
    srand(time(nullptr));
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
        else if (i==9)
        {
            fp = fopen("inputData/sanlazzaro_1961_2018.txt","r");
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
        /*else if (i==10)
        {
            fp = fopen("inputData/bedonia_1961_2018.txt","r");
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
        else if (i==11)
        {
            fp = fopen("inputData/berceto_1961_2018.txt","r");
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
        else if (i==12)
        {
            fp = fopen("inputData/borgovalditaro_1961_2018.txt","r");
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
        else if (i==13)
        {
            fp = fopen("inputData/caffaraccia_1961_2018.txt","r");
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
        else if (i==14)
        {
            fp = fopen("inputData/campi_1961_2018.txt","r");
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
        else if (i==15)
        {
            fp = fopen("inputData/drusco_1961_2018.txt","r");
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
        else if (i==16)
        {
            fp = fopen("inputData/farini_1961_2018.txt","r");
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
        else if (i==17)
        {
            fp = fopen("inputData/marra_1961_2018.txt","r");
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
        else if (i==18)
        {
            fp = fopen("inputData/primaansapo_1961_2018.txt","r");
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
        else if (i==19)
        {
            fp = fopen("inputData/ravezza_1961_2018.txt","r");
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
        else if (i==20)
        {
            fp = fopen("inputData/tavernago_1961_2018.txt","r");
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
        else if (i==21)
        {
            fp = fopen("inputData/farodelbacucco_1961_2018.txt","r");
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
        else if (i==22)
        {
            fp = fopen("inputData/gorino_1961_2018.txt","r");
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
        else if (i==23)
        {
            fp = fopen("inputData/calatis_1961_2018.txt","r");
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
        else if (i==24)
        {
            fp = fopen("inputData/lidodivolano_1961_2018.txt","r");
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
        else if (i==25)
        {
            fp = fopen("inputData/portogaribaldi_1961_2018.txt","r");
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
        else if (i==26)
        {
            fp = fopen("inputData/misanoadriatico_1961_2018.txt","r");
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
        else if (i==27)
        {
            fp = fopen("inputData/cattolica_1961_2018.txt","r");
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
        else if (i==28)
        {
            fp = fopen("inputData/riccione_1961_2018.txt","r");
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
        else if (i==29)
        {
            fp = fopen("inputData/roncofreddo_1961_2018.txt","r");
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
        else if (i==30)
        {
            fp = fopen("inputData/puntamarina_1961_2018.txt","r");
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
        else if (i==31)
        {
            fp = fopen("inputData/pennabilli_1961_2018.txt","r");
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
        else if (i==32)
        {
            fp = fopen("inputData/reggioemilia_1961_2018.txt","r");
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
        else if (i==33)
        {
            fp = fopen("inputData/fiumalbo_1961_2018.txt","r");
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
        }*/
        else
        {
            for (int j=0;j<numberMeteoLines;j++)
            {
                weatherArray[i][j][0] = weatherArray[i%10][j][0] + (1.0*rand())/RAND_MAX - 0.5;
                weatherArray[i][j][1] = weatherArray[i%10][j][1] + (1.0*rand())/RAND_MAX - 0.5;
                weatherArray[i][j][2] = MAXVALUE(0,weatherArray[i%10][j][2] + (1.0*rand())/RAND_MAX - 0.5);
            }
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


    bool computePrecipitation = true;
    bool computeTemperature = true;
    printf("weather generator\n");
    if (computePrecipitation) printf("compute precipitation\n");
    if (computeTemperature) printf("compute temperature\n");


    WG2D.initializeParameters(precipitationThreshold, yearsOfSimulations, distributionType,
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



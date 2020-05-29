
#include <QString>
#include <QFile>
#include <QTextStream>

#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdio.h>

#include "commonConstants.h"
#include "furtherMathFunctions.h"
#include "wg2D.h"
#include "readPragaFormatData.h"
#include "readErg5FilesC4C7.h"

#define NR_SIMULATION_YEARS 100
// [ 1 - 10 ]
#define NR_STATIONS 5

#define DEFAULT_TEST 0
#define RECLAMATION_CONSORTIA_TEST 1

weatherGenerator2D WG2D;

void printSimulationResults(ToutputWeatherData* output,int nrStations,int lengthArray);

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
            if (dummy != '\n')
                vectorDummy[counter]= dummy;
            counter++;
        } while (dummy != '\n');
        randomSeries[j] = atof(vectorDummy);
    }
    fclose(fp);
    WG2D.initializeRandomNumbers(randomSeries);
    int numberMeteoLines;
    double precipitationThreshold = 0.25;
    int nrStations = NR_STATIONS;
    int distributionType = 1; // 1 multiexponential 2 multigamma 3 Weibull
    int yearsOfSimulations = NR_SIMULATION_YEARS;
    int lengthDataSeries;
    float *** weatherArray = nullptr;
    int ** dateArray = nullptr;

    bool kindOfTest;
    kindOfTest = RECLAMATION_CONSORTIA_TEST;
    if (kindOfTest == DEFAULT_TEST)
    {
        fp = fopen("inputData/argelato_1961_2018.txt", "r");
        if (fp == nullptr)
        {
            printf("Error! File not found\n");
            return -1;
        }


        numberMeteoLines = readPragaLineFileNumber(fp);
        fclose(fp);
        int doy,day,month,year;
        double prec,minT,maxT,meanT;
        doy = day = month = year = NODATA;
        prec = minT = maxT = meanT = NODATA;
        bool firstDay = true;
        lengthDataSeries = numberMeteoLines;
        int nrVariables = 3;
        int nrDate = 3;
        //float *** weatherArray = nullptr;
        //int ** dateArray = nullptr;

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
    }
    else
    {
        // open the list of cells
        fp = fopen("inputDataC4/list_C4.txt","r");
        int numberOfCells;
        numberOfCells = readERG5CellListNumber(fp);
        fclose(fp);
        // find the cell with the earliest date and with latest date
        int earliestDate, latestDate;
        earliestDate = latestDate = NODATA;
        QString nameOfFile;
        fp = fopen("inputDataC4/list_C4.txt","r");
        char* numCell = (char *)calloc(6, sizeof(char));
        FILE* fp1;
        for (int i=0; i<numberOfCells; i++)
        {
            readTheCellNumber(fp,numCell);
            //printf("%c%c%c%c%c\n",numCell[0],numCell[1],numCell[2],numCell[3],numCell[4]);
            QString stringNumber(numCell);
            nameOfFile = "inputDataC4/" + stringNumber + ".txt";
            std::string stringNameOfFile;
            stringNameOfFile = nameOfFile.toStdString();
            const char * charNameOfFile = stringNameOfFile.c_str();
            fp1 = fopen(charNameOfFile,"r");
            int firstDate,lastDate;
            firstDate = lastDate = NODATA;
            readEarliestLatestDateC4C7(fp1,&firstDate,&lastDate);
            if (i == 0)
            {
                earliestDate = firstDate;
                latestDate = lastDate;
            }
            else
            {
                if (firstDate < earliestDate)
                    earliestDate = firstDate;
                if (lastDate < latestDate)
                    latestDate = lastDate;
            }

            fclose(fp1);
        }
        fclose(fp);
        nrStations = numberOfCells;
        numberMeteoLines = latestDate - earliestDate + 1;

        int doy,day,month,year;
        double prec,minT,maxT,meanT;
        doy = day = month = year = NODATA;
        prec = minT = maxT = meanT = NODATA;
        bool firstDay = true;
        lengthDataSeries = numberMeteoLines;
        int nrVariables = 3;
        int nrDate = 3;

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

        fp = fopen("inputDataC4/list_C4.txt","r");
        for (int i=0; i<numberOfCells; i++)
        {

            readTheCellNumber(fp,numCell);
            //printf("%c%c%c%c%c\n",numCell[0],numCell[1],numCell[2],numCell[3],numCell[4]);
            QString stringNumber(numCell);
            nameOfFile = "inputDataC4/" + stringNumber + ".txt";
            std::string stringNameOfFile;
            stringNameOfFile = nameOfFile.toStdString();
            const char * charNameOfFile = stringNameOfFile.c_str();
            fp1 = fopen(charNameOfFile,"r");
            bool firstRead = true;
            bool checkFirstRead;
            for (int j=0;j<numberMeteoLines;j++)
            {
                int currentDate;
                checkFirstRead = firstRead;
                readPragaERG5DailyDataC4C7(fp1,&firstRead,&currentDate,&doy,&day,&month,&year,&minT,&maxT,&prec);

                if (checkFirstRead != firstRead)
                {
                    while (currentDate > earliestDate+j)
                    {
                        weatherArray[i][j][0] = NODATA;
                        weatherArray[i][j][1] = NODATA;
                        weatherArray[i][j][2] = NODATA;
                        if (i == 0)
                        {
                            int yearInitial =1899;
                            int monthInitial = 12;
                            int dayInitial = 31;
                            int dayFinal,monthFinal,yearFinal;
                            dayFinal = monthFinal = yearFinal = NODATA;
                            getTheNewDateShiftingDays(earliestDate+j,dayInitial,monthInitial,yearInitial,&dayFinal,&monthFinal,&yearFinal);
                            dateArray[j][0] = dayFinal;
                            dateArray[j][1] = monthFinal;
                            dateArray[j][2] = yearFinal;
                        }
                        j++;
                    }

                }
                weatherArray[i][j][0] = minT;
                weatherArray[i][j][1] = maxT;
                weatherArray[i][j][2] = prec;
                if (i == 0)
                {
                    dateArray[j][0] = day;
                    dateArray[j][1] = month;
                    dateArray[j][2] = year;
                }
                printf ("%d %d %d ",dateArray[j][0],dateArray[j][1],dateArray[j][2]);
                printf ("%f %f %f \n",weatherArray[i][j][0],weatherArray[i][j][1],weatherArray[i][j][2]);
            }
            getchar();
        }


    }
    //int yearInitial =1899;
    //int monthInitial = 12;
    //int dayInitial = 31;
    //int dayFinal,monthFinal,yearFinal;
    //dayFinal = monthFinal = yearFinal = NODATA;
    /*
     * getTheNewDateShiftingDays(365,dayInitial,monthInitial,yearInitial,&dayFinal,&monthFinal,&yearFinal);
    printf("%d  %d %d\n",dayFinal,monthFinal,yearFinal);
    getTheNewDateShiftingDays(731,dayInitial,monthInitial,yearInitial,&dayFinal,&monthFinal,&yearFinal);
    printf("%d  %d %d\n",dayFinal,monthFinal,yearFinal);
    getTheNewDateShiftingDays(1096,dayInitial,monthInitial,yearInitial,&dayFinal,&monthFinal,&yearFinal);
    printf("%d  %d %d\n",dayFinal,monthFinal,yearFinal);
    getTheNewDateShiftingDays(1461,dayInitial,monthInitial,yearInitial,&dayFinal,&monthFinal,&yearFinal);
    printf("%d  %d %d\n",dayFinal,monthFinal,yearFinal);
    */

    /*
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
    int startingYear = 2001;
    int lengthArraySimulation;
    lengthArraySimulation = 365 * yearsOfSimulations;
    ToutputWeatherData* results;
    results = (ToutputWeatherData *)calloc(nrStations, sizeof(ToutputWeatherData));
    for (int iStation=0;iStation<nrStations;iStation++)
    {
        results[iStation].daySimulated = (int *)calloc(lengthArraySimulation, sizeof(int));
        results[iStation].monthSimulated = (int *)calloc(lengthArraySimulation, sizeof(int));
        results[iStation].yearSimulated = (int *)calloc(lengthArraySimulation, sizeof(int));
        results[iStation].doySimulated = (int *)calloc(lengthArraySimulation, sizeof(int));
        results[iStation].minT = (double *)calloc(lengthArraySimulation, sizeof(double));
        results[iStation].maxT = (double *)calloc(lengthArraySimulation, sizeof(double));
        results[iStation].precipitation = (double *)calloc(lengthArraySimulation, sizeof(double));
    }
    WG2D.initializeParameters(precipitationThreshold, yearsOfSimulations, distributionType,
                              computePrecipitation, computeTemperature,true);
    WG2D.computeWeatherGenerator2D();
    results = WG2D.getWeatherGeneratorOutput(startingYear);
    printSimulationResults(results,nrStations,lengthArraySimulation);

    //free memory
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
    */
    return 0;
}

void printSimulationResults(ToutputWeatherData* output,int nrStations,int lengthArray)
{
    FILE* fp;
    QString outputName;
    for (int iStation=0; iStation<nrStations;iStation++)
    {
        outputName = "wgStation_" + QString::number(iStation) + ".csv";
        QFile file(outputName);
        file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
        QTextStream stream( &file );
        for (int m=0; m<lengthArray; m++)
        {
            stream <<  output[iStation].daySimulated[m] << "/" << output[iStation].monthSimulated[m] << "/" << output[iStation].yearSimulated[m] << "," << output[iStation].doySimulated[m] << "," << output[iStation].minT[m]<< "," << output[iStation].maxT[m]<< "," << output[iStation].precipitation[m]<<endl;
        }
        file.close();
    }

}

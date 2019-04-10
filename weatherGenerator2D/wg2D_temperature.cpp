#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <malloc.h>
#include <time.h>
#include <iostream>

#include "wg2D.h"
#include "commonConstants.h"
#include "furtherMathFunctions.h"
#include "statistics.h"
#include "eispack.h"
#include "gammaFunction.h"
#include "crit3dDate.h"

int weatherGenerator2D::doyFromDate(int day,int month,int year)
{
    int daysOfMonth[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
    if (isLeapYear(year)) (daysOfMonth[1])++;
    if (month == 1)
    {
        return day;
    }
    int doy = 0;
    int counter =0;
    while (counter < (month-1))
    {
        doy += daysOfMonth[counter];
        counter++;
    }
    doy += day;
    return doy;
}

void weatherGenerator2D::temperatureCompute()
{
    // step 1 of temperature WG2D
    weatherGenerator2D::computeTemperatureParameters();
    printf("end of run\n");
    // step 2 of temperature WG2D
    // step 3 of temperature WG2D
    // step 4 of temperature WG2D
    // step 5 of temperature WG2D
}

void weatherGenerator2D::initializeTemperatureParameters()
{
    // initialize temp parameters
    temperatureCoefficients = (TtemperatureCoefficients *)calloc(nrStations, sizeof(TtemperatureCoefficients));
    for (int i = 0; i < nrStations; i++)
    {
        temperatureCoefficients[i].maxTDry.averageEstimation = (double *)calloc(365, sizeof(double));
        for (int j=0; j<365; j++) temperatureCoefficients[i].maxTDry.averageEstimation[j] = NODATA;
        temperatureCoefficients[i].maxTDry.stdDevEstimation = (double *)calloc(365, sizeof(double));
        for (int j=0; j<365; j++) temperatureCoefficients[i].maxTDry.stdDevEstimation[j] = NODATA;
        temperatureCoefficients[i].maxTDry.averageFourierParameters.a0 = NODATA;
        temperatureCoefficients[i].maxTDry.averageFourierParameters.aCos1 = NODATA;
        temperatureCoefficients[i].maxTDry.averageFourierParameters.aSin1 = NODATA;
        temperatureCoefficients[i].maxTDry.averageFourierParameters.aCos2 = NODATA;
        temperatureCoefficients[i].maxTDry.averageFourierParameters.aSin2 = NODATA;
        temperatureCoefficients[i].maxTDry.standardDeviationFourierParameters.a0 = NODATA;
        temperatureCoefficients[i].maxTDry.standardDeviationFourierParameters.aCos1 = NODATA;
        temperatureCoefficients[i].maxTDry.standardDeviationFourierParameters.aSin1 = NODATA;
        temperatureCoefficients[i].maxTDry.standardDeviationFourierParameters.aCos2 = NODATA;
        temperatureCoefficients[i].maxTDry.standardDeviationFourierParameters.aSin2 = NODATA;

        temperatureCoefficients[i].minTDry.averageEstimation = (double *)calloc(365, sizeof(double));
        for (int j=0; j<365; j++) temperatureCoefficients[i].minTDry.averageEstimation[j] = NODATA;
        temperatureCoefficients[i].minTDry.stdDevEstimation = (double *)calloc(365, sizeof(double));
        for (int j=0; j<365; j++) temperatureCoefficients[i].minTDry.stdDevEstimation[j] = NODATA;
        temperatureCoefficients[i].minTDry.averageFourierParameters.a0 = NODATA;
        temperatureCoefficients[i].minTDry.averageFourierParameters.aCos1 = NODATA;
        temperatureCoefficients[i].minTDry.averageFourierParameters.aSin1 = NODATA;
        temperatureCoefficients[i].minTDry.averageFourierParameters.aCos2 = NODATA;
        temperatureCoefficients[i].minTDry.averageFourierParameters.aSin2 = NODATA;
        temperatureCoefficients[i].minTDry.standardDeviationFourierParameters.a0 = NODATA;
        temperatureCoefficients[i].minTDry.standardDeviationFourierParameters.aCos1 = NODATA;
        temperatureCoefficients[i].minTDry.standardDeviationFourierParameters.aSin1 = NODATA;
        temperatureCoefficients[i].minTDry.standardDeviationFourierParameters.aCos2 = NODATA;
        temperatureCoefficients[i].minTDry.standardDeviationFourierParameters.aSin2 = NODATA;

        temperatureCoefficients[i].maxTWet.averageEstimation = (double *)calloc(365, sizeof(double));
        for (int j=0; j<365; j++) temperatureCoefficients[i].maxTWet.averageEstimation[j] = NODATA;
        temperatureCoefficients[i].maxTWet.stdDevEstimation = (double *)calloc(365, sizeof(double));
        for (int j=0; j<365; j++) temperatureCoefficients[i].maxTWet.stdDevEstimation[j] = NODATA;
        temperatureCoefficients[i].maxTWet.averageFourierParameters.a0 = NODATA;
        temperatureCoefficients[i].maxTWet.averageFourierParameters.aCos1 = NODATA;
        temperatureCoefficients[i].maxTWet.averageFourierParameters.aSin1 = NODATA;
        temperatureCoefficients[i].maxTWet.averageFourierParameters.aCos2 = NODATA;
        temperatureCoefficients[i].maxTWet.averageFourierParameters.aSin2 = NODATA;
        temperatureCoefficients[i].maxTWet.standardDeviationFourierParameters.a0 = NODATA;
        temperatureCoefficients[i].maxTWet.standardDeviationFourierParameters.aCos1 = NODATA;
        temperatureCoefficients[i].maxTWet.standardDeviationFourierParameters.aSin1 = NODATA;
        temperatureCoefficients[i].maxTWet.standardDeviationFourierParameters.aCos2 = NODATA;
        temperatureCoefficients[i].maxTWet.standardDeviationFourierParameters.aSin2 = NODATA;

        temperatureCoefficients[i].minTWet.averageEstimation = (double *)calloc(365, sizeof(double));
        for (int j=0; j<365; j++) temperatureCoefficients[i].minTWet.averageEstimation[j] = NODATA;
        temperatureCoefficients[i].minTWet.stdDevEstimation = (double *)calloc(365, sizeof(double));
        for (int j=0; j<365; j++) temperatureCoefficients[i].minTWet.stdDevEstimation[j] = NODATA;
        temperatureCoefficients[i].minTWet.averageFourierParameters.a0 = NODATA;
        temperatureCoefficients[i].minTWet.averageFourierParameters.aCos1 = NODATA;
        temperatureCoefficients[i].minTWet.averageFourierParameters.aSin1 = NODATA;
        temperatureCoefficients[i].minTWet.averageFourierParameters.aCos2 = NODATA;
        temperatureCoefficients[i].minTWet.averageFourierParameters.aSin2 = NODATA;
        temperatureCoefficients[i].minTWet.standardDeviationFourierParameters.a0 = NODATA;
        temperatureCoefficients[i].minTWet.standardDeviationFourierParameters.aCos1 = NODATA;
        temperatureCoefficients[i].minTWet.standardDeviationFourierParameters.aSin1 = NODATA;
        temperatureCoefficients[i].minTWet.standardDeviationFourierParameters.aCos2 = NODATA;
        temperatureCoefficients[i].minTWet.standardDeviationFourierParameters.aSin2 = NODATA;

        for (int i=0; i<2; i++)
        {
            for (int j=0; j<2; j++)
            {
                temperatureCoefficients[i].A[i][j]= NODATA;
                temperatureCoefficients[i].B[i][j]= NODATA;
            }
        }
     }
}


void weatherGenerator2D::computeTemperatureParameters()
{
    weatherGenerator2D::initializeTemperatureParameters();

    for (int iStation=0; iStation<nrStations; iStation++)
    {
        double averageTMaxDry[365];
        double averageTMaxWet[365];
        double stdDevTMaxDry[365];
        double stdDevTMaxWet[365];
        double averageTMinDry[365];
        double averageTMinWet[365];
        double stdDevTMinDry[365];
        double stdDevTMinWet[365];
        int countTMaxDry[365];
        int countTMaxWet[365];
        int countTMinDry[365];
        int countTMinWet[365];

        int finalDay = 365;
        for (int iDay=0;iDay<finalDay;iDay++)
        {

            averageTMaxDry[iDay]=0;
            averageTMaxWet[iDay]=0;
            stdDevTMaxDry[iDay]=0;
            stdDevTMaxWet[iDay]=0;
            averageTMinDry[iDay]=0;
            averageTMinWet[iDay]=0;
            stdDevTMinDry[iDay]=0;
            stdDevTMinWet[iDay]=0;
            countTMaxDry[iDay] = 0;
            countTMaxWet[iDay] = 0;
            countTMinDry[iDay] = 0;
            countTMinWet[iDay] = 0;

        }
        for (int iDatum=0; iDatum<nrData; iDatum++)
        {
            obsDataD[iStation][iDatum].tMax += EPSILON;
            obsDataD[iStation][iDatum].tMin += EPSILON;
        }
        for (int iDatum=0; iDatum<nrData; iDatum++)
        {
            int dayOfYear;
            dayOfYear = weatherGenerator2D::doyFromDate(obsDataD[iStation][iDatum].date.day,obsDataD[iStation][iDatum].date.month,obsDataD[iStation][iDatum].date.year);
            if ((isLeapYear(obsDataD[iStation][iDatum].date.year)) && (obsDataD[iStation][iDatum].date.month > 2)) dayOfYear--;
            dayOfYear--;
            if (obsDataD[iStation][iDatum].date.month == 2 && obsDataD[iStation][iDatum].date.day == 29)
            {
                dayOfYear-- ;
            }
            //if (fabs(obsDataD[iStation][iDatum].prec-NODATA)> EPSILON && getDoyFromDate(&(obsDataD[iStation][iDatum].date))== iDay)
            //if (dayOfYear == 71) printf("%f  %f \n ",obsDataD[iStation][iDatum].prec, obsDataD[iStation][iDatum].tMax);
            if (fabs(obsDataD[iStation][iDatum].prec-NODATA) > EPSILON)
                {
                    if (obsDataD[iStation][iDatum].prec > parametersModel.precipitationThreshold)
                    {
                        if ((fabs((obsDataD[iStation][iDatum].tMax)-NODATA))> EPSILON)
                        {
                            ++countTMaxWet[dayOfYear];
                            averageTMaxWet[dayOfYear] += obsDataD[iStation][iDatum].tMax;
                        }
                        if ((fabs(obsDataD[iStation][iDatum].tMin)-NODATA)> EPSILON)
                        {
                            ++countTMinWet[dayOfYear];
                            averageTMinWet[dayOfYear] += obsDataD[iStation][iDatum].tMin;
                        }
                    }
                    else if (obsDataD[iStation][iDatum].prec <= parametersModel.precipitationThreshold)
                    {
                        if ((fabs((obsDataD[iStation][iDatum].tMax)-NODATA))> EPSILON)
                        {
                            ++countTMaxDry[dayOfYear];
                            averageTMaxDry[dayOfYear] += obsDataD[iStation][iDatum].tMax;
                        }
                        if ((fabs((obsDataD[iStation][iDatum].tMin)-NODATA))> EPSILON)
                        {
                            ++countTMinDry[dayOfYear];
                            averageTMinDry[dayOfYear] += obsDataD[iStation][iDatum].tMin;
                        }
                    }
                }

        }
        for (int iDay=0; iDay<365; iDay++)
        {
            if (countTMaxDry[iDay] != 0) averageTMaxDry[iDay] /= countTMaxDry[iDay];
            else averageTMaxDry[iDay] = NODATA;
            if (countTMaxWet[iDay] != 0) averageTMaxWet[iDay] /= countTMaxWet[iDay];
            else averageTMaxWet[iDay] = NODATA;
            if (countTMinDry[iDay] != 0) averageTMinDry[iDay] /= countTMinDry[iDay];
            else averageTMinDry[iDay] = NODATA;
            if (countTMinWet[iDay] != 0) averageTMinWet[iDay] /= countTMinWet[iDay];
            else averageTMinWet[iDay] = NODATA;
            //printf("%d %f %d %f %d\n",iDay,averageTMaxWet[iDay],countTMaxWet[iDay],averageTMinWet[iDay],countTMinWet[iDay]);
            //printf("%d %f %d %f %d\n",iDay,averageTMaxDry[iDay],countTMaxDry[iDay],averageTMinDry[iDay],countTMinDry[iDay]);
        }

        for (int iDatum=0; iDatum<nrData; iDatum++)
        {
            int dayOfYear;
            dayOfYear = weatherGenerator2D::doyFromDate(obsDataD[iStation][iDatum].date.day,obsDataD[iStation][iDatum].date.month,obsDataD[iStation][iDatum].date.year);
            if ((isLeapYear(obsDataD[iStation][iDatum].date.year)) && (obsDataD[iStation][iDatum].date.month > 2)) dayOfYear--;
            dayOfYear--;
            if (obsDataD[iStation][iDatum].date.month == 2 && obsDataD[iStation][iDatum].date.day == 29)
            {
                dayOfYear-- ;
            }
            //if (fabs(obsDataD[iStation][iDatum].prec-NODATA)> EPSILON && getDoyFromDate(&(obsDataD[iStation][iDatum].date))== iDay)
            //if (dayOfYear == 71) printf("%f  %f \n ",obsDataD[iStation][iDatum].prec, obsDataD[iStation][iDatum].tMax);
            if (fabs(obsDataD[iStation][iDatum].prec - NODATA) > EPSILON)
                {
                    if (obsDataD[iStation][iDatum].prec > parametersModel.precipitationThreshold)
                    {
                        if ((fabs((obsDataD[iStation][iDatum].tMax) - NODATA))> EPSILON)
                        {
                            stdDevTMaxWet[dayOfYear] += (obsDataD[iStation][iDatum].tMax - averageTMaxWet[dayOfYear])*(obsDataD[iStation][iDatum].tMax - averageTMaxWet[dayOfYear]);
                        }
                        if ((fabs(obsDataD[iStation][iDatum].tMin) - NODATA)> EPSILON)
                        {
                            stdDevTMinWet[dayOfYear] += (obsDataD[iStation][iDatum].tMin - averageTMinWet[dayOfYear])*(obsDataD[iStation][iDatum].tMin - averageTMinWet[dayOfYear]);
                        }
                    }
                    else if (obsDataD[iStation][iDatum].prec <= parametersModel.precipitationThreshold)
                    {
                        if ((fabs((obsDataD[iStation][iDatum].tMax) - NODATA))> EPSILON)
                        {
                            stdDevTMaxDry[dayOfYear] += (obsDataD[iStation][iDatum].tMax - averageTMaxDry[dayOfYear])*(obsDataD[iStation][iDatum].tMax - averageTMaxDry[dayOfYear]);
                        }
                        if ((fabs((obsDataD[iStation][iDatum].tMin) - NODATA))> EPSILON)
                        {
                            stdDevTMinDry[dayOfYear] += (obsDataD[iStation][iDatum].tMin - averageTMinDry[dayOfYear])*(obsDataD[iStation][iDatum].tMin - averageTMinDry[dayOfYear]);
                        }
                    }
                }

        }
        for (int iDay=0; iDay<365; iDay++)
        {
            if (countTMaxDry[iDay] != 0) stdDevTMaxDry[iDay] /= countTMaxDry[iDay];
            else stdDevTMaxDry[iDay] = NODATA;
            if (countTMaxWet[iDay] != 0) stdDevTMaxWet[iDay] /= countTMaxWet[iDay];
            else stdDevTMaxWet[iDay] = NODATA;
            if (countTMinDry[iDay] != 0) stdDevTMinDry[iDay] /= countTMinDry[iDay];
            else stdDevTMinDry[iDay] = NODATA;
            if (countTMinWet[iDay] != 0) stdDevTMinWet[iDay] /= countTMinWet[iDay];
            else stdDevTMinWet[iDay] = NODATA;

            if (countTMaxDry[iDay] != 0) stdDevTMaxDry[iDay] = sqrt(stdDevTMaxDry[iDay]);
            if (countTMaxWet[iDay] != 0) stdDevTMaxWet[iDay] = sqrt(stdDevTMaxWet[iDay]);
            if (countTMinDry[iDay] != 0) stdDevTMinDry[iDay] = sqrt(stdDevTMinDry[iDay]);
            if (countTMinWet[iDay] != 0) stdDevTMinWet[iDay] = sqrt(stdDevTMinWet[iDay]);

            //printf("%d %f %d %f %d\n",iDay,stdDevTMaxWet[iDay],countTMaxWet[iDay],stdDevTMinWet[iDay],countTMinWet[iDay]);
            //getchar();
            //printf("%d %f %d %f %d\n",iDay,stdDevTMaxDry[iDay],countTMaxDry[iDay],stdDevTMinDry[iDay],countTMinDry[iDay]);
        }
        for (int iDatum=0; iDatum<nrData; iDatum++)
        {
            obsDataD[iStation][iDatum].tMax -= EPSILON;
            obsDataD[iStation][iDatum].tMin -= EPSILON;
        }


        // compute the Fourier coefficients

        double *par;
        int nrPar = 5;
        par = (double *) calloc(nrPar, sizeof(double));
        for (int i=0;i<nrPar;i++)
        {
            par[i] = NODATA;
        }
        weatherGenerator2D::harmonicsFourier(averageTMaxDry,par,nrPar,temperatureCoefficients[iStation].maxTDry.averageEstimation,365);
        temperatureCoefficients[iStation].maxTDry.averageFourierParameters.a0 = par[0];
        temperatureCoefficients[iStation].maxTDry.averageFourierParameters.aCos1 = par[1];
        temperatureCoefficients[iStation].maxTDry.averageFourierParameters.aSin1 = par[2];
        temperatureCoefficients[iStation].maxTDry.averageFourierParameters.aCos2 = par[3];
        temperatureCoefficients[iStation].maxTDry.averageFourierParameters.aSin2 = par[4];
        //for (int i=0;i<365;i++) printf("%d %f \n",i,temperatureCoefficients[iStation].maxTDry.averageEstimation[i]);


        for (int i=0;i<nrPar;i++)
        {
            par[i] = NODATA;
        }
        weatherGenerator2D::harmonicsFourier(averageTMinDry,par,nrPar,temperatureCoefficients[iStation].minTDry.averageEstimation,365);
        temperatureCoefficients[iStation].minTDry.averageFourierParameters.a0 = par[0];
        temperatureCoefficients[iStation].minTDry.averageFourierParameters.aCos1 = par[1];
        temperatureCoefficients[iStation].minTDry.averageFourierParameters.aSin1 = par[2];
        temperatureCoefficients[iStation].minTDry.averageFourierParameters.aCos2 = par[3];
        temperatureCoefficients[iStation].minTDry.averageFourierParameters.aSin2 = par[4];

        for (int i=0;i<nrPar;i++)
        {
            par[i] = NODATA;
        }
        weatherGenerator2D::harmonicsFourier(averageTMaxWet,par,nrPar,temperatureCoefficients[iStation].maxTDry.averageEstimation,365);
        temperatureCoefficients[iStation].maxTWet.averageFourierParameters.a0 = par[0];
        temperatureCoefficients[iStation].maxTWet.averageFourierParameters.aCos1 = par[1];
        temperatureCoefficients[iStation].maxTWet.averageFourierParameters.aSin1 = par[2];
        temperatureCoefficients[iStation].maxTWet.averageFourierParameters.aCos2 = par[3];
        temperatureCoefficients[iStation].maxTWet.averageFourierParameters.aSin2 = par[4];
        for (int i=0;i<nrPar;i++)
        {
            par[i] = NODATA;
        }
        weatherGenerator2D::harmonicsFourier(averageTMinWet,par,nrPar,temperatureCoefficients[iStation].minTWet.averageEstimation,365);
        temperatureCoefficients[iStation].minTWet.averageFourierParameters.a0 = par[0];
        temperatureCoefficients[iStation].minTWet.averageFourierParameters.aCos1 = par[1];
        temperatureCoefficients[iStation].minTWet.averageFourierParameters.aSin1 = par[2];
        temperatureCoefficients[iStation].minTWet.averageFourierParameters.aCos2 = par[3];
        temperatureCoefficients[iStation].minTWet.averageFourierParameters.aSin2 = par[4];

        for (int i=0;i<nrPar;i++)
        {
            par[i] = NODATA;
        }
        weatherGenerator2D::harmonicsFourier(stdDevTMaxDry,par,nrPar,temperatureCoefficients[iStation].maxTDry.stdDevEstimation,365);
        temperatureCoefficients[iStation].maxTDry.standardDeviationFourierParameters.a0 = par[0];
        temperatureCoefficients[iStation].maxTDry.standardDeviationFourierParameters.aCos1 = par[1];
        temperatureCoefficients[iStation].maxTDry.standardDeviationFourierParameters.aSin1 = par[2];
        temperatureCoefficients[iStation].maxTDry.standardDeviationFourierParameters.aCos2 = par[3];
        temperatureCoefficients[iStation].maxTDry.standardDeviationFourierParameters.aSin2 = par[4];
        for (int i=0;i<nrPar;i++)
        {
            par[i] = NODATA;
        }
        weatherGenerator2D::harmonicsFourier(stdDevTMinDry,par,nrPar,temperatureCoefficients[iStation].minTDry.stdDevEstimation,365);
        temperatureCoefficients[iStation].minTDry.standardDeviationFourierParameters.a0 = par[0];
        temperatureCoefficients[iStation].minTDry.standardDeviationFourierParameters.aCos1 = par[1];
        temperatureCoefficients[iStation].minTDry.standardDeviationFourierParameters.aSin1 = par[2];
        temperatureCoefficients[iStation].minTDry.standardDeviationFourierParameters.aCos2 = par[3];
        temperatureCoefficients[iStation].minTDry.standardDeviationFourierParameters.aSin2 = par[4];

        for (int i=0;i<nrPar;i++)
        {
            par[i] = NODATA;
        }
        weatherGenerator2D::harmonicsFourier(stdDevTMaxWet,par,nrPar,temperatureCoefficients[iStation].maxTWet.stdDevEstimation,365);
        temperatureCoefficients[iStation].maxTWet.standardDeviationFourierParameters.a0 = par[0];
        temperatureCoefficients[iStation].maxTWet.standardDeviationFourierParameters.aCos1 = par[1];
        temperatureCoefficients[iStation].maxTWet.standardDeviationFourierParameters.aSin1 = par[2];
        temperatureCoefficients[iStation].maxTWet.standardDeviationFourierParameters.aCos2 = par[3];
        temperatureCoefficients[iStation].maxTWet.standardDeviationFourierParameters.aSin2 = par[4];
        for (int i=0;i<nrPar;i++)
        {
            par[i] = NODATA;
        }
        weatherGenerator2D::harmonicsFourier(stdDevTMinWet,par,nrPar,temperatureCoefficients[iStation].minTWet.stdDevEstimation,365);
        temperatureCoefficients[iStation].minTWet.standardDeviationFourierParameters.a0 = par[0];
        temperatureCoefficients[iStation].minTWet.standardDeviationFourierParameters.aCos1 = par[1];
        temperatureCoefficients[iStation].minTWet.standardDeviationFourierParameters.aSin1 = par[2];
        temperatureCoefficients[iStation].minTWet.standardDeviationFourierParameters.aCos2 = par[3];
        temperatureCoefficients[iStation].minTWet.standardDeviationFourierParameters.aSin2 = par[4];
        free(par);


    } // end of iStation "for" cycle

}

void weatherGenerator2D::harmonicsFourier(double* variable, double *par,int nrPar, double* estimatedVariable, int nrEstimatedVariable)
{
    int maxIterations = 100000;
    int functionCode;

    // find the upper bound
    double valueMax,valueMin;
    bool validDays[365];
    int nrValidDays;


    valueMax = 0;
    nrValidDays = 0;
    for (int i=0;i<365;i++)
    {
        if (fabs(variable[i] - NODATA)< EPSILON  || fabs(variable[i]) < EPSILON)
        {
            validDays[i] = false;
        }
        else
        {
            if (fabs(variable[i]) > valueMax) valueMax = fabs(variable[i]);
            validDays[i] = true;
            nrValidDays++;
        }
        //printf("%d\n",validDays[i]);
    }
    //printf("%d giorni validi \n",nrValidDays);
    //pressEnterToContinue();
    valueMin = -valueMax;

    double* x = (double *) calloc(nrValidDays, sizeof(double));
    double* y = (double *) calloc(nrValidDays, sizeof(double));
    int indexVariable = 0;
    for (int i=0;i<365;i++)
    {
        if(validDays[i])
        {
            x[indexVariable] = i + 1.0;
            y[indexVariable] = variable[i];
            indexVariable++;
        }
    }
    //for (int i=0; i<nrValidDays; i++)
        //printf("%f %f\n",x[i],y[i]);
    //pressEnterToContinue();

    double *parMin = (double *) calloc(nrPar+1, sizeof(double));
    double* parMax = (double *) calloc(nrPar+1, sizeof(double));
    double* parDelta = (double *) calloc(nrPar+1, sizeof(double));
    double* parMarquardt = (double *) calloc(nrPar+1, sizeof(double));
    for (int i=0;i<(nrPar);i++)
    {
        parMin[i]= valueMin;
        parMax[i]= valueMax;
        parDelta[i] = 0.0001;
    }
    parMin[5]= 365 - EPSILON;
    parMax[5]= 365 + EPSILON;
    parDelta[5] = EPSILON;
    double meanVariable = 0;
    for (int i=0;i<nrValidDays;i++) meanVariable += y[i];
    meanVariable /= nrValidDays;
    parMarquardt[0] = par[0] = meanVariable;
    parMarquardt[1] = par[1] = 0;
    parMarquardt[2] = par[2] = 0;
    parMarquardt[3] = par[3] = 0;
    parMarquardt[4] = par[4] = 0;
    //parMarquardt[5] = par[5] = 0;
    //parMarquardt[6] = par[6] = 0;

    parMarquardt[5] = 365;

    interpolation::fittingMarquardt(parMin,parMax,parMarquardt,nrPar+1,parDelta,10000,0.0001,FUNCTION_CODE_FOURIER_GENERAL_HARMONICS,x,nrValidDays,y);

    for (int i=0;i<nrPar;i++)
    {
        par[i] = parMarquardt[i];
        //printf("%f\n",par[i]);
    }
    //pressEnterToContinue();
    for (int i=0;i<365;i++)
    {
        estimatedVariable[i] = par[0] + par[1]*cos(2*PI/nrEstimatedVariable*i) + par[2]*sin(2*PI/nrEstimatedVariable*i) + par[3]*cos(4*PI/nrEstimatedVariable*i) + par[4]*sin(4*PI/nrEstimatedVariable*i);
        //estimatedVariable[i] += parMarquardt[5]*cos(6*PI/nrEstimatedVariable*i) + parMarquardt[6]*sin(6*PI/nrEstimatedVariable*i);
        //printf("%d %f\n",i, estimatedVariable[i]);
    }
    //pressEnterToContinue();
    // par[0] + par[1]*cos[2*PI/365*X] + par[2]*sin[2*PI/365*X] + par[3]*cos[4*PI/365*X] + par[4]*sin[4*PI/365*X]

    // free memory
    free(x);
    free(y);
    free(parMin);
    free(parMax);
    free(parDelta);
    free(parMarquardt);

}

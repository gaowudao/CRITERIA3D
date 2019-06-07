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
#include "eispack.h"



void weatherGenerator2D::temperatureCompute()
{
    // step 1 of temperature WG2D
    weatherGenerator2D::computeTemperatureParameters();
    weatherGenerator2D::temperaturesCorrelationMatrices();
    weatherGenerator2D::multisiteRandomNumbersTemperature();
    weatherGenerator2D::multisiteTemperatureGeneration();
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

        for (int k=0; k<2; k++)
        {
            for (int j=0; j<2; j++)
            {
                temperatureCoefficients[i].A[k][j]= NODATA;
                temperatureCoefficients[i].B[k][j]= NODATA;
            }
        }
     }
    dailyResidual = (TdailyResidual*) calloc(nrData,sizeof(TdailyResidual));
    for (int i=0; i<nrData; i++)
    {
        dailyResidual[i].maxTDry = 0;
        dailyResidual[i].minTDry = 0;
        dailyResidual[i].maxTWet = 0;
        dailyResidual[i].minTWet = 0;
    }

}

void weatherGenerator2D::initializeTemperaturecorrelationMatrices()
{
    correlationMatrixTemperature.maxT = (double **)calloc(nrStations, sizeof(double *));
    correlationMatrixTemperature.minT = (double **)calloc(nrStations, sizeof(double *));
    for (int i=0;i<nrStations;i++)
    {
        correlationMatrixTemperature.maxT[i] = (double *)calloc(nrStations, sizeof(double));
        correlationMatrixTemperature.minT[i] = (double *)calloc(nrStations, sizeof(double));
        for (int j=0;j<nrStations;j++)
        {
            correlationMatrixTemperature.maxT[i][j] = NODATA;
            correlationMatrixTemperature.minT[i][j] = NODATA;
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
            if(fabs(obsDataD[iStation][iDatum].tMax) > 60) obsDataD[iStation][iDatum].tMax = NODATA;
            if(fabs(obsDataD[iStation][iDatum].tMin) > 60) obsDataD[iStation][iDatum].tMin = NODATA;
        }
        for (int iDatum=0; iDatum<nrData; iDatum++)
        {
           if ((fabs((obsDataD[iStation][iDatum].tMax)))< EPSILON)
            {
                obsDataD[iStation][iDatum].tMax += EPSILON;

            }
            if ((fabs(obsDataD[iStation][iDatum].tMin))< EPSILON)
            {
                obsDataD[iStation][iDatum].tMin += EPSILON;
            }
        }
        for (int iDatum=0; iDatum<nrData; iDatum++)
        {
            int dayOfYear;
            dayOfYear = weatherGenerator2D::doyFromDate(obsDataD[iStation][iDatum].date.day,obsDataD[iStation][iDatum].date.month,obsDataD[iStation][iDatum].date.year);
            //if (dayOfYear == 366) dayOfYear--;
            if ((isLeapYear(obsDataD[iStation][iDatum].date.year)) && (obsDataD[iStation][iDatum].date.month > 2)) dayOfYear--;
            dayOfYear--;
            if (obsDataD[iStation][iDatum].date.month == 2 && obsDataD[iStation][iDatum].date.day == 29)
            {
                dayOfYear-- ;
            }


            if (fabs(obsDataD[iStation][iDatum].prec - NODATA) > EPSILON)
            {
                if (obsDataD[iStation][iDatum].prec > parametersModel.precipitationThreshold)
                {
                    if ((fabs((obsDataD[iStation][iDatum].tMax)- NODATA))> EPSILON)
                    {
                        ++countTMaxWet[dayOfYear];
                        averageTMaxWet[dayOfYear] += obsDataD[iStation][iDatum].tMax;
                    }
                    if ((fabs(obsDataD[iStation][iDatum].tMin - NODATA))> EPSILON)
                    {
                        ++countTMinWet[dayOfYear];
                        averageTMinWet[dayOfYear] += obsDataD[iStation][iDatum].tMin;
                        //printf("%d %f %f\n ",dayOfYear,obsDataD[iStation][iDatum].tMin,(fabs(obsDataD[iStation][iDatum].tMin)-NODATA));
                        //getchar();
                    }
                }
                else if (obsDataD[iStation][iDatum].prec <= parametersModel.precipitationThreshold)
                {
                    if ((fabs((obsDataD[iStation][iDatum].tMax)- NODATA))> EPSILON)
                    {
                        ++countTMaxDry[dayOfYear];
                        averageTMaxDry[dayOfYear] += obsDataD[iStation][iDatum].tMax;
                    }
                    if ((fabs((obsDataD[iStation][iDatum].tMin)- NODATA))> EPSILON)
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
        //pressEnterToContinue();
        for (int iDatum=0; iDatum<nrData; iDatum++)
        {
            int dayOfYear;
            dayOfYear = weatherGenerator2D::doyFromDate(obsDataD[iStation][iDatum].date.day,obsDataD[iStation][iDatum].date.month,obsDataD[iStation][iDatum].date.year);
            //if (dayOfYear == 366) dayOfYear--;
            if ((isLeapYear(obsDataD[iStation][iDatum].date.year)) && (obsDataD[iStation][iDatum].date.month > 2))
            {
                dayOfYear--; // to avoid problems in leap years
            }
            dayOfYear--; // to change from 1-365 to 0-364
            if (obsDataD[iStation][iDatum].date.month == 2 && obsDataD[iStation][iDatum].date.day == 29)
            {
                dayOfYear-- ; // to avoid problems in leap years
            }

            if (fabs(obsDataD[iStation][iDatum].prec - NODATA) > EPSILON)
            {
                if (obsDataD[iStation][iDatum].prec > parametersModel.precipitationThreshold)
                {
                    if ((fabs((obsDataD[iStation][iDatum].tMax) - NODATA))> EPSILON)
                    {
                        stdDevTMaxWet[dayOfYear] += (obsDataD[iStation][iDatum].tMax - averageTMaxWet[dayOfYear])*(obsDataD[iStation][iDatum].tMax - averageTMaxWet[dayOfYear]);
                    }
                    if ((fabs(obsDataD[iStation][iDatum].tMin - NODATA))> EPSILON)
                    {
                        stdDevTMinWet[dayOfYear] += (obsDataD[iStation][iDatum].tMin - averageTMinWet[dayOfYear])*(obsDataD[iStation][iDatum].tMin - averageTMinWet[dayOfYear]);
                        //printf("%d %f %f\n ",dayOfYear,obsDataD[iStation][iDatum].tMin,(fabs(obsDataD[iStation][iDatum].tMin)-NODATA));
                        //getchar();
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

            //printf("%d %f %d %f %d\n",iDay,stdDevTMaxWet[iDay],countTMaxWet[iDay],averageTMaxWet[iDay],countTMaxWet[iDay]);
            //printf("%d %f %d %f %d\n",iDay,stdDevTMinWet[iDay],countTMinWet[iDay],averageTMinWet[iDay],countTMinWet[iDay]);
            //getchar();
        }
        /*
        for (int iDatum=0; iDatum<nrData; iDatum++)
        {
            obsDataD[iStation][iDatum].tMax -= EPSILON;
            obsDataD[iStation][iDatum].tMin -= EPSILON;
        }
        */

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
        weatherGenerator2D::harmonicsFourier(averageTMaxWet,par,nrPar,temperatureCoefficients[iStation].maxTWet.averageEstimation,365);
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

        weatherGenerator2D::computeResiduals(temperatureCoefficients[iStation].maxTDry.averageEstimation,temperatureCoefficients[iStation].maxTWet.averageEstimation,temperatureCoefficients[iStation].maxTDry.stdDevEstimation,temperatureCoefficients[iStation].maxTWet.stdDevEstimation,temperatureCoefficients[iStation].minTDry.averageEstimation,temperatureCoefficients[iStation].minTWet.averageEstimation,temperatureCoefficients[iStation].minTDry.stdDevEstimation,temperatureCoefficients[iStation].minTWet.stdDevEstimation,365,iStation);
        //printf("che d'è?");
        int matrixRang = 2;
        double** matrixCovarianceLag0 = (double **) calloc(matrixRang, sizeof(double*));
        double** matrixCovarianceLag1 = (double **) calloc(matrixRang, sizeof(double*));
        double** matrixA = (double **) calloc(matrixRang, sizeof(double*));
        double** matrixC = (double **) calloc(matrixRang, sizeof(double*));
        double** matrixB = (double **) calloc(matrixRang, sizeof(double*));
        double** matrixDummy = (double **) calloc(matrixRang, sizeof(double*));
        double** eigenvectors = (double **) calloc(matrixRang, sizeof(double*));
        double* eigenvalues = (double *) calloc(matrixRang, sizeof(double));

        for (int i=0;i<matrixRang;i++)
        {
            matrixCovarianceLag0[i] = (double *) calloc(matrixRang, sizeof(double));
            matrixCovarianceLag1[i] = (double *) calloc(matrixRang, sizeof(double));
            matrixA[i] = (double *) calloc(matrixRang, sizeof(double));
            matrixC[i] = (double *) calloc(matrixRang, sizeof(double));
            matrixB[i] = (double *) calloc(matrixRang, sizeof(double));
            matrixDummy[i] = (double *) calloc(matrixRang, sizeof(double));
            eigenvectors[i]=  (double *) calloc(matrixRang, sizeof(double));
            for (int j=0;j<matrixRang;j++)
            {
                matrixCovarianceLag0[i][j] = NODATA;
                matrixCovarianceLag1[i][j] = NODATA;
                matrixA[i][j] = NODATA;
                matrixC[i][j] = NODATA;
                matrixB[i][j] = NODATA;
                matrixDummy[i][j] = NODATA;
                eigenvectors[i][j] = NODATA;
            }
            eigenvalues[i] = NODATA;
        }
        weatherGenerator2D::covarianceOfResiduals(matrixCovarianceLag0,0);
        weatherGenerator2D::covarianceOfResiduals(matrixCovarianceLag1,1);
        /*printf("lag0\n");
        for (int i=0;i<matrixRang;i++)
        {
            for (int j=0; j<matrixRang; j++)
                printf("%f\t",matrixCovarianceLag0[i][j]);
            printf("\n");
        }
        printf("lag1\n");
        for (int i=0;i<matrixRang;i++)
        {
            for (int j=0; j<matrixRang; j++)
                printf("%f\t",matrixCovarianceLag1[i][j]);
            printf("\n");
        }*/

        matricial::inverse(matrixCovarianceLag0,matrixC,matrixRang); // matrixC becomes temporarely the inverse of lag0
        matricial::matrixProduct(matrixCovarianceLag1,matrixC,matrixRang,matrixRang,matrixRang,matrixRang,matrixA);
        matricial::transposedSquareMatrix(matrixCovarianceLag1,matrixRang);
        matricial::matrixProduct(matrixA,matrixCovarianceLag1,matrixRang,matrixRang,matrixRang,matrixRang,matrixC);
        matricial::matrixDifference(matrixCovarianceLag0,matrixC,matrixRang,matrixRang,matrixRang,matrixRang,matrixC);


        /*for (int i=0;i<matrixRang;i++)
            for (int j=0; j<matrixRang; j++)
                printf("%f\n",matrixC[i][j]);
        printf("\n");printf("\n");
        */
        matricial::eigenSystemMatrix2x2(matrixC,eigenvalues,eigenvectors,matrixRang);
        int negativeEigenvalues = 0;
        if (eigenvalues[0] < 0)
        {
            eigenvalues[0] = EPSILON;
            negativeEigenvalues++;
        }
        if (eigenvalues[1] < 0)
        {
            eigenvalues[1] = EPSILON;
            negativeEigenvalues++;
        }
        if (negativeEigenvalues > 0)
        {
            matricial::inverse(eigenvectors,matrixDummy,matrixRang); // matrix C temporarely becomes the inverse matrix of the right eigenvectors
            matrixDummy[0][0] *= eigenvalues[0];
            matrixDummy[0][1] *= eigenvalues[0];
            matrixDummy[1][0] *= eigenvalues[1];
            matrixDummy[1][1] *= eigenvalues[1];
            matricial::matrixProduct(eigenvectors,matrixDummy,matrixRang,matrixRang,matrixRang,matrixRang,matrixC);
            matricial::eigenSystemMatrix2x2(matrixC,eigenvalues,eigenvectors,matrixRang);
        }
        //matricial::transposedMatrix(eigenvectors,2,2,matrixB);
        matricial::inverse(eigenvectors,matrixDummy,2); // compulsory because our algorithm does not look at orthogonal vectors;
        matrixDummy[0][0] *= sqrt(eigenvalues[0]);
        matrixDummy[0][1] *= sqrt(eigenvalues[0]);
        matrixDummy[1][0] *= sqrt(eigenvalues[1]);
        matrixDummy[1][1] *= sqrt(eigenvalues[1]);
        matricial::matrixProduct(eigenvectors,matrixDummy,matrixRang,matrixRang,matrixRang,matrixRang,matrixB);

        for (int i=0;i<matrixRang;i++)
        {
            for (int j=0; j<matrixRang; j++)
            {
                temperatureCoefficients[iStation].A[i][j] = matrixA[i][j];
                temperatureCoefficients[iStation].B[i][j] = matrixB[i][j];
            }
        }

        /*
        for (int i=0;i<matrixRang;i++)
            //printf("%f\n",eigenvalues[i]);
        printf("\n");
        for (int i=0;i<matrixRang;i++)
            for (int j=0; j<matrixRang; j++)
                printf("%f\n",matrixB[i][j]);
        printf("\n"); printf("\n");
        */


        for (int i=0;i<matrixRang;i++)
        {
            free(matrixCovarianceLag0[i]);
            free(matrixCovarianceLag1[i]);
            free(matrixA[i]);
            free(matrixC[i]);
            free(matrixB[i]);
            free(matrixDummy[i]);
            free(eigenvectors[i]);
        }
        free(matrixCovarianceLag0);
        free(matrixCovarianceLag1);
        free(matrixA);
        free(matrixC);
        free(matrixB);
        free(matrixDummy);
        free(eigenvalues);
        free(eigenvectors);


        /*for (int i=0;i<nrData;i++)
        {
            if (dailyResidual[i].maxTDry >5)
            {
                printf("%d %f %f\n",i,dailyResidual[i].maxTDry,dailyResidual[i].minTDry);
                getchar();
            }
        }*/
        //for (int i=0;i<365;i++)
        //{
            //printf("%f %f\n",averageTMaxWet[i],stdDevTMaxWet[i]);
            //printf("%f %f\n",temperatureCoefficients[iStation].maxTWet.averageEstimation[i],temperatureCoefficients[iStation].maxTWet.stdDevEstimation[i]);
            //pressEnterToContinue();
        //}
        //pressEnterToContinue();
    } // end of iStation "for" cycle
    free(dailyResidual);

}

void weatherGenerator2D::harmonicsFourier(double* variable, double *par,int nrPar, double* estimatedVariable, int nrEstimatedVariable)
{
    int maxIterations = 100000;
    int functionCode;

    // find the upper bound
    double valueMax,valueMin;
    bool validDays[365];
    int nrValidDays;

    double valueCurrent;
    valueMax = 0;
    nrValidDays = 0;
    for (int i=0;i<365;i++)
    {
        valueCurrent = variable[i];
        valueCurrent = variable[i];
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
    }
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

    parMarquardt[5] = 365;

    interpolation::fittingMarquardt(parMin,parMax,parMarquardt,nrPar+1,parDelta,10000,0.0001,FUNCTION_CODE_FOURIER_GENERAL_HARMONICS,x,nrValidDays,y);

    for (int i=0;i<nrPar;i++)
    {
        par[i] = parMarquardt[i];
    }
    for (int i=0;i<365;i++)
    {
        estimatedVariable[i] = par[0] + par[1]*cos(2*PI/nrEstimatedVariable*i) + par[2]*sin(2*PI/nrEstimatedVariable*i) + par[3]*cos(4*PI/nrEstimatedVariable*i) + par[4]*sin(4*PI/nrEstimatedVariable*i);
    }

    // free memory
    free(x);
    free(y);
    free(parMin);
    free(parMax);
    free(parDelta);
    free(parMarquardt);

}

void weatherGenerator2D::computeResiduals(double* averageTMaxDry,double* averageTMaxWet,double* stdDevTMaxDry,double* stdDevTMaxWet,double* averageTMinDry,double* averageTMinWet,double* stdDevTMinDry,double* stdDevTMinWet,int lengthArray,int idStation)
{
    for (int i=0; i<nrData; i++)
    {
        dailyResidual[i].maxTDry = 0.;
        dailyResidual[i].minTDry = 0.;
        dailyResidual[i].maxTWet = 0.;
        dailyResidual[i].minTWet = 0.;
        dailyResidual[i].maxT = 0.;
        dailyResidual[i].minT = 0.;
    }
    for (int i = 0; i< nrData ; i++)
    {
        int currentDayOfYear;
        currentDayOfYear = weatherGenerator2D::doyFromDate(obsDataD[idStation][i].date.day,obsDataD[idStation][i].date.month,obsDataD[idStation][i].date.year);

        if ((isLeapYear(obsDataD[idStation][i].date.year)) && (obsDataD[idStation][i].date.month > 2)) currentDayOfYear--;
        currentDayOfYear--;
        if (obsDataD[idStation][i].date.month == 2 && obsDataD[idStation][i].date.day == 29)
        {
            currentDayOfYear-- ;
        }
        if ((fabs(obsDataD[idStation][i].tMax - NODATA)> EPSILON) && (fabs(obsDataD[idStation][i].tMax)> EPSILON) && (obsDataD[idStation][i].prec >= 0))
        {
            if(obsDataD[idStation][i].prec > parametersModel.precipitationThreshold)
            {
                dailyResidual[i].maxTWet = (obsDataD[idStation][i].tMax - averageTMaxWet[currentDayOfYear])/stdDevTMaxWet[currentDayOfYear];
                //if (fabs(obsDataD[idStation][i].tMax)> 40) printf("%d  %f\n", i,obsDataD[idStation][i].tMax);
                if (fabs(dailyResidual[i].maxTWet)> 20) printf("%d  %f %f %f\n", i,obsDataD[idStation][i].tMax,averageTMaxWet[currentDayOfYear],stdDevTMaxWet[currentDayOfYear]);
            }
            else
            {
                //if (fabs(obsDataD[idStation][i].tMax)> 40) printf("%d  %f\n", i,obsDataD[idStation][i].tMax);
                //if (fabs(dailyResidual[i].maxTDry)> 20) printf("%d  %f\n", i,dailyResidual[i].maxTDry);
                dailyResidual[i].maxTDry = (obsDataD[idStation][i].tMax - averageTMaxDry[currentDayOfYear])/stdDevTMaxDry[currentDayOfYear];
                //if (i == 1459) printf("%d %f %f %f \n",currentDayOfYear,obsDataD[idStation][i].tMax, averageTMaxDry[currentDayOfYear], stdDevTMaxDry[currentDayOfYear]);
            }
        }
        if ((fabs(obsDataD[idStation][i].tMin - NODATA)> EPSILON) && (fabs(obsDataD[idStation][i].tMin)> EPSILON) && (obsDataD[idStation][i].prec >= 0))
        {
            if(obsDataD[idStation][i].prec > parametersModel.precipitationThreshold)
            {
                dailyResidual[i].minTWet = (obsDataD[idStation][i].tMin - averageTMinWet[currentDayOfYear])/stdDevTMinWet[currentDayOfYear];
                //if (fabs(obsDataD[idStation][i].tMin)> 40) printf("%d  %f\n", i,obsDataD[idStation][i].tMin);
               //if (fabs(dailyResidual[i].minTWet)> 20) printf("%d  %f\n", i,dailyResidual[i].minTWet);
            }
            else
            {
                dailyResidual[i].minTDry = (obsDataD[idStation][i].tMin - averageTMinDry[currentDayOfYear])/stdDevTMinDry[currentDayOfYear];
                //if (fabs(obsDataD[idStation][i].tMin)> 40) printf("%d  %f\n", i,obsDataD[idStation][i].tMin);
                //if (fabs(dailyResidual[i].minTWet)> 20) printf("%d  %f\n", i,dailyResidual[i].minTWet);
            }
        }
        dailyResidual[i].maxT = dailyResidual[i].maxTWet + dailyResidual[i].maxTDry;
        dailyResidual[i].minT = dailyResidual[i].minTWet + dailyResidual[i].minTDry;
    }
}

void weatherGenerator2D::covarianceOfResiduals(double** covarianceMatrix, int lag)
{
    if (lag == 0)
    {
        covarianceMatrix[0][0] = 1.0;
        covarianceMatrix[1][1] = 1.0;
        covarianceMatrix[1][0] = 0.0;
        covarianceMatrix[0][1] = 0.0;
        int denominator = -1;

        for (int i=0; i<nrData; i++)
        {
            if ((fabs(dailyResidual[i].maxT)> EPSILON) && (fabs(dailyResidual[i].minT)> EPSILON))
            {
                denominator++;
                covarianceMatrix[0][1] += dailyResidual[i].maxT * dailyResidual[i].minT;
            }
        }
        if (denominator > 0)
        {
            covarianceMatrix[0][1] /= denominator;
        }
        covarianceMatrix[1][0] =  covarianceMatrix[0][1];
    }
    else
    {
        covarianceMatrix[0][0] = 0.0;
        covarianceMatrix[1][1] = 0.0;
        covarianceMatrix[1][0] = 0.0;
        covarianceMatrix[0][1] = 0.0;
        int denominator11 = -2;
        int denominator12 = -2;
        int denominator21 = -2;
        int denominator22 = -2;
        for (int i=0; i<nrData-1; i++)
        {
            if ((fabs(dailyResidual[i].maxT)> EPSILON) && (fabs(dailyResidual[i+1].maxT)> EPSILON))
            {
                denominator11++;
                covarianceMatrix[0][0] += dailyResidual[i].maxT*dailyResidual[i+1].maxT;
            }
            if ((fabs(dailyResidual[i].maxT)> EPSILON) && (fabs(dailyResidual[i+1].minT)> EPSILON))
            {
                denominator12++;
                covarianceMatrix[0][1] += dailyResidual[i].maxT*dailyResidual[i+1].minT;
            }
            if ((fabs(dailyResidual[i].minT)> EPSILON) && (fabs(dailyResidual[i+1].maxT)> EPSILON))
            {
                denominator21++;
                covarianceMatrix[1][0] += dailyResidual[i].minT*dailyResidual[i+1].maxT;
            }
            if ((fabs(dailyResidual[i].minT)> EPSILON) && (fabs(dailyResidual[i+1].minT)> EPSILON))
            {
                denominator22++;
                covarianceMatrix[1][1] += dailyResidual[i].minT*dailyResidual[i+1].minT;
            }
        }
        if (denominator11 != 0)
        {
            covarianceMatrix[0][0] /= denominator11;
        }
        if (denominator12 != 0)
        {
            covarianceMatrix[0][1] /= denominator12;
        }
        if (denominator21 != 0)
        {
            covarianceMatrix[1][0] /= denominator21;
        }
        if (denominator22 != 0)
        {
            covarianceMatrix[1][1] /= denominator22;
        }
    }

}

void weatherGenerator2D::temperaturesCorrelationMatrices()
{
    weatherGenerator2D::initializeTemperaturecorrelationMatrices();
    TcorrelationVar maxT;
    TcorrelationVar minT;
    int counterMaxT;
    int counterMinT;
    for (int j=0; j<nrStations;j++)
    {
        correlationMatrixTemperature.maxT[j][j] = 1;
        correlationMatrixTemperature.minT[j][j] = 1;
    }

    // compute correlation for tmax
    for (int j=0; j<nrStations-1;j++)
    {
        for (int i=j+1; i<nrStations;i++)
        {
            counterMaxT = 0;
            maxT.meanValue1=0.;
            maxT.meanValue2=0.;
            maxT.covariance = maxT.variance1 = maxT.variance2 = 0.;

            for (int k=0; k<nrData;k++) // compute the monthly means
            {
                if (((obsDataD[j][k].tMax - NODATA) > EPSILON) && ((obsDataD[i][k].tMax - NODATA) > EPSILON))
                {
                    counterMaxT++;
                    maxT.meanValue1 += obsDataD[j][k].tMax ;
                    maxT.meanValue2 += obsDataD[i][k].tMax;

                }
            }
            if (counterMaxT != 0)
            {
                maxT.meanValue1 /= counterMaxT;
                maxT.meanValue2 /= counterMaxT;
            }
            // printf("counter %d station1 %d station2 %d\n",counterMaxT,j,i);
            // printf("%f  %f\n",maxT.meanValue1,maxT.meanValue2);
            // pressEnterToContinue();
            // compute the monthly rho off-diagonal elements
            for (int k=0; k<nrData;k++)
            {
                if (((obsDataD[j][k].tMax - NODATA) > EPSILON) && ((obsDataD[i][k].tMax - NODATA) > EPSILON))
                {
                    double value1,value2;
                    value1 = obsDataD[j][k].tMax;
                    value2 = obsDataD[i][k].tMax;

                    maxT.covariance += (value1 - maxT.meanValue1)*(value2 - maxT.meanValue2);
                    maxT.variance1 += (value1 - maxT.meanValue1)*(value1 - maxT.meanValue1);
                    maxT.variance2 += (value2 - maxT.meanValue2)*(value2 - maxT.meanValue2);
                }
            }
            correlationMatrixTemperature.maxT[j][i]= maxT.covariance / sqrt(maxT.variance1*maxT.variance2);
            correlationMatrixTemperature.maxT[i][j] = correlationMatrixTemperature.maxT[j][i];
        }
    }

    // compute correlation for tmin
    for (int j=0; j<nrStations-1;j++)
    {
        for (int i=j+1; i<nrStations;i++)
        {
            counterMinT = 0;
            minT.meanValue1=0.;
            minT.meanValue2=0.;
            minT.covariance = minT.variance1 = minT.variance2 = 0.;

            for (int k=0; k<nrData;k++) // compute the monthly means
            {
                if (((obsDataD[j][k].tMin - NODATA) > EPSILON) && ((obsDataD[i][k].tMin - NODATA) > EPSILON))
                {
                    counterMinT++;
                    minT.meanValue1 += obsDataD[j][k].tMin ;
                    minT.meanValue2 += obsDataD[i][k].tMin;

                }
            }
            if (counterMaxT != 0)
            {
                minT.meanValue1 /= counterMinT;
                minT.meanValue2 /= counterMinT;
            }
             //printf("counter %d station1 %d station2 %d\n",counterMinT,j,i);
             //printf("%f  %f\n",minT.meanValue1,minT.meanValue2);
             //pressEnterToContinue();
            // compute the monthly rho off-diagonal elements
            for (int k=0; k<nrData;k++)
            {
                if (((obsDataD[j][k].tMin - NODATA) > EPSILON) && ((obsDataD[i][k].tMin - NODATA) > EPSILON))
                {
                    double value1,value2;
                    value1 = obsDataD[j][k].tMin;
                    value2 = obsDataD[i][k].tMin;

                    minT.covariance += (value1 - minT.meanValue1)*(value2 - minT.meanValue2);
                    minT.variance1 += (value1 - minT.meanValue1)*(value1 - minT.meanValue1);
                    minT.variance2 += (value2 - minT.meanValue2)*(value2 - minT.meanValue2);
                }
            }
            correlationMatrixTemperature.minT[j][i]= minT.covariance / sqrt(minT.variance1*minT.variance2);
            correlationMatrixTemperature.minT[i][j] = correlationMatrixTemperature.minT[j][i];
        }
    }

    for (int i=0;i<nrStations;i++)
    {
        for (int j=0; j<nrStations;j++)
        {
           printf("%f\t",correlationMatrixTemperature.maxT[i][j]);
        }
        printf("\n");
    }


    for (int i=0;i<nrStations;i++)
    {
        for (int j=0; j<nrStations;j++)
        {
           printf("%f\t",correlationMatrixTemperature.minT[i][j]);
        }
        printf("\n");
    }
}

void weatherGenerator2D::initializeNormalRandomMatricesTemperatures()
{
    int lengthOfRandomSeries;
    lengthOfRandomSeries = parametersModel.yearOfSimulation*365;
    normRandomMaxT = (double**)calloc(lengthOfRandomSeries, sizeof(double*));
    normRandomMinT = (double**)calloc(lengthOfRandomSeries, sizeof(double*));
    for (int i=0;i<lengthOfRandomSeries;i++)
    {
        normRandomMaxT[i] = (double*)calloc(nrStations, sizeof(double));
        normRandomMinT[i] = (double*)calloc(nrStations, sizeof(double));
    }
    for (int i=0; i<nrStations;i++)
    {
        for (int j=0; j<lengthOfRandomSeries;j++)
        {
            normRandomMaxT[j][i] = NODATA;
            normRandomMinT[j][i] = NODATA;
        }
    }
}

void weatherGenerator2D::multisiteRandomNumbersTemperature()
{
    weatherGenerator2D::initializeNormalRandomMatricesTemperatures();
    int gasDevIset;
    double gasDevGset;
    srand (time(NULL));
    int lengthOfRandomSeries;
    lengthOfRandomSeries = parametersModel.yearOfSimulation*365;
    int nrSquareOfStations;
    nrSquareOfStations = nrStations*nrStations;

    double* correlationArray = (double *) calloc(nrSquareOfStations, sizeof(double));
    double* eigenvectors = (double *) calloc(nrSquareOfStations, sizeof(double));
    double* eigenvalues = (double *) calloc(nrStations, sizeof(double));
    double** dummyMatrix = (double**)calloc(nrStations, sizeof(double*));
    double** dummyMatrix2 = (double**)calloc(nrStations, sizeof(double*));
    double** dummyMatrix3 = (double**)calloc(nrStations, sizeof(double*));
    double** normRandom = (double**)calloc(nrStations, sizeof(double*));
    double** normRandom2 = (double**)calloc(nrStations, sizeof(double*));
    // initialization internal arrays
    for (int i=0;i<nrStations;i++)
    {
        dummyMatrix[i]= (double*)calloc(nrStations, sizeof(double));
        dummyMatrix2[i]= (double*)calloc(nrStations, sizeof(double));
        dummyMatrix3[i] = (double*)calloc(nrStations, sizeof(double));
        normRandom[i] = (double*)calloc(lengthOfRandomSeries, sizeof(double));
        normRandom2[i] = (double*)calloc(lengthOfRandomSeries, sizeof(double));
    }
    int counter;
    bool isLowerDiagonal;

    // for Tmax
    counter = 0;
    for (int i=0; i<nrStations;i++)
    {
        for (int j=0; j<nrStations;j++)
        {
            correlationArray[counter] = correlationMatrixTemperature.maxT[i][j];
            eigenvectors[counter] = NODATA;
            counter++;
        }
        for (int j=0; j<lengthOfRandomSeries;j++)
        {
            normRandom[i][j] = NODATA;
            normRandom2[i][j] = NODATA;
        }
        eigenvalues[i]= NODATA;
    }

    eigenproblem::rs(nrStations,correlationArray,eigenvalues,true,eigenvectors);
    counter = 0;
    for (int i=0; i<nrStations;i++)
    {
        if (eigenvalues[i] < 0) counter++;
    }

    if (counter > 0)
    {
        counter=0;
        for (int i=0;i<nrStations;i++)
        {
            for (int j=0;j<nrStations;j++)
            {
                dummyMatrix[j][i]= eigenvectors[counter];
                dummyMatrix2[i][j]= eigenvectors[counter]*eigenvalues[i];
                counter++;
            }
        }
        matricial::matrixProduct(dummyMatrix,dummyMatrix2,nrStations,nrStations,nrStations,nrStations,correlationMatrixTemperature.maxT);
    }

    for (int i=0;i<nrStations;i++)
    {
        for (int j=0;j<nrStations;j++)
        {
            dummyMatrix[i][j] = correlationMatrixTemperature.maxT[i][j];
            dummyMatrix2[i][j] = correlationMatrixTemperature.maxT[i][j];
        }

    }

    isLowerDiagonal = false;
    matricial::choleskyDecompositionTriangularMatrix(dummyMatrix,nrStations,isLowerDiagonal);
    isLowerDiagonal = true;
    matricial::choleskyDecompositionTriangularMatrix(dummyMatrix2,nrStations,isLowerDiagonal);
    matricial::matrixProduct(dummyMatrix,dummyMatrix2,nrStations,nrStations,nrStations,nrStations,dummyMatrix3);
    isLowerDiagonal = true;
    matricial::choleskyDecompositionTriangularMatrix(dummyMatrix3,nrStations,isLowerDiagonal);
    gasDevGset = 0;
    gasDevIset = 0;
    for (int i=0;i<nrStations;i++)
    {
        for (int j=0;j<lengthOfRandomSeries;j++)
        {
            normRandom[i][j] = myrandom::normalRandom(&gasDevIset,&gasDevGset);
        }
    }
    matricial::matrixProduct(dummyMatrix3,normRandom,nrStations,nrStations,nrStations,lengthOfRandomSeries,normRandom2);
    matricial::transposedMatrix(normRandom2,nrStations,lengthOfRandomSeries,normRandomMaxT);

    // for Tmin
    counter = 0;
    for (int i=0; i<nrStations;i++)
    {
        for (int j=0; j<nrStations;j++)
        {
            correlationArray[counter] = correlationMatrixTemperature.minT[i][j];
            eigenvectors[counter] = NODATA;
            counter++;
        }
        for (int j=0; j<lengthOfRandomSeries;j++)
        {
            normRandom[i][j] = NODATA;
            normRandom2[i][j] = NODATA;
        }
        eigenvalues[i]= NODATA;
    }

    eigenproblem::rs(nrStations,correlationArray,eigenvalues,true,eigenvectors);
    counter = 0;
    for (int i=0; i<nrStations;i++)
    {
        if (eigenvalues[i] < 0) counter++;
    }

    if (counter > 0)
    {
        counter=0;
        for (int i=0;i<nrStations;i++)
        {
            for (int j=0;j<nrStations;j++)
            {
                dummyMatrix[j][i]= eigenvectors[counter];
                dummyMatrix2[i][j]= eigenvectors[counter]*eigenvalues[i];
                counter++;
            }
        }
        matricial::matrixProduct(dummyMatrix,dummyMatrix2,nrStations,nrStations,nrStations,nrStations,correlationMatrixTemperature.maxT);
    }

    for (int i=0;i<nrStations;i++)
    {
        for (int j=0;j<nrStations;j++)
        {
            dummyMatrix[i][j] = correlationMatrixTemperature.minT[i][j];
            dummyMatrix2[i][j] = correlationMatrixTemperature.minT[i][j];
        }

    }

    isLowerDiagonal = false;
    matricial::choleskyDecompositionTriangularMatrix(dummyMatrix,nrStations,isLowerDiagonal);
    isLowerDiagonal = true;
    matricial::choleskyDecompositionTriangularMatrix(dummyMatrix2,nrStations,isLowerDiagonal);
    matricial::matrixProduct(dummyMatrix,dummyMatrix2,nrStations,nrStations,nrStations,nrStations,dummyMatrix3);
    isLowerDiagonal = true;
    matricial::choleskyDecompositionTriangularMatrix(dummyMatrix3,nrStations,isLowerDiagonal);
    gasDevGset = 0;
    gasDevIset = 0;
    for (int i=0;i<nrStations;i++)
    {
        for (int j=0;j<lengthOfRandomSeries;j++)
        {
            normRandom[i][j] = myrandom::normalRandom(&gasDevIset,&gasDevGset);
        }
    }
    matricial::matrixProduct(dummyMatrix3,normRandom,nrStations,nrStations,nrStations,lengthOfRandomSeries,normRandom2);
    matricial::transposedMatrix(normRandom2,nrStations,lengthOfRandomSeries,normRandomMinT);




    for (int i=0;i<nrStations;i++)
    {
        free(dummyMatrix[i]);
        free(dummyMatrix2[i]);
        free(dummyMatrix3[i]);
        free(normRandom[i]);
        free(normRandom2[i]);
    }

    free(dummyMatrix);
    free(dummyMatrix2);
    free(dummyMatrix3);
    free(correlationArray);
    free(eigenvalues);
    free(eigenvectors);
    free(normRandom);
    free(normRandom2);

}

void weatherGenerator2D::initializeMultiOccurrenceTemperature(int length)
{
    multiOccurrenceTemperature = (TmultiOccurrenceTemperature *) calloc(length, sizeof(TmultiOccurrenceTemperature));
    for (int i=0;i<length;i++)
    {
        multiOccurrenceTemperature[i].occurrence_simulated = (double *) calloc(nrStations, sizeof(double));
        for(int j=0;j<nrStations;j++)
        {
            multiOccurrenceTemperature[i].occurrence_simulated[j] = NODATA;
        }
        multiOccurrenceTemperature[i].day_simulated = NODATA;
        multiOccurrenceTemperature[i].month_simulated = NODATA;
        multiOccurrenceTemperature[i].year_simulated = NODATA;
    }
}

void weatherGenerator2D::initializeTemperaturesOutput(int length)
{
    maxTGenerated = (double **) calloc(length, sizeof(double *));
    minTGenerated = (double **) calloc(length, sizeof(double *));
    for (int i=0;i<length;i++)
    {
        maxTGenerated[i] = (double *) calloc(nrStations, sizeof(double));
        minTGenerated[i] = (double *) calloc(nrStations, sizeof(double));
        for(int j=0;j<nrStations;j++)
        {
            maxTGenerated[i][j] = NODATA;
            minTGenerated[i][j] = NODATA;
        }

    }
}



void weatherGenerator2D::multisiteTemperatureGeneration()
{
    int lengthOfRandomSeries;
    lengthOfRandomSeries = parametersModel.yearOfSimulation*365;
    weatherGenerator2D::initializeMultiOccurrenceTemperature(lengthOfRandomSeries);
    // fill in the data of simulations
    int day,month;
    int counter = 0;
    for (int j=1;j<=parametersModel.yearOfSimulation;j++)
    {
        for (int i=0; i<365;i++)
        {
            weatherGenerator2D::dateFromDoy(i+1,1,&day,&month);
            multiOccurrenceTemperature[counter].year_simulated = j;
            multiOccurrenceTemperature[counter].month_simulated = month;
            multiOccurrenceTemperature[counter].day_simulated = day;
            counter++;
        }
    }

    for (int j=0;j<12;j++)
    {
        int counter2 = 0;
        counter = 0;
        for (int k=0; k<lengthOfRandomSeries; k++)
        {
            if (multiOccurrenceTemperature[counter].month_simulated == (j+1))
            {
                for (int i=0;i<nrStations;i++)
                {
                    multiOccurrenceTemperature[counter].occurrence_simulated[i] = randomMatrix[j].matrixOccurrences[i][counter2];
                    //printf("month %d %d %d %f\n",j+1,counter,counter2,randomMatrix[j].matrixOccurrences[i][counter2]);
                }
                //getchar();
                counter2++;
            }
                counter++;
        }
    }
    weatherGenerator2D::initializeTemperaturesOutput(lengthOfRandomSeries);
    double* X = (double*)calloc(lengthOfRandomSeries, sizeof(double));
    double** averageT = (double**)calloc(4, sizeof(double*));
    double** stdDevT = (double**)calloc(4, sizeof(double*));
    for (int i=0;i<4;i++)
    {
        averageT[i] = (double*)calloc(lengthOfRandomSeries, sizeof(double));
        stdDevT[i] = (double*)calloc(lengthOfRandomSeries, sizeof(double));
        for (int j=0; j<lengthOfRandomSeries;j++)
        {
            averageT[i][j] = NODATA;
            stdDevT[i][j] = NODATA;
        }
    }

    for (int i=0; i<lengthOfRandomSeries ; i++)
    {
        X[i] = NODATA;
    }
    for (int i=0;i<nrStations;i++)
    {
        int jModulo;
        for (int j=0;j<lengthOfRandomSeries;j++)
        {
            X[j] = multiOccurrenceTemperature[j].occurrence_simulated[i];
            jModulo = j%365;

            averageT[0][j] = temperatureCoefficients[i].maxTDry.averageEstimation[jModulo];
            averageT[1][j] = temperatureCoefficients[i].minTDry.averageEstimation[jModulo];
            averageT[2][j] = temperatureCoefficients[i].maxTWet.averageEstimation[jModulo];
            averageT[3][j] = temperatureCoefficients[i].minTWet.averageEstimation[jModulo];

            stdDevT[0][j] = temperatureCoefficients[i].maxTDry.stdDevEstimation[jModulo];
            stdDevT[1][j] = temperatureCoefficients[i].minTDry.stdDevEstimation[jModulo];
            stdDevT[2][j] = temperatureCoefficients[i].maxTWet.stdDevEstimation[jModulo];
            stdDevT[3][j] = temperatureCoefficients[i].minTWet.stdDevEstimation[jModulo];
        }
        double* residuals = (double*)calloc(2, sizeof(double));
        residuals[0] = residuals[1] = 0;
        double** ksi = (double**)calloc(2, sizeof(double*));
        double** eps = (double**)calloc(2, sizeof(double*));
        for (int j=0;j<2;j++)
        {
            ksi[j] = (double*)calloc(lengthOfRandomSeries, sizeof(double));
            eps[j] = (double*)calloc(lengthOfRandomSeries, sizeof(double));
            for (int k=0;k<lengthOfRandomSeries;k++)
            {
               ksi[j][k] = 0;
               eps[j][k] = 0;
            }
        }

        for (int j=0;j<lengthOfRandomSeries;j++)
        {
            eps[0][j] = normRandomMaxT[j][i];
            eps[1][j] = normRandomMinT[j][i];
        }

        for (int j=0;j<lengthOfRandomSeries;j++)
        {
            residuals[0] = temperatureCoefficients[i].A[0][0]*residuals[0] + temperatureCoefficients[i].A[0][1]*residuals[1];
            residuals[0] += temperatureCoefficients[i].B[0][0]*eps[0][j] + temperatureCoefficients[i].B[0][1]*eps[1][j];
            residuals[1] = temperatureCoefficients[i].A[1][0]*residuals[0] + temperatureCoefficients[i].A[1][1]*residuals[1];
            residuals[1] += temperatureCoefficients[i].B[1][0]*eps[0][j] + temperatureCoefficients[i].B[1][1]*eps[1][j];
            ksi[0][j] = residuals[0];
            ksi[1][j] = residuals[1];
        }

        double** cAverage = (double**)calloc(2, sizeof(double*));
        double** cStdDev = (double**)calloc(2, sizeof(double*));
        double** Xp = (double**)calloc(2, sizeof(double*));

        for (int j=0;j<2;j++)
        {
            cAverage[j] = (double*)calloc(lengthOfRandomSeries, sizeof(double));
            cStdDev[j] = (double*)calloc(lengthOfRandomSeries, sizeof(double));
            Xp[j] = (double*)calloc(lengthOfRandomSeries, sizeof(double));

            for (int k=0;k<lengthOfRandomSeries;k++)
            {
                cAverage[j][k] = NODATA;
                cStdDev[j][k] = NODATA;
                Xp[j][k] = NODATA;
            }
        }

        for (int j=0;j<lengthOfRandomSeries;j++)
        {
            cAverage[0][j] = X[j]*averageT[0][j] + (1- X[j])*averageT[2][j]; // for Tmax
            cAverage[1][j] = X[j]*averageT[1][j] + (1- X[j])*averageT[3][j]; // for Tmin
            cStdDev[0][j] = X[j]*stdDevT[0][j] + (1-X[j])*stdDevT[2][j]; // for Tmax
            cStdDev[1][j] = X[j]*stdDevT[1][j] + (1-X[j])*stdDevT[3][j]; // for Tmin
        }

        for (int j=0;j<lengthOfRandomSeries;j++)
        {
            if(cStdDev[0][j] >= cStdDev[1][j])
            {
                Xp[1][j] = ksi[1][j]*cStdDev[1][j] + cAverage[1][j];
                Xp[0][j] = ksi[0][j]*sqrt(cStdDev[0][j]*cStdDev[0][j] - cStdDev[1][j]*cStdDev[1][j]) + (cAverage[0][j] - cAverage[1][j]) + Xp[1][j];
            }
            else
            {
                Xp[0][j] = ksi[0][j]*cStdDev[0][j] + cAverage[0][j];
                Xp[1][j] = ksi[1][j]*sqrt(cStdDev[1][j]*cStdDev[1][j] - cStdDev[0][j]*cStdDev[0][j]) - (cAverage[0][j] - cAverage[1][j]) + Xp[0][j];
            }
        }

        for (int j=0;j<lengthOfRandomSeries;j++)
        {
            if (Xp[0][j] <= Xp[1][j])
            {
                Xp[1][j] = Xp[0][j] - 0.2*fabs(Xp[0][j]);
            }
        }
        for (int j=0;j<lengthOfRandomSeries;j++)
        {
            maxTGenerated[j][i] = Xp[0][j];
            minTGenerated[j][i] = Xp[1][j];
        }

        free(ksi[0]);
        free(ksi[1]);
        free(eps[0]);
        free(eps[1]);
        free(cAverage[0]);
        free(cAverage[1]);
        free(cStdDev[0]);
        free(cStdDev[1]);
        free(Xp[0]);
        free(Xp[1]);
        free(ksi);
        free(eps);
        free(cAverage);
        free(cStdDev);
        free(residuals);
    }

    for (int i=0;i<4;i++)
    {
        free(averageT[i]);
        free(stdDevT[i]);
    }
    free(averageT);
    free(stdDevT);
    free(X);
}

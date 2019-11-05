#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>

#include "wg2D.h"
#include "commonConstants.h"
#include "furtherMathFunctions.h"
//#include "statistics.h"
//#include "eispack.h"
//#include "gammaFunction.h"
#include "crit3dDate.h"
#include "weatherGenerator.h"
//#include "eispack.h"

void weatherGenerator2D::initializePrecipitationAmountParameters()
{
    precipitationAmount = (Tvariable *)calloc(nrStations, sizeof(Tvariable));
    for (int i = 0; i < nrStations; i++)
    {
        precipitationAmount[i].averageEstimation = (double *)calloc(365, sizeof(double));
        precipitationAmount[i].stdDevEstimation = (double *)calloc(365, sizeof(double));
        for (int j=0; j<365; j++)
        {
            precipitationAmount[i].averageEstimation[j] = NODATA;
            precipitationAmount[i].stdDevEstimation[j] = NODATA;
        }
        precipitationAmount[i].averageFourierParameters.a0 = NODATA;
        precipitationAmount[i].averageFourierParameters.aCos1 = NODATA;
        precipitationAmount[i].averageFourierParameters.aSin1 = NODATA;
        precipitationAmount[i].averageFourierParameters.aCos2 = NODATA;
        precipitationAmount[i].averageFourierParameters.aSin2 = NODATA;
        precipitationAmount[i].standardDeviationFourierParameters.a0 = NODATA;
        precipitationAmount[i].standardDeviationFourierParameters.aCos1 = NODATA;
        precipitationAmount[i].standardDeviationFourierParameters.aSin1 = NODATA;
        precipitationAmount[i].standardDeviationFourierParameters.aCos2 = NODATA;
        precipitationAmount[i].standardDeviationFourierParameters.aSin2 = NODATA;
    }
}

void weatherGenerator2D::computeprecipitationAmountParameters()
{
    weatherGenerator2D::initializePrecipitationAmountParameters();
    for (int iStation=0; iStation<nrStations; iStation++)
    {
        //double averageAmountPrec[365];
        double stdDevAmountPrec[365];
        int countAmountPrec[365];

        float* averageMonthlyAmountPrec = nullptr;
        averageMonthlyAmountPrec = (float *)calloc(12, sizeof(float));
        double* averageMonthlyAmountPrecLarger = nullptr;
        averageMonthlyAmountPrecLarger = (double *)calloc(14, sizeof(double));
        double* month = nullptr;
        month = (double *)calloc(14, sizeof(double));

        float* averageAmountPrec = nullptr;
        averageAmountPrec = (float *)calloc(365, sizeof(float));
        //double stdDevMonthlyAmountPrec[12];
        int countMonthlyAmountPrec[12];

        int finalDay = 365;
        for (int iMonth=0;iMonth<12;iMonth++)
        {
            averageMonthlyAmountPrec[iMonth] = 0;
            //stdDevMonthlyAmountPrec[iMonth] = 0;
            countMonthlyAmountPrec[iMonth] = 0;
        }

        //month = {, 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365, 396};
        month[0] = -31;
        month[1] = 0;
        month[2] = 31;
        month[3] = 59;
        month[4] = 90;
        month[5] = 120;
        month[6] = 151;
        month[7] = 181;
        month[8] = 212;
        month[9] = 243;
        month[10] = 273;
        month[11] = 304;
        month[12] = 334;
        month[13] = 365;

        for (int iMonth=0;iMonth<14;iMonth++)
        {
            month[iMonth] += 15;
        }

        for (int iDay=0;iDay<finalDay;iDay++)
        {

            averageAmountPrec[iDay] = 0;
            stdDevAmountPrec[iDay] = 0;
            countAmountPrec[iDay] = 0;
        }
        for (int iDatum=0; iDatum<nrData; iDatum++)
        {
            if(fabs(obsDataD[iStation][iDatum].prec) > 600) obsDataD[iStation][iDatum].prec = NODATA;
            if((obsDataD[iStation][iDatum].prec) < 0) obsDataD[iStation][iDatum].prec = NODATA;
        }
        /*for (int iDatum=0; iDatum<nrData; iDatum++)
        {
           if ((fabs((obsDataD[iStation][iDatum].tMax)))< EPSILON)
            {
                obsDataD[iStation][iDatum].tMax += EPSILON;

            }
            if ((fabs(obsDataD[iStation][iDatum].tMin))< EPSILON)
            {
                obsDataD[iStation][iDatum].tMin += EPSILON;
            }
        }*/
        // compute average precipitation of the stations
        for (int iDatum=0; iDatum<nrData; iDatum++)
        {
            //int dayOfYear;
            //dayOfYear = weatherGenerator2D::doyFromDate(obsDataD[iStation][iDatum].date.day,obsDataD[iStation][iDatum].date.month,obsDataD[iStation][iDatum].date.year);
            if (fabs(obsDataD[iStation][iDatum].prec - NODATA) > EPSILON)
            {
                if (obsDataD[iStation][iDatum].prec > parametersModel.precipitationThreshold)
                {
                    if ((fabs((obsDataD[iStation][iDatum].prec)- NODATA))> EPSILON)
                    {
                        ++countMonthlyAmountPrec[obsDataD[iStation][iDatum].date.month-1];
                        averageMonthlyAmountPrec[obsDataD[iStation][iDatum].date.month-1] += obsDataD[iStation][iDatum].prec - parametersModel.precipitationThreshold;
                        //printf("%d %f\n",countMonthlyAmountPrec[obsDataD[iStation][iDatum].date.month-1],averageMonthlyAmountPrec[obsDataD[iStation][iDatum].date.month-1]);
                    }

                }

            }
            //pressEnterToContinue();
        }

        for (int iMonth=0; iMonth<12; iMonth++)
        {
            if (countMonthlyAmountPrec[iMonth] != 0) averageMonthlyAmountPrec[iMonth] /= countMonthlyAmountPrec[iMonth];
            else averageMonthlyAmountPrec[iMonth] = NODATA;
        }

        for (int iMonth=0; iMonth<12; iMonth++)
        {
            averageMonthlyAmountPrecLarger[iMonth+1] = averageMonthlyAmountPrec[iMonth];
        }
        averageMonthlyAmountPrecLarger[0]  = averageMonthlyAmountPrec[11];
        averageMonthlyAmountPrecLarger[13] = averageMonthlyAmountPrec[0];
        //double risultato;
        for (int jjj=0; jjj<365; jjj++)
        {
            averageAmountPrec[jjj] = interpolation::cubicSpline(jjj*1.0,month,averageMonthlyAmountPrecLarger,14);
        }
        pressEnterToContinue();
        double dummy;
        for (int i=0; i<365 ; i++)
        {
            precipitationAmount[iStation].averageEstimation[i] = double(averageAmountPrec[(i+334)%365]);
        }

        //for (int i=0;i<365;i++) {printf("prima %d %f\n",(i+334)%365,precipitationAmount[iStation].averageEstimation[i]);
        //pressEnterToContinue();}
        free (averageAmountPrec);
        free (averageMonthlyAmountPrec);
        free (averageMonthlyAmountPrecLarger);
        free (month);


    }
}

void weatherGenerator2D::getSeasonalMeanPrecipitation(int iStation, int iSeason, int length, double* meanPrec)
{
    int index = 0;
    if (iSeason == 0)
    {
        for (int j=0; j<parametersModel.yearOfSimulation ; j++)
        {
            for (int i=0; i<31 ; i++)
            {
                meanPrec[index] = precipitationAmount[iStation].averageEstimation[i];
                index++;
            }
        }
        for (int j=0; j<parametersModel.yearOfSimulation ; j++)
        {
            for (int i=0; i<31 ; i++)
            {
                meanPrec[index] = precipitationAmount[iStation].averageEstimation[i+31];
                index++;
            }
        }
        for (int j=0; j<parametersModel.yearOfSimulation ; j++)
        {
            for (int i=0; i<28 ; i++)
            {
                meanPrec[index] = precipitationAmount[iStation].averageEstimation[i+62];
                index++;
            }
        }
    }
    else if (iSeason == 1)
    {
        for (int j=0; j<parametersModel.yearOfSimulation ; j++)
        {
            for (int i=0; i<31 ; i++)
            {
                meanPrec[index] = precipitationAmount[iStation].averageEstimation[i+90];
                index++;
            }
        }
        for (int j=0; j<parametersModel.yearOfSimulation ; j++)
        {
            for (int i=0; i<30 ; i++)
            {
                meanPrec[index] = precipitationAmount[iStation].averageEstimation[i+121];
                index++;
            }
        }
        for (int j=0; j<parametersModel.yearOfSimulation ; j++)
        {
            for (int i=0; i<31 ; i++)
            {
                meanPrec[index] = precipitationAmount[iStation].averageEstimation[i+151];
                index++;
            }
        }
    }
    else if (iSeason == 2)
    {
        for (int j=0; j<parametersModel.yearOfSimulation ; j++)
        {
            for (int i=0; i<30 ; i++)
            {
                meanPrec[index] = precipitationAmount[iStation].averageEstimation[i+182];
                index++;
            }
        }
        for (int j=0; j<parametersModel.yearOfSimulation ; j++)
        {
            for (int i=0; i<31 ; i++)
            {
                meanPrec[index] = precipitationAmount[iStation].averageEstimation[i+212];
                index++;
            }
        }
        for (int j=0; j<parametersModel.yearOfSimulation ; j++)
        {
            for (int i=0; i<31 ; i++)
            {
                meanPrec[index] = precipitationAmount[iStation].averageEstimation[i+243];
                index++;
            }
        }
    }
    else if (iSeason == 3)
    {
        for (int j=0; j<parametersModel.yearOfSimulation ; j++)
        {
            for (int i=0; i<30 ; i++)
            {
                meanPrec[index] = precipitationAmount[iStation].averageEstimation[i+274];
                index++;
            }
        }
        for (int j=0; j<parametersModel.yearOfSimulation ; j++)
        {
            for (int i=0; i<31 ; i++)
            {
                meanPrec[index] = precipitationAmount[iStation].averageEstimation[i+304];
                index++;
            }
        }
        for (int j=0; j<parametersModel.yearOfSimulation ; j++)
        {
            for (int i=0; i<30 ; i++)
            {
                meanPrec[index] = precipitationAmount[iStation].averageEstimation[i+335];
                index++;
            }
        }
    }


}

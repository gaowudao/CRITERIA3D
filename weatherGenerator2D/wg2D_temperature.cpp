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



void weatherGenerator2D::temperatureCompute()
{
    // step 1 of temperature WG2D
    weatherGenerator2D::computeTemperatureParameters();
    // step 2 of temperature WG2D
    // step 3 of temperature WG2D
    // step 4 of temperature WG2D
    // step 5 of temperature WG2D
}

void weatherGenerator2D::initializeTemperatureParameters()
{
    // initialize temp parameters
    temperatureCoefficients = (TtemperatureCoefficients *)calloc(nrStations, sizeof(TtemperatureCoefficients));

    temperatureCoefficients->maxTDry.averageFourierParameters.a0 = NODATA;
    temperatureCoefficients->maxTDry.averageFourierParameters.aCos1 = NODATA;
    temperatureCoefficients->maxTDry.averageFourierParameters.aSin1 = NODATA;
    temperatureCoefficients->maxTDry.averageFourierParameters.aCos2 = NODATA;
    temperatureCoefficients->maxTDry.averageFourierParameters.aSin2 = NODATA;
    temperatureCoefficients->maxTDry.standardDeviationFourierParameters.a0 = NODATA;
    temperatureCoefficients->maxTDry.standardDeviationFourierParameters.aCos1 = NODATA;
    temperatureCoefficients->maxTDry.standardDeviationFourierParameters.aSin1 = NODATA;
    temperatureCoefficients->maxTDry.standardDeviationFourierParameters.aCos2 = NODATA;
    temperatureCoefficients->maxTDry.standardDeviationFourierParameters.aSin2 = NODATA;

    temperatureCoefficients->minTDry.averageFourierParameters.a0 = NODATA;
    temperatureCoefficients->minTDry.averageFourierParameters.aCos1 = NODATA;
    temperatureCoefficients->minTDry.averageFourierParameters.aSin1 = NODATA;
    temperatureCoefficients->minTDry.averageFourierParameters.aCos2 = NODATA;
    temperatureCoefficients->minTDry.averageFourierParameters.aSin2 = NODATA;
    temperatureCoefficients->minTDry.standardDeviationFourierParameters.a0 = NODATA;
    temperatureCoefficients->minTDry.standardDeviationFourierParameters.aCos1 = NODATA;
    temperatureCoefficients->minTDry.standardDeviationFourierParameters.aSin1 = NODATA;
    temperatureCoefficients->minTDry.standardDeviationFourierParameters.aCos2 = NODATA;
    temperatureCoefficients->minTDry.standardDeviationFourierParameters.aSin2 = NODATA;

    temperatureCoefficients->maxTWet.averageFourierParameters.a0 = NODATA;
    temperatureCoefficients->maxTWet.averageFourierParameters.aCos1 = NODATA;
    temperatureCoefficients->maxTWet.averageFourierParameters.aSin1 = NODATA;
    temperatureCoefficients->maxTWet.averageFourierParameters.aCos2 = NODATA;
    temperatureCoefficients->maxTWet.averageFourierParameters.aSin2 = NODATA;
    temperatureCoefficients->maxTWet.standardDeviationFourierParameters.a0 = NODATA;
    temperatureCoefficients->maxTWet.standardDeviationFourierParameters.aCos1 = NODATA;
    temperatureCoefficients->maxTWet.standardDeviationFourierParameters.aSin1 = NODATA;
    temperatureCoefficients->maxTWet.standardDeviationFourierParameters.aCos2 = NODATA;
    temperatureCoefficients->maxTWet.standardDeviationFourierParameters.aSin2 = NODATA;

    temperatureCoefficients->minTWet.averageFourierParameters.a0 = NODATA;
    temperatureCoefficients->minTWet.averageFourierParameters.aCos1 = NODATA;
    temperatureCoefficients->minTWet.averageFourierParameters.aSin1 = NODATA;
    temperatureCoefficients->minTWet.averageFourierParameters.aCos2 = NODATA;
    temperatureCoefficients->minTWet.averageFourierParameters.aSin2 = NODATA;
    temperatureCoefficients->minTWet.standardDeviationFourierParameters.a0 = NODATA;
    temperatureCoefficients->minTWet.standardDeviationFourierParameters.aCos1 = NODATA;
    temperatureCoefficients->minTWet.standardDeviationFourierParameters.aSin1 = NODATA;
    temperatureCoefficients->minTWet.standardDeviationFourierParameters.aCos2 = NODATA;
    temperatureCoefficients->minTWet.standardDeviationFourierParameters.aSin2 = NODATA;

    for (int i=0; i<2; i++)
    {
        for (int j=0; j<2; j++)
        {
            temperatureCoefficients->A[i][j]= NODATA;
            temperatureCoefficients->B[i][j]= NODATA;
        }
    }
}


void weatherGenerator2D::computeTemperatureParameters()
{
    weatherGenerator2D::initializeTemperatureParameters();

    for (int iStation; iStation<nrStations; iStation++)
    {
        double averageTMaxDry[365];
        double averageTMaxWet[365];
        double stdDevTMaxDry[365];
        double stdDevTMaxWet[365];
        double averageTMinDry[365];
        double averageTMinWet[365];
        double stdDevTMinDry[365];
        double stdDevTMinWet[365];

        for (int iDay=1;iDay<365;iDay++)
        {
            int countTMaxDry = 0;
            int countTMaxWet = 0;
            int countTMinDry = 0;
            int countTMinWet = 0;

            averageTMaxDry[iDay]=0;
            averageTMaxWet[iDay]=0;
            stdDevTMaxDry[iDay]=0;
            stdDevTMaxWet[iDay]=0;
            averageTMinDry[iDay]=0;
            averageTMinWet[iDay]=0;
            stdDevTMinDry[iDay]=0;
            stdDevTMinWet[iDay]=0;

            for (int iDatum=0; iDatum<nrData; iDatum++)
            {
                //if (fabs(obsDataD[iStation][iDatum].prec-NODATA)> EPSILON && getDoyFromDate(&(obsDataD[iStation][iDatum].date))== iDay)
                if (fabs(obsDataD[iStation][iDatum].prec-NODATA)> EPSILON)
                {
                    if(obsDataD[iStation][iDatum].prec > parametersModel.precipitationThreshold)
                    {
                        if ((fabs(obsDataD[iStation][iDatum].tMax)-NODATA)> EPSILON)
                        {
                            countTMaxWet++;
                            averageTMaxWet[iDay] += obsDataD[iStation][iDatum].tMax;
                        }
                        if ((fabs(obsDataD[iStation][iDatum].tMin)-NODATA)> EPSILON)
                        {
                            countTMinWet++;
                            averageTMinWet[iDay] += obsDataD[iStation][iDatum].tMin;
                        }
                    }
                    else
                    {
                        if ((fabs(obsDataD[iStation][iDatum].tMax)-NODATA)> EPSILON)
                        {
                            countTMaxDry++;
                            averageTMaxDry[iDay] += obsDataD[iStation][iDatum].tMax;
                        }
                        if ((fabs(obsDataD[iStation][iDatum].tMin)-NODATA)> EPSILON)
                        {
                            countTMinDry++;
                            averageTMinDry[iDay] += obsDataD[iStation][iDatum].tMin;
                        }
                    }
                }
            }
            if (countTMaxDry != 0) averageTMaxDry[iDay] /= countTMaxDry;
            else averageTMaxDry[iDay] = NODATA;
            if (countTMaxWet != 0) averageTMaxWet[iDay] /= countTMaxWet;
            else averageTMaxWet[iDay] = NODATA;
            if (countTMinDry != 0) averageTMinDry[iDay] /= countTMinDry;
            else averageTMinDry[iDay] = NODATA;
            if (countTMinDry != 0) averageTMinWet[iDay] /= countTMinWet;
            else averageTMinWet[iDay] = NODATA;

            for (int iDatum=0; iDatum<nrData; iDatum++)
            {
                //if (fabs(obsDataD[iStation][iDatum].prec-NODATA)> EPSILON && getDoyFromDate(&(obsDataD[iStation][iDatum].date))== iDay)
                if (fabs(obsDataD[iStation][iDatum].prec-NODATA)> EPSILON)
                {
                    if(obsDataD[iStation][iDatum].prec > parametersModel.precipitationThreshold)
                    {
                        if ((fabs(obsDataD[iStation][iDatum].tMax)-NODATA)> EPSILON)
                        {
                            //countTMaxWet++;
                            stdDevTMaxWet[iDay] += (obsDataD[iStation][iDatum].tMax - averageTMaxWet[iDay])*(obsDataD[iStation][iDatum].tMax - averageTMaxWet[iDay]);
                        }
                        if ((fabs(obsDataD[iStation][iDatum].tMin)-NODATA)> EPSILON)
                        {
                            //countTMinWet++;
                            stdDevTMinWet[iDay] += (obsDataD[iStation][iDatum].tMin - averageTMinWet[iDay])*(obsDataD[iStation][iDatum].tMin - averageTMinWet[iDay]);
                        }
                    }
                    else
                    {
                        if ((fabs(obsDataD[iStation][iDatum].tMax)-NODATA)> EPSILON)
                        {
                            //countTMaxDry++;
                            stdDevTMaxDry[iDay] += (obsDataD[iStation][iDatum].tMax - averageTMaxDry[iDay])*(obsDataD[iStation][iDatum].tMax - averageTMaxDry[iDay]);
                        }
                        if ((fabs(obsDataD[iStation][iDatum].tMin)-NODATA)> EPSILON)
                        {
                            //countTMinDry++;
                            stdDevTMinDry[iDay] += (obsDataD[iStation][iDatum].tMin - averageTMinDry[iDay])*(obsDataD[iStation][iDatum].tMin - averageTMinDry[iDay]);
                        }
                    }
                }
            }
            if (countTMaxDry != 0) stdDevTMaxDry[iDay] /= countTMaxDry;
            else stdDevTMaxDry[iDay] = NODATA;
            if (countTMaxWet != 0) stdDevTMaxWet[iDay] /= countTMaxWet;
            else stdDevTMaxWet[iDay] = NODATA;
            if (countTMinDry != 0) stdDevTMinDry[iDay] /= countTMinDry;
            else stdDevTMinDry[iDay] = NODATA;
            if (countTMinDry != 0) stdDevTMinWet[iDay] /= countTMinWet;
            else stdDevTMinWet[iDay] = NODATA;

            if (countTMaxDry != 0) stdDevTMaxDry[iDay] = sqrt(stdDevTMaxDry[iDay]);
            if (countTMaxWet != 0) stdDevTMaxWet[iDay] = sqrt(stdDevTMaxWet[iDay]);
            if (countTMinDry != 0) stdDevTMinDry[iDay] = sqrt(stdDevTMinDry[iDay]);
            if (countTMinDry != 0) stdDevTMinWet[iDay] = sqrt(stdDevTMinWet[iDay]);


        } // end of iDay "for" cycle



    } // end of iStation "for" cycle
}

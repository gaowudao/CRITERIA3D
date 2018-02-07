

//  The spatial weather generator model is translated from the MulGets model available online on:
//  https://it.mathworks.com/matlabcentral/fileexchange/47537-multi-site-stochstic-weather-generator--mulgets-

/*
 *************************************************************************
 Multisite weather generator of the Ecole de technologie superieure
                               (MulGETS)
 *************************************************************************

 MulGETS is Matlab-based multisite weather generator for generating
 spatially correlated daily precipitation, maximum and minimum
 temperatures (Tmax and Tmin) series. The algorithm originates from the
 Wilks approach proposed in 1998. In 2007, Francois Brissette et al.
 presented an algorithm for efficient generation of multisite
 precipitation data following the Wilks approach. Afterward, A minor tune
 was conducted in 2013 after a further evaluation over various climates. A
 component for generating multisite temperature was also added.

 The Matlab code was programmed by Dr. Jie Chen, followed a work version
 provided by Prof. Francois Brissette, at the Ecole de technologie
 superieure, University of Quebec
 Contact email: jie.chen@etsmtl.ca (Jie Chen)

 ****************************
 Input data
 ****************************
 1. The input data consists of daily precipitation, Tmax and Tmin for
 multisite, meteorological data shall be separated by stations with a matlab
 structure named "observation", the data of each station shall be provided
 with the order of year, month, day, Tmax, Tmin and precipitation
 Missing data should be assigned as NaN.

 2. nome_exp: nome esperimento

 varargin	: optional parameters
        'threshold'	    -	soglia per definire un giorno con precipitazione (1 by default)
        'years_sim'  	-	anni simulati (30 by default)
        'dist'			-   tipo di distribuzione 1: Multi-exponential or 2: Multi-gamma (2 by default)
        'DIR'	        -	directory dove lavoro ('mac' by default)
       'soft'          -   software che uso %1=OCTAVE; 2=MATLAB; (2 by default)

 ****************************
 Output data
 ****************************
 The output consists of daily precipitation, Tmax and Tmin, the generated
 meteorological time series is separated by stations with a matlab structure
 named "generation", the order of each station is year, month, day, Tmax,
 Tmin and precipitation

 ****************************
 Example
 ****************************
generation=MulGETS(observation,varargin)

 ****************************
 Generation procedure
 ****************************
 the generation of precipitation includes five steps
 step 1: determination of weather generator parameters: p00, p10 and
         precip distribution function on a monthly scale
 step 2: computation of correlation matrices of precipatation occurrence
         and amounts
 step 3: generate spatially correlated precipitation occurrence
 step 4: establish link between occurrence index and average
         precip amounts for each station and construct the multi-exponential
         or multi-gamma distribution for each station
 step 5: generate precipitation amounts based on the occurrence index of
         generated occurrence

 the generation of temperature includes four steps
 step 1: calculate weather generator parameters for Tmax and Tmin
 step 2: calculate the spatial correlation for observed Tmax and Tmin
 step 3: generate spatial correlated random number
 step 4: generate spatial correlated Tmax and Tmin

 References:
 (1) Wilks, D. S., 1998. Multisite generalization of a daily stochastic
 precipitation generation model. J. Hydrol. 210, 178-191.
 (2) Brissette, F.P., Khalili, M., Leconte, R., 2007. Efficient stochastic
 generation of multi-site sythetic precipitation data. J. Hydrol. 345,
 121-133.
 (3) Chen, J., Brissette, F. P.,Leconte, R., Caron, A., 2012. A versatile
 weather generator for daily precipitation and temperature. Transactions
 of the ASABE. 55(3): 895-906.
 (4) Chen, J., Brissette, F. P.,Zhang, X.-C., 2014. A multi-site stochastic
 weather generator for daily precipitation and temperature. Transaction of
 the ASABE (Accepted)

MULGETS C/C++ version provided by Arpae-SIMC
contributors:
1) Dr. Antonio Volta avolta@arpae.it

*/


#include <stdio.h>
#include <math.h>
#include <malloc.h>

#include "wg2D.h"
#include "commonConstants.h"

//precipitation prec;
//temperature temp;

bool weatherGenerator2D::initializeObservedData(int lengthDataSeries, int stations)

{
    nrData = lengthDataSeries;
    nrStations = stations;

    // use of PRAGA formats from meteoPoint.h
    obsDataD = (TObsDataD **)calloc(nrStations, sizeof(TObsDataD*));
    for (int i=0;i<nrStations;i++)
    {
        obsDataD[i] = (TObsDataD *)calloc(nrData, sizeof(TObsDataD));
    }

    /*
    weatherObservations = (double ***)calloc(nrStations, sizeof(double**));
    for (int i=0;i<nrStations;i++)
    {
        weatherObservations[i] = (double **)calloc(lengthDataSeries, sizeof(double*));
        for (int j=0;j<lengthDataSeries;j++)
        {
            weatherObservations[i][j] = (double *)calloc(nrVariables, sizeof(double));
        }
    }
    for(int i=0;i<nrStations;i++)
    {
        for (int j=0;j<lengthDataSeries;j++)
        {
            for (int k=0;k<nrVariables;k++)
            {
               weatherObservations[i][j][k]= weatherArray[i][j][k];
            }
        }
     }
    dateObservations = (int **)calloc(lengthDataSeries, sizeof(int*));
    for (int j=0;j<lengthDataSeries;j++)
    {
        dateObservations[j] = (int *)calloc(nrDate, sizeof(int));
    }

    for (int j=0;j<lengthDataSeries;j++)
    {
        for (int k=0;k<nrDate;k++)
        {
           dateObservations[j][k]= dateArray[j][k];
        }
    }
    */
    return 0;
}

void weatherGenerator2D::initializeParameters(double thresholdPrecipitation, int simulatedYears, int distributionType, bool computePrecWG2D, bool computeTempWG2D)
{
    isPrecWG2D = computePrecWG2D;
    isTempWG2D = computeTempWG2D;
    // default parameters
    if (thresholdPrecipitation == NODATA) parametersModel.precipitationThreshold = 1.; //1mm default
    else parametersModel.precipitationThreshold = thresholdPrecipitation;
    if (simulatedYears == NODATA) parametersModel.yearOfSimulation = 30;
    else parametersModel.yearOfSimulation = simulatedYears;
    if (distributionType == NODATA) parametersModel.distributionPrecipitation = 2; //Select a distribution to generate daily precipitation amount,1: Multi-exponential or 2: Multi-gamma
    else parametersModel.distributionPrecipitation = distributionType;
}

void weatherGenerator2D::setObservedData(float*** weatherArray, int** dateArray)
{
    for(int i=0;i<nrStations;i++)
    {
        for (int j=0;j<nrData;j++)
        {

            obsDataD[i][j].date.day = dateArray[j][0];
            obsDataD[i][j].date.month = dateArray[j][1];
            obsDataD[i][j].date.year = dateArray[j][2];
            obsDataD[i][j].tMin = weatherArray[i][j][0];
            obsDataD[i][j].tMax = weatherArray[i][j][1];
            obsDataD[i][j].prec = weatherArray[i][j][2];
        }

    }
    for(int i=0;i<nrStations;i++)
    {
        for (int j=0;j<nrData;j++)
        {

            //printf("%d,%d,%d,%f,%f,%f\n",obsDataD[i][j].date.day,obsDataD[i][j].date.month,obsDataD[i][j].date.year,obsDataD[i][j].tMin,obsDataD[i][j].tMax,obsDataD[i][j].prec);

        }
    }

    month = (int *)calloc(12, sizeof(int));
    lengthMonth = (int *)calloc(12, sizeof(int));
    beginMonth = (int *)calloc(12, sizeof(int));

    int monthNumber = 0 ;
    lengthMonth[monthNumber] = 31;
    month[monthNumber] = monthNumber + 1;
    beginMonth[monthNumber] = 1;
    monthNumber++;
    month[monthNumber] = monthNumber + 1;
    beginMonth[monthNumber] = beginMonth[monthNumber-1] + lengthMonth[monthNumber - 1];
    lengthMonth[monthNumber] = 28;
    monthNumber++;
    month[monthNumber] = monthNumber + 1;
    beginMonth[monthNumber] = beginMonth[monthNumber-1] + lengthMonth[monthNumber - 1];
    lengthMonth[monthNumber] = 31;
    monthNumber++;
    month[monthNumber] = monthNumber + 1;
    beginMonth[monthNumber] = beginMonth[monthNumber-1] + lengthMonth[monthNumber - 1];
    lengthMonth[monthNumber] = 30;
    monthNumber++;
    month[monthNumber] = monthNumber + 1;
    beginMonth[monthNumber] = beginMonth[monthNumber-1] + lengthMonth[monthNumber - 1];
    lengthMonth[monthNumber] = 31;
    monthNumber++;
    month[monthNumber] = monthNumber + 1;
    beginMonth[monthNumber] = beginMonth[monthNumber-1] +  lengthMonth[monthNumber - 1];
    lengthMonth[monthNumber] = 30;
    monthNumber++;
    month[monthNumber] = monthNumber + 1;
    beginMonth[monthNumber] = beginMonth[monthNumber-1] +  lengthMonth[monthNumber - 1];
    lengthMonth[monthNumber] = 31;
    monthNumber++;
    month[monthNumber] = monthNumber + 1;
    beginMonth[monthNumber] = beginMonth[monthNumber-1] +  lengthMonth[monthNumber - 1];
    lengthMonth[monthNumber] = 31;
    monthNumber++;
    month[monthNumber] = monthNumber + 1;
    beginMonth[monthNumber] = beginMonth[monthNumber-1] +  lengthMonth[monthNumber - 1];
    lengthMonth[monthNumber] = 30;
    monthNumber++;
    month[monthNumber] = monthNumber + 1;
    beginMonth[monthNumber] = beginMonth[monthNumber-1] +  lengthMonth[monthNumber - 1];
    lengthMonth[monthNumber] = 31;
    monthNumber++;
    month[monthNumber] = monthNumber + 1;
    beginMonth[monthNumber] = beginMonth[monthNumber-1] +  lengthMonth[monthNumber - 1];
    lengthMonth[monthNumber] = 30;
    monthNumber++;
    month[monthNumber] = monthNumber + 1;
    beginMonth[monthNumber] = beginMonth[monthNumber-1] +  lengthMonth[monthNumber - 1];
    lengthMonth[monthNumber] = 31;

}


void weatherGenerator2D::computeWeatherGenerator2D()
{
    if (isPrecWG2D) weatherGenerator2D::precipitationCompute();
    if (isTempWG2D) weatherGenerator2D::temperatureCompute();
}

void weatherGenerator2D::precipitationCompute()
{
   weatherGenerator2D::precipitationOccurrence();
}

void weatherGenerator2D::precipitationOccurrence()
{
    for (int j=0; j<nrStations;j++)
    {
       /*
       double* precipitationAmountsD;
       bool* precipitationOccurrencesD;
       weatherGenerator2D::precipitation29February(j);
       precipitationAmountsD = (double*)calloc(nrDataWithout29February, sizeof(double));
       precipitationOccurrencesD = (bool*)calloc(nrDataWithout29February, sizeof(bool));
       weatherGenerator2D::precipitationAmountsOccurences(j,precipitationAmountsD,precipitationOccurrencesD);


       free(precipitationAmountsD);
       free(precipitationOccurrencesD);
       */
        weatherGenerator2D::precipitationP00P10(j);
    }
}
void weatherGenerator2D::precipitationP00P10(int idStation)
{
    int daysWithoutRain[12]={0,0,0,0,0,0,0,0,0,0,0,0};
    int daysWithRain[12]={0,0,0,0,0,0,0,0,0,0,0,0};
    int occurrence00[12]={0,0,0,0,0,0,0,0,0,0,0,0};
    int occurrence10[12]={0,0,0,0,0,0,0,0,0,0,0,0};
    double p00[12]={0,0,0,0,0,0,0,0,0,0,0,0};
    double p10[12]={0,0,0,0,0,0,0,0,0,0,0,0};

    for(int i=0;i<nrData-1;i++)
    {
        if ((obsDataD[idStation][i].prec != NODATA && obsDataD[idStation][i+1].prec != NODATA))
        {
            for (int month=1;month<13;month++)
            {
                if(obsDataD[idStation][i].date.month == month)
                {
                    if (obsDataD[idStation][i].prec > parametersModel.precipitationThreshold)
                    {
                        daysWithRain[month-1]++;
                        if (obsDataD[idStation][i+1].prec < parametersModel.precipitationThreshold)
                            occurrence10[month-1]++;
                    }
                    else
                    {
                        daysWithoutRain[month-1]++;
                        if (obsDataD[idStation][i+1].prec < parametersModel.precipitationThreshold)
                            occurrence00[month-1]++;
                    }
                }
            }
        }
    }
    for (int month=0;month<12;month++)
    {
        p00[month]=(1.0*occurrence00[month])/daysWithoutRain[month];
        p10[month]=(1.0*occurrence10[month])/daysWithRain[month];
        //printf("no pioggia %d,%d,%d\n",month+1,occurrence00[month],daysWithoutRain[month]);
        //printf("pioggia %d,%d,%d\n",month+1,occurrence10[month],daysWithRain[month]);
        //printf("%d,p10:%f,p00:%f\n",month+1,p10[month],p00[month]);
    }

}

void weatherGenerator2D::precipitation29February(int idStation)
{
    nrDataWithout29February = nrData;

    for (int i=0; i<nrData;i++)
    {
        if (isLeapYear(obsDataD[idStation][i].date.year))
        {
            if ((obsDataD[idStation][i].date.day == 29) && (obsDataD[idStation][i].date.month == 2))
            {
                if (i!= 0)obsDataD[idStation][i-1].prec += obsDataD[idStation][i-1].prec /2;
                if (i != (nrData-1)) obsDataD[idStation][i+1].prec += obsDataD[idStation][i+1].prec /2;
                nrDataWithout29February--;
            }
        }

    }

}

void weatherGenerator2D::precipitationAmountsOccurences(int idStation, double* precipitationAmountsD,bool* precipitationOccurrencesD)
{
    int counter = 0;
    for (int i=0; i<nrData;i++)
    {
            if (!((obsDataD[idStation][i].date.day == 29) && (obsDataD[idStation][i].date.month == 2)))
            {

                if (precipitationAmountsD[counter] < parametersModel.precipitationThreshold)
                {
                    precipitationOccurrencesD[counter]= false;
                    precipitationAmountsD[counter]=0;
                }
                else
                {
                    precipitationOccurrencesD[counter] = true;
                    precipitationAmountsD[counter]=obsDataD[idStation][i].prec;
                }
                counter++;
            }
    }
}


void weatherGenerator2D::temperatureCompute()
{

}

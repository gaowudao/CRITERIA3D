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

precipitation prec;
temperature temp;

bool weatherGenerator2D::initializeObservedData(int lengthDataSeries, int nrStations, int nrVariables,int nrDate,double*** weatherArray, int** dateArray)

{
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

    return 0;
}

void weatherGenerator2D::initializeParameters(double thresholdPrecipitation, int simulatedYears, int distributionType)
{
    // default parameters
    if (thresholdPrecipitation == NODATA) parametersModel.precipitationThreshold = 1.; //1mm default
    else parametersModel.precipitationThreshold = thresholdPrecipitation;
    if (simulatedYears == NODATA) parametersModel.yearOfSimulation = 30;
    else parametersModel.yearOfSimulation = simulatedYears;
    if (distributionType == NODATA) parametersModel.distributionPrecipitation = 2; //Select a distribution to generate daily precipitation amount,1: Multi-exponential or 2: Multi-gamma
    else parametersModel.distributionPrecipitation = distributionType;
}

void precipitation::compute()
{

}

void temperature::compute()
{

}



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


int randomPseudo(int next)
{
    next = next*1103515245 + 12345;
    //return (unsigned int)(next/65536) % 32768;
    return (int) (sqrt(((next/65536) % 32768)*((next/65536) % 32768)));
}


bool weatherGenerator2D::initializeData(int lengthDataSeries, int stations)

{
    nrData = lengthDataSeries;
    nrStations = stations;

    // use of PRAGA formats from meteoPoint.h
    obsDataD = (TObsDataD **)calloc(nrStations, sizeof(TObsDataD*));

    for (int i=0;i<nrStations;i++)
    {
        obsDataD[i] = (TObsDataD *)calloc(nrData, sizeof(TObsDataD));
    }
    // occurrence structure
    precOccurence = (TprecOccurrence **) calloc(nrStations, sizeof(TprecOccurrence*));

    for (int i=0;i<nrStations;i++)
    {
        precOccurence[i] = (TprecOccurrence *)calloc(12, sizeof(TprecOccurrence));
    }
    // correlation matrix structure
    correlationMatrix = (TcorrelationMatrix*)calloc(12, sizeof(TcorrelationMatrix));
    for (int iMonth=0;iMonth<12;iMonth++)
    {
        correlationMatrix[iMonth].amount = (double**)calloc(nrStations, sizeof(double*));
        correlationMatrix[iMonth].occurrence = (double**)calloc(nrStations, sizeof(double*));
        for (int i=0;i<nrStations;i++)
        {
            correlationMatrix[iMonth].amount[i]= (double*)calloc(nrStations, sizeof(double));
            correlationMatrix[iMonth].occurrence[i]= (double*)calloc(nrStations, sizeof(double));
        }
    }

    obsPrecDataD = (TObsPrecDataD **)calloc(nrStations, sizeof(TObsPrecDataD*));
    for (int i=0;i<nrStations;i++)
    {
        obsPrecDataD[i] = (TObsPrecDataD *)calloc(nrData, sizeof(TObsPrecDataD));
    }
    for (int i=0;i<nrStations;i++)
    {
        for (int j=0;j<nrData;j++)
        {
            obsPrecDataD[i][j].amounts = NODATA;
            obsPrecDataD[i][j].amountsLessThreshold = NODATA;
            obsPrecDataD[i][j].date.day = NODATA;
            obsPrecDataD[i][j].date.month = NODATA;
            obsPrecDataD[i][j].date.year = NODATA;
            obsPrecDataD[i][j].occurrences = NODATA;
            obsPrecDataD[i][j].prec = NODATA;
        }

    }

    return 0;
}

void weatherGenerator2D::initializeParameters(float thresholdPrecipitation, int simulatedYears, int distributionType, bool computePrecWG2D, bool computeTempWG2D)
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

void weatherGenerator2D::setObservedData(TObsDataD** observations)
{
    for(int i=0;i<nrStations;i++)
    {
        for (int j=0;j<nrData;j++)
        {
            obsDataD[i][j].date.day = observations[i][j].date.day ;
            obsDataD[i][j].date.month = observations[i][j].date.month;
            obsDataD[i][j].date.year = observations[i][j].date.year;
            obsDataD[i][j].tMin = observations[i][j].tMin;
            obsDataD[i][j].tMax = observations[i][j].tMax;
            obsDataD[i][j].prec = observations[i][j].prec;
            //printf("%d/%d/%d,%.1f,%.1f,%.1f\n",observations[i][j].date.day,observations[i][j].date.month,observations[i][j].date.year,observations[i][j].tMin,observations[i][j].tMax,observations[i][j].prec);

        }
        //pressEnterToContinue();
    }
}


void weatherGenerator2D::computeWeatherGenerator2D()
{
    if (isPrecWG2D) weatherGenerator2D::precipitationCompute();
    if (isTempWG2D) weatherGenerator2D::temperatureCompute();
}

void weatherGenerator2D::precipitationCompute()
{
   // step 1 of precipitation WG2D
   weatherGenerator2D::initializePrecipitationInternalArrays();
   weatherGenerator2D::initializePrecipitationOutputs(lengthSeason);
   weatherGenerator2D::precipitationP00P10(); // it computes the monthly probabilities p00 and p10
   printf("fase 1\n");
   /*int month = 0; // gennaio
   precOccurence[0][month].p00 = 0.8254;
   precOccurence[0][month].p10 = 0.4444;
   precOccurence[1][month].p00 = 0.8413;
   precOccurence[1][month].p10 = 0.4074;
   precOccurence[2][month].p00 = 0.8413;
   precOccurence[2][month].p10 = 0.4074;
   month++; // febbraio
   precOccurence[0][month].p00 = 0.66;
   precOccurence[0][month].p10 = 0.4839;
   precOccurence[1][month].p00 = 0.6981;
   precOccurence[1][month].p10 = 0.5;
   precOccurence[2][month].p00 = 0.6939;
   precOccurence[2][month].p10 = 0.4063;
   month++; // marzo
   precOccurence[0][month].p00 = 0.7273;
   precOccurence[0][month].p10 = 0.4286;
   precOccurence[1][month].p00 = 0.75;
   precOccurence[1][month].p10 = 0.4118;
   precOccurence[2][month].p00 = 0.8033;
   precOccurence[2][month].p10 = 0.4138;
   month++; // aprile
   precOccurence[0][month].p00 = 0.8806;
   precOccurence[0][month].p10 = 0.4;
   precOccurence[1][month].p00 = 0.8125;
   precOccurence[1][month].p10 = 0.5652;
   precOccurence[2][month].p00 = 0.8525;
   precOccurence[2][month].p10 = 0.4231;
   month++; // maggio
   precOccurence[0][month].p00 = 0.8438;
   precOccurence[0][month].p10 = 0.4615;
   precOccurence[1][month].p00 = 0.8382;
   precOccurence[1][month].p10 = 0.5455;
   precOccurence[2][month].p00 = 0.8382;
   precOccurence[2][month].p10 = 0.5455;
   month++; //giugno
   precOccurence[0][month].p00 = 0.7541;
   precOccurence[0][month].p10 = 0.5769;
   precOccurence[1][month].p00 = 0.8358;
   precOccurence[1][month].p10 = 0.55;
   precOccurence[2][month].p00 = 0.8406;
   precOccurence[2][month].p10 = 0.6111;
   month++; // luglio
   precOccurence[0][month].p00 = 0.8533;
   precOccurence[0][month].p10 = 0.7333;
   precOccurence[1][month].p00 = 0.9;
   precOccurence[1][month].p10 = 0.8;
   precOccurence[2][month].p00 = 0.8684;
   precOccurence[2][month].p10 = 0.7143;
   month++; // agosto
   precOccurence[0][month].p00 = 0.8182;
   precOccurence[0][month].p10 = 1.;
   precOccurence[1][month].p00 = 0.7778;
   precOccurence[1][month].p10 = 0.8333;
   precOccurence[2][month].p00 = 0.8734;
   precOccurence[2][month].p10 = 0.9091;
   month++; // settembre
   precOccurence[0][month].p00 = 0.8493;
   precOccurence[0][month].p10 = 0.7143;
   precOccurence[1][month].p00 = 0.8611;
   precOccurence[1][month].p10 = 0.6000;
   precOccurence[2][month].p00 = 0.8784;
   precOccurence[2][month].p10 = 0.6154;
   month++; // ottobre
   precOccurence[0][month].p00 = 0.8356;
   precOccurence[0][month].p10 = 0.5882;
   precOccurence[1][month].p00 = 0.7941;
   precOccurence[1][month].p10 = 0.5455;
   precOccurence[2][month].p00 = 0.7941;
   precOccurence[2][month].p10 = 0.5455;
   month++; // novembre
   precOccurence[0][month].p00 = 0.8571;
   precOccurence[0][month].p10 = 0.4167;
   precOccurence[1][month].p00 = 0.8197;
   precOccurence[1][month].p10 = 0.4615;
   precOccurence[2][month].p00 = 0.8;
   precOccurence[2][month].p10 = 0.6364;
   month++; // dicembre
   precOccurence[0][month].p00 = 0.9375;
   precOccurence[0][month].p10 = 0.4;
   precOccurence[1][month].p00 = 0.9259;
   precOccurence[1][month].p10 = 0.5556;
   precOccurence[2][month].p00 = 0.939;
   precOccurence[2][month].p10 = 0.625;
   */



   // step 2 of precipitation WG2D
   weatherGenerator2D::precipitationCorrelationMatrices();
   printf("fase 2\n");
   // step 3 of precipitation WG2D
   weatherGenerator2D::precipitationMultisiteOccurrenceGeneration();
   printf("fase 3\n");
   // step 4 of precipitation WG2D
   //weatherGenerator2D::precipitationMultiDistributionParameterization();
   printf("fase 4\n");
   // step 5 of precipitation WG2D
   //weatherGenerator2D::precipitationMultisiteAmountsGeneration();
   printf("fase 5\n");
}


void weatherGenerator2D::precipitationP00P10()
{
    // initialization
    for (int idStation=0;idStation<nrStations;idStation++)
    {
        int daysWithoutRain[12]={0,0,0,0,0,0,0,0,0,0,0,0};
        int daysWithRain[12]={0,0,0,0,0,0,0,0,0,0,0,0};
        int occurrence00[12]={0,0,0,0,0,0,0,0,0,0,0,0};
        int occurrence10[12]={0,0,0,0,0,0,0,0,0,0,0,0};

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
                            if (obsDataD[idStation][i+1].prec <= parametersModel.precipitationThreshold)
                                occurrence10[month-1]++;
                        }
                        else
                        {
                            daysWithoutRain[month-1]++;
                            if (obsDataD[idStation][i+1].prec <= parametersModel.precipitationThreshold)
                                occurrence00[month-1]++;
                        }
                        /*if (month == 1)
                        {
                            printf("%d/%d/%d  ",obsDataD[idStation][i].date.day,obsDataD[idStation][i].date.month,obsDataD[idStation][i].date.year);
                            printf("month %d n10 %d n00 %d \n",month,occurrence10[month-1],occurrence00[month-1]);
                            pressEnterToContinue();
                        }*/
                    }
                }
            }
        }
        /*for (int month=0;month<12;month++)
        {
            printf("month %d n10 %d n00 %d \n",month,occurrence10[month],occurrence00[month]);
        }
        pressEnterToContinue();*/
        for (int month=0;month<12;month++)
        {
            if (daysWithoutRain[month] != 0)
                precOccurence[idStation][month].p00 = minValue(0.9999999,(double)((1.0*occurrence00[month])/daysWithoutRain[month]));
            else
                precOccurence[idStation][month].p00 = 0.0;
            if (daysWithRain[month] != 0)
                precOccurence[idStation][month].p10 = minValue(0.9999999,(double)((1.0*occurrence10[month])/daysWithRain[month]));
            else
                precOccurence[idStation][month].p10 = 0.0;

            precOccurence[idStation][month].month = month +1;
        }
    }
    /*pressEnterToContinue();
    for (int month=0;month<12;month++)
    {

           printf("%f  %f %f\n",precOccurence[0][month].p00,precOccurence[1][month].p00,precOccurence[2][month].p00);
           printf("%f  %f %f\n",precOccurence[0][month].p10,precOccurence[1][month].p10,precOccurence[2][month].p10);
           pressEnterToContinue();

    }
    pressEnterToContinue();*/
}



void weatherGenerator2D::precipitationCorrelationMatrices()
{
    int counter =0;
    TcorrelationVar amount,occurrence;
    for (int iMonth=0;iMonth<12;iMonth++)
    {
        correlationMatrix[iMonth].month = iMonth + 1 ; // define the month of the correlation matrix;
        for (int k=0; k<nrStations;k++) // correlation matrix diagonal elements;
        {
            correlationMatrix[iMonth].amount[k][k] = 1.;
            correlationMatrix[iMonth].occurrence[k][k]= 1.;
        }

        for (int j=0; j<nrStations-1;j++)
        {
            for (int i=j+1; i<nrStations;i++)
            {
                counter = 0;
                amount.meanValue1=0.;
                amount.meanValue2=0.;
                amount.covariance = amount.variance1 = amount.variance2 = 0.;
                occurrence.meanValue1=0.;
                occurrence.meanValue2=0.;
                occurrence.covariance = occurrence.variance1 = occurrence.variance2 = 0.;

                for (int k=0; k<nrData;k++) // compute the monthly means
                {
                    if (obsDataD[j][k].date.month == (iMonth+1) && obsDataD[i][k].date.month == (iMonth+1))
                    {
                        if (((obsDataD[j][k].prec - NODATA) > EPSILON) && ((obsDataD[i][k].prec - NODATA) > EPSILON))
                        {
                            counter++;
                            if (obsDataD[j][k].prec > parametersModel.precipitationThreshold)
                            {
                                amount.meanValue1 += obsDataD[j][k].prec ;
                                occurrence.meanValue1++ ;
                            }
                            if (obsDataD[i][k].prec > parametersModel.precipitationThreshold)
                            {
                                amount.meanValue2 += obsDataD[i][k].prec;
                                occurrence.meanValue2++ ;
                            }
                        }
                    }
                }
                if (counter != 0)
                {
                    amount.meanValue1 /= counter;
                    occurrence.meanValue1 /= counter;
                }

                if (counter != 0)
                {
                    amount.meanValue2 /= counter;
                    occurrence.meanValue2 /= counter;
                }
                // compute the monthly rho off-diagonal elements
                for (int k=0; k<nrData;k++)
                {
                    if (obsDataD[j][k].date.month == (iMonth+1) && obsDataD[i][k].date.month == (iMonth+1))
                    {
                        if ((obsDataD[j][k].prec != NODATA) && (obsDataD[i][k].prec != NODATA))
                        {
                            double value1,value2;
                            if (obsDataD[j][k].prec <= parametersModel.precipitationThreshold) value1 = 0.;
                            else value1 = obsDataD[j][k].prec;
                            if (obsDataD[i][k].prec <= parametersModel.precipitationThreshold) value2 = 0.;
                            else value2 = obsDataD[i][k].prec;

                            amount.covariance += (value1 - amount.meanValue1)*(value2 - amount.meanValue2);
                            amount.variance1 += (value1 - amount.meanValue1)*(value1 - amount.meanValue1);
                            amount.variance2 += (value2 - amount.meanValue2)*(value2 - amount.meanValue2);

                            if (obsDataD[j][k].prec <= parametersModel.precipitationThreshold) value1 = 0.;
                            else value1 = 1.;
                            if (obsDataD[i][k].prec <= parametersModel.precipitationThreshold) value2 = 0.;
                            else value2 = 1.;

                            occurrence.covariance += (value1 - occurrence.meanValue1)*(value2 - occurrence.meanValue2);
                            occurrence.variance1 += (value1 - occurrence.meanValue1)*(value1 - occurrence.meanValue1);
                            occurrence.variance2 += (value2 - occurrence.meanValue2)*(value2 - occurrence.meanValue2);
                        }
                    }
                }
                correlationMatrix[iMonth].amount[j][i]= amount.covariance / sqrt(amount.variance1*amount.variance2);
                correlationMatrix[iMonth].amount[i][j] = correlationMatrix[iMonth].amount[j][i];
                correlationMatrix[iMonth].occurrence[j][i]= occurrence.covariance / sqrt(occurrence.variance1*occurrence.variance2);
                correlationMatrix[iMonth].occurrence[i][j] = correlationMatrix[iMonth].occurrence[j][i];
            }
        }

    }
}

void weatherGenerator2D::precipitationMultisiteOccurrenceGeneration()
{
    int nrDaysIterativeProcessMonthly[12];
    int gasDevIset = 0;
    double gasDevGset = 0;
    srand (time(NULL));


    for (int i=0;i<12;i++)
    {
        nrDaysIterativeProcessMonthly[i] = lengthMonth[i]*parametersModel.yearOfSimulation;
    }
    double** matrixOccurrence;
    matrixOccurrence = (double **)calloc(nrStations, sizeof(double*));
    double** normalizedTransitionProbability;
    normalizedTransitionProbability = (double **)calloc(nrStations, sizeof(double*));

    for (int i=0;i<nrStations;i++)
    {
        matrixOccurrence[i] = (double *)calloc(nrStations, sizeof(double));
        normalizedTransitionProbability[i]= (double *)calloc(2, sizeof(double));
        for (int j=0;j<nrStations;j++)
        {
           matrixOccurrence[i][j]= NODATA;
        }
        normalizedTransitionProbability[i][0]= NODATA;
        normalizedTransitionProbability[i][1]= NODATA;        
    }
    // random Occurrence structure. Used from step 3 on

        randomMatrix = (TrandomMatrix*)calloc(12,sizeof(TrandomMatrix));
        for (int iMonth=0;iMonth<12;iMonth++)
        {
            randomMatrix[iMonth].matrixK = (double**)calloc(nrStations, sizeof(double*));
            randomMatrix[iMonth].matrixM = (double**)calloc(nrStations, sizeof(double*));
            randomMatrix[iMonth].matrixOccurrences = (double**)calloc(nrStations, sizeof(double*));

            for (int i=0;i<nrStations;i++)
            {
                randomMatrix[iMonth].matrixK[i]= (double*)calloc(nrStations, sizeof(double));
                randomMatrix[iMonth].matrixM[i]= (double*)calloc(nrStations, sizeof(double));
                randomMatrix[iMonth].matrixOccurrences[i]= (double*)calloc(nrDaysIterativeProcessMonthly[iMonth], sizeof(double));
            }
        }

    // arrays initialization
    //int randomGeneration = 1;
    for (int iMonth=0; iMonth<12; iMonth++)
    {
        // initialization and definition of the random matrix
        double** normalizedRandomMatrix;
        normalizedRandomMatrix = (double **)calloc(nrStations, sizeof(double*));
        for (int i=0;i<nrStations;i++)
        {
            normalizedRandomMatrix[i] = (double *)calloc(nrDaysIterativeProcessMonthly[iMonth], sizeof(double));
            for (int j=0;j<nrDaysIterativeProcessMonthly[iMonth];j++)
            {
               normalizedRandomMatrix[i][j]= NODATA;
            }
        }
        //printf("mese %d\n", iMonth+1);
        //double* arrayRandomNormal;
        //arrayRandomNormal = (double *)calloc(nrStations*nrDaysIterativeProcessMonthly[iMonth], sizeof(double));
        //randomSet(arrayRandomNormal,nrStations*nrDaysIterativeProcessMonthly[iMonth]);

        //int counterRandomNumber = 0;
        //double testValue;
        for (int i=0;i<nrStations;i++)
        {
            for (int j=0;j<nrStations;j++)
            {
                matrixOccurrence[i][j]= correlationMatrix[iMonth].occurrence[i][j]; //checked
            }

            /* since random numbers generated have a normal distribution, each p00 and
               p10 have to be recalculated according to a normal number*/
            normalizedTransitionProbability[i][0]= - (SQRT_2*(statistics::inverseERFC(2*precOccurence[i][iMonth].p00,0.0001)));
            normalizedTransitionProbability[i][1]= - (SQRT_2*(statistics::inverseERFC(2*precOccurence[i][iMonth].p10,0.0001)));
            //printf("%f\t",normalizedTransitionProbability[i][0]);
            //printf("%f\n",normalizedTransitionProbability[i][1]);
            // checked


            for (int jCount=0;jCount<nrDaysIterativeProcessMonthly[iMonth];jCount++)
            {

               normalizedRandomMatrix[i][jCount]= myrandom::normalRandom(&gasDevIset,&gasDevGset);
               //normalizedRandomMatrix[i][jCount]= arrayRandomNormal[counterRandomNumber];
               //counterRandomNumber++;

            }

        }
        //free(arrayRandomNormal);
        // initialization outputs of weatherGenerator2D::spatialIterationOccurrence
        double** M;
        double** K;
        double** occurrences;
        M = (double **)calloc(nrStations, sizeof(double*));
        K = (double **)calloc(nrStations, sizeof(double*));
        occurrences = (double **)calloc(nrStations, sizeof(double*));

        for (int i=0;i<nrStations;i++)
        {
            M[i] = (double *)calloc(nrStations, sizeof(double));
            K[i] = (double *)calloc(nrStations, sizeof(double));
            for (int j=0;j<nrStations;j++)
            {
                M[i][j]= NODATA;
                K[i][j]= NODATA;
            }
        }
        for (int i=0;i<nrStations;i++)
        {
          occurrences[i] = (double *)calloc(nrDaysIterativeProcessMonthly[iMonth], sizeof(double));
          for (int j=0;j<nrDaysIterativeProcessMonthly[iMonth];j++)
          {
              occurrences[i][j]= NODATA;
          }
        }

        weatherGenerator2D::spatialIterationOccurrence(M,K,occurrences,matrixOccurrence,normalizedRandomMatrix,normalizedTransitionProbability,nrDaysIterativeProcessMonthly[iMonth]);
        //printf("mese %d\n",iMonth+1);
        for (int i=0;i<nrStations;i++)
        {
            for (int j=0;j<nrStations;j++)
            {
                randomMatrix[iMonth].matrixK[i][j] = double(K[i][j]);
                randomMatrix[iMonth].matrixM[i][j] = double(M[i][j]);
                //printf("%f  ",randomMatrix[iMonth].matrixM[i][j]);
            }

            //printf("partenza");
            for (int j=0;j<nrDaysIterativeProcessMonthly[iMonth];j++)
            {
                randomMatrix[iMonth].matrixOccurrences[i][j]= double(occurrences[i][j]);
                printf("%f \n",randomMatrix[iMonth].matrixOccurrences[i][j]);
            }
            printf("\n");
        }
        //pressEnterToContinue();
        randomMatrix[iMonth].month = iMonth + 1;
        // free memory
        for (int i=0;i<nrStations;i++)
        {
            free(normalizedRandomMatrix[i]);
            free(M[i]);
            free(K[i]);
            free(occurrences[i]);

        }

        free(normalizedRandomMatrix);
        free(M);
        free(K);
        free(occurrences);

    }


    // free memory
    for (int i=0;i<nrStations;i++)
    {
        free(matrixOccurrence[i]);
        free(normalizedTransitionProbability[i]);
    }
    free(matrixOccurrence);
    free(normalizedTransitionProbability);


}

void weatherGenerator2D::spatialIterationOccurrence(double ** M, double** K,double** occurrences, double** matrixOccurrence, double** normalizedMatrixRandom,double ** transitionNormal,int lengthSeries)
{
    // M and K matrices are also used as ancillary dummy matrices
    double val=5;
    int ii=0;
    double kiter=0.1;   // iteration parameter in calculation of new estimate of matrix 'mat'
    double* eigenvalues =(double*)calloc(nrStations, sizeof(double));
    double* eigenvectors =(double*)calloc(nrStations*nrStations, sizeof(double));
    double* correlationArray =(double*)calloc(nrStations*nrStations, sizeof(double));
    double** dummyMatrix = (double**)calloc(nrStations, sizeof(double*));
    double** dummyMatrix2 = (double**)calloc(nrStations, sizeof(double*));
    double** dummyMatrix3 = (double**)calloc(nrStations, sizeof(double*));
    double** normRandom = (double**)calloc(nrStations, sizeof(double*));

    // initialization internal arrays
    for (int i=0;i<nrStations;i++)
    {
        dummyMatrix[i]= (double*)calloc(nrStations, sizeof(double));
        dummyMatrix2[i]= (double*)calloc(nrStations, sizeof(double));

    }
    for (int i=0;i<nrStations;i++)
    {
        dummyMatrix3[i]= (double*)calloc(lengthSeries, sizeof(double));
        normRandom[i]= (double*)calloc(lengthSeries, sizeof(double));
    }

    // initialization output M
    for (int i=0;i<nrStations;i++)
    {
       for (int j=0;j<nrStations;j++)
        {
            M[i][j] = matrixOccurrence[i][j];  // M is the matrix named mat in the original code
            //printf("%f\t",M[i][j]);
        }
       //printf("\n"); pressEnterToContinue();
    }
    //M[0][2]=M[2][0]=M[2][1];
    /*M[0][0]=1;
    M[0][1]=0.9478;
    M[0][2]=0.7391;
    M[1][0]=0.9478;
    M[2][0]=0.7391;
    M[1][1]=1;
    M[1][2]=0.7391;
    M[2][1]=0.7391;
    M[2][2]=1;
    for (int i=0;i<nrStations;i++)
    {
       for (int j=0;j<nrStations;j++)
        {
            matrixOccurrence[i][j] = M[i][j];  // M is the matrix named mat in the original code
            //printf("%f\t",M[i][j]);
        }
       //printf("\n");
    }

*/
    double minimalValueToExitFromCycle = NODATA;
    int counterConvergence=0;
    bool exitWhileCycle = false;
    while ((val>TOLERANCE_MULGETS) && (ii<MAX_ITERATION_MULGETS) && (!exitWhileCycle))
    //while ((val>TOLERANCE_MULGETS) && (ii<2))
    {
        ii++;
        int nrEigenvaluesLessThan0 = 0;
        int counter = 0;
        for (int i=0;i<nrStations;i++)
        {
            for (int j=0;j<nrStations;j++) // avoid solutions with correlation coefficient greater than 1
            {
                M[i][j] = minValue(M[i][j],1);
                correlationArray[counter] = M[i][j];
                counter++;
            }
        }
        //eigenproblem::Jacobi_Cyclic_Method(eigenvalues,eigenvectors,correlationArray,nrStations);
        eigenproblem::rs(nrStations,correlationArray,eigenvalues,true,eigenvectors);
        /*double sumEigenvector[9]={0,0,0,0,0,0,0,0,0};
        int countEigenvector=0;
        for (int i=0;i<nrStations;i++)
        {
            for (int j=0;j<nrStations;j++)
            {
                for (int k=0;k<nrStations;k++)
                {
                    sumEigenvector[countEigenvector]+= eigenvectors[3*i+k]*eigenvectors[3*j+k];
                }
                countEigenvector++;
            }
        }*/
        /*for (int i=0;i<nrStations*nrStations;i++)
        {
            //printf("sum of eigenVector %f\n",sumEigenvector[i]);
            //printf("eigenVectors %f\n",eigenvectors[i]);
        }*/
        for (int i=0;i<nrStations;i++)
        {
            //printf("%d eigenvalue %f \n",ii,eigenvalues[i]);
            if (eigenvalues[i] <= 0)
            {
                nrEigenvaluesLessThan0++;
                eigenvalues[i] = 0.000001;
            }
        }
        if (nrEigenvaluesLessThan0 > 0)
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
            matricial::matrixProduct(dummyMatrix,dummyMatrix2,nrStations,nrStations,nrStations,nrStations,M);
        }
        // the matrix called M is the final matrix exiting from the calculation
        for (int i=0;i<nrStations;i++)
            for (int j=0;j<nrStations;j++) dummyMatrix[i][j] = M[i][j];


        bool isLowerDiagonal = true;
        matricial::choleskyDecompositionTriangularMatrix(dummyMatrix,nrStations,isLowerDiagonal);
        /*printf("Cholesky \n");
        for (int i=0;i<nrStations;i++)
        {
            for (int j=0;j<nrStations;j++)
            {
                printf("%f\t", dummyMatrix[i][j]);
            }
            printf("\n");
        } pressEnterToContinue();*/

        matricial::matrixProduct(dummyMatrix,normalizedMatrixRandom,nrStations,nrStations,lengthSeries,nrStations,dummyMatrix3);

        for (int i=0;i<nrStations;i++)
        {
            // compute mean and standard deviation without NODATA check
            double meanValue,stdDevValue;
            meanValue = stdDevValue = 0;
            for (int j=0;j<lengthSeries;j++)
                meanValue += dummyMatrix3[i][j];
            meanValue /= lengthSeries;
            for (int j=0;j<lengthSeries;j++)
                stdDevValue += (dummyMatrix3[i][j]- meanValue)*(dummyMatrix3[i][j]- meanValue);
            stdDevValue /= (lengthSeries-1);
            stdDevValue = sqrt(stdDevValue);

            for (int j=0;j<lengthSeries;j++)
            {
                normRandom[i][j]= (dummyMatrix3[i][j]-meanValue)/stdDevValue;
            }
        }
        /*
        for (int j=0;j<lengthSeries;j++)
        {
            for (int i=0;i<nrStations;i++)
            {
                printf("%f\t", normRandom[i][j]);
            }
            printf("\n");
        } pressEnterToContinue();*/
        // initialize occurrence to 0
        for (int i=0;i<nrStations;i++)
        {
            for (int j=0;j<lengthSeries;j++)
                occurrences[i][j]= 0.;
        }

        for (int i=0;i<nrStations;i++)
        {
            for (int j=1;j<lengthSeries;j++)
            {
                if(fabs(occurrences[i][j-1]) < EPSILON)
                {
                    if(normRandom[i][j]> transitionNormal[i][0]) occurrences[i][j] = 1.;
                }
                else
                {
                    if(normRandom[i][j]> transitionNormal[i][1]) occurrences[i][j] = 1.;
                }
            }
        }
        statistics::correlationsMatrix(nrStations,occurrences,lengthSeries,K);
        val = 0;
        for (int i=0; i<nrStations;i++)
        {
            for (int j=0;j<nrStations;j++)
            {
                val = maxValue(val, fabs(K[i][j] - matrixOccurrence[i][j]));
            }

        }

        if (val < fabs(minimalValueToExitFromCycle))
        {
            minimalValueToExitFromCycle = val;
            counterConvergence = 0;
        }
        else
        {
            counterConvergence++;
        }
        if (counterConvergence > 20)
        {
            if (val <= fabs(minimalValueToExitFromCycle) + TOLERANCE_MULGETS) exitWhileCycle = true;
        }
        for (int i=0; i<nrStations;i++)
        {
            M[i][i]= 1.;
        }
        if ((ii != MAX_ITERATION_MULGETS) && (val > TOLERANCE_MULGETS)  && (!exitWhileCycle))
        //if ((ii != 2) && (val > TOLERANCE_MULGETS))
        {
            for (int i=0; i<nrStations;i++)
            {
                //M[i][i]= 1.;
                for (int j=i+1;j<nrStations;j++)
                {
                    M[i][j] += kiter*(matrixOccurrence[i][j]-K[i][j]);
                    M[j][i] = M[i][j];
                }                
            }
        }
        //printf("contatore per uscire %d contatore %d val %f",counterConvergence, ii, val); pressEnterToContinue();
        /*for (int i=0;i<nrStations;i++)
        {
            for (int j=0;j<nrStations;j++)
            {
                //printf("%f\t", K[i][j]);
            }
            //printf("\n");
        }
        //printf(" M \n");
        for (int i=0;i<nrStations;i++)
        {
            for (int j=0;j<nrStations;j++)
            {
                //printf("%f ", M[i][j]);
            }
           //printf("\n");
        } printf("iterazione %d\n",ii);pressEnterToContinue();
        */


    }  // end of the while cycle
        //pressEnterToContinue();


    // free memory

    for (int i=0;i<nrStations;i++)
    {
        free(dummyMatrix[i]);
        free(dummyMatrix2[i]);
        free(dummyMatrix3[i]);
        free(normRandom[i]);
    }
    free(normRandom);
    free(dummyMatrix);
    free(dummyMatrix2);
    free(dummyMatrix3);
    free(eigenvalues);
    free(eigenvectors);
    free(correlationArray);

}
 void weatherGenerator2D::initializePrecipitationInternalArrays()
 {

     month = (int *)calloc(12, sizeof(int));
     for (int i=0; i<12;i++) month[i] = NODATA;
     lengthMonth = (int *)calloc(12, sizeof(int));
     for (int i=0; i<12;i++) lengthMonth[i] = NODATA;
     //beginMonth = (int *)calloc(12, sizeof(int));
     int monthNumber = 0 ;
     lengthMonth[monthNumber] = 31;
     month[monthNumber] = monthNumber + 1;
     //beginMonth[monthNumber] = 1;
     monthNumber++;
     month[monthNumber] = monthNumber + 1;
     //beginMonth[monthNumber] = beginMonth[monthNumber-1] + lengthMonth[monthNumber - 1];
     lengthMonth[monthNumber] = 28;
     monthNumber++;
     month[monthNumber] = monthNumber + 1;
     //beginMonth[monthNumber] = beginMonth[monthNumber-1] + lengthMonth[monthNumber - 1];
     lengthMonth[monthNumber] = 31;
     monthNumber++;
     month[monthNumber] = monthNumber + 1;
     //beginMonth[monthNumber] = beginMonth[monthNumber-1] + lengthMonth[monthNumber - 1];
     lengthMonth[monthNumber] = 30;
     monthNumber++;
     month[monthNumber] = monthNumber + 1;
     //beginMonth[monthNumber] = beginMonth[monthNumber-1] + lengthMonth[monthNumber - 1];
     lengthMonth[monthNumber] = 31;
     monthNumber++;
     month[monthNumber] = monthNumber + 1;
     //beginMonth[monthNumber] = beginMonth[monthNumber-1] +  lengthMonth[monthNumber - 1];
     lengthMonth[monthNumber] = 30;
     monthNumber++;
     month[monthNumber] = monthNumber + 1;
     //beginMonth[monthNumber] = beginMonth[monthNumber-1] +  lengthMonth[monthNumber - 1];
     lengthMonth[monthNumber] = 31;
     monthNumber++;
     month[monthNumber] = monthNumber + 1;
     //beginMonth[monthNumber] = beginMonth[monthNumber-1] +  lengthMonth[monthNumber - 1];
     lengthMonth[monthNumber] = 31;
     monthNumber++;
     month[monthNumber] = monthNumber + 1;
     //beginMonth[monthNumber] = beginMonth[monthNumber-1] +  lengthMonth[monthNumber - 1];
     lengthMonth[monthNumber] = 30;
     monthNumber++;
     month[monthNumber] = monthNumber + 1;
     //beginMonth[monthNumber] = beginMonth[monthNumber-1] +  lengthMonth[monthNumber - 1];
     lengthMonth[monthNumber] = 31;
     monthNumber++;
     month[monthNumber] = monthNumber + 1;
     //beginMonth[monthNumber] = beginMonth[monthNumber-1] +  lengthMonth[monthNumber - 1];
     lengthMonth[monthNumber] = 30;
     monthNumber++;
     month[monthNumber] = monthNumber + 1;
     //beginMonth[monthNumber] = beginMonth[monthNumber-1] +  lengthMonth[monthNumber - 1];
     lengthMonth[monthNumber] = 31;



     lengthSeason[0] = lengthMonth[11]+lengthMonth[0]+lengthMonth[1];
     lengthSeason[1] = lengthMonth[2]+lengthMonth[3]+lengthMonth[4];
     lengthSeason[2] = lengthMonth[5]+lengthMonth[6]+lengthMonth[7];
     lengthSeason[3] = lengthMonth[8]+lengthMonth[9]+lengthMonth[10];



     // create the seasonal correlation matrices
     occurrenceMatrixSeasonDJF = (double **)calloc(nrStations, sizeof(double*));
     occurrenceMatrixSeasonMAM = (double **)calloc(nrStations, sizeof(double*));
     occurrenceMatrixSeasonJJA = (double **)calloc(nrStations, sizeof(double*));
     occurrenceMatrixSeasonSON = (double **)calloc(nrStations, sizeof(double*));
     for (int i=0;i<nrStations;i++)
     {
         occurrenceMatrixSeasonDJF[i] = (double *)calloc(lengthSeason[0]*parametersModel.yearOfSimulation, sizeof(double));
         occurrenceMatrixSeasonMAM[i] = (double *)calloc(lengthSeason[1]*parametersModel.yearOfSimulation, sizeof(double));
         occurrenceMatrixSeasonJJA[i] = (double *)calloc(lengthSeason[2]*parametersModel.yearOfSimulation, sizeof(double));
         occurrenceMatrixSeasonSON[i] = (double *)calloc(lengthSeason[3]*parametersModel.yearOfSimulation, sizeof(double));
     }

     for (int i=0;i<nrStations;i++)
     {
         for (int j=0;j<lengthSeason[0]*parametersModel.yearOfSimulation;j++)
         {
             occurrenceMatrixSeasonDJF[i][j] = NODATA;
         }
         for (int j=0;j<lengthSeason[1]*parametersModel.yearOfSimulation;j++)
         {
             occurrenceMatrixSeasonMAM[i][j] = NODATA;
         }
         for (int j=0;j<lengthSeason[2]*parametersModel.yearOfSimulation;j++)
         {
             occurrenceMatrixSeasonJJA[i][j] = NODATA;
         }
         for (int j=0;j<lengthSeason[3]*parametersModel.yearOfSimulation;j++)
         {
             occurrenceMatrixSeasonSON[i][j] = NODATA;
         }

     }

     wDJF = (double **)calloc(nrStations, sizeof(double*));
     wMAM = (double **)calloc(nrStations, sizeof(double*));
     wJJA = (double **)calloc(nrStations, sizeof(double*));
     wSON = (double **)calloc(nrStations, sizeof(double*));
     wSeason = (double **)calloc(nrStations, sizeof(double*));

     for (int i=0;i<nrStations;i++)
     {
         wDJF[i]= (double *)calloc(nrStations, sizeof(double));
         wMAM[i]= (double *)calloc(nrStations, sizeof(double));
         wJJA[i]= (double *)calloc(nrStations, sizeof(double));
         wSON[i]= (double *)calloc(nrStations, sizeof(double));
         wSeason[i]= (double *)calloc(nrStations, sizeof(double));
     }

     for (int i=0;i<nrStations;i++)
     {
         for (int j=0;j<nrStations;j++)
         {
            wDJF[i][j] = wMAM[i][j] = wJJA[i][j] = wSON[i][j] = wSeason[i][j] = NODATA;
         }
     }

 }

void weatherGenerator2D::precipitationMultiDistributionParameterization()
{
    /*int nrDaysIterativeProcessMonthly[12];
    for (int i=0;i<12;i++)
    {
        nrDaysIterativeProcessMonthly[i] = lengthMonth[i]*parametersModel.yearOfSimulation;
    }
    */
    int beginYear,endYear;
    beginYear = endYear = obsDataD[0][0].date.year;
    for (int i=0;i<nrStations;i++)
    {
        for (int j=0;j<nrData;j++)
        {
            beginYear = minValue(beginYear,obsDataD[i][j].date.year);
            endYear =  maxValue(endYear,obsDataD[i][j].date.year);
        }
    }
    // create the seasonal correlation matrices
    /*occurrenceMatrixSeasonDJF = (double **)calloc(nrStations, sizeof(double*));
    occurrenceMatrixSeasonMAM = (double **)calloc(nrStations, sizeof(double*));
    occurrenceMatrixSeasonJJA = (double **)calloc(nrStations, sizeof(double*));
    occurrenceMatrixSeasonSON = (double **)calloc(nrStations, sizeof(double*));

    wDJF = (double **)calloc(nrStations, sizeof(double*));
    wMAM = (double **)calloc(nrStations, sizeof(double*));
    wJJA = (double **)calloc(nrStations, sizeof(double*));
    wSON = (double **)calloc(nrStations, sizeof(double*));
    wSeason = (double **)calloc(nrStations, sizeof(double*));



    //int lengthDJF,lengthMAM,lengthJJA,lengthSON;
    //int lengthSeason[4];


    for (int i=0;i<nrStations;i++)
    {
        occurrenceMatrixSeasonDJF[i] = (double *)calloc(lengthSeason[0]*parametersModel.yearOfSimulation, sizeof(double));
        occurrenceMatrixSeasonMAM[i] = (double *)calloc(lengthSeason[1]*parametersModel.yearOfSimulation, sizeof(double));
        occurrenceMatrixSeasonJJA[i] = (double *)calloc(lengthSeason[2]*parametersModel.yearOfSimulation, sizeof(double));
        occurrenceMatrixSeasonSON[i] = (double *)calloc(lengthSeason[3]*parametersModel.yearOfSimulation, sizeof(double));
    }

    for (int i=0;i<nrStations;i++)
    {
        for (int j=0;j<lengthSeason[0]*parametersModel.yearOfSimulation;j++)
        {
            occurrenceMatrixSeasonDJF[i][j] = NODATA;
        }
        for (int j=0;j<lengthSeason[1]*parametersModel.yearOfSimulation;j++)
        {
            occurrenceMatrixSeasonMAM[i][j] = NODATA;
        }
        for (int j=0;j<lengthSeason[2]*parametersModel.yearOfSimulation;j++)
        {
            occurrenceMatrixSeasonJJA[i][j] = NODATA;
        }
        for (int j=0;j<lengthSeason[3]*parametersModel.yearOfSimulation;j++)
        {
            occurrenceMatrixSeasonSON[i][j] = NODATA;
        }

    }
    */
    for (int i=0;i<nrStations;i++)
    {
        int counterMonth = 11;
        int nrDaysOfMonth = 0;
        //printf("%d\n",lengthDJF);
        for (int j=0;j<lengthSeason[0]*parametersModel.yearOfSimulation;j++)
        {

                occurrenceMatrixSeasonDJF[i][j] = randomMatrix[counterMonth].matrixOccurrences[i][nrDaysOfMonth];
                //printf("%d,%d,%d",j,counterMonth,nrDaysOfMonth);
                //printf("%f\t",occurrenceMatrixSeasonDJF[i][j]);
                //getchar();
                nrDaysOfMonth++;
                if (nrDaysOfMonth >= lengthMonth[counterMonth]*parametersModel.yearOfSimulation)
                {
                    counterMonth++;
                    counterMonth = counterMonth%12;
                    nrDaysOfMonth = 0;
                }
                //printf("\n");

        }


        //getchar();
    }

    /*for (int j=0;j<lengthSeason[0]*parametersModel.yearOfSimulation;j++)
    {
        for (int i=0;i<nrStations;i++)
        {
            printf("%f\t",occurrenceMatrixSeasonDJF[i][j]);
        }
        //pressEnterToContinue();
    }*/
    //pressEnterToContinue();
    for (int i=0;i<nrStations;i++)
    {
        int counterMonth = 2;
        int nrDaysOfMonth = 0;
        //printf("%d\n",lengthMAM);
        for (int j=0;j<lengthSeason[1]*parametersModel.yearOfSimulation;j++)
        {

                occurrenceMatrixSeasonMAM[i][j] = randomMatrix[counterMonth].matrixOccurrences[i][nrDaysOfMonth];
                //printf("%d,%d,%d",j,counterMonth,nrDaysOfMonth);
                //getchar();
                nrDaysOfMonth++;
                if (nrDaysOfMonth >= lengthMonth[counterMonth]*parametersModel.yearOfSimulation)
                {
                    counterMonth++;
                    counterMonth = counterMonth%12;
                    nrDaysOfMonth = 0;
                }

        }
    }

    for (int i=0;i<nrStations;i++)
    {
        int counterMonth = 5;
        int nrDaysOfMonth = 0;
        //printf("%d\n",lengthJJA);
        for (int j=0;j<lengthSeason[2]*parametersModel.yearOfSimulation;j++)
        {

                occurrenceMatrixSeasonJJA[i][j] = randomMatrix[counterMonth].matrixOccurrences[i][nrDaysOfMonth];
                //printf("%f\t",occurrenceMatrixSeasonJJA[i][j]);
                //getchar();
                nrDaysOfMonth++;
                if (nrDaysOfMonth >= lengthMonth[counterMonth]*parametersModel.yearOfSimulation)
                {
                    counterMonth++;
                    counterMonth = counterMonth%12;
                    nrDaysOfMonth = 0;
                }
        }

    }
    for (int i=0;i<nrStations;i++)
    {
        int counterMonth = 8;
        int nrDaysOfMonth = 0;
        //printf("%d\n",lengthSON);
        for (int j=0;j<lengthSeason[3]*parametersModel.yearOfSimulation;j++)
        {

                occurrenceMatrixSeasonSON[i][j] = randomMatrix[counterMonth].matrixOccurrences[i][nrDaysOfMonth];
                //printf("%d,%d,%d",j,counterMonth,nrDaysOfMonth);
                //getchar();
                nrDaysOfMonth++;
                if (nrDaysOfMonth >= lengthMonth[counterMonth]*parametersModel.yearOfSimulation)
                {
                    counterMonth++;
                    counterMonth = counterMonth%12;
                    nrDaysOfMonth = 0;
                }
        }
    }


    /*for (int i=0;i<nrStations;i++)
    {
        wDJF[i]= (double *)calloc(nrStations, sizeof(double));
        wMAM[i]= (double *)calloc(nrStations, sizeof(double));
        wJJA[i]= (double *)calloc(nrStations, sizeof(double));
        wSON[i]= (double *)calloc(nrStations, sizeof(double));
        wSeason[i]= (double *)calloc(nrStations, sizeof(double));
    }*/
    statistics::correlationsMatrix(nrStations,occurrenceMatrixSeasonDJF,lengthSeason[0]*parametersModel.yearOfSimulation,wDJF);
    statistics::correlationsMatrix(nrStations,occurrenceMatrixSeasonMAM,lengthSeason[1]*parametersModel.yearOfSimulation,wMAM);
    statistics::correlationsMatrix(nrStations,occurrenceMatrixSeasonJJA,lengthSeason[2]*parametersModel.yearOfSimulation,wJJA);
    statistics::correlationsMatrix(nrStations,occurrenceMatrixSeasonSON,lengthSeason[3]*parametersModel.yearOfSimulation,wSON);
    /*for (int i=0;i<nrStations;i++)
    {
        for (int j=0;j<nrStations;j++)
        {
            printf("%f\t", wJJA[i][j]);
        }
        printf("\n");
    }
    for (int i=0;i<nrStations;i++)
    {
        for (int j=0;j<nrStations;j++)
        {
            printf("%f\t", wSON[i][j]);
        }
        printf("\n");
    }
    pressEnterToContinue();*/


    // initialize amounts and occurrences structures for precipitation
    // !!!!!! if precipitation is equal to threshold it could generate some computational problems
    for (int i=0;i<nrStations;i++)
    {
        for (int j=0;j<nrData;j++)
        {
           obsPrecDataD[i][j].date.day = obsDataD[i][j].date.day;
           obsPrecDataD[i][j].date.month = obsDataD[i][j].date.month;
           obsPrecDataD[i][j].date.year = obsDataD[i][j].date.year;
           obsPrecDataD[i][j].prec = obsDataD[i][j].prec;
           if (obsPrecDataD[i][j].prec == NODATA)
           {
               obsPrecDataD[i][j].amounts = NODATA;
               obsPrecDataD[i][j].occurrences = NODATA;
               obsPrecDataD[i][j].amountsLessThreshold = NODATA;
           }
           else
           {
               if  (obsPrecDataD[i][j].prec <= parametersModel.precipitationThreshold)
               {
                   obsPrecDataD[i][j].amounts = 0.;
                   obsPrecDataD[i][j].occurrences = 0.;
                   obsPrecDataD[i][j].amountsLessThreshold = 0.;
               }
               else
               {
                   obsPrecDataD[i][j].amounts = obsDataD[i][j].prec;
                   obsPrecDataD[i][j].occurrences = 1.;
                   obsPrecDataD[i][j].amountsLessThreshold = obsPrecDataD[i][j].amounts - parametersModel.precipitationThreshold;
               }
           }
        }

    }

    for (int i=0;i<nrData;i++)
    {
        //printf("%d\n",i);
        for (int j=0;j<nrStations;j++)
        {
            //printf("site %d,%.1f,%.0f\n",j,obsPrecDataD[j][i].amounts,obsPrecDataD[j][i].occurrences);
            //printf("%f\t",obsPrecDataD[j][i].amountsLessThreshold );
        }
        //pressEnterToContinue();
    }


    weatherGenerator2D::initializeOccurrenceIndex();

    double*** moran; // coefficient for each station, each season, each nr of days of simulation in the season.
    moran = (double***)calloc(nrStations, sizeof(double**));
    double*** rainfallLessThreshold; // coefficient for each station, each season, each nr of days of simulation in the season.
    rainfallLessThreshold = (double***)calloc(nrStations, sizeof(double**));


    for (int ijk=0;ijk<nrStations;ijk++)
    {
        //int idStation = ijk;
        // determine how many observed points per season
        //int numberObservedDJF,numberObservedMAM,numberObservedJJA,numberObservedSON;
        //int numberObservedMax;
        numberObservedDJF = numberObservedMAM = numberObservedJJA = numberObservedSON = 0;
        for (int j=0;j<nrData;j++)
        {
            if (obsPrecDataD[ijk][j].date.month == 12 || obsPrecDataD[ijk][j].date.month == 1 || obsPrecDataD[ijk][j].date.month == 2)
                numberObservedDJF++;
            if (obsPrecDataD[ijk][j].date.month == 3 || obsPrecDataD[ijk][j].date.month == 4 || obsPrecDataD[ijk][j].date.month == 5)
                numberObservedMAM++;
            if (obsPrecDataD[ijk][j].date.month == 6 || obsPrecDataD[ijk][j].date.month == 7 || obsPrecDataD[ijk][j].date.month == 8)
                numberObservedJJA++;
            if (obsPrecDataD[ijk][j].date.month == 9 || obsPrecDataD[ijk][j].date.month == 10 || obsPrecDataD[ijk][j].date.month == 11)
                numberObservedSON++;
        }
        numberObservedMax = maxValue(numberObservedDJF,maxValue(numberObservedMAM,maxValue(numberObservedJJA,numberObservedSON)));

        moran[ijk] = (double**)calloc(4, sizeof(double*));
        rainfallLessThreshold[ijk] = (double**)calloc(4, sizeof(double*));
        for (int qq=0;qq<4;qq++)
        {
            moran[ijk][qq] = (double*)calloc(numberObservedMax, sizeof(double));
            rainfallLessThreshold[ijk][qq] = (double*)calloc(numberObservedMax, sizeof(double));
        }
        for (int qq=0;qq<4;qq++)
        {
            for (int i=0;i<numberObservedMax;i++)
            {
                moran[ijk][qq][i] = NODATA;
                rainfallLessThreshold[ijk][qq][i] = NODATA;
            }
        }
        for (int qq=0;qq<4;qq++)
        {
            //double* occCoeff;
            int monthList[3];
            if (qq == 0)
            {
                monthList[0]= 12;
                monthList[1]= 1;
                monthList[2]= 2;
                //moranDJF = (double*)calloc(numberObservedDJF, sizeof(double));
                //occCoeff = (double*)calloc(numberObservedDJF, sizeof(double));
                for (int j=0;j<nrStations;j++)
                {
                    for (int i=0;i<nrStations;i++)
                        wSeason[i][j]=wDJF[i][j];
                }

            }
            else if (qq == 1)
            {
                monthList[0]= 3;
                monthList[1]= 4;
                monthList[2]= 5;
                //moranMAM = (double*)calloc(numberObservedMAM, sizeof(double));
                //occCoeff = (double*)calloc(numberObservedMAM, sizeof(double));
                for (int j=0;j<nrStations;j++)
                {
                    for (int i=0;i<nrStations;i++)
                        wSeason[i][j]=wMAM[i][j];
                }
            }
            else if (qq == 2)
            {
                monthList[0]= 6;
                monthList[1]= 7;
                monthList[2]= 8;
                //moranJJA = (double*)calloc(numberObservedJJA, sizeof(double));
                //occCoeff = (double*)calloc(numberObservedJJA, sizeof(double));
                for (int j=0;j<nrStations;j++)
                {
                    for (int i=0;i<nrStations;i++)
                        wSeason[i][j]=wJJA[i][j];
                }
            }
            else if (qq == 3)
            {
                monthList[0]= 9;
                monthList[1]= 10;
                monthList[2]= 11;
                //moranSON = (double*)calloc(numberObservedSON, sizeof(double));
                //occCoeff = (double*)calloc(numberObservedSON, sizeof(double));
                for (int j=0;j<nrStations;j++)
                {
                    for (int i=0;i<nrStations;i++)
                        wSeason[i][j]=wSON[i][j];
                }
            }
            int counterData = 0;
            //double rainCumulated, moranCumulated; // ??????????????????????????????????
            //rainCumulated = moranCumulated = 0; // ??????????????????????????????????????
            //int contaDatiNonNulli = 0;
            for (int i=0;i<nrData;i++)
            {
                //moran[qq][counterData]= 0;
                if ((obsPrecDataD[ijk][i].date.month == monthList[0]) || (obsPrecDataD[ijk][i].date.month == monthList[1]) || (obsPrecDataD[ijk][i].date.month == monthList[2]))
                {
                    if (obsPrecDataD[ijk][i].occurrences>0)
                    {
                        double denominatorMoran = 0.;
                        double numeratorMoran = 0.;
                        for (int iStations=0;iStations<nrStations;iStations++)
                        {
                            //if ((iStations != ijk) && (obsPrecDataD[iStations][i].occurrences != NODATA))
                            //{
                                //for (int countStation=0; countStation<nrStations;countStation++)
                                //{
                                    if (obsPrecDataD[iStations][i].occurrences != NODATA)
                                    {
                                        //double value;
                                        //printf("%f\n",obsPrecDataD[iStations][i].occurrences);
                                        //value = obsPrecDataD[iStations][i].occurrences*wSeason[ijk][iStations];
                                        numeratorMoran += obsPrecDataD[iStations][i].occurrences*wSeason[ijk][iStations];
                                        denominatorMoran += wSeason[ijk][iStations];
                                        //printf("%f\t",obsPrecDataD[iStations][i].occurrences);
                                        //printf("num %f den %f\n", numeratorMoran,denominatorMoran);
                                        //if (qq==0)
                                        //{
                                            //printf("%f\t%f\t%f",obsPrecDataD[0][i].amountsLessThreshold,obsPrecDataD[1][i].amountsLessThreshold,obsPrecDataD[2][i].amountsLessThreshold);
                                            //getchar();
                                        //}
                                    }
                                //}

                                  //printf("precipitazione sito %f prec atre %f\n",obsPrecDataD[ijk][i].amounts,obsPrecDataD[iStations][i].amounts);
                                  //printf(" %f   %f\n",obsPrecDataD[iStations][i].occurrences*wSeason[ijk][iStations],wSeason[ijk][iStations]);
                            //}
                        }
                        //printf("num %f den %f\n", numeratorMoran,denominatorMoran);
                        if (denominatorMoran != 0)
                        {
                            moran[ijk][qq][counterData] = numeratorMoran / denominatorMoran;
                            rainfallLessThreshold[ijk][qq][counterData] = obsPrecDataD[ijk][i].amountsLessThreshold ;
                        }

                        else
                        {
                           moran[ijk][qq][counterData]= 1;
                           rainfallLessThreshold[ijk][qq][counterData] = obsPrecDataD[ijk][i].amountsLessThreshold;
                        }
                    }
                    /*else
                    {
                        moran[ijk][qq][counterData] = NODATA;
                    }*/
                    /*if (qq == 0 && moran[ijk][qq][counterData] > -1)
                    {

                        printf("%f",moran[ijk][qq][counterData]);
                        getchar();

                    }*/


                    counterData++;
                }

                //pressEnterToContinue();
            }
            //for (int i=0;i<nrStations;i++)
            //{
                //for (int j=0; j<4;j++)
                //{
            for (int k=0; k<numberObservedMax;k++)
            {
                //printf("%d %f\n",k,moran[ijk][qq][k]);
                //pressEnterToContinue();
            }
            //pressEnterToContinue();
                //}
            //}
            int lengthBins = 11;
            double bins[11], bins2[11],bincenter[10];
            int nrBins[10];
            double Pmean[10];
            double PstdDev[10];
            bins[10]= 1.0001;
            for (int i=0;i<10;i++)
            {
                bins[i] = i*0.1;
                bincenter[i] = bins[i] + 0.05;
                nrBins[i] = 0;
                Pmean[i] = 0;
                PstdDev[i] = 0;
            }
            for (int k=0; k<lengthBins-1;k++)
            {
                //printf("%d %f %f\n",k,bins[k],bincenter[k]);
                //pressEnterToContinue();
            }
            //pressEnterToContinue();

            for (int i=0;i<10;i++)
            {
                for (int j=0;j<numberObservedMax;j++)
                {
                    if (moran[ijk][qq][j] >= bins[i] && moran[ijk][qq][j] < bins[i+1])
                    {
                        nrBins[i]++;
                    }
                }
                //printf("prima %d %.1f %d\n",i,bins[i],nrBins[i]);   // risultati strani da fare debug
            }
            //for (int i=0;i<11;i++)
                //printf("prima %d %.1f\n",i,bins[i]);
            //pressEnterToContinue();


            int counter = 1;

            for (int i=1;i<11;i++)
                bins2[i] = NODATA;
            bins2[0]= 0;
            int nrMinimalPointsForBins = 50;
            for (int i=0;i<9;i++)
            {
                if(nrBins[i] < nrMinimalPointsForBins)
                {
                    nrBins[i+1] += nrBins[i];
                }
                else
                {
                    bins2[counter] = bins[i+1];
                    counter++;
                }
            }

            if (nrBins[9] < nrMinimalPointsForBins)
            {
                --counter;
            }
            bins2[counter] = bins[10];
            int newCounter = 1;
            for (int i=1;i<11;i++)
            {
                bins[i]=bins2[i];
                nrBins[i-1] = 0;
                if (bins2[i] != NODATA)
                {
                    bincenter[i-1]= (bins2[i-1] + bins[i])*0.5; //?????
                    newCounter++;
                }
                else
                    bincenter[i-1]= NODATA;

            }
            /*for (int i=0;i<10;i++)
            {
               printf("dopo %d %d %f\n",i,nrBins[i]);
            }
            pressEnterToContinue();
            */
            for (int i=0;i<(newCounter-1);i++)
            {
                for (int j=0;j<numberObservedMax;j++)
                {
                    if (moran[ijk][qq][j] >= bins[i] && moran[ijk][qq][j] < bins[i+1])
                    {
                        nrBins[i]++;
                        Pmean[i] += rainfallLessThreshold[ijk][qq][j];
                    }
                }
                Pmean[i] /= nrBins[i];  // mean for each bin
                if (parametersModel.distributionPrecipitation == 2)
                {
                    for (int j=0;j<numberObservedMax;j++)
                    {
                        if (moran[ijk][qq][j] >= bins[i] && moran[ijk][qq][j] < bins[i+1])
                        {
                            double product;
                            product = rainfallLessThreshold[ijk][qq][j] - Pmean[i];
                            PstdDev[i] += product*product;
                        }
                    }
                    PstdDev[i] = sqrt(PstdDev[i]/(nrBins[i]-1));
                }

            }


            double *parMin;
            double *parMax;
            double *par;
            double *parDelta;
            int maxIterations;
            double epsilon;
            int functionCode;
            int nrPar = 3;
            parMin = (double *) calloc(nrPar, sizeof(double));
            parMax = (double *) calloc(nrPar, sizeof(double));
            par = (double *) calloc(nrPar, sizeof(double));
            parDelta = (double *) calloc(nrPar, sizeof(double));
            parMin[0]= -20.;
            parMax[0]= 20;
            parDelta[0] = 0.01;
            par[0] = (parMin[0]+parMax[0])*0.5;
            parMin[1]= 0;
            parMax[1]= 50.;
            parDelta[1] = 0.0001;
            par[1] = (parMin[1]+parMax[1])*0.5;
            parMin[2]= 1.5;
            parMax[2]= 20.;
            parDelta[2] = 0.01;
            par[2] = (parMin[2]+parMax[2])*0.5;

            maxIterations = 1000000;
            epsilon = 0.0001;
            functionCode = FUNCTION_CODE_TWOPARAMETERSPOLYNOMIAL;
            int nrBincenter=0;
            for (int i=0;i<(lengthBins-1);i++)
            {
                if(bincenter[i] != NODATA)
                    nrBincenter++;
            }
            double* meanPFit = (double *) calloc(nrBincenter, sizeof(double));
            double* stdDevFit = (double *) calloc(nrBincenter, sizeof(double));
            double* binCenter = (double *) calloc(nrBincenter, sizeof(double));
            for (int i=0;i<nrBincenter;i++)
            {
               meanPFit[i]=Pmean[i];
               stdDevFit[i]=PstdDev[i];
               binCenter[i]= bincenter[i];
            }
            for (int i=0;i<nrBincenter;i++)
            {
                //printf("mean obs %f %f\n",binCenter[i],Pmean[i]);
                //printf("std obs %f %f\n",binCenter[i],PstdDev[i]);
            }
            //for (int i=0;i<3;i++)
                //printf("prima %f\n",par[i]);

            interpolation::fittingMarquardt(parMin,parMax,par,nrPar,parDelta,maxIterations,epsilon,functionCode,binCenter,nrBincenter,meanPFit);
            //weatherGenerator2D::bestParametersNonLinearFit(par,nrPar,binCenter,meanPFit,nrBincenter);
            //for (int i=0;i<3;i++) printf("marquardt %f\n",par[i]);
            //for (int i=0;i<nrBincenter;i++) printf("marquardt %f %f \n",Pmean[i],par[0]+par[1]* pow(binCenter[i],par[2])); //pressEnterToContinue();
            // con marquardt stimo già tutti i parametri compreso l'esponente quindi il ciclo
            // for da 2 a 20 (presente nel codice originale) risulta inutile nel codice tradotto            
            for (int i=0;i<nrBincenter;i++)
            {
                meanPFit[i] = par[0]+par[1]* pow(binCenter[i],par[2]);
            }

            // !!! da togliere il seguente for
            //for (int i=0;i<nrBincenter;i++)
            //{
              // meanPFit[i]=Pmean[i];
            //}
            //for (int i=0;i<3;i++)
                //printf("dopo %f\n",par[i]);

            if (parametersModel.distributionPrecipitation == 2)
            {
                interpolation::fittingMarquardt(parMin,parMax,par,nrPar,parDelta,maxIterations,epsilon,functionCode,binCenter,nrBincenter,stdDevFit);
                //weatherGenerator2D::bestParametersNonLinearFit(par,nrPar,binCenter,stdDevFit,nrBincenter);
                //par[0] = 2.0953;
                //par[1] = 18.5122;
                //par[2] = 7.;
                for (int i=0;i<nrBincenter;i++)
                {
                    stdDevFit[i] = par[0]+par[1]* pow(binCenter[i],par[2]);
                }
            }
            //for (int i=0;i<3;i++) printf("marquardt %f\n",par[i]);
            //for (int i=0;i<nrBincenter;i++) printf("marquardt %f %f \n",PstdDev[i],stdDevFit[i]); pressEnterToContinue();
            // !!! da togliere il seguente for
            /*for (int i=0;i<nrBincenter;i++)
            {
               stdDevFit[i]= PstdDev[i];
            }*/
            double** occurrenceMatrixSeason = (double **)calloc(nrStations, sizeof(double*));
            double* moranArray = (double *)calloc(lengthSeason[qq]*parametersModel.yearOfSimulation, sizeof(double));
            int counterMoranPrec = 0;
            for(int i=0; i< nrStations;i++)
            {
                occurrenceMatrixSeason[i] = (double *)calloc(lengthSeason[qq]*parametersModel.yearOfSimulation, sizeof(double));
            }

            for(int i=0; i< nrStations;i++)
            {
                for(int j=0; j< parametersModel.yearOfSimulation*lengthSeason[qq];j++)
                {
                    if (qq == 0)
                    {
                        occurrenceMatrixSeason[i][j]= occurrenceMatrixSeasonDJF[i][j];
                    }
                    else if (qq == 1)
                    {
                        occurrenceMatrixSeason[i][j]= occurrenceMatrixSeasonMAM[i][j];
                    }
                    else if (qq == 2)
                    {
                        occurrenceMatrixSeason[i][j]= occurrenceMatrixSeasonJJA[i][j];
                    }
                    else if (qq == 3)
                    {
                        occurrenceMatrixSeason[i][j]= occurrenceMatrixSeasonSON[i][j];
                    }
                }
            }
            //printf("calcolo MoranArray \n");
            for(int i=0; i< parametersModel.yearOfSimulation*lengthSeason[qq];i++)
            {
                double weightSum = 0;
                moranArray[i]= 0;

                for(int j=0; j< nrStations;j++)
                {
                    weightSum += wSeason[ijk][j];
                }

                for(int j=0;j<nrStations;j++)
                {

                    moranArray[i] += occurrenceMatrixSeason[j][i]* wSeason[ijk][j];
                }
                if (weightSum == 0) moranArray[i] = 0;
                else moranArray[i] /= weightSum;
                //printf("%f\n",moranArray[i]);
                if (occurrenceMatrixSeason[ijk][i] > (1 - EPSILON)) counterMoranPrec++ ;

            }
            int indexMoranArrayPrec = 0;
            double* moranArrayPrec = (double *)calloc(counterMoranPrec, sizeof(double));
            for(int i=0; i< parametersModel.yearOfSimulation*lengthSeason[qq];i++)
            {
                if (occurrenceMatrixSeason[ijk][i] > (1 - EPSILON))
                {
                    moranArrayPrec[indexMoranArrayPrec] = moranArray[i];
                    indexMoranArrayPrec++;
                }
            }
            //printf("moran %d moranSenzaNODATA %d \n",parametersModel.yearOfSimulation*lengthSeason[qq],indexMoranArrayPrec);
            /*for (int i=0;i<indexMoranArrayPrec;i++)
            {
                //printf("%f\n",moranArrayPrec[i]);
            }*/
            //pressEnterToContinue();

            int counterBins = 0;
            for(int i=0; i<11;i++)
            {
                if (bins[i] > NODATA + EPSILON) counterBins++;
                nrBins[i]= 0;
            }

            for(int i=0; i<counterBins-1;i++)
            {
                for (int j=0;j<indexMoranArrayPrec-1;j++)
                {
                    if (moranArrayPrec[j] >= bins[i] && moranArrayPrec[j] < bins[i+1]) nrBins[i]++;
                }
            }
            int nrTotal = 0;
            double frequencyBins[11];
            for(int i=0; i<11;i++)
            {
                nrTotal += nrBins[i];
                frequencyBins[i]=0;
                //printf("%d\t",nrBins[i]);
            }
            //printf("\n totale %d \n",nrTotal);
            for(int i=0; i<counterBins-1;i++)
            {
                frequencyBins[i]= (double)(nrBins[i])/(double)(nrTotal);
            }
            int numberOfDaysAbovePrecThreshold=0;
            double precipitationMean=0.0;
            for(int i=0; i<numberObservedMax ;i++)
            {
                if(rainfallLessThreshold[ijk][qq][i] > 0)
                {
                    precipitationMean += rainfallLessThreshold[ijk][qq][i];
                    numberOfDaysAbovePrecThreshold++;
                }
            }
            if (numberOfDaysAbovePrecThreshold > 0) precipitationMean /= numberOfDaysAbovePrecThreshold;

            double correctionFactor = 0.;
            double cumulatedWeight=0;
            for(int i=0; i<nrBincenter ;i++)
            {
                cumulatedWeight += frequencyBins[i]*meanPFit[i];
            }
            correctionFactor = precipitationMean / cumulatedWeight;

            double* meanFit = (double *) calloc(nrBincenter, sizeof(double));
            double* lambda = (double *) calloc(nrBincenter, sizeof(double));
            //double* lambda2 = (double *) calloc(nrBincenter, sizeof(double));
            for(int i=0; i<nrBincenter ;i++)
            {
                meanFit[i]= meanPFit[i];
                meanPFit[i] *= correctionFactor;
                if (parametersModel.distributionPrecipitation == 1) lambda[i] = 1.0/meanPFit[i];
                //lambda2[i] = 1.0/meanFit[i];
            }
            // start to fill the module output
            if (parametersModel.distributionPrecipitation == 1)
            {
                for (int i=0;i<nrBincenter;i++)
                    occurrenceIndexSeasonal[ijk].parMultiexp[qq][i][0]= lambda[i];
            }
            else if (parametersModel.distributionPrecipitation == 2)
            {
                for (int i=0;i<nrBincenter;i++)
                {
                    occurrenceIndexSeasonal[ijk].parMultiexp[qq][i][0]=meanPFit[i]*meanPFit[i]/(PstdDev[i]*PstdDev[i]);
                    occurrenceIndexSeasonal[ijk].parMultiexp[qq][i][1]=(PstdDev[i]*PstdDev[i])/meanPFit[i];
                    //printf("lambda %f\t%f\n",occurrenceIndexSeasonal[ijk].parMultiexp[qq][i][0],occurrenceIndexSeasonal[ijk].parMultiexp[qq][i][1]);

                }
                //pressEnterToContinue();
            }

            for (int i=0;i<nrBincenter;i++)
            {
                occurrenceIndexSeasonal[ijk].meanP[qq][i] = Pmean[i];
                occurrenceIndexSeasonal[ijk].meanFit[qq][i] = meanFit[i];
                occurrenceIndexSeasonal[ijk].binCenter[qq][i] = binCenter[i];
                if (parametersModel.distributionPrecipitation == 2)
                {
                    occurrenceIndexSeasonal[ijk].stdDevP[qq][i] = PstdDev[i];
                    occurrenceIndexSeasonal[ijk].stdDevFit[qq][i] = stdDevFit[i];
                }
            }
            for (int i=0;i<= nrBincenter;i++)
            {
                occurrenceIndexSeasonal[ijk].bin[qq][i] = bins2[i];
            }

            //printf("\n correction factor %f \n",correctionFactor);
            //getchar();
            // da verificare i numeri che siano numeri plausibili confrontare con i bins precedenti


            // free memory moran and occCoeff

            for(int i=0; i< nrStations;i++)
            {
                free(occurrenceMatrixSeason[i]);
            }
            free (occurrenceMatrixSeason);
            free (moranArray);
            free (par);
            free(parDelta);
            free(parMax);
            free(parMin);
            free(meanPFit);
            free(stdDevFit);
            free(binCenter);
            free(meanFit);
            free(lambda);
        }

    }

    // free the memory step 4

    for (int i=0;i<nrStations;i++)
    {
        for (int qq=0;qq<4;qq++)
        {
            free(moran[i][qq]);
            free(rainfallLessThreshold[i][qq]);
        }
        free(moran[i]);
        free(rainfallLessThreshold[i]);
    }
    free(moran);
    free(rainfallLessThreshold);


}

void weatherGenerator2D::precipitationMultisiteAmountsGeneration()
{
    // begin of step 5

    double** amountMatrixSeasonDJF = (double **)calloc(nrStations, sizeof(double*));
    double** amountMatrixSeasonMAM = (double **)calloc(nrStations, sizeof(double*));
    double** amountMatrixSeasonJJA = (double **)calloc(nrStations, sizeof(double*));
    double** amountMatrixSeasonSON = (double **)calloc(nrStations, sizeof(double*));

    for (int i=0;i<nrStations;i++)
    {
        amountMatrixSeasonDJF[i] = (double *)calloc(numberObservedMax, sizeof(double));
        amountMatrixSeasonMAM[i] = (double *)calloc(numberObservedMax, sizeof(double));
        amountMatrixSeasonJJA[i] = (double *)calloc(numberObservedMax, sizeof(double));
        amountMatrixSeasonSON[i] = (double *)calloc(numberObservedMax, sizeof(double));
    }

    double** amountCorrelationMatrixDJF = (double **)calloc(nrStations, sizeof(double*));
    double** amountCorrelationMatrixMAM = (double **)calloc(nrStations, sizeof(double*));
    double** amountCorrelationMatrixJJA = (double **)calloc(nrStations, sizeof(double*));
    double** amountCorrelationMatrixSON = (double **)calloc(nrStations, sizeof(double*));
    double** amountCorrelationMatrixSeason = (double **)calloc(nrStations, sizeof(double*));
    double** amountCorrelationMatrixSeasonSimulated = (double **)calloc(nrStations, sizeof(double*));
    for (int i=0;i<nrStations;i++)
    {
        amountCorrelationMatrixDJF[i] = (double *)calloc(nrStations, sizeof(double));
        amountCorrelationMatrixMAM[i] = (double *)calloc(nrStations, sizeof(double));
        amountCorrelationMatrixJJA[i] = (double *)calloc(nrStations, sizeof(double));
        amountCorrelationMatrixSON[i] = (double *)calloc(nrStations, sizeof(double));
        amountCorrelationMatrixSeason[i] = (double *)calloc(nrStations, sizeof(double));
        amountCorrelationMatrixSeasonSimulated[i] = (double *)calloc(nrStations, sizeof(double));
    }

    int counterDJF, counterMAM, counterJJA, counterSON;

    for (int i=0;i<nrStations;i++)
    {
        for (int j=0;j<numberObservedMax;j++)
        {
            amountMatrixSeasonDJF[i][j] = NODATA;
            amountMatrixSeasonMAM[i][j] = NODATA;
            amountMatrixSeasonJJA[i][j] = NODATA;
            amountMatrixSeasonSON[i][j] = NODATA;
        }
        for (int j=0;j<nrStations;j++)
        {
            amountCorrelationMatrixSeason[i][j]= NODATA;
            amountCorrelationMatrixSeasonSimulated[i][j]= NODATA;
        }

        counterDJF = counterJJA = counterMAM = counterSON = 0;
        for (int j=0;j<nrData;j++)
        {
            if (obsPrecDataD[i][j].date.month == 12 || obsPrecDataD[i][j].date.month == 1 || obsPrecDataD[i][j].date.month == 2)
            {
                if (obsPrecDataD[i][j].amounts > parametersModel.precipitationThreshold) amountMatrixSeasonDJF[i][counterDJF] = obsPrecDataD[i][j].amounts;
                else amountMatrixSeasonDJF[i][counterDJF] = 0.;
                counterDJF++;
            }
            if (obsPrecDataD[i][j].date.month == 3 || obsPrecDataD[i][j].date.month == 4 || obsPrecDataD[i][j].date.month == 5)
            {
                if (obsPrecDataD[i][j].amounts > parametersModel.precipitationThreshold) amountMatrixSeasonMAM[i][counterMAM] = obsPrecDataD[i][j].amounts;
                else amountMatrixSeasonMAM[i][counterMAM] = 0.;
                counterMAM++;
            }
            if (obsPrecDataD[i][j].date.month == 6 || obsPrecDataD[i][j].date.month == 7 || obsPrecDataD[i][j].date.month == 8)
            {
                if (obsPrecDataD[i][j].amounts > parametersModel.precipitationThreshold)amountMatrixSeasonJJA[i][counterJJA] = obsPrecDataD[i][j].amounts;
                else amountMatrixSeasonJJA[i][counterJJA] = 0.;
                counterJJA++;
            }
            if (obsPrecDataD[i][j].date.month == 9 || obsPrecDataD[i][j].date.month == 10 || obsPrecDataD[i][j].date.month == 11)
            {
                if (obsPrecDataD[i][j].amounts > parametersModel.precipitationThreshold) amountMatrixSeasonSON[i][counterSON] = obsPrecDataD[i][j].amounts;
                else amountMatrixSeasonSON[i][counterSON] = 0.;
                counterSON++;
            }

        }

    }


    statistics::correlationsMatrix(nrStations,amountMatrixSeasonDJF,numberObservedMax,amountCorrelationMatrixDJF);
    statistics::correlationsMatrix(nrStations,amountMatrixSeasonMAM,numberObservedMax,amountCorrelationMatrixMAM);
    statistics::correlationsMatrix(nrStations,amountMatrixSeasonJJA,numberObservedMax,amountCorrelationMatrixJJA);
    statistics::correlationsMatrix(nrStations,amountMatrixSeasonSON,numberObservedMax,amountCorrelationMatrixSON);



    for (int iSeason=0;iSeason<4;iSeason++)
    {
       double** occurrenceSeason = (double **)calloc(nrStations, sizeof(double*));
       double** moranRandom = (double **)calloc(nrStations, sizeof(double*));
       for (int i=0;i<nrStations;i++)
       {
           occurrenceSeason[i] = (double *)calloc(lengthSeason[iSeason]*parametersModel.yearOfSimulation, sizeof(double));
           moranRandom[i] = (double *)calloc(lengthSeason[iSeason]*parametersModel.yearOfSimulation, sizeof(double));
       }
       for (int i=0;i<nrStations;i++)
       {
           for (int j=0;j<lengthSeason[iSeason]*parametersModel.yearOfSimulation;j++)
           {
               occurrenceSeason[i][j]= NODATA;
               moranRandom[i][j] = NODATA;
           }
       }


       if (iSeason == 0)
       {
           for (int i=0;i<nrStations;i++)
           {
               for (int j=0;j<nrStations;j++)
               {
                   wSeason[i][j] = wDJF[i][j];
                   amountCorrelationMatrixSeason[i][j]= amountCorrelationMatrixDJF[i][j];
               }
               for (int j=0;j<lengthSeason[iSeason]*parametersModel.yearOfSimulation;j++)
               {
                    occurrenceSeason[i][j] = occurrenceMatrixSeasonDJF[i][j];
               }
           }

       }
       if (iSeason == 1)
       {
           for (int i=0;i<nrStations;i++)
           {
               for (int j=0;j<nrStations;j++)
               {
                   wSeason[i][j] = wMAM[i][j];
                   amountCorrelationMatrixSeason[i][j]= amountCorrelationMatrixMAM[i][j];
                   //printf("%f ",wMAM[i][j]);
               }
               for (int j=0;j<lengthSeason[iSeason]*parametersModel.yearOfSimulation;j++)
               {
                    occurrenceSeason[i][j] = occurrenceMatrixSeasonMAM[i][j];
                    //printf("%f \n",occurrenceMatrixSeasonMAM[i][j]);
               }
               //printf("\n");
               //pressEnterToContinue();
           }
       }
       if (iSeason == 2)
       {
           for (int i=0;i<nrStations;i++)
           {
               for (int j=0;j<nrStations;j++)
               {
                   wSeason[i][j] = wJJA[i][j];
                   amountCorrelationMatrixSeason[i][j]= amountCorrelationMatrixJJA[i][j];
               }
               for (int j=0;j<lengthSeason[iSeason]*parametersModel.yearOfSimulation;j++)
               {
                    occurrenceSeason[i][j] = occurrenceMatrixSeasonJJA[i][j];
               }
           }
       }
       if (iSeason == 3)
       {
           for (int i=0;i<nrStations;i++)
           {
               for (int j=0;j<nrStations;j++)
               {
                   wSeason[i][j] = wSON[i][j];
                   amountCorrelationMatrixSeason[i][j]= amountCorrelationMatrixSON[i][j];
               }
               for (int j=0;j<lengthSeason[iSeason]*parametersModel.yearOfSimulation;j++)
               {
                    occurrenceSeason[i][j] = occurrenceMatrixSeasonSON[i][j];
               }
           }
       }
       for (int iStations=0;iStations<nrStations;iStations++)
       {
                   for (int i=0;i<lengthSeason[iSeason]*parametersModel.yearOfSimulation;i++)
                   {

                       if (occurrenceSeason[iStations][i] > EPSILON)
                       {
                           double denominatorMoran = 0.;
                           double numeratorMoran = 0.;
                           for (int jStations=0;jStations<nrStations;jStations++)
                           {
                                 //double dummyTest;
                                 //dummyTest = occurrenceSeason[jStations][i];
                                 numeratorMoran += occurrenceSeason[jStations][i]*wSeason[nrStations-1][jStations];
                                 denominatorMoran += wSeason[nrStations-1][jStations];
                           }
                           if (fabs(denominatorMoran) < EPSILON)
                           {
                               moranRandom[iStations][i] = numeratorMoran / denominatorMoran;
                           }

                           else
                           {
                              moranRandom[iStations][i]= 1;
                           }

                       }

                        //printf("%d %f\n",iSeason,moranRandom[iStations][i]);
                   }
                   //for (int jStations=0;jStations<nrStations;jStations++)
                   //{


                   //}
                    //printf("\n");
                   //pressEnterToContinue();

       }


       double** phatAlpha = (double **)calloc(nrStations, sizeof(double*));
       double** phatBeta = (double **)calloc(nrStations, sizeof(double*));
       double** randomMatrixNormalDistribution = (double **)calloc(nrStations, sizeof(double*));
       double** simulatedPrecipitationAmountsSeasonal = (double **)calloc(nrStations, sizeof(double*));

       for (int i=0;i<nrStations;i++)
       {
            phatAlpha[i] = (double *)calloc(lengthSeason[iSeason]*parametersModel.yearOfSimulation, sizeof(double));
            phatBeta[i] = (double *)calloc(lengthSeason[iSeason]*parametersModel.yearOfSimulation, sizeof(double));
            randomMatrixNormalDistribution[i] = (double *)calloc(lengthSeason[iSeason]*parametersModel.yearOfSimulation, sizeof(double));
            simulatedPrecipitationAmountsSeasonal[i] = (double *)calloc(lengthSeason[iSeason]*parametersModel.yearOfSimulation, sizeof(double));
       }

       for (int i=0;i<nrStations;i++)
       {
           for (int j=0;j<lengthSeason[iSeason]*parametersModel.yearOfSimulation;j++)
           {
              simulatedPrecipitationAmountsSeasonal[i][j]= NODATA;
           }
       }

       for (int j=0;j<lengthSeason[iSeason]*parametersModel.yearOfSimulation;j++)
       {
           for (int i=0;i<nrStations;i++)
           {
                for (int k=0;k<10;k++) //to improve
                {
                    if ((moranRandom[i][j] > occurrenceIndexSeasonal[i].bin[iSeason][k]) && (moranRandom[i][j] <= occurrenceIndexSeasonal[i].bin[iSeason][k+1]))
                    {
                        phatAlpha[i][j] = occurrenceIndexSeasonal[i].parMultiexp[iSeason][k][0];
                        if (parametersModel.distributionPrecipitation == 2)
                        {
                            phatBeta[i][j] = occurrenceIndexSeasonal[i].parMultiexp[iSeason][k][1];
                        }
                    }

                }
                //printf("%f  ",phatAlpha[i][j]);

           }
           //printf("\n");
       }



       int gasDevIset = 0;
       double gasDevGset = 0;
       srand (time(NULL));
       for (int j=0;j<lengthSeason[iSeason]*parametersModel.yearOfSimulation;j++)
       {
           for (int i=0;i<nrStations;i++)
           {
                randomMatrixNormalDistribution[i][j] = myrandom::normalRandom(&gasDevIset,&gasDevGset);
                //printf("%f  ",randomMatrixNormalDistribution[i][j]);
           }
           //printf("\n");
       }
       //pressEnterToContinue();
       // !! da cavare
       /*int counterRandom=0;
       double* arrayRandomNormal;
       arrayRandomNormal = (double *)calloc(nrStations*lengthSeason[iSeason]*parametersModel.yearOfSimulation, sizeof(double));
       randomSet(arrayRandomNormal,nrStations*lengthSeason[iSeason]*parametersModel.yearOfSimulation);
       for (int i=0;i<nrStations*lengthSeason[iSeason]*parametersModel.yearOfSimulation;i++)
       {
            printf("%d %f\n",i,arrayRandomNormal[i]);
            pressEnterToContinue();
       }
       for (int i=0;i<nrStations;i++)
       {
           for (int j=0;j<lengthSeason[iSeason]*parametersModel.yearOfSimulation;j++)
           {
                randomMatrixNormalDistribution[i][j] = arrayRandomNormal[counterRandom] ;
                counterRandom++;
           }
       }*/
       /*
       for (int j=0;j<lengthSeason[iSeason]*parametersModel.yearOfSimulation;j++)
       {
           for (int i=0;i<nrStations;i++)
           {
                printf("%f  ",randomMatrixNormalDistribution[i][j]);
           }
           printf("\n");
       }
       pressEnterToContinue();
       */
       //free(arrayRandomNormal);
       // fine da cavare


       printf("fase 5a\n");
       weatherGenerator2D::spatialIterationAmounts(amountCorrelationMatrixSeasonSimulated , amountCorrelationMatrixSeason,randomMatrixNormalDistribution,lengthSeason[iSeason]*parametersModel.yearOfSimulation,occurrenceSeason,phatAlpha,phatBeta,simulatedPrecipitationAmountsSeasonal);
       //printf("%d \n",iSeason);


       for (int i=0;i<nrStations;i++)
       {
            for (int j=0;j<nrStations;j++)
            {
                simulatedPrecipitationAmounts[iSeason].matrixK[i][j] = amountCorrelationMatrixSeasonSimulated[i][j];
                simulatedPrecipitationAmounts[iSeason].matrixM[i][j] = amountCorrelationMatrixSeason[i][j];
            }
            for (int j=0;j<lengthSeason[iSeason]*parametersModel.yearOfSimulation;j++)
            {
                simulatedPrecipitationAmounts[iSeason].matrixAmounts[i][j]= simulatedPrecipitationAmountsSeasonal[i][j];
                //printf("%f  ",simulatedPrecipitationAmounts[iSeason].matrixAmounts[i][j]);
            }
            //printf("\n");
       }

       printf("fase 5b\n");

       // free memory
       for (int i=0;i<nrStations;i++)
       {
           free(phatAlpha[i]);
           free(phatBeta[i]);
           free(randomMatrixNormalDistribution[i]);
           free(occurrenceSeason[i]);
           free(moranRandom[i]);
           free(simulatedPrecipitationAmountsSeasonal[i]);

       }
       free(phatAlpha);
       free(phatBeta);
       free(randomMatrixNormalDistribution);
       free(occurrenceSeason);
       free(moranRandom);
       free(simulatedPrecipitationAmountsSeasonal);

    }





    // free the memory step 5
    for (int i=0;i<nrStations;i++)
    {

        free(amountMatrixSeasonJJA[i]);
        free(amountMatrixSeasonDJF[i]);
        free(amountMatrixSeasonMAM[i]);
        free(amountMatrixSeasonSON[i]);
    }

    free(amountMatrixSeasonJJA);
    free(amountMatrixSeasonDJF);
    free(amountMatrixSeasonMAM);
    free(amountMatrixSeasonSON);

    for (int i=0;i<nrStations;i++)
    {

        free(amountCorrelationMatrixDJF[i]);
        free(amountCorrelationMatrixJJA[i]);
        free(amountCorrelationMatrixMAM[i]);
        free(amountCorrelationMatrixSON[i]);
        free(amountCorrelationMatrixSeason[i]);
        free(amountCorrelationMatrixSeasonSimulated[i]);
    }

    free(amountCorrelationMatrixDJF);
    free(amountCorrelationMatrixJJA);
    free(amountCorrelationMatrixMAM);
    free(amountCorrelationMatrixSON);
    free(amountCorrelationMatrixSeason);
    free(amountCorrelationMatrixSeasonSimulated);

    // free the memory of arrays declared in step 4 but used in 5
    for (int i=0;i<nrStations;i++)
    {
        free(wDJF[i]);
        free(wJJA[i]);
        free(wMAM[i]);
        free(wSON[i]);
        free(wSeason[i]);
        free(occurrenceMatrixSeasonJJA[i]);
        free(occurrenceMatrixSeasonDJF[i]);
        free(occurrenceMatrixSeasonMAM[i]);
        free(occurrenceMatrixSeasonSON[i]);
    }
    free(wDJF);
    free(wJJA);
    free(wMAM);
    free(wSON);
    free(wSeason);
    free(occurrenceMatrixSeasonDJF);
    free(occurrenceMatrixSeasonJJA);
    free(occurrenceMatrixSeasonMAM);
    free(occurrenceMatrixSeasonSON);



}

void weatherGenerator2D::initializeOccurrenceIndex()
{
    occurrenceIndexSeasonal = (ToccurrenceIndexSeasonal *)calloc(nrStations, sizeof(ToccurrenceIndexSeasonal));
    for (int i=0;i<nrStations;i++)
    {
        occurrenceIndexSeasonal[i].binCenter = (double **)calloc(4, sizeof(double*));
        occurrenceIndexSeasonal[i].bin = (double **)calloc(4, sizeof(double*));
        occurrenceIndexSeasonal[i].meanFit = (double **)calloc(4, sizeof(double*));
        occurrenceIndexSeasonal[i].meanP = (double **)calloc(4, sizeof(double*));
        occurrenceIndexSeasonal[i].stdDevFit = (double **)calloc(4, sizeof(double*));
        occurrenceIndexSeasonal[i].stdDevP = (double **)calloc(4, sizeof(double*));
        occurrenceIndexSeasonal[i].parMultiexp = (double ***)calloc(4, sizeof(double**));
    }
    for (int i=0;i<nrStations;i++)
    {
        for (int j=0;j<4;j++)
        {
            occurrenceIndexSeasonal[i].bin[j] = (double *)calloc(11, sizeof(double));
            occurrenceIndexSeasonal[i].binCenter[j] = (double *)calloc(10, sizeof(double));
            occurrenceIndexSeasonal[i].meanFit[j] = (double *)calloc(10, sizeof(double));
            occurrenceIndexSeasonal[i].meanP[j] = (double *)calloc(10, sizeof(double));
            occurrenceIndexSeasonal[i].stdDevFit[j] = (double *)calloc(10, sizeof(double));
            occurrenceIndexSeasonal[i].stdDevP[j] = (double *)calloc(10, sizeof(double));

            occurrenceIndexSeasonal[i].parMultiexp[j] = (double **)calloc(10, sizeof(double*));
        }
        for (int j=0;j<4;j++)
        {
            for (int k=0;k<10;k++)
            {
                occurrenceIndexSeasonal[i].parMultiexp[j][k] = (double *)calloc(2, sizeof(double));
            }
        }
    }

    for (int i=0;i<nrStations;i++)
    {
        for (int j=0;j<4;j++)
        {
            for (int k=0;k<10;k++)
            {
                occurrenceIndexSeasonal[i].bin[j][k] = NODATA;
                occurrenceIndexSeasonal[i].binCenter[j][k] = NODATA;
                occurrenceIndexSeasonal[i].meanFit[j][k] = NODATA;
                occurrenceIndexSeasonal[i].meanP[j][k] = NODATA;
                occurrenceIndexSeasonal[i].stdDevFit[j][k] = NODATA;
                occurrenceIndexSeasonal[i].stdDevP[j][k] = NODATA;
                occurrenceIndexSeasonal[i].parMultiexp[j][k][0] = NODATA;
                occurrenceIndexSeasonal[i].parMultiexp[j][k][1] = NODATA;
            }
            occurrenceIndexSeasonal[i].bin[j][10] = NODATA;
        }
    }


}

void weatherGenerator2D::initializePrecipitationOutputs(int lengthSeason[])
{
    simulatedPrecipitationAmounts = (TsimulatedPrecipitationAmounts *)calloc(4, sizeof(TsimulatedPrecipitationAmounts));
    for (int iSeason=0;iSeason<4;iSeason++)
    {
         if (iSeason == 0)
            simulatedPrecipitationAmounts[iSeason].season = DJF;
        else if (iSeason == 1)
            simulatedPrecipitationAmounts[iSeason].season = MAM;
        else if (iSeason == 2)
            simulatedPrecipitationAmounts[iSeason].season = JJA;
        else if (iSeason == 3)
            simulatedPrecipitationAmounts[iSeason].season = SON;

        simulatedPrecipitationAmounts[iSeason].matrixAmounts = (double **)calloc(nrStations, sizeof(double*));
        simulatedPrecipitationAmounts[iSeason].matrixK = (double **)calloc(nrStations, sizeof(double*));
        simulatedPrecipitationAmounts[iSeason].matrixM = (double **)calloc(nrStations, sizeof(double*));

        for (int i=0;i<nrStations;i++)
        {
            simulatedPrecipitationAmounts[iSeason].matrixAmounts[i] = (double *)calloc(lengthSeason[iSeason]*parametersModel.yearOfSimulation, sizeof(double));
            simulatedPrecipitationAmounts[iSeason].matrixK[i] = (double *)calloc(nrStations, sizeof(double));
            simulatedPrecipitationAmounts[iSeason].matrixM[i] = (double *)calloc(nrStations, sizeof(double));
        }

        for (int i=0;i<nrStations;i++)
        {
            for (int j=0;j<nrStations;j++)
            {
                simulatedPrecipitationAmounts[iSeason].matrixK[i][j] = NODATA;
                simulatedPrecipitationAmounts[iSeason].matrixM[i][j] = NODATA;
            }
            for (int j=0;j<lengthSeason[iSeason]*parametersModel.yearOfSimulation;j++)
            {
                simulatedPrecipitationAmounts[iSeason].matrixAmounts[i][j] = NODATA;
            }
        }
    }
}



/*void weatherGenerator2D::nonLinearFit(double* a1, double* a2, double* x, double* y,int lengthArray, int order)
{
    double* fitFunction;
    fitFunction = (double*)calloc(lengthArray, sizeof(double));
    double step = 0.001;
    double par1 = -20.;
    double par2 = 0.;
    double bestFit = 1000000;
    for (int ii=0;ii<4000;ii++)
    {
        for (int jj=0;jj<2000;jj++)
        {

            for (int j=0;j<lengthArray;j++)
            {
                fitFunction[j]= par1 + par2*pow(x[j],order);                
            }
            statistics::rootMeanSquareError(y,fitFunction,lengthArray);
        }
    }

}
*/






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

void weatherGenerator2D::spatialIterationAmounts(double** correlationMatrixSimulatedData,double ** amountsCorrelationMatrix , double** randomMatrix, int lengthSeries, double** occurrences, double** phatAlpha, double** phatBeta, double** simulatedPrecipitationAmountsSeasonal)
{
    double val=5;
    int ii=0;
    double kiter=0.1;
    double** dummyMatrix = (double**)calloc(nrStations, sizeof(double*));
    double** dummyMatrix2 = (double**)calloc(nrStations, sizeof(double*));
    double* correlationArray =(double*)calloc(nrStations*nrStations, sizeof(double));
    double* eigenvalues =(double*)calloc(nrStations, sizeof(double));
    double* eigenvectors =(double*)calloc(nrStations*nrStations, sizeof(double));
    double** dummyMatrix3 = (double**)calloc(nrStations, sizeof(double*));
    double** normRandom = (double**)calloc(nrStations, sizeof(double*));
    double** uniformRandom = (double**)calloc(nrStations, sizeof(double*));
    //double** correlationMatrixSimulatedData = (double**)calloc(nrStations, sizeof(double*));
    double** initialAmountsCorrelationMatrix = (double**)calloc(nrStations, sizeof(double*));


    // initialization internal arrays
    for (int i=0;i<nrStations;i++)
    {
        dummyMatrix[i]= (double*)calloc(nrStations, sizeof(double));
        dummyMatrix2[i]= (double*)calloc(nrStations, sizeof(double));
        correlationMatrixSimulatedData[i]= (double*)calloc(nrStations, sizeof(double));
        initialAmountsCorrelationMatrix[i]= (double*)calloc(nrStations, sizeof(double));
        for (int j=0;j<nrStations;j++)
        {
            dummyMatrix[i][j]= NODATA;
            dummyMatrix2[i][j]= NODATA;
            correlationMatrixSimulatedData[i][j]= NODATA;
            initialAmountsCorrelationMatrix[i][j]= NODATA;
        }
    }
    for (int i=0;i<nrStations;i++)
    {
        eigenvalues[i]=NODATA;
        for (int j=0;j<nrStations;j++) eigenvectors[i*nrStations+j] = NODATA;
    }
    for (int i=0;i<nrStations;i++)
    {
        dummyMatrix3[i]= (double*)calloc(lengthSeries, sizeof(double));
        normRandom[i]= (double*)calloc(lengthSeries, sizeof(double));
        uniformRandom[i]= (double*)calloc(lengthSeries, sizeof(double));
        for (int j=0;j<lengthSeries;j++)
        {
            dummyMatrix3[i][j]= NODATA;
            normRandom[i][j]= NODATA;
            uniformRandom[i][j]= NODATA;
        }

    }

    for (int i=0;i<nrStations;i++)
    {
        for (int j=0;j<nrStations;j++)
        {
            initialAmountsCorrelationMatrix[i][j] = amountsCorrelationMatrix[i][j];
            //printf("%f ",initialAmountsCorrelationMatrix[i][j]);
        }
        //printf("\n");
    }
    //pressEnterToContinue();
    double minimalValueToExitFromCycle = NODATA;
    int counterConvergence=0;
    bool exitWhileCycle = false;

    while ((val>TOLERANCE_MULGETS) && (ii<MAX_ITERATION_MULGETS) && (!exitWhileCycle))
    {
        ii++;
        int nrEigenvaluesLessThan0 = 0;
        int counter = 0;
        for (int i=0;i<nrStations;i++)
        {
            for (int j=0;j<nrStations;j++) // avoid solutions with correlation coefficient greater than 1
            {
                amountsCorrelationMatrix[i][j] = minValue(amountsCorrelationMatrix[i][j],1);
                correlationArray[counter] = amountsCorrelationMatrix[i][j];
                counter++;
                //printf("%f ",M[i][j]);
            }
            //printf("\n");
        }
        //printf("inizio sub\n");
        eigenproblem::rs(nrStations,correlationArray,eigenvalues,true,eigenvectors);


        for (int i=0;i<nrStations;i++)
        {
            if (eigenvalues[i] <= 0)
            {
                nrEigenvaluesLessThan0++;
                eigenvalues[i] = 0.000001;
            }
        }
        if (nrEigenvaluesLessThan0 > 0)
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
            matricial::matrixProduct(dummyMatrix,dummyMatrix2,nrStations,nrStations,nrStations,nrStations,amountsCorrelationMatrix);
        }

        for (int i=0;i<nrStations;i++)
            for (int j=0;j<nrStations;j++) dummyMatrix[i][j] = amountsCorrelationMatrix[i][j];

        matricial::choleskyDecompositionTriangularMatrix(dummyMatrix,nrStations,true);
        /*printf("Cholesky\n");
        for (int i=0;i<nrStations;i++)
        {
            for (int j=0;j<nrStations;j++) printf("%f ",dummyMatrix[i][j]);
        printf("\n");
        }*/
        //pressEnterToContinue();

        matricial::matrixProduct(dummyMatrix,randomMatrix,nrStations,nrStations,lengthSeries,nrStations,dummyMatrix3);
        /*printf("prodottoMatriciale\n");
        for (int i=0;i<lengthSeries;i++)
        {
            for (int j=0;j<nrStations;j++) printf("%f ",dummyMatrix3[j][i]);
        printf("\n");
        }
        pressEnterToContinue();
        */
        for (int i=0;i<nrStations;i++)
        {
            // compute mean and standard deviation without NODATA check
            double meanValue,stdDevValue;
            meanValue = stdDevValue = 0;
            for (int j=0;j<lengthSeries;j++)
                meanValue += dummyMatrix3[i][j];
            meanValue /= lengthSeries;
            for (int j=0;j<lengthSeries;j++)
                stdDevValue += (dummyMatrix3[i][j]- meanValue)*(dummyMatrix3[i][j]- meanValue);
            stdDevValue /= (lengthSeries-1);
            stdDevValue = sqrt(stdDevValue);

            for (int j=0;j<lengthSeries;j++)
            {
                normRandom[i][j]= (dummyMatrix3[i][j]-meanValue)/stdDevValue;
            }
        }
        /*
        for (int i=0;i<lengthSeries;i++)
        {
            for (int j=0;j<nrStations;j++) printf("%f ",normRandom[j][i]);
            printf("\n");
        }
        pressEnterToContinue();*/

        for (int i=0;i<nrStations;i++)
        {
            for (int j=0;j<lengthSeries;j++)
            {
               uniformRandom[i][j] =0.5*statistics::ERFC(-normRandom[i][j]/SQRT_2,0.0001);
            }
        }
        /*for (int i=0;i<lengthSeries;i++)
        {
            for (int j=0;j<nrStations;j++) printf("%f ",uniformRandom[j][i]);
            printf("\n");
        }pressEnterToContinue();*/
        for (int i=0;i<nrStations;i++)
        {
            for (int j=0;j<lengthSeries;j++)
            {
                simulatedPrecipitationAmountsSeasonal[i][j]=0.;
            }
        }

        for (int i=0;i<nrStations;i++)
        {
            for (int j=0;j<lengthSeries;j++)
            {
                if (fabs(occurrences[i][j]-1) <= 0.00001)
                {
                    if (parametersModel.distributionPrecipitation == 1)
                    {
                        simulatedPrecipitationAmountsSeasonal[i][j] =-log(1-uniformRandom[i][j])/phatAlpha[i][j]+ parametersModel.precipitationThreshold;
                    }
                    else
                    {
                        if (uniformRandom[i][j] <= 0) uniformRandom[i][j] = 0.00001;
                        simulatedPrecipitationAmountsSeasonal[i][j] = weatherGenerator2D::inverseGammaFunction(uniformRandom[i][j],phatAlpha[i][j],phatBeta[i][j],0.0001) + parametersModel.precipitationThreshold;
                        //printf("%f %f %f %f\n",simulatedPrecipitationAmounts[i][j], uniformRandom[i][j],phatAlpha[i][j],phatBeta[i][j]);
                        //pressEnterToContinue();
                        // check uniformRandom phatAlpha e phatBeta i dati non vanno bene
                    }
                }

            }
        }
        /*for (int i=0;i<lengthSeries;i++)
                {
                    for (int j=0;j<nrStations;j++) printf("%f ",simulatedPrecipitationAmountsSeasonal[j][i]);
                    printf("\n");
                }pressEnterToContinue();*/

        for (int i=0;i<nrStations;i++)
        {
            for (int j=0;j<nrStations;j++)
            {
                statistics::correlationsMatrix(nrStations,simulatedPrecipitationAmountsSeasonal,lengthSeries,correlationMatrixSimulatedData);
                // da verificare dovrebbe esserci correlazione solo per i dati diversi da zero
            }
        }
        /*printf("matrice di correlazione\n");
        for (int i=0;i<nrStations;i++)
                {
                    for (int j=0;j<nrStations;j++) printf("%f ",correlationMatrixSimulatedData[j][i]);
                    printf("\n");
                }pressEnterToContinue();*/

        //double maxValueCorrelation = NODATA;
        val = 0;
        for (int i=0;i<nrStations;i++)
        {
            for (int j=0;j<nrStations;j++)
            {
                val = maxValue(val,fabs(correlationMatrixSimulatedData[i][j]- initialAmountsCorrelationMatrix[i][j]));
            }
        }
        if (val < fabs(minimalValueToExitFromCycle))
        {
            minimalValueToExitFromCycle = val;
            counterConvergence = 0;
        }
        else
        {
            counterConvergence++;
        }
        if (counterConvergence > 20)
        {
            if (val <= fabs(minimalValueToExitFromCycle) + TOLERANCE_MULGETS) exitWhileCycle = true;
        }
        //printf("contatore per uscire %d contatore %d val %f",counterConvergence, ii, val); pressEnterToContinue();

        if (ii != MAX_ITERATION_MULGETS && val> TOLERANCE_MULGETS && (!exitWhileCycle))
        {
            for (int i=0;i<nrStations;i++)
            {
                for (int j=0;j<nrStations;j++)
                {
                    if (i == j)
                    {
                        amountsCorrelationMatrix[i][j]=1.;
                    }
                    else
                    {
                        amountsCorrelationMatrix[i][j] += kiter*(initialAmountsCorrelationMatrix[i][j]-correlationMatrixSimulatedData[i][j]);
                    }
                }
            }
        }
        printf("step %d %f\n",ii,val);
        /*for (int i=0;i<nrStations;i++)
                {
                    for (int j=0;j<nrStations;j++) printf("%f ",amountsCorrelationMatrix[j][i]);
                    printf("\n");
                }pressEnterToContinue();*/

    }

    printf("\t");
    // free memory
    for (int i=0;i<nrStations;i++)
    {
        free(dummyMatrix[i]);
        free(dummyMatrix2[i]);
        free(dummyMatrix3[i]);
        free(normRandom[i]);
        free(uniformRandom[i]);
        //free(correlationMatrixSimulatedData[i]);
        free(initialAmountsCorrelationMatrix[i]);
    }


        free(dummyMatrix);
        free(dummyMatrix2);
        free(dummyMatrix3);
        free(normRandom);
        free(uniformRandom);
        free(correlationArray);
        free(eigenvalues);
        free(eigenvectors);
        //free(correlationMatrixSimulatedData);
        free(initialAmountsCorrelationMatrix);

}


double weatherGenerator2D::inverseGammaFunction(double valueProbability, double alpha, double beta, double accuracy)
{
    double x;
    double y;
    //double gammaComplete;
    double rightBound = 25.0;
    double leftBound = 0.0;
    int counter = 0;
    do {
        y = gammaDistributions::incompleteGamma(alpha,rightBound/beta);
        if (valueProbability>y)
        {
            //leftBound += rightBound - 2*accuracy;
            rightBound += 25;
            counter++;
        }
    } while ((valueProbability>y) && (counter<10));
    counter = 0;
    x = (rightBound + leftBound)*0.5;
    y = gammaDistributions::incompleteGamma(alpha,x/beta);
    //printf("prova\n"); //pressEnterToContinue();
    while ((fabs(valueProbability - y) > accuracy) && (counter < 200))
    {
        if (y > valueProbability)
        {
            rightBound = x;
        }
        else
        {
            leftBound = x;
        }
        x = (rightBound + leftBound)*0.5;
        //y = gammaDistributions::incompleteGamma(alpha,x/beta,&gammaComplete);
        y = gammaDistributions::incompleteGamma(alpha,x/beta);
        counter++;
        //printf("valore %f valore Input %f prec %f contatore %d\n",y,valueProbability,x,counter);
    }
    x = (rightBound + leftBound)*0.5;
    return x;
}


void weatherGenerator2D::pressEnterToContinue()
{
    printf("press return to continue\n");
    getchar();
}

double weatherGenerator2D::bestFit(double *par, int nrPar, double*x, double *yObs, int nrX)
{
    double sigma=0.;
    double diff;
    double ySim[3];
    for (int i=0; i<nrX; i++)
    {
        diff = par[0]+ par[1]*pow(x[i],par[2])-yObs[i];
        sigma += (diff * diff);
    }


                //diff = ySim[i]-yObs[i];
                //sigma += (diff * diff);
                //nrValidValues++;

    return sigma;
    //return statistics::rootMeanSquareError(yObs,ySim,nrX);
}



int weatherGenerator2D::bestParametersNonLinearFit(double *par, int nrPar, double*x, double *yObs, int nrX)
{
    par[0]= 0.;
    par[1]= 0.;
    double bestPar[3], rootMeanSquare;
    double maxPar[2];
    int deltaStep[2];
    bool firstStep = true;
    double accuracy = 0.5;
    double rmsNew;
    maxPar[0] = maxPar [1] = yObs[0];
    rootMeanSquare = NODATA;
    for (int i=0 ; i<nrX-1; i++)
    {
        maxPar[0]= maxValue(yObs[i],yObs[i+1])*10;
        maxPar[1]= 5*maxPar[0];
    }
    deltaStep[0] = (int)(ceil((maxPar[0]-par[0])/accuracy));
    deltaStep[1] = (int)(ceil((maxPar[1]-par[1])/accuracy));
    for (int i=2 ; i<20; i++)
    {
        par[2]=(double)(i);
        //printf("esponente %d\t ",i);
        for (int j=0 ; j<deltaStep[0]; j++)
        {
            par[0] += accuracy;
            for (int k=0 ; k<deltaStep[1]; k++)
            {
                par[1] += accuracy;
                rmsNew = weatherGenerator2D::bestFit(par,3,x,yObs,nrX);
                if (firstStep)
                {
                    rootMeanSquare = rmsNew;
                    bestPar[0]=par[0];
                    bestPar[1]=par[1];
                    bestPar[2]=par[2];
                    firstStep = false;
                }
                else if (rmsNew < rootMeanSquare)
                {
                    rootMeanSquare = rmsNew;
                    bestPar[0]=par[0];
                    bestPar[1]=par[1];
                    bestPar[2]=par[2];
                }
            }
            par[1]= 0;
        }
        par[0]= 0;
        par[1]= 0.;
    }
    par[0]= bestPar[0];
    par[1]= bestPar[1];
    par[2]= bestPar[2];
    if ((par[0] >= maxPar[0]) || (par[1] >= maxPar[1])) return 0;
    else return 1;

}

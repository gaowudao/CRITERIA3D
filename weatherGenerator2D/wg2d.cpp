/*!
    \file wg2D.cpp

    \abstract 2D weather generator

    \note
    The spatial weather generator model is translated from the MulGets model available online on:
    https://it.mathworks.com/matlabcentral/fileexchange/47537-multi-site-stochstic-weather-generator--mulgets-

    \copyright
    This file is part of CRITERIA-3D distribution.
    CRITERIA-3D has been developed by A.R.P.A.E. Emilia-Romagna.

    CRITERIA-3D is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    CRITERIA-3D is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.
    You should have received a copy of the GNU Lesser General Public License
    along with CRITERIA-3D.  If not, see <http://www.gnu.org/licenses/>.

    \authors
    Antonio volta avolta@arpae.it
    Fausto Tomei ftomei@arpae.it
    Gabriele Antolini gantolini@arpae.it
*/


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
    }
}


void weatherGenerator2D::computeWeatherGenerator2D()
{
    weatherGenerator2D::precipitationCompute();
    if (isTempWG2D)
        weatherGenerator2D::temperatureCompute();

    weatherGenerator2D::getWeatherGeneratorOutput();
}

void weatherGenerator2D::precipitationCompute()
{
   // step 1 of precipitation WG2D
   weatherGenerator2D::initializePrecipitationInternalArrays();
   weatherGenerator2D::initializePrecipitationOutputs(lengthSeason);
   weatherGenerator2D::precipitationP00P10(); // it computes the monthly probabilities p00 and p10
   printf("fase 1\n");
   // step 2 of precipitation WG2D
   weatherGenerator2D::precipitationCorrelationMatrices();
   printf("fase 2\n");
   // step 3 of precipitation WG2D
   weatherGenerator2D::precipitationMultisiteOccurrenceGeneration();
   printf("fase 3\n");
   if (isPrecWG2D)
   {
    // step 4 of precipitation WG2D
    weatherGenerator2D::precipitationMultiDistributionParameterization();
    printf("fase 4\n");
    // step 5 of precipitation WG2D
    weatherGenerator2D::precipitationMultisiteAmountsGeneration();
    printf("fase 5\n");
   }
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
                    }
                }
            }
        }

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

    /*for (int month=0;month<12;month++)
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
    srand (time(nullptr));


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
                //printf("%f \n",randomMatrix[iMonth].matrixOccurrences[i][j]);
            }
            //printf("\n");
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

void weatherGenerator2D::initializeOutputData()
{
    int length = 365*parametersModel.yearOfSimulation;
    outputWeatherData = (ToutputWeatherData*)calloc(nrStations, sizeof(ToutputWeatherData));
    for (int iStation=0;iStation<nrStations;iStation++)
    {
        outputWeatherData[iStation].yearSimulated = (int*)calloc(length, sizeof(int));
        outputWeatherData[iStation].monthSimulated = (int*)calloc(length, sizeof(int));
        outputWeatherData[iStation].daySimulated = (int*)calloc(length, sizeof(int));
        outputWeatherData[iStation].doySimulated = (int*)calloc(length, sizeof(int));
        if (isTempWG2D)
        {
            outputWeatherData[iStation].maxT = (double*)calloc(length, sizeof(double));
            outputWeatherData[iStation].minT = (double*)calloc(length, sizeof(double));
        }
        if (isPrecWG2D)
            outputWeatherData[iStation].precipitation = (double*)calloc(length, sizeof(double));
    }
    // = (double*)calloc(lengthOfRandomSeries, sizeof(double));
}

void weatherGenerator2D::getWeatherGeneratorOutput()
{
    int counter;
    int counterSeason[4];
    int day,month;
    weatherGenerator2D::initializeOutputData();
    for (int iStation=0;iStation<nrStations;iStation++)
    {
        counter = 0;
        counterSeason[3] = counterSeason[2] = counterSeason[1] = counterSeason[0] = 0;
        for (int iYear=0;iYear<parametersModel.yearOfSimulation;iYear++)
        {
            for (int iDoy=0; iDoy<365; iDoy++)
            {
                outputWeatherData[iStation].yearSimulated[counter] = iYear;
                outputWeatherData[iStation].doySimulated[counter] = iDoy+1;
                weatherGenerator2D::dateFromDoy(outputWeatherData[iStation].doySimulated[counter],1,&day,&month);
                outputWeatherData[iStation].daySimulated[counter] = day;
                outputWeatherData[iStation].monthSimulated[counter] = month;
                if (isTempWG2D)
                {
                    outputWeatherData[iStation].maxT[counter] = maxTGenerated[counter][iStation];
                    outputWeatherData[iStation].minT[counter] = minTGenerated[counter][iStation];
                }
                if (isPrecWG2D)
                {
                    //simulatedPrecipitationAmounts[iSeason].matrixAmounts[i][j]
                    int iSeason;
                    if (month == 1 || month == 2 || month == 12) iSeason = 0;
                    else if (month == 3 || month == 4 || month == 5) iSeason = 1;
                    else if (month == 6 || month == 7 || month == 8) iSeason = 2;
                    else iSeason = 3;
                    outputWeatherData[iStation].precipitation[counter] = simulatedPrecipitationAmounts[iSeason].matrixAmounts[iStation][counterSeason[iSeason]];
                    (counterSeason[iSeason])++;
                }
                counter++;
            }
        }
    }
}

int weatherGenerator2D::dateFromDoy(int doy,int year, int* day, int* month)
{
    //int leap = 0;
    int daysOfMonth[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
    //int startingDayOfMonth[12];
    if (isLeapYear(year)) (daysOfMonth[1])++;
    int counter = 0;
    while(doy > daysOfMonth[counter])
    {
        doy -= daysOfMonth[counter];
        counter++;
        if (counter >= 12) return PARAMETER_ERROR;
    }
    *day = doy;
    //counter++;
    *month = ++counter;


    return 0;
}

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

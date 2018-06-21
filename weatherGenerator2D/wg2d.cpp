

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

#include "wg2D.h"
#include "commonConstants.h"
#include "furtherMathFunctions.h"
#include "statistics.h"
#include "eispack.h"

//precipitation prec;
//temperature temp;

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
   // step 1 of precipitation WG2D
   weatherGenerator2D::precipitationP00P10(); // computes the monthly probabilities p00 and p10
   // step 2 of precipitation WG2D
   weatherGenerator2D::precipitationCorrelationMatrices();
   // step 3 of precipitation WG2D
   weatherGenerator2D::precipitationMultisiteOccurrenceGeneration();
   // step 4 of precipitation WG2D
   weatherGenerator2D::precipitationMultiDistributionAmounts();
   // step 5 of precipitation WG2D
}


void weatherGenerator2D::precipitationP00P10()
{
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
            precOccurence[idStation][month].p00 =(float)((1.0*occurrence00[month])/daysWithoutRain[month]);
            precOccurence[idStation][month].p10 =(float)((1.0*occurrence10[month])/daysWithRain[month]);
            precOccurence[idStation][month].month = month +1;
        }
    }

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
                amount.meanValueMonthlyPrec1=0.;
                amount.meanValueMonthlyPrec2=0.;
                amount.covariance = amount.variance1 = amount.variance2 = 0.;
                occurrence.meanValueMonthlyPrec1=0.;
                occurrence.meanValueMonthlyPrec2=0.;
                occurrence.covariance = occurrence.variance1 = occurrence.variance2 = 0.;

                for (int k=0; k<nrData;k++) // compute the monthly means
                {
                    if (obsDataD[j][k].date.month == (iMonth+1) && obsDataD[i][k].date.month == (iMonth+1))
                    {
                        if ((obsDataD[j][k].prec != NODATA) && (obsDataD[i][k].prec != NODATA))
                        {
                            counter++;
                            if (obsDataD[j][k].prec > parametersModel.precipitationThreshold)
                            {
                                amount.meanValueMonthlyPrec1 += obsDataD[j][k].prec;
                                occurrence.meanValueMonthlyPrec1++ ;
                            }
                            if (obsDataD[i][k].prec > parametersModel.precipitationThreshold)
                            {
                                amount.meanValueMonthlyPrec2 += obsDataD[i][k].prec;
                                occurrence.meanValueMonthlyPrec2++ ;
                            }

                        }
                    }

                }
                if (counter != 0)
                {
                    amount.meanValueMonthlyPrec1 /= counter;
                    occurrence.meanValueMonthlyPrec1 /= counter;
                }

                if (counter != 0)
                {
                    amount.meanValueMonthlyPrec2 /= counter;
                    occurrence.meanValueMonthlyPrec2 /= counter;
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

                            amount.covariance += (value1 - amount.meanValueMonthlyPrec1)*(value2 - amount.meanValueMonthlyPrec2);
                            amount.variance1 += (value1 - amount.meanValueMonthlyPrec1)*(value1 - amount.meanValueMonthlyPrec1);
                            amount.variance2 += (value2 - amount.meanValueMonthlyPrec2)*(value2 - amount.meanValueMonthlyPrec2);


                            if (obsDataD[j][k].prec <= parametersModel.precipitationThreshold) value1 = 0.;
                            else value1 = 1.;
                            if (obsDataD[i][k].prec <= parametersModel.precipitationThreshold) value2 = 0.;
                            else value2 = 1.;

                            occurrence.covariance += (value1 - occurrence.meanValueMonthlyPrec1)*(value2 - occurrence.meanValueMonthlyPrec2);
                            occurrence.variance1 += (value1 - occurrence.meanValueMonthlyPrec1)*(value1 - occurrence.meanValueMonthlyPrec1);
                            occurrence.variance2 += (value2 - occurrence.meanValueMonthlyPrec2)*(value2 - occurrence.meanValueMonthlyPrec2);


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
    //printf("%d\n",nrStations);
    /*
     * for (int i=0;i<nrStations;i++)
    {
        for (int j=0;j<nrStations;j++)
        {

            printf("%f ",correlationMatrix[3].amount[i][j] );
        }
        printf("\n");
    }
    for (int i=0;i<nrStations;i++)
    {
        for (int j=0;j<nrStations;j++)
        {

            printf("%f ",correlationMatrix[3].occurrence[i][j] );
        }
        printf("\n");
    }*/
}

void weatherGenerator2D::precipitationMultisiteOccurrenceGeneration()
{
    int nrDaysIterativeProcessMonthly[12];
    int gasDevIset = 0;
    float gasDevGset = 0;
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
            randomMatrix[iMonth].matrixK = (float**)calloc(nrStations, sizeof(float*));
            randomMatrix[iMonth].matrixM = (float**)calloc(nrStations, sizeof(float*));
            randomMatrix[iMonth].matrixOccurrences = (float**)calloc(nrStations, sizeof(float*));

            for (int i=0;i<nrStations;i++)
            {
                randomMatrix[iMonth].matrixK[i]= (float*)calloc(nrStations, sizeof(float));
                randomMatrix[iMonth].matrixM[i]= (float*)calloc(nrStations, sizeof(float));
                randomMatrix[iMonth].matrixOccurrences[i]= (float*)calloc(nrDaysIterativeProcessMonthly[iMonth], sizeof(float));
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
                matrixOccurrence[i][j]= correlationMatrix[iMonth].occurrence[i][j];
                //printf("%f,",matrixOccurrence[i][j]);
            }
            //printf("month %d,\n",iMonth);

            /* since random numbers generated have a normal distribution, each p00 and
               p10 have to be recalculated according to a normal number*/
            normalizedTransitionProbability[i][0]= - (SQRT_2*(statistics::inverseERFC(2*precOccurence[i][iMonth].p00,0.0001)));
            normalizedTransitionProbability[i][1]= - (SQRT_2*(statistics::inverseERFC(2*precOccurence[i][iMonth].p10,0.0001)));
            for (int j=0;j<nrDaysIterativeProcessMonthly[iMonth];j++)
            {
               normalizedRandomMatrix[i][j]= myrandom::normalRandom(&gasDevIset,&gasDevGset);               
            }


        }

        // initialization outputs of weatherGenerator2D::spatialIterationOccurrence
        double** M;
        double** K;
        double** occurrences;
        M = (double **)calloc(nrStations, sizeof(double*));
        K = (double **)calloc(nrStations, sizeof(double*));
        //occurrences = (double **)calloc(nrDaysIterativeProcessMonthly[iMonth], sizeof(double*));
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
/*
        for (int i=0;i<nrStations;i++)
        {
            for (int j=0;j<nrDaysIterativeProcessMonthly[iMonth];j++)
            {
                printf("%.0f\t",occurrences[i][j]);
            }
            printf("\n");
        }
        getchar();
*/

        for (int i=0;i<nrStations;i++)
        {
            for (int j=0;j<nrStations;j++)
            {
                randomMatrix[iMonth].matrixK[i][j] = float(K[i][j]);
                randomMatrix[iMonth].matrixM[i][j] = float(M[i][j]);
            }
            for (int j=0;j<nrDaysIterativeProcessMonthly[iMonth];j++)
            {
                randomMatrix[iMonth].matrixOccurrences[i][j]= float(occurrences[i][j]);
            }
        }
        randomMatrix[iMonth].month = iMonth + 1;
        // free memory
        for (int i=0;i<nrStations;i++)
        {
            free(normalizedRandomMatrix[i]);
            free(M[i]);
            free(K[i]);
            occurrences[i];

        }
        //for (int i=0;i<nrDaysIterativeProcessMonthly[iMonth];i++) free(occurrences[i]);
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
    /*for (int i=0;i<lengthSeries;i++)
    {
        correlationRandom[i]= (double*)calloc(nrStations, sizeof(double));
    }*/



    // initialization output M
    for (int i=0;i<nrStations;i++)
    {
       for (int j=0;j<nrStations;j++)
        {
            M[i][j] = matrixOccurrence[i][j];  // M is the matrix named mat in the original code
        }
    }

    while ((val>TOLERANCE_MULGETS) && (ii<MAX_ITERATION_MULGETS))
    {
        ii++;
        int nrEigenvaluesLessThan0 = 0;
        int counter = 0;
        for (int i=0;i<nrStations;i++)
        {
            for (int j=0;j<nrStations;j++) // avoid solutions with correlation coefficient greater than 1
            {
                M[i][j] = min_value(M[i][j],1);
                correlationArray[counter] = M[i][j];
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
            matricial::matrixProduct(dummyMatrix,dummyMatrix2,nrStations,nrStations,nrStations,nrStations,M);
        }
        // the matrix called M is the final matrix exiting from the calculation
        for (int i=0;i<nrStations;i++)
            for (int j=0;j<nrStations;j++) dummyMatrix[i][j] = M[i][j];

        matricial::choleskyDecompositionTriangularMatrix(dummyMatrix,nrStations,true);
        //printf("Cholesky\n");
        matricial::matrixProduct(dummyMatrix,normalizedMatrixRandom,nrStations,nrStations,lengthSeries,nrStations,dummyMatrix3);
        //printf("prodottoMatriciale\n");
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
                if(occurrences[i][j-1] == 0)
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
                val = max_value(val,fabs(K[i][j] - matrixOccurrence[i][j]));
            }
        }
        //printf("%f \n",val);
        //getchar();
        if ((ii != MAX_ITERATION_MULGETS) && (val > TOLERANCE_MULGETS))
        {
            for (int i=0; i<nrStations;i++)
            {
                M[i][i]= 1.;
                for (int j=i+1;j<nrStations;j++)
                {
                    M[i][j] += kiter*(matrixOccurrence[i][j]-K[i][j]);
                    M[j][i] = M[i][j];
                }
            }
        }
    } // end of the while cycle
    //printf("press return to continue");
    //getchar();
    //printf("%d\n",ii);



    // free memory

    for (int i=0;i<nrStations;i++)
    {
        free(dummyMatrix[i]);
        free(dummyMatrix2[i]);
        free(dummyMatrix3[i]);
        free(normRandom[i]);
    }
    free(normRandom);
    //free(dummyArray);
    free(dummyMatrix);
    free(dummyMatrix2);
    free(dummyMatrix3);
    free(eigenvalues);
    free(eigenvectors);
    free(correlationArray);

}

void weatherGenerator2D::precipitationMultiDistributionAmounts()
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
            beginYear = min_value(beginYear,obsDataD[i][j].date.year);
            endYear =  max_value(endYear,obsDataD[i][j].date.year);
        }
    }
    // create the seasonal correlation matrices
    double** occurrenceMatrixSeasonDJF = (double **)calloc(nrStations, sizeof(double*));
    double** occurrenceMatrixSeasonMAM = (double **)calloc(nrStations, sizeof(double*));
    double** occurrenceMatrixSeasonJJA = (double **)calloc(nrStations, sizeof(double*));
    double** occurrenceMatrixSeasonSON = (double **)calloc(nrStations, sizeof(double*));

    double** wDJF = (double **)calloc(nrStations, sizeof(double*));
    double** wMAM = (double **)calloc(nrStations, sizeof(double*));
    double** wJJA = (double **)calloc(nrStations, sizeof(double*));
    double** wSON = (double **)calloc(nrStations, sizeof(double*));
    double** wSeason = (double **)calloc(nrStations, sizeof(double*));



    //int lengthDJF,lengthMAM,lengthJJA,lengthSON;
    int lengthSeason[4];
    lengthSeason[0] = lengthMonth[11]+lengthMonth[0]+lengthMonth[1];
    lengthSeason[1] = lengthMonth[2]+lengthMonth[3]+lengthMonth[4];
    lengthSeason[2] = lengthMonth[5]+lengthMonth[6]+lengthMonth[7];
    lengthSeason[3] = lengthMonth[8]+lengthMonth[9]+lengthMonth[10];

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

    for (int i=0;i<nrStations;i++)
    {
        int counterMonth = 11;
        int nrDaysOfMonth = 0;
        //printf("%d\n",lengthDJF);
        for (int j=0;j<lengthSeason[0]*parametersModel.yearOfSimulation;j++)
        {

                occurrenceMatrixSeasonDJF[i][j] = randomMatrix[counterMonth].matrixOccurrences[i][nrDaysOfMonth];
                //printf("%d,%d,%d",j,counterMonth,nrDaysOfMonth);
                //printf("%f\n",correlationMatrixSeasonDJF[i][j]);
                //getchar();
                nrDaysOfMonth++;
                if (nrDaysOfMonth >= lengthMonth[counterMonth]*parametersModel.yearOfSimulation)
                {
                    counterMonth++;
                    counterMonth = counterMonth%12;
                    nrDaysOfMonth = 0;
                }

        }
        //getchar();
    }

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


    for (int i=0;i<nrStations;i++)
    {
        wDJF[i]= (double *)calloc(nrStations, sizeof(double));
        wMAM[i]= (double *)calloc(nrStations, sizeof(double));
        wJJA[i]= (double *)calloc(nrStations, sizeof(double));
        wSON[i]= (double *)calloc(nrStations, sizeof(double));
        wSeason[i]= (double *)calloc(nrStations, sizeof(double));
    }
    statistics::correlationsMatrix(nrStations,occurrenceMatrixSeasonDJF,lengthSeason[0]*parametersModel.yearOfSimulation,wDJF);
    statistics::correlationsMatrix(nrStations,occurrenceMatrixSeasonMAM,lengthSeason[1]*parametersModel.yearOfSimulation,wMAM);
    statistics::correlationsMatrix(nrStations,occurrenceMatrixSeasonJJA,lengthSeason[2]*parametersModel.yearOfSimulation,wJJA);
    statistics::correlationsMatrix(nrStations,occurrenceMatrixSeasonSON,lengthSeason[3]*parametersModel.yearOfSimulation,wSON);



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
               if  (obsPrecDataD[i][j].prec < parametersModel.precipitationThreshold)
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
    /*
    for (int i=0;i<nrData;i++)
    {
        for (int j=0;j<nrStations;j++)
        {
            printf("site %d,%.1f,%.0f\n",j,obsPrecDataD[j][i].amounts,obsPrecDataD[j][i].occurrences);
        }
        getchar();
    }
    */
    // create the array seasonal amounts
    /*
    TseasonalMatrix* seasonalMatrix;
    seasonalMatrix = (TseasonalMatrix*)calloc(4, sizeof(TseasonalMatrix));
    int lengthSeasonTimesYearsOfObservations[4] ;

    for (int i=0;i<4;i++)
    {
       lengthSeasonTimesYearsOfObservations[i]= lengthSeason[i]*(endYear - beginYear);
       seasonalMatrix[i].amount= (double**)calloc(lengthSeasonTimesYearsOfObservations[i], sizeof(double*));
    }

    for (int i=0;i<4;i++)
    {
        for (int j=0;j<lengthSeasonTimesYearsOfObservations[i];j++)
        {
            seasonalMatrix[i].amount[j]= (double*)calloc(nrStations, sizeof(double));
        }

    }

    for (int i=0;i<4;i++)
    {
        for (int j=0;j<lengthSeasonTimesYearsOfObservations[i];j++)
        {
            for (int k=0;k<nrStations;k++)
            {
                seasonalMatrix[i].amount[j][k] = NODATA;
            }
        }
    }
    int counterData = 0;
    int currentMonth = 11;
    for (int i=0;i<4;i++)
    {

        for (int j=0;j<lengthSeasonTimesYearsOfObservations[i];j++)
        {
            for(int k=0;k<nrStations;k++)
            {
                counterData = 0;
                while (counterData < nrData)
                {
                    if (obsPrecDataD[k][counterData].date.month == (currentMonth%12 +1) || obsPrecDataD[k][counterData].date.month == (currentMonth%12 +2) || obsPrecDataD[k][counterData].date.month == (currentMonth%12 +3))
                    {
                        if (obsPrecDataD[k][counterData].date.day != 29 && obsPrecDataD[k][counterData].date.month == 2)
                            seasonalMatrix[i].amount[j][k] = obsPrecDataD[k][counterData].amountsLessThreshold;
                    }
                    counterData++;
                }
            }
        }
        currentMonth = currentMonth + 3;
        currentMonth = currentMonth%12;
    }
    */

    double*** moran; // coefficient for each station, each season, each nr of days of simulation in the season.
    moran = (double***)calloc(nrStations, sizeof(double**));
    double*** rainfallLessThreshold; // coefficient for each station, each season, each nr of days of simulation in the season.
    rainfallLessThreshold = (double***)calloc(nrStations, sizeof(double**));


    for (int ijk=0;ijk<nrStations;ijk++)
    {
        int idStation = ijk;
        // determine how many observed points per season
        int numberObservedDJF,numberObservedMAM,numberObservedJJA,numberObservedSON;
        int numberObservedMax;
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
        numberObservedMax = max_value(numberObservedDJF,max_value(numberObservedMAM,max_value(numberObservedJJA,numberObservedSON)));

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
            //double dummy1,dummy2;
            //dummy1 = dummy2 = 0;
            //printf("stagione %d\n",qq);
            //printf("%d %d %d %d\n",numberObservedDJF,numberObservedMAM,numberObservedJJA,numberObservedSON);
            double rainCumulated, moranCumulated;
            rainCumulated = moranCumulated = 0;

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
                            if ((iStations != ijk) && (obsPrecDataD[iStations][i].occurrences != NODATA))
                            {
                                  numeratorMoran += obsPrecDataD[iStations][i].occurrences*wSeason[ijk][iStations];
                                  denominatorMoran += wSeason[ijk][iStations];
                                  //printf("precipitazione sito %f prec atre %f\n",obsPrecDataD[ijk][i].amounts,obsPrecDataD[iStations][i].amounts);
                                  //printf(" %f   %f\n",obsPrecDataD[iStations][i].occurrences*wSeason[ijk][iStations],wSeason[ijk][iStations]);
                            }
                        }
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
                    //printf("%f\n",moran[ijk][qq][counterData]);

                    counterData++;
                }


            }
            int lengthBins = 11;
            double bins[11],bincenter[10];
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


            double bins2[11];
            int counter = 1;

            for (int i=1;i<11;i++)
                bins2[i] = NODATA;
            bins2[0]= 0;
            int nrMinimalPointsForBins = 5;
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

            if (nrBins[10] < nrMinimalPointsForBins)
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
               printf("dopo %d %f %f\n",i,bins[i],bincenter[i]);
            }*/
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
            parMin[1]= -10;
            parMax[1]= 30.;
            parDelta[1] = 0.0001;
            par[1] = (parMin[1]+parMax[1])*0.5;
            parMin[2]= 2;
            parMax[2]= 20.;
            parDelta[2] = 0.01;
            par[2] = (parMin[2]+parMax[2])*0.5;

            maxIterations = 1000000;
            epsilon = 0.0001;
            functionCode = TWOPARAMETERSPOLYNOMIAL;
            int nrBincenter=0;
            for (int i=0;i<(lengthBins-1);i++)
            {
                if(bincenter[i] != NODATA)
                    nrBincenter++;
            }
            double* meanP = (double *) calloc(nrBincenter, sizeof(double));
            double* stdDevP = (double *) calloc(nrBincenter, sizeof(double));
            double* binCenter = (double *) calloc(nrBincenter, sizeof(double));
            for (int i=0;i<nrBincenter;i++)
            {
               meanP[i]=Pmean[i];
               stdDevP[i]=PstdDev[i];
               binCenter[i]= bincenter[i];
            }
            for (int i=0;i<nrBincenter;i++)
            {
                printf("prima %f %f\n",binCenter[i],meanP[i]);
            }
            //for (int i=0;i<3;i++)
                //printf("prima %f\n",par[i]);

            interpolation::fittingMarquardt(parMin,parMax,par,nrPar,parDelta,maxIterations,epsilon,functionCode,binCenter,nrBincenter,meanP);
            // con marquardt stimo già tutti i parametri compreso l'esponente quindi il ciclo
            // for da 2 a 20 (presente nel codice originale) risulta inutile nel codice tradotto
            for (int i=0;i<nrBincenter;i++)
            {
                meanP[i] = par[0]+par[1]* powf(binCenter[i],par[2]);
            }
            //for (int i=0;i<nrBincenter;i++)
            //{
               //printf("prima %f %f\n",binCenter[i],par[0]+par[1]*powf(binCenter[i],par[2]));
            //}
            //for (int i=0;i<3;i++)
                //printf("dopo %f\n",par[i]);

            if (parametersModel.distributionPrecipitation == 2)
            {
                interpolation::fittingMarquardt(parMin,parMax,par,nrPar,parDelta,maxIterations,epsilon,functionCode,binCenter,nrBincenter,stdDevP);
                for (int i=0;i<nrBincenter;i++)
                {
                    stdDevP[i] = par[0]+par[1]* powf(binCenter[i],par[2]);
                }
            }

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
            printf("moran %d moranSenzaNODATA %d \n",parametersModel.yearOfSimulation*lengthSeason[qq],indexMoranArrayPrec);
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
                printf("%d\t",nrBins[i]);
            }
            printf("\n totale %d \n",nrTotal);
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

            //printf("\n media precipitazione al netto soglia %f \n",precipitationMean);
            //printf("\n numero bin center %d \n",nrBincenter);
            double correctionFactor = 0.;
            double cumulatedWeight=0;
            for(int i=0; i<nrBincenter ;i++)
            {
                cumulatedWeight += frequencyBins[i]*meanP[i];
            }
            correctionFactor = precipitationMean / cumulatedWeight;

            double* meanFit = (double *) calloc(nrBincenter, sizeof(double));
            double* lambda = (double *) calloc(nrBincenter, sizeof(double));
            //double* lambda2 = (double *) calloc(nrBincenter, sizeof(double));
            for(int i=0; i<nrBincenter ;i++)
            {
                meanFit[i]= meanP[i];
                meanP[i] *= correctionFactor;
                lambda[i] = 1.0/meanP[i];
                //lambda2[i] = 1.0/meanFit[i];
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
            free(meanP);
            free(stdDevP);
            free(binCenter);
            free(meanFit);
            free(lambda);
        }

    }



    // free the memory

    for (int i=0;i<nrStations;i++)
    {

        free(occurrenceMatrixSeasonJJA[i]);
        free(occurrenceMatrixSeasonDJF[i]);
        free(occurrenceMatrixSeasonMAM[i]);
        free(occurrenceMatrixSeasonSON[i]);
        for (int qq=0;qq<4;qq++)
        {
            free(moran[i][qq]);
            free(rainfallLessThreshold[i][qq]);
        }
        free(moran[i]);
        free(rainfallLessThreshold[i]);
    }
    free(occurrenceMatrixSeasonDJF);
    free(occurrenceMatrixSeasonJJA);
    free(occurrenceMatrixSeasonMAM);
    free(occurrenceMatrixSeasonSON);
    free(moran);
    free(rainfallLessThreshold);

    for (int i=0;i<nrStations;i++)
    {
        free(wDJF[i]);
        free(wJJA[i]);
        free(wMAM[i]);
        free(wSON[i]);
        free(wSeason[i]);
    }
    free(wDJF);
    free(wJJA);
    free(wMAM);
    free(wSON);
    free(wSeason);

}


void weatherGenerator2D::temperatureCompute()
{
    // step 1 of temperature WG2D
    // step 2 of temperature WG2D
    // step 3 of temperature WG2D
    // step 4 of temperature WG2D
    // step 5 of temperature WG2D
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

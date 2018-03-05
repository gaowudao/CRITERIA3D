

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
   // step 1 of precipitation WG2D
   weatherGenerator2D::precipitationP00P10(); // computes the monthly probabilities p00 and p10
   // step 2 of precipitation WG2D
   weatherGenerator2D::precipitationCorrelationMatrices();
   // step 3 of precipitation WG2D
   weatherGenerator2D::precipitationMultisiteOccurrenceGeneration();


   // step 4 of precipitation WG2D
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
                            amount.variance1 += pow((value1 - amount.meanValueMonthlyPrec1),2);
                            amount.variance2 += pow((value2 - amount.meanValueMonthlyPrec2),2);


                            if (obsDataD[j][k].prec <= parametersModel.precipitationThreshold) value1 = 0.;
                            else value1 = 1.;
                            if (obsDataD[i][k].prec <= parametersModel.precipitationThreshold) value2 = 0.;
                            else value2 = 1.;

                            occurrence.covariance += (value1 - occurrence.meanValueMonthlyPrec1)*(value2 - occurrence.meanValueMonthlyPrec2);
                            occurrence.variance1 += pow((value1 - occurrence.meanValueMonthlyPrec1),2);
                            occurrence.variance2 += pow((value2 - occurrence.meanValueMonthlyPrec2),2);


                        }
                    }
                }
                correlationMatrix[iMonth].amount[j][i]= amount.covariance / pow((amount.variance1*amount.variance2),0.5);
                correlationMatrix[iMonth].amount[i][j] = correlationMatrix[iMonth].amount[j][i];
                correlationMatrix[iMonth].occurrence[j][i]= occurrence.covariance / pow((occurrence.variance1*occurrence.variance2),0.5);
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
    float** normalizedTransitionProbability;
    normalizedTransitionProbability = (float **)calloc(nrStations, sizeof(float*));

    for (int i=0;i<nrStations;i++)
    {
        matrixOccurrence[i] = (double *)calloc(nrStations, sizeof(double));
        normalizedTransitionProbability[i]= (float *)calloc(2, sizeof(float));
        for (int j=0;j<nrStations;j++)
        {
           matrixOccurrence[i][j]= NODATA;
        }
        normalizedTransitionProbability[i][0]= NODATA;
        normalizedTransitionProbability[i][1]= NODATA;        
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
            normalizedTransitionProbability[i][0]= - (float)(SQRT_2*(float)(statistics::inverseERFC(2*precOccurence[i][iMonth].p00,0.0001)));
            normalizedTransitionProbability[i][1]= - (float)(SQRT_2*(float)(statistics::inverseERFC(2*precOccurence[i][iMonth].p10,0.0001)));
            for (int j=0;j<nrDaysIterativeProcessMonthly[iMonth];j++)
            {
               normalizedRandomMatrix[i][j]= myrandom::normalRandom(&gasDevIset,&gasDevGset);
            }


        }

        // initialization outputs of weatherGenerator2D::spatialIterationOccurrence
        double** M;
        float** K;
        float** occurrences;
        M = (double **)calloc(nrStations, sizeof(double*));
        K = (float **)calloc(nrStations, sizeof(float*));
        //occurrences = (double **)calloc(nrDaysIterativeProcessMonthly[iMonth], sizeof(double*));
        occurrences = (float **)calloc(nrStations, sizeof(float*));

        for (int i=0;i<nrStations;i++)
        {
            M[i] = (double *)calloc(nrStations, sizeof(double));
            K[i] = (float *)calloc(nrStations, sizeof(float));
            for (int j=0;j<nrStations;j++)
            {
                M[i][j]= NODATA;
                K[i][j]= NODATA;

            }

        }
        for (int i=0;i<nrStations;i++)
        {
          occurrences[i] = (float *)calloc(nrDaysIterativeProcessMonthly[iMonth], sizeof(float));
          for (int j=0;j<nrDaysIterativeProcessMonthly[iMonth];j++)
          {
              occurrences[i][j]= NODATA;
          }
        }

        weatherGenerator2D::spatialIterationOccurrence(M,K,occurrences,matrixOccurrence,normalizedRandomMatrix,normalizedTransitionProbability,nrDaysIterativeProcessMonthly[iMonth]);


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

void weatherGenerator2D::spatialIterationOccurrence(double ** M, float** K,float** occurrences, double** matrixOccurrence, double** normalizedMatrixRandom,float ** transitionNormal,int lengthSeries)
{
    // M and K matrices are used as ancillary dummy matrices
    double val=5;
    int ii=0;
    double kiter=0.1;   // iteration parameter in calculation of new estimate of matrix 'mat'
    double* eigenvalues =(double*)calloc(nrStations, sizeof(double));
    double* eigenvectors =(double*)calloc(nrStations*nrStations, sizeof(double));
    double* correlationArray =(double*)calloc(nrStations*nrStations, sizeof(double));
    float* dummyArray =(float*)calloc(lengthSeries, sizeof(float));
    double** dummyMatrix = (double**)calloc(nrStations, sizeof(double*));
    double** dummyMatrix2 = (double**)calloc(nrStations, sizeof(double*));
    double** dummyMatrix3 = (double**)calloc(nrStations, sizeof(double*));
    //double** correlationRandom = (double**)calloc(lengthSeries, sizeof(double*));

    // initialization internal arrays
    for (int i=0;i<nrStations;i++)
    {
        dummyMatrix[i]= (double*)calloc(nrStations, sizeof(double));
        dummyMatrix2[i]= (double*)calloc(nrStations, sizeof(double));

    }
    for (int i=0;i<nrStations;i++)
    {
        dummyMatrix3[i]= (double*)calloc(lengthSeries, sizeof(double));
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
            M[i][j] = matrixOccurrence[i][j];
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
            }
        }
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
                    dummyMatrix[i][j]= eigenvectors[counter];
                    dummyMatrix2[j][i] = dummyMatrix[i][j] * eigenvalues[j]; //product of diagonal matrix times transposed eigenvectors matrix!!!!
                    //if (i != j) M[i][j]= 0.;
                    //else M[i][i]= eigenvalues[i];
                    counter++;
                }
            }
            //matricial::matrixProduct(M,dummyMatrix,nrStations,nrStations,nrStations,nrStations,dummyMatrix2);
            //matricial::transposedSquareMatrix(dummyMatrix,nrStations);
            matricial::matrixProduct(dummyMatrix,dummyMatrix2,nrStations,nrStations,nrStations,nrStations,M);
        }
        // the matrix called M is the final matrix exiting from the calculation
        for (int i=0;i<nrStations;i++)
            for (int j=0;j<nrStations;j++) dummyMatrix[i][j] = M[i][j];

        matricial::choleskyDecompositionTriangularMatrix(dummyMatrix,nrStations,true);
        matricial::matrixProduct(dummyMatrix,normalizedMatrixRandom,nrStations,nrStations,lengthSeries,nrStations,dummyMatrix3);

        for (int i=0;i<nrStations;i++)
        {
            for (int j=0;j<lengthSeries;j++) dummyArray[j]= (float)(dummyMatrix3[i][j]);
            for (int j=0;j<lengthSeries;j++)
            {
                normalizedMatrixRandom[i][j]= (dummyMatrix3[i][j]-statistics::mean(dummyArray,lengthSeries))/statistics::standardDeviation(dummyArray,lengthSeries);
            }
        }
        // initialize occurrence to 0

        for (int i=0;i<nrStations;i++)
        {
            for (int j=0;j<lengthSeries;j++)
            {
                occurrences[i][j]= 0.;
            }
        }

        for (int i=0;i<nrStations;i++)
        {
            for (int j=1;j<lengthSeries;j++)
            {
                if(occurrences[i][j-1] == 0)
                {
                    if(normalizedMatrixRandom[i][j]> transitionNormal[i][0]) occurrences[i][j] = 1;
                }
                else
                {
                    if(normalizedMatrixRandom[i][j]> transitionNormal[i][1]) occurrences[i][j] = 1;
                }
            }
        }
        statistics::correlationsMatrix(nrStations,occurrences,lengthSeries,K);





    } // end of the while cycle

    printf("%d\n",lengthSeries);



    // free memory

    for (int i=0;i<nrStations;i++)
    {
        free(dummyMatrix[i]);
        free(dummyMatrix2[i]);
        free(dummyMatrix3[i]);
    }
    //for (int i=0;i<lengthSeries;i++) free(correlationRandom[i]);
    free(dummyArray);
    free(dummyMatrix);
    free(dummyMatrix2);
    free(dummyMatrix3);
    //free(correlationRandom);
    free(eigenvalues);
    free(eigenvectors);
    free(correlationArray);

}


void weatherGenerator2D::temperatureCompute()
{
    // step 1 of temperature WG2D
    // step 2 of temperature WG2D
    // step 3 of temperature WG2D
    // step 4 of temperature WG2D
    // step 5 of temperature WG2D
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

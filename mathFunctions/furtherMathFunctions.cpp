/*!
    \copyright 2016 Fausto Tomei, Gabriele Antolini,
    Alberto Pistocchi, Marco Bittelli, Antonio Volta, Laura Costantini

    This file is part of CRITERIA3D.
    CRITERIA3D has been developed under contract issued by A.R.P.A. Emilia-Romagna

    CRITERIA3D is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    CRITERIA3D is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with CRITERIA3D.  If not, see <http://www.gnu.org/licenses/>.

    Contacts:
    Antonio Volta  avolta@arpae.it
*/

#include <math.h>
#include <malloc.h>
#include <stdlib.h>
#include <iostream>

#include "commonConstants.h"
#include "furtherMathFunctions.h"


float blackBodyShape(TfunctionInput fInput)
{
    float b, y;
    b = fInput.par[0];
    y = b*(float)(pow(fInput.x, 3)*(1. / (exp(b*fInput.x)-0.99)));
    return (y);
}
float errorFunctionPrimitive(float x)
{
    return expf(-x*x);
}

/*float straightLine(TfunctionInput fInput)
{
    float m,q,y;
    m = fInput.par[0];
    q = fInput.par[1];
    y = m*fInput.x + q;
    return(y);
}
*/

namespace integration
{
    /*! this is a set for function integrations by means of the Simpson */

    float qsimpParametric(float (*func)(TfunctionInput), int nrPar, float *par,float a , float b , float EPS)
    {
        /*! this function calculates definte integrals using the Simpson rule */
        if (a > b)
        {
            return (-qsimpParametric(func,nrPar, par , b, a , EPS)); //recursive formula
        }
        float old_s [10] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
        //void nerror(char error_text[]);
        int j;
        float s , st , ost=0.0 , os = 0.0 ;
        float s1 = 0.;
        for ( j=1 ; j <= 20 ; j++)
        {
            st = trapzdParametric(func,nrPar,par,a,b,j) ;
            s = (float)((4.0*st-ost)/3.0) ;
            for ( short k=1 ; k < 10 ; k++)
            {
                old_s[k-1]=old_s[k];
            }
            old_s[9] = s ;
            if (j == 5) s1 = s ;
            if (j > 5 )
                if (fabs(s-os) < EPS*fabs(os) || (s == 0.0 && os == 0.0) ) return s ;
                os = s ;
                ost = st ;
        }
        float average_s=0.0 , average_s2 = 0.0 , variance ;
        for ( short k=0 ; k < 10 ; k++)
        {
            average_s  += old_s[k];
            average_s2 += powf(old_s[k],2) ;
        }
        average_s  /= 10.0 ;
        average_s2 /= 10.0 ;
        variance = average_s2 - powf(average_s,2) ;

        if (variance < 0.01*fabs(s1)) return s ; // s is converging slowly
        else return average_s ; // s ondulates
    }


    float trapzdParametric(float (*func)(TfunctionInput) , int nrPar, float *par , float a , float b , int n)
    {
        float x , tnm , sum , del ;
        static float s ;
        TfunctionInput functionInput ;
        if (nrPar > 0)
        {
            functionInput.par = (float *) calloc(nrPar, sizeof(float));
            functionInput.nrPar = nrPar ;
            for (int i = 0 ; i<nrPar ; i++)
            {
                functionInput.par[i]=par[i];
            }
        }

        else
        {
            functionInput.par = (float *) calloc(1, sizeof(float));
            functionInput.nrPar = 1 ;
            for (int i = 0 ; i<1 ; i++)
            {
                functionInput.par[i]=1;
            }
        }
        int it , j ;

        if (n == 1)
        {
            functionInput.x = a ;
            s = (float)(0.5*(b-a)*((*func)(functionInput))) ;
            functionInput.x = b ;
            s += (float)(0.5*(b-a)*((*func)(functionInput))) ;
            //return (s) ;
        }
        else
        {
            for (it = 1 , j = 1 ; j < n-1 ; j++ ) it <<= 1 ;
            tnm = (float)(it) ;
            del = (b-a) / tnm ;
            x = (float)(a + 0.5 * del) ;
            for(sum = 0.0 , j=1 ; j <= it ; j++ , x += del)
            {
                functionInput.x = x ;
                sum += (*func)(functionInput) ;
            }
            //s = (float)(0.5 * (s + (b-a)*sum/tnm)) ;
            s= (float)(0.5 * (s + (b-a)*sum/tnm)) ;
        }
        free(functionInput.par);
        return s;
    }

    float trapzd(float (*func)(float) , float a , float b , int n)
    {
        float x , tnm , sum , del ;
        static float s ;
        int it , j ;

        if (n == 1)
        {
            return (s=(float)(0.5*(b-a)* ((*func) (a) +(*func)(b)))) ;
        }
        else
        {
            for (it = 1 , j = 1 ; j < n-1 ; j++ ) it <<= 1 ;
            tnm = (float)(it) ;
            del = (b-a) / tnm ;
            x = (float)(a + 0.5 * del) ;
            for(sum = 0.0 , j=1 ; j <= it ; j++ , x += del) sum += (*func)(x) ;
            //s = (float)(0.5 * (s + (b-a)*sum/tnm)) ;
            return (s= (float)(0.5 * (s + (b-a)*sum/tnm))) ;
        }
    }


    float qsimp(float (*func)(float),float a , float b , float EPS)
    {
        /*! this function calculates definte integrals using the Simpson rule */
        if (a > b)
        {
            return (-qsimp(func,b, a , EPS)); //recursive formula
        }
        float old_s [10] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
        float trapzd(float (*func)(float) , float a , float b , int n) ;
        //void nerror(char error_text[]);
        int j;
        float s , st , ost=0.0 , os = 0.0 ;
        float s1 = 0.;
        for ( j=1 ; j <= 20 ; j++)
        {
            st = trapzd(func,a,b,j) ;
            s = (float)((4.0*st-ost)/3.0) ;
            for ( short k=1 ; k < 10 ; k++)
            {
                old_s[k-1]=old_s[k];
            }
            old_s[9] = s ;
            if (j == 5) s1 = s ;
            if (j > 5 )
                if (fabs(s-os) < EPS*fabs(os) || (s == 0.0 && os == 0.0) ) return s ;
                os = s ;
                ost = st ;
        }
        float average_s=0.0 , average_s2 = 0.0 , variance ;
        for ( short k=0 ; k < 10 ; k++)
        {
            average_s  += old_s[k];
            average_s2 += powf(old_s[k],2) ;
        }
        average_s  /= 10.0 ;
        average_s2 /= 10.0 ;
        variance = average_s2 - powf(average_s,2) ;
        //nerror("Too many steps in routine qsimp");
        if (variance < 0.01*fabs(s1)) return s ; // s is converging slowly
        else return average_s ; // s ondulates

    }
}


namespace interpolation
{
    float linearInterpolation (float x, float *xColumn , float *yColumn, int dimTable )
    {
        float *firstColumn = (float *) calloc(dimTable, sizeof(float));
        float *secondColumn = (float *) calloc(dimTable, sizeof(float));
        firstColumn = xColumn ;
        secondColumn = yColumn ;
        double slope , offset ;
        short stage=1;
        if (x < firstColumn[0]) return secondColumn[0] ;
        if (x > firstColumn[dimTable-1]) return secondColumn[dimTable-1];
        while (x > firstColumn[stage]) stage++ ;
        slope = (secondColumn[stage]- secondColumn[stage-1])/(firstColumn[stage] - firstColumn[stage-1]);
        offset = -firstColumn[stage-1]*slope + secondColumn[stage-1];
        free(firstColumn);
        free(secondColumn);
        return float(slope * x + offset) ;
    }

    float linearExtrapolation(double x3,double x1,double y1,double x2 , double y2)
    {
        double m,y3 ;
        m = (y1 - y2)/(x1-x2);
        y3 = m*(x3-x2) + y2 ;
        return float(y3) ;
    }


    float estimateFunction(int myFunctionCode, double *myParameters, double *myX)
    {
        /*
            '' spherical
            '' myParameters(0): range
            '' myParameters(1): nugget
            '' myParameters(2): sill
        */
        float output;
        switch (myFunctionCode)
        {
            case FUNCTION_CODE_SPHERICAL :
                double myTmp;
                if (myParameters[0] == 0)
                {
                    output = NODATA;
                    return output;
                }
                myTmp = (*myX) / myParameters[0];
                if ((*myX) < myParameters[0])
                    output = float(myParameters[1] + (myParameters[2] - myParameters[1]) * (1.5 * myTmp - 0.5 * myTmp * myTmp * myTmp));
                else
                    output = float(myParameters[2]);
                break;
            default:
                output = NODATA ;
        }
        return output;
    }


    double normGeneric(double *myParameters,double *myX, int nrMyX,double *myY,int myFunctionCode)
    {
        int i;
        float myError;
        double myEstimate;
        double output = 0;
        for (i = 0; i < nrMyX ; i++)
        {
            myEstimate = estimateFunction(myFunctionCode, myParameters, &(myX[i]));
            if (myEstimate == NODATA)
            {
                output = NODATA;
                return output;
            }
            myError = float(myY[i] - estimateFunction(myFunctionCode, myParameters,  &(myX[i])));
            output  += pow(myError,2) ;
        }
        return output;
    }

}


namespace sorting
{
    void quicksortAscendingInteger(int *x, int first,int last)
    {
       int pivot,j,temp,i;

        if(first<last){
            pivot=first;
            i=first;
            j=last;

            while(i<j){
                while(x[i]<=x[pivot]&&i<last)
                    i++;
                while(x[j]>x[pivot])
                    j--;
                if(i<j){
                    temp=x[i];
                     x[i]=x[j];
                     x[j]=temp;
                }
            }
            temp=x[pivot];
            x[pivot]=x[j];
            x[j]=temp;
            quicksortAscendingInteger(x,first,j-1);
            quicksortAscendingInteger(x,j+1,last);
       }
   }

    void quicksortAscendingDouble(double *x, int first,int last)
    {
       int pivot,j,i;
        double temp;

        if(first<last){
            pivot=first;
            i=first;
            j=last;

            while(i<j){
                while(x[i]<=x[pivot]&&i<last)
                    i++;
                while(x[j]>x[pivot])
                    j--;
                if(i<j){
                    temp=x[i];
                     x[i]=x[j];
                     x[j]=temp;
                }
            }
            temp=x[pivot];
            x[pivot]=x[j];
            x[j]=temp;
            quicksortAscendingDouble(x,first,j-1);
            quicksortAscendingDouble(x,j+1,last);
       }
   }

    void quicksortDescendingInteger(int *x, int first,int last)
    {
        int temp;
        quicksortAscendingInteger(x,first,last);
        //temp = x[first];
        for (int i = first ; i < (last/2) ; i++)
        {
            //swap
            temp = x[i];
            x[i]= x[last-i];
            x[last-i] = temp;
        }
    }

    double percentile(double* list, int* nList, double perc, bool sortValues)
    {
        // check
        if (*nList == 0 || perc <= 0.0 || perc >= 100.0)
            return (NODATA);
        perc /= 100.0;

        if (sortValues)
        {
            // clean nodata
            double* cleanList = (double*) calloc(*nList, sizeof(double));
            int n = 0;
            for (int i = 0; i < *nList; i++)
                if (list[i] != NODATA)
                    cleanList[n++] = list[i];

            // switch
            *nList = n;
            *list = *cleanList;

            // check on data presence
            if (*nList == 0)
                return (NODATA);

            // sort
            quicksortAscendingDouble(list, 0, *nList - 1);
        }

        double rank = (*nList * perc) - 1.;

        // return percentile
        if ((rank + 1.) > (*nList - 1))
            return list[*nList - 1];
        else if (rank < 0.)
            return list[0];
        else
            return ((rank - (int)(rank)) * (list[(int)(rank) + 1] - list[(int)(rank)])) + list[(int)(rank)];
    }
}


namespace matricial
{
    int matrixSum(double**a , double**b, int rowA , int rowB, int colA, int colB, double **c)
    {
        if ((rowA != rowB) || (colA!=colB)) return NODATA;
        for (int i = 0 ; i< rowA; i++)
        {
            for (int j=0 ; j< colA ; j++)
            {
                c[i][j]= a[i][j]+b[i][j];
            }
        }
        return CRIT3D_OK ;
    }

    int matrixProduct(double **first,double**second,int colFirst,int rowFirst,int colSecond,int rowSecond,double ** multiply)
    {
        int c, d, k;
        double sum = 0;
        if ((colFirst != rowSecond)) return NODATA;
        for ( c = 0 ; c < rowFirst ; c++ )
        {
            for ( d = 0 ; d < colSecond ; d++ )
            {
                for ( k = 0 ; k < colFirst ; k++ )
                {
                    sum += first[c][k] * second[k][d];
                }
                multiply[c][d] = sum;
                sum = 0.;
            }
        }
        return CRIT3D_OK;
    }

    void choleskyDecompositionSinglePointer(double *a, int n, double* p)
    {
        // adapted from http://www.math.hawaii.edu/~williamdemeo/C/stat243/reports/hw3/hw3/node6.html
        int i,j,k;
             for(j=0;j<n;j++)
                  p[j] = a[n*j+j];
             for(j=0;j<n;j++)
             {
                  for(k=0;k<j;k++)
                       p[j] -= a[n*k+j]*a[n*k+j];
                  p[j] = sqrt(p[j]);
                  for(i=j+1;i<n;i++)
                  {
                       for(k=0;k<j;k++)
                            a[n*j+i] -= a[n*k+i]*a[n*k+j];
                       a[n*j+i]/=p[j];
                  }
             }
    }



    void choleskyDecompositionTriangularMatrix(double **a, int n, bool isLowerMatrix)
    {
        // input double pointer (square matrix: symmetric and positive definite)
        // n: matrix dimension (n x n)
        // isLowerMatrix: 1) if true: lower triangular matrix 2) if false: upper triangular matrix
        double* diagonalElementsCholesky =(double*)calloc(n, sizeof(double));
        double* aLinear =(double*)calloc(n*n, sizeof(double));
        int counter = 0;
        for (int i=0;i<n;i++)
        {
            for (int j=0;j<n;j++)
            {
                aLinear[counter]= a[i][j];
                counter++;
            }
        }
        matricial::choleskyDecompositionSinglePointer(aLinear,n,diagonalElementsCholesky);
        counter = 0;
        if (isLowerMatrix)
        {
            for (int i=0;i<n;i++)
            {
                for (int j=0;j<n;j++)
                {

                    //if (isLowerMatrix)
                        a[j][i]= aLinear[counter]; // for lower output matrix
                    //else    a[i][j]= aLinear[counter]; // for upper output matrix
                    counter++;
                }
                a[i][i]= diagonalElementsCholesky[i];
            }

            for (int i=0;i<n;i++)
            {
                //if (isLowerMatrix)
                    for (int j=i+1;j<n;j++) a[i][j]=0.;
                //else
                    //for (int j=0;j<i;j++) a[i][j]=0.;
            }
        }
        else
        {
            for (int i=0;i<n;i++)
            {
                for (int j=0;j<n;j++)
                {

                    //if (isLowerMatrix) a[j][i]= aLinear[counter]; // for lower output matrix
                    //else
                    a[i][j]= aLinear[counter]; // for upper output matrix
                    counter++;
                }
                a[i][i]= diagonalElementsCholesky[i];
            }

            for (int i=0;i<n;i++)
            {
                //if (isLowerMatrix)
                    //for (int j=i+1;j<n;j++) a[i][j]=0.;
                //else
                    for (int j=0;j<i;j++) a[i][j]=0.;
            }
        }


        free(diagonalElementsCholesky);
        free(aLinear);
    }

    void transposedSquareMatrix(double** a, int n)
    {
        double** b = (double**)calloc(n, sizeof(double*));
        for (int i=0;i<n;i++)
            b[i]= (double*)calloc(n, sizeof(double));

        for (int i=0;i<n;i++)
        {
            for (int j=0;j<n;j++)
                b[i][j]=a[j][i];
        }
        for (int i=0;i<n;i++)
        {
            for (int j=0;j<n;j++)
                a[i][j] = b[i][j];
        }

        for (int i=0;i<n;i++)
            free(b[i]);
        free(b);
    }

    void transposedMatrix(double **inputMatrix, int nrRows, int nrColumns, double **outputMatrix)
    {
        for (int i=0;i<nrRows;i++)
        {
            for (int j=0;j<nrColumns;j++)
            {
                outputMatrix[j][i] = inputMatrix[i][j];
            }
        }
    }


}


namespace distribution
{
    float normalGauss(TfunctionInput fInput)
    {
        return float(pow((2*PI*pow(fInput.par[1],2)),-0.5)*exp(-0.5*pow((fInput.x-fInput.par[0])/fInput.par[1],2)));
    }
}

namespace myrandom {
/*
    #define IA 16807
    #define IM 2147483647
    #define AM (1.0/IM)
    #define IQ 127773
    #define IR 2836
    #define NTAB 32
    #define NDIV (1+(IM-1)/NTAB)
    #define EPS 1.2e-7
    #define RNMX (1.0-EPS)
    float ran1(long *idum)
        //“Minimal” random number generator of Park and Miller with Bays-Durham shuffle and added
        //safeguards. Returns a uniform random deviate between 0.0 and 1.0 (exclusive of the endpoint
        //values). Call with idum a negative integer to initialize; thereafter, do not alter idum between
        //successive deviates in a sequence. RNMX should approximate the largest floating value that is
        //less than 1.
    {
        int j;
        long k;
        static long iy=0;
        static long iv[NTAB];
        float temp;
        if (*idum <= 0 || !iy)
        { //Initialize.
            if (-(*idum) < 1) *idum=1; //Be sure to prevent idum = 0.
            else *idum = -(*idum);
            for (j=NTAB+7;j>=0;j--)
            {                        //Load the shuffle table (after 8 warm-ups).
                k=(*idum)/IQ;
                *idum=IA*(*idum-k*IQ)-IR*k;
                if (*idum < 0) *idum += IM;
                if (j < NTAB) iv[j] = *idum;
            }
            iy=iv[0];
        }
        k=(*idum)/IQ; // Start here when not initializing.
        *idum=IA*(*idum-k*IQ)-IR*k; // Compute idum=(IA*idum) % IM without over
        if (*idum < 0) *idum += IM; // flows by Schrage’s method.
        j=iy/NDIV; // Will be in the range 0..NTAB-1.
        iy=iv[j]; // Output previously stored value and refill the
        iv[j] = *idum; // shuffle table.
        if ((temp=AM*iy) > RNMX) return RNMX; // Because users don’t expect endpoint values.
        else return temp;
    }



    float gasdev(long *idum)
    {
        //float ran1(long *idum);
        static int iset=0;
        static float gset;
        float fac,rsq,v1,v2;
        if (*idum < 0) iset=0;
        if (iset == 0)
        {
            do {
                v1=2.0*ran1(idum)-1.0;
                v2=2.0*ran1(idum)-1.0;
                rsq=v1*v1+v2*v2;
                } while (rsq >= 1.0 || rsq == 0.0);
            fac=sqrt(-2.0*log(rsq)/rsq);
            gset=v1*fac;
            iset=1;
            return v2*fac;
        }
        else
        {
            iset=0;
            return gset;
        }
    }*/

    //----------------------------------------------------------------------
    // Generate a standard normally-distributed random variable
    // (See Numerical Recipes in Pascal W. H. Press, et al. 1989 p. 225)
    //----------------------------------------------------------------------
    float normalRandom(int *gasDevIset,float *gasDevGset)
    {
        float fac = 0;
        float r = 0;
        float v1, v2, normalRandom;
        float temp;

        if (*gasDevIset == 0) //We don't have an extra deviate
        {
            do
            {
                temp = (float) rand() / (RAND_MAX);
                v1 = 2*temp - 1;
                temp = (float) rand() / (RAND_MAX);
                v2 = 2*temp - 1;
                r = v1 * v1 + v2 * v2;
            } while ( (r>=1) | (r==0) ); // see if they are in the unit circle, and if they are not, try again.
            // Box-Muller transformation to get two normal deviates. Return one and save the other for next time.
            fac = sqrt(-2 * log(r) / r);
            *gasDevGset = v1 * fac; //Gaussian random deviates
            normalRandom = v2 * fac;
            *gasDevIset = 1; //set the flag
        }
        // We have already an extra deviate
        else
        {
            *gasDevIset = 0; //unset the flag
            normalRandom = *gasDevGset;
        }
        return normalRandom;
    }





}




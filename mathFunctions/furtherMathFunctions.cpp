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

std::string binary(unsigned x)
{
    // Warning: this breaks for numbers with more than 64 bits
    char buffer[64];
    char* p = buffer + 64;
    do
    {
        *--p = '0' + (x & 1);
    } while (x >>= 1);
    return std::string(p, buffer + 64);
}

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

double twoParametersAndExponentialPolynomialFunctions(double x, double* par)
{
    return (double)(par[0]+par[1]*pow(x,par[2]));
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

    float trapezoidalRule(float (*func)(float) , float a , float b , int n)
    {
        float x , tnm , sum , del ;
        static float sumInfinitesimal ;
        int it , j ;

        if (n == 1)
        {
            return (sumInfinitesimal=(float)(0.5*(b-a)* ((*func) (a) +(*func)(b)))) ;
        }
        else
        {
            for (it = 1 , j = 1 ; j < n-1 ; j++ ) it <<= 1 ;
            tnm = (float)(it) ;
            del = (b-a) / tnm ;
            x = (float)(a + 0.5 * del) ;
            for(sum = 0.0 , j=1 ; j <= it ; j++ , x += del) sum += (*func)(x) ;
            //s = (float)(0.5 * (s + (b-a)*sum/tnm)) ;
            return (sumInfinitesimal= (float)(0.5 * (sumInfinitesimal + (b-a)*sum/tnm))) ;
        }
    }


    float simpsonRule(float (*func)(float),float a , float b , float EPS)
    {
        /*! this function calculates definte integrals using the Simpson rule */
        if (a > b)
        {
            return (-simpsonRule(func,b, a , EPS)); //recursive formula
        }
        float old_s [10] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
        float trapezoidalRule(float (*func)(float) , float a , float b , int n) ;
        int j;
        float sumInfenitesimal , sumTrapezoidal , old_sumTrapezoidal=0.0 , old_sumInfinitesimal = 0.0 ;
        float s1 = 0.;
        for ( j=1 ; j <= 20 ; j++)
        {
            sumTrapezoidal = trapezoidalRule(func,a,b,j) ;
            sumInfenitesimal = (float)((4.0*sumTrapezoidal-old_sumTrapezoidal)/3.0) ;
            for ( short k=1 ; k < 10 ; k++)
            {
                old_s[k-1]=old_s[k];
            }
            old_s[9] = sumInfenitesimal ;
            if (j == 5) s1 = sumInfenitesimal ;
            if (j > 5 )
                if (fabs(sumInfenitesimal-old_sumInfinitesimal) < EPS*fabs(old_sumInfinitesimal) || (sumInfenitesimal == 0.0 && old_sumInfinitesimal == 0.0) ) return sumInfenitesimal ;
                old_sumInfinitesimal = sumInfenitesimal ;
                old_sumTrapezoidal = sumTrapezoidal ;
        }
        float average_s=0.0 , average_s2 = 0.0 , variance ;
        for ( short k=0 ; k < 10 ; k++)
        {
            average_s  += old_s[k];
            average_s2 += old_s[k]*old_s[k] ;
        }
        average_s  /= 10.0 ;
        average_s2 /= 10.0 ;
        variance = average_s2 - average_s * average_s ;
        if (variance < 0.01*fabs(s1)) return sumInfenitesimal ; // s is converging slowly
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


    bool fittingMarquardt(double *myParMin, double *myParMax,
        double *myPar, int nrMyPar, double *myParDelta, int myMaxIterations,double myEpsilon, int myFunctionCode,
        double *myX,int nrMyX,double *myY)
    {
        //int i;
        int myIterations = 1;
        static double lambda= 0.01; //Damping parameter
        double mySSE;           //Sum of Squared Erros
        double myDiffSSE;
        double myNewSSE;
        double *myParChange=(double *) calloc(nrMyPar, sizeof(double));
        double *myNewPar =(double *) calloc(nrMyPar, sizeof(double));
        double *L=(double *) calloc(nrMyPar, sizeof(double));
        static double VFACTOR = 10;

        /*
         myParMin               'parameters minimum values
         myParMax               'parameters maximum values
         myParFirst             'parameters first guess values
         myX                    'X values
         myY                    'Y values
        */
        bool output = false;

        for(int i = 0 ; i< nrMyPar ; i++)
        {
            L[i] = lambda ;
            myParChange[i] = 0 ;
        }


        mySSE = normGeneric(myPar, myX, nrMyX, myY, myFunctionCode);
        myDiffSSE = myEpsilon * 1000 ;

        do
        {
            leastSquares(myFunctionCode, myPar,nrMyPar, myX,nrMyX, myY, L, myParDelta, myParChange);

            // change parameters
            for (int i = 0 ; i < nrMyPar; i++ )
            {
                myNewPar[i] = myPar[i] + myParChange[i];
                if ((myNewPar[i] > myParMax[i]) && (L[i] < 1000))
                {
                    L[i]  *= VFACTOR ;
                    myNewPar[i] = myParMax[i];
                }
                if ((myNewPar[i] < myParMin[i]) && (L[i] < 1000))
                {
                    L[i] *= VFACTOR;
                    myNewPar[i] = myParMin[i];
                }
            }

            myNewSSE = normGeneric(myNewPar, myX, nrMyX, myY, myFunctionCode);

            if (myNewSSE == NODATA) return output;

            myDiffSSE = mySSE - myNewSSE ;

            if (myDiffSSE > 0)
            {
                mySSE = myNewSSE;
                for (int i = 0; i<nrMyPar ; i++)
                {
                    myPar[i] = myNewPar[i];
                    L[i] /= VFACTOR;
                }
            }
            else
            {
                for(int i = 0; i< nrMyPar; i++) L[i]*= VFACTOR ;
            }

            myIterations++ ;
        } while ((myIterations <= myMaxIterations) && (fabs(myDiffSSE) > myEpsilon));

        output = ((myIterations <= myMaxIterations) && (fabs(myDiffSSE) <= myEpsilon));

        // free pointer memory
        free(L);
        free(myParChange);
        free(myNewPar);

        return output;
    }



    void leastSquares(int myFunctionCode,double *myParameters, int nrMyParameters,
        double *myX,int nrMyX, double *myY,double *myLambda, double *myParametersDelta, double *myParametersChange)
    {
        int i,j,k;
        int nrMyData;
        nrMyData = nrMyX ;
        double* g=(double *) calloc(nrMyParameters+1, sizeof(double));
        double* z=(double *) calloc(nrMyParameters+1, sizeof(double));
        double* myFirstEst=(double *) calloc(nrMyData+1, sizeof(double));
        double myNewEst;

        double** a = (double **) calloc(nrMyParameters+1, sizeof(double*));
        double** P = (double **) calloc(nrMyParameters+1, sizeof(double*));
        for (i = 0; i < nrMyParameters+1; i++)
        {
                a[i] = (double *) calloc(nrMyParameters+1, sizeof(double));
                P[i] = (double *) calloc(nrMyData+1, sizeof(double));
        }
        double pivot,mult,top;
        //get a first set of estimates
        for (i = 0 ; i< nrMyData ; i++) myFirstEst[i] = estimateFunction(myFunctionCode, myParameters, &(myX[i]));


        //change parameters and compute derivatives
        for (i = 0 ; i< nrMyParameters ; i++)
        {
            myParameters[i] += myParametersDelta[i];
            for (j = 0 ;j< nrMyData;j++)
            {
                myNewEst = estimateFunction(myFunctionCode, myParameters, &(myX[j]));
                P[i][j] = (myNewEst - myFirstEst[j]) / myParametersDelta[i];
            }
            myParameters[i] -= myParametersDelta[i];
        }
        for (i = 0 ; i<nrMyParameters; i++)
        {
            for (j = i ; j< nrMyParameters;j++)
            {
                a[i][j] = 0.;
                for (k = 0 ; k< nrMyData ; k++) a[i][j] += P[i][k]*P[i][k];
            }
            z[i] = sqrt(a[i][i]) + 0.000001; //?
        }
        for (i = 0 ; i<nrMyParameters;i++)
        {
            g[i] = 0.;
            for (k = 0 ; k<nrMyData ; k++)
            {
                g[i] += P[i][k]* (myY[k] - myFirstEst[k]);
            }
            g[i] /= z[i];
            for (j = i ; j <nrMyParameters ; j++) a[i][j] /= (z[i] * z[j]);
       }

        for (i = 0 ; i< (nrMyParameters+1); i++)
        {
            a[i][i] += myLambda[i];
            for (j = i + 1 ; j< nrMyParameters ; j++) a[j][i] = a[i][j];
        }

        for (j = 0 ; j< (nrMyParameters - 1) ; j++)
        {
            pivot = a[j][j];
            for (i = j + 1 ; i<(nrMyParameters); i++)
            {
                mult = a[i][j] / pivot;
                for (k = j + 1; k<(nrMyParameters) ; k++) a[i][k] -= mult * a[j][k];
                g[i] -= mult * g[j];
            }
        }
        myParametersChange[nrMyParameters - 1] = g[nrMyParameters - 1] / a[nrMyParameters - 1][nrMyParameters - 1];

        for (i = nrMyParameters - 2; i>=0 ; i--)
        {
            top = g[i];
            for (k = i + 1 ; k<(nrMyParameters); k++) top -= a[i][k] * myParametersChange[k];
            myParametersChange[i] = top / a[i][i];
        }
        for (i = 0 ; i<nrMyParameters ; i++) myParametersChange[i] /= z[i];

        // free pointer memory
        for (i = 0 ; i<nrMyParameters+1 ; i++)
        {
            free(a[i]);
            free(P[i]);
        }
        free(a);
        free(P);
        free(g);
        free(z);
        free(myFirstEst);
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
        double myTmp;
        switch (myFunctionCode)
        {
            case FUNCTION_CODE_SPHERICAL :

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
            case TWOPARAMETERSPOLYNOMIAL :
                myTmp = *myX;
                output = (float)(twoParametersAndExponentialPolynomialFunctions(myTmp,myParameters));
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
            myError = float(myY[i] - myEstimate);
            output  += myError*myError ;
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


    void quicksortAscendingFloat(std::vector<float> &values, int first,int last)
    {
        int pivot,j,i;
        float temp;

        if(first<last){
            pivot=first;
            i=first;
            j=last;

            while(i<j){
                while(values[i]<=values[pivot]&&i<last)
                    i++;
                while(values[j]>values[pivot])
                    j--;
                if(i<j){
                    temp=values[i];
                     values[i]=values[j];
                     values[j]=temp;
                }
            }
            temp=values[pivot];
            values[pivot]=values[j];
            values[j]=temp;
            quicksortAscendingFloat(values,first,j-1);
            quicksortAscendingFloat(values,j+1,last);
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


    float percentile(std::vector<float> &list, int* nList, float perc, bool sortValues)
    {
        // check
        if (*nList == 0 || perc <= 0.0 || perc >= 100.0)
            return (NODATA);

        perc /= 100.0;

        if (sortValues)
        {
            // clean nodata
            std::vector<float> cleanList;
            for (int i = 0; i < *nList; i++)
            {
                if (list[i] != NODATA)
                {
                    cleanList.push_back(list[i]);
                }
            }

            // switch
            *nList = (int)cleanList.size();

            // check on data presence
            if (*nList == 0)
                return (NODATA);

            // sort
            quicksortAscendingFloat(cleanList, 0, *nList - 1);

            list.erase(list.begin(),list.end());
            list = cleanList;
        }

        float rank = (*nList * perc) - 1.f;

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
                    a[j][i]= aLinear[counter]; // for lower output matrix
                    counter++;
                }
                a[i][i]= diagonalElementsCholesky[i];
            }

            for (int i=0;i<n;i++)
            {
                    for (int j=i+1;j<n;j++) a[i][j]=0.;
            }
        }
        else
        {
            for (int i=0;i<n;i++)
            {
                for (int j=0;j<n;j++)
                {
                    a[i][j]= aLinear[counter]; // for upper output matrix
                    counter++;
                }
                a[i][i]= diagonalElementsCholesky[i];
            }

            for (int i=0;i<n;i++)
            {
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
        return float(pow((2*PI*fInput.par[1]*fInput.par[1]),-0.5)*exp(-0.5*pow((fInput.x-fInput.par[0])/fInput.par[1],2)));
    }
}

namespace myrandom {

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




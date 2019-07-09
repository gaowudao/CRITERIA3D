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


float errorFunctionPrimitive(float x)
{
    return expf(-x*x);
}

double errorFunctionPrimitive(double x)
{
    return exp(-x*x);
}


float blackBodyShape(TfunctionInput fInput)
{
    float b, y;
    b = fInput.par[0];
    y = b*(float)(pow(fInput.x, 3)*(1. / (exp(b*fInput.x)-0.99)));
    return (y);
}


double twoParametersAndExponentialPolynomialFunctions(double x, double* par)
{
    return (double)(par[0]+par[1]*pow(x,par[2]));
}

double twoHarmonicsFourier(double x, double* par)
{
    return par[0] + par[1]*cos(2*PI/par[5]*x) + par[2]*sin(2*PI/par[5]*x) + par[3]*cos(4*PI/par[5]*x) + par[4]*sin(4*PI/par[5]*x);
}

double harmonicsFourierGeneral(double x, double* par,int nrPar)
{
    // the last parameter is the period
    if (nrPar == 2) return par[0];
    else
    {
        int counter = 0;
        double y = par[0];
        int requiredHarmonics;
        requiredHarmonics = (nrPar - 2)/2;
        double angularVelocity;
        angularVelocity = 2*PI/par[nrPar-1];
        while (counter < requiredHarmonics)
        {
            y += par[1+counter*2]*cos(angularVelocity*(counter+1)*x) + par[2+counter*2]*sin(angularVelocity*(counter+1)*x);
            counter++;
        }
        return y;
    }

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
        float s = NODATA , st = NODATA , ost = 0.0 , os = 0.0 ;
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
        //float trapezoidalRule(float (*func)(float) , float a , float b , int n) ;
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

    double trapezoidalRule(double (*func)(double) , double a , double b , int n)
    {
        float x , tnm , sum , del ;
        static double sumInfinitesimal ;
        int it , j ;

        if (n == 1)
        {
            return (sumInfinitesimal=(float)(0.5*(b-a)* ((*func) (a) +(*func)(b)))) ;
        }
        else
        {
            for (it = 1 , j = 1 ; j < n-1 ; j++ ) it <<= 1 ;
            tnm = (double)(it) ;
            del = (b-a) / tnm ;
            x = (float)(a + 0.5 * del) ;
            for(sum = 0.0 , j=1 ; j <= it ; j++ , x += del) sum += (*func)(x) ;
            //s = (float)(0.5 * (s + (b-a)*sum/tnm)) ;
            return (sumInfinitesimal= (double)(0.5 * (sumInfinitesimal + (b-a)*sum/tnm))) ;
        }
    }


    double simpsonRule(double (*func)(double),double a , double b , double EPS)
    {
        /*! this function calculates definte integrals using the Simpson rule */
        if (a > b)
        {
            return (-simpsonRule(func,b, a , EPS)); //recursive formula
        }
        double old_s [10] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
        //double trapezoidalRule(double (*func)(double) , double a , double b , int n) ;
        int j;
        double sumInfenitesimal = 0. , sumTrapezoidal = 0. , old_sumTrapezoidal=0.0 , old_sumInfinitesimal = 0.0 ;
        double s1 = 0.;
        for ( j=1 ; j <= 20 ; j++)
        {
            sumTrapezoidal = trapezoidalRule(func,a,b,j) ;
            sumInfenitesimal = (4.0*sumTrapezoidal-old_sumTrapezoidal)/3.0 ;
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
        double average_s=0.0 , average_s2 = 0.0 , variance ;
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
        //float *firstColumn = (float *) calloc(dimTable, sizeof(float));
        //float *secondColumn = (float *) calloc(dimTable, sizeof(float));
        //firstColumn = xColumn ;
        //secondColumn = yColumn ;
        float slope , offset ;
        short stage=1;
        if (x < xColumn[0]) return yColumn[0] ;
        if (x > xColumn[dimTable-1]) return yColumn[dimTable-1];
        while (x > xColumn[stage]) stage++ ;
        slope = (yColumn[stage]- yColumn[stage-1])/(xColumn[stage] - xColumn[stage-1]);
        offset = -xColumn[stage-1]*slope + yColumn[stage-1];
        //free(firstColumn);
        //free(secondColumn);
        return (slope * x + offset) ;
    }

    double linearInterpolation (double x, double *xColumn , double *yColumn, int dimTable )
    {
        //double *firstColumn = (double *) calloc(dimTable, sizeof(double));
        //double *secondColumn = (double *) calloc(dimTable, sizeof(double));
        //firstColumn = xColumn ;
        //secondColumn = yColumn ;
        double slope , offset ;
        short stage=1;
        if (x < xColumn[0]) return yColumn[0] ;
        if (x > xColumn[dimTable-1]) return yColumn[dimTable-1];
        while (x > xColumn[stage]) stage++ ;
        slope = (yColumn[stage]- yColumn[stage-1])/(xColumn[stage] - xColumn[stage-1]);
        offset = -xColumn[stage-1]*slope + yColumn[stage-1];
        //free(firstColumn);
        //free(secondColumn);
        return (slope * x + offset) ;
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


        mySSE = normGeneric(myPar, nrMyPar, myX, nrMyX, myY, myFunctionCode);
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

            myNewSSE = normGeneric(myNewPar,nrMyPar, myX, nrMyX, myY, myFunctionCode);

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
        for (i = 0 ; i< nrMyData ; i++) myFirstEst[i] = estimateFunction(myFunctionCode, myParameters, nrMyParameters, &(myX[i]));


        //change parameters and compute derivatives
        for (i = 0 ; i< nrMyParameters ; i++)
        {
            myParameters[i] += myParametersDelta[i];
            for (j = 0 ;j< nrMyData;j++)
            {
                myNewEst = estimateFunction(myFunctionCode, myParameters, nrMyParameters, &(myX[j]));
                P[i][j] = (myNewEst - myFirstEst[j]) / myParametersDelta[i];
            }
            myParameters[i] -= myParametersDelta[i];
        }
        for (i = 0 ; i<nrMyParameters; i++)
        {
            for (j = i ; j< nrMyParameters;j++)
            {
                a[i][j] = 0.;
                for (k = 0 ; k< nrMyData ; k++) a[i][j] += P[i][k]*P[j][k];
            }
            z[i] = sqrt(a[i][i]) + EPSILON; //?
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


    double estimateFunction(int myFunctionCode, double *myParameters,int nrMyParameters, double *myX)
    {
        /*
            '' spherical
            '' myParameters(0): range
            '' myParameters(1): nugget
            '' myParameters(2): sill
        */
        double output;
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
                    output = (myParameters[1] + (myParameters[2] - myParameters[1]) * (1.5 * myTmp - 0.5 * myTmp * myTmp * myTmp));
                else
                    output = (myParameters[2]);
                break;
            case FUNCTION_CODE_TWOPARAMETERSPOLYNOMIAL :
                myTmp = *myX;
                output = (twoParametersAndExponentialPolynomialFunctions(myTmp,myParameters));
                break;
            case FUNCTION_CODE_FOURIER_2_HARMONICS :
                myTmp = *myX;
                output = (twoHarmonicsFourier(myTmp,myParameters));
                break;
            case FUNCTION_CODE_FOURIER_GENERAL_HARMONICS :
                myTmp = *myX;
                output = harmonicsFourierGeneral(myTmp,myParameters,nrMyParameters);
                break;

            default:
                output = NODATA ;


        }
        return output;
    }


    double normGeneric(double *myParameters,int nrMyParameters, double *myX, int nrMyX,double *myY,int myFunctionCode)
    {
        int i;
        float myError;
        double myEstimate;
        double output = 0;
        for (i = 0; i < nrMyX ; i++)
        {
            myEstimate = estimateFunction(myFunctionCode, myParameters, nrMyParameters, &(myX[i]));
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

    double cubicSpline(double x , double *firstColumn , double *secondColumn, int dim)
    {
        double a,b,c,d,y ;
        int i = 0 ;
        double *secondDerivative = (double *) calloc(dim, sizeof(double));
        for (int i=0 ; i < dim; i++)
        {
            secondDerivative[i] = NODATA;
        }
        punctualSecondDerivative(dim,firstColumn,secondColumn,secondDerivative);
        while (x > firstColumn[i]) i++ ;
        double step;
        step = (firstColumn[i]- firstColumn[i-1]);
        a = (firstColumn[i] - x)/ step ;
        b = 1 - a ;
        d = c = step*step/6;
        c *= (a*a*a - a);
        d *= (b*b*b - b);
        y = a*secondColumn[i-1]+b*secondColumn[i]+c*secondDerivative[i-1]+d*secondDerivative[i];
        free(secondDerivative);
        return y ;
    }

    void punctualSecondDerivative(int dim, double *firstColumn , double *secondColumn, double* secondDerivative)
    {
        int matrixDimension;
        matrixDimension = dim-2;
        double *y2 = (double *) calloc(matrixDimension, sizeof(double));
        double *constantTerm = (double *) calloc(matrixDimension, sizeof(double));
        double *diagonal =  (double *) calloc(matrixDimension, sizeof(double));
        double *subDiagonal =  (double *) calloc(matrixDimension, sizeof(double));
        double *superDiagonal =  (double *) calloc(matrixDimension, sizeof(double));
        for (int i=0 ; i < matrixDimension; i++)
        {
            y2[i] = 0;
            diagonal[i] = (firstColumn[i+2]-firstColumn[i])/3 ;
            subDiagonal[i] = (firstColumn[i+1]-firstColumn[i])/6;
            superDiagonal[i] = (firstColumn[i+2]-firstColumn[i+1])/6;
            constantTerm[i] = (secondColumn[i+2]-secondColumn[i+1])/(firstColumn[i+2]-firstColumn[i+1])
                    -(secondColumn[i+1]-secondColumn[i])/(firstColumn[i+1]-firstColumn[i]);
        }
        tridiagonalThomasAlgorithm(matrixDimension,subDiagonal,diagonal,superDiagonal,constantTerm,y2);
        for (int i = 0 ; i < dim ; i++) secondDerivative[i]= 0;
        for (int i = 1 ; i < dim-1 ; i++) secondDerivative[i] = y2[i-1];
        free(y2);
        free(constantTerm);
        free(subDiagonal);
        free(superDiagonal);

    }

    void tridiagonalThomasAlgorithm (int n, double *subDiagonal, double *mainDiagonal, double *superDiagonal, double *constantTerm, double* output)
    {

            // * n - number of equations
            // * subDiagonal - sub-diagonal (means it is the diagonal below the main diagonal) -- indexed from 1..n-1
            // * b - the main diagonal
            // * c - sup-diagonal (means it is the diagonal above the main diagonal) -- indexed from 0..n-2
            // * v - right part
            // * output - the answer

            double *newDiagonal, *newConstantTerm;
            newDiagonal = (double *) calloc(n, sizeof(double));
            newConstantTerm =   (double *) calloc(n, sizeof(double));

            newDiagonal[0] = mainDiagonal[0];
            newConstantTerm[0]= constantTerm[0];
            for (int i = 1; i < n; i++)
            {
                    double m = subDiagonal[i]/mainDiagonal[i-1];
                    newDiagonal[i] = mainDiagonal[i] - m*superDiagonal[i-1];
                    newConstantTerm[i] = constantTerm[i] - m*constantTerm[i-1];
            }

            output[n-1] = newConstantTerm[n-1]/newDiagonal[n-1];
            for (int i = n - 2; i >= 0; i--)
                    output[i]=(newConstantTerm[i]-superDiagonal[i]*output[i+1])/newDiagonal[i];

            free(newDiagonal);
            free(newConstantTerm);
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
                c[i][j]= a[i][j] + b[i][j];
            }
        }
        return CRIT3D_OK ;
    }

    int matrixDifference(double**a , double**b, int rowA , int rowB, int colA, int colB, double **c)
    {
        if ((rowA != rowB) || (colA!=colB)) return NODATA;
        for (int i = 0 ; i< rowA; i++)
        {
            for (int j=0 ; j< colA ; j++)
            {
                c[i][j]= a[i][j] - b[i][j];
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

    void minor(double** b,double** a,int i,int n)
    {
        //	calculate minor of matrix OR build new matrix : k-had = minor
        int j,l,h=0,k=0;
        for(l=1;l<n;l++)
            for( j=0;j<n;j++)
            {
                if(j == i)
                    continue;
                b[h][k] = a[l][j];
                k++;
                if(k == (n-1))
                {
                    h++;
                    k=0;
                }
            }
    }


    double determinant(double** a,int n)
    {
        //	calculate determinte of matrix
        int i;
        double sum=0;
        if (n == 1)
            return a[0][0];
        else if(n == 2)
            return (a[0][0]*a[1][1]-a[0][1]*a[1][0]);
        else
        {
            double** b = (double**)calloc(n, sizeof(double*));
            for (int i=0;i<n;i++)
                b[i]= (double*)calloc(n, sizeof(double));
            for(i=0;i<n;i++)
            {
                matricial::minor(b,a,i,n);	// read function
                sum = (sum + a[0][i]*pow(-1,i)*matricial::determinant(b,(n-1)));	// sum = determinte matrix
            }
            for (int i=0;i<n;i++)
                free(b[i]);
            free(b);
            return sum;
        }
    }

    void cofactor(double** a,double** d,int n,double determinantOfMatrix)
    {
        double** b;
        double** c;
        b = (double**)calloc(n, sizeof(double*));
        for (int i=0;i<n;i++)
        {
            b[i]= (double*)calloc(n, sizeof(double));
        }
        c = (double**)calloc(n, sizeof(double*));
        for (int i=0;i<n;i++)
        {
            c[i]= (double*)calloc(n, sizeof(double));
        }
        //int l,h,m,k,i,j;
        int m,k;
        for (int h=0;h<n;h++)
            for (int l=0;l<n;l++)
            {
                m=0;
                k=0;
                for (int i=0;i<n;i++)
                    for (int j=0;j<n;j++)
                        if (i != h && j != l){
                            b[m][k]=a[i][j];
                            if (k<(n-2))
                                k++;
                            else{
                                k=0;
                                m++;
                            }
                        }
                c[h][l] = pow(-1,(h+l))*matricial::determinant(b,(n-1));	// c = cofactor Matrix
            }
        matricial::transposedMatrix(c,n,n,d);
        for (int h=0;h<n;h++)
        {
            for (int l=0;l<n;l++)
            {
                d[h][l] /= determinantOfMatrix;
            }
        }

        for (int h=0;h<n;h++)
        {
            free(b[h]);
            free(c[h]);
        }
        free(b);
        free(c);
    }

    void inverse(double** a,double** d,int n)
    {
        double determinantOfMatrix;
        determinantOfMatrix = determinant(a,n);
        //	calculate inverse of matrix
        if(determinantOfMatrix == 0)
            printf("\nInverse of Entered Matrix is not possible\n");
        else if(n == 1)
            d[0][0] = 1;
        else
            matricial::cofactor(a,d,n,determinantOfMatrix);
    }

    int eigenSystemMatrix2x2(double** a, double* eigenvalueA, double** eigenvectorA, int n)
    {
        if (n != 2) return PARAMETER_ERROR;
        double b,c;
        b = - a[0][0] - a[1][1];
        c = (a[0][0] * a[1][1] - a[0][1] * a[1][0]);
        if ((b*b - 4*c) < 0) return PARAMETER_ERROR;

        if (fabs(a[0][1]) < EPSILON && fabs(a[1][0]) < EPSILON)
        {
            eigenvalueA[0]= a[0][0];
            eigenvalueA[1]= a[1][1];
            eigenvectorA[1][0] = 1;
            eigenvectorA[0][0] = 0;
            eigenvectorA[0][1] = 0;
            eigenvectorA[1][1] = 1;
            return 0;
        }
        if (fabs(a[0][1]) < EPSILON)
        {
            eigenvalueA[0]= a[0][0];
            eigenvalueA[1]= a[1][1];
            eigenvectorA[1][0] = 1;
            eigenvectorA[0][0] = (eigenvalueA[0] - a[1][1]) * eigenvectorA[1][0];
            eigenvectorA[0][1] = 0;
            eigenvectorA[1][1] = 1;
            return 0;
        }
        if (fabs(a[1][0]) < EPSILON)
        {
            eigenvalueA[0]= a[0][0];
            eigenvalueA[1]= a[1][1];
            eigenvectorA[0][0] = 1;
            eigenvectorA[1][0] = 0;
            eigenvectorA[0][1] = 1;
            eigenvectorA[1][1] = (eigenvalueA[1] - a[0][0]) * eigenvectorA[0][1];
            return 0;

        }
        eigenvalueA[0] = (-b + sqrt(b*b - 4*c))/2;
        eigenvalueA[1] = (-b - sqrt(b*b - 4*c))/2;
        for (int i=0;i<2;i++)
        {
            eigenvectorA[0][i] = 1;
            eigenvectorA[1][i] = (eigenvalueA[i]-a[0][0])/a[0][1];
        }
        return 0;
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

    double normalRandom(int *gasDevIset,double *gasDevGset)
    {
        double fac = 0;
        double r = 0;
        double v1, v2, normalRandom;
        double temp;

        if (*gasDevIset == 0) //We don't have an extra deviate
        {
            do
            {
                temp = (double) rand() / (RAND_MAX);
                v1 = 2*temp - 1;
                temp = (double) rand() / (RAND_MAX);
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

namespace statistics
{

    double ERF(double x, double accuracy) // error function
    {

        return (1.12837916709551*double(integration::simpsonRule(errorFunctionPrimitive,0.,x,accuracy))); // the constant in front of integration is equal to 2*pow(PI,-0.5)
    }

    double ERFC(double x, double accuracy) // error function
    {
        return (1. - ERF(x, accuracy));
    }

    double tabulatedERFC(double x)
    {
        return (1. - tabulatedERF(x));
    }

    double tabulatedERF(double x)
    {
        double output = 0;
        double variable;
        variable = fabs(x);
        double* extendedValueX = (double *) calloc(400, sizeof(double));
        double* extendedValueY = (double *) calloc(400, sizeof(double));
        double valueY[400] = {0.000000000,0.011283000,0.022565000,0.033841000,0.045111000,0.056372000,0.067622000,0.078858000,0.090078000,0.101282000,0.112463000,0.123623000,0.134758000,0.145867000,0.156947000	,0.167996000	,0.179012000	,0.189992000	,0.200936000	,0.211840000	,0.222703000	,0.233522000	,0.244296000	,0.255023000	,0.265700000	,0.276326000	,0.286900000	,0.297418000	,0.307880000	,0.318283000	,0.328627000	,0.338908000	,0.349126000	,0.359279000	,0.369365000	,0.379382000	,0.389330000	,0.399206000	,0.409009000	,0.418739000	,0.428392000	,0.437969000	,0.447468000	,0.456887000	,0.466225000	,0.475482000	,0.484655000	,0.493745000	,0.502750000	,0.511668000	,0.520500000	,0.529244000	,0.537899000	,0.546464000	,0.554939000	,0.563323000	,0.571616000	,0.579816000	,0.587923000	,0.595936000	,0.603856000	,0.611681000	,0.619411000	,0.627046000	,0.634586000	,0.642029000	,0.649377000	,0.656628000	,0.663782000	,0.670840000	,0.677801000	,0.684666000	,0.691433000	,0.698104000	,0.704678000	,0.711156000	,0.717537000	,0.723822000	,0.730010000	,0.736103000	,0.742101000	,0.748003000	,0.753811000	,0.759524000	,0.765143000	,0.770668000	,0.776100000	,0.781440000	,0.786687000	,0.791843000	,0.796908000	,0.801883000	,0.806768000	,0.811564000	,0.816271000	,0.820891000	,0.825424000	,0.829870000	,0.834232000	,0.838508000	,0.842701000	,0.846810000	,0.850838000	,0.854784000	,0.858650000	,0.862436000	,0.866144000	,0.869773000	,0.873326000	,0.876803000	,0.880205000	,0.883533000	,0.886788000	,0.889971000	,0.893082000	,0.896124000	,0.899096000	,0.902000000	,0.904837000	,0.907608000	,0.910314000	,0.912956000	,0.915534000	,0.918050000	,0.920505000	,0.922900000	,0.925236000	,0.927514000	,0.929734000	,0.931899000	,0.934008000	,0.936063000	,0.938065000	,0.940015000	,0.941914000	,0.943762000	,0.945561000	,0.947312000	,0.949016000	,0.950673000	,0.952285000	,0.953852000	,0.955376000	,0.956857000	,0.958297000	,0.959695000	,0.961054000	,0.962373000	,0.963654000	,0.964898000	,0.966105000	,0.967277000	,0.968413000	,0.969516000	,0.970586000	,0.971623000	,0.972628000	,0.973603000	,0.974547000	,0.975462000	,0.976348000	,0.977207000	,0.978038000	,0.978843000	,0.979622000	,0.980376000	,0.981105000	,0.981810000	,0.982493000	,0.983153000	,0.983790000	,0.984407000	,0.985003000	,0.985578000	,0.986135000	,0.986672000	,0.987190000	,0.987691000	,0.988174000	,0.988641000	,0.989091000	,0.989525000	,0.989943000	,0.990347000	,0.990736000	,0.991111000	,0.991472000	,0.991821000	,0.992156000	,0.992479000	,0.992790000	,0.993090000	,0.993378000	,0.993656000	,0.993923000	,0.994179000	,0.994426000	,0.994664000	,0.994892000	,0.995111000	,0.995322000	,0.995525000	,0.995719000	,0.995906000	,0.996086000	,0.996258000	,0.996423000	,0.996582000	,0.996734000	,0.996880000	,0.997021000	,0.997155000	,0.997284000	,0.997407000	,0.997525000	,0.997639000	,0.997747000	,0.997851000	,0.997951000	,0.998046000	,0.998137000	,0.998224000	,0.998308000	,0.998388000	,0.998464000	,0.998537000	,0.998607000	,0.998674000	,0.998738000	,0.998799000	,0.998857000	,0.998912000	,0.998966000	,0.999016000	,0.999065000	,0.999111000	,0.999155000	,0.999197000	,0.999237000	,0.999275000	,0.999311000	,0.999346000	,0.999379000	,0.999411000	,0.999441000	,0.999469000	,0.999497000	,0.999523000	,0.999547000	,0.999571000	,0.999593000	,0.999614000	,0.999635000	,0.999654000	,0.999672000	,0.999689000	,0.999706000	,0.999722000	,0.999736000	,0.999751000	,0.999764000	,0.999777000	,0.999789000	,0.999800000	,0.999811000	,0.999822000	,0.999831000	,0.999841000	,0.999849000	,0.999858000	,0.999866000	,0.999873000	,0.999880000	,0.999887000	,0.999893000	,0.999899000	,0.999905000	,0.999910000	,0.999916000	,0.999920000	,0.999925000	,0.999929000	,0.999933000	,0.999937000	,0.999941000	,0.999944000	,0.999948000	,0.999951000	,0.999954000	,0.999956000	,0.999959000	,0.999961000	,0.999964000	,0.999966000	,0.999968000	,0.999970000	,0.999972000	,0.999973000	,0.999975000	,0.999977000	,0.999977910	,0.999979260	,0.999980530	,0.999981730	,0.999982860	,0.999983920	,0.999984920	,0.999985860	,0.999986740	,0.999987570	,0.999988350	,0.999989080	,0.999989770	,0.999990420	,0.999991030	,0.999991600	,0.999992140	,0.999992640	,0.999993110	,0.999993560	,0.999993970	,0.999994360	,0.999994730	,0.999995070	,0.999995400	,0.999995700	,0.999995980	,0.999996240	,0.999996490	,0.999996720	,0.999996940	,0.999997150	,0.999997340	,0.999997510	,0.999997680	,0.999997838	,0.999997983	,0.999998120	,0.999998247	,0.999998367	,0.999998478	,0.999998582	,0.999998679	,0.999998770	,0.999998855	,0.999998934	,0.999999008	,0.999999077	,0.999999141	,0.999999201	,0.999999257	,0.999999309	,0.999999358	,0.999999403	,0.999999445	,0.999999485	,0.999999521	,0.999999555	,0.999999587	,0.999999617	,0.999999644	,0.999999670	,0.999999694	,0.999999716	,0.999999736	,0.999999756	,0.999999773	,0.999999790	,0.999999805,0.999999820,0.999999833,0.999999845,0.999999857,0.999999867,0.999999877,0.999999886,0.999999895,0.999999903,0.999999910,0.999999917,0.999999923,0.999999929,0.999999934,0.999999939,0.999999944,0.999999948,0.999999952,0.999999956,0.999999959,0.999999962,0.999999965,0.999999968,0.999999970,0.999999973,0.999999975,0.999999977,0.999999979,0.999999980,0.999999982,0.999999983};

        for (int i=0;i<400;i++)
        {
            extendedValueX[i] = i*0.01;
            extendedValueY[i] = valueY[i];
        }

        if (variable >= 3.99)
        {
            output = 1 - exp(-variable*variable)/(sqrt(PI)*variable);
        }
        else
        {
            /*int counter = 0;
            while(variable > valueX[counter])
            {
                counter++;
            }
            output = 100*((variable - valueX[counter-1])*valueY[counter-1] + (valueX[counter]-variable)*valueY[counter]);
        }*/
            output = interpolation::linearInterpolation(variable,extendedValueX,extendedValueY,400);
            free(extendedValueX);
            free(extendedValueY);
        }
        if (x < 0)
            return -output;
        else
            return output;

    }
    double inverseERF(double value, double accuracy)
    {

        if (value >=1 || value <= -1)
        {
            return PARAMETER_ERROR;
        }
        double root;

        if (fabs(value) < EPSILON)
        {
            return 0.;
        }
        else if (value  >= EPSILON)
        {
            double leftBound, rightBound;
            leftBound = 0.;
            rightBound = 100.;
            do
            {
                root = ERF((rightBound+leftBound)/2,accuracy);
                if (root < value)
                {
                    leftBound = (rightBound+leftBound)/2;
                }
                else
                {
                    rightBound = (rightBound+leftBound)/2;
                }
            } while(fabs(leftBound - rightBound) > accuracy);

            return (rightBound+leftBound)/2;
        }
        else
        {
            double leftBound, rightBound;
            leftBound = -100.;
            rightBound = 0.;
            do
            {
                root = ERF((rightBound+leftBound)/2,accuracy);
                if (root < value)
                {
                    leftBound = (rightBound+leftBound)/2;
                }
                else
                {
                    rightBound = (rightBound+leftBound)/2;
                }
            } while(fabs(leftBound - rightBound) > accuracy);

            return (rightBound+leftBound)/2;
        }
    }


    double inverseERFC(double value, double accuracy)
    {

        if (value >=2 || value <= 0)
        {
            return PARAMETER_ERROR;
        }

        double root;

        if (fabs(value-1) <= EPSILON)
        {
            return 0. ;
        }
        else if (value  < 1)
        {
            double leftBound, rightBound;
            leftBound = 0.;
            rightBound = 100.;
            do
            {
                root = ERFC((rightBound+leftBound)/2,accuracy);
                if (root > value)
                {
                    leftBound = (rightBound+leftBound)/2;
                }
                else
                {
                    rightBound = (rightBound+leftBound)/2;
                }
            } while(fabs(leftBound - rightBound) > accuracy);
            return (rightBound+leftBound)/2;
        }
        else
        {
            double leftBound, rightBound;
            leftBound = -100.;
            rightBound = 0.;
            do
            {
                root = ERFC((rightBound+leftBound)/2,accuracy);
                if (root > value)
                {
                    leftBound = (rightBound+leftBound)/2;
                }
                else
                {
                    rightBound = (rightBound+leftBound)/2;
                }
            } while(fabs(leftBound - rightBound) > accuracy);
            return (rightBound+leftBound)/2;
        }
    }

    double inverseTabulatedERF(double value)
    {
        // precision on the third digit after dot
        if (fabs(value >= 1)) return PARAMETER_ERROR;

        double output = 0;
        double variable;
        variable = fabs(value);
        double valueY[400] = {0.000000000,0.011283000,0.022565000,0.033841000,0.045111000,0.056372000,0.067622000,0.078858000,0.090078000,0.101282000,0.112463000,0.123623000,0.134758000,0.145867000,0.156947000	,0.167996000	,0.179012000	,0.189992000	,0.200936000	,0.211840000	,0.222703000	,0.233522000	,0.244296000	,0.255023000	,0.265700000	,0.276326000	,0.286900000	,0.297418000	,0.307880000	,0.318283000	,0.328627000	,0.338908000	,0.349126000	,0.359279000	,0.369365000	,0.379382000	,0.389330000	,0.399206000	,0.409009000	,0.418739000	,0.428392000	,0.437969000	,0.447468000	,0.456887000	,0.466225000	,0.475482000	,0.484655000	,0.493745000	,0.502750000	,0.511668000	,0.520500000	,0.529244000	,0.537899000	,0.546464000	,0.554939000	,0.563323000	,0.571616000	,0.579816000	,0.587923000	,0.595936000	,0.603856000	,0.611681000	,0.619411000	,0.627046000	,0.634586000	,0.642029000	,0.649377000	,0.656628000	,0.663782000	,0.670840000	,0.677801000	,0.684666000	,0.691433000	,0.698104000	,0.704678000	,0.711156000	,0.717537000	,0.723822000	,0.730010000	,0.736103000	,0.742101000	,0.748003000	,0.753811000	,0.759524000	,0.765143000	,0.770668000	,0.776100000	,0.781440000	,0.786687000	,0.791843000	,0.796908000	,0.801883000	,0.806768000	,0.811564000	,0.816271000	,0.820891000	,0.825424000	,0.829870000	,0.834232000	,0.838508000	,0.842701000	,0.846810000	,0.850838000	,0.854784000	,0.858650000	,0.862436000	,0.866144000	,0.869773000	,0.873326000	,0.876803000	,0.880205000	,0.883533000	,0.886788000	,0.889971000	,0.893082000	,0.896124000	,0.899096000	,0.902000000	,0.904837000	,0.907608000	,0.910314000	,0.912956000	,0.915534000	,0.918050000	,0.920505000	,0.922900000	,0.925236000	,0.927514000	,0.929734000	,0.931899000	,0.934008000	,0.936063000	,0.938065000	,0.940015000	,0.941914000	,0.943762000	,0.945561000	,0.947312000	,0.949016000	,0.950673000	,0.952285000	,0.953852000	,0.955376000	,0.956857000	,0.958297000	,0.959695000	,0.961054000	,0.962373000	,0.963654000	,0.964898000	,0.966105000	,0.967277000	,0.968413000	,0.969516000	,0.970586000	,0.971623000	,0.972628000	,0.973603000	,0.974547000	,0.975462000	,0.976348000	,0.977207000	,0.978038000	,0.978843000	,0.979622000	,0.980376000	,0.981105000	,0.981810000	,0.982493000	,0.983153000	,0.983790000	,0.984407000	,0.985003000	,0.985578000	,0.986135000	,0.986672000	,0.987190000	,0.987691000	,0.988174000	,0.988641000	,0.989091000	,0.989525000	,0.989943000	,0.990347000	,0.990736000	,0.991111000	,0.991472000	,0.991821000	,0.992156000	,0.992479000	,0.992790000	,0.993090000	,0.993378000	,0.993656000	,0.993923000	,0.994179000	,0.994426000	,0.994664000	,0.994892000	,0.995111000	,0.995322000	,0.995525000	,0.995719000	,0.995906000	,0.996086000	,0.996258000	,0.996423000	,0.996582000	,0.996734000	,0.996880000	,0.997021000	,0.997155000	,0.997284000	,0.997407000	,0.997525000	,0.997639000	,0.997747000	,0.997851000	,0.997951000	,0.998046000	,0.998137000	,0.998224000	,0.998308000	,0.998388000	,0.998464000	,0.998537000	,0.998607000	,0.998674000	,0.998738000	,0.998799000	,0.998857000	,0.998912000	,0.998966000	,0.999016000	,0.999065000	,0.999111000	,0.999155000	,0.999197000	,0.999237000	,0.999275000	,0.999311000	,0.999346000	,0.999379000	,0.999411000	,0.999441000	,0.999469000	,0.999497000	,0.999523000	,0.999547000	,0.999571000	,0.999593000	,0.999614000	,0.999635000	,0.999654000	,0.999672000	,0.999689000	,0.999706000	,0.999722000	,0.999736000	,0.999751000	,0.999764000	,0.999777000	,0.999789000	,0.999800000	,0.999811000	,0.999822000	,0.999831000	,0.999841000	,0.999849000	,0.999858000	,0.999866000	,0.999873000	,0.999880000	,0.999887000	,0.999893000	,0.999899000	,0.999905000	,0.999910000	,0.999916000	,0.999920000	,0.999925000	,0.999929000	,0.999933000	,0.999937000	,0.999941000	,0.999944000	,0.999948000	,0.999951000	,0.999954000	,0.999956000	,0.999959000	,0.999961000	,0.999964000	,0.999966000	,0.999968000	,0.999970000	,0.999972000	,0.999973000	,0.999975000	,0.999977000	,0.999977910	,0.999979260	,0.999980530	,0.999981730	,0.999982860	,0.999983920	,0.999984920	,0.999985860	,0.999986740	,0.999987570	,0.999988350	,0.999989080	,0.999989770	,0.999990420	,0.999991030	,0.999991600	,0.999992140	,0.999992640	,0.999993110	,0.999993560	,0.999993970	,0.999994360	,0.999994730	,0.999995070	,0.999995400	,0.999995700	,0.999995980	,0.999996240	,0.999996490	,0.999996720	,0.999996940	,0.999997150	,0.999997340	,0.999997510	,0.999997680	,0.999997838	,0.999997983	,0.999998120	,0.999998247	,0.999998367	,0.999998478	,0.999998582	,0.999998679	,0.999998770	,0.999998855	,0.999998934	,0.999999008	,0.999999077	,0.999999141	,0.999999201	,0.999999257	,0.999999309	,0.999999358	,0.999999403	,0.999999445	,0.999999485	,0.999999521	,0.999999555	,0.999999587	,0.999999617	,0.999999644	,0.999999670	,0.999999694	,0.999999716	,0.999999736	,0.999999756	,0.999999773	,0.999999790	,0.999999805,0.999999820,0.999999833,0.999999845,0.999999857,0.999999867,0.999999877,0.999999886,0.999999895,0.999999903,0.999999910,0.999999917,0.999999923,0.999999929,0.999999934,0.999999939,0.999999944,0.999999948,0.999999952,0.999999956,0.999999959,0.999999962,0.999999965,0.999999968,0.999999970,0.999999973,0.999999975,0.999999977,0.999999979,0.999999980,0.999999982,0.999999983};
        double* extendedValueX = (double *) calloc(1000, sizeof(double));
        double* extendedValueY = (double *) calloc(1000, sizeof(double));
        double* secondDerivative = (double *) calloc(1000, sizeof(double));
        for (int i=0;i<1000;i++)
        {
            extendedValueX[i] = i*0.01;
            if (i > 399)
            {
                extendedValueY[i] = 1 - exp(-extendedValueX[i]*extendedValueX[i])/(sqrt(PI)*variable);
                if (extendedValueY[i] < extendedValueY[i-1]) extendedValueY[i] = extendedValueY[i-1];
            }
            else
            {
                extendedValueY[i] = valueY[i];
            }
        }
        output = interpolation::linearInterpolation(variable,extendedValueY,extendedValueX,1000);

        free(extendedValueX);
        free(extendedValueY);
        if (value < 0)
            return -output;
        else
            return output;

    }


    double inverseTabulatedERFC(double value)
    {
        // precision on the third digit after dots
        if (value >=2 || value <= 0)
        {
            return PARAMETER_ERROR;
        }
        return statistics::inverseTabulatedERF(1-value);
    }
}




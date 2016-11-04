/*-----------------------------------------------------------------------------------
    furtherMathFunctions.h
    Antonio Volta  avolta @arpa.smr.it

    CRITERIA 3D
    Copyright (C) 2011 Fausto Tomei, Gabriele Antolini, Antonio Volta
    Alberto Pistocchi, Marco Bittelli

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

    contacts:
    ftomei@arpa.emr.it
    fausto.tomei@gmail.com
    gantolini@arpa.emr.it
    avolta@arpa.emr.it
    alberto.pistocchi@gecosistema.it
    marco.bittelli@unibo.it
-----------------------------------------------------------------------------------*/

#include <math.h>
#include <malloc.h>

#include "commonConstants.h"
#include "furtherMathFunctions.h"


float blackBodyShape(TfunctionInput fInput)
{
    float b, y;
    b = fInput.par[0];
    y = b*(float)(pow(fInput.x, 3)*(1. / (exp(b*fInput.x)-0.99)));
    return (y);
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
    // this is a set for function integrations by means of the Simpson

    float qsimpParametric(float (*func)(TfunctionInput), int nrPar, float *par,float a , float b , float EPS)
    {
        // this function calculates definte integrals using the Simpson rule
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
            average_s2 += pow(old_s[k],2) ;
        }
        average_s  /= 10.0 ;
        average_s2 /= 10.0 ;
        variance = average_s2 - pow(average_s,2) ;

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
        // this function calculates definte integrals using the Simpson rule
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
            average_s2 += pow(old_s[k],2) ;
        }
        average_s  /= 10.0 ;
        average_s2 /= 10.0 ;
        variance = average_s2 - pow(average_s,2) ;
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

    double percentile(double* list, int nList, double perc, bool sortValues)
    {
        double rank;

        if (nList == 0 || perc <= 0.0 || perc >= 100.0)
            return (NODATA);

        //TODO nodata clean

        perc /= 100.0;
        rank = (perc * nList) - 1.0;

        if (sortValues)
            quicksortAscendingDouble(list, 0, nList-1);

        if (((int)(rank) + 1) > nList - 1)
            return list[nList - 1];
        else if (rank < 0.0)
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
}


namespace distribution
{
    float normalGauss(TfunctionInput fInput)
    {
        return float(pow((2*PI*pow(fInput.par[1],2)),-0.5)*exp(-0.5*pow((fInput.x-fInput.par[0])/fInput.par[1],2)));
    }
}






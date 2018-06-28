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
    fausto.tomei@gmail.com
    ftomei@arpae.it
*/

#include <math.h>
#include <malloc.h>
#include <float.h>
#include "commonConstants.h"
#include "furtherMathFunctions.h"
#include "basicMath.h"
#include "statistics.h"
#include "physics.h"

namespace elaborations {

float statisticalElab(int elab, float param, std::vector<float> values, int nValues)
{

    switch(elab)
    {
        case ELAB_MEAN:
            return statistics::mean(values, nValues);
        case ELAB_MAX:
            return statistics::maxList(values, nValues);
        case ELAB_MIN:
            return statistics::minList(values, nValues);
        case ELAB_SUM:
            return statistics::sumList(values, nValues);
        case ELAB_SUM_WITH_THRESHOLD:
            return statistics::sumListThreshold(values, nValues, param);
        case ELAB_THRESHOLD_DIFFERENCE:
            return statistics::diffListThreshold(values, nValues, param);
        case ELAB_DAYS_ABOVE_THRESHOLD:
            return statistics::countAbove(values, nValues, param);
        case ELAB_DAYS_UNDER_THRESHOLD:
            return statistics::countBelow(values, nValues, param);
        case ELAB_CONSECUTIVE_DAYS_ABOVE_THRESHOLD:
            return statistics::countConsecutive(values, nValues, param, true);
        case ELAB_CONSECUTIVE_DAYS_UNDER_THRESHOLD:
            return statistics::countConsecutive(values, nValues, param, false);
        case ELAB_PERCENTILE:
            return sorting::percentile(values, &nValues, param, true);
        case ELAB_FREQUENCY_POSITIVE:
            return statistics::frequencyPositive(values, nValues);
        case ELAB_PREVAILING_DIR:
            return windPrevailingDir(values, values, nValues, false);
        case ELAB_TREND:
            return statistics::trend(values, nValues, param);
        case ELAB_MANNKENDALL:
            return statistics::mannKendall(values, nValues);
        case ELAB_EROSIVITY:
            return erosivityFactor(values, nValues);
        case ELAB_RAININTENSITY:
            return rainIntensity(values, nValues);
        case ELAB_STDDEVIATION:
            return statistics::standardDeviation(values, nValues);

        default:
            return NODATA;

    }

}

}
namespace statistics
{

    double rootMeanSquareError(double *measured , double *simulated , int nrData)
    {
        double sigma=0.;
        double diff;
        long nrValidValues = 0;

        for (int i=0; i< nrData; i++)
        {
            if ((measured[i] != NODATA) && (simulated[i] != NODATA))
            {
                diff = measured[i]-simulated[i];
                sigma += (diff * diff);
                nrValidValues++;
            }
        }

        sigma /= nrValidValues;
        sigma = sqrt(sigma);
        return sigma;
    }

    float rootMeanSquareError(float *measured , float *simulated , int nrData)
    {
        double sigma=0.;
        double diff;
        long nrValidValues = 0;

        for (int i=0; i< nrData; i++)
        {
            if ((measured[i] != NODATA) && (simulated[i] != NODATA))
            {
                diff = measured[i]-simulated[i];
                sigma += (diff * diff);
                nrValidValues++;
            }
        }

        sigma /= nrValidValues;
        sigma = sqrt(sigma);
        return float(sigma);
    }

    float coefficientOfVariation(float *measured , float *simulated , int nrData)
    {
        double sigma=0;
        double measuredMean = 0;
        double coefVar;
        double diff;
        for (int i=0; i< nrData; i++)
        {
            diff = measured[i]-simulated[i];
            sigma += diff * diff;
            measuredMean += measured[i];
        }
        sigma /= nrData;
        measuredMean /= nrData;
        sigma = sqrt(sigma);
        coefVar = sigma / measuredMean ;
        return float(coefVar);
    }

    float weighedMean(float *data , float *weights, int nrData)
    {
        float mean=0 ;
        float weightsSum=0 ;
        for (int i = 0 ; i<nrData;i++) weightsSum += weights[i];
        if ((weightsSum< 0.99) || (weightsSum > 1.01)) return -8888 ;
        for (int i = 0 ; i<nrData;i++) mean += weights[i]*data[i];
        return mean ;
    }

    void linearRegression( float* x,  float* y, long nrItems, bool zeroIntercept, float* y_intercept, float* mySlope, float* r2)
    {
       double SUMx = 0;         /*!< sum of x values */
       double SUMy = 0;         /*!< sum of y values */
       double SUMxy = 0;        /*!< sum of x * y */
       double SUMxx = 0;        /*!< sum of x^2 */
       double AVGy = 0;         /*!< mean of y */
       double AVGx = 0;         /*!< mean of x */
       double dy = 0;           /*!< squared of the discrepancies */
       double SUM_dy = 0;       /*!< sum of squared of the discrepancies */
       double SUMres = 0;       /*!< sum of squared residue */
       double res = 0;          /*!< residue squared */

       long nrValidItems = 0;

       *mySlope = 0;             /*!< slope of regression line */
       *y_intercept = 0;         /*!< y intercept of regression line */
       *r2 = 0;                  /*!< coefficient of determination */

       /*! calculate various sums */
       for (int i = 0; i < nrItems; i++)
           if ((x[i] != NODATA) && (y[i] != NODATA))
           {
                nrValidItems++;
                SUMx += x[i];
                SUMy += y[i];
                SUMxy += x[i] * y[i];
                SUMxx += x[i] * x[i];
           }

        /*! means of x and y */
        AVGy = SUMy / nrValidItems;
        AVGx = SUMx / nrValidItems;

        if (!zeroIntercept)
        {
            *mySlope = float((SUMxy - SUMx * SUMy / nrValidItems) / (SUMxx - SUMx * SUMx / nrValidItems));
            *y_intercept = float(AVGy - *mySlope * AVGx);
        }
        else
        {
            *mySlope = float(SUMxy / SUMxx);
            *y_intercept = 0.;
        }

        /*! calculate squared residues and their sum */
        for (int i = 0; i < nrItems; i++)
           if ((x[i] != NODATA) && (y[i] != NODATA))
           {
              /*! sum of squared of the discrepancies */
              dy = y[i] - (*y_intercept + *mySlope * x[i]);
              dy *= dy;
              SUM_dy += dy;

              /*! sum of squared residues */
              res = y[i] - AVGy;
              res *= res;
              SUMres += res;
           }

        /*! calculate r^2 (coefficient of determination) */
        *r2 = float((SUMres - SUM_dy) / SUMres);
    }

    void linearRegression( std::vector<float> x,  std::vector<float> y, long nrItems, bool zeroIntercept, float* y_intercept, float* mySlope, float* r2)
    {
       double SUMx = 0;         /*!< sum of x values */
       double SUMy = 0;         /*!< sum of y values */
       double SUMxy = 0;        /*!< sum of x * y */
       double SUMxx = 0;        /*!< sum of x^2 */
       double AVGy = 0;         /*!< mean of y */
       double AVGx = 0;         /*!< mean of x */
       double dy = 0;           /*!< squared of the discrepancies */
       double SUM_dy = 0;       /*!< sum of squared of the discrepancies */
       double SUMres = 0;       /*!< sum of squared residue */
       double res = 0;          /*!< residue squared */

       long nrValidItems = 0;

       *mySlope = 0;             /*!< slope of regression line */
       *y_intercept = 0;         /*!< y intercept of regression line */
       *r2 = 0;                  /*!< coefficient of determination */

       /*! calculate various sums */
       for (int i = 0; i < nrItems; i++)
           if ((x[i] != NODATA) && (y[i] != NODATA))
           {
                nrValidItems++;
                SUMx += x[i];
                SUMy += y[i];
                SUMxy += x[i] * y[i];
                SUMxx += x[i] * x[i];
           }

        /*! means of x and y */
        AVGy = SUMy / nrValidItems;
        AVGx = SUMx / nrValidItems;

        if (!zeroIntercept)
        {
            *mySlope = float((SUMxy - SUMx * SUMy / nrValidItems) / (SUMxx - SUMx * SUMx / nrValidItems));
            *y_intercept = float(AVGy - *mySlope * AVGx);
        }
        else
        {
            *mySlope = float(SUMxy / SUMxx);
            *y_intercept = 0.;
        }

        /*! calculate squared residues and their sum */
        for (int i = 0; i < nrItems; i++)
           if ((x[i] != NODATA) && (y[i] != NODATA))
           {
              /*! sum of squared of the discrepancies */
              dy = y[i] - (*y_intercept + *mySlope * x[i]);
              dy *= dy;
              SUM_dy += dy;

              /*! sum of squared residues */
              res = y[i] - AVGy;
              res *= res;
              SUMres += res;
           }

        /*! calculate r^2 (coefficient of determination) */
        *r2 = float((SUMres - SUM_dy) / SUMres);
    }

    /*! Variance */
    float variance(float *myList, int nrList)
    {
        float myMean, myDiff, squareDiff;
        int i, nrValidValues;

        if (nrList <= 1) return NODATA;

        myMean = mean(myList,nrList);

        squareDiff = 0;
        nrValidValues = 0;
        for (i = 0; i<nrList; i++)
        {
            if (myList[i]!= NODATA)
            {
                myDiff = (myList[i] - myMean);
                squareDiff += (myDiff * myDiff);
                nrValidValues++;
            }
        }

        if (nrValidValues > 1)
            return (squareDiff / (nrValidValues - 1));
        else
            return NODATA;
    }

    float variance(std::vector<float> myList, int nrList)
    {
        float myMean, myDiff, squareDiff;
        int i, nrValidValues;

        if (nrList <= 1) return NODATA;

        myMean = mean(myList,nrList);

        squareDiff = 0;
        nrValidValues = 0;
        for (i = 0; i<nrList; i++)
        {
            if (myList[i]!= NODATA)
            {
                myDiff = (myList[i] - myMean);
                squareDiff += (myDiff * myDiff);
                nrValidValues++;
            }
        }

        if (nrValidValues > 1)
            return (squareDiff / (nrValidValues - 1));
        else
            return NODATA;
    }

    double variance(double *myList, int nrList)
    {
        double myMean, myDiff, squareDiff;
        int i, nrValidValues;

        if (nrList <= 1) return NODATA;

        myMean = mean(myList,nrList);

        squareDiff = 0;
        nrValidValues = 0;
        for (i = 0; i<nrList; i++)
        {
            if (myList[i]!= NODATA)
            {
                myDiff = (myList[i] - myMean);
                squareDiff += (myDiff * myDiff);
                nrValidValues++;
            }
        }

        if (nrValidValues > 1)
            return (squareDiff / (nrValidValues - 1));
        else
            return NODATA;
    }
    float mean(float *myList, int nrList)
    {
        float sum=0.;
        int i, nrValidValues;

        if (nrList < 1) return NODATA;
        nrValidValues = 0;

        for (i = 0; i < nrList; i++)
        {
            if (myList[i]!= NODATA)
            {
                sum += myList[i];
                nrValidValues++;
            }
        }

        if (nrValidValues > 0)
            return (sum/(float)(nrValidValues));
        else
            return NODATA;
    }
    float mean(std::vector<float> myList, int nrList)
    {
        float sum=0.;
        int i, nrValidValues;

        if (nrList < 1) return NODATA;
        nrValidValues = 0;

        for (i = 0; i < nrList; i++)
        {
            if (myList[i]!= NODATA)
            {
                sum += myList[i];
                nrValidValues++;
            }
        }

        if (nrValidValues > 0)
            return (sum/(float)(nrValidValues));
        else
            return NODATA;
    }
    double mean(double *myList, int nrList)
    {
        double sum=0.;
        int i, nrValidValues;

        if (nrList < 1) return NODATA;
        nrValidValues = 0;

        for (i = 0; i < nrList; i++)
        {
            if (myList[i]!= NODATA)
            {
                sum += myList[i];
                nrValidValues++;
            }
        }

        if (nrValidValues > 0)
            return (sum/(nrValidValues));
        else
            return NODATA;
    }

    float standardDeviation(float *myList, int nrList)
    {
        return sqrtf(variance(myList,nrList));
    }

    float standardDeviation(std::vector<float> myList, int nrList)
    {
        return sqrtf(variance(myList,nrList));
    }

    double standardDeviation(double *myList, int nrList)
    {
        return sqrt(variance(myList,nrList));
    }

    /*! covariance */
    float covariance(float *myList1, int nrList1,float *myList2, int nrList2)
    {
        float myMean1,myMean2,myDiff1,myDiff2,prodDiff;
        int i, nrValidValues;

        if (nrList1 <= 1) return NODATA;
        if (nrList2 <= 1) return NODATA;
        if (nrList2 != nrList1) return NODATA;

        myMean1 = mean(myList1,nrList1);
        myMean2 = mean(myList2,nrList2);
        prodDiff = 0;
        nrValidValues = 0;
        for (i = 0;i<nrList1;i++)
        {
            if ((myList1[i]!= NODATA)&&myList2[i]!=NODATA)
            {
                myDiff1 = (myList1[i] - myMean1);
                myDiff2 = (myList2[i] - myMean2);
                prodDiff += myDiff1*myDiff2;
                nrValidValues++;
            }
        }
        return (prodDiff / (nrValidValues - 1));
    }
    double covariance(double *myList1, int nrList1,double *myList2, int nrList2)
    {
        double myMean1,myMean2,myDiff1,myDiff2,prodDiff;
        int i, nrValidValues;

        if (nrList1 <= 1) return NODATA;
        if (nrList2 <= 1) return NODATA;
        if (nrList2 != nrList1) return NODATA;

        myMean1 = mean(myList1,nrList1);
        myMean2 = mean(myList2,nrList2);
        prodDiff = 0;
        nrValidValues = 0;
        for (i = 0;i<nrList1;i++)
        {
            if ((myList1[i]!= NODATA)&&myList2[i]!=NODATA)
            {
                myDiff1 = (myList1[i] - myMean1);
                myDiff2 = (myList2[i] - myMean2);
                prodDiff += myDiff1*myDiff2;
                nrValidValues++;
            }
        }
        return (prodDiff / (nrValidValues - 1));
    }

    float coefficientPearson(float *myList1, int nrList1,float *myList2, int nrList2)
    {
        return (covariance(myList1,nrList1,myList2,nrList2)/(standardDeviation(myList1,nrList1)*standardDeviation(myList2,nrList2)));
    }

    float** covariancesMatrix(int nrRowCol, float**myLists,int nrLists)
    {
        float** c = (float**)calloc(nrRowCol, sizeof(float*));
        for(int i = 0;i<nrRowCol;i++)
        {
            c[i] = (float*)calloc(nrRowCol, sizeof(float));
        }
        for(int i = 0;i<nrRowCol;i++)
        {
            c[i][i]= variance(myLists[i],nrLists);
            for(int j = i+1;j<nrRowCol;j++)
            {
                c[i][j]= covariance(myLists[i],nrLists,myLists[j],nrLists);
                c[j][i]=c[i][j];
            }

        }
        return c;
    }

    void correlationsMatrix(int nrRowCol, double**myLists,int nrLists, double** c)
    {
        // input: myLists matrix
        // output: c matrix
        for(int i = 0;i<nrRowCol;i++)
        {
            c[i][i]=1.;
            for(int j = i+1;j<nrRowCol;j++)
            {
                c[i][j]= covariance(myLists[i],nrLists,myLists[j],nrLists);
                if (c[i][j] != 0) c[i][j] /= sqrt(variance(myLists[i],nrLists)*variance(myLists[j],nrLists));
                c[j][i] = c[i][j];
            }

        }
    }

    double ERF(double x, double accuracy) // error function
    {

        return (1.12837916709551*double(integration::qsimp(errorFunctionPrimitive,0.,float(x),float(accuracy)))); // the constant in front of integration is equal to 2*pow(PI,-0.5)
    }

    double ERFC(double x, double accuracy) // error function
    {
        return (1. - ERF(x, accuracy));
    }

    double inverseERF(double value, double accuracy)
    {

        if (value >=1 || value <= -1)
        {
            return PARAMETER_ERROR;
        }
        double root;

        if (value == 0)
        {
            return 0.;
        }
        else if (value  > 0)
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

        if (value == 1)
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

    float maxList(std::vector<float> values, int nValue)
    {

        float max = -FLT_MAX;

        if (nValue == 0)
            return NODATA;

        for (int i = 0; i < nValue; i++)
        {
            if (values[i] > max)
            {
                max = values[i] ;
            }
        }

        return max;
    }


    float minList(std::vector<float> values, int nValue)
    {

        float min = FLT_MAX;

        if (nValue == 0)
            return NODATA;

        for (int i = 0; i < nValue; i++)
        {
            if (values[i] < min)
            {
                min = values[i] ;
            }
        }

        return min;
    }

    float sumList(std::vector<float> values, int nValue)
    {

        float sum = 0;

        if (nValue == 0)
            return NODATA;

        for (int i = 0; i < nValue; i++)
        {
            sum = sum  + values[i] ;
        }

        return sum;
    }

    float sumListThreshold(std::vector<float> values, int nValue, float threshold)
    {

        float sum = 0;

        if (nValue == 0 || threshold == NODATA)
            return NODATA;

        for (int i = 0; i < nValue; i++)
        {
            if (values[i] > threshold)
            {
                sum = sum  + ( values[i] - threshold);
            }
        }

        return sum;
    }

    float diffListThreshold(std::vector<float> values, int nValue, float threshold)
    {

        float diff = 0;

        if (nValue == 0 || threshold == NODATA)
            return NODATA;

        for (int i = 0; i < nValue; i++)
        {
            if (values[i] < threshold)
            {
                diff = diff  + (threshold - values[i]);
            }
        }

        return diff;
    }


    float countAbove(std::vector<float> values, int nValue, float threshold)
    {

        float countAbove = 0;

        if (nValue == 0 || threshold == NODATA)
            return NODATA;

        for (int i = 0; i < nValue; i++)
        {
            if (values[i] > threshold)
            {
                countAbove = countAbove + 1;
            }
        }

        return countAbove;
    }

    float countBelow(std::vector<float> values, int nValue, float threshold)
    {

        float countBelow = 0;

        if (nValue == 0 || threshold == NODATA)
            return NODATA;

        for (int i = 0; i < nValue; i++)
        {
            if (values[i] < threshold)
            {
                countBelow = countBelow + 1;
            }
        }

        return countBelow;
    }

    float countConsecutive(std::vector<float> values, int nValue, float threshold, bool isPositive)
    {

        float countConsecutive = 0;

        if (nValue == 0 || threshold == NODATA)
            return NODATA;

        bool inPeriod = false;
        int nPeriod = 0;

        std::vector<float> myListNumDays;
        myListNumDays.push_back(0);

        for (int i = 0; i < nValue; i++)
        {
            if (values[i] != NODATA)
            {
                if (inPeriod == false)
                {
                    if (compareValue( values[i], threshold, isPositive))
                    {
                        inPeriod = true;
                        myListNumDays[nPeriod] = myListNumDays[nPeriod] + 1;
                    }
                }
                else
                {
                    if (compareValue( values[i], threshold, isPositive))
                    {
                        myListNumDays[nPeriod] = myListNumDays[nPeriod] + 1;
                        if (i == (nValue - 1))
                        {
                            nPeriod = nPeriod + 1;
                            myListNumDays.push_back(0);
                        }
                    }
                    else
                    {
                        nPeriod = nPeriod + 1;
                        myListNumDays.push_back(0);
                        inPeriod = false;
                    }
                }
            }
            else if (inPeriod == true)
            {
                nPeriod = nPeriod + 1;
                myListNumDays.push_back(0);
                inPeriod = false;
            }
        }
        if (nPeriod == 0)
        {
            countConsecutive = 0;
        }
        else
        {
            countConsecutive = statistics::maxList(myListNumDays, nPeriod);
        }

        return countConsecutive;

    }

    float frequencyPositive(std::vector<float> values, int nValue)
    {

        if (nValue <= 0)
            return NODATA;

        float frequencyPositive = 0;

        for (int i = 0; i < nValue; i++)
        {
            if ( values[i] > 0)
                frequencyPositive = frequencyPositive + 1;

        }
        frequencyPositive = frequencyPositive / nValue;

        return frequencyPositive;

    }

    float trend(std::vector<float> values, int nValues, float myFirstYear)
    {

        float trend;
        float r2, y_intercept;


        if (nValues < 2 || myFirstYear == NODATA)
            return NODATA;

        std::vector<float> myYears(nValues);

        for (int i = 0; i < nValues; i++)
        {
            myYears[i] = (myFirstYear + i) ;
        }

        statistics::linearRegression(myYears, values, nValues, false, &y_intercept, &trend, &r2);
        return trend;

    }

    float mannKendall(std::vector<float> values, int nValues)
    {

        double myVar;
        float zMK;

        // minimum 3 values
        if (nValues < 3)
            return NODATA;

        int myS = 0;
        int myValidNR = 0;

        for (int i = 0; i < nValues - 1; i++)
        {
             if ( values[i] != NODATA )
             {
                for (int j = i + 1; j < nValues ; j++)
                {
                    if (values[j] != NODATA)
                    {
                        myS = myS + sgn(values[j] - values[i]);
                    }
                }
                myValidNR = myValidNR + 1;
            }
        }
        myVar = myValidNR * (myValidNR - 1) * (2.0 * myValidNR + 5.0) / 18.0;

        if (myS > 0)
        {
            zMK = static_cast<float>((myS - 1) / sqrt(myVar));
        }
        else if (myS < 0)
        {
            zMK = -static_cast<float>((myS + 1) / sqrt(myVar));
        }
        else
        {
            return 0;
        }

        int stdDev = 1;
        int mean = 0;

        double sumGauss = 0;
        double deltaXGauss = 1 / 1000;
        double myX = 0;
        std::vector<float> GaussIntegralTwoTailsFactor1000(1000);

        for (unsigned int i = 0; i < 1000; i++)
        {
            myX = myX + deltaXGauss;
            double rapporto = (myX - mean) / stdDev;
            double gauss = (1 / (sqrt(2 * PI) * stdDev)) * exp(-0.5 * (rapporto * rapporto));
            sumGauss = sumGauss + gauss * deltaXGauss;
            GaussIntegralTwoTailsFactor1000[i] = (sumGauss * 2.f);
        }

        return GaussIntegralTwoTailsFactor1000[int(zMK * 1000)];

    }

}

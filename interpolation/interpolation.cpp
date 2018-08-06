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

    contacts:
    fausto.tomei@gmail.com
    ftomei@arpae.it
*/


#include <stdlib.h>

#include <math.h>
#include <vector>

#include "commonConstants.h"
#include "basicMath.h"
#include "statistics.h"
#include "furtherMathFunctions.h"
#include "meteoPoint.h"
#include "gis.h"
#include "interpolation.h"


using namespace std;


void assignDistances(vector <Crit3DInterpolationDataPoint> &myPoints, float x, float y, float z)
{
    for (unsigned long i = 0; i < myPoints.size() ; i++)
    {
        myPoints.at(i).distance = gis::computeDistance(x, y, float((myPoints.at(i)).point->utm.x) , float((myPoints.at(i)).point->utm.y));
        myPoints.at(i).deltaZ = float(fabs(myPoints.at(i).point->z - z));
    }
}

float getMinHeight(std::vector <Crit3DInterpolationDataPoint> &myInterpolationPoints)
{
    float myZmin = NODATA;

    for (long i = 0; i < long(myInterpolationPoints.size()); i++)
        if (myInterpolationPoints.at(i).point->z != NODATA)
            if (myZmin == NODATA || myInterpolationPoints.at(i).point->z < myZmin)
                myZmin = float(myInterpolationPoints.at(i).point->z);
    return myZmin;
}

float getMaxHeight(std::vector <Crit3DInterpolationDataPoint> &myInterpolationPoints)
{
    float zMax;
    zMax = NODATA;

    for (long i = 1; i < long(myInterpolationPoints.size()); i++)
        if (myInterpolationPoints.at(i).value != NODATA && myInterpolationPoints.at(i).isActive)
            if (zMax == NODATA || (myInterpolationPoints.at(i)).point->z > zMax)
                zMax = float(myInterpolationPoints.at(i).point->z);


    return zMax;
}

int sortPointsByDistance(int maxIndex, vector <Crit3DInterpolationDataPoint> &myPoints, vector <Crit3DInterpolationDataPoint> &myValidPoints)
{   
    int i, first, index;
    float min_value;
    int* indici_ordinati;
    int* indice_minimo;
    int outIndex;

    if (myPoints.size() == 0) return 0;

    indici_ordinati = (int *) calloc(maxIndex, sizeof(int));
    indice_minimo = (int *) calloc(myPoints.size(), sizeof(int));

    first = 0;
    index = 0;

    bool exit = false;

    while (index < maxIndex && !exit)
    {
        if (first == 0)
        {
            i = 0;
            while ((! myPoints.at(i).isActive || (myPoints.at(i).distance == 0)) && (i < int(myPoints.size())-1))
                i++;

            if (i == int(myPoints.size())-1 && ! myPoints.at(i).isActive)
                exit=true;
            else
            {
                first = 1;
                indice_minimo[first-1] = i;
                min_value = myPoints.at(i).distance;
            }
        }
        else
            min_value = myPoints.at(indice_minimo[first-1]).distance;

        if (!exit)
        {
            for (i = indice_minimo[first-1] + 1; i < int(myPoints.size()); i++)
                if (myPoints.at(i).distance < min_value)
                    if (myPoints.at(i).isActive)
                        if (myPoints.at(i).distance > 0)
                        {
                            first++;
                            min_value = myPoints.at(i).distance;
                            indice_minimo[first-1] = i;
                        }

            indici_ordinati[index] = indice_minimo[first-1];
            myPoints.at(indice_minimo[first-1]).isActive = false;
            index++;
            first--;
        }
    }

    outIndex = minValue(index, maxIndex);
    myValidPoints.resize(outIndex);

    for (i=0; i<outIndex; i++)
    {
        myPoints.at(indici_ordinati[i]).isActive = true;
        myValidPoints.at(i) = myPoints.at(indici_ordinati[i]);
    }

    return outIndex;
}

float shepardFindRadius(Crit3DInterpolationSettings* mySettings,
                        std::vector <Crit3DInterpolationDataPoint> myPoints,
                        std::vector <Crit3DInterpolationDataPoint> myOutPoints)
{
    int i;
    std::vector <Crit3DInterpolationDataPoint> neighbourPoints;
    neighbourPoints.clear();

    // define a first neighborhood inside initial radius
    for (i=1; i<myPoints.size(); i++)
        if (myPoints.at(i).distance <= mySettings->getShepardInitialRadius() && myPoints.at(i).distance > 0 && myPoints.at(i).index != mySettings->getIndexPointCV())
            neighbourPoints.push_back(myPoints.at(i));

    if (neighbourPoints.size() <= SHEPARD_MIN_NRPOINTS)
    {
        sortPointsByDistance(SHEPARD_MIN_NRPOINTS + 1, myPoints, myOutPoints);
        if (myOutPoints.size() > SHEPARD_MIN_NRPOINTS)
        {
            myOutPoints.pop_back();
            return myOutPoints.at(SHEPARD_MIN_NRPOINTS).distance;
        }
        else
            return myOutPoints.at(myOutPoints.size()-1).distance + 1;
    }
    else if (neighbourPoints.size() > SHEPARD_MAX_NRPOINTS)
    {
        sortPointsByDistance(SHEPARD_MAX_NRPOINTS + 1, neighbourPoints, myOutPoints);
        myOutPoints.pop_back();
        return myOutPoints.at(SHEPARD_MAX_NRPOINTS).distance;
    }
    else
    {
        myOutPoints = neighbourPoints;
        return mySettings->getShepardInitialRadius();
    }
}

bool neighbourhoodVariability(std::vector <Crit3DInterpolationDataPoint> &myInterpolationPoints, float x, float y, float z, int nMax,
                              float* devSt, float* devStDeltaZ, float* minDistance)
{
    int i, max_points;
    float* dataNeighborhood;
    float myValue;
    vector <float> deltaZ;
    vector <Crit3DInterpolationDataPoint> validPoints;

    assignDistances(myInterpolationPoints, x, y, z);
    max_points = sortPointsByDistance(nMax, myInterpolationPoints, validPoints);

    if (max_points > 1)
    {
        dataNeighborhood = (float *) calloc (max_points, sizeof(float));

        for (i=0; i<max_points; i++)
        {
            myValue = validPoints.at(i).value;
            dataNeighborhood[i] = myValue;
        }

        *devSt = statistics::standardDeviation(dataNeighborhood, max_points);

        *minDistance = validPoints.at(0).distance;

        deltaZ.clear();
        if (z != NODATA)
            deltaZ.push_back(1);

        for (i=0; i<max_points;i++)
            if ((validPoints.at(i)).point->z != NODATA)
                deltaZ.push_back(fabs(((float)(validPoints.at(i)).point->z) - z));

        *devStDeltaZ = statistics::mean(deltaZ.data(), int(deltaZ.size()));

        return true;
    }
    else
        return false;
}


void regressionSimple(std::vector <Crit3DInterpolationDataPoint> &myPoints,
                      int proxyPosition, bool isZeroIntercept, float* myCoeff, float* myIntercept, float* myR2)
{
    long i;
    float myProxyValue;
    Crit3DInterpolationDataPoint myPoint;
    vector <float> myValues, myZ;

    *myCoeff = NODATA;
    *myIntercept = NODATA;
    *myR2 = NODATA;

    myValues.clear();
    myZ.clear();

    for (i = 0; i < long(myPoints.size()); i++)
    {
        myPoint = myPoints.at(i);
        myProxyValue = NODATA;
        if (myPoint.isActive)
        {
            myProxyValue = myPoint.getProxy(proxyPosition);
            if (myProxyValue != NODATA)
            {
                myValues.push_back(myPoint.value);
                myZ.push_back(myProxyValue);
            }
        }
    }

    if (myValues.size() >= MIN_REGRESSION_POINTS)
        statistics::linearRegression((float*)(myZ.data()), (float*)(myValues.data()), (long)(myZ.size()), isZeroIntercept,
                                     myIntercept, myCoeff, myR2);
}

bool regressionGeneric(std::vector <Crit3DInterpolationDataPoint> &myPoints, Crit3DInterpolationSettings* mySettings,
                       int proxyPos, bool isZeroIntercept)
{
    float q, m, r2;

    regressionSimple(myPoints, proxyPos, isZeroIntercept, &m, &q, &r2);
    Crit3DProxyInterpolation* myProxy = mySettings->getProxy(proxyPos);

    myProxy->setRegressionSlope(m);
    myProxy->setRegressionR2(r2);

    return (r2 >= mySettings->getMinRegressionR2());
}


bool regressionSimpleT(std::vector <Crit3DInterpolationDataPoint> &myPoints, Crit3DInterpolationSettings* mySettings,
                       meteoVariable myVar, int orogProxyPos)
{
    float q, m, r2;

    Crit3DProxyInterpolation* myProxyOrog = mySettings->getProxy(orogProxyPos);
    myProxyOrog->initializeOrography();

    regressionSimple(myPoints, orogProxyPos, false, &m, &q, &r2);

    if (r2 < mySettings->getMinRegressionR2())
        return false;

    myProxyOrog->setRegressionSlope(m);
    myProxyOrog->setRegressionR2(r2);

    // only pre-inversion data
    if (m > 0)
    {
        myProxyOrog->setInversionLapseRate(m);

        float maxZ = minValue(getMaxHeight(myPoints), mySettings->getMaxHeightInversion());
        myProxyOrog->setLapseRateH1(maxZ);
        myProxyOrog->setRegressionSlope(mySettings->getCurrentClimateLapseRate(myVar));
        myProxyOrog->setInversionIsSignificative(true);
    }

    return true;
}


float findHeightIntervalAvgValue(std::vector <Crit3DInterpolationDataPoint> &myPoints,
                                 float heightInf, float heightSup, float maxPointsZ)
{
    long myIndex;
    float myValue, mySum, nValues;

    mySum = 0.;
    nValues = 0;

    for (myIndex = 0; myIndex < long(myPoints.size()); myIndex++)
         if (myPoints.at(myIndex).point->z != NODATA && myPoints.at(myIndex).isActive)
            if (myPoints.at(myIndex).point->z >= heightInf && myPoints.at(myIndex).point->z <= heightSup)
            {
                myValue = (myPoints.at(myIndex)).value;
                if (myValue != NODATA)
                {
                    mySum += myValue;
                    nValues ++;
                }
            }

    if (nValues > 1 || (nValues > 0 && heightSup >= maxPointsZ))
        return (mySum / nValues);
    else
        return NODATA;
}

bool regressionOrographyT(std::vector <Crit3DInterpolationDataPoint> &myPoints, Crit3DInterpolationSettings* mySettings,
                          meteoVariable myVar, int orogProxyPos, bool climateExists)
{
    long i;
    float heightInf, heightSup;
    float myAvg;
    vector <float> myData1, myData2;
    vector <float> myHeights1, myHeights2;
    vector <float> myIntervalsHeight, myIntervalsHeight1, myIntervalsHeight2;
    vector <float> myIntervalsValues, myIntervalsValues1, myIntervalsValues2;
    float m, q, r2;
    float r2_values, r2_intervals;
    float q1, m1, r21, q2, m2, r22;
    float mySignificativeR2, mySignificativeR2Inv;
    float maxPointsZ, deltaZ;
    float climateLapseRate, lapseRateT0, lapseRateT1, lapseRateH1_;
    float DELTAZ_INI = 80.;
    float maxHeightInv = mySettings->getMaxHeightInversion();

    Crit3DProxyInterpolation* myProxyOrog = mySettings->getProxy(orogProxyPos);

    mySignificativeR2 = maxValue(mySettings->getMinRegressionR2(), float(0.2));
    mySignificativeR2Inv = maxValue(mySettings->getMinRegressionR2(), float(0.1));

    /*! initialize */
    myProxyOrog->initializeOrography();

    if (climateExists)
        climateLapseRate = mySettings->getCurrentClimateLapseRate(myVar);
    else
        climateLapseRate = 0.;

    myProxyOrog->setRegressionSlope(climateLapseRate);

    maxPointsZ = getMaxHeight(myPoints);

    /*! not enough data to define a curve (use climate) */
    if (myPoints.size() < MIN_REGRESSION_POINTS)
        return true;

    /*! find intervals averages */
    heightInf = getMinHeight(myPoints);
    heightSup = heightInf;
    deltaZ = DELTAZ_INI;
    while (heightSup <= maxPointsZ)
    {
        myAvg = NODATA;
        while (myAvg == NODATA)
        {
            heightSup = heightSup + deltaZ;
            if (heightSup > maxPointsZ && !(maxPointsZ == 0 && heightInf == 0))
                heightSup = maxPointsZ + (maxPointsZ - heightInf);

            myAvg = findHeightIntervalAvgValue(myPoints, heightInf, heightSup, maxPointsZ);
        }
        myIntervalsHeight.push_back((heightSup + heightInf) / float(2.));
        myIntervalsValues.push_back(myAvg);

        deltaZ = DELTAZ_INI * exp(heightInf / maxHeightInv);
        heightInf = heightSup;
    }

    /*! find inversion height */
    lapseRateT1 = myIntervalsValues.at(0);
    myProxyOrog->setLapseRateH1(myIntervalsHeight.at(0));
    for (i = 1; i < long(myIntervalsValues.size()); i++)
        if (myIntervalsHeight.at(i) <= maxHeightInv && (myIntervalsValues.at(i) >= lapseRateT1) && (myIntervalsValues.at(i) > (myIntervalsValues.at(0) + 0.001 * (myIntervalsHeight.at(i) - myIntervalsHeight.at(0)))))
        {
            myProxyOrog->setLapseRateH1(myIntervalsHeight.at(i));
            lapseRateT1 = myIntervalsValues.at(i);
            myProxyOrog->setInversionIsSignificative(true);
        }

    /*! no inversion: try regression with all data */
    if (! myProxyOrog->getInversionIsSignificative())
        return (regressionGeneric(myPoints, mySettings, orogProxyPos, false));

    /*! create vectors below and above inversion */
    for (i = 0; i < long(myPoints.size()); i++)
        if (myPoints.at(i).point->z != NODATA && myPoints.at(i).isActive)
        {
            if (myPoints.at(i).point->z <= myProxyOrog->getLapseRateH1())
            {
                myData1.push_back(myPoints.at(i).value);
                myHeights1.push_back(float(myPoints.at(i).point->z));
            }
            else
            {
                myData2.push_back(myPoints.at(i).value);
                myHeights2.push_back(float(myPoints.at(i).point->z));
            }
        }


    /*! create vectors of height intervals below and above inversion */
    for (i = 0; i < long(myIntervalsValues.size()); i++)
        if (myIntervalsHeight.at(i) <= myProxyOrog->getLapseRateH1())
        {
            myIntervalsValues1.push_back(myIntervalsValues.at(i));
            myIntervalsHeight1.push_back(myIntervalsHeight.at(i));
        }
        else
        {
            myIntervalsValues2.push_back(myIntervalsValues.at(i));
            myIntervalsHeight2.push_back(myIntervalsHeight.at(i));
        }


    /*! only positive lapse rate*/
    if (myProxyOrog->getInversionIsSignificative() && myIntervalsValues1.size() == myIntervalsValues.size())
    {
        regressionSimple(myPoints, orogProxyPos, false, &m, &q, &r2);

        if (r2 >= mySignificativeR2)
        {
            myProxyOrog->setInversionLapseRate(m);
            myProxyOrog->setRegressionR2(r2);
            lapseRateT0 = q;
            lapseRateT1 = q + m * myProxyOrog->getLapseRateH1();
        }
        else
        {
            statistics::linearRegression(myIntervalsHeight1.data(), myIntervalsValues1.data(), (long)myIntervalsHeight1.size(), false, &q, &m, &r2);

            myProxyOrog->setRegressionR2(NODATA);

            if (r2 >= mySignificativeR2)
            {
                lapseRateT0 = q;
                myProxyOrog->setInversionLapseRate(m);
                lapseRateT1 = q + m * myProxyOrog->getLapseRateH1();
            }
            else
            {
                myProxyOrog->setInversionLapseRate(0.);
                lapseRateT0 = myIntervalsValues.at(0);
                lapseRateT1 = lapseRateT0;
            }
        }

        return true;
    }

    /*! check inversion significance */
    statistics::linearRegression(myHeights1.data(), myData1.data(), long(myHeights1.size()), false, &q1, &m1, &r2_values);
    if (myIntervalsValues1.size() > 2)
        statistics::linearRegression(myIntervalsHeight1.data(), myIntervalsValues1.data(), long(myIntervalsHeight1.size()), false, &q, &m, &r2_intervals);
    else
        r2_intervals = 0.;

    /*! inversion is not significant with data neither with intervals */
    if (r2_values < mySignificativeR2Inv && r2_intervals < mySignificativeR2Inv)
    {
        regressionSimple(myPoints, orogProxyPos, false, &m, &q, &r2);

        /*! case 0: regression with all data much significant */
        if (r2 >= 0.5)
        {
            myProxyOrog->setInversionIsSignificative(false);
            myProxyOrog->setLapseRateH1(NODATA);
            myProxyOrog->setInversionLapseRate(NODATA);
            myProxyOrog->setRegressionSlope(minValue(m, float(0.0)));
            myProxyOrog->setRegressionR2(r2);
            return true;
        }

        /*! case 1: analysis only above inversion, flat lapse rate below */
        myProxyOrog->setInversionLapseRate(0.);
        statistics::linearRegression(myHeights2.data(), myData2.data(), long(myHeights2.size()), false, &q2, &m2, &r2);

        if (myData2.size() >= MIN_REGRESSION_POINTS)
        {
            if (r2 >= mySignificativeR2)
            {
                myProxyOrog->setRegressionSlope(minValue(m2, float(0.0)));
                lapseRateT1 = q2 + myProxyOrog->getLapseRateH1() * myProxyOrog->getRegressionSlope();
                lapseRateT0 = lapseRateT1;
                myProxyOrog->setRegressionR2(r2);
                return true;
            }
            else
            {
                statistics::linearRegression(myIntervalsHeight2.data(), myIntervalsValues2.data(),
                                             int(myIntervalsHeight2.size()), false, &q2, &m2, &r2);
                if (r2 >= mySignificativeR2)
                {
                    myProxyOrog->setRegressionSlope(minValue(m2, float(0.0)));
                    lapseRateT1 = q2 + myProxyOrog->getLapseRateH1() * myProxyOrog->getRegressionSlope();
                    lapseRateT0 = lapseRateT1;
                    return true;
                }
            }
        }

        myProxyOrog->setInversionIsSignificative(false);
        myProxyOrog->setLapseRateH1(NODATA);
        myProxyOrog->setInversionLapseRate(NODATA);
        lapseRateT0 = q;
        lapseRateT1 = NODATA;

        /*! case 2: regression with data */
        regressionSimple(myPoints, orogProxyPos, false, &m, &q, &r2);

        if (r2 >= mySignificativeR2)
        {
            myProxyOrog->setRegressionSlope(minValue(m, 0));
            lapseRateT0 = q;
            myProxyOrog->setRegressionR2(r2);
            return true;
        }
        else
        {
            lapseRateT0 = myIntervalsValues.at(0);
            if (m > 0.)
                myProxyOrog->setRegressionSlope(0.);
            else
                myProxyOrog->setRegressionSlope(climateLapseRate);

            return true;
        }

    }

    /*! significance analysis */
    statistics::linearRegression(myHeights1.data(), myData1.data(), int(myHeights1.size()), false, &q1, &m1, &r21);
    statistics::linearRegression(myHeights2.data(), myData2.data(), int(myHeights2.size()), false, &q2, &m2, &r22);

    myProxyOrog->setRegressionR2(r22);

    if (r21 >= mySignificativeR2Inv && r22 >= mySignificativeR2)
    {
        if (myHeights2.size() < MIN_REGRESSION_POINTS && m2 > 0.)
        {
            m2 = 0.;
            q2 = lapseRateT1;
        }
        findLinesIntersection(q1, m1, q2, m2, &lapseRateH1_, &lapseRateT1);
        lapseRateT0 = q1;
        myProxyOrog->setInversionLapseRate(m1);
        myProxyOrog->setRegressionSlope(m2);
        myProxyOrog->setLapseRateH1(lapseRateH1_);
        if (myProxyOrog->getLapseRateH1() > maxHeightInv)
        {
            lapseRateT1 = lapseRateT1 - (myProxyOrog->getLapseRateH1() - maxHeightInv) * myProxyOrog->getRegressionSlope();
            myProxyOrog->setLapseRateH1(maxHeightInv);
            myProxyOrog->setInversionLapseRate((lapseRateT1 - lapseRateT0) / (myProxyOrog->getLapseRateH1() - myProxyOrog->getLapseRateH0()));
        }
        return true;
    }
    else if (r21 < mySignificativeR2Inv && r22 >= mySignificativeR2)
    {
        if (myHeights2.size() < MIN_REGRESSION_POINTS && m2 > 0.)
        {
            m2 = 0.;
            q2 = lapseRateT1;
        }

        statistics::linearRegression(myIntervalsHeight1.data(), myIntervalsValues1.data(),
                                     long(myIntervalsHeight1.size()), false, &q, &m, &r2);

        myProxyOrog->setRegressionSlope(m2);
        if (r2 >= mySignificativeR2Inv)
        {
            if (findLinesIntersectionAboveThreshold(q, m, q2, m2, 40, &lapseRateH1_, &lapseRateT1))
            {
                myProxyOrog->setLapseRateH1(lapseRateH1_);
                lapseRateT0 = q;
                myProxyOrog->setInversionLapseRate(m);
                if (myProxyOrog->getLapseRateH1() > maxHeightInv)
                {
                    lapseRateT1 = lapseRateT1 - (myProxyOrog->getLapseRateH1() - maxHeightInv) * myProxyOrog->getRegressionSlope();
                    myProxyOrog->setLapseRateH1(maxHeightInv);
                    myProxyOrog->setInversionLapseRate((lapseRateT1 - lapseRateT0) / (myProxyOrog->getLapseRateH1() - myProxyOrog->getLapseRateH0()));
                }
                return true;
            }
        }
        else
        {
            myProxyOrog->setInversionLapseRate(0.);
            lapseRateT1 = q2 + m2 * myProxyOrog->getLapseRateH1();
            lapseRateT0 = lapseRateT1;
            return true;
        }
    }

    else if (r21 >= mySignificativeR2Inv && r22 < mySignificativeR2)
    {
        lapseRateT0 = q1;
        myProxyOrog->setInversionLapseRate(m1);

        statistics::linearRegression(myIntervalsHeight2.data(), myIntervalsValues2.data(),
                                     long(myIntervalsHeight2.size()), false, &q, &m, &r2);
        if (r2 >= mySignificativeR2)
        {
            myProxyOrog->setRegressionSlope(minValue(m, float(0.)));
            findLinesIntersection(lapseRateT0, myProxyOrog->getInversionLapseRate(), q, myProxyOrog->getRegressionSlope(), &lapseRateH1_, &lapseRateT1);
            myProxyOrog->setLapseRateH1(lapseRateH1_);
        }
        else
        {
            myProxyOrog->setRegressionSlope(climateLapseRate);
            findLinesIntersection(lapseRateT0, myProxyOrog->getInversionLapseRate(), lapseRateT1 - myProxyOrog->getRegressionSlope()* myProxyOrog->getLapseRateH1(), myProxyOrog->getRegressionSlope(), &lapseRateH1_, &lapseRateT1);
            myProxyOrog->setLapseRateH1(lapseRateH1_);
        }
        return true;
    }

    else if (r21 < mySignificativeR2Inv && r22 < mySignificativeR2)
    {
        statistics::linearRegression(myIntervalsHeight1.data(), myIntervalsValues1.data(),
                                     long(myIntervalsHeight1.size()), false, &q, &m, &r2);

        if (r2 >= mySignificativeR2Inv)
        {
            lapseRateT0 = q;
            myProxyOrog->setInversionLapseRate(m);
            lapseRateT1 = q + m * myProxyOrog->getLapseRateH1();
        }
        else
        {
            myProxyOrog->setInversionLapseRate(0.);
            lapseRateT0 = myIntervalsValues.at(0);
            lapseRateT1 = lapseRateT0;
        }

        statistics::linearRegression(myIntervalsHeight2.data(), myIntervalsValues2.data(),
                                     long(myIntervalsHeight2.size()), false, &q, &m, &r2);

        if (r2 >= mySignificativeR2)
        {
            myProxyOrog->setRegressionSlope(minValue(m, 0));
            if (findLinesIntersectionAboveThreshold(lapseRateT0, myProxyOrog->getInversionLapseRate(), q, myProxyOrog->getRegressionSlope(), 40, &lapseRateH1_, &lapseRateT1))
            {
                myProxyOrog->setLapseRateH1(lapseRateH1_);
                return true;
            }
        }
        else
        {
            myProxyOrog->setRegressionSlope(climateLapseRate);
            return true;
        }

    }

    /*! check max lapse rate (20 C / 1000 m) */
    if (myProxyOrog->getRegressionSlope() < -0.02)
        myProxyOrog->setRegressionSlope((float)-0.02);

    myProxyOrog->initializeOrography();
    return (regressionGeneric(myPoints, mySettings, orogProxyPos, false));

}

float computeShepard(vector <Crit3DInterpolationDataPoint> myPoints, Crit3DInterpolationSettings* settings, float X, float Y)
{
    std::vector <Crit3DInterpolationDataPoint> validPoints;
    std::vector <Crit3DInterpolationDataPoint> neighbourPoints;
    int i;
    float radius;

    neighbourPoints.clear();

    // define a first neighborhood inside initial radius
    for (i=1; i<myPoints.size(); i++)
        if (myPoints.at(i).distance <= settings->getShepardInitialRadius() && myPoints.at(i).distance > 0 && myPoints.at(i).index != settings->getIndexPointCV())
            neighbourPoints.push_back(myPoints.at(i));

    if (neighbourPoints.size() <= SHEPARD_MIN_NRPOINTS)
    {
        sortPointsByDistance(SHEPARD_MIN_NRPOINTS, myPoints, validPoints);
        if (validPoints.size() > SHEPARD_MIN_NRPOINTS)
        {
            validPoints.pop_back();
            radius = validPoints.at(SHEPARD_MIN_NRPOINTS).distance;
        }
        else
            radius = validPoints.at(validPoints.size()-1).distance + 1;
    }
    else if (neighbourPoints.size() > SHEPARD_MAX_NRPOINTS)
    {
        sortPointsByDistance(SHEPARD_MAX_NRPOINTS + 1, neighbourPoints, validPoints);
        validPoints.pop_back();
        radius = validPoints.at(SHEPARD_MAX_NRPOINTS).distance;
    }
    else
    {
        validPoints = neighbourPoints;
        radius = settings->getShepardInitialRadius();
    }

    int j;
    float weightSum, radius_27_4, radius_3, tmp, cosine, result;
    vector <float> weight, t, S;

    weight.resize(validPoints.size());
    t.resize(validPoints.size());
    S.resize(validPoints.size());

    weightSum = 0;
    radius_3 = radius / 3;
    radius_27_4 = (27 / 4) / radius;
    for (i=0; i<validPoints.size(); i++)
        if (validPoints.at(i).distance > 0)
        {
            if (validPoints.at(i).distance <= radius_3)
                S.at(i) = 1 / (validPoints.at(i).distance);
            else if (validPoints.at(i).distance <= radius)
            {
                tmp = (validPoints.at(i).distance / radius) - 1;
                S.at(i) = radius_27_4 * tmp * tmp;
            }
            else
                S.at(i) = 0;

            weightSum = weightSum + S.at(i);
        }

    if (weightSum == 0)
        return NODATA;

    // including direction
    for (i=0; i<validPoints.size(); i++)
    {
        t.at(i) = 0;
        for (j=0; j<validPoints.size(); j++)
            if (i != j)
            {
                cosine = ((X - (float)validPoints.at(i).point->utm.x) * (X - (float)validPoints.at(j).point->utm.x) + (Y - (float)validPoints.at(i).point->utm.y) * (Y - (float)validPoints.at(j).point->utm.y)) / (validPoints.at(i).distance * validPoints.at(j).distance);
                t.at(i) = t.at(i) + S.at(j) * (1 - cosine);
            }

        if (weightSum != 0)
            t.at(i) /= weightSum;
    }

    // weights
    weightSum = 0;
    for (i=0; i<validPoints.size(); i++)
    {
       weight.at(i) = S.at(i) * S.at(i) * (1 + t.at(i));
       weightSum += weight.at(i);
    }
    for (i=0; i<validPoints.size(); i++)
        weight.at(i) /= weightSum;

    result = 0;
    for (i=0; i<validPoints.size(); i++)
        result += weight.at(i) * validPoints.at(i).value;

    return result;
}

float inverseDistanceWeighted(vector <Crit3DInterpolationDataPoint> &myPointList)
{
    double sum, sumWeights, weight;

    sum = 0 ;
    sumWeights = 0 ;
    for (int i = 0 ; i < (int)(myPointList.size()); i++)
    {
        Crit3DInterpolationDataPoint myPoint = myPointList.at(i);
        if (myPoint.distance > 0.)
        {
            weight = myPoint.distance / 10000. ;
            weight = fabs(1 / (weight * weight * weight));
            sumWeights += weight;
            sum += myPoint.value * weight;
        }
    }

    if (sumWeights > 0.0)
        return float(sum / sumWeights);
    else
        return NODATA;
}

float gaussWeighted(vector <Crit3DInterpolationDataPoint> &myPointList)
{
    double sum, sumWeights, weight;
    double distance, deltaZ;
    double Rd=10;
    double Rz=1;

    sum = 0 ;
    sumWeights = 0 ;
    for (int i = 0 ; i < (int)(myPointList.size()); i++)
    {
        Crit3DInterpolationDataPoint myPoint = myPointList.at(i);
        distance = myPoint.distance / 1000.;
        deltaZ = myPoint.deltaZ / 1000.;
        if (myPoint.distance > 0.)
        {
            weight = 1 - exp(-(distance*distance)/(Rd*Rd)) * exp(-(deltaZ*deltaZ)/(Rz*Rz));
            weight = fabs(1 / (weight * weight * weight));
            sumWeights += weight;
            sum += myPoint.value * weight;
        }
    }

    if (sumWeights > 0.0)
        return float(sum / sumWeights);
    else
        return NODATA;
}

bool checkPrecipitationZero(std::vector <Crit3DInterpolationDataPoint> &myPoints, int* nrPrecNotNull, bool* flatPrecipitation)
{
    *flatPrecipitation = true;
    *nrPrecNotNull = 0;
    float myValue = NODATA;

    for (int i = 0; i < (int)(myPoints.size()); i++)
        if (myPoints.at(i).isActive)
            if (myPoints.at(i).value != NODATA)
                if (myPoints.at(i).value >= PREC_THRESHOLD)
                {
                    if (*nrPrecNotNull > 0 && myPoints.at(i).value != myValue)
                        *flatPrecipitation = false;

                    myValue = myPoints.at(i).value;
                    nrPrecNotNull++;
                }

    return (nrPrecNotNull == 0);
}

bool getUseDetrendingVar(meteoVariable myVar)
{
    if (myVar == airTemperature ||
            myVar == airDewTemperature ||
            myVar == dailyAirTemperatureAvg ||
            myVar == dailyAirTemperatureMax ||
            myVar == dailyAirTemperatureMin ||
            myVar == dailyAirDewTemperatureAvg ||
            myVar == dailyAirDewTemperatureMax ||
            myVar == dailyAirDewTemperatureMin)

        return true;
    else
        return false;
}

void detrendPoints(std::vector <Crit3DInterpolationDataPoint> &myPoints, Crit3DInterpolationSettings* mySettings,
                   meteoVariable myVar, int pos)
{
    float detrendValue, proxyValue;
    long myIndex;
    Crit3DInterpolationDataPoint* myPoint;
    Crit3DProxyInterpolation* myProxy;

    if (! getUseDetrendingVar(myVar)) return;

    myProxy = mySettings->getProxy(pos);

    for (myIndex = 0; myIndex < long(myPoints.size()); myIndex++)
    {
        detrendValue = 0;
        myPoint = &(myPoints.at(myIndex));

        proxyValue = myPoint->getProxy(pos);

        if (ProxyVarNames.at(myProxy->getName()) == height)
        {
            if (proxyValue != NODATA)
            {
                float LR_above = myProxy->getRegressionSlope();
                if (myProxy->getInversionIsSignificative())
                {
                    float LR_H1 = myProxy->getLapseRateH1();
                    float LR_H0 = myProxy->getLapseRateH0();
                    float LR_below = myProxy->getInversionLapseRate();

                    if (proxyValue <= LR_H1)
                        detrendValue = maxValue(proxyValue - LR_H0, 0) * LR_below;
                    else
                        detrendValue = ((LR_H1 - LR_H0) * LR_below) + (proxyValue - LR_H1) * LR_above;
                }
                else
                    detrendValue = maxValue(proxyValue, 0) * LR_above;
            }
        }

        else
        {
            if (proxyValue != NODATA)
                if (myProxy->getRegressionR2() >= mySettings->getMinRegressionR2())
                    detrendValue = proxyValue * myProxy->getRegressionSlope();
        }

        myPoint->value -= detrendValue;
    }
}

float retrend(meteoVariable myVar, vector <float> myProxyValues, Crit3DInterpolationSettings* mySettings)
{

    if (! getUseDetrendingVar(myVar)) return 0.;

    float retrendValue = 0.;
    float myProxyValue;
    Crit3DProxyInterpolation* myProxy;

    for (int pos=0; pos<mySettings->getProxyNr(); pos++)
    {
        myProxy = mySettings->getProxy(pos);

        if (myProxy->getIsActive())
        {
            myProxyValue = mySettings->getProxyValue(pos, myProxyValues);

            if (myProxyValue != NODATA)
            {
                float proxySlope = myProxy->getRegressionSlope();

                if (myProxy->getProxyPragaName() == height)
                {
                    if (mySettings->getUseThermalInversion() && myProxy->getInversionIsSignificative())
                    {
                        float LR_H0 = myProxy->getLapseRateH0();
                        float LR_H1 = myProxy->getLapseRateH1();
                        float LR_Below = myProxy->getInversionLapseRate();
                        if (myProxyValue <= LR_H1)
                            retrendValue += (maxValue(myProxyValue - LR_H0, 0) * LR_Below);
                        else
                            retrendValue += ((LR_H1 - LR_H0) * LR_Below) + (myProxyValue - LR_H1) * proxySlope;
                    }
                    else
                        retrendValue += maxValue(myProxyValue, 0) * proxySlope;
                }
                else
                    retrendValue += myProxyValue * proxySlope;
            }
        }
    }

    return retrendValue;
}

bool regressionOrography(std::vector <Crit3DInterpolationDataPoint> &myPoints, Crit3DInterpolationSettings* mySettings,
                         meteoVariable myVar, int orogProxyPos)
{
    if (getUseDetrendingVar(myVar))
    {
        if (mySettings->getUseThermalInversion())
            return regressionOrographyT(myPoints, mySettings, myVar, orogProxyPos, true);
        else
            return regressionSimpleT(myPoints, mySettings, myVar, orogProxyPos);
    }
    else
    {
        return regressionGeneric(myPoints, mySettings, orogProxyPos, false);
    }

}

void detrending(std::vector <Crit3DInterpolationDataPoint> &myPoints, Crit3DInterpolationSettings* mySettings,
                meteoVariable myVar)
{
    if (! getUseDetrendingVar(myVar)) return;

    int nrProxy = mySettings->getProxyNr();
    Crit3DProxyInterpolation* myProxy;

    for (int pos=0; pos<nrProxy; pos++)
    {
        myProxy = mySettings->getProxy(pos);

        if (myProxy->getProxyPragaName() == height)
        {
            if (regressionOrography(myPoints, mySettings, myVar, pos))
            {
                myProxy->setIsActive(true);
                detrendPoints(myPoints, mySettings, myVar, pos);
            }
            else
                myProxy->setIsActive(false);
        }
        else
        {
            if (regressionGeneric(myPoints, mySettings, pos, false))
            {
                myProxy->setIsActive(true);
                detrendPoints(myPoints, mySettings, myVar, pos);
            }
            else
                myProxy->setIsActive(false);

        }
    }
}

bool preInterpolation(std::vector <Crit3DInterpolationDataPoint> &myPoints, Crit3DInterpolationSettings* mySettings,
                      meteoVariable myVar)
{
    if (myVar == precipitation || myVar == dailyPrecipitation)
    {
        int nrPrecNotNull;
        bool isFlatPrecipitation;
        if (checkPrecipitationZero(myPoints, &nrPrecNotNull, &isFlatPrecipitation))
        {
            mySettings->setPrecipitationAllZero(true);
            return true;
        }
        else
            mySettings->setPrecipitationAllZero(false);
    }

    detrending(myPoints, mySettings, myVar);

    return (true);
}

float interpolate(std::vector <Crit3DInterpolationDataPoint> &myPoints, Crit3DInterpolationSettings* mySettings,
                  meteoVariable myVar, float myX, float myY, float myZ, std::vector <float> myProxyValues)

{
    if ((myVar == precipitation || myVar == dailyPrecipitation) && mySettings->getPrecipitationAllZero()) return 0.;

    float myResult = NODATA;

    assignDistances(myPoints, myX, myY, myZ);

    if (mySettings->getInterpolationMethod() == idw)
        myResult = inverseDistanceWeighted(myPoints);
    else if (mySettings->getInterpolationMethod() == kriging)
        myResult = NODATA; //todo
    else if (mySettings->getInterpolationMethod() == shepard)
        myResult = computeShepard(myPoints, mySettings, myX, myY);

    if (myResult != NODATA)
        myResult += retrend(myVar, myProxyValues, mySettings);
    else
        return NODATA;

    if (myVar == precipitation || myVar == dailyPrecipitation)
    {
        if (myResult < PREC_THRESHOLD)
            return 0.;
    }
    else if (myVar == airRelHumidity || myVar == dailyAirRelHumidityAvg
             || myVar == dailyAirRelHumidityMax || myVar == dailyAirRelHumidityMin)
        myResult = maxValue(minValue(myResult, 100), 0);
    else if (myVar == dailyAirTemperatureRange || myVar == leafWetness || myVar == dailyLeafWetness
             || myVar == globalIrradiance || myVar == dailyGlobalRadiation || myVar == atmTransmissivity
             || myVar == windIntensity || myVar == dailyWindIntensityAvg || myVar == dailyWindIntensityMax
             || myVar == atmPressure)
        myResult = maxValue(myResult, 0);

    return myResult;

}

std::vector <float> getProxyValuesXY(gis::Crit3DUtmPoint myPoint, Crit3DInterpolationSettings* mySettings)
{
    std::vector <float> myValues;
    float myValue;
    gis::Crit3DRasterGrid* proxyGrid;

    myValues.resize(mySettings->getProxyNr());

    for (int i=0; i<myValues.size(); i++)
    {
        myValues.at(i) = NODATA;

        proxyGrid = mySettings->getProxy(i)->getGrid();
        if (proxyGrid != NULL && proxyGrid->isLoaded)
        {
            myValue = gis::getValueFromXY(*proxyGrid, myPoint.x, myPoint.y);
            if (myValue != proxyGrid->header->flag)
                myValues.at(i) = myValue;
        }
    }

    return myValues;
}


bool interpolateGridDtm(std::vector <Crit3DInterpolationDataPoint> &myPoints, Crit3DInterpolationSettings* mySettings,
                        gis::Crit3DRasterGrid* myGrid, const gis::Crit3DRasterGrid& myDTM, meteoVariable myVar)
{
    if (! myGrid->initializeGrid(myDTM))
        return (false);

    float myX, myY;

    for (long myRow = 0; myRow < myGrid->header->nrRows ; myRow++)
        for (long myCol = 0; myCol < myGrid->header->nrCols; myCol++)
        {
            gis::getUtmXYFromRowColSinglePrecision(*myGrid, myRow, myCol, &myX, &myY);
            float myZ = myDTM.value[myRow][myCol];
            if (myZ != myGrid->header->flag)
                myGrid->value[myRow][myCol] = interpolate(myPoints, mySettings, myVar, myX, myY, myZ, getProxyValuesXY(gis::Crit3DUtmPoint(myX, myY), mySettings));
        }

    if (! gis::updateMinMaxRasterGrid(myGrid))
        return (false);

    return (true);
}


// require data loaded in interpolationPointList (use checkAndPassDataToInterpolation function)
bool interpolationRaster(std::vector <Crit3DInterpolationDataPoint> &myPoints, Crit3DInterpolationSettings *mySettings,
                         meteoVariable myVar, const Crit3DTime& myTime, const  gis::Crit3DRasterGrid& myDTM,
                         gis::Crit3DRasterGrid *myRaster, std::string *myError)
{
    // check data presence
    if (myPoints.size() == 0)
    {
        *myError = "No data to interpolate";
        return false;
    }

    // check DTM
    if (! myDTM.isLoaded)
    {
        *myError = "Load DTM before";
        return false;
    }

    // Interpolation settings
    mySettings->setCurrentDate(myTime.date);
    mySettings->setCurrentHour(myTime.getHour());

    // Proxy vars regression and detrend
    if (! preInterpolation(myPoints, mySettings, myVar))
    {
        *myError = "Interpolation: error in function preInterpolation";
        return false;
    }

    // Interpolate
    if (! interpolateGridDtm(myPoints, mySettings, myRaster, myDTM, myVar))
    {
        *myError = "Interpolation: error in function interpolateGridDtm";
        return false;
    }

    Crit3DTime t = myTime;
    myRaster->timeString = t.toStdString();
    return true;
}



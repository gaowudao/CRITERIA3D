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
#include "spatialControl.h"
#include "interpolation.h"


using namespace std;


float getMinHeight(std::vector <Crit3DInterpolationDataPoint> &myPoints, bool useLapseRateCode)
{
    float myZmin = NODATA;

    for (long i = 0; i < long(myPoints.size()); i++)
        if (myPoints.at(i).point->z != NODATA && myPoints.at(i).isActive && checkLapseRateCode(myPoints.at(i).lapseRateCode, useLapseRateCode, true))
            if (myZmin == NODATA || myPoints.at(i).point->z < myZmin)
                myZmin = float(myPoints.at(i).point->z);
    return myZmin;
}

float getMaxHeight(std::vector <Crit3DInterpolationDataPoint> &myPoints, bool useLapseRateCode)
{
    float zMax;
    zMax = NODATA;

    for (long i = 1; i < long(myPoints.size()); i++)
        if (myPoints.at(i).value != NODATA && myPoints.at(i).isActive && checkLapseRateCode(myPoints.at(i).lapseRateCode, useLapseRateCode, true))
            if (zMax == NODATA || (myPoints.at(i)).point->z > zMax)
                zMax = float(myPoints.at(i).point->z);


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
    unsigned int i;
    std::vector <Crit3DInterpolationDataPoint> neighbourPoints;
    neighbourPoints.clear();

    // define a first neighborhood inside initial radius
    for (i=1; i < myPoints.size(); i++)
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



void computeDistances(vector <Crit3DInterpolationDataPoint> &myPoints,  Crit3DInterpolationSettings* mySettings,
                      float x, float y, float z, bool excludeSupplemental)
{
    int row, col;

    for (unsigned long i = 0; i < myPoints.size() ; i++)
    {
        if (excludeSupplemental && ! checkLapseRateCode(myPoints.at(i).lapseRateCode, mySettings->getUseLapseRateCode(), false))
            myPoints.at(i).distance = 0;
        else
        {            
            myPoints.at(i).distance = gis::computeDistance(x, y, float((myPoints.at(i)).point->utm.x) , float((myPoints.at(i)).point->utm.y));
            myPoints.at(i).deltaZ = float(fabs(myPoints.at(i).point->z - z));

            if (mySettings->getUseTAD())
            {
                float topoDistance = 0.;
                float kh = mySettings->getTopoDist_Kh();
                if (kh != 0)
                {
                    topoDistance = NODATA;
                    if (myPoints.at(i).topographicDistance != nullptr)
                    {
                        if (! gis::isOutOfGridXY(x, y, myPoints.at(i).topographicDistance->header))
                        {
                            gis::getRowColFromXY(*(myPoints.at(i).topographicDistance->header), x, y, &row, &col);
                            topoDistance = myPoints.at(i).topographicDistance->value[row][col];
                        }
                    }

                    if (topoDistance = NODATA)
                        topoDistance = topographicDistance(x, y, z, (float)myPoints.at(i).point->utm.x,
                                                           (float)myPoints.at(i).point->utm.y,
                                                           (float)myPoints.at(i).point->z, myPoints.at(i).distance,
                                                           *(mySettings->getCurrentDEM()));
                }

                myPoints.at(i).distance += (kh * topoDistance) + (mySettings->getTopoDist_Kz() * myPoints.at(i).deltaZ);
            }
        }
    }

    return;
}

bool neighbourhoodVariability(std::vector <Crit3DInterpolationDataPoint> &myInterpolationPoints,
                              Crit3DInterpolationSettings* mySettings,
                              float x, float y, float z, int nMax,
                              float* devSt, float* devStDeltaZ, float* minDistance)
{
    int i, max_points;
    float* dataNeighborhood;
    float myValue;
    vector <float> deltaZ;
    vector <Crit3DInterpolationDataPoint> validPoints;

    computeDistances(myInterpolationPoints, mySettings, x, y, z, true);
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


bool regressionSimple(std::vector <Crit3DInterpolationDataPoint> &myPoints, bool useLapseRateCode,
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
        if (myPoint.isActive && checkLapseRateCode(myPoint.lapseRateCode, useLapseRateCode, true))
        {
            myProxyValue = myPoint.getProxyValue(proxyPosition);
            if (myProxyValue != NODATA)
            {
                myValues.push_back(myPoint.value);
                myZ.push_back(myProxyValue);
            }
        }
    }

    if (myValues.size() >= MIN_REGRESSION_POINTS)
    {
        statistics::linearRegression((float*)(myZ.data()), (float*)(myValues.data()), (long)(myZ.size()), isZeroIntercept,
                                     myIntercept, myCoeff, myR2);
        return true;
    }
    else
        return false;
}

bool regressionGeneric(std::vector <Crit3DInterpolationDataPoint> &myPoints, Crit3DInterpolationSettings* mySettings,
                       int proxyPos, bool isZeroIntercept)
{
    float q, m, r2;

    if (! regressionSimple(myPoints, mySettings->getUseLapseRateCode(), proxyPos, isZeroIntercept, &m, &q, &r2))
        return false;

    Crit3DProxy* myProxy = mySettings->getProxy(proxyPos);
    myProxy->setRegressionSlope(m);
    myProxy->setRegressionR2(r2);
    return (r2 >= mySettings->getMinRegressionR2());
}


bool regressionSimpleT(std::vector <Crit3DInterpolationDataPoint> &myPoints, Crit3DInterpolationSettings* mySettings,
                       Crit3DTime myTime, meteoVariable myVar, int orogProxyPos)
{
    float q, m, r2;
    bool useLRCode = mySettings->getUseLapseRateCode();

    Crit3DProxy* myProxyOrog = mySettings->getProxy(orogProxyPos);
    myProxyOrog->initializeOrography();

    if (! regressionSimple(myPoints, useLRCode, orogProxyPos, false, &m, &q, &r2))
        return false;

    if (r2 < mySettings->getMinRegressionR2())
        return false;

    myProxyOrog->setRegressionSlope(m);
    myProxyOrog->setRegressionR2(r2);

    // only pre-inversion data
    if (m > 0)
    {
        myProxyOrog->setInversionLapseRate(m);

        float maxZ = minValue(getMaxHeight(myPoints, useLRCode), mySettings->getMaxHeightInversion());
        myProxyOrog->setLapseRateH1(maxZ);
        myProxyOrog->setRegressionSlope(mySettings->getCurrentClimateLapseRate(myVar, myTime));
        myProxyOrog->setInversionIsSignificative(true);
    }

    return true;
}


float findHeightIntervalAvgValue(bool useLapseRateCode, std::vector <Crit3DInterpolationDataPoint> &myPoints,
                                 float heightInf, float heightSup, float maxPointsZ)
{
    float myValue, mySum, nValues;

    mySum = 0.;
    nValues = 0;

    for (long i = 0; i < long(myPoints.size()); i++)
         if (myPoints.at(i).point->z != NODATA && myPoints.at(i).isActive && checkLapseRateCode(myPoints.at(i).lapseRateCode, useLapseRateCode, true))
            if (myPoints.at(i).point->z >= heightInf && myPoints.at(i).point->z <= heightSup)
            {                    
                myValue = (myPoints.at(i)).value;
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
                          Crit3DTime myTime, meteoVariable myVar, int orogProxyPos, bool climateExists)
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

    bool useLRCode = mySettings->getUseLapseRateCode();

    Crit3DProxy* myProxyOrog = mySettings->getProxy(orogProxyPos);

    mySignificativeR2 = maxValue(mySettings->getMinRegressionR2(), float(0.2));
    mySignificativeR2Inv = maxValue(mySettings->getMinRegressionR2(), float(0.1));

    /*! initialize */
    myProxyOrog->initializeOrography();

    if (climateExists)
        climateLapseRate = mySettings->getCurrentClimateLapseRate(myVar, myTime);
    else
        climateLapseRate = 0.;

    myProxyOrog->setRegressionSlope(climateLapseRate);

    maxPointsZ = getMaxHeight(myPoints, useLRCode);
    heightInf = getMinHeight(myPoints, useLRCode);

    if (maxPointsZ == heightInf)
        return false;

    /*! not enough data to define a curve (use climate) */
    if (myPoints.size() < MIN_REGRESSION_POINTS)
        return true;

    /*! find intervals averages */

    heightSup = heightInf;
    deltaZ = DELTAZ_INI;
    while (heightSup <= maxPointsZ)
    {
        myAvg = NODATA;
        while (myAvg == NODATA)
        {
            heightSup = heightSup + deltaZ;
            myAvg = findHeightIntervalAvgValue(useLRCode, myPoints, heightInf, heightSup, maxPointsZ);
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
        if (myPoints.at(i).point->z != NODATA && checkLapseRateCode(myPoints.at(i).lapseRateCode, useLRCode, true))
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
        if (! regressionSimple(myPoints, useLRCode, orogProxyPos, false, &m, &q, &r2))
            return false;

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
        if (! regressionSimple(myPoints, useLRCode, orogProxyPos, false, &m, &q, &r2))
            return false;

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
        if (! regressionSimple(myPoints, useLRCode, orogProxyPos, false, &m, &q, &r2))
            return false;

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

    if (m1 <= 0)
        r21 = 0;

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
    unsigned int i;
    float radius;

    neighbourPoints.clear();

    // define a first neighborhood inside initial radius
    for (i=1; i < myPoints.size(); i++)
        if (myPoints.at(i).distance <= settings->getShepardInitialRadius() && myPoints.at(i).distance > 0 && myPoints.at(i).index != settings->getIndexPointCV())
            neighbourPoints.push_back(myPoints.at(i));

    if (neighbourPoints.size() <= SHEPARD_MIN_NRPOINTS)
    {
        sortPointsByDistance(SHEPARD_MIN_NRPOINTS + 1, myPoints, validPoints);
        if (validPoints.size() > SHEPARD_MIN_NRPOINTS)
        {
            radius = validPoints.at(SHEPARD_MIN_NRPOINTS).distance;
            validPoints.pop_back();
        }
        else
            radius = validPoints.at(validPoints.size()-1).distance + 1;
    }
    else if (neighbourPoints.size() > SHEPARD_MAX_NRPOINTS)
    {
        sortPointsByDistance(SHEPARD_MAX_NRPOINTS + 1, neighbourPoints, validPoints);
        radius = validPoints.at(SHEPARD_MAX_NRPOINTS).distance;
        validPoints.pop_back();
    }
    else
    {
        validPoints = neighbourPoints;
        radius = settings->getShepardInitialRadius();
    }

    unsigned int j;
    float weightSum, radius_27_4, radius_3, tmp, cosine, result;
    vector <float> weight, t, S;

    weight.resize(validPoints.size());
    t.resize(validPoints.size());
    S.resize(validPoints.size());

    weightSum = 0;
    radius_3 = radius / 3;
    radius_27_4 = (27 / 4) / radius;
    for (i=0; i < validPoints.size(); i++)
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
        for (j=0; j < validPoints.size(); j++)
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
        if (myPointList.at(i).distance > 0.)
        {
            weight = myPointList.at(i).distance / 10000. ;
            weight = fabs(1 / (weight * weight * weight));
            sumWeights += weight;
            sum += myPointList.at(i).value * weight;
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

    for (unsigned int i = 0; i < myPoints.size(); i++)
        if (myPoints.at(i).isActive)
            if (int(myPoints.at(i).value) != int(NODATA))
                if (myPoints.at(i).value >= float(PREC_THRESHOLD))
                {
                    if (*nrPrecNotNull > 0 && myPoints.at(i).value != myValue)
                        *flatPrecipitation = false;

                    myValue = myPoints.at(i).value;
                    (*nrPrecNotNull)++;
                }

    return (*nrPrecNotNull == 0);
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
    Crit3DProxy* myProxy;

    if (! getUseDetrendingVar(myVar)) return;

    myProxy = mySettings->getProxy(pos);

    for (myIndex = 0; myIndex < long(myPoints.size()); myIndex++)
    {
        detrendValue = 0;
        myPoint = &(myPoints.at(myIndex));

        proxyValue = myPoint->getProxyValue(pos);

        if (getProxyPragaName(myProxy->getName()) == height)
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
    Crit3DProxy* myProxy;
    Crit3DProxyCombination* myCombination = mySettings->getCurrentCombination();

    for (int pos=0; pos < mySettings->getProxyNr(); pos++)
    {
        myProxy = mySettings->getProxy(pos);

        if (myCombination->getValue(pos) && myProxy->getIsSignificant())
        {
            myProxyValue = mySettings->getProxyValue(pos, myProxyValues);

            if (myProxyValue != NODATA)
            {
                float proxySlope = myProxy->getRegressionSlope();

                if (getProxyPragaName(myProxy->getName()) == height)
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

bool regressionOrography(std::vector <Crit3DInterpolationDataPoint> &myPoints,
                         Crit3DProxyCombination myCombination, Crit3DInterpolationSettings* mySettings,
                         Crit3DTime myTime, meteoVariable myVar, int orogProxyPos)
{
    if (getUseDetrendingVar(myVar))
    {
        if (myCombination.getUseThermalInversion())
            return regressionOrographyT(myPoints, mySettings, myTime, myVar, orogProxyPos, true);
        else
            return regressionSimpleT(myPoints, mySettings, myTime, myVar, orogProxyPos);
    }
    else
    {
        return regressionGeneric(myPoints, mySettings, orogProxyPos, false);
    }

}

void detrending(std::vector <Crit3DInterpolationDataPoint> &myPoints,
                Crit3DProxyCombination myCombination, Crit3DInterpolationSettings* mySettings,
                meteoVariable myVar, Crit3DTime myTime)
{
    if (! getUseDetrendingVar(myVar)) return;

    Crit3DProxy* myProxy;

    for (int pos=0; pos < mySettings->getProxyNr(); pos++)
    {
        if (myCombination.getValue(pos))
        {
            myProxy = mySettings->getProxy(pos);
            myProxy->setIsSignificant(false);

            if (getProxyPragaName(myProxy->getName()) == height)
            {
                if (regressionOrography(myPoints, myCombination, mySettings, myTime, myVar, pos))
                {
                    myProxy->setIsSignificant(true);
                    detrendPoints(myPoints, mySettings, myVar, pos);
                }
            }
            else
            {
                if (regressionGeneric(myPoints, mySettings, pos, false))
                {
                    myProxy->setIsSignificant(true);
                    detrendPoints(myPoints, mySettings, myVar, pos);
                }
            }
        }
    }
}

void topographicDistanceOptimize(meteoVariable myVar,
                                 Crit3DMeteoPoint* &myMeteoPoints,
                                 int nrMeteoPoints,
                                 std::vector <Crit3DInterpolationDataPoint> &interpolationPoints,
                                 Crit3DInterpolationSettings* mySettings,
                                 const Crit3DTime &myTime)
{
    float avgError, bestError, bestK;
    float kh, kz;

    kh = kz = 0;

    bestError = NODATA;

    while (kz <= 256)
    {
        mySettings->setTopoDist_Kz(kz);
        if (computeResiduals(myVar, myMeteoPoints, nrMeteoPoints, interpolationPoints, mySettings, true, true))
        {
            avgError = computeErrorCrossValidation(myVar, myMeteoPoints, nrMeteoPoints, myTime);
            if (bestError == NODATA || avgError < bestError)
            {
                bestError = avgError;
                bestK = kz;
            }
            kz = (kz == 0 ? 1 : kz*2);
        }
    }

    mySettings->setTopoDist_Kz(bestK);

    kh = 0;
    bestError = NODATA;
    while (kh <= 1000000)
    {
        mySettings->setTopoDist_Kh(kh);
        if (computeResiduals(myVar, myMeteoPoints, nrMeteoPoints, interpolationPoints, mySettings, true, true))
        {
            avgError = computeErrorCrossValidation(myVar, myMeteoPoints, nrMeteoPoints, myTime);
            if (bestError == NODATA || avgError < bestError)
            {
                bestError = avgError;
                bestK = kh;
            }
            kh = (kh == 0 ? 1 : kh*2);
        }
    }

    mySettings->setTopoDist_Kh(bestK);

}

void optimalDetrending(meteoVariable myVar,
                    Crit3DMeteoPoint* &myMeteoPoints,
                    int nrMeteoPoints,
                    std::vector <Crit3DInterpolationDataPoint> &outInterpolationPoints,
                    Crit3DInterpolationSettings* mySettings,
                    const Crit3DTime &myTime)
{

    std::vector <Crit3DInterpolationDataPoint> interpolationPoints;
    short i, nrCombination, bestCombinationIndex;
    float avgError, minError;
    int proxyNr = mySettings->getProxyNr();
    Crit3DProxyCombination myCombination, bestCombination;
    myCombination = mySettings->getSelectedCombination();

    nrCombination = (short)pow(2, (proxyNr + 1));

    minError = NODATA;

    for (i=0; i < nrCombination; i++)
    {
        if (mySettings->getCombination(i, &myCombination))
        {
            passDataToInterpolation(myMeteoPoints, nrMeteoPoints, interpolationPoints, mySettings);
            detrending(interpolationPoints, myCombination, mySettings, myVar, myTime);
            mySettings->setCurrentCombination(&myCombination);

            if (mySettings->getUseTAD())
                topographicDistanceOptimize(myVar, myMeteoPoints, nrMeteoPoints, interpolationPoints, mySettings, myTime);

            if (computeResiduals(myVar, myMeteoPoints, nrMeteoPoints, interpolationPoints, mySettings, true, true))
            {
                avgError = computeErrorCrossValidation(myVar, myMeteoPoints, nrMeteoPoints, myTime);
                if (avgError != NODATA && (minError == NODATA || avgError < minError))
                {
                    minError = avgError;
                    bestCombinationIndex = i;
                }
            }

        }
    }

    if (mySettings->getCombination(bestCombinationIndex, &bestCombination))
    {
        passDataToInterpolation(myMeteoPoints, nrMeteoPoints, outInterpolationPoints, mySettings);
        detrending(outInterpolationPoints, bestCombination, mySettings, myVar, myTime);
        mySettings->setOptimalCombination(bestCombination);
    }

    return;
}


bool preInterpolation(std::vector <Crit3DInterpolationDataPoint> &myPoints, Crit3DInterpolationSettings* mySettings,
                      Crit3DMeteoPoint* myMeteoPoints, int nrMeteoPoints,
                      meteoVariable myVar, Crit3DTime myTime)
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

    if (mySettings->getUseBestDetrending())
    {
        optimalDetrending(myVar, myMeteoPoints, nrMeteoPoints, myPoints, mySettings, myTime);
        mySettings->setCurrentCombination(mySettings->getOptimalCombinationRef());
    }
    else
    {
        detrending(myPoints, mySettings->getSelectedCombination(), mySettings, myVar, myTime);
        mySettings->setCurrentCombination(mySettings->getSelectedCombinationRef());
        if (mySettings->getUseTAD())
            topographicDistanceOptimize(myVar, myMeteoPoints, nrMeteoPoints, myPoints, mySettings, myTime);
    }

    return (true);
}


float interpolate(vector <Crit3DInterpolationDataPoint> &myPoints, Crit3DInterpolationSettings* mySettings,
                  meteoVariable myVar, float myX, float myY, float myZ, std::vector <float> myProxyValues,
                  bool excludeSupplemental)

{
    if ((myVar == precipitation || myVar == dailyPrecipitation) && mySettings->getPrecipitationAllZero()) return 0.;

    float myResult = NODATA;

    computeDistances(myPoints, mySettings, myX, myY, myZ, excludeSupplemental);

    if (mySettings->getInterpolationMethod() == idw)
    {
        myResult = inverseDistanceWeighted(myPoints);
    }
    else if (mySettings->getInterpolationMethod() == kriging)
    {
        //TODO kriging
        myResult = NODATA;
    }
    else if (mySettings->getInterpolationMethod() == shepard)
    {
        myResult = computeShepard(myPoints, mySettings, myX, myY);
    }

    if (int(myResult) != int(NODATA))
    {
        myResult += retrend(myVar, myProxyValues, mySettings);
    }
    else
        return NODATA;

    if (myVar == precipitation || myVar == dailyPrecipitation)
    {
        if (myResult < float(PREC_THRESHOLD))
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

std::vector <float> getProxyValuesXY(float x, float y, Crit3DInterpolationSettings* mySettings)
{
    std::vector <float> myValues;
    float myValue;
    gis::Crit3DRasterGrid* proxyGrid;

    myValues.resize(mySettings->getProxyNr());
    Crit3DProxyCombination* myCombination = mySettings->getCurrentCombination();

    for (unsigned int i=0; i < myValues.size(); i++)
    {
        myValues.at(i) = NODATA;

        if (myCombination->getValue(i))
        {
            proxyGrid = mySettings->getProxy(i)->getGrid();
            if (proxyGrid != nullptr && proxyGrid->isLoaded)
            {
                myValue = gis::getValueFromXY(*proxyGrid, x, y);
                if (myValue != proxyGrid->header->flag)
                    myValues.at(i) = myValue;
            }
        }
    }

    return myValues;
}


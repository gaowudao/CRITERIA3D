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
#include <QDebug>

#include <math.h>
#include <vector>

#include "commonConstants.h"
#include "basicMath.h"
#include "statistics.h"
#include "meteoPoint.h"
#include "gis.h"
#include "interpolation.h"


using namespace std;
using namespace statistics;

vector <Crit3DInterpolationDataPoint> interpolationPointList;
vector <Crit3DInterpolationDataPoint> precBinaryPointList;

float lapseRateH1;
float lapseRateH0;
float inversionLapseRate;
bool inversionIsSignificative;

gis::Crit3DRasterGrid orogIndexMap;
gis::Crit3DRasterGrid distSeaMap;
gis::Crit3DRasterGrid genericProxyMap;
gis::Crit3DRasterGrid urbanFractionMap;

float actualLapseRate;
float actualR2;
float actualR2Levels;
float urbanCoefficient;
float urbanIntercept;
float urbanR2;
float orogIndexCoefficient;
float orogIndexIntercept;
float orogIndexR2;
float seaDistCoefficient;
float seaDistIntercept;
float seaDistR2;
float aspectCoefficient;
float aspectIntercept;
float aspectR2;
float genericCoefficient;
float genericIntercept;
float genericR2;

bool precipitationAllZero = false;
int indexPointJacknife = NODATA;

Crit3DInterpolationSettings currentSettings;

void setInterpolationSettings(Crit3DInterpolationSettings* mySettings)
{
    currentSettings = *(mySettings);
}

void setindexPointJacknife(int index)
{
    indexPointJacknife = index;
}

void clearInterpolationPoints()
{
    interpolationPointList.clear();
}

bool addInterpolationPoint(int myIndex, float myValue, float myX, float myY, float myHeight, float myOrogIndex, float myUrban, float mySeaDist, float myAspect, float myGenericProxy)
{
    Crit3DInterpolationDataPoint myPoint;

    myPoint.index = myIndex;
    myPoint.value = myValue;
    myPoint.point->utm.x = myX;
    myPoint.point->utm.y = myY;
    myPoint.isActive = true;
    myPoint.point->z = myHeight;
    myPoint.setOrogIndex(myOrogIndex);
    myPoint.setUrbanFraction(myUrban);
    myPoint.setSeaDistance(mySeaDist);
    myPoint.setAspect(myAspect);
    myPoint.setGenericProxy(myGenericProxy);

    interpolationPointList.push_back(myPoint);

    return (true);
}


void printInterpolationData()
{
    for (unsigned long i = 0; i < interpolationPointList.size() ; i++)
    {
        qDebug() << i << interpolationPointList[i].value;
    }
}


void assignDistances(vector <Crit3DInterpolationDataPoint> *myPoints, float x, float y, float z)
{
    for (unsigned long i = 0; i < myPoints->size() ; i++)
    {
        myPoints->at(i).distance = gis::computeDistance(x, y, float((myPoints->at(i)).point->utm.x) , float((myPoints->at(i)).point->utm.y));
        myPoints->at(i).deltaZ = float(fabs(myPoints->at(i).point->z - z));
    }
}

bool initializeOrography()
{
    lapseRateH0 = 0.;
    lapseRateH1 = NODATA;

    inversionLapseRate = NODATA;
    actualLapseRate = NODATA;

    actualR2 = NODATA;
    actualR2Levels = NODATA;

    inversionIsSignificative = false;

    return true;
}


float getMinHeight()
{
    float myZmin = NODATA;

    for (long i = 0; i < long(interpolationPointList.size()); i++)
        if (interpolationPointList.at(i).point->z != NODATA)
            if (myZmin == NODATA || interpolationPointList.at(i).point->z < myZmin)
                myZmin = float(interpolationPointList.at(i).point->z);
    return myZmin;
}

float getMaxHeight()
{
    float zMax;
    zMax = NODATA;

    for (long i = 1; i < long(interpolationPointList.size()); i++)
        if ((interpolationPointList.at(i)).value != NODATA && (interpolationPointList.at(i)).isActive)
            if (zMax == NODATA || (interpolationPointList.at(i)).point->z > zMax)
                zMax = float(interpolationPointList.at(i).point->z);


    return zMax;
}

int sortPointsByDistance(int maxIndex, vector <Crit3DInterpolationDataPoint> myPoints, vector <Crit3DInterpolationDataPoint>* myValidPoints)
{   
    int i, first, index;
    float min_value;
    int* indici_ordinati;
    int* indice_minimo;
    int outIndex;

    if (myPoints.size() == 0) return 0;

    indici_ordinati = (int *) calloc(maxIndex + 1, sizeof(int));
    indice_minimo = (int *) calloc(myPoints.size(), sizeof(int));

    first = 0;
    index = 0;

    bool exit = false;

    while (index <= maxIndex && !exit)
    {
        if (first == 0)
        {
            i = 0;
            while ((! myPoints.at(i).isActive || (myPoints.at(i).distance == 0 && currentSettings.isCrossValidation)) && (i < int(myPoints.size())-1))
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
                        if (myPoints.at(i).distance > 0 || ! currentSettings.isCrossValidation)
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

    outIndex = minValue(index - 1, maxIndex);
    (*myValidPoints).resize(outIndex+1);

    for (i=0; i<outIndex+1; i++)
    {
        myPoints.at(indici_ordinati[i]).isActive = true;
        (*myValidPoints).at(i) = myPoints.at(indici_ordinati[i]);
    }

    return outIndex+1;
}


bool neighbourhoodVariability(float x, float y, float z, int nMax,
                              float* devSt, float* devStDeltaZ, float* minDistance)
{
    int i, max_points;
    float* dataNeighborhood;
    float myValue;
    vector <float> deltaZ;
    vector <Crit3DInterpolationDataPoint> validPoints;

    assignDistances(&interpolationPointList, x, y, z);
    max_points = sortPointsByDistance(nMax, interpolationPointList, &validPoints);

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

void regressionSimple(proxyVars::TProxyVar myProxy, bool isZeroIntercept, float* myCoeff, float* myIntercept, float* myR2)
{
    long i;
    float myProxyValue;
    Crit3DInterpolationDataPoint* myPoint;
    vector <float> myValues, myZ;

    *myCoeff = NODATA;
    *myIntercept = NODATA;
    *myR2 = NODATA;

    myValues.clear();
    myZ.clear();

    for (i = 0; i < long(interpolationPointList.size()); i++)
    {
        myPoint = &(interpolationPointList.at(i));
        myProxyValue = NODATA;
        if (myPoint->isActive)
        {
            if (myProxy == proxyVars::height)
                myProxyValue = float(myPoint->point->z);
            else if (myProxy == proxyVars::urbanFraction)
                myProxyValue = myPoint->getUrbanFraction();
            else if (myProxy == proxyVars::orogIndex)
                myProxyValue = myPoint->getOrogIndex();
            else if (myProxy == proxyVars::seaDistance)
                myProxyValue = myPoint->getSeaDistance();
            else if (myProxy == proxyVars::aspect)
                myProxyValue = myPoint->getAspect();
            else if (myProxy == proxyVars::generic)
                myProxyValue = myPoint->getGenericProxy();

            if (myProxyValue != NODATA)
            {
                myValues.push_back(myPoint->value);
                myZ.push_back(myProxyValue);
            }
        }
    }

    if (myValues.size() >= MIN_REGRESSION_POINTS)
        statistics::linearRegression((float*)(myZ.data()), (float*)(myValues.data()), (long)(myZ.size()), isZeroIntercept,
                                     myIntercept, myCoeff, myR2);
}

bool regressionGeneric(proxyVars::TProxyVar myProxy, bool isZeroIntercept)
{
    float q, m, r2;

    regressionSimple(myProxy, isZeroIntercept, &m, &q, &r2);

    if (myProxy == proxyVars::height)
    {
        inversionIsSignificative = false;
        lapseRateH1 = NODATA;
        inversionLapseRate = NODATA;
        actualLapseRate = m;
        actualR2 = r2;
        actualR2Levels = NODATA;
    }
    else if (myProxy == proxyVars::urbanFraction)
    {
        urbanCoefficient = m;
        urbanIntercept = q;
        urbanR2 = r2;
    }
    else if (myProxy == proxyVars::orogIndex)
    {
        orogIndexCoefficient = m;
        orogIndexIntercept = q;
        orogIndexR2 = r2;
    }
    else if (myProxy == proxyVars::seaDistance)
    {
        seaDistCoefficient = m;
        seaDistIntercept = q;
        seaDistR2 = r2;
    }
    else if (myProxy == proxyVars::aspect)
    {
        aspectCoefficient = m;
        aspectIntercept = q;
        aspectR2 = r2;
    }
    else if (myProxy == proxyVars::generic)
    {
        genericCoefficient = m;
        genericIntercept = q;
        genericR2 = r2;
    }

    if (r2 >= currentSettings.getGenericPearsonThreshold())
        return true;
    else
        return false;

}


bool regressionSimpleT(meteoVariable myVar)
{
    float q, m, r2;

    initializeOrography();

    regressionSimple(proxyVars::height, false, &m, &q, &r2);

    if (r2 < currentSettings.getGenericPearsonThreshold())
        return false;

    actualLapseRate = m;
    actualR2 = r2;

    // only pre-inversion data
    if (m > 0)
    {
        inversionLapseRate = m;

        float maxZ = minValue(getMaxHeight(), currentSettings.getMaxHeightInversion());
        lapseRateH1 = maxZ;

        actualLapseRate = currentSettings.getCurrentClimateLapseRate(myVar);

        inversionIsSignificative = true;
    }

    return true;
}


float findHeightIntervalAvgValue(float heightInf, float heightSup, float maxPointsZ)
{
    long myIndex;
    float myValue, mySum, nValues;

    mySum = 0.;
    nValues = 0;

    for (myIndex = 0; myIndex < long(interpolationPointList.size()); myIndex++)
         if ((interpolationPointList.at(myIndex)).point->z != NODATA && (interpolationPointList.at(myIndex)).isActive)
            if ((interpolationPointList.at(myIndex)).point->z >= heightInf && (interpolationPointList.at(myIndex)).point->z <= heightSup)
            {
                myValue = (interpolationPointList.at(myIndex)).value;
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

bool regressionOrographyT(meteoVariable myVar, bool climateExists)
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
    float climateLapseRate, lapseRateT0, lapseRateT1;

    float DELTAZ_INI = 80.;

    mySignificativeR2 = maxValue(currentSettings.getGenericPearsonThreshold(), float(0.2));
    mySignificativeR2Inv = maxValue(currentSettings.getGenericPearsonThreshold(), float(0.1));

    /*! initialize */
    initializeOrography();

    if (climateExists)
        climateLapseRate = currentSettings.getCurrentClimateLapseRate(myVar);
    else
        climateLapseRate = 0.;

    actualLapseRate = climateLapseRate;

    maxPointsZ = getMaxHeight();

    /*! not enough data to define a curve (use climate) */
    if (interpolationPointList.size() < MIN_REGRESSION_POINTS)
    {
        inversionIsSignificative = false;
        lapseRateH1 = NODATA;
        inversionLapseRate = NODATA;
        actualLapseRate = climateLapseRate;
        actualR2Levels = NODATA;
        return true;
    }

    /*! find intervals averages */
    heightInf = getMinHeight();
    heightSup = heightInf;
    deltaZ = DELTAZ_INI;
    while (heightSup <= maxPointsZ)
    {
        myAvg = NODATA;
        while (myAvg == NODATA)
        {
            heightSup = heightSup + deltaZ;
            if (heightSup > maxPointsZ)
                heightSup = maxPointsZ + (maxPointsZ - heightInf);

            myAvg = findHeightIntervalAvgValue(heightInf, heightSup, maxPointsZ);
        }
        myIntervalsHeight.push_back((heightSup + heightInf) / float(2.));
        myIntervalsValues.push_back(myAvg);

        deltaZ = DELTAZ_INI * exp(heightInf / currentSettings.getMaxHeightInversion());
        heightInf = heightSup;
    }

    /*! find inversion height */
    inversionIsSignificative = false;
    lapseRateT1 = myIntervalsValues.at(0);
    lapseRateH1 = myIntervalsHeight.at(0);
    for (i = 1; i < long(myIntervalsValues.size()); i++)
        if (myIntervalsHeight.at(i) <= currentSettings.getMaxHeightInversion() && (myIntervalsValues.at(i) >= lapseRateT1) && (myIntervalsValues.at(i) > (myIntervalsValues.at(0) + 0.001 * (myIntervalsHeight.at(i) - myIntervalsHeight.at(0)))))
        {
            lapseRateH1 = myIntervalsHeight.at(i);
            lapseRateT1 = myIntervalsValues.at(i);
            inversionIsSignificative = true;
        }

    /*! no inversion: try regression with all data */
    if (! inversionIsSignificative)
        return (regressionGeneric(proxyVars::height, false));

    /*! create vectors below and above inversion */
    for (i = 1; i < long(interpolationPointList.size()); i++)
        if ((interpolationPointList.at(i)).point->z != NODATA && (interpolationPointList.at(i)).isActive)
        {
            if ((interpolationPointList.at(i)).point->z <= lapseRateH1)
            {
                myData1.push_back((interpolationPointList.at(i)).value);
                myHeights1.push_back(float((interpolationPointList.at(i)).point->z));
            }
            else
            {
                myData2.push_back((interpolationPointList.at(i)).value);
                myHeights2.push_back(float((interpolationPointList.at(i)).point->z));
            }
        }


    /*! create vectors of height intervals below and above inversion */
    for (i = 0; i < long(myIntervalsValues.size()); i++)
        if (myIntervalsHeight.at(i) <= lapseRateH1)
        {
            myIntervalsValues1.push_back(myIntervalsValues.at(i));
            myIntervalsHeight1.push_back(myIntervalsHeight.at(i));
        }
        else
        {
            myIntervalsValues2.push_back(myIntervalsValues.at(i));
            myIntervalsHeight2.push_back(myIntervalsHeight.at(i));
        }


    /*! only positive lapse ratec*/
    if (inversionIsSignificative && myIntervalsValues1.size() == myIntervalsValues.size())
    {
        regressionSimple(proxyVars::height, false, &m, &q, &r2);

        if (r2 >= mySignificativeR2)
        {
            inversionLapseRate = m;
            actualR2 = r2;
            actualR2Levels = NODATA;
            lapseRateT0 = q;
            lapseRateT1 = q + m * lapseRateH1;
        }
        else
        {
            statistics::linearRegression(myIntervalsHeight1.data(), myIntervalsValues1.data(), (long)myIntervalsHeight1.size(), false, &q, &m, &r2);

            actualR2 = NODATA;
            actualR2Levels = r2;

            if (r2 >= mySignificativeR2)
            {
                lapseRateT0 = q;
                inversionLapseRate = m;
                lapseRateT1 = q + m * lapseRateH1;
            }
            else
            {
                inversionLapseRate = 0.;
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
        regressionSimple(proxyVars::height, false, &m, &q, &actualR2);

        /*! case 0: regression with all data much significant */
        if (actualR2 >= 0.5)
        {
            inversionIsSignificative = false;
            lapseRateH1 = NODATA;
            inversionLapseRate = NODATA;
            actualLapseRate = minValue(m, float(0.0));
            actualR2Levels = NODATA;
            return true;
        }

        /*! case 1: analysis only above inversion, flat lapse rate below */
        inversionLapseRate = 0.;
        statistics::linearRegression(myHeights2.data(), myData2.data(), long(myHeights2.size()), false, &q2, &m2, &actualR2);

        if (myData2.size() >= MIN_REGRESSION_POINTS)
        {
            if (actualR2 >= mySignificativeR2)
            {
                actualLapseRate = minValue(m2, float(0.0));
                actualR2Levels = NODATA;
                lapseRateT1 = q2 + lapseRateH1 * actualLapseRate;
                lapseRateT0 = lapseRateT1;
                return true;
            }
            else
            {
                statistics::linearRegression(myIntervalsHeight2.data(), myIntervalsValues2.data(),
                                             int(myIntervalsHeight2.size()), false, &q2, &m2, &actualR2Levels);
                if (actualR2Levels >= mySignificativeR2)
                {
                    actualLapseRate = minValue(m2, float(0.0));
                    lapseRateT1 = q2 + lapseRateH1 * actualLapseRate;
                    lapseRateT0 = lapseRateT1;
                    return true;
                }
            }
        }

        inversionIsSignificative = false;
        lapseRateH1 = NODATA;
        inversionLapseRate = NODATA;
        lapseRateT0 = q;
        lapseRateT1 = NODATA;

        /*! case 2: regression with data */
        regressionSimple(proxyVars::height, false, &m, &q, &actualR2);
        actualR2Levels = NODATA;

        if (actualR2 >= mySignificativeR2)
        {
            actualLapseRate = minValue(m, 0);
            lapseRateT0 = q;
            return true;
        }
        else
        {
            lapseRateT0 = myIntervalsValues.at(0);
            if (m > 0.)
                actualLapseRate = 0.;
            else
                actualLapseRate = climateLapseRate;

            return true;
        }

    }

    /*! significance analysis */
    statistics::linearRegression(myHeights1.data(), myData1.data(), int(myHeights1.size()), false, &q1, &m1, &r21);
    statistics::linearRegression(myHeights2.data(), myData2.data(), int(myHeights2.size()), false, &q2, &m2, &r22);

    actualR2 = r22;
    actualR2Levels = NODATA;

    if (r21 >= mySignificativeR2Inv && r22 >= mySignificativeR2)
    {
        if (myHeights2.size() < MIN_REGRESSION_POINTS && m2 > 0.)
        {
            m2 = 0.;
            q2 = lapseRateT1;
        }
        findLinesIntersection(q1, m1, q2, m2, &lapseRateH1, &lapseRateT1);
        lapseRateT0 = q1;
        inversionLapseRate = m1;
        actualLapseRate = m2;
        if (lapseRateH1 > currentSettings.getMaxHeightInversion())
        {
            lapseRateT1 = lapseRateT1 - (lapseRateH1 - currentSettings.getMaxHeightInversion()) * actualLapseRate;
            lapseRateH1 = currentSettings.getMaxHeightInversion();
            inversionLapseRate = (lapseRateT1 - lapseRateT0) / (lapseRateH1 - lapseRateH0);
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
        actualLapseRate = m2;
        if (r2 >= mySignificativeR2Inv)
        {
            if (findLinesIntersectionAboveThreshold(q, m, q2, m2, 40, &lapseRateH1, &lapseRateT1))
            {
                lapseRateT0 = q;
                inversionLapseRate = m;
                if (lapseRateH1 > currentSettings.getMaxHeightInversion())
                {
                    lapseRateT1 = lapseRateT1 - (lapseRateH1 - currentSettings.getMaxHeightInversion()) * actualLapseRate;
                    lapseRateH1 = currentSettings.getMaxHeightInversion();
                    inversionLapseRate = (lapseRateT1 - lapseRateT0) / (lapseRateH1 - lapseRateH0);
                }
                return true;
            }
        }
        else
        {
            inversionLapseRate = 0.;
            lapseRateT1 = q2 + m2 * lapseRateH1;
            lapseRateT0 = lapseRateT1;
            return true;
        }
    }

    else if (r21 >= mySignificativeR2Inv && r22 < mySignificativeR2)
    {
        lapseRateT0 = q1;
        inversionLapseRate = m1;

        statistics::linearRegression(myIntervalsHeight2.data(), myIntervalsValues2.data(),
                                     long(myIntervalsHeight2.size()), false, &q, &m, &r2);
        actualR2Levels = r2;

        if (r2 >= mySignificativeR2)
        {
            actualLapseRate = minValue(m, float(0.));
            findLinesIntersection(lapseRateT0, inversionLapseRate, q, actualLapseRate, &lapseRateH1, &lapseRateT1);
        }
        else
        {
            actualLapseRate = climateLapseRate;
            findLinesIntersection(lapseRateT0, inversionLapseRate, lapseRateT1 - actualLapseRate * lapseRateH1, actualLapseRate, &lapseRateH1, &lapseRateT1);
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
            inversionLapseRate = m;
            lapseRateT1 = q + m * lapseRateH1;
        }
        else
        {
            inversionLapseRate = 0.;
            lapseRateT0 = myIntervalsValues.at(0);
            lapseRateT1 = lapseRateT0;
        }

        statistics::linearRegression(myIntervalsHeight2.data(), myIntervalsValues2.data(),
                                     long(myIntervalsHeight2.size()), false, &q, &m, &r2);
        actualR2Levels = r2;

        if (r2 >= mySignificativeR2)
        {
            actualLapseRate = minValue(m, 0);
            if (findLinesIntersectionAboveThreshold(lapseRateT0, inversionLapseRate, q, actualLapseRate, 40, &lapseRateH1, &lapseRateT1))
                return true;
        }
        else
        {
            actualLapseRate = climateLapseRate;
            return true;
        }

    }

    /*! check max lapse rate (20 C / 1000 m) */
    if (actualLapseRate < -0.02)
        actualLapseRate = (float)-0.02;

    initializeOrography();
    return (regressionGeneric(proxyVars::height, false));

}


bool regressionOrography(meteoVariable myVar)
{
    initializeOrography();

    if (myVar == airDewTemperature || myVar == airTemperature
            || myVar == dailyAirTemperatureMax
            || myVar == dailyAirTemperatureMin
            || myVar == dailyAirTemperatureAvg )
    {
        if (currentSettings.getUseThermalInversion())
            return regressionOrographyT(myVar, true);
        else
            return regressionSimpleT(myVar);
    }
    else
    {
        return regressionGeneric(proxyVars::height, false);
    }

}


float inverseDistanceWeighted(vector <Crit3DInterpolationDataPoint> myPointList)
{
    double sum, sumWeights, weight;

    sum = 0 ;
    sumWeights = 0 ;
    for (int i = 0 ; i < (int)(myPointList.size()); i++)
    {
        Crit3DInterpolationDataPoint* myPoint = &(myPointList.at(i));
        if (myPoint->distance > 0.)
        {
            weight = myPoint->distance / 10000. ;
            weight = fabs(1 / (weight * weight * weight));
            sumWeights += weight;
            sum += myPoint->value * weight;
        }
        else if (myPoint->index != indexPointJacknife)
            return myPoint->value;
    }

    if (sumWeights > 0.0)
        return float(sum / sumWeights);
    else
        return NODATA;
}

float gaussWeighted(vector <Crit3DInterpolationDataPoint> myPointList)
{
    double sum, sumWeights, weight;
    double distance, deltaZ;
    double Rd=10;
    double Rz=1;

    sum = 0 ;
    sumWeights = 0 ;
    for (int i = 0 ; i < (int)(myPointList.size()); i++)
    {
        Crit3DInterpolationDataPoint* myPoint = &(myPointList.at(i));
        distance = myPoint->distance / 1000.;
        deltaZ = myPoint->deltaZ / 1000.;
        if (myPoint->distance > 0.)
        {
            weight = 1 - exp(-(distance*distance)/(Rd*Rd)) * exp(-(deltaZ*deltaZ)/(Rz*Rz));
            weight = fabs(1 / (weight * weight * weight));
            sumWeights += weight;
            sum += myPoint->value * weight;
        }
        else if (myPoint->index != indexPointJacknife)
            return myPoint->value;
    }

    if (sumWeights > 0.0)
        return float(sum / sumWeights);
    else
        return NODATA;
}

bool checkPrecipitationZero(int* nrPrecNotNull, bool* flatPrecipitation)
{
    *flatPrecipitation = true;
    *nrPrecNotNull = 0;
    float myValue = NODATA;

    for (int i = 0; i < (int)(interpolationPointList.size()); i++)
        if (interpolationPointList.at(i).isActive)
            if (interpolationPointList.at(i).value != NODATA)
                if (interpolationPointList.at(i).value >= PREC_THRESHOLD)
                {
                    if (*nrPrecNotNull > 0 && interpolationPointList.at(i).value != myValue)
                        *flatPrecipitation = false;

                    myValue = interpolationPointList.at(i).value;
                    nrPrecNotNull++;
                }

    return (nrPrecNotNull == 0);
}

void prepareJRC()
{
    vector <Crit3DInterpolationDataPoint> precPoints;

    precBinaryPointList.clear();
    precBinaryPointList.resize(interpolationPointList.size());

    int nrPrec = 0;
    for (int i = 0; i < (int)(interpolationPointList.size()); i++)
        if (interpolationPointList.at(i).isActive)
        {
            precBinaryPointList.at(i) = interpolationPointList.at(i);
            precBinaryPointList.at(i).value = 0;

            if (interpolationPointList.at(i).value >= PREC_THRESHOLD)
            {
                precBinaryPointList.at(i).value = 1;
                nrPrec++;
                precPoints.push_back(interpolationPointList.at(i));
            }
        }

    interpolationPointList.clear();
    interpolationPointList = precPoints;
}

float interpolatePrecStep1()
{
    return inverseDistanceWeighted(precBinaryPointList);
}

float interpolatePrecStep2()
{
    if (currentSettings.getInterpolationMethod() == geostatisticsMethods::idw)
        return inverseDistanceWeighted(interpolationPointList);
        //return gaussWeighted(interpolationPointList);
    else if (currentSettings.getInterpolationMethod() == geostatisticsMethods::kriging)
        return NODATA;
    else
        return NODATA;
}

float interpolatePrec()
{
    float myResult;

    if (! currentSettings.getUseJRC())
        myResult = interpolatePrecStep2();
    else
        if (interpolatePrecStep1() >= PREC_BINARY_THRESHOLD)
            myResult = interpolatePrecStep2();
        else
            myResult = 0.;

    return ((myResult < 0 && myResult != NODATA) ? 0 : myResult);
}

bool getDetrendActive(int myPosition)
{
    if (myPosition >= 0 && myPosition <= PROXY_VAR_NR)
    {
        if (currentSettings.getDetrendList(myPosition) == proxyVars::height)
            return currentSettings.getUseHeight();
        else if (currentSettings.getDetrendList(myPosition) == proxyVars::orogIndex)
            return currentSettings.getUseOrogIndex();
        else if (currentSettings.getDetrendList(myPosition) == proxyVars::urbanFraction)
            return currentSettings.getUseUrbanFraction();
        else if (currentSettings.getDetrendList(myPosition) == proxyVars::seaDistance)
            return currentSettings.getUseSeaDistance();
        else if (currentSettings.getDetrendList(myPosition) == proxyVars::aspect)
            return currentSettings.getUseAspect();
        else if (currentSettings.getDetrendList(myPosition) == proxyVars::generic)
            return currentSettings.getUseGenericProxy();
        else
            return (false);
    }
    else
        return false;

}

proxyVars::TProxyVar getDetrendType(int myPosition)
{
    if (myPosition >= 0 && myPosition <= PROXY_VAR_NR)
        return (currentSettings.getDetrendList(myPosition));
    else
        return (proxyVars::noProxy);
}

void detrend(meteoVariable myVar, proxyVars::TProxyVar myProxy)
{
    float detrendValue;
    long myIndex;
    Crit3DInterpolationDataPoint* myPoint;

    if (myVar != precipitation && myVar != dailyPrecipitation
            && myVar != globalIrradiance && myVar != dailyGlobalRadiation)
    {
        for (myIndex = 0; myIndex < long(interpolationPointList.size()); myIndex++)
        {
            detrendValue = 0;
            myPoint = &(interpolationPointList.at(myIndex));
            if (myProxy == proxyVars::height)
            {
                if (currentSettings.getUseHeight() && myPoint->point->z != NODATA)
                {
                    if (inversionIsSignificative)
                        if (myPoint->point->z <= lapseRateH1)
                            detrendValue = maxValue(float(myPoint->point->z) - lapseRateH0, 0) * inversionLapseRate;
                        else
                            detrendValue = ((lapseRateH1 - lapseRateH0) * inversionLapseRate) + (float(myPoint->point->z) - lapseRateH1) * actualLapseRate;
                    else
                        detrendValue = maxValue(float(myPoint->point->z), 0) * actualLapseRate;
                }
            }

            else if (myProxy == proxyVars::urbanFraction)
            {
                if (currentSettings.getUseUrbanFraction() && urbanR2 >= currentSettings.getGenericPearsonThreshold())
                    if (myPoint->getUrbanFraction() != NODATA)
                        detrendValue = myPoint->getUrbanFraction() * urbanCoefficient;
            }

            else if (myProxy == proxyVars::orogIndex)
            {
                if (currentSettings.getUseOrogIndex() && orogIndexR2 >= currentSettings.getGenericPearsonThreshold())
                    if (myPoint->getOrogIndex() != NODATA)
                        detrendValue = myPoint->getOrogIndex() * orogIndexCoefficient;
            }

            else if (myProxy == proxyVars::seaDistance)
            {
                if (currentSettings.getUseSeaDistance() && seaDistR2 >= currentSettings.getGenericPearsonThreshold())
                    if (myPoint->getSeaDistance() != NODATA)
                        detrendValue = myPoint->getSeaDistance() * seaDistCoefficient;
            }

            else if (myProxy == proxyVars::aspect)
            {
                if (currentSettings.getUseAspect() && aspectR2 >= currentSettings.getGenericPearsonThreshold())
                    if (myPoint->getAspect() != NODATA)
                        detrendValue = myPoint->getAspect() * aspectCoefficient;
            }

            myPoint->value -= detrendValue;
        }
    }
}

float retrend(meteoVariable myVar, float myZ, float myOrogIndex, float mySeaDist, float myUrban, float myAspect)
{

    float retrendZ = 0.;
    float retrendIPL = 0.;
    float retrendDistSea = 0.;
    float retrendUrban = 0.;
    float retrendAspect = 0.;

    if ( myVar == airDewTemperature || myVar == airTemperature
         || myVar == dailyAirTemperatureMax
         || myVar == dailyAirTemperatureMin
         || myVar == dailyAirTemperatureAvg )
    {
        if (currentSettings.getUseHeight() && currentSettings.getDetrendOrographyActive() && myZ != NODATA)
        {
            if (currentSettings.getUseThermalInversion() && inversionIsSignificative)
            {
                if (myZ <= lapseRateH1)
                    retrendZ = (maxValue(myZ - lapseRateH0, 0) * inversionLapseRate);
                else
                    retrendZ = ((lapseRateH1 - lapseRateH0) * inversionLapseRate) + (myZ - lapseRateH1) * actualLapseRate;
            }
            else
                retrendZ = maxValue(myZ, 0) * actualLapseRate;
        }

        if (currentSettings.getUseOrogIndex() && currentSettings.getDetrendOrogIndexActive() && myOrogIndex != NODATA)
            retrendIPL = myOrogIndex * orogIndexCoefficient;

        if (currentSettings.getUseUrbanFraction() && currentSettings.getDetrendUrbanActive() && myUrban != NODATA)
            retrendUrban = myUrban * urbanCoefficient;

        if (currentSettings.getUseSeaDistance() && currentSettings.getDetrendSeaDistanceActive() && mySeaDist != NODATA)
            retrendDistSea = mySeaDist * seaDistCoefficient;

        if (currentSettings.getUseAspect() && currentSettings.getDetrendAspectActive() && myAspect != NODATA)
            retrendAspect = myAspect * aspectCoefficient;
    }
    else
        return 0.;

    return (retrendZ + retrendIPL + retrendDistSea + retrendUrban + retrendAspect);
}

bool preInterpolation(meteoVariable myVar)
{
    if (myVar == precipitation || myVar == dailyPrecipitation)
    {
        int nrPrecNotNull;
        bool isFlatPrecipitation;
        precipitationAllZero = checkPrecipitationZero(&nrPrecNotNull, &isFlatPrecipitation);
        if (precipitationAllZero) return true;

        if (currentSettings.getUseJRC()) prepareJRC();
    }

    else if (  myVar == airDewTemperature
            || myVar == airTemperature
            || myVar == dailyAirTemperatureMax
            || myVar == dailyAirTemperatureMin
            || myVar == dailyAirTemperatureAvg )
    {
        for (int pos=0; pos<PROXY_VAR_NR; pos++)
        {
            if (getDetrendActive(pos))
            {
                proxyVars::TProxyVar myProxy = getDetrendType(pos);
                if (myProxy == proxyVars::height)
                {
                    if (regressionOrography(myVar))
                    {
                        currentSettings.setDetrendOrographyActive(true);
                        detrend(myVar, proxyVars::height);
                    }
                    else
                        currentSettings.setDetrendOrographyActive(false);
                }
                else if (myProxy == proxyVars::urbanFraction)
                {
                    currentSettings.setDetrendUrbanActive(regressionGeneric(myProxy, false));
                    if (currentSettings.getDetrendUrbanActive())
                        detrend(myVar, myProxy);
                }
                else if (myProxy == proxyVars::orogIndex)
                {
                    currentSettings.setDetrendOrogIndexActive(regressionGeneric(myProxy, false));
                    if (currentSettings.getDetrendOrogIndexActive())
                        detrend(myVar, myProxy);
                }
                else if (myProxy == proxyVars::seaDistance)
                {
                    currentSettings.setDetrendSeaDistanceActive(regressionGeneric(myProxy, false));
                    if (currentSettings.getDetrendSeaDistanceActive())
                        detrend(myVar, myProxy);
                }
                else if (myProxy == proxyVars::aspect)
                {
                    currentSettings.setDetrendAspectActive(regressionGeneric(myProxy, false));
                    if (currentSettings.getDetrendAspectActive())
                        detrend(myVar, myProxy);
                }
            }
        }
    }

    return (true);
}


float interpolateSimple(meteoVariable myVar, float myZ, float myOrogIndex, float myDistSea, float myUrban, float myAspect)
{
    float myResult = NODATA;

    /*! interpolate residuals */
    if (currentSettings.getInterpolationMethod() == geostatisticsMethods::idw)
    {
        myResult = inverseDistanceWeighted(interpolationPointList);
    }
    else if (currentSettings.getInterpolationMethod() == geostatisticsMethods::kriging)
        myResult = NODATA;

    if (myResult != NODATA)
        return (myResult + retrend(myVar, myZ, myOrogIndex, myDistSea, myUrban, myAspect));
    else
        return NODATA;
}


float interpolate(meteoVariable myVar, float myX, float myY, float myZ, float myOrogIndex, float myUrban, float mySeaDist, float myAspect)
{
    if ((myVar == precipitation || myVar == dailyPrecipitation) && precipitationAllZero) return 0.;

    float myResult = NODATA;

    if (currentSettings.getInterpolationMethod() == geostatisticsMethods::idw)
    {
        if ((myVar == precipitation || myVar == dailyPrecipitation) && currentSettings.getUseJRC())
            assignDistances(&precBinaryPointList, myX, myY, myZ);

        assignDistances(&interpolationPointList, myX, myY, myZ);
    }

    if (myVar == precipitation || myVar == dailyPrecipitation)
    {
        myResult = interpolatePrec();
        if (myResult != NODATA)
            if (!currentSettings.getUseJRC() && myResult <= PREC_THRESHOLD) myResult = 0.;
    }
    else
        myResult = interpolateSimple(myVar, myZ, myOrogIndex, mySeaDist, myUrban, myAspect);

    return myResult;

}

bool interpolateGridDtm(gis::Crit3DRasterGrid* myGrid, const gis::Crit3DRasterGrid& myDTM, meteoVariable myVar)
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
                myGrid->value[myRow][myCol] = interpolate(myVar, myX, myY, myZ, NODATA, NODATA, NODATA, NODATA);
        }

    if (! gis::updateMinMaxRasterGrid(myGrid))
        return (false);

    return (true);
}


// assume che i dati siano già stati caricati in interpolationPointList (checkData)
bool interpolationRaster(meteoVariable myVar, Crit3DInterpolationSettings *mySettings,
                         gis::Crit3DRasterGrid *myRaster, const  gis::Crit3DRasterGrid& myDTM,
                         const Crit3DTime& myTime, std::string *myError)
{
    // Check data presence
    if (interpolationPointList.size() == 0)
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
    setInterpolationSettings(mySettings);

    // Proxy vars regression and detrend
    if (! preInterpolation(myVar))
    {
        *myError = "Interpolation: error in function preInterpolation";
        return false;
    }

    // Interpolate
    if (! interpolateGridDtm(myRaster, myDTM, myVar))
    {
        *myError = "Interpolation: error in function interpolateGridDtm";
        return false;
    }

    Crit3DTime t = myTime;
    myRaster->timeString = t.toStdString();
    return true;
}


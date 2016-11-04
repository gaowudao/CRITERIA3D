/*-----------------------------------------------------------------------------------
    basicMath.cpp
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
#include "commonConstants.h"

namespace basicMath
{
    bool greaterThan(float a, float b)
    {
        return (fabs(a - b) > fabs(b / 100.));
    }

    bool greaterThan(double a, double b)
    {
        return (fabs(a - b) > fabs(b / 100.));
    }

    int integralPart(double number)
    {
        double intPart;
        modf(number,&intPart);
        return (int)(intPart);
    }

    double fractionalPart(double number)
    {
        double intPart,fracPart;
        fracPart = modf(number,&intPart);
        return fracPart;
    }

    double inputSwitch (double x, double y1, double y2)
    {
        if (x < 0) return y1;
        else return y2;
    }
    double stepFunction (double x, double change, double y1, double y2)
    {
        if (x < change) return y1;
        else return y2;
    }
    double boundedValue (double x, double lowerBound, double upperBound)
    {
        if (x < lowerBound) return lowerBound;
        else if (x > upperBound) return upperBound;
        else return x;
    }

    void directRotation(float *point, float angle)
    {
        point[0] = cos(angle)*point[0] - sin(angle)*point[1];
        point[0] = sin(angle)*point[0] + cos(angle)*point[1];
    }

    void inverseRotation(float *point, float angle)
    {
        angle *=-1;
        point[0] = cos(angle)*point[0] - sin(angle)*point[1];
        point[0] = sin(angle)*point[0] + cos(angle)*point[1];
    }

    float distance(float* x,float* y, int vectorLength)
    {
        float dist = 0 ;
        for (int i=0; i<vectorLength;i++)
            dist = pow(x[i]-y[i],2);

        dist = sqrt(dist);
        return dist;
    }
    float distance2D(float x1,float y1, float x2, float y2)
    {
        float dist;
        dist = sqrt(pow((x1-x2),2) + pow((y1-y2),2));
        return dist;
    }

    float norm(float* x, int vectorLength)
    {
        float myNorm = 0 ;
        for (int i=0; i<vectorLength;i++)
            myNorm = pow(x[i],2);

        myNorm = sqrt(myNorm);
        return myNorm;
    }

}

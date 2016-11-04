#ifndef BASICMATH_H
#define BASICMATH_H

/*-----------------------------------------------------------------------------------
    basicMath.h
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


    namespace basicMath
    {
        float getSin(float);
        float getCos(float);
        int integralPart(double number);
        double fractionalPart(double number);
        double inputSwitch (double x, double y1, double y2);
        double stepFunction (double x, double change, double y1, double y2);
        double boundedValue (double x, double lowerBound, double upperBound);
        void directRotation(float *point, float angle);
        void inverseRotation(float *point, float angle);
        float distance(float* x,float* y, int vectorLength);
        float distance2D(float x1,float y1, float x2, float y2);
        float norm(float* x, int vectorLength);
        bool greaterThan(float a, float b);
        bool greaterThan(double a, double b);
    }

#endif // BASICMATH_H

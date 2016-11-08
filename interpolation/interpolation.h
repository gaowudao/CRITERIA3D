/*-----------------------------------------------------------------------------------
    COPYRIGHT 2016 Fausto Tomei, Gabriele Antolini,
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
-----------------------------------------------------------------------------------*/

#ifndef INTERPOLATION_H
#define INTERPOLATION_H

    #ifndef GIS_H
        #include "gis.h"
    #endif
    #ifndef METEO_H
        #include "meteo.h"
    #endif
    #ifndef INTERPOLATIONSETTINGS_H
        #include "interpolationSettings.h"
    #endif
    #ifndef INTERPOLATIONPOINT_H
        #include "interpolationPoint.h"
    #endif

    #define MIN_REGRESSION_POINTS 3

    class Crit3DMeteoPoint;

    enum TkrigingMode {KRIGING_SPHERICAL = 1,
                       KRIGING_EXPONENTIAL=2,
                       KRIGING_GAUSSIAN=3,
                       KRIGING_LINEAR=4
                      };

    void setInterpolationSettings(Crit3DInterpolationSettings *mySettings);
    void setindexPointJacknife(int index);

    bool preInterpolation(meteoVariable myVar);

    bool krigingEstimateVariogram(float *myDist, float *mySemiVar,int sizeMyVar, int nrMyPoints,float myMaxDistance, double *mySill, double *myNugget, double *myRange, double *mySlope, TkrigingMode *myMode, int nrPointData);
    bool krigLinearPrep(double *mySlope, double *myNugget, int nrPointData);

    bool addInterpolationPoint(int index, float myValue, float myX, float myY, float myHeight, float myOrogIndex, float myUrban, float mySeaDist, float myAspect, float myGenericProxy);

    void clearInterpolationPoints();

    bool neighbourhoodVariability(float x, float y, float z, int nMax,
                                  float* devSt, float* devStDeltaZ, float* minDistance);

    float interpolate(meteoVariable myVar, float myX, float myY, float myZ, float myOrogIndex, float myUrban, float mySeaDist, float myAspect);
    bool interpolateGridDtm(gis::Crit3DRasterGrid* myGrid, const gis::Crit3DRasterGrid &myGridDtm, meteoVariable myVar);

#endif // INTERPOLATION_H

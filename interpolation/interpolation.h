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

#ifndef INTERPOLATION_H
#define INTERPOLATION_H

    #ifndef METEO_H
        #include "meteoPoint.h"
    #endif
    #ifndef INTERPOLATIONSETTINGS_H
        #include "interpolationSettings.h"
    #endif
    #ifndef INTERPOLATIONPOINT_H
        #include "interpolationPoint.h"
    #endif

    class Crit3DMeteoPoint;

    bool preInterpolation(std::vector<Crit3DInterpolationDataPoint> &myPoints, Crit3DInterpolationSettings *mySettings, meteoVariable myVar);

    bool krigingEstimateVariogram(float *myDist, float *mySemiVar,int sizeMyVar, int nrMyPoints,float myMaxDistance, double *mySill, double *myNugget, double *myRange, double *mySlope, TkrigingMode *myMode, int nrPointData);
    bool krigLinearPrep(double *mySlope, double *myNugget, int nrPointData);

    void clearInterpolationPoints();

    bool neighbourhoodVariability(std::vector<Crit3DInterpolationDataPoint> &myInterpolationPoints, float x, float y, float z, int nMax,
                                  float* devSt, float* devStDeltaZ, float* minDistance);

    float interpolate(std::vector<Crit3DInterpolationDataPoint> &myPoints, Crit3DInterpolationSettings *mySettings, meteoVariable myVar, float myX, float myY, float myZ, std::vector<float> myProxyValues);
    bool interpolateGridDtm(std::vector<Crit3DInterpolationDataPoint> &myPoints, Crit3DInterpolationSettings* mySettings, gis::Crit3DRasterGrid* myGrid, const gis::Crit3DRasterGrid &myGridDtm, meteoVariable myVar);

    bool interpolationRaster(std::vector<Crit3DInterpolationDataPoint> &myPoints, Crit3DInterpolationSettings *mySettings, meteoVariable myVar,
                             const Crit3DTime& myTime, const gis::Crit3DRasterGrid& myDTM,
                             gis::Crit3DRasterGrid *myRaster, std::string *myError);


#endif // INTERPOLATION_H

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


#include <stdio.h>
#include "commonConstants.h"
#include "quality.h"


namespace quality
{
    qualityRange::qualityRange()
    {
        min = NODATA;
        max = NODATA;
    }

    qualityRange::qualityRange(float myMin, float myMax)
    {
        min = myMin;
        max = myMax;
    }

    float qualityRange::getMin()
    { return min;}

    float qualityRange::getMax()
    { return max;}

    qualityMeteo::qualityMeteo()
    {
        qualityHourlyT = new qualityRange(-50, 60);
        qualityHourlyTd = new qualityRange(-60, 50);
        qualityHourlyP = new qualityRange(0, 300);
        qualityHourlyRH = new qualityRange(0, 102);
        qualityHourlyWInt = new qualityRange(0, 150);
        qualityHourlyWDir = new qualityRange(0, 360);
        qualityHourlyGIrr = new qualityRange(-20, 1353);

        qualityDailyT = new qualityRange(-50, 50);
        qualityDailyP = new qualityRange(0, 800);
        qualityDailyRH = new qualityRange(0, 102);
        qualityDailyWInt = new qualityRange(0, 150);
        qualityDailyWDir = new qualityRange(0, 360);
        qualityDailyGRad = new qualityRange(-20, 120);
    }

    qualityRange* getQualityRange(meteoVariable myVar, qualityMeteo myQualityMeteo)
    {
        if (myVar == airTemperature)
            return myQualityMeteo.qualityHourlyT;
        else if (myVar == airDewTemperature)
            return myQualityMeteo.qualityHourlyTd;
        else if (myVar == precipitation)
            return myQualityMeteo.qualityHourlyP;
        else if (myVar == airHumidity)
            return myQualityMeteo.qualityHourlyRH;
        else if (myVar == windIntensity)
            return myQualityMeteo.qualityHourlyWInt;
        else if (myVar == windDirection)
            return myQualityMeteo.qualityHourlyWDir;
        else if (myVar == globalIrradiance)
            return myQualityMeteo.qualityHourlyGIrr;
        else
            return NULL;
    }
}

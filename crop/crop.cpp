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

#include <stdio.h>
#include <math.h>
#include <malloc.h>
#include <algorithm>

#include "crit3dDate.h"
#include "commonConstants.h"
#include "crop.h"
#include "root.h"


    Crit3DCrop::Crit3DCrop()
    {
        idCrop = "";
        type = HERBACEOUS_ANNUAL;

        //crop cycle
        isLiving = false;
        isEmerged = false;
        sowingDoy = NODATA;
        doyStartSenescence = NODATA;
        plantCycle = NODATA;
        LAImin = NODATA;
        LAImax = NODATA;
        LAIgrass = NODATA;
        LAIcurve_a = NODATA;
        LAIcurve_b = NODATA;
        thermalThreshold = NODATA;
        upperThermalThreshold = NODATA;
        degreeDaysIncrease = NODATA;
        degreeDaysDecrease = NODATA;
        degreeDaysEmergence = NODATA;

        //water need
        kcMax  = NODATA;
        degreeDaysMaxSensibility = NODATA;
        psiLeaf = NODATA;
        stressTolerance = NODATA;              //[0-1]
        frac_read_avail_water_min = NODATA;
        frac_read_avail_water_max = NODATA;

        //irrigation
        irrigationShift = NODATA;
        irrigationVolume = NODATA;
        degreeDaysStartIrrigation = NODATA;
        degreeDaysEndIrrigation = NODATA;
        maxSurfacePuddle = NODATA;

        degreeDays = NODATA;
        LAI = NODATA;
        waterStressSensibility = NODATA;
        lastWaterStress = NODATA;
    }

    bool isPluriannual(speciesType myType)
    {
        return (myType == HERBACEOUS_PERENNIAL ||
                myType == GRASS ||
                myType == FALLOW ||
                myType == FRUIT_TREE);
    }

    bool isGrass(speciesType myType)
    {
        return (myType == GRASS || myType == GRASS_FIRST_YEAR);
    }

    speciesType getCropType(std::string cropType)
    {
        // lower case
        std::transform(cropType.begin(), cropType.end(), cropType.begin(), ::tolower);

        if (cropType == "herbaceous")
            return HERBACEOUS_ANNUAL;
        else if (cropType == "herbaceous_perennial")
            return HERBACEOUS_PERENNIAL;
        else if (cropType == "horticultural")
            return HORTICULTURAL;
        else if (cropType == "grass")
            return GRASS;
        else if (cropType == "grass_first_year")
            return GRASS_FIRST_YEAR;
        else if (cropType == "fallow")
            return FALLOW;
        else if (cropType == "tree", "fruit_tree")
            return FRUIT_TREE;
        else
            return HERBACEOUS_ANNUAL;
    }

    bool isWaterSurplusResistant(Crit3DCrop* myCrop)
    {
        return (isGrass(myCrop->type) || myCrop->type == FALLOW || myCrop->idCrop == "KIWIFRUIT" || myCrop->idCrop == "RICE");
    }

    double computeDegreeDays(double myTmin, double myTmax, double myLowerThreshold, double myUpperThreshold)
    {
        return maxValue((myTmin + minValue(myTmax, myUpperThreshold)) / 2 - myLowerThreshold, 0);
    }



/*!
    \copyright 2018 Fausto Tomei, Gabriele Antolini,
    Alberto Pistocchi, Marco Bittelli, Antonio Volta, Laura Costantini

    This file is part of CRITERIA3D.
    CRITERIA3D has been developed under contract issued by ARPAE Emilia-Romagna

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
    gantolini@arpae.it
*/

#include "commonConstants.h"
#include "crit3dDate.h"
#include "development.h"
#include "soil.h"
#include "Criteria1D.h"
#include "croppingSystem.h"
#include "dbTools.h"
#include "root.h"


int daysSinceIrrigation;


// initialization of crop
void initializeCrop(Criteria1D* myCase, int currentDoy)
{    
    // initialize root density
    if (myCase->myCrop.roots.rootDensity != NULL) free(myCase->myCrop.roots.rootDensity);
    myCase->myCrop.roots.rootDensity = (double*) calloc(myCase->nrLayers, sizeof(double));

    // initialize root depth
    myCase->myCrop.roots.rootDepth = 0;

    // initialize transpiration
    if (myCase->myCrop.roots.transpiration != NULL) free(myCase->myCrop.roots.transpiration);
    myCase->myCrop.roots.transpiration = (double*) calloc(myCase->nrLayers, sizeof(double));

    // root max depth
    if (myCase->myCrop.roots.rootDepthMax > myCase->mySoil.totalDepth)
        myCase->myCrop.roots.rootDepthMax = myCase->mySoil.totalDepth;

    myCase->myCrop.degreeDays = 0;

    if (myCase->meteoPoint.latitude > 0)
        myCase->myCrop.doyStartSenescence = 305;
    else
        myCase->myCrop.doyStartSenescence = 120;

    myCase->myCrop.LAIstartSenescence = NODATA;
    myCase->myCrop.currentSowingDoy = NODATA;

    daysSinceIrrigation = NODATA;

    // is crop living?
    if (myCase->myCrop.isPluriannual())
        myCase->myCrop.isLiving = true;
    else
    {
        myCase->myCrop.isLiving = myCase->myCrop.isInsideTypicalCycle(currentDoy);

        if (myCase->myCrop.isLiving == true)
            myCase->myCrop.currentSowingDoy = myCase->myCrop.sowingDoy;
    }

    // reset crop
    myCase->myCrop.resetCrop(myCase->nrLayers);
}



bool cropWaterDemand(Criteria1D* myCase)
{
    double Kc;                  // crop coefficient
    double TC;                  // momentary turbulence coefficient
    double ke = 0.6;            // evaporation extinction factor (equal to light extinction factor?)
    double maxEvapRatio = 0.66;

    if (myCase->myCrop.idCrop == "" || ! myCase->myCrop.isLiving || myCase->myCrop.LAI == 0)
    {
        myCase->output.dailyMaxEvaporation = myCase->output.dailyEt0 * maxEvapRatio;
        myCase->output.dailyMaxTranspiration = 0.0;
        myCase->output.dailyKc = 0.0;
    }
    else
    {
        Kc = 1 - exp(-ke * myCase->myCrop.LAI);
        TC = 1 + (myCase->myCrop.kcMax - 1.0) * Kc;
        myCase->output.dailyMaxEvaporation = myCase->output.dailyEt0 * maxEvapRatio * (1.0 - Kc);
        myCase->output.dailyKc = TC * Kc;
        myCase->output.dailyMaxTranspiration = myCase->output.dailyEt0 * myCase->output.dailyKc;
    }

    return true;
}



bool updateLai(Criteria1D* myCase, int myDoy)
{
    double degreeDaysLai = 0;
    double myLai = 0;

    if (! myCase->myCrop.isPluriannual())
    {
        if (! myCase->myCrop.isEmerged)
        {
            if (myCase->myCrop.degreeDays < myCase->myCrop.degreeDaysEmergence)
                return true;
            else if (myDoy - myCase->myCrop.sowingDoy >= MIN_EMERGENCE_DAYS)
            {
                myCase->myCrop.isEmerged = true;
                degreeDaysLai = myCase->myCrop.degreeDays - myCase->myCrop.degreeDaysEmergence;
            }
            else
                return true;
        }
        else
        {
            degreeDaysLai = myCase->myCrop.degreeDays - myCase->myCrop.degreeDaysEmergence;
        }

        if (degreeDaysLai > 0)
            myLai = leafDevelopment::getLAICriteria(&(myCase->myCrop), degreeDaysLai);
    }
    else
    {
        if (myCase->myCrop.type == GRASS)
            // grass cut
            if (myCase->myCrop.degreeDays >= myCase->myCrop.degreeDaysIncrease)
                myCase->myCrop.resetCrop(myCase->nrLayers);

        if (myCase->myCrop.degreeDays > 0)
            myLai = leafDevelopment::getLAICriteria(&(myCase->myCrop), myCase->myCrop.degreeDays);
        else
            myLai = myCase->myCrop.LAImin;

        bool inSenescence;
        if (myCase->meteoPoint.latitude > 0)
            inSenescence = (myDoy >= myCase->myCrop.doyStartSenescence);
        else
            inSenescence = ((myDoy >= myCase->myCrop.doyStartSenescence) && (myDoy < 182));

        if (inSenescence)
        {
            if (myDoy == myCase->myCrop.doyStartSenescence || myCase->myCrop.LAIstartSenescence == NODATA)
                myCase->myCrop.LAIstartSenescence = myLai;
            else
                myLai = leafDevelopment::getLAISenescence(myCase->myCrop.LAImin,
                        myCase->myCrop.LAIstartSenescence, myDoy-myCase->myCrop.doyStartSenescence);
        }

        if (myCase->myCrop.type == FRUIT_TREE)
            myLai += myCase->myCrop.LAIgrass;
    }

    myCase->myCrop.LAI = myLai;

    return true;
}


bool updateRoots(Criteria1D* myCase)
{
    root::computeRootDepth(&(myCase->myCrop), myCase->mySoil.totalDepth, myCase->myCrop.degreeDays, myCase->output.dailyWaterTable);

    return root::computeRootDensity(&(myCase->myCrop), myCase->layer, myCase->nrLayers, myCase->mySoil.totalDepth);
}


bool updateCropWaterStressSensibility(Crit3DCrop* myCrop)
{
    if ((myCrop->idCrop == "") || (! myCrop->isLiving))
        return false;

    double avgSensibility = (myCrop->frac_read_avail_water_max + myCrop->frac_read_avail_water_min) / 2.0;
    double diffSensibility = myCrop->frac_read_avail_water_max - myCrop->frac_read_avail_water_min;

    double degreeDaysTot = myCrop->degreeDaysDecrease + myCrop->degreeDaysIncrease + myCrop->degreeDaysEmergence;
    if (degreeDaysTot > 0)
        myCrop->waterStressSensibility = avgSensibility - (diffSensibility / 2.0) * cos(6.28 * (myCrop->degreeDays - myCrop->degreeDaysMaxSensibility) / degreeDaysTot);

    return true;
}


double getWeighredRAW(Criteria1D* myCase)
{
    if (! myCase->myCrop.isLiving) return 0.;
    if (myCase->myCrop.roots.rootDepth <= myCase->myCrop.roots.rootDepthMin) return 0.;
    if (myCase->myCrop.roots.firstRootLayer == NODATA) return 0.;

    double densMax = 0.0;
    for (int i = myCase->myCrop.roots.firstRootLayer; i <= myCase->myCrop.roots.lastRootLayer; i++)
        densMax = maxValue(densMax, myCase->myCrop.roots.rootDensity[i]);

    double depth, threshold, layerRAW;
    double sumRAW = 0.0;

    for (int i = myCase->myCrop.roots.firstRootLayer; i <= myCase->myCrop.roots.lastRootLayer; i++)
    {
        threshold = myCase->layer[i].FC - myCase->myCrop.waterStressSensibility * (myCase->layer[i].FC - myCase->layer[i].WP);

        layerRAW = (myCase->layer[i].waterContent - threshold) *(myCase->myCrop.roots.rootDensity[i] / densMax);

        depth = myCase->layer[i].depth + myCase->layer[i].thickness / 2.0;

        if (myCase->myCrop.roots.rootDepth < depth)
                layerRAW *= (myCase->myCrop.roots.rootDepth - depth) / myCase->layer[i].thickness;

        sumRAW += layerRAW;

    }

    return sumRAW;
}


// soil water deficit [mm]
double getSoilWaterDeficit(Criteria1D* myCase)
{
    //check
    if (! myCase->myCrop.isLiving) return NODATA;
    if (myCase->myCrop.roots.rootDepth <= myCase->myCrop.roots.rootDepthMin) return NODATA;
    if (myCase->myCrop.roots.firstRootLayer == NODATA) return NODATA;

    double myDeficit = 0.0;
    for (int i = myCase->myCrop.roots.firstRootLayer; i <= myCase->myCrop.roots.lastRootLayer; i++)
        myDeficit += myCase->layer[i].FC - myCase->layer[i].waterContent;

    return maxValue(myDeficit, 0.0);
}


/*!
 * \brief getReadilyAvailableWater
 * \param myCase
 * \return sum of readily available water (mm) in the rooting zone (minimum first meter of soil)
 */
double getReadilyAvailableWater(Criteria1D* myCase)
{
    if (! myCase->myCrop.isLiving) return NODATA;
    if (myCase->myCrop.roots.rootDepth <= myCase->myCrop.roots.rootDepthMin) return NODATA;
    if (myCase->myCrop.roots.firstRootLayer == NODATA) return NODATA;

    double threshold;
    int lastLayer = 0;

    while ((lastLayer < (myCase->nrLayers-1)) && (myCase->layer[lastLayer].depth < 1.0))
        lastLayer++;

    lastLayer = maxValue(lastLayer, myCase->myCrop.roots.lastRootLayer);

    // surface water
    double RAW = myCase->layer[0].waterContent;

    for (int i = 1; i <= lastLayer; i++)
    {
        if (i < myCase->myCrop.roots.firstRootLayer)
            threshold = myCase->layer[i].FC;
        else
            // rooting zone
            threshold = myCase->layer[i].FC - myCase->myCrop.waterStressSensibility * (myCase->layer[i].FC - myCase->layer[i].WP);

        if(myCase->layer[i].waterContent > threshold)
            RAW += (myCase->layer[i].waterContent - threshold);
    }

    return RAW;
}


float cropIrrigationDemand(Criteria1D* myCase, float currentPrec, float nextPrec)
{
    // check crop
    if ((myCase->myCrop.idCrop == "")
        || (! myCase->myCrop.isLiving)
        || (myCase->myCrop.irrigationVolume == 0)) return 0;

    // check irrigation period
    if (myCase->myCrop.degreeDays < myCase->myCrop.degreeDaysStartIrrigation ||
            myCase->myCrop.degreeDays > myCase->myCrop.degreeDaysEndIrrigation) return 0.;

    // check rainfall (forecast)
    if (currentPrec >= 5.) return 0.;
    if (myCase->myCrop.irrigationShift > 1)
        if ((currentPrec + nextPrec) >  myCase->myCrop.irrigationVolume * 0.5) return 0.;

    // check water stress
    double threshold = 1. - myCase->myCrop.stressTolerance;
    double waterStress = cropTranspiration(myCase, true);
    if (waterStress <= threshold) return 0.;

    // check readily available water (weighted on root density)
    if (getWeighredRAW(myCase) > 10.) return 0.;

    // check irrigation shift
    if (daysSinceIrrigation != NODATA)
    {
        ++daysSinceIrrigation;
        if (waterStress < (threshold + 0.2))
        {
            if (daysSinceIrrigation < myCase->myCrop.irrigationShift)
                return 0;
        }
        else
        {
            if (daysSinceIrrigation < (myCase->myCrop.irrigationShift * 0.5))
                return 0;
        }
    }

    // all check passed --> IRRIGATION

    // reset irrigation shift
    daysSinceIrrigation = 0;

    if (myCase->optimizeIrrigation)
        return minValue(getSoilWaterDeficit(myCase), myCase->myCrop.irrigationVolume);
    else
        return myCase->myCrop.irrigationVolume;
}


bool irrigateCrop(Criteria1D* myCase, double irrigationDemand)
{
    double myDeficit;

    myCase->output.dailyIrrigation = irrigationDemand;

    int i=0;
    while (i < myCase->nrLayers && irrigationDemand > 0.0f)
    {
        if (myCase->layer[i].waterContent < myCase->layer[i].FC)
        {
            myDeficit = myCase->layer[i].FC - myCase->layer[i].waterContent;
            if (myDeficit > irrigationDemand)
                myDeficit = irrigationDemand;

            myCase->layer[i].waterContent += myDeficit;
            irrigationDemand -= myDeficit;
        }
        i++;
    }

    if (irrigationDemand > 0)
        myCase->output.dailyIrrigation -= irrigationDemand;

    return true;
}


bool evaporation(Criteria1D* myCase)
{
    const double EVAP_THRESHOLD = 0.00001;      //[mm]

    // evaporation on surface
    double evaporationOpenWater = minValue(myCase->output.dailyMaxEvaporation, myCase->layer[0].waterContent);
    myCase->layer[0].waterContent -= evaporationOpenWater;
    myCase->output.dailyEvaporation = evaporationOpenWater;

    double residualEvaporation = myCase->output.dailyMaxEvaporation - evaporationOpenWater;
    if (residualEvaporation < EVAP_THRESHOLD)
        return true;

    // evaporation on soil
    int lastLayerEvap = floor(MAX_EVAPORATION_DEPTH / myCase->layerThickness) +1;
    double* coeffEvap = (double *) calloc(lastLayerEvap, sizeof(double));
    double sumCoeff = 0.0;
    double layerDepth;
    int i;

    for (i=1; i<=lastLayerEvap; i++)
    {
        layerDepth = myCase->layer[i].depth + myCase->layer[i].thickness / 2.0;
        // coeff = ~0.9 at 1 cm, ~0.1 at MAX_EVAPORATION_DEPTH
        coeffEvap[i-1] = minValue(1.0, exp((-layerDepth * 2.0) / MAX_EVAPORATION_DEPTH));
        sumCoeff += coeffEvap[i-1];
    }

    bool isWaterSupply = true;
    double sumEvap, evapLayerThreshold, evapLayer;
    while ((residualEvaporation > EVAP_THRESHOLD) && (isWaterSupply == true))
    {
        isWaterSupply = false;
        sumEvap = 0.0;

        for (i=1; i<=lastLayerEvap; i++)
        {
            evapLayerThreshold = myCase->layer[i].FC - coeffEvap[i-1] * (myCase->layer[i].FC - myCase->layer[i].HH);
            evapLayer = (coeffEvap[i-1] / sumCoeff) * residualEvaporation;

            if (myCase->layer[i].waterContent > (evapLayerThreshold + evapLayer))
                isWaterSupply = true;
            else if (myCase->layer[i].waterContent > evapLayerThreshold)
                evapLayer = myCase->layer[i].waterContent - evapLayerThreshold;
            else
                evapLayer = 0.0;

            myCase->layer[i].waterContent -= evapLayer;
            sumEvap += evapLayer;
        }

        residualEvaporation -= sumEvap;
        myCase->output.dailyEvaporation  += sumEvap;
    }

    free(coeffEvap);

    return true;
}


// return total daily transpiration [mm]
// or percentage of water stress (if getWaterStress = true)
double cropTranspiration(Criteria1D* myCase, bool getWaterStress)
{
    //check
    if (myCase->myCrop.idCrop == "") return 0.0;
    if (! myCase->myCrop.isLiving) return 0.0;
    if (myCase->myCrop.roots.rootDepth <= myCase->myCrop.roots.rootDepthMin) return 0.0;
    if (myCase->myCrop.roots.firstRootLayer == NODATA) return 0.0;

    double thetaWP;                                 // [m3 m-3] volumetric water content at Wilting Point
    double soilThickness;                           // [mm] thickness of soil layer
    double surplusThreshold;                        // [mm] water surplus stress threshold
    double waterScarcityThreshold;                  // [mm] water scarcity stress threshold
    double cropWP;                                  // [mm] wilting point specific for crop
    double WSS;                                     // [] water surplus stress

    double TRs=0.0;                                 // [mm] actual transpiration with only water scarsity stress
    double TRe=0.0;                                 // [mm] actual transpiration with only water surplus stress
    double totRootDensityWithoutStress = 0.0;       // [-]
    double stress = 0.0;                            // [-]
    double balance = 0.0;                           // [mm]
    int i;

    // initialize layer transpiration
    for (i=0; i < myCase->nrLayers; i++)
        myCase->myCrop.roots.transpiration[i] = 0.0;

    if (myCase->output.dailyMaxTranspiration == 0)
        return 0.f;

    // initialize stressed layers
    bool* isLayerStressed = (bool*) calloc(myCase->nrLayers, sizeof(bool));
    for (i=0; i < myCase->nrLayers; i++)
        isLayerStressed[i] = false;

    if (myCase->myCrop.isWaterSurplusResistant())
        WSS = 0.0;
    else
        WSS = 0.5;

    for (i = myCase->myCrop.roots.firstRootLayer; i <= myCase->myCrop.roots.lastRootLayer; i++)
    {
        surplusThreshold = myCase->layer[i].SAT - (WSS * (myCase->layer[i].SAT - myCase->layer[i].FC));

        soilThickness = myCase->layer[i].thickness * myCase->layer[i].soilFraction * 1000.0;    // [mm]

        thetaWP = soil::thetaFromSignPsi(-soil::cmTokPa(myCase->myCrop.psiLeaf), myCase->layer[i].horizon);

        cropWP = thetaWP * soilThickness;                                                         // [mm]

        waterScarcityThreshold = cropWP + myCase->myCrop.waterStressSensibility * (myCase->layer[i].FC - cropWP);

        if (myCase->layer[i].waterContent > surplusThreshold)
        {
            //WATER SURPLUS
            myCase->myCrop.roots.transpiration[i] = myCase->output.dailyMaxTranspiration * myCase->myCrop.roots.rootDensity[i] *
                    (myCase->layer[i].SAT - myCase->layer[i].waterContent) / (myCase->layer[i].SAT - surplusThreshold);

            TRe += myCase->myCrop.roots.transpiration[i];
            TRs += myCase->output.dailyMaxTranspiration * myCase->myCrop.roots.rootDensity[i];
            isLayerStressed[i] = true;
        }
        else if (myCase->layer[i].waterContent < waterScarcityThreshold)
        {
            //WATER SCARSITY
            myCase->myCrop.roots.transpiration[i] = myCase->output.dailyMaxTranspiration * myCase->myCrop.roots.rootDensity[i] *
                    (myCase->layer[i].waterContent - cropWP) / (waterScarcityThreshold - cropWP);

            TRs += myCase->myCrop.roots.transpiration[i];
            TRe += myCase->output.dailyMaxTranspiration * myCase->myCrop.roots.rootDensity[i];
            isLayerStressed[i] = true;
        }
        else
        {
            //normal conditions
            myCase->myCrop.roots.transpiration[i] = myCase->output.dailyMaxTranspiration * myCase->myCrop.roots.rootDensity[i];

            TRs += myCase->myCrop.roots.transpiration[i];
            TRe += myCase->myCrop.roots.transpiration[i];
            isLayerStressed[i] = false;
            totRootDensityWithoutStress +=  myCase->myCrop.roots.rootDensity[i];
        }
    }

    // water compensation (only not stressed layers)
    double value;
    if (myCase->output.dailyMaxTranspiration > 0)
    {
        stress = 1.0 - (TRs / myCase->output.dailyMaxTranspiration);

        if (stress > 0.01 && totRootDensityWithoutStress > 0.01)
        {
            balance = minValue(stress, totRootDensityWithoutStress) * myCase->output.dailyMaxTranspiration;

            for (int i = myCase->myCrop.roots.firstRootLayer; i <= myCase->myCrop.roots.lastRootLayer; i++)
            {
                if (! isLayerStressed[i])
                {
                    value = balance * (myCase->myCrop.roots.rootDensity[i] / totRootDensityWithoutStress);
                    myCase->myCrop.roots.transpiration[i] += value;
                    TRs += value;
                    TRe += value;
                }
            }
        }
    }

    free(isLayerStressed);

    if (getWaterStress)
    {
        // return water stress
        return 1.0 - (TRs / myCase->output.dailyMaxTranspiration);
    }
    else
    {
        // update water content
        double dailyTranspiration = 0.0;
        for (i = myCase->myCrop.roots.firstRootLayer; i <= myCase->myCrop.roots.lastRootLayer; i++)
        {
            myCase->layer[i].waterContent -= myCase->myCrop.roots.transpiration[i];
            dailyTranspiration += myCase->myCrop.roots.transpiration[i];
        }

        return dailyTranspiration;
    }

}


/*
// OLD CriteriaBdP version - following Driessen (1992)Land-use systems analysis, p89-108
bool cropTranspiration_old(Criteria1D* myCase)
{
    //check
    if (myCase->myCrop.idCrop == "") return false;
    if (! myCase->myCrop.isLiving) return true;
    if (myCase->myCrop.roots.rootDepth <= myCase->myCrop.roots.rootDepthMin) return true;
    if (myCase->myCrop.roots.firstRootLayer == NODATA) return true;

    double psi;                                     // [kPa] water potential
    double avgPSI;                                  // [cm] average water potential of rooted soil
    double kPsi;                                    // [cm d-1] water conductivity
    double avgKpsi;                                 // [cm d-1] average water conductivity of rooted soil
    double theta;                                   // [m3 m-3] volumetric water content
    double soilThickness;                           // [mm] thickness of soil layer
    double criticalWC;                              // [mm] critical water content
    double wpCrop;                                  // [m3 m-3] crop wilting point
    double WSS;                                     // [] water surplus stress
    double MUR;                                     // [mm d-1] maximum uptake ratio
    double rRoot;                                   // [d] resistance
    double rPlant;                                  // plant resistance

    double sumPsi = 0.0;
    double sumKpsi = 0.0;
    double TRs=0.0, TRe=0.0;
    double totRootDensityWithoutStress = 0.0;
    double stress, stressThreshold;
    int i;

    for (i=0; i < myCase->nrLayers; i++)
        myCase->myCrop.roots.transpiration[i] = 0.0;

    if (myCase->output.dailyMaxTranspiration == 0)
        return true;

    bool* isStressed = (bool*) calloc(myCase->nrLayers, sizeof(bool));
    for (i=0; i < myCase->nrLayers; i++) isStressed[i] = false;

    int nrRootedLayers=0;

    for (i = myCase->myCrop.roots.firstRootLayer; i <= myCase->myCrop.roots.lastRootLayer; i++)
    {
        nrRootedLayers++;
        soilThickness = myCase->layer[i].thickness * myCase->layer[i].soilFraction * 1000.0;
        theta = myCase->layer[i].waterContent / soilThickness;
        psi = soil::psiFromTheta(theta, myCase->layer[i].horizon);
        kPsi = soil::waterConductivity(soil::SeFromTheta(theta, myCase->layer[i].horizon), myCase->layer[i].horizon);

        sumKpsi += log(kPsi);
        sumPsi += log(psi + 0.01);
    }

    avgPSI = soil::kPaToCm(exp(sumPsi / nrRootedLayers));  // [cm]
    avgKpsi = exp(sumKpsi / nrRootedLayers);

    if (avgKpsi == 0.0)
        rRoot = 0.0;
    else
        rRoot = 13.0 / (myCase->myCrop.roots.rootDepth * 100.0 * avgKpsi);

    rPlant = 680.0 + 0.53 * myCase->myCrop.psiLeaf;

    // from cm to mm
    MUR = maxValue(10.0 * (myCase->myCrop.psiLeaf - avgPSI) / (rPlant + rRoot), 0.0);

    if (myCase->myCrop.isWaterSurplusResistant())
        WSS = 0.0;
    else
        WSS = 0.5;

    for (i = myCase->myCrop.roots.firstRootLayer; i <= myCase->myCrop.roots.lastRootLayer; i++)
    {
        criticalWC = myCase->layer[i].SAT - (WSS * (myCase->layer[i].SAT - myCase->layer[i].FC));
        theta = soil::thetaFromSignPsi(-soil::cmTokPa(myCase->myCrop.psiLeaf), myCase->layer[i].horizon);
        soilThickness = myCase->layer[i].thickness * myCase->layer[i].soilFraction * 1000.0;
        wpCrop = theta * soilThickness;

        if (myCase->layer[i].waterContent > criticalWC)
        {
            // water content beyond critical value, transpiration is reduced linearly
            myCase->myCrop.roots.transpiration[i] = myCase->output.dailyMaxTranspiration * myCase->myCrop.roots.rootDensity[i] *
                    (myCase->layer[i].SAT - myCase->layer[i].waterContent) / (myCase->layer[i].SAT - criticalWC);

            TRe += myCase->myCrop.roots.transpiration[i];
            TRs += myCase->output.dailyMaxTranspiration * myCase->myCrop.roots.rootDensity[i];
            isStressed[i] = true;
        }
        else
        {
            if (MUR >= myCase->output.dailyMaxTranspiration)
                myCase->myCrop.roots.transpiration[i] = myCase->output.dailyMaxTranspiration * myCase->myCrop.roots.rootDensity[i];
            else
                myCase->myCrop.roots.transpiration[i] = MUR * myCase->myCrop.roots.rootDensity[i];

            // near wilting point
            if (myCase->myCrop.roots.transpiration[i] > myCase->layer[i].waterContent - wpCrop)
            {
                myCase->myCrop.roots.transpiration[i] = maxValue(myCase->layer[i].waterContent - wpCrop, 0.0);
            }

            // stress
            stressThreshold = myCase->layer[i].FC - myCase->myCrop.waterStressSensibility * (myCase->layer[i].FC - myCase->layer[i].WP);
            if (myCase->layer[i].waterContent > stressThreshold)
            {
                isStressed[i] = false;
                totRootDensityWithoutStress +=  myCase->myCrop.roots.rootDensity[i];
            }
            else  isStressed[i] = true;

            TRs += myCase->myCrop.roots.transpiration[i];
            TRe += myCase->output.dailyMaxTranspiration * myCase->myCrop.roots.rootDensity[i];
        }
    }

    // compensation for water deficit stress
    double compensation, compensationFraction, compensationFactor, value;
    compensationFactor = 0.25;
    if (myCase->output.dailyMaxTranspiration > 0)
    {
        stress = maxValue(1.0 - (TRs / myCase->output.dailyMaxTranspiration), 0.0);

        if (stress > 0.01 && totRootDensityWithoutStress > 0.01)
        {
            compensationFraction = minValue(stress, compensationFactor * totRootDensityWithoutStress);
            compensation = compensationFraction * myCase->output.dailyMaxTranspiration;

            for (int i = myCase->myCrop.roots.firstRootLayer; i <= myCase->myCrop.roots.lastRootLayer; i++)
                if (! isStressed[i])
                {
                    value = compensation * (myCase->myCrop.roots.rootDensity[i] / totRootDensityWithoutStress);
                    myCase->myCrop.roots.transpiration[i] += value;
                    TRs += value;
                    TRe += value;
                }
        }
    }

    for (i = myCase->myCrop.roots.firstRootLayer; i <= myCase->myCrop.roots.lastRootLayer; i++)
    {
        myCase->layer[i].waterContent -= myCase->myCrop.roots.transpiration[i];
        myCase->output.dailyTranspiration += myCase->myCrop.roots.transpiration[i];
    }

    myCase->myCrop.lastWaterStress = 1.0 - (TRs / myCase->output.dailyMaxTranspiration);

    free(isStressed);

    return true;
}
*/


bool updateCrop(Criteria1D* myCase, std::string* myError, Crit3DDate myDate,
                double tmin, double tmax, float waterTableDepth)
{
    *myError = "";

    if (myCase->myCrop.idCrop == "")
        return false;

    // check start/end crop cycle (update isLiving)
    if (myCase->myCrop.needReset(myDate, myCase->meteoPoint.latitude, waterTableDepth))
        myCase->myCrop.resetCrop(myCase->nrLayers);

    if (myCase->myCrop.isLiving)
    {
        int currentDoy = getDoyFromDate(myDate);

        // update degree days
        myCase->myCrop.degreeDays += computeDegreeDays(tmin, tmax, myCase->myCrop.thermalThreshold, myCase->myCrop.upperThermalThreshold);

        // update LAI
        if (! updateLai(myCase, currentDoy))
        {
            *myError = "Error in updating LAI for crop " + myCase->myCrop.idCrop;
            return false;
        }

        // update roots
        if (! updateRoots(myCase))
        {
            *myError = "Error in updating roots for crop " + myCase->myCrop.idCrop;
            return false;
        }

        // update water stress sensibility
        if (! updateCropWaterStressSensibility(&(myCase->myCrop)))
        {
            *myError = "Error in updating water stress sensibility for crop " + myCase->myCrop.idCrop;
            return false;
        }
    }

    return true;
}

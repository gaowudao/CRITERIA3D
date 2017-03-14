#include "crit3dDate.h"
#include "commonConstants.h"
#include "development.h"
#include "soil.h"
#include "Criteria1D.h"
#include "croppingSystem.h"


double LAI_StartSenescence;
int daysSinceIrrigation;


// initialization of crop
bool initializeCrop(Criteria1D* myCase)
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

    LAI_StartSenescence = NODATA;
    daysSinceIrrigation = NODATA;
    myCase->myCrop.lastWaterStress = NODATA;

    return true;
}

// reset of (already initialized) crop
// TODO: partenza intelligente (usando sowing doy e ciclo)
bool resetCrop(Criteria1D* myCase)
{
    // roots
    if (! isPluriannual(myCase->myCrop.type))
    {
        myCase->myCrop.roots.rootDensity[0] = 0.0;
        for (int i = 1; i < myCase->nrLayers; i++)
            myCase->myCrop.roots.rootDensity[i] = 0;
    }

    myCase->myCrop.isEmerged = false;

    if (myCase->myCrop.isLiving)
    {
        myCase->myCrop.degreeDays = 0;
        myCase->myCrop.waterStressSensibility = 0;
        // LAI
        myCase->myCrop.LAI = myCase->myCrop.LAImin;
        if (myCase->myCrop.type == FRUIT_TREE)
            myCase->myCrop.LAI += myCase->myCrop.LAIgrass;
    }
    else
    {
        myCase->myCrop.degreeDays = NODATA;
        myCase->myCrop.LAI = NODATA;
        myCase->myCrop.waterStressSensibility = NODATA;
    }

    LAI_StartSenescence = NODATA;
    myCase->myCrop.lastWaterStress = 0;

    return true;
}

bool isInsideCycle(Crit3DCrop* myCrop, int myDoy)
{
    return (((myDoy >= myCrop->sowingDoy)
             && ((myDoy - myCrop->sowingDoy) % 365) < myCrop->plantCycle));
}

bool isSowingDoy(Crit3DCrop* myCrop, int myDoy)
{
    return ((myDoy == myCrop->sowingDoy) || (myDoy == myCrop->sowingDoy + 365));
}


bool cropWaterDemand(Criteria1D* myCase, int myDoy, double tmax, double tmin, double et0)
{
    double Kc;                  // crop coefficient
    double TC;                  // momentary turbulence coefficient
    double ke = 0.6;            // evaporation extinction factor (should be equal to light extinction factor)
    double maxEvapRatio = 0.66;

    if (et0 != NODATA)
        myCase->output.dailyEt0 = et0;
    else
        myCase->output.dailyEt0 = ET0_Hargreaves(0.17, myCase->meteoPoint.latitude, myDoy, tmax, tmin);


    if (myCase->myCrop.idCrop == "" || ! myCase->myCrop.isLiving || myCase->myCrop.LAI == 0)
    {
        myCase->output.dailyMaxEvaporation = myCase->output.dailyEt0 * maxEvapRatio;
        myCase->output.dailyMaxTranspiration = 0.0;
    }
    else
    {
        Kc = 1 - exp(-ke * myCase->myCrop.LAI);
        TC = 1 + (myCase->myCrop.kcMax - 1.0) * Kc;
        myCase->output.dailyMaxEvaporation = myCase->output.dailyEt0 * maxEvapRatio * (1.0 - Kc);
        myCase->output.dailyMaxTranspiration = myCase->output.dailyEt0 * TC * Kc;
    }

    return true;
}


bool updateLai(Criteria1D* myCase, int myDoy)
{
    double degreeDaysLai = 0;
    double myLai = 0;

    if (! isPluriannual(myCase->myCrop.type))
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
        if (isGrass(myCase->myCrop.type))
            // grass cut
            if (myCase->myCrop.degreeDays >= myCase->myCrop.degreeDaysIncrease)
                if (! resetCrop(myCase))
                    return false;

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
            if (myDoy == myCase->myCrop.doyStartSenescence || LAI_StartSenescence == NODATA)
                LAI_StartSenescence = myLai;
            else
                myLai = leafDevelopment::getLAISenescence(myCase->myCrop.LAImin, LAI_StartSenescence, myDoy-myCase->myCrop.doyStartSenescence);
        }

        if (myCase->myCrop.type == FRUIT_TREE)
            myLai += myCase->myCrop.LAIgrass;
    }

    myCase->myCrop.LAI = myLai;

    return true;
}


bool updateRoots(Criteria1D* myCase)
{
    root::computeRootDepth(&(myCase->myCrop), myCase->mySoil.totalDepth, myCase->myCrop.degreeDays);

    return root::computeRootDensity(&(myCase->myCrop), myCase->layer, myCase->nrLayers, myCase->mySoil.totalDepth);
}


bool updateCropWaterStressSensibility(Crit3DCrop* myCrop, int myDoy)
{
    if (myCrop->idCrop == "") return false;

    if (isPluriannual(myCrop->type) || isInsideCycle(myCrop, myDoy))
    {
        double avgSensibility = (myCrop->frac_read_avail_water_max + myCrop->frac_read_avail_water_min) / 2.0;
        double diffSensibility = myCrop->frac_read_avail_water_max - myCrop->frac_read_avail_water_min;

        double degreeDaysTot = myCrop->degreeDaysDecrease + myCrop->degreeDaysIncrease + myCrop->degreeDaysEmergence;
        if (degreeDaysTot > 0)
            myCrop->waterStressSensibility = avgSensibility - (diffSensibility / 2.0) * cos(6.28 * (myCrop->degreeDays - myCrop->degreeDaysMaxSensibility) / degreeDaysTot);
    }

    return true;
}

double getTotalEasyWater(Criteria1D* myCase)
{
    double myEasyWater = 0.0;

    if (! myCase->myCrop.isLiving) return myEasyWater;
    if (myCase->myCrop.roots.rootDepth <= myCase->myCrop.roots.rootDepthMin) return myEasyWater;

    double myDepth;
    double densMax = 0.0;
    double threshold, deltaAW;
    int i;

    for (i=myCase->myCrop.roots.firstRootLayer; i <= myCase->myCrop.roots.lastRootLayer; i++)
       densMax = maxValue(densMax, myCase->myCrop.roots.rootDensity[i]);

    for (i=myCase->myCrop.roots.firstRootLayer; i <= myCase->myCrop.roots.lastRootLayer; i++)
    {
        threshold = myCase->layer[i].FC -
                myCase->myCrop.waterStressSensibility * (myCase->layer[i].FC - myCase->layer[i].WP);

        deltaAW = (myCase->layer[i].waterContent - threshold) *(myCase->myCrop.roots.rootDensity[i] / densMax);

        myDepth = myCase->layer[i].depth + myCase->layer[i].thickness / 2.0;
        if (myCase->myCrop.roots.rootDepth < myDepth)
            deltaAW *= (myCase->myCrop.roots.rootDepth - myDepth) / myCase->layer[i].thickness;

        myEasyWater += deltaAW;

    }

    return myEasyWater;
}

double getTotalDeficit(Criteria1D* myCase)
{
    double myDeficit = 0.0;

    for (int i=myCase->myCrop.roots.firstRootLayer; i <= myCase->myCrop.roots.lastRootLayer; i++)
        myDeficit += myCase->layer[i].FC - myCase->layer[i].waterContent;

    return maxValue(myDeficit, 0.0);
}


// sum of readily available water (mm) in surface and in the rooting zone
double getReadilyAvailableWater(Criteria1D* myCase)
{
    if (! myCase->myCrop.isLiving) return NODATA;

    double threshold;
    // surface water
    double RAW = myCase->layer[0].waterContent;

    for (int i = 1; i <= myCase->myCrop.roots.lastRootLayer; i++)
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



float cropIrrigationDemand(Criteria1D* myCase, float currentPrec, float nextPrec, int doy)
{
    // check crop
    if ((myCase->myCrop.idCrop == "")
        || (! myCase->myCrop.isLiving)
        || (myCase->myCrop.irrigationVolume == 0)) return 0;

    // check alfalfa
    // TODO migliorare
    if ((myCase->myCrop.idCrop == "ALFALFA")
       && (doy >= 243)) return 0;

    // check irrigation period
    if (myCase->myCrop.degreeDays < myCase->myCrop.degreeDaysStartIrrigation ||
            myCase->myCrop.degreeDays > myCase->myCrop.degreeDaysEndIrrigation) return 0.;

    // check irrigation shift
    if (daysSinceIrrigation != NODATA &&
            ++daysSinceIrrigation < myCase->myCrop.irrigationShift) return 0;

    // check rainfall forecast
    if (currentPrec > 5.) return 0.;
    if (myCase->myCrop.irrigationShift > 1)
        if ((currentPrec + nextPrec) >  myCase->myCrop.irrigationVolume * 0.5) return 0.;

    // check stress tolerance
    double threshold = 1. - myCase->myCrop.stressTolerance;
    if ((myCase->myCrop.lastWaterStress - threshold) < 0.001) return 0.;

    // check readily available water (depending on water sensitivity)
    if (getTotalEasyWater(myCase) > 0.) return 0.;

    // all check passed --> IRRIGATION

    // reset irrigation shift
    daysSinceIrrigation = 0;

    if (myCase->optimizeIrrigation)
        return minValue(getTotalDeficit(myCase), myCase->myCrop.irrigationVolume);
    else
        return myCase->myCrop.irrigationVolume;
}


bool irrigateCrop(Criteria1D* myCase, double irrigationDemand)
{
    double myDeficit;

    int i=0;

    myCase->output.dailyIrrigation = irrigationDemand;

    while (i<myCase->nrLayers && irrigationDemand>0)
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
    // open water
    double evaporationOpenWater = minValue(myCase->output.dailyMaxEvaporation, myCase->layer[0].waterContent);
    double residualEvaporation = maxValue(0.0, myCase->output.dailyMaxEvaporation - myCase->layer[0].waterContent);
    myCase->layer[0].waterContent -= evaporationOpenWater;
    myCase->output.dailyEvaporation = evaporationOpenWater;

    // soil
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
    while (residualEvaporation > 0.00001 && isWaterSupply)
    {
        isWaterSupply = false;
        sumEvap = 0.0;

        for (i=1; i<=lastLayerEvap; i++)
        {
            evapLayerThreshold = myCase->layer[i].FC - coeffEvap[i] * (myCase->layer[i].FC - myCase->layer[i].HH);
            evapLayer = (coeffEvap[i] / sumCoeff) * residualEvaporation;

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


bool cropTranspiration(Criteria1D* myCase)
{

    if (myCase->myCrop.idCrop == "") return false;
    if (! myCase->myCrop.isLiving) return true;
    if (myCase->myCrop.roots.rootDepth <= myCase->myCrop.roots.rootDepthMin) return true;

    double theta;                                   // [m3 m-3] volumetric water content
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
    myCase->output.dailyTranspiration = 0.0;
    for (i=0; i < myCase->nrLayers; i++)
        myCase->myCrop.roots.transpiration[i] = 0.0;

    if (myCase->output.dailyMaxTranspiration == 0) return true;

    // initialize stressed layers
    bool* isStressed = (bool*) calloc(myCase->nrLayers, sizeof(bool));
    for (i=0; i < myCase->nrLayers; i++) isStressed[i] = false;

    //TODO check on kiwifruit
    if (isWaterSurplusResistant(&(myCase->myCrop)))
        WSS = 0.0;
    else
        WSS = 0.5;

    for (i = myCase->myCrop.roots.firstRootLayer; i <= myCase->myCrop.roots.lastRootLayer; i++)
    {
        surplusThreshold = myCase->layer[i].SAT - (WSS * (myCase->layer[i].SAT - myCase->layer[i].FC));

        theta = soil::thetaFromSignPsi(-myCase->myCrop.psiLeaf * 101.0 / 1033.0, myCase->layer[i].horizon);
        //[mm]
        soilThickness = myCase->layer[i].thickness * myCase->layer[i].soilFraction * 1000.0;
        //[mm]
        cropWP = theta * soilThickness;

        waterScarcityThreshold = cropWP + myCase->myCrop.waterStressSensibility * (myCase->layer[i].FC - cropWP);

        if (myCase->layer[i].waterContent > surplusThreshold)
        {
            // water surplus
            myCase->myCrop.roots.transpiration[i] = myCase->output.dailyMaxTranspiration * myCase->myCrop.roots.rootDensity[i] *
                    (myCase->layer[i].SAT - myCase->layer[i].waterContent) / (myCase->layer[i].SAT - surplusThreshold);

            TRe += myCase->myCrop.roots.transpiration[i];
            TRs += myCase->output.dailyMaxTranspiration * myCase->myCrop.roots.rootDensity[i];
            isStressed[i] = true;
        }
        else if (myCase->layer[i].waterContent < waterScarcityThreshold)
        {
            // water scarcity
            myCase->myCrop.roots.transpiration[i] = myCase->output.dailyMaxTranspiration * myCase->myCrop.roots.rootDensity[i] *
                    (myCase->layer[i].waterContent - cropWP) / (waterScarcityThreshold - cropWP);

            TRs += myCase->myCrop.roots.transpiration[i];
            TRe += myCase->output.dailyMaxTranspiration * myCase->myCrop.roots.rootDensity[i];
            isStressed[i] = true;
        }
        else
        {
            // normal conditions
            myCase->myCrop.roots.transpiration[i] = myCase->output.dailyMaxTranspiration * myCase->myCrop.roots.rootDensity[i];

            TRs += myCase->myCrop.roots.transpiration[i];
            TRe += myCase->myCrop.roots.transpiration[i];
            isStressed[i] = false;
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
                if (! isStressed[i])
                {
                    value = balance * (myCase->myCrop.roots.rootDensity[i] / totRootDensityWithoutStress);
                    myCase->myCrop.roots.transpiration[i] += value;
                    TRs += value;
                    TRe += value;
                }
            }
        }
    }

    for (i=myCase->myCrop.roots.firstRootLayer; i <= myCase->myCrop.roots.lastRootLayer; i++)
    {
        myCase->layer[i].waterContent -= myCase->myCrop.roots.transpiration[i];
        myCase->output.dailyTranspiration += myCase->myCrop.roots.transpiration[i];
    }

    myCase->myCrop.lastWaterStress = 1.0 - (TRs / myCase->output.dailyMaxTranspiration);

    free(isStressed);

    return true;
}


bool cropTranspiration_old(Criteria1D* myCase)
{
    // following Driessen (1992)Land-use systems analysis, p89-108

    if (myCase->myCrop.idCrop == "") return false;

    if (! myCase->myCrop.isLiving) return true;

    if (myCase->myCrop.roots.rootDepth <= myCase->myCrop.roots.rootDepthMin) return true;

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

    avgPSI = exp(sumPsi / nrRootedLayers) * (1033. / 101.); //from kPa to cm
    avgKpsi = exp(sumKpsi / nrRootedLayers);

    if (avgKpsi == 0.0)
        rRoot = 0.0;
    else
        rRoot = 13.0 / (myCase->myCrop.roots.rootDepth * 100.0 * avgKpsi);

    rPlant = 680.0 + 0.53 * myCase->myCrop.psiLeaf;

    // from cm to mm
    MUR = maxValue(10.0 * (myCase->myCrop.psiLeaf - avgPSI) / (rPlant + rRoot), 0.0);

    if (isWaterSurplusResistant(&(myCase->myCrop)))
        WSS = 0.0;
    else
        WSS = 0.5;

    for (i=myCase->myCrop.roots.firstRootLayer; i <= myCase->myCrop.roots.lastRootLayer; i++)
    {
        criticalWC = myCase->layer[i].SAT - (WSS * (myCase->layer[i].SAT - myCase->layer[i].FC));
        theta = soil::thetaFromSignPsi(-myCase->myCrop.psiLeaf * 101.0 / 1033.0, myCase->layer[i].horizon);
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

    for (i=myCase->myCrop.roots.firstRootLayer; i <= myCase->myCrop.roots.lastRootLayer; i++)
    {
        myCase->layer[i].waterContent -= myCase->myCrop.roots.transpiration[i];
        myCase->output.dailyTranspiration += myCase->myCrop.roots.transpiration[i];
    }

    myCase->myCrop.lastWaterStress = 1.0 - (TRs / myCase->output.dailyMaxTranspiration);

    free(isStressed);

    return true;
}


bool updateCrop(Criteria1D* myCase, QString* myError, Crit3DDate myDate, bool isFirstSimulationDay, double tmin, double tmax)
{
    *myError = "";

    if (myCase->myCrop.idCrop == "") return false;

    bool isCropToReset = false;

    int currentDoy = getDoyFromDate(myDate);

    QString nextCrop = "";

    // reset pluriannual
    if (isPluriannual(myCase->myCrop.type))
    {
        if (! myCase->myCrop.isLiving)
        {
            myCase->myCrop.isLiving = true;
            isCropToReset = true;
        }

        // check end of year for pluriannual
        if(myCase->meteoPoint.latitude >= 0 && myDate.month == 1 && myDate.day == 1 ||
           myCase->meteoPoint.latitude < 0 && myDate.month == 7 && myDate.day == 1)
        {
            isCropToReset = true;
        }
    }
    else
    {
        if (isInsideCycle(&(myCase->myCrop), currentDoy))
        {
            if (myCase->myCrop.isLiving)
            {
                // forced harvest (out of degree days)
                if (myCase->myCrop.type != GRASS_FIRST_YEAR)
                {
                    if (myCase->myCrop.degreeDays > (myCase->myCrop.degreeDaysIncrease + myCase->myCrop.degreeDaysDecrease + myCase->myCrop.degreeDaysEmergence))
                    {
                        myCase->myCrop.isLiving = false;
                        isCropToReset = true;
                    }
                }
                else
                {
                    // first grass cut
                    if (myCase->myCrop.degreeDays >= myCase->myCrop.degreeDaysIncrease)
                    {
                        // alfalfa
                        if (myCase->myCrop.idCrop == "ALFALFA1Y")
                        {
                            // risolvere meglio: una sola coltura e gestire primo anno da dentro?
                            nextCrop = "ALFALFA";
                            isCropToReset = true;
                        }
                    }
                }
            }
            else
            {
                if (isFirstSimulationDay || isSowingDoy(&(myCase->myCrop), currentDoy))
                {
                    // reset or sowing
                    myCase->myCrop.isLiving = true;
                    isCropToReset = true;
                }
                else return true;
            }    
        }
        else
        {
            if (myCase->myCrop.isLiving)
            {
                myCase->myCrop.isLiving = false;
                isCropToReset = true;
            }
        }
    }

    if (nextCrop != "")
        if (! myCase->loadCropParameters(nextCrop, myError))
            return false;

    if (isCropToReset)
        if (! resetCrop(myCase))
            *myError = "Error in resetting crop " + QString::fromStdString(myCase->myCrop.idCrop);

    // update degree days
    if (myCase->myCrop.isLiving)
        myCase->myCrop.degreeDays += computeDegreeDays(tmin, tmax, myCase->myCrop.thermalThreshold, myCase->myCrop.upperThermalThreshold);

    // update LAI
    if (myCase->myCrop.isLiving)
        if (! updateLai(myCase, currentDoy))
            *myError = "Error in updating LAI for crop " + QString::fromStdString(myCase->myCrop.idCrop);

    // update roots
    if (! updateRoots(myCase))
        *myError = "Error in updating roots for crop " + QString::fromStdString(myCase->myCrop.idCrop);

    // update water stress sensibility
    if (myCase->myCrop.isLiving)
        if (! updateCropWaterStressSensibility(&(myCase->myCrop), currentDoy))
            *myError = "Error in updating water stress sensibility for crop " + QString::fromStdString(myCase->myCrop.idCrop);

    return true;
}

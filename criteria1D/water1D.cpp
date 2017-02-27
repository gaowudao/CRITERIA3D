#include <math.h>

#include "commonConstants.h"
#include "Criteria1D.h"
#include "water1D.h"
#include "crop.h"



double getWaterContent(soil::Crit3DLayer *myLayer, double availableWater)
// --------------------------------------------------------------------------------
// availableWater   [-] fraction of water between wilting point and field capacity
// waterContent     [mm] water content in the layer
// --------------------------------------------------------------------------------
{
    if (availableWater < 0)
        return (myLayer->WP);
    else if (availableWater > 1)
        return (myLayer->FC);
    else
        return (myLayer->WP + availableWater * (myLayer->FC - myLayer->WP));
}



void initializeWater(Criteria1D* myCase)
// ----------------------------------------------------------------------------
// Assign two different initial available water
// initialAW[0]         [-] available water in the ploughed soil layer
// initialAW[1]         [-] available water in the deep soil
//
// TODO migliorare in base a stagione come in Vintage (anche in BDP?)
// ----------------------------------------------------------------------------
{
    myCase->layer[0].waterContent = 0.0;
    for (int i = 1; i < myCase->nrLayers; i++)
    {
        if (myCase->layer[i].depth <= myCase->depthPloughedSoil)
            myCase->layer[i].waterContent = getWaterContent(&(myCase->layer[i]), myCase->initialAW[0]);
        else
            myCase->layer[i].waterContent = getWaterContent(&(myCase->layer[i]), myCase->initialAW[1]);
    }
}


bool empiricalInfiltration(Criteria1D* myCase, QString* myError, float prec, float surfaceIrrigation)
{
    int i, j, l, nrPloughLayers;
    int reached = NODATA;            // [-] index of reached layer for surpuls water
    double avgPloughSat = NODATA;    // [-] average degree of saturation ploughed soil
    double fluxLayer = NODATA;       // [mm]
    double residualFlux = NODATA;    // [mm]
    double localFlux = NODATA;       // [mm]
    double waterSurplus = NODATA;    // [mm]
    double waterDeficit = NODATA;    // [mm]
    double localWater = NODATA;      // [mm]
    double distrH2O = NODATA;        // [mm] la quantità di acqua (=imax dello strato sotto) che potrebbe saturare il profilo sotto lo strato in surplus
    double maxInfiltration = NODATA; // [mm] maximum infiltration (Driessen)

    *myError = "";

    // Assign precipitation (surface pond)
    myCase->layer[0].waterContent += prec + surfaceIrrigation;

    // Initialize fluxes
    for (i = 0; i< myCase->nrLayers; i++)
    {
        myCase->layer[i].flux = 0.0;
        myCase->layer[9].maxInfiltration = 0.0;
    }

    // Average degree of saturation (ploughed soil)
    // TODO strati geometrici
    i = 1;
    nrPloughLayers = 0;
    avgPloughSat = 0.0;
    while ((i < myCase->nrLayers) && (myCase->layer[i].depth < myCase->depthPloughedSoil))
    {
        nrPloughLayers++;
        avgPloughSat += (myCase->layer[i].waterContent / myCase->layer[i].SAT);
        i++;
    }
    avgPloughSat /= nrPloughLayers;

    // compute max infiltration:
    // infiltration due to gravitational force and permeability
    // Driessen, 1986
    double permFactor = 1.0 - avgPloughSat;
    for (i = 1; i< myCase->nrLayers; i++)
    {
        maxInfiltration = 10.0 * myCase->layer[i].horizon->Driessen.gravConductivity;
        if (myCase->layer[i].depth < myCase->depthPloughedSoil)
            maxInfiltration += 10.0 * permFactor * myCase->layer[i].horizon->Driessen.maxSorptivity;
        myCase->layer[i].maxInfiltration = maxInfiltration;
    }

    myCase->layer[0].maxInfiltration = myCase->layer[1].maxInfiltration;
    myCase->layer[0].flux = 0.0;

    for (l = myCase->nrLayers-1; l >= 0; l--)
    {
        // find layer with water surplus
        if (myCase->layer[l].waterContent > myCase->layer[l].critical)
        {
            fluxLayer = minValue(myCase->layer[l].maxInfiltration, myCase->layer[l].waterContent - myCase->layer[l].critical);
            myCase->layer[l].flux += fluxLayer;
            myCase->layer[l].waterContent -= fluxLayer;

            // TODO tradurre
            // cerco il punto di arrivo del fronte, saturando virtualmente il profilo sottostante con la quantità di Imax
            // Tengo conto degli Imax  e i flussi già passati degli strati sottostanti prendendo il minimo
            // Ogni passo tolgo la parte che va a saturare lo strato
            if (l == (myCase->nrLayers-1))
                reached = l;
            else
            {
                distrH2O = myCase->layer[l+1].maxInfiltration;
                i = l+1;
                while ((i < myCase->nrLayers-1) && (distrH2O > 0.0))
                {
                    distrH2O -= (myCase->layer[i].SAT - myCase->layer[i].waterContent);
                    distrH2O = minValue(distrH2O, myCase->layer[i].maxInfiltration);
                    if (distrH2O > 0.0) i++;
                }
                reached = i;
            }

            if ((l == reached) && (l < (myCase->nrLayers -1)))
                    reached += 1;

            while ((reached < (myCase->nrLayers -1))
                   && (myCase->layer[reached].waterContent >= myCase->layer[reached].SAT))
                    reached += 1;

            // move water and compute fluxes
            for (i = l+1; i <= reached; i++)
            {
                // TODO tradurre
                // ridefinisco fluxLayer in base allo stato idrico dello strato sottostante
                // a CC, fluxLayer non varia, sotto CC tolgo il deficit al fluxLayer, in surplus, aggiungo il surplus al fluxLayer
                if (myCase->layer[i].waterContent > myCase->layer[i].critical)
                {
                    // this layer is in surplus
                    waterSurplus = myCase->layer[i].waterContent - myCase->layer[i].critical;
                    fluxLayer += waterSurplus;
                    myCase->layer[i].waterContent -= waterSurplus;
                }
                else
                {
                    // this layer is before critical point (FC)
                    waterDeficit = myCase->layer[i].critical - myCase->layer[i].waterContent;
                    localWater = minValue(fluxLayer, waterDeficit);
                    fluxLayer -= localWater;
                    myCase->layer[i].waterContent += localWater;
                    if (fluxLayer <= 0.0) break;
                }

                residualFlux = myCase->layer[i].maxInfiltration - myCase->layer[i].flux;
                residualFlux = maxValue(residualFlux, 0.0);

                if (residualFlux >= fluxLayer)
                {
                    // layer is in field capacity
                    myCase->layer[i].flux += fluxLayer;
                }
                else
                {
                    // passa residualFlux, varFlux=Imax e si crea un surplus (localflux)
                    localFlux = fluxLayer - residualFlux;
                    fluxLayer = residualFlux;
                    myCase->layer[i].flux += fluxLayer;

                    // surplus management (localflux)
                    if (localFlux <= (myCase->layer[i].SAT - myCase->layer[i].waterContent))
                    {
                        // available space for water in the layer
                        myCase->layer[i].waterContent += localFlux;
                        localFlux = 0;
                    }
                    else
                    {
                        // Not enough space for water, upper layers are involved
                        for (j = i; j >= l + 1; j--)
                        {
                            if (localFlux <= 0.0) break;

                            localWater = minValue(localFlux, myCase->layer[j].SAT - myCase->layer[j].waterContent);
                            if (j < i)
                                myCase->layer[j].flux -= localFlux;

                            localFlux -= localWater;
                            myCase->layer[j].waterContent += localWater;
                        }

                        // residual water
                        if ((localFlux > 0.0) && (j == l))
                        {
                            myCase->layer[l].waterContent += localFlux;
                            myCase->layer[l].flux -= localFlux;
                        }
                    }
                }
            }
            // end cycle l+1-->reached layer

            // drainage
            if ((reached == myCase->nrLayers-1) && (fluxLayer > 0))
            {
                myCase->output.dailyDrainage += fluxLayer;
                fluxLayer = 0;
            }

            // surpuls distribution (saturated layers)
            i = reached;
            while ((fluxLayer > 0) && (i >= l+1))
            {
                localWater = minValue(fluxLayer, myCase->layer[i].SAT - myCase->layer[i].waterContent);
                fluxLayer -= localWater;
                myCase->layer[i].flux -= localWater;
                myCase->layer[i].waterContent += localWater;
                i--;
            }

            // first layer (pond on surface)
            if ((fluxLayer != 0) && (i == l))
            {
                myCase->layer[l].waterContent += fluxLayer;
                myCase->layer[l].flux -= fluxLayer;
            }
        }
        // end if surplus layer
    }

    return true;
}


bool computeRunoff(Criteria1D* myCase)
{
    // initialize runoff
    for (int i = 0; i< myCase->nrLayers; i++)
    {
        myCase->layer[i].runoff = 0.0;
    }

    if (! surfaceRunoff(myCase)) return false;

    if (! subSurfaceRunoff(myCase)) return false;

    return true;
}


bool surfaceRunoff(Criteria1D* myCase)
{
    double clodHeight;           // [mm] effective height of clod
    double roughness;            // [mm]

    if (isPluriannual(myCase->myCrop.type))
        clodHeight = 0.0;
    else
    {
        // TODO: algoritmo operazioni colturali, zolla, pendenza
        clodHeight = 5.0;
    }

    roughness = myCase->myCrop.maxSurfacePuddle + clodHeight;

    if (myCase->layer[0].waterContent > roughness)
    {
       myCase->layer[0].runoff = myCase->layer[0].waterContent - roughness;
       myCase->layer[0].waterContent = roughness;
    }
    else
       myCase->layer[0].runoff = 0.0;

    // [mm] daily surface runoff
    myCase->output.dailySurfaceRunoff = myCase->layer[0].runoff;

    return true;
}


//Driessen 1986, eq 58 (p.113)
bool subSurfaceRunoff(Criteria1D* myCase)
{
    double waterSurplus;                  // [mm]
    double satFactor;                     // [-]
    double maxRunoff;                     // [mm]
    double hydrHead;                      // [m]
    const double drainRadius = 0.25;      // [m]
    const double drainDepth = 1.0;        // [m]
    const double fieldWidth = 100.0;      // [m]

    for (int l = 1; l < myCase->nrLayers; l++)
    {
        if (myCase->layer[l].depth > drainDepth) break;

        waterSurplus = myCase->layer[l].waterContent - myCase->layer[l].critical;
        if (waterSurplus > 0.0)
        {
            satFactor = waterSurplus / (myCase->layer[l].SAT - myCase->layer[l].critical);

            hydrHead = satFactor * (drainDepth - myCase->layer[l].depth);

            maxRunoff =  10 * myCase->layer[l].horizon->Driessen.k0 * hydrHead /
                    (hydrHead + (fieldWidth / PI) * log(fieldWidth / (PI * drainRadius)));

            myCase->layer[l].runoff = minValue(waterSurplus, maxRunoff);

            myCase->layer[l].waterContent -= myCase->layer[l].runoff;

            // daily sub-surface runoff [mm]
            myCase->output.dailySubsurfaceRunoff += myCase->layer[l].runoff;
        }
    }

    return true;
}


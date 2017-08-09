#include <math.h>

#include "commonConstants.h"
#include "water1D.h"
#include "Criteria1D.h"
#include "crop.h"
#include "soil.h"


/*!
 * \brief
 * \param myLayer
 * \param availableWater   fraction of water between wilting point and field capacity [-]
 * \return  water content in the layer [mm]
 */
double getWaterContent(soil::Crit3DLayer *myLayer, double availableWater)
{
    if (availableWater < 0)
        return (myLayer->WP);
    else if (availableWater > 1)
        return (myLayer->FC);
    else
        return (myLayer->WP + availableWater * (myLayer->FC - myLayer->WP));
}


/*!
 * \brief
 * Assign two different initial available water
 * in the ploughed soil layer
 * and in the deep soil
 */
// TODO migliorare - variare in base al mese come in Vintage
void initializeWater(Criteria1D* myCase)
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


// TODO extend to geometric layers
/*!
 * \brief Heuristic algorithm for 1D soil water infiltration and redistribution
 * \param myCase
 * \param myError
 * \param prec      [mm]
 * \param surfaceIrrigation [mm]
 * \author Margot van Soetendael
 * \note P.M.Driessen, 1986, "The water balance of soil"
 */
bool infiltration(Criteria1D* myCase, std::string* myError, float prec, float surfaceIrrigation)
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

    // max infiltration due to gravitational force and permeability (Driessen, 1986)
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
        // find layer in water surplus
        if (myCase->layer[l].waterContent > myCase->layer[l].critical)
        {
            fluxLayer = minValue(myCase->layer[l].maxInfiltration, myCase->layer[l].waterContent - myCase->layer[l].critical);
            myCase->layer[l].flux += fluxLayer;
            myCase->layer[l].waterContent -= fluxLayer;

            // TODO translate
            // cerca il punto di arrivo del fronte
            // saturando virtualmente il profilo sottostante con la quantità Imax
            // tiene conto degli Imax  e dei flussi già passati dagli strati sottostanti prendendo il minimo
            // ogni passo toglie la parte che va a saturare lo strato
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
                // TODO translate
                // ridefinisce fluxLayer in base allo stato idrico dello strato sottostante
                // sotto Field Capacity tolgo il deficit al fluxLayer,
                // in water surplus, aggiungo il surplus al fluxLayer
                if (myCase->layer[i].waterContent > myCase->layer[i].critical)
                {
                    // layer in water surplus (critical point: usually is FC)
                    waterSurplus = myCase->layer[i].waterContent - myCase->layer[i].critical;
                    fluxLayer += waterSurplus;
                    myCase->layer[i].waterContent -= waterSurplus;
                }
                else
                {
                    // layer before critical point
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
                    myCase->layer[i].flux += fluxLayer;
                }
                else
                {
                    // si crea un surplus (localflux)
                    localFlux = fluxLayer - residualFlux;
                    fluxLayer = residualFlux;
                    myCase->layer[i].flux += fluxLayer;

                    // surplus management
                    if (localFlux <= (myCase->layer[i].SAT - myCase->layer[i].waterContent))
                    {
                        // available space for water in the layer
                        myCase->layer[i].waterContent += localFlux;
                        localFlux = 0;
                    }
                    else
                    {
                        // not enough space for water, upper layers are involved
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
            } // end cycle l+1-->reached layer

            // drainage
            if ((reached == myCase->nrLayers-1) && (fluxLayer > 0))
            {
                myCase->output.dailyDrainage += fluxLayer;
                fluxLayer = 0;
            }

            // surplus distribution (saturated layers)
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

        }  // end if surplus layer
    }

    return true;
}


/*!
 * \brief compute capillary rise due to watertable
 * \param myCase
 * \param waterTableDepth [m]
 * \return
 */
bool capillaryRise(Criteria1D* myCase, float waterTableDepth)
{
    float psi, previousPsi;             // [kPa] water potential
    float he;                           // [kPa] air entry point boundary layer
    float dz;                           // [m]

    float dPsi;                         // [cm]  ??

    float k_psi;                        // [cm/d] water conductivity
    double capillaryRiseSum = 0.0;      // [mm]

    // wrong watertable
    if (waterTableDepth == NODATA || waterTableDepth <= 0)
        return false;

    // watertable too depth: no effect
    int lastLayer = myCase->nrLayers - 1;
    if (waterTableDepth > (myCase->layer[lastLayer].depth * 3))
        return false;

    // search boundary layer: first layer over watertable
    // depth is assigned at center of each layer
    int i = lastLayer;
    if (waterTableDepth < myCase->mySoil.totalDepth)
        while ((i > 1) && (waterTableDepth <= myCase->layer[i].depth))
               i--;

    int boundaryLayer = i;

    // layers below watertable: saturated
    if (boundaryLayer < lastLayer)
        for (i = boundaryLayer + 1; i <= lastLayer; i++)
        {
            myCase->layer[i].critical = myCase->layer[i].SAT;

            if (myCase->layer[i].waterContent < myCase->layer[i].SAT)
            {
                capillaryRiseSum += (myCase->layer[i].SAT - myCase->layer[i].waterContent);
                myCase->layer[i].waterContent = myCase->layer[i].SAT;
            }
        }

    // air entry point of boundary layer [kPa]
    he = myCase->layer[boundaryLayer].horizon->vanGenuchten.he;

    // layer above watertabel: compute water content threshold for dreinage
    for (i = 1; i <= boundaryLayer; i++)
    {
        dz = (waterTableDepth - myCase->layer[i].depth);    // [m]
        psi = soil::metersTokPa(dz) + he;                   // [kPa]
        myCase->layer[i].critical = soil::getWaterContent(psi, &(myCase->layer[i]));    // [mm]

        if (myCase->layer[i].critical > myCase->layer[i].FC)
            myCase->layer[i].critical = myCase->layer[i].FC;
    }

    return true;
}



/*!
 * \brief computeRunoff
 * \param myCase
 */
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


/*!
 * \brief compute surface runoff
 * \param myCase
 * \return
 */
bool surfaceRunoff(Criteria1D* myCase)
{
    double clodHeight;           // [mm] effective height of clod
    double roughness;            // [mm]

    // height of clod
    // very rough solution for taking into account tillage and others operations
    if (isPluriannual(myCase->myCrop.type))
        clodHeight = 0.0;
    else
        clodHeight = 5.0;

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


/*!
 * \brief
 * Compute lateral drainage
 * \note P.M.Driessen, 1986, eq.58
 */
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


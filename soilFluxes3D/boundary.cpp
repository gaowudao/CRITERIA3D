/*!

    CRITERIA 3D
    \copyright (C) 2011 Fausto Tomei, Gabriele Antolini, Alberto Pistocchi,
    Antonio Volta, Giulia Villani, Marco Bittelli

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
    along with CRITERIA3D.  If not, see <http://www.gnu.org/licenses/>

    contacts:
    ftomei@arpa.emr.it
    fausto.tomei@gmail.com
    gantolini@arpa.emr.it
    alberto.pistocchi@gecosistema.it
    marco.bittelli@unibo.it
*/

#include <stdio.h>
#include <math.h>

#include "physics.h"
#include "header/types.h"
#include "header/solver.h"
#include "header/soilPhysics.h"
#include "header/boundary.h"
#include "header/soilFluxes3D.h"
#include "header/water.h"
#include "header/heat.h"

#include <iostream>

void initializeBoundary(Tboundary *myBoundary, int myType, float slope)
{
	(*myBoundary).type = myType;
	(*myBoundary).slope = slope;
    (*myBoundary).waterFlow = 0.;
    (*myBoundary).sumBoundaryWaterFlow = 0;
	(*myBoundary).prescribedTotalPotential = NODATA;
    (*myBoundary).advectiveHeatFlux = 0.;
    (*myBoundary).fixedTemperature = NODATA;

    if (myStructure.computeHeat)
    {
        (*myBoundary).Heat = new(TboundaryHeat);

        //surface characteristics
        (*myBoundary).Heat->heightWind = NODATA;
        (*myBoundary).Heat->heightTemperature = NODATA;
        (*myBoundary).Heat->roughnessHeight = NODATA;
        (*myBoundary).Heat->aerodynamicConductance = NODATA;
        (*myBoundary).Heat->soilConductance = NODATA;
        (*myBoundary).Heat->albedo = NODATA;

        //atmospheric variables
        (*myBoundary).Heat->temperature = NODATA;
        (*myBoundary).Heat->relativeHumidity = NODATA;
        (*myBoundary).Heat->vaporConcentration = NODATA;
        (*myBoundary).Heat->windSpeed = NODATA;
        (*myBoundary).Heat->globalIrradiance = NODATA;
        (*myBoundary).Heat->netIrradiance = NODATA;

        //surface energy fluxes
        (*myBoundary).Heat->radiativeFlux = 0;
        (*myBoundary).Heat->latentFlux = 0;
        (*myBoundary).Heat->sensibleFlux = 0;
    }
    else (*myBoundary).Heat = NULL;
}

double computeSoilSurfaceResistance(double myThetaTop)
{	// soil surface resistance (s m-1)
    // Van De Griend and Owe (1994)
    const double THETAMIN = 0.15;
    return (10 * exp(0.3563 * (THETAMIN - myThetaTop) * 100));
}

double computeSoilSurfaceResistanceCG(double theta, double thetaSat)
{	// soil surface resistance (s m-1)
    // Camillo and Gurney (1986)
    return (-805 + 4140 * (thetaSat - theta));
}

/*!
 * \brief atmospheric sensible heat flux
 * \param i
 * \return latent heat (W m-2)
 */
double computeAtmosphericSensibleFlow(long i)
{
    if (myNode[i].boundary->Heat == NULL || ! myNode[myNode[i].up.index].isSurface)
        return 0;

    double myPressure = PressureFromAltitude(myNode[i].z);

    double myDeltaT = myNode[i].boundary->Heat->temperature - myNode[i].extra->Heat->T;

    double myCvAir = AirVolumetricSpecificHeat(myPressure, myNode[i].boundary->Heat->temperature);

    return (myCvAir * myDeltaT * myNode[i].boundary->Heat->aerodynamicConductance);
}

/*!
 * \brief atmospheric latent flux (evaporation/condensation)
 * \param i
 * \return latent flux (m3 m-2 s-1)
 */
double computeAtmosphericLatentFlux(long i)
{
    if (myNode[i].boundary->Heat == NULL || ! myNode[myNode[i].up.index].isSurface)
        return 0;

    // kg m-3
    double myDeltaVapor = myNode[i].boundary->Heat->vaporConcentration - soilFluxes3D::getNodeVapor(i);

    // m s-1
    double myTotalConductance = 1./((1./myNode[i].boundary->Heat->aerodynamicConductance) + (1. / myNode[i].boundary->Heat->soilConductance));

    // kg m-2 s-1
    double myVaporFlow = myDeltaVapor * myTotalConductance;

    // m3 m-2 s-1
    double myLatentFlux = myVaporFlow / WATER_DENSITY;

    return myLatentFlux;
}


/*!
 * \brief atmospheric latent heat flux (evaporation/condensation)
 * \param i
 * \return latent flux (W m-2)
 */
double computeAtmosphericLatentHeatFlow(long i)
{
    if (myNode[i].boundary->Heat == NULL || ! myNode[myNode[i].up.index].isSurface)
        return 0;

    double latentHeatFlow = 0.;

    // J kg-1
    double lambda = LatentHeatVaporization(myNode[i].extra->Heat->T - ZEROCELSIUS);
    // waterFlow = vapor sink source
    latentHeatFlow = lambda * myNode[i].boundary->waterFlow * WATER_DENSITY;

    return latentHeatFlow;
}

double getSurfaceWaterFraction(int i)
{
    if (! myNode[i].isSurface)
        return 0.0;
    else
    {
        double h = maxValue(myNode[i].H - myNode[i].z, 0.0);
        return 1.0 - maxValue(0.0, myNode[i].Soil->Pond - h) / myNode[i].Soil->Pond;
    }
}

void updateBoundary()
{
    for (long i = 0; i < myStructure.nrNodes; i++)
        if (myNode[i].boundary != NULL)
            if (myStructure.computeHeat)
                if (myNode[i].extra->Heat != NULL)
                    if (myNode[i].boundary->type == BOUNDARY_HEAT)
                    {
                        // update aerodynamic conductance
                        myNode[i].boundary->Heat->aerodynamicConductance =
                                AerodynamicConductance(myNode[i].boundary->Heat->heightTemperature,
                                    myNode[i].boundary->Heat->heightWind,
                                    myNode[i].extra->Heat->T,
                                    myNode[i].boundary->Heat->roughnessHeight,
                                    myNode[i].boundary->Heat->temperature,
                                    myNode[i].boundary->Heat->windSpeed);

                        if (myStructure.computeWater)
                            // update soil surface conductance
                        {
                            double theta = theta_from_sign_Psi(myNode[i].H - myNode[i].z, i);
                            myNode[i].boundary->Heat->soilConductance = 1./ computeSoilSurfaceResistance(theta);
                        }
                    }
}


void updateBoundaryWater(double deltaT)
{
    double boundaryPsi, boundarySe, boundaryK, meanK;
    double const EPSILON_mm = 0.0001;          //0.1 mm
    double area, boundarySide, boundaryArea, Hs, avgH, maxFlow, flow;

    for (long i = 0; i < myStructure.nrNodes; i++)
    {
        // extern sink/source
        myNode[i].Qw = myNode[i].waterSinkSource;

        if (myNode[i].boundary != NULL)
        {
            // initialize
            myNode[i].boundary->waterFlow = 0.;
            if (myNode[i].boundary->type == BOUNDARY_RUNOFF)
            {
                // current surface water available to runoff [m]
                avgH = (myNode[i].H + myNode[i].oldH) * 0.5;
                Hs = maxValue(avgH - (myNode[i].z + myNode[i].Soil->Pond), 0.0);
                if (Hs > EPSILON_mm)
                {
                    area = myNode[i].volume_area;       //  [m^2] (surface)
                    boundarySide = sqrt(area);          //  [m] approximation: side = sqrt(area)
                    maxFlow = (Hs * area) / deltaT;     //  [m^3 s^-1] max available flow in time step
                    boundaryArea = boundarySide * Hs;   //  [m^2]
                    // [m^3 s^-1] Manning
                    flow = boundaryArea *(pow(Hs, (2./3.)) / myNode[i].Soil->Roughness) * sqrt(myNode[i].boundary->slope);
                    myNode[i].boundary->waterFlow = -minValue(flow, maxFlow);
                }
            }
            else if (myNode[i].boundary->type == BOUNDARY_FREEDRAINAGE)
            {
                // [m^3 s^-1] Darcy unit gradient
                // dH=dz=L  ->  q=K(h)
                double myFlux = -myNode[i].k * myNode[i].up.area;
                myNode[i].boundary->waterFlow = myFlux;               
            }

            else if (myNode[i].boundary->type == BOUNDARY_FREELATERALDRAINAGE)
            {
                // TODO approximation: boundary area equal to other lateral link
				area = myNode[i].lateral[0].area;
                // [m^3 s^-1] Darcy,  gradient = slope (dH=dz)
                myNode[i].boundary->waterFlow = -myNode[i].k * area * myNode[i].boundary->slope
                                            * myParameters.k_lateral_vertical_ratio;
            }

            else if (myNode[i].boundary->type == BOUNDARY_PRESCRIBEDTOTALPOTENTIAL)
            {
                if (myNode[i].boundary->prescribedTotalPotential >= myNode[i].z)
                    boundaryK = myNode[i].Soil->K_sat;
                else
                {
                    boundaryPsi = fabs(myNode[i].boundary->prescribedTotalPotential - myNode[i].z);
                    boundarySe = computeSefromPsi(boundaryPsi, myNode[i].Soil);
                    boundaryK = computeWaterConductivity(boundarySe, myNode[i].Soil);
                }
                meanK = computeMean(myNode[i].k, boundaryK);
                myNode[i].boundary->waterFlow = meanK * (myNode[i].boundary->prescribedTotalPotential - myNode[i].H) * myNode[i].up.area;
            }

            else if (myNode[i].boundary->type == BOUNDARY_HEAT)
            {
                if (myStructure.computeHeat)
                    myNode[i].boundary->waterFlow = computeAtmosphericLatentFlux(i) * myNode[i].up.area;
            }

            myNode[i].Qw += myNode[i].boundary->waterFlow;
        }
    }
}


void updateBoundaryHeat()
{
    double waterFlux, advTemperature, heatFlux;

    for (long i = 0; i < myStructure.nrNodes; i++)
    {
        if (myNode[i].extra->Heat != NULL)
        {
            myNode[i].extra->Heat->Qh = myNode[i].extra->Heat->sinkSource;

            if (myNode[i].boundary != NULL)
            {
                myNode[i].boundary->advectiveHeatFlux = 0.;

                if (myNode[i].boundary->type == BOUNDARY_HEAT)
                {
                    myNode[i].boundary->Heat->sensibleFlux = 0.;
                    myNode[i].boundary->Heat->latentFlux = 0.;
                    myNode[i].boundary->Heat->radiativeFlux = 0.;

                    if (myNode[i].boundary->Heat->netIrradiance == NODATA)
                        myNode[i].boundary->Heat->radiativeFlux = myNode[i].boundary->Heat->netIrradiance;

                    myNode[i].boundary->Heat->sensibleFlux += computeAtmosphericSensibleFlow(i);

                    if (myStructure.computeWater)
                        myNode[i].boundary->Heat->latentFlux += computeAtmosphericLatentHeatFlow(i) / myNode[i].up.area;

                    // advective heat from rain or evaporation
                    if (myStructure.computeWater)
                    {
                        waterFlux = myNode[i].up.linkedExtra->heatFlux->waterFlux;
                        if (waterFlux != NODATA)
                        {
                            advTemperature = myNode[i].boundary->Heat->temperature;
                            heatFlux =  waterFlux * HEAT_CAPACITY_WATER * advTemperature / myNode[i].up.area;
                            myNode[i].boundary->advectiveHeatFlux = heatFlux;
                        }
                    }

                    myNode[i].extra->Heat->Qh += myNode[i].up.area * (myNode[i].boundary->Heat->radiativeFlux +
                                                                      myNode[i].boundary->Heat->sensibleFlux +
                                                                      myNode[i].boundary->Heat->latentFlux +
                                                                      myNode[i].boundary->advectiveHeatFlux);
                }
                else if (myNode[i].boundary->type == BOUNDARY_FREEDRAINAGE ||
                         myNode[i].boundary->type == BOUNDARY_PRESCRIBEDTOTALPOTENTIAL)
                {
                    if (myStructure.computeWater)
                    {
                         waterFlux = myNode[i].boundary->waterFlow;

                        if (waterFlux < 0)
                            advTemperature = myNode[i].extra->Heat->T;
                        else
                            advTemperature = myNode[i].boundary->fixedTemperature;

                        heatFlux =  waterFlux * HEAT_CAPACITY_WATER * advTemperature / myNode[i].up.area;
                        myNode[i].boundary->advectiveHeatFlux = heatFlux;

                        myNode[i].extra->Heat->Qh += myNode[i].up.area * myNode[i].boundary->advectiveHeatFlux;
                    }

                    if (myNode[i].boundary->fixedTemperature != NODATA)
                    {
                        double boundaryHeatConductivity = SoilHeatConductivity(i, myNode[i].extra->Heat->T, myNode[i].H - myNode[i].z);
                        double deltaT = myNode[i].boundary->fixedTemperature - myNode[i].extra->Heat->T;
                        double deltaZ = myNode[i-1].z - myNode[i].z;
                        myNode[i].extra->Heat->Qh += boundaryHeatConductivity * deltaT / deltaZ * myNode[i].up.area;
                    }

                }                
            }
        }
    }
}

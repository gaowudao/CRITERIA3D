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

#include "header/types.h"
#include "header/solver.h"
#include "header/soilPhysics.h"
#include "header/boundary.h"
#include "header/soilFluxes3D.h"
#include "header/water.h"


void initializeBoundary(Tboundary *myBoundary, int myType, float slope)
{
	(*myBoundary).type = myType;
	(*myBoundary).slope = slope;
    (*myBoundary).waterFlow = 0.;
    (*myBoundary).sumBoundaryWaterFlow = 0;
	(*myBoundary).prescribedTotalPotential = NODATA;
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


void updateBoundaryWater(double deltaT)
{
    double boundaryPsi, boundarySe, boundaryK, meanK;
    double const EPSILON_mm = 0.0001;          //0.1 mm
    double area, boundarySide, boundaryArea, Hs, avgH, maxFlow, flow;

    for (long i = 0; i < myStructure.nrNodes; i++)
    {
        /*! extern sink/source */
        myNode[i].Qw = myNode[i].waterSinkSource;

        if (myNode[i].boundary != NULL)
        {
            /*! initialize */
            myNode[i].boundary->waterFlow = 0.;
            if (myNode[i].boundary->type == BOUNDARY_RUNOFF)
            {
                /*! current surface water available to runoff [m] */
                avgH = (myNode[i].H + myNode[i].oldH) * 0.5;
                Hs = maxValue(avgH - (myNode[i].z + myNode[i].Soil->Pond), 0.0);
                if (Hs > EPSILON_mm)
                {
                    area = myNode[i].volume_area;       /*!<  [m^2] (surface)  */
                    boundarySide = sqrt(area);          /*!<  [m] approximation: side = sqrt(area)  */
                    maxFlow = (Hs * area) / deltaT;     /*!<  [m^3 s^-1] max available flow in time step  */
                    boundaryArea = boundarySide * Hs;   /*!<  [m^2]  */
                    /*! [m^3 s^-1] Manning */
                    flow = boundaryArea *(pow(Hs, (2./3.)) / myNode[i].Soil->Roughness) * sqrt(myNode[i].boundary->slope);
                    myNode[i].boundary->waterFlow = -minValue(flow, maxFlow);
                }
            }
            else if (myNode[i].boundary->type == BOUNDARY_FREEDRAINAGE)
            {
                /*! [m^3 s^-1] Darcy unit gradient */
                /*! dH=dz=L  ->  q=K(h) */
                myNode[i].boundary->waterFlow = -myNode[i].k * myNode[i].up.area;
            }

            else if (myNode[i].boundary->type == BOUNDARY_FREELATERALDRAINAGE)
            {
                // TODO approximation: boundary area equal to other lateral link
				area = myNode[i].lateral[0].area;
                /*! [m^3 s^-1] Darcy,  gradient = slope (dH=dz) */
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

            myNode[i].Qw += myNode[i].boundary->waterFlow;
        }
    }
}

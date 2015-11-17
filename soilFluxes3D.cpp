/*-----------------------------------------------------------------------------------

    CRITERIA 3D
    Copyright (C) 2011 Fausto Tomei, Gabriele Antolini, Alberto Pistocchi,
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
    along with CRITERIA3D.  If not, see <http://www.gnu.org/licenses/>.

    contacts:
    ftomei@arpa.emr.it
    fausto.tomei@gmail.com
    gantolini@arpa.emr.it
    alberto.pistocchi@gecosistema.it
    marco.bittelli@unibo.it
-----------------------------------------------------------------------------------*/

#include <stdio.h>
#include <math.h>
#include <malloc.h>

#include "header/types.h"
#include "header/memory.h"
#include "header/soilPhysics.h"
#include "header/soilFluxes3D.h"
#include "header/solver.h"
#include "header/balance.h"
#include "header/water.h"
#include "header/boundary.h"

//------------------------------------------
// global variables
//------------------------------------------
TParameters myParameters;
TCrit3DStructure myStructure;

TCrit3Dnode *myNode = NULL;
TmatrixElement **A = NULL;

double *C0 = NULL;
double *C = NULL;
double *X = NULL;
double *b = NULL;

Tsoil Soil_List[MAX_SOILS][MAX_HORIZONS];
Tsoil Surface_List[MAX_SURFACES];
//------------------------------------------


namespace soilFluxes3D {

 void DLL_EXPORT cleanMemory()
	{
        cleanNodes();
        cleanArrays();
        //clean balance
	}

 int DLL_EXPORT initialize(long nrNodes, int nrLayers, int nrLateralLinks)
{
    // clean the old data structures
    cleanMemory();

    myParameters.initialize();
    myStructure.initialize();

    myStructure.nrNodes = nrNodes;
    myStructure.nrLayers = nrLayers;
    myStructure.nrLateralLinks = nrLateralLinks;
    // max nr columns = nr. of lateral links + 2 columns for up and down link + 1 column for diagonal
    myStructure.maxNrColumns = nrLateralLinks + 2 + 1;

    // build the nodes vector
    myNode = (TCrit3Dnode *) calloc(myStructure.nrNodes, sizeof(TCrit3Dnode));
	for (long i = 0; i < myStructure.nrNodes; i++)
	{
		myNode[i].Soil = NULL;
		myNode[i].boundary = NULL;
		myNode[i].up = NULL;
		myNode[i].down = NULL;
		if  (myStructure.nrLateralLinks == 0)
            myNode[i].lateral = NULL;
        else
            {
            myNode[i].lateral = (TlinkedNode **) calloc(myStructure.nrLateralLinks, sizeof(TlinkedNode *));
            for (short l = 0; l < myStructure.nrLateralLinks; l++) myNode[i].lateral[l] = NULL;
            }
    }

    // build the matrix
    if (myNode == NULL)
        {return(MEMORY_ERROR);}
    else
		{return(initializeArrays());}
 }


 int DLL_EXPORT setNumericalParameters(float minDeltaT, float maxDeltaT, int maxIterationNumber,
                        int maxApproximationsNumber, int ResidualTolerance, float MBRThreshold)
 {
	 //-----------------------------------------------------------------------------
     // Set numerical solution parameters
	 //-----------------------------------------------------------------------------

        if (minDeltaT < 0.1) minDeltaT = 0.1;
        if (minDeltaT > 3600) minDeltaT = 3600;
        myParameters.delta_t_min = minDeltaT;

        if (maxDeltaT < 60) maxDeltaT = 60;
        if (maxDeltaT > 3600) maxDeltaT = 3600;
        if (maxDeltaT < minDeltaT) maxDeltaT = minDeltaT;
        myParameters.delta_t_max = maxDeltaT;

        myParameters.current_delta_t = myParameters.delta_t_max;

        if (maxIterationNumber < 10) maxIterationNumber = 10;
        if (maxIterationNumber > MAX_NUMBER_ITERATIONS) maxIterationNumber = MAX_NUMBER_ITERATIONS;
        myParameters.iterazioni_max = maxIterationNumber;

        if (maxApproximationsNumber < 1) maxApproximationsNumber = 1;

		if (maxApproximationsNumber > MAX_NUMBER_APPROXIMATIONS)
				maxApproximationsNumber = MAX_NUMBER_APPROXIMATIONS;

        myParameters.maxApproximationsNumber = maxApproximationsNumber;

        if (ResidualTolerance < 2) ResidualTolerance = 2;
        if (ResidualTolerance > 20) ResidualTolerance = 20;
        myParameters.ResidualTolerance = pow(double(10.), -ResidualTolerance);

        if (MBRThreshold < 1.) MBRThreshold = 1.;
        if (MBRThreshold > 10.) MBRThreshold = 10.;
        myParameters.MBRThreshold = pow(double(10.), double(-MBRThreshold));

        return(CRIT3D_OK);
 }


 int DLL_EXPORT setHydraulicProperties(int waterRetentionCurve,
                        int conductivityMeanType, float horizVertRatioConductivity)
 {
	 //-----------------------------------------------------------------------------
	 // Set hydraulic properties
	 //-----------------------------------------------------------------------------
	 // default values:
     // waterRetentionCurve = MODIFIED_VANGENUCHTEN
     // conductivityMean = MEAN_LOG
	 // k_lateral_vertical_ratio = 10
	 //-----------------------------------------------------------------------------

	myParameters.waterRetentionCurve = waterRetentionCurve;

    myParameters.conductivityMean = conductivityMeanType;

	if  ((horizVertRatioConductivity >= 1) && (horizVertRatioConductivity <= 100))
    {
        myParameters.k_lateral_vertical_ratio = horizVertRatioConductivity;
        return(CRIT3D_OK);
    }
	else
	{
	    myParameters.k_lateral_vertical_ratio = 10.;
	    return(PARAMETER_ERROR);
	}
 }

 int DLL_EXPORT setNode(long myIndex, float x, float y, float z, double volume_or_area, bool isSurface,
                        bool isBoundary, int boundaryType, float slope)
 {
	//-------------------------------------------------------------------------------------------
	// Set node position and properties
	//-------------------------------------------------------------------------------------------

    if (myNode == NULL) return(MEMORY_ERROR);
    if ((myIndex < 0) || (myIndex >= myStructure.nrNodes)) return(INDEX_ERROR);

	if (isBoundary)
	{
		myNode[myIndex].boundary = new(Tboundary);
		initializeBoundary(myNode[myIndex].boundary, boundaryType, slope);
	}

    myNode[myIndex].x = x;
    myNode[myIndex].y = y;
    myNode[myIndex].z = z;
    myNode[myIndex].volume_area = volume_or_area;   // area on surface elements, volume on sub-surface

	myNode[myIndex].isSurface = isSurface;

    return(CRIT3D_OK);
 }


 int DLL_EXPORT setNodeLink(long n, long linkIndex, int direction, float interfaceArea)
 {
    // error check
    if (myNode == NULL) return(MEMORY_ERROR);

    if ((n < 0) || (n >= myStructure.nrNodes) || (linkIndex < 0) || (linkIndex >= myStructure.nrNodes))
        return(INDEX_ERROR);

    short j;
    switch (direction)
    {
        case UP :
                    myNode[n].up  = (TlinkedNode *) calloc(1, sizeof(TlinkedNode));
                    myNode[n].up->index = linkIndex;
                    myNode[n].up->area = interfaceArea;
                    break;
        case DOWN :
                    myNode[n].down  = (TlinkedNode *) calloc(1, sizeof(TlinkedNode));
                    myNode[n].down->index = linkIndex;
                    myNode[n].down->area = interfaceArea;
                    break;
        case LATERAL :
                    j = 0;
                    while ((j < myStructure.nrLateralLinks) && (myNode[n].lateral[j] != NULL)) j++;
                    if (j == myStructure.nrLateralLinks) return (TOPOGRAPHY_ERROR);
                    myNode[n].lateral[j]  = (TlinkedNode *) calloc(1, sizeof(TlinkedNode));
                    myNode[n].lateral[j]->index = linkIndex;
                    myNode[n].lateral[j]->area = interfaceArea;
                    break;
        default :
                    return(PARAMETER_ERROR);
    }
    return(CRIT3D_OK);
 }


 // Assign surface index to node
 int DLL_EXPORT setNodeSurface(long nodeIndex, int surfaceIndex)
 {
	if (myNode == NULL) return(MEMORY_ERROR);
    if ((nodeIndex < 0) || (! myNode[nodeIndex].isSurface)) return(INDEX_ERROR);
    if ((surfaceIndex < 0) || (surfaceIndex >= MAX_SURFACES)) return(PARAMETER_ERROR);

    myNode[nodeIndex].Soil = &Surface_List[surfaceIndex];

    return(CRIT3D_OK);
 }


 // Assign soil to node
 int DLL_EXPORT setNodeSoil(long nodeIndex, int soilIndex, int horizonIndex)
 {
	if (myNode == NULL) return(MEMORY_ERROR);
    if ((nodeIndex < 0) || (nodeIndex >= myStructure.nrNodes)) return(INDEX_ERROR);
    if ((soilIndex < 0) || (soilIndex >= MAX_SOILS)) return(PARAMETER_ERROR);
    if ((horizonIndex < 0) || (horizonIndex >= MAX_HORIZONS)) return(PARAMETER_ERROR);

    myNode[nodeIndex].Soil = &Soil_List[soilIndex][horizonIndex];

    return(CRIT3D_OK);
 }


 int DLL_EXPORT setSoilProperties(int nSoil, int nHorizon, double VG_alpha, double VG_n, double VG_m,
                        double VG_he, double ThetaR, double ThetaS, double Ksat, double L)
 {
	//----------------------------------------------------------------------------------------------
	// Set soil myParameters
    //----------------------------------------------------------------------------------------------
    // VG_alpha             [m^-1]      Van Genutchen alpha parameter
    //                                  (warning! usually cm^-1 in literature)
	// N                    [-]         n Van Genutchen  ]1, 10]
    // Theta_r              [m^3/m^3]   residual water content
    // Theta_s              [m^3/m^3]   saturated water content
    // Ksat                 [m/s]       saturated hydraulic conductivity
    // L                    [-]         tortuosity (Mualem formula)
	//----------------------------------------------------------------------------------------------

	if ((nSoil < 0) || (nSoil >= MAX_SOILS)) return(INDEX_ERROR);

    if ((nHorizon < 0) || (nHorizon >= MAX_HORIZONS)) return(INDEX_ERROR);

    if ((VG_alpha <= 0.) || (ThetaR < 0.) || (ThetaR >= 1.)
    || (ThetaS <= 0.) || (ThetaS > 1.) || (ThetaR > ThetaS))
        return(PARAMETER_ERROR);

    Soil_List[nSoil][nHorizon].VG_alpha  = VG_alpha;
    Soil_List[nSoil][nHorizon].VG_n  = VG_n;
	Soil_List[nSoil][nHorizon].VG_m =  VG_m;

    Soil_List[nSoil][nHorizon].VG_he = VG_he;
    Soil_List[nSoil][nHorizon].VG_Sc = pow(1. + pow(VG_alpha * VG_he, VG_n),-VG_m);

    Soil_List[nSoil][nHorizon].Theta_r = ThetaR;
    Soil_List[nSoil][nHorizon].Theta_s = ThetaS;
    Soil_List[nSoil][nHorizon].K_sat = Ksat;
    Soil_List[nSoil][nHorizon].Mualem_L = L;

    return(CRIT3D_OK);
 }


 int DLL_EXPORT setSurfaceProperties(int surfaceIndex, double roughness, double surfacePond)
 {
    if ((surfaceIndex < 0) || (surfaceIndex >= MAX_SURFACES)) return(INDEX_ERROR);
    if ((roughness < 0.) || (surfacePond < 0.)) return(PARAMETER_ERROR);

    Surface_List[surfaceIndex].Roughness = roughness;
    Surface_List[surfaceIndex].Pond = surfacePond;

    return(CRIT3D_OK);
 }


 int DLL_EXPORT setMatricPotential(long nodeIndex, double potential)
 {
 //----------------------------------------------------------------------------------------------
 // Set current matric potential
 //----- Input ----------------------------------------------------------------------------------
 // myPotential              [m]
 //----------------------------------------------------------------------------------------------

     if (myNode == NULL)
         return(MEMORY_ERROR);
     if ((nodeIndex < 0) || (nodeIndex >= myStructure.nrNodes))
         return(INDEX_ERROR);

     myNode[nodeIndex].H = potential + myNode[nodeIndex].z;
     myNode[nodeIndex].oldH = myNode[nodeIndex].H;

     if (myNode[nodeIndex].isSurface)
     {
         myNode[nodeIndex].Se = 1.;
         myNode[nodeIndex].k = NODATA;
     }
     else
     {
         myNode[nodeIndex].Se = computeSe(nodeIndex);
         myNode[nodeIndex].k = computeK(nodeIndex);
     }

     return(CRIT3D_OK);
 }


 int DLL_EXPORT setTotalPotential(long nodeIndex, double totalPotential)
 {
	 //----------------------------------------------------------------------------------------------
     // Set current total potential
	 //----- Input ----------------------------------------------------------------------------------
     // totalPotential              [m]
	 //----------------------------------------------------------------------------------------------

	 if (myNode == NULL)
		 return(MEMORY_ERROR);

	 if ((nodeIndex < 0) || (nodeIndex >= myStructure.nrNodes))
		 return(INDEX_ERROR);

     myNode[nodeIndex].H = totalPotential;
	 myNode[nodeIndex].oldH = myNode[nodeIndex].H;

	 if (myNode[nodeIndex].isSurface)
	 {
		 myNode[nodeIndex].Se = 1.;
		 myNode[nodeIndex].k = NODATA;
	 }
	 else
	 {
         myNode[nodeIndex].Se = computeSe(nodeIndex);
         myNode[nodeIndex].k = computeK(nodeIndex);
	 }

	 return(CRIT3D_OK);
 }


 int DLL_EXPORT setWaterContent(long nodeIndex, double waterContent)
 {
	//----------------------------------------------------------------------------------------------
    // Set current volumetric water content
	//----- Input ----------------------------------------------------------------------------------
    // waterContent              [m^3 m^-3]
	//----------------------------------------------------------------------------------------------

    if (myNode == NULL) return(MEMORY_ERROR);

    if ((nodeIndex < 0) || (nodeIndex >= myStructure.nrNodes)) return(INDEX_ERROR);

    if (waterContent < 0.) return(PARAMETER_ERROR);

    if (myNode[nodeIndex].isSurface)
            {
			//surface
            myNode[nodeIndex].H = myNode[nodeIndex].z + waterContent;
            myNode[nodeIndex].oldH = myNode[nodeIndex].H;
            myNode[nodeIndex].Se = 1.;
            myNode[nodeIndex].k = 0.;
            }
    else
            {
            if (waterContent > 1.0) return(PARAMETER_ERROR);
            myNode[nodeIndex].Se = Se_from_theta(nodeIndex, waterContent);
            myNode[nodeIndex].H = myNode[nodeIndex].z - psi_from_Se(nodeIndex);
            myNode[nodeIndex].oldH = myNode[nodeIndex].H;
            myNode[nodeIndex].k = computeK(nodeIndex);
            }

    return(CRIT3D_OK);
 }


 int DLL_EXPORT setWaterSinkSource(long nodeIndex, double waterSinkSource)
 {
    //----------------------------------------------------------------------------------------------
    // Set current water sink/source
    //----- Input ----------------------------------------------------------------------------------
    // waterSinkSource   [m^3/sec] flow
    //----------------------------------------------------------------------------------------------

    if (myNode == NULL) return(MEMORY_ERROR);
    if ((nodeIndex < 0) || (nodeIndex >= myStructure.nrNodes)) return(INDEX_ERROR);

    myNode[nodeIndex].waterSinkSource = waterSinkSource;

    return(CRIT3D_OK);
 }


 int DLL_EXPORT setPrescribedTotalPotential(long nodeIndex, double prescribedTotalPotential)
 {
	 //----------------------------------------------------------------------------------------------
	 // Set prescribed Total Potential
	 //----- Input ----------------------------------------------------------------------------------
     // prescribedTotalPotential   [m]
	 //----------------------------------------------------------------------------------------------
    if (myNode == NULL) return(MEMORY_ERROR);
    if ((nodeIndex < 0) || (nodeIndex >= myStructure.nrNodes)) return(INDEX_ERROR);
    if (myNode[nodeIndex].boundary == NULL) return(BOUNDARY_ERROR);
	if (myNode[nodeIndex].boundary->type != BOUNDARY_PRESCRIBEDTOTALPOTENTIAL) return(BOUNDARY_ERROR);

    myNode[nodeIndex].boundary->prescribedTotalPotential = prescribedTotalPotential;

    return(CRIT3D_OK);
 }


 double DLL_EXPORT getWaterContent(long nodeIndex)
 //-----------------------------------------------------------------------------
 // return water content
 //----- Output ----------------------------------------------------------------
 // surface:							 [m] surface water height
 // sub-surface							 [m^3 m^-3] volumetric water content
 //-----------------------------------------------------------------------------
 {
        if (myNode == NULL) return(MEMORY_ERROR);
        if ((nodeIndex < 0) || (nodeIndex >= myStructure.nrNodes)) return(INDEX_ERROR);

        if  (myNode[nodeIndex].isSurface)
			//surface
            return (myNode[nodeIndex].H - myNode[nodeIndex].z);
        else
			//sub-surface
            return (theta_from_Se(nodeIndex));
 }


 double DLL_EXPORT getAvailableWaterContent(long index)
 //-----------------------------------------------------------------------------
 // return available water content (over wilting point)
 //----- Output ----------------------------------------------------------------
 // surface:							 [m] 0-1 water presence
 // sub-surface							 [m^3 m^-3] awc
 //-----------------------------------------------------------------------------
 {
        if (myNode == NULL) return(MEMORY_ERROR);
        if ((index < 0) || (index >= myStructure.nrNodes)) return(INDEX_ERROR);

        if  (myNode[index].isSurface)
            //surface
            return (myNode[index].H - myNode[index].z);
        else
            //sub-surface
            return maxValue(0.0, theta_from_Se(index) - theta_from_sign_Psi(-160, index));
 }


 double DLL_EXPORT getWaterDeficit(long index, double fieldCapacity)
 //-----------------------------------------------------------------------------
 // return current deficit from fieldCapacity [m]
 //----- Output ----------------------------------------------------------------
 // surface:							 0
 // sub-surface							 [m^3 m^-3]
 //-----------------------------------------------------------------------------
 {
        if (myNode == NULL) return(MEMORY_ERROR);
        if ((index < 0) || (index >= myStructure.nrNodes)) return(INDEX_ERROR);

        if  (myNode[index].isSurface)
            //surface
            return (0.0);
        else
            //sub-surface
            return (theta_from_sign_Psi(-fieldCapacity, index) - theta_from_Se(index));
 }



 double DLL_EXPORT getDegreeOfSaturation(long nodeIndex)
 //-----------------------------------------------------------------------------
 // return degree of saturation (normalized water content)
 //----- Output ----------------------------------------------------------------
 // surface:							 [-] water presence 0-100
 // sub-surface							 [%] degree of saturation
 //-----------------------------------------------------------------------------
 {
        if (myNode == NULL) return(MEMORY_ERROR);
        if ((nodeIndex < 0) || (nodeIndex >= myStructure.nrNodes)) return(INDEX_ERROR);

        if  (myNode[nodeIndex].isSurface)
        {
            if ((myNode[nodeIndex].H - myNode[nodeIndex].z) > 0.0001)
                return(100.0);
            else
                return(0.0);
        }
        else
            return (myNode[nodeIndex].Se*100.0);
 }


 double DLL_EXPORT getTotalWaterContent()
 //-----------------------------------------------------------------------------
 // return total water content          [m^3]
 //-----------------------------------------------------------------------------
 {
    return(computeTotalWaterContent());
 }


 double DLL_EXPORT getWaterConductivity(long nodeIndex)
 //------------------------------------------------------------------------
 // return hydraulic conductivity
 //----- Output -----------------------------------------------------------
 // hydraulic conductivity                [m/s]
 //------------------------------------------------------------------------
 {
    //error check
    if (myNode == NULL) return(MEMORY_ERROR);
    if ((nodeIndex < 0) || (nodeIndex >= myStructure.nrNodes)) return(INDEX_ERROR);

    return (myNode[nodeIndex].k);
 }


 double DLL_EXPORT getMatricPotential(long nodeIndex)
 //-----------------------------------------------------------------------------
 // Return matric potential (psi)           [m]
 //----- Input -----------------------------------------------------------------
 // node index                              [-]
 //-----------------------------------------------------------------------------
 {
    if (myNode == NULL) return(MEMORY_ERROR);
    if ((nodeIndex < 0) || (nodeIndex >= myStructure.nrNodes)) return(INDEX_ERROR);

    return (myNode[nodeIndex].H - myNode[nodeIndex].z);
 }


 double DLL_EXPORT getTotalPotential(long nodeIndex)
 //-----------------------------------------------------------------------------
 // Return total potential H (psi + z)      [m]
 //-----------------------------------------------------------------------------
 {
	 if (myNode == NULL) return(MEMORY_ERROR);
	 if ((nodeIndex < 0) || (nodeIndex >= myStructure.nrNodes)) return(INDEX_ERROR);

	 return (myNode[nodeIndex].H);
 }


 double DLL_EXPORT getWaterFlow(long n, short direction)
 //-------------------------------------------------------------------
 // [m^3] maximum integrated flow over the time step
 //-------------------------------------------------------------------
 {
    if (myNode == NULL) return MEMORY_ERROR;
    if ((n < 0) || (n >= myStructure.nrNodes)) return INDEX_ERROR;

    double maxFlow = 0.0;
	switch (direction) {
        case UP:
            if (myNode[n].up != NULL) return (myNode[n].up->sumFlow);
            else return INDEX_ERROR;
            break;
		case DOWN:
            if (myNode[n].down != NULL) return (myNode[n].down->sumFlow);
            else return INDEX_ERROR;
            break;
		case LATERAL:
            for (short i = 0; i < myStructure.nrLateralLinks; i++)
                if (myNode[n].lateral[i] != NULL)
                    if (fabs(myNode[n].lateral[i]->sumFlow) > maxFlow)
                        maxFlow = myNode[n].lateral[i]->sumFlow;
            return maxFlow;
            break;
        default:
            break;
        }
    return INDEX_ERROR;
 }


 double DLL_EXPORT getSumLateralWaterFlow(long n)
 //-------------------------------------------------------------------
 // [m^3] integrated flow over the time step
 //-------------------------------------------------------------------
 {
    if (myNode == NULL) return MEMORY_ERROR;
    if ((n < 0) || (n >= myStructure.nrNodes)) return INDEX_ERROR;

    double sumFlow = 0.0;
    for (short i = 0; i < myStructure.nrLateralLinks; i++)
        if (myNode[n].lateral[i] != NULL)
            sumFlow += myNode[n].lateral[i]->sumFlow;
    return sumFlow;
 }


void DLL_EXPORT initializeBalance()
{
    InitializeBalanceWater();
}


double DLL_EXPORT getWaterMBR()
 {
    return (balanceWholePeriod.MBR_Water);
 }


double DLL_EXPORT getBoundaryWaterFlow(long nodeIndex)
 {
 //-------------------------------------------------------------------
 // [m^3] integrated water flow from boundary over the time step
 //-------------------------------------------------------------------
    if (myNode == NULL) return(MEMORY_ERROR);
    if ((nodeIndex < 0) || (nodeIndex >= myStructure.nrNodes)) return(INDEX_ERROR);

	if (myNode[nodeIndex].boundary == NULL) return(BOUNDARY_ERROR);

    return(myNode[nodeIndex].boundary->sumBoundaryWaterFlow);
 }


 double DLL_EXPORT getBoundaryWaterSumFlow(int boundaryType)
 {
 //-------------------------------------------------------------------
 // [m^3] integrated water flow from boundary over the time step
 //-------------------------------------------------------------------
    double sumFlow = 0.0;

    for (long n = 0; n < myStructure.nrNodes; n++)
        if (myNode[n].boundary != NULL)
            if (myNode[n].boundary->type == boundaryType)
                sumFlow += myNode[n].boundary->sumBoundaryWaterFlow;

    return(sumFlow);
 }


void DLL_EXPORT computePeriod(float myPeriod)
// ---------------------------------------------------------------------------
// compute a period of time [s]
// ---------------------------------------------------------------------------
    {
        float deltaT, ResidualTime, mySecond = 0.0;

        balanceCurrentPeriod.sinkSourceWater = 0.;

        while (mySecond < myPeriod)
        {
            ResidualTime = myPeriod - mySecond;
            computeWater(ResidualTime, &deltaT);
            mySecond += deltaT;
        }

        updateBalanceWaterWholePeriod();
    }

}

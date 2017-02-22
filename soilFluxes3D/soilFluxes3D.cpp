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
    along with CRITERIA3D.  If not, see <http://www.gnu.org/licenses/>.

    contacts:
    ftomei@arpa.emr.it
    fausto.tomei@gmail.com
    gantolini@arpa.emr.it
    alberto.pistocchi@gecosistema.it
    marco.bittelli@unibo.it
*/

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
#include "header/heat.h"
#include "header/physics.h"
#include "header/extra.h"

/*! global variables */

TParameters myParameters;
TCrit3DStructure myStructure;

TCrit3Dnode *myNode = NULL;
TmatrixElement **A = NULL;

TgroundWater groundWater;

double *C0 = NULL;
double *C = NULL;
double *X = NULL;
double *b = NULL;

Tsoil Soil_List[MAX_SOILS][MAX_HORIZONS];
Tsoil Surface_List[MAX_SURFACES];



namespace soilFluxes3D {

	int DLL_EXPORT __STDCALL test()
	{
        return(CRIT3D_OK);
	}

	void DLL_EXPORT __STDCALL cleanMemory()
	{
        cleanNodes();
        cleanArrays();
        //clean balance
	}

    void initializeHeatFluxes(TCrit3DLinkedNodeExtra *myLinkExtra)
    {
        (*myLinkExtra).heatFlux = new(THeatFlux);
        (*myLinkExtra).heatFlux->diffusive = 0.;
        (*myLinkExtra).heatFlux->isothermLatent = 0.;
        (*myLinkExtra).heatFlux->thermLatent = 0.;
        (*myLinkExtra).heatFlux->advective = 0.;
    }

    int DLL_EXPORT __STDCALL initialize(long nrNodes, int nrLayers, int nrLateralLinks,
                                        bool computeWater_, bool computeHeat_, bool computeSolutes_,
                                        bool computeHeatLatent_, bool computeHeatAdvective_)
{
    /*! clean the old data structures */
    cleanMemory();

    myParameters.initialize();
    myStructure.initialize();   

    myStructure.computeWater = computeWater_;
    myStructure.computeHeat = computeHeat_;
    myStructure.computeSolutes = computeSolutes_;
    myStructure.computeHeatLatent = computeHeatLatent_;
    myStructure.computeHeatAdvective = computeHeatAdvective_;

    if (myStructure.computeHeatAdvective || myStructure.computeHeatLatent)
    {
        myStructure.computeHeat = true;
        myStructure.computeWater = true;
    }

    groundWater.initialize();

    myStructure.nrNodes = nrNodes;
    myStructure.nrLayers = nrLayers;
    myStructure.nrLateralLinks = nrLateralLinks;
    /*! max nr columns = nr. of lateral links + 2 columns for up and down link + 1 column for diagonal */
    myStructure.maxNrColumns = nrLateralLinks + 2 + 1;

    /*! build the nodes vector */
    myNode = (TCrit3Dnode *) calloc(myStructure.nrNodes, sizeof(TCrit3Dnode));
	for (long i = 0; i < myStructure.nrNodes; i++)
	{
		myNode[i].Soil = NULL;
		myNode[i].boundary = NULL;
		myNode[i].up.index = NOLINK;
		myNode[i].down.index = myNode[i].up.index = NOLINK;

        myNode[i].lateral = (TlinkedNode *) calloc(myStructure.nrLateralLinks, sizeof(TlinkedNode));
        for (short l = 0; l < myStructure.nrLateralLinks; l++)
        {
            myNode[i].lateral[l].index = NOLINK;
            if (myStructure.computeHeat || myStructure.computeSolutes)
                myNode[i].lateral[l].linkedExtra = new(TCrit3DLinkedNodeExtra);
        }
    }

    /*! build the matrix */
    if (myNode == NULL)
        {return(MEMORY_ERROR);}
    else
		{return(initializeArrays());}
 }

	int DLL_EXPORT __STDCALL setNumericalParameters(float minDeltaT, float maxDeltaT, int maxIterationNumber,
                        int maxApproximationsNumber, int ResidualTolerance, float MBRThreshold)
 {
     /*!
        \brief Set numerical solution parameters
     */

        if (minDeltaT < 0.1) minDeltaT = float(0.1);
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

        if (ResidualTolerance < 4) ResidualTolerance = 4;
        if (ResidualTolerance > 16) ResidualTolerance = 16;
        myParameters.ResidualTolerance = pow(double(10.), -ResidualTolerance);

        if (MBRThreshold < 1.) MBRThreshold = 1.;
        if (MBRThreshold > 6.) MBRThreshold = 6.;
        myParameters.MBRThreshold = pow(double(10.), double(-MBRThreshold));

        return(CRIT3D_OK);
 }


    /*!
     * \brief Set hydraulic properties
     *  default values:
     *  waterRetentionCurve = MODIFIED_VANGENUCHTEN
     *  meanType = MEAN_LOG
     *  k_lateral_vertical_ratio = 10
     * \param waterRetentionCurve
     * \param conductivityMeanType
     * \param horizVertRatioConductivity
     * \return OK/ERROR
     */

	int DLL_EXPORT __STDCALL setHydraulicProperties(int waterRetentionCurve,
                        int conductivityMeanType, float horizVertRatioConductivity)
 {

	myParameters.waterRetentionCurve = waterRetentionCurve;

    myParameters.meanType = conductivityMeanType;

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

    /*!
     * \brief Set node position and properties
     * \param myIndex
     * \param x
     * \param y
     * \param z
     * \param volume_or_area
     * \param isSurface
     * \param isBoundary
     * \param boundaryType
     * \param slope
     * \return
     */
	int DLL_EXPORT __STDCALL setNode(long myIndex, float x, float y, float z, double volume_or_area, bool isSurface,
                        bool isBoundary, int boundaryType, float slope)
 {


    if (myNode == NULL) return(MEMORY_ERROR);
    if ((myIndex < 0) || (myIndex >= myStructure.nrNodes)) return(INDEX_ERROR);

	if (isBoundary)
	{
		myNode[myIndex].boundary = new(Tboundary);
		initializeBoundary(myNode[myIndex].boundary, boundaryType, slope);
	}

    if (myStructure.computeHeat || myStructure.computeSolutes)
    {
        myNode[myIndex].extra = new(TCrit3DnodeExtra);
        initializeExtra(myNode[myIndex].extra, myStructure.computeHeat, myStructure.computeSolutes);
    }

    myNode[myIndex].x = x;
    myNode[myIndex].y = y;
    myNode[myIndex].z = z;
    myNode[myIndex].volume_area = volume_or_area;   /*!< area on surface elements, volume on sub-surface */

	myNode[myIndex].isSurface = isSurface;

    return(CRIT3D_OK);
 }


	int DLL_EXPORT __STDCALL setNodeLink(long n, long linkIndex, int direction, float interfaceArea)
 {
    /*! error check */
    if (myNode == NULL) return(MEMORY_ERROR);

    if ((n < 0) || (n >= myStructure.nrNodes) || (linkIndex < 0) || (linkIndex >= myStructure.nrNodes))
        return(INDEX_ERROR);

    short j;
    switch (direction)
    {
        case UP :
                    myNode[n].up.index = linkIndex;
                    myNode[n].up.area = interfaceArea;
                    myNode[n].up.sumFlow = 0;

                    if (myStructure.computeHeat || myStructure.computeSolutes)
                    {
                        myNode[n].up.linkedExtra = new(TCrit3DLinkedNodeExtra);
                        initializeLinkExtra(myNode[n].up.linkedExtra, myStructure.computeHeat, myStructure.computeSolutes);
                    }

                    break;
        case DOWN :
                    myNode[n].down.index = linkIndex;
                    myNode[n].down.area = interfaceArea;
					myNode[n].down.sumFlow = 0;

                    if (myStructure.computeHeat || myStructure.computeSolutes)
                    {
                        myNode[n].down.linkedExtra = new(TCrit3DLinkedNodeExtra);
                        initializeLinkExtra(myNode[n].down.linkedExtra, myStructure.computeHeat, myStructure.computeSolutes);
                    }

                    break;
        case LATERAL :
                    j = 0;
                    while ((j < myStructure.nrLateralLinks) && (myNode[n].lateral[j].index != NOLINK)) j++;
                    if (j == myStructure.nrLateralLinks) return (TOPOGRAPHY_ERROR);
                    myNode[n].lateral[j].index = linkIndex;
                    myNode[n].lateral[j].area = interfaceArea;
					myNode[n].lateral[j].sumFlow = 0;

                    if (myStructure.computeHeat || myStructure.computeSolutes)
                    {
                        myNode[n].lateral[j].linkedExtra = new(TCrit3DLinkedNodeExtra);
                        initializeLinkExtra(myNode[n].lateral[j].linkedExtra, myStructure.computeHeat, myStructure.computeSolutes);
                    }

                    break;
        default :
                    return(PARAMETER_ERROR);
    }
	return(CRIT3D_OK);
 }


    /*!
     * \brief Assign surface index to node
     * \param nodeIndex
     * \param surfaceIndex
     * \return
     */
	int DLL_EXPORT __STDCALL setNodeSurface(long nodeIndex, int surfaceIndex)
 {
	if (myNode == NULL) return(MEMORY_ERROR);
    if ((nodeIndex < 0) || (! myNode[nodeIndex].isSurface)) return(INDEX_ERROR);
    if ((surfaceIndex < 0) || (surfaceIndex >= MAX_SURFACES)) return(PARAMETER_ERROR);

    myNode[nodeIndex].Soil = &Surface_List[surfaceIndex];

    return(CRIT3D_OK);
 }


    /*!
     * \brief Assign soil to node
     * \param nodeIndex
     * \param soilIndex
     * \param horizonIndex
     * \return
     */
	int DLL_EXPORT __STDCALL setNodeSoil(long nodeIndex, int soilIndex, int horizonIndex)
 {
	if (myNode == NULL) return(MEMORY_ERROR);
    if ((nodeIndex < 0) || (nodeIndex >= myStructure.nrNodes)) return(INDEX_ERROR);
    if ((soilIndex < 0) || (soilIndex >= MAX_SOILS)) return(PARAMETER_ERROR);
    if ((horizonIndex < 0) || (horizonIndex >= MAX_HORIZONS)) return(PARAMETER_ERROR);

    myNode[nodeIndex].Soil = &Soil_List[soilIndex][horizonIndex];

    return(CRIT3D_OK);
 }


    /*!
     * \brief Set soil myParameters
     * \param nSoil
     * \param nHorizon
     * \param VG_alpha [m^-1]      Van Genutchen alpha parameter - (warning! usually cm^-1 in literature)
     * \param VG_n     [-]         n Van Genutchen  ]1, 10]
     * \param VG_m
     * \param VG_he
     * \param ThetaR    [m^3/m^3]   residual water content
     * \param ThetaS    [m^3/m^3]   saturated water content
     * \param Ksat      [m/s]       saturated hydraulic conductivity
     * \param L         [-]         tortuosity (Mualem formula)
     * \return OK/ERROR
     */
    int DLL_EXPORT __STDCALL setSoilProperties(int nSoil, int nHorizon, double VG_alpha, double VG_n, double VG_m,
                        double VG_he, double ThetaR, double ThetaS, double Ksat, double L, double organicMatter, double clay)
 {


	if ((nSoil < 0) || (nSoil >= MAX_SOILS)) return(INDEX_ERROR);

    if ((nHorizon < 0) || (nHorizon >= MAX_HORIZONS)) return(INDEX_ERROR);

    if ((VG_alpha <= 0.) || (ThetaR < 0.) || (ThetaR >= 1.)
    || (ThetaS <= 0.) || (ThetaS > 1.) || (ThetaR > ThetaS))
        return(PARAMETER_ERROR);

    Soil_List[nSoil][nHorizon].VG_alpha  = VG_alpha;
    Soil_List[nSoil][nHorizon].VG_n  = VG_n;
	Soil_List[nSoil][nHorizon].VG_m =  VG_m;

    Soil_List[nSoil][nHorizon].VG_he = VG_he;
    Soil_List[nSoil][nHorizon].VG_Sc = pow(1. + pow(VG_alpha * VG_he, VG_n), -VG_m);

    Soil_List[nSoil][nHorizon].Theta_r = ThetaR;
    Soil_List[nSoil][nHorizon].Theta_s = ThetaS;
    Soil_List[nSoil][nHorizon].K_sat = Ksat;
    Soil_List[nSoil][nHorizon].Mualem_L = L;

    Soil_List[nSoil][nHorizon].organicMatter = organicMatter;
    Soil_List[nSoil][nHorizon].clay = clay;

    return(CRIT3D_OK);
 }


	int DLL_EXPORT __STDCALL setSurfaceProperties(int surfaceIndex, double roughness, double surfacePond)
 {
    if ((surfaceIndex < 0) || (surfaceIndex >= MAX_SURFACES)) return(INDEX_ERROR);
    if ((roughness < 0.) || (surfacePond < 0.)) return(PARAMETER_ERROR);

    Surface_List[surfaceIndex].Roughness = roughness;
    Surface_List[surfaceIndex].Pond = surfacePond;

    return(CRIT3D_OK);
 }


    /*!
     * \brief Set current matric potential
     * \param nodeIndex
     * \param potential [m]
     * \return OK/ERROR
     */
	int DLL_EXPORT __STDCALL setMatricPotential(long nodeIndex, double potential)
 {


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


    /*!
     * \brief Set current total potential
     * \param nodeIndex
     * \param totalPotential [m]
     * \return OK/ERROR
     */
	int DLL_EXPORT __STDCALL setTotalPotential(long nodeIndex, double totalPotential)
 {

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


    /*!
     * \brief Set current volumetric water content
     * \param nodeIndex
     * \param waterContent [m^3 m^-3]
     * \return OK/ERROR
     */
	int DLL_EXPORT __STDCALL setWaterContent(long nodeIndex, double waterContent)
 {


    if (myNode == NULL) return(MEMORY_ERROR);

    if ((nodeIndex < 0) || (nodeIndex >= myStructure.nrNodes)) return(INDEX_ERROR);

    if (waterContent < 0.) return(PARAMETER_ERROR);

    if (myNode[nodeIndex].isSurface)
            {
            /*! surface */
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


    /*!
     * \brief Set current water sink/source
     * \param nodeIndex
     * \param waterSinkSource [m^3/sec] flow
     * \return OK/ERROR
     */
	int DLL_EXPORT __STDCALL setWaterSinkSource(long nodeIndex, double waterSinkSource)
 {

    if (myNode == NULL) return(MEMORY_ERROR);
    if ((nodeIndex < 0) || (nodeIndex >= myStructure.nrNodes)) return(INDEX_ERROR);

    myNode[nodeIndex].waterSinkSource = waterSinkSource;

    return(CRIT3D_OK);
 }


    /*!
     * \brief Set prescribed Total Potential
     * \param nodeIndex
     * \param prescribedTotalPotential [m]
     * \return OK/ERROR
     */
	int DLL_EXPORT __STDCALL setPrescribedTotalPotential(long nodeIndex, double prescribedTotalPotential)
 {

    if (myNode == NULL) return(MEMORY_ERROR);
    if ((nodeIndex < 0) || (nodeIndex >= myStructure.nrNodes)) return(INDEX_ERROR);
    if (myNode[nodeIndex].boundary == NULL) return(BOUNDARY_ERROR);
	if (myNode[nodeIndex].boundary->type != BOUNDARY_PRESCRIBEDTOTALPOTENTIAL) return(BOUNDARY_ERROR);

    myNode[nodeIndex].boundary->prescribedTotalPotential = prescribedTotalPotential;

    return(CRIT3D_OK);
 }


    /*!
     * \brief return water content
     * \param nodeIndex
     * \return  surface: [m] surface water height , sub-surface: [m^3 m^-3] volumetric water content
     */
	double DLL_EXPORT __STDCALL getWaterContent(long nodeIndex)

 {
        if (myNode == NULL) return(MEMORY_ERROR);
        if ((nodeIndex < 0) || (nodeIndex >= myStructure.nrNodes)) return(INDEX_ERROR);

        if  (myNode[nodeIndex].isSurface)
            /*! surface */
            return (myNode[nodeIndex].H - myNode[nodeIndex].z);
        else
            /*! sub-surface */
            return (theta_from_Se(nodeIndex));
 }


    /*!
     * \brief return available water content (over wilting point)
     * \param index
     * \return  surface: [m] 0-1 water presence, sub-surface: [m^3 m^-3] awc
     */
	double DLL_EXPORT __STDCALL getAvailableWaterContent(long index)
 {
        if (myNode == NULL) return(MEMORY_ERROR);
        if ((index < 0) || (index >= myStructure.nrNodes)) return(INDEX_ERROR);

        if  (myNode[index].isSurface)
            /*! surface */
            return (myNode[index].H - myNode[index].z);
        else
            /*! sub-surface */
            return maxValue(0.0, theta_from_Se(index) - theta_from_sign_Psi(-160, index));
 }


    /*!
     * \brief return current deficit from fieldCapacity [m]
     * \param index
     * \param fieldCapacity
     * \return surface:	0, sub-surface: [m^3 m^-3]
     */
	double DLL_EXPORT __STDCALL getWaterDeficit(long index, double fieldCapacity)
 {
        if (myNode == NULL) return(MEMORY_ERROR);
        if ((index < 0) || (index >= myStructure.nrNodes)) return(INDEX_ERROR);

        if  (myNode[index].isSurface)
            /*! surface */
            return (0.0);
        else
            /*! sub-surface */
            return (theta_from_sign_Psi(-fieldCapacity, index) - theta_from_Se(index));
 }



/*!
  * \brief return degree of saturation (normalized water content)
  * \param nodeIndex
  * \return surface: [-] water presence 0-100 , sub-surface: [%] degree of saturation
  */
 double DLL_EXPORT __STDCALL getDegreeOfSaturation(long nodeIndex)
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


 /*!
  * \brief computes total water content          [m^3]
  * \return result
  */
 double DLL_EXPORT __STDCALL getTotalWaterContent()
 {
    return(computeTotalWaterContent());
 }


 /*!
  * \brief computes hydraulic conductivity                [m/s]
  * \param nodeIndex
  * \return result
  */
 double DLL_EXPORT __STDCALL getWaterConductivity(long nodeIndex)
 {
    /*! error check */
    if (myNode == NULL) return(MEMORY_ERROR);
    if ((nodeIndex < 0) || (nodeIndex >= myStructure.nrNodes)) return(INDEX_ERROR);

    return (myNode[nodeIndex].k);
 }


 /*!
  * \brief comoputes matric potential (psi)           [m]
  * \param nodeIndex [-]
  * \return result
  */
 double DLL_EXPORT __STDCALL getMatricPotential(long nodeIndex)
 {
    if (myNode == NULL) return(MEMORY_ERROR);
    if ((nodeIndex < 0) || (nodeIndex >= myStructure.nrNodes)) return(INDEX_ERROR);

    return (myNode[nodeIndex].H - myNode[nodeIndex].z);
 }


 /*!
  * \brief computes total potential H (psi + z)      [m]
  * \param nodeIndex
  * \return result
  */
 double DLL_EXPORT __STDCALL getTotalPotential(long nodeIndex)
 {
	 if (myNode == NULL) return(MEMORY_ERROR);
	 if ((nodeIndex < 0) || (nodeIndex >= myStructure.nrNodes)) return(INDEX_ERROR);

	 return (myNode[nodeIndex].H);
 }


 /*!
  * \brief computes [m^3] maximum integrated flow over the time step
  * \param n
  * \param direction
  * \return result
  */
 double DLL_EXPORT __STDCALL getWaterFlow(long n, short direction)
 {
    if (myNode == NULL) return MEMORY_ERROR;
    if ((n < 0) || (n >= myStructure.nrNodes)) return INDEX_ERROR;

	double maxFlow = 0.0;

	switch (direction) {
        case UP:
            if (myNode[n].up.index != NOLINK) return (myNode[n].up.sumFlow);
            else return INDEX_ERROR;
            break;
		case DOWN:
            if (myNode[n].down.index != NOLINK) return (myNode[n].down.sumFlow);
            else return INDEX_ERROR;
            break;
		case LATERAL:
			
            for (short i = 0; i < myStructure.nrLateralLinks; i++)
                if (myNode[n].lateral[i].index != NOLINK)
                    if (fabs(myNode[n].lateral[i].sumFlow) > maxFlow)
                        maxFlow = myNode[n].lateral[i].sumFlow;
            return maxFlow;
            break;
        default:
            break;
        }
    return INDEX_ERROR;
 }


 /*!
  * \brief computes [m^3] integrated flow over the time step
  * \param n
  * \return result
  */
 double DLL_EXPORT __STDCALL getSumLateralWaterFlow(long n)
 {
    if (myNode == NULL) return MEMORY_ERROR;
    if ((n < 0) || (n >= myStructure.nrNodes)) return INDEX_ERROR;

    double sumLateralFlow = 0.0;
    for (short i = 0; i < myStructure.nrLateralLinks; i++)
        if (myNode[n].lateral[i].index != NOLINK)
			sumLateralFlow += myNode[n].lateral[i].sumFlow;
	return sumLateralFlow;
 }


 void DLL_EXPORT __STDCALL initializeBalance()
{
    InitializeBalanceWater();
    if (myStructure.computeHeat)
        initializeBalanceHeat();
}

 double DLL_EXPORT __STDCALL getWaterMBR()
 {
    return (balanceWholePeriod.waterMBR);
 }

 double DLL_EXPORT __STDCALL GetHeatMBR()
  {
     return (balanceWholePeriod.heatMBR);
  }

 double DLL_EXPORT __STDCALL GetHeatMBE()
  {
     return (balanceWholePeriod.heatMBE);
  }

 /*!
  * \brief computes [m^3] integrated water flow from boundary over the time step
  * \param nodeIndex
  * \return result
  */
 double DLL_EXPORT __STDCALL getBoundaryWaterFlow(long nodeIndex)
 {

    if (myNode == NULL) return(MEMORY_ERROR);
    if ((nodeIndex < 0) || (nodeIndex >= myStructure.nrNodes)) return(INDEX_ERROR);

	if (myNode[nodeIndex].boundary == NULL) return(BOUNDARY_ERROR);

    return(myNode[nodeIndex].boundary->sumBoundaryWaterFlow);
 }


 /*!
  * \brief computes [m^3] integrated water flow from boundary over the time step
  * \param boundaryType
  * \return result
  */
 double DLL_EXPORT __STDCALL getBoundaryWaterSumFlow(int boundaryType)
 {

    double sumBoundaryFlow = 0.0;

    for (long n = 0; n < myStructure.nrNodes; n++)
        if (myNode[n].boundary != NULL)
            if (myNode[n].boundary->type == boundaryType)
				sumBoundaryFlow += myNode[n].boundary->sumBoundaryWaterFlow;

	return(sumBoundaryFlow);
 }


 /*!
  * \brief computes a period of time [s]
  * \param myPeriod
  */
 void DLL_EXPORT __STDCALL computePeriod(double myPeriod)
    {
		double deltaT, ResidualTime, sumTime = 0.0;

        balanceCurrentPeriod.sinkSourceWater = 0.;
        balanceCurrentPeriod.sinkSourceHeat = 0.;

		while (sumTime < myPeriod)
        {
			ResidualTime = myPeriod - sumTime;
			deltaT = computeStep(ResidualTime);
			sumTime += deltaT;
        }

        if (myStructure.computeWater) updateBalanceWaterWholePeriod();
        if (myStructure.computeHeat) updateBalanceHeatWholePeriod();

    }


 /*!
 * \brief computes a single step of time [s]
 * \param maxTime
 * \return result
 */
double DLL_EXPORT __STDCALL computeStep(double maxTime)
{
    double deltaT;

    updateBoundary();

    if (myStructure.computeWater)
        computeWater(maxTime, &deltaT);
    else
        deltaT = maxTime;

    if (myStructure.computeHeat)
    {
        updateBoundaryHeat();
        HeatComputation(deltaT);
    }

    return maxTime;
}

/*!
 * \brief Set temperature
 * \param nodeIndex
 * \param myT [K]
 * \return OK/ERROR
 */
int DLL_EXPORT __STDCALL SetTemperature(long nodeIndex, double myT)
{
   //----------------------------------------------------------------------------------------------
   // Set current temperature of node
   //----- Input ----------------------------------------------------------------------------------
   // myT              [K] temperature
   //----------------------------------------------------------------------------------------------

   if (myNode == NULL) return(MEMORY_ERROR);

   if ((nodeIndex < 0) || (nodeIndex >= myStructure.nrNodes)) return(INDEX_ERROR);

   if ((myT < 200) && (myT > 500)) return(PARAMETER_ERROR);

   if (myNode->extra == NULL) return(MEMORY_ERROR);

   myNode[nodeIndex].extra->Heat->T = myT;
   myNode[nodeIndex].extra->Heat->oldT = myT;

   return(CRIT3D_OK);
}

/*!
 * \brief Set fixed temperature
 * \param nodeIndex
 * \param myT [K]
 * \return OK/ERROR
 */
int DLL_EXPORT __STDCALL SetFixedTemperature(long nodeIndex, double myT)
{
   if (myNode == NULL) return(MEMORY_ERROR);
   if ((nodeIndex < 0) || (nodeIndex >= myStructure.nrNodes)) return(INDEX_ERROR);
   if (myNode[nodeIndex].boundary == NULL) return(BOUNDARY_ERROR);
   if (myNode[nodeIndex].boundary->type != BOUNDARY_PRESCRIBEDTOTALPOTENTIAL &&
           myNode[nodeIndex].boundary->type != BOUNDARY_FREEDRAINAGE) return(BOUNDARY_ERROR);

   myNode[nodeIndex].boundary->fixedTemperature = myT;

   return(CRIT3D_OK);
}

/*!
 * \brief Set ground water temperature
 * \param myTemperature [K]
 * \return OK/ERROR
 */
int DLL_EXPORT SetGroundWaterTemperature(double myTemperature)
{
    groundWater.temperature = myTemperature;
    return(CRIT3D_OK);
}

/*!
 * \brief Set boundary wind speed
 * \param nodeIndex
 * \param myWindSpeed [m s-1]
 * \return OK/ERROR
 */
int DLL_EXPORT __STDCALL SetHeatBoundaryWindSpeed(long nodeIndex, double myWindSpeed)
{
   if (myNode == NULL) return(MEMORY_ERROR);
   if ((nodeIndex < 0) || (nodeIndex >= myStructure.nrNodes)) return(INDEX_ERROR);
   if ((myWindSpeed < 0) && (myWindSpeed > 1000)) return(PARAMETER_ERROR);

   if (myNode[nodeIndex].boundary == NULL || myNode[nodeIndex].boundary->Heat == NULL)
       return (BOUNDARY_ERROR);

   myNode[nodeIndex].boundary->Heat->windSpeed = myWindSpeed;

   return(CRIT3D_OK);
}

/*!
 * \brief Set boundary rain temperature
 * \param nodeIndex
 * \param myTemperature [K]
 * \return OK/ERROR
 */
int DLL_EXPORT __STDCALL SetHeatBoundaryRainTemperature(long nodeIndex, double myTemperature)
{
   if (myNode == NULL) return(MEMORY_ERROR);
   if ((nodeIndex < 0) || (nodeIndex >= myStructure.nrNodes)) return(INDEX_ERROR);

   if (myNode[nodeIndex].boundary == NULL || myNode[nodeIndex].boundary->Heat == NULL)
       return (BOUNDARY_ERROR);

   myNode[nodeIndex].boundary->Heat->rainTemperature = myTemperature;

   return(CRIT3D_OK);
}

/*!
 * \brief Set boundary roughness height
 * \param nodeIndex
 * \param myRoughness [m]
 * \return OK/ERROR
 */
int DLL_EXPORT __STDCALL SetHeatBoundaryRoughness(long nodeIndex, double myRoughness)
{
   if (myNode == NULL) return(MEMORY_ERROR);
   if ((nodeIndex < 0) || (nodeIndex >= myStructure.nrNodes)) return(INDEX_ERROR);
   if (myRoughness < 0) return(PARAMETER_ERROR);

   if (myNode[nodeIndex].boundary == NULL || myNode[nodeIndex].boundary->Heat == NULL)
       return (BOUNDARY_ERROR);

   myNode[nodeIndex].boundary->Heat->roughnessHeight = myRoughness;

   return(CRIT3D_OK);
}

/*!
 * \brief Set heat sink source
 * \param nodeIndex
 * \param myHeatFlow [W]
 * \return OK/ERROR
 */
int DLL_EXPORT __STDCALL SetHeatSinkSource(long nodeIndex, double myHeatFlow)
{
   if (myNode == NULL)
       return(MEMORY_ERROR);

   if ((nodeIndex < 0) || (nodeIndex >= myStructure.nrNodes))
       return(INDEX_ERROR);

   if (myNode[nodeIndex].boundary != NULL && myNode[nodeIndex].boundary->Heat != NULL)
       myNode[nodeIndex].boundary->Heat->invariantFluxes = myHeatFlow;
   else
       myNode[nodeIndex].extra->Heat->Qh = myHeatFlow;

   return(CRIT3D_OK);
}

/*!
 * \brief Set boundary temperature
 * \param nodeIndex
 * \param myTemperature [K]
 * \return OK/ERROR
 */
int DLL_EXPORT __STDCALL SetHeatBoundaryTemperature(long nodeIndex, double myTemperature)
{
   if (myNode == NULL)
       return(MEMORY_ERROR);

   if ((nodeIndex < 0) || (nodeIndex >= myStructure.nrNodes))
       return(INDEX_ERROR);

   if (myNode[nodeIndex].boundary == NULL || myNode[nodeIndex].boundary->Heat == NULL)
       return (BOUNDARY_ERROR);

   myNode[nodeIndex].boundary->Heat->temperature = myTemperature;

   return(CRIT3D_OK);
}

/*!
 * \brief Set boundary global irradiance
 * \param nodeIndex
 * \param myGlobalIrradiance [W m-2]
 * \return OK/ERROR
 */
int DLL_EXPORT __STDCALL SetHeatBoundaryGlobalIrradiance(long nodeIndex, double myGlobalIrradiance)
{
   if (myNode == NULL)
       return(MEMORY_ERROR);

   if ((nodeIndex < 0) || (nodeIndex >= myStructure.nrNodes))
       return(INDEX_ERROR);

   if (myNode[nodeIndex].boundary == NULL || myNode[nodeIndex].boundary->Heat == NULL)
       return (BOUNDARY_ERROR);

   myNode[nodeIndex].boundary->Heat->globalIrradiance = myGlobalIrradiance;

   return(CRIT3D_OK);
}

/*!
 * \brief Set boundary net irradiance
 * \param nodeIndex
 * \param myNetIrradiance [W m-2]
 * \return OK/ERROR
 */
int DLL_EXPORT __STDCALL SetHeatBoundaryNetIrradiance(long nodeIndex, double myNetIrradiance)
{
   if (myNode == NULL)
       return(MEMORY_ERROR);

   if ((nodeIndex < 0) || (nodeIndex >= myStructure.nrNodes))
       return(INDEX_ERROR);

   if (myNode[nodeIndex].boundary == NULL || myNode[nodeIndex].boundary->Heat == NULL)
       return (BOUNDARY_ERROR);

   myNode[nodeIndex].boundary->Heat->netIrradiance = myNetIrradiance;

   return(CRIT3D_OK);
}

/*!
 * \brief Set boundary air humidity
 * \param nodeIndex
 * \param myRelativeHumidity [%]
 * \return OK/ERROR
 */
int DLL_EXPORT __STDCALL SetHeatBoundaryRelativeHumidity(long nodeIndex, double myRelativeHumidity)
{
   if (myNode == NULL)
       return(MEMORY_ERROR);

   if ((nodeIndex < 0) || (nodeIndex >= myStructure.nrNodes))
       return(INDEX_ERROR);

   if (myNode[nodeIndex].boundary == NULL || myNode[nodeIndex].boundary->Heat == NULL)
       return (BOUNDARY_ERROR);

   myNode[nodeIndex].boundary->Heat->relativeHumidity = myRelativeHumidity;

   double PressSat, ConcVapSat;
   PressSat = SaturationVaporPressure(myNode[nodeIndex].boundary->Heat->temperature - ZEROCELSIUS);
   ConcVapSat = VaporConcentrationFromPressure(PressSat, myNode[nodeIndex].boundary->Heat->temperature);

   myNode[nodeIndex].boundary->Heat->vaporConcentration = ConcVapSat * (myNode[nodeIndex].boundary->Heat->relativeHumidity / 100.);

   return(CRIT3D_OK);
}

/*!
 * \brief Set boundary reference height
 * \param nodeIndex
 * \param myHeight [m]
 * \return OK/ERROR
 */
int DLL_EXPORT __STDCALL SetHeatBoundaryHeight(long nodeIndex, double myHeight)
{
   if (myNode == NULL) return(MEMORY_ERROR);
   if ((nodeIndex < 0) || (nodeIndex >= myStructure.nrNodes)) return(INDEX_ERROR);

   if (myNode[nodeIndex].boundary == NULL || myNode[nodeIndex].boundary->Heat == NULL)
       return (BOUNDARY_ERROR);

   myNode[nodeIndex].boundary->Heat->height = myHeight;

   return(CRIT3D_OK);
}


/*!
 * \brief return node temperature
 * \param nodeIndex
 * \return temperature [K]
*/
double DLL_EXPORT __STDCALL getTemperature(long nodeIndex)
{
    if (myNode == NULL) return(TOPOGRAPHY_ERROR);
    if ((nodeIndex >= myStructure.nrNodes)) return(INDEX_ERROR);
    if (! myStructure.computeHeat) return (MISSING_DATA_ERROR);
    if (myNode->extra == NULL) return (MEMORY_ERROR);

    return (myNode[nodeIndex].extra->Heat->T);
}

/*!
 * \brief return instantaneous thermally driven latent heat flux
 * \param nodeIndex
 * \param myDirection
 * \return thermal latent heat flux [W m-2]
*/
double DLL_EXPORT __STDCALL getThermalLatentHeatFlux(long nodeIndex, short myDirection)
{
    if (myNode == NULL) return(TOPOGRAPHY_ERROR);
    if ((nodeIndex >= myStructure.nrNodes)) return(INDEX_ERROR);
    if (! myStructure.computeHeat && ! myStructure.computeHeatLatent) return (MISSING_DATA_ERROR);
    if (myNode->extra == NULL) return (MEMORY_ERROR);

    switch (myDirection)
    {
    case UP:
        if (myNode[nodeIndex].up.linkedExtra != NULL)
            return (myNode[nodeIndex].up.linkedExtra->heatFlux->thermLatent);
        else
            return(INDEX_ERROR);

    case DOWN:
        if (myNode[nodeIndex].down.linkedExtra != NULL)
            return (myNode[nodeIndex].down.linkedExtra->heatFlux->thermLatent);
        else
            return(INDEX_ERROR);

    case LATERAL:
        for (short i = 0; i < myStructure.nrLateralLinks; i++)
            if (myNode[nodeIndex].lateral[i].index != NOLINK)
                if (myNode[nodeIndex].lateral[i].linkedExtra != NULL)
                    return (myNode[nodeIndex].lateral[i].linkedExtra->heatFlux->thermLatent);

    default : return(INDEX_ERROR);
    }
}

/*!
 * \brief return instantaneous isothermal latent heat flux
 * \param nodeIndex
 * \param myDirection
 * \return isothermal latent heat flux [W m-2]
*/
double DLL_EXPORT __STDCALL getIsothermalLatentHeatFlux(long nodeIndex, short myDirection)
{
    if (myNode == NULL) return(TOPOGRAPHY_ERROR);
    if (nodeIndex >= myStructure.nrNodes) return(INDEX_ERROR);
    if (! myStructure.computeHeat && ! myStructure.computeHeatLatent) return (MISSING_DATA_ERROR);
    if (myNode->extra == NULL) return (MEMORY_ERROR);

    switch (myDirection)
    {
    case UP:
        if (myNode[nodeIndex].up.linkedExtra != NULL)
            return (myNode[nodeIndex].up.linkedExtra->heatFlux->isothermLatent);
        else
            return(INDEX_ERROR);

    case DOWN:
        if (myNode[nodeIndex].down.linkedExtra != NULL)
            return (myNode[nodeIndex].down.linkedExtra->heatFlux->isothermLatent);
        else
            return(INDEX_ERROR);

    case LATERAL:
        for (short i = 0; i < myStructure.nrLateralLinks; i++)
            if (myNode[nodeIndex].lateral[i].index != NOLINK)
                if (myNode[nodeIndex].lateral[i].linkedExtra != NULL)
                    return (myNode[nodeIndex].lateral[i].linkedExtra->heatFlux->isothermLatent);

    default : return(INDEX_ERROR);
    }
}

/*!
 * \brief return instantaneous diffusive heat flux
 * \param nodeIndex
 * \param myDirection
 * \return diffusive heat flux [W m-2]
*/
double DLL_EXPORT __STDCALL getDiffusiveHeatFlux(long nodeIndex, short myDirection)
{
    if (myNode == NULL) return(TOPOGRAPHY_ERROR);
    if ((nodeIndex >= myStructure.nrNodes)) return(INDEX_ERROR);
    if (! myStructure.computeHeat) return (MISSING_DATA_ERROR);
    if (myNode->extra == NULL) return (MEMORY_ERROR);

    switch (myDirection)
    {
    case UP:
        if (myNode[nodeIndex].up.linkedExtra != NULL)
            return (myNode[nodeIndex].up.linkedExtra->heatFlux->diffusive);
        else
            return(INDEX_ERROR);

    case DOWN:
        if (myNode[nodeIndex].down.linkedExtra != NULL)
            return (myNode[nodeIndex].down.linkedExtra->heatFlux->diffusive);
        else
            return(INDEX_ERROR);

    case LATERAL:
        for (short i = 0; i < myStructure.nrLateralLinks; i++)
            if (myNode[nodeIndex].lateral[i].index != NOLINK)
                if (myNode[nodeIndex].lateral[i].linkedExtra != NULL)
                    return (myNode[nodeIndex].lateral[i].linkedExtra->heatFlux->diffusive);

    default : return(INDEX_ERROR);
    }
}

/*!
 * \brief return boundary sensible heat flux
 * \param nodeIndex
 * \return sensbile latent heat flux [W m-2]
*/
double DLL_EXPORT getBoundarySensibleFlux(long nodeIndex)
{
    if (myNode == NULL) return (TOPOGRAPHY_ERROR);
    if (nodeIndex >= myStructure.nrNodes) return (INDEX_ERROR);
    if (! myStructure.computeHeat) return (MISSING_DATA_ERROR);
    if (myNode[nodeIndex].boundary == NULL) return (INDEX_ERROR);
    if (myNode[nodeIndex].boundary->type != BOUNDARY_HEAT) return (INDEX_ERROR);

    // boundary sensible heat flow density
    return (myNode[nodeIndex].boundary->Heat->sensibleFlux);
}


/*!
 * \brief return boundary latent heat flux
 * \param nodeIndex
 * \return latent heat flux [W m-2]
*/
double DLL_EXPORT getBoundaryLatentFlux(long nodeIndex)
{
    if (myNode == NULL) return (TOPOGRAPHY_ERROR);
    if (nodeIndex >= myStructure.nrNodes) return (INDEX_ERROR);
    if (! myStructure.computeHeat || ! myStructure.computeHeatLatent) return (MISSING_DATA_ERROR);
    if (myNode[nodeIndex].boundary == NULL) return (INDEX_ERROR);
    if (myNode[nodeIndex].boundary->type != BOUNDARY_HEAT) return (INDEX_ERROR);

    // boundary latent heat flow density
    return (myNode[nodeIndex].boundary->Heat->latentFlux);
}

/*!
 * \brief return boundary advective heat flux
 * \param nodeIndex
 * \return advective heat flux [W m-2]
*/
double DLL_EXPORT getBoundaryAdvectiveFlux(long nodeIndex)
{
    if (myNode == NULL) return (TOPOGRAPHY_ERROR);
    if (nodeIndex >= myStructure.nrNodes) return (INDEX_ERROR);
    if (! myStructure.computeHeat || ! myStructure.computeHeatAdvective) return (MISSING_DATA_ERROR);
    if (myNode[nodeIndex].boundary == NULL) return (INDEX_ERROR);
    if (myNode[nodeIndex].boundary->type != BOUNDARY_HEAT) return (INDEX_ERROR);

    // boundary advective heat flow density
    return (myNode[nodeIndex].boundary->advectiveHeatFlux);
}

/*!
 * \brief return boundary radiative heat flux
 * \param nodeIndex
 * \return radiative heat flux [W m-2]
*/
double DLL_EXPORT getBoundaryRadiativeFlux(long nodeIndex)
{
    if (myNode == NULL) return (TOPOGRAPHY_ERROR);
    if (nodeIndex >= myStructure.nrNodes) return (INDEX_ERROR);
    if (! myStructure.computeHeat) return (MISSING_DATA_ERROR);
    if (myNode[nodeIndex].boundary == NULL) return (INDEX_ERROR);
    if (myNode[nodeIndex].boundary->type != BOUNDARY_HEAT) return (INDEX_ERROR);

    // boundary net radiative heat flow density
    return (myNode[nodeIndex].boundary->Heat->radiativeFlux);
}

/*!
 * \brief return boundary aerodynamic conductance
 * \param nodeIndex
 * \return aerodynamic conductance [m s-1]
*/
double DLL_EXPORT getBoundaryAerodynamicConductance(long nodeIndex)
{
    if (myNode == NULL) return (TOPOGRAPHY_ERROR);
    if (nodeIndex >= myStructure.nrNodes) return (INDEX_ERROR);
    if (! myStructure.computeHeat) return (MISSING_DATA_ERROR);
    if (myNode[nodeIndex].boundary == NULL) return (INDEX_ERROR);
    if (myNode[nodeIndex].boundary->type != BOUNDARY_HEAT) return (INDEX_ERROR);

    // boundary aerodynamic resistance
    return (myNode[nodeIndex].boundary->Heat->aerodynamicConductance);
}

/*
/*!
 * \brief return boundary aerodynamic conductance for open water
 * \param nodeIndex
 * \return aerodynamic conductance [m s-1]

double DLL_EXPORT getBoundaryAerodynamicConductanceOpenWater(long nodeIndex)
{
    if (myNode == NULL) return (TOPOGRAPHY_ERROR);
    if ((nodeIndex < 1) || (nodeIndex >= myStructure.nrNodes)) return (INDEX_ERROR);
    if (! myStructure.computeHeat) return (MISSING_DATA_ERROR);
    if (myNode[nodeIndex].boundary == NULL) return (INDEX_ERROR);
    if (myNode[nodeIndex].boundary->type != BOUNDARY_HEAT) return (INDEX_ERROR);

    // boundary aerodynamic resistance
    return (myNode[nodeIndex].boundary->Heat->aerodynamicConductanceOpenWater);
}
*/

/*!
 * \brief return boundary soil conductance
 * \param nodeIndex
 * \return soil conductance [m s-1]
*/
double DLL_EXPORT getBoundarySoilConductance(long nodeIndex)
{
    if (myNode == NULL) return (TOPOGRAPHY_ERROR);
    if (nodeIndex >= myStructure.nrNodes) return (INDEX_ERROR);
    if (! myStructure.computeHeat) return (MISSING_DATA_ERROR);
    if (myNode[nodeIndex].boundary == NULL) return (INDEX_ERROR);
    if (myNode[nodeIndex].boundary->type != BOUNDARY_HEAT) return (INDEX_ERROR);

    // boundary soil conductance
    return (myNode[nodeIndex].boundary->Heat->soilConductance);
}

/*!
 * \brief return vapor concentration
 * \param nodeIndex
 * \return vapor concentration [kg m-3]
*/
double DLL_EXPORT getNodeVapor(long nodeIndex)
{
    if (myNode == NULL) return(TOPOGRAPHY_ERROR);
    if (nodeIndex >= myStructure.nrNodes) return(INDEX_ERROR);
    if (! myStructure.computeHeat || ! myStructure.computeHeatLatent) return (MISSING_DATA_ERROR);

    double mySatVapPressure, mySatVapConcentration, myRelHum;
    double myVapor;

    mySatVapPressure = SaturationVaporPressure(myNode[nodeIndex].extra->Heat->T - ZEROCELSIUS);
    mySatVapConcentration = VaporConcentrationFromPressure(mySatVapPressure, myNode[nodeIndex].extra->Heat->T);
    myRelHum = getSoilRelativeHumidity(getPsiMean(nodeIndex), myNode[nodeIndex].extra->Heat->T);

    myVapor = mySatVapConcentration * myRelHum;

    return (myVapor);
}

/*!
 * \brief return heat storage
 * \param nodeIndex
 * \return heat storage [J]
*/
double DLL_EXPORT getHeat(long i)
{
    if (myNode == NULL) return(TOPOGRAPHY_ERROR);
    if (i >= myStructure.nrNodes) return(INDEX_ERROR);
    if (! myStructure.computeHeat) return (MISSING_DATA_ERROR);
    if (myNode[i].extra->Heat == NULL) return MISSING_DATA_ERROR;
    if (myNode[i].H == NODATA || myNode[i].extra->Heat->T == NODATA) return MISSING_DATA_ERROR;

    return (soilHeatCapacity(i, myNode[i].H) * myNode[i].volume_area  * myNode[i].extra->Heat->T);

}

}

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


#include <math.h>
#include <stdlib.h>

#include "header/types.h"
#include "header/heat.h"
#include "header/soilPhysics.h"
#include "header/water.h"
#include "header/solver.h"
#include "header/soilFluxes3D.h"
#include "header/boundary.h"
#include "header/physics.h"


/*!
 * \brief [m2 s-1] binary vapor diffusivity
 * (Do) in Bittelli (2008) or vapor diffusion coefficient in air (Dva) in Monteith (1973)
 * \param myPressure
 * \param myTemperature
 * \return result
 */
double getVaporBinaryDiffusivity(double myPressure, double myTemperature)
{	return (VaporDiffusivity0 * (p0 / myPressure) * pow(myTemperature / ZEROCELSIUS, 1.75)); }

/*!
 * \brief [m2 s-1] vapor diffusivity
 * \param i
 * \param myT
 * \return result
 */
double getSoilVaporDiffusivity(long i, double myT)
{
	double binaryDiffusivity;	// [m2 s-1]
	double airFilledPorosity;	// [m3 m-3]
	double myPressure;			// [Pa]
	double const beta = 0.9;	// [] following Bittelli et al 2008
	double const emme = 2.3;	// [] idem

    myPressure = PressureFromAltitude(myNode[i].z);
	binaryDiffusivity = getVaporBinaryDiffusivity(myPressure, myT);
    airFilledPorosity = myNode[i].Soil->Theta_s - getThetaMean(i);

	return (binaryDiffusivity * beta * pow(airFilledPorosity, emme));
}

/*!
 * \brief [] soil relative humidity
 * \param myPsi
 * \param myT
 * \return result
 */
double getSoilRelativeHumidity(double myPsi, double myT)
{	return (exp(MH2O * (myPsi * GRAVITY) / (R * myT))); }

/*!
 * \brief [kg s m-3] isothermal vapor conductivity
 * \param i
 * \param myT
 * \return result
 */
double getIsothermalVaporConductivity(long i, double myT)
{
	double Dv = getSoilVaporDiffusivity(i, myT);
    return (Dv * soilFluxes3D::getNodeVapor(i) * MH2O / (R * myT));
}

/*!
 * \brief [J m-3 K-1] volumetric specific heat
 * Soil specific heat according to Campbell "Soil physics with basic" pp 31-32
 * \param i
 * \return result
 */
double getSpecificHeat(long i)
{   /* Soil specific heat according to Campbell "Soil physics with basic" pp 31-32
	INPUT:
	bulk density [Mg m-3]
	volumetric water content [m3 m-3]
	OUTPUT:
	volumetric specific heat [J m-3 K-1]
	*/

    return (getBulkDensity(i) / 2.65 * VolSpecHeatMineral + theta_from_Se(myNode[i].Se ,i) * VolSpecHeatH2O);
}

/*!
 * \brief [J m-3 K-1] volumetric heat capacity
 * \param i
 * \param myH
 * \return result
 */
double soilHeatCapacity(long i, double H)
{
    double theta=0.3;//getTheta(i, H);
    return getBulkDensity(i) / 2.65 * VolSpecHeatMineral + theta * VolSpecHeatH2O;
}

/*!
 * \brief [] water return flow factor
 * Campbell 1994
 * \param myTheta
 * \param myClayFraction
 * \param myTemperature
 * \return result
 */
double getWaterReturnFlowFactor(double myTheta, double myClayFraction, double myTemperature)
{
    double Q0, Q;                                   // [] power
    double xw0 = 0.33 * myClayFraction + 0.078;		// [] cutoff water content
    if (myTheta < (0.01 * xw0))
		return 0.;
	else
    {
        Q0 = 7.25 * myClayFraction + 2.52;
        Q = Q0 * (pow(myTemperature / 303., 2));
    }

    return (1 / (1 + pow(myTheta / xw0, -Q)));
}

/*!
 * \brief [kg m-1 s-1 K-1] non isothermal air vapor conductivity
 * DTvap in Hammel et al. (1981)
 * \param i
 * \param myTMean
 * \return result
 */
double getNonIsothermalAirVaporConductivity(long i, double myTMean)
{
	double fw;						// [] liquid return flow cutoff
    double myPressure;				// [Pa] total air pressure
	double Dv;						// [m2 s-1] vapor diffusivity
	double svp;						// [Pa] saturation vapor pressure
	double slopesvp;				// [Pa K-1] slope of saturation vapor pressure curve
    double slopesvc;                // [kg m-3 K-1] slope of saturation vapor concentration
    double myVapor;					// [kg m-3] vapor concentration
	double myVaporPressure;			// [Pa] vapor partial pressure
	double hr;						// [] relative humidity
	double Stefan;					// [Pa-1] Stefan correction factor
	double myTCelsiusMean;

    myTCelsiusMean = myTMean - ZEROCELSIUS;

    myPressure = PressureFromAltitude(myNode[i].z);

	// vapor diffusivity
    Dv = getSoilVaporDiffusivity(i, myTMean);

	// slope of saturation vapor pressure
    svp = SaturationVaporPressure(myTCelsiusMean);
    slopesvp = SaturationSlope(myTCelsiusMean, svp);

    // slope of saturation vapor density
    slopesvc = MH2O / (R * myNode[i].extra->Heat->T) * (slopesvp - svp / myNode[i].extra->Heat->T);

	// relative humidity
    myVapor = soilFluxes3D::getNodeVapor(i);
    myVaporPressure = VaporPressureFromConcentration(myVapor, myTMean);
	hr = myVaporPressure / svp;

    // Stefan correction factor (De Vries, 1957)
    Stefan = myPressure / (myPressure - (hr * myVaporPressure));

	// correction factor for return flow of water
    fw = getWaterReturnFlowFactor(getThetaMean(i), myNode[i].Soil->clay, myTMean);

    return (Dv * slopesvc * hr * Stefan * fw);

}

/*!
 * \brief [J s-1 m-1 K-1] air thermal conductivity
 * \param i
 * \return result
 */
double getAirHeatConductivity(long i)
{
    double Kda;						// [J s-1 m-1 K-1] thermal conductivity of dry air
    double Ka;						// [J s-1 m-1 K-1] thermal conductivity of air
    double myDtvap;                 // [kg m-1 s-1 K-1] non isothermal vapor conductivity
    double myLambda;				// [J kg-1] latent heat of vaporization
    double myTCelsiusMean;          // [°C]

    // dry air conductivity
    myTCelsiusMean = myNode[i].extra->Heat->T - ZEROCELSIUS;
	Kda = 0.024 + 0.0000773 * myTCelsiusMean - 0.000000026 * myTCelsiusMean * myTCelsiusMean;

    Ka = Kda;

    if (myStructure.computeHeatLatent)
    {
        myLambda = LatentHeatVaporization(myNode[i].extra->Heat->T - ZEROCELSIUS);
        myDtvap = getNonIsothermalAirVaporConductivity(i, myNode[i].extra->Heat->T);
        Ka += myLambda * myDtvap;
    }

	return (Ka);
}

/*!
 * \brief [W m-1 K-1] soil thermal conductivity
 * according to Campbell et al. Soil Sci. 158:307-313
 * \param i
 * \return result
 */
double soilHeatConductivity(long i)
{
	double ga = 0.088;				// [] deVries shape factor; assume same for all mineral soils
	double gc;						// [] shape factor
	double ea;						// [] air weighting factor
	double es ;						// [] solid weighting factor
	double ew;						// [] water weighting factor
	double Ka;						// [W m-1 K-1] thermal conductivity of air
	double Kw;						// [W m-1 K-1] thermal conductivity of water
	double Kf;						// [W m-1 K-1] thermal conductivity of fluids
	double xa;						// [m3 m-3] volume fraction of air
	double xw;						// [m3 m-3] volume fraction of water
	double xs;						// [m3 m-3] volume fraction of solids
	double myConductivity;			// [W m-1 K-1] total thermal conductivity
	double myTCelsiusMean;
    double fw;						// [] water return flow factor (same in air conductivity)

    myTCelsiusMean = (myNode[i].extra->Heat->T + myNode[i].extra->Heat->oldT) / 2. - ZEROCELSIUS;

	// water conductivity
	Kw = 0.554 + 0.0024 * myTCelsiusMean - 0.00000987 * myTCelsiusMean * myTCelsiusMean;

	// air conductivity
	Ka = getAirHeatConductivity(i);

    xw = getThetaMean(i);

    fw = getWaterReturnFlowFactor(xw, myNode[i].Soil->clay, myTCelsiusMean + ZEROCELSIUS);
	Kf = Ka + fw * (Kw - Ka);

	gc = 1. - 2. * ga;

    ea = (2. / (1 + (Ka / Kf - 1) * ga) + 1 / (1 + (Ka / Kf - 1) * gc)) / 3.;
	ew = (2. / (1 + (Kw / Kf - 1) * ga) + 1 / (1 + (Kw / Kf - 1) * gc)) / 3.;
	es = (2. / (1 + (kMineral / Kf - 1) * ga) + 1 / (1 + (kMineral / Kf - 1) * gc)) / 3.;

	xs = 1. - myNode[i].Soil->Theta_s;
	xa = myNode[i].Soil->Theta_s - xw;

	myConductivity = (xw * ew * Kw + xa * ea * Ka + xs * es * kMineral) / (ew * xw + ea * xa + es * xs);
    return myConductivity;

}

double getMeanIsothermalVaporConductivity(long myIndex, long myLinkIndex)
{
    double myKv, myLinkKv;				// (kg m-3 s-1) vapor conductivity

    myKv = getIsothermalVaporConductivity(myIndex, myNode[myIndex].extra->Heat->T);
    myLinkKv = getIsothermalVaporConductivity(myLinkIndex, myNode[myLinkIndex].extra->Heat->T);

	return (computeMean(myKv, myLinkKv));
}

double soilLatentIsothermal(long myIndex, TlinkedNode *myLink)
{	// isothermal latent heat
    // the thermal component is already explicitly computed inside getHeatConductivity

	double myLambda, linkLambda, meanLambda;	// (J kg-1) latent heat of vaporization
	double myKv;									// (kg2 s-1 m-3) vapor conductivity
	double myPsi, myLinkPsi;						// (J kg-1) water matric potential
	double myDeltaPsi;								// (J kg-1) water potential difference
	double mylatentFlux;							// (J m-1 s-1) latent heat flow

	long myLinkIndex = (*myLink).index;

    myLambda = LatentHeatVaporization(myNode[myIndex].extra->Heat->T - ZEROCELSIUS);
    linkLambda = LatentHeatVaporization(myNode[myLinkIndex].extra->Heat->T - ZEROCELSIUS);

	myKv = getMeanIsothermalVaporConductivity(myIndex, myLinkIndex);
    meanLambda = computeMean(myLambda, linkLambda);

	// conversion to J/kg (m2/s2)
    myPsi = (getHMean(myIndex) - myNode[myIndex].z) * GRAVITY;
    myLinkPsi = (getHMean(myLinkIndex) - myNode[myLinkIndex].z) * GRAVITY;

	myDeltaPsi = (myPsi - myLinkPsi);

	mylatentFlux = meanLambda * myKv * myDeltaPsi / distance(myIndex, myLinkIndex);

    return (mylatentFlux);
}

double soilAdvection(long i, TlinkedNode *myLink)
{
	long myLinkIndex;
    double myDeltaT;
	double myWaterFlow;

	myLinkIndex = (*myLink).index;
    myWaterFlow = getWaterFlux(i, myLink);
    myDeltaT = ((myNode[myLinkIndex].extra->Heat->oldT + myNode[myLinkIndex].extra->Heat->T)/2. - (myNode[i].extra->Heat->oldT + myNode[i].extra->Heat->T)/2.);
	// water flow (m3 s-1) already contains volume
	// J m-3 K-1 * m3 s-1 * K = J s-1
    return (VolSpecHeatH2O * myWaterFlow * myDeltaT);
}

double soilConduction(long myIndex, TlinkedNode *myLink)
{
	double myConductivity, linkConductivity, meanKh;
    double zeta;

	long myLinkIndex = (*myLink).index;
    double myDistance = distance(myIndex, myLinkIndex);

    zeta = myLink->area / myDistance;

    myConductivity = soilHeatConductivity(myIndex);
    linkConductivity = soilHeatConductivity(myLinkIndex);
    meanKh = computeMean(myConductivity, linkConductivity);

    return (zeta * meanKh);
}

bool computeHeatFlux(long i, int myMatrixIndex, TlinkedNode *myLink)
{
    if (myLink == NULL) return false;
    if ((*myLink).index == NOLINK) return false;

    long myLinkIndex = (*myLink).index;
    double myConduction, myAdvection, myLatent;

    myConduction = 0.;
    myAdvection = 0.;
    myLatent = 0.;

    if (!myNode[myLinkIndex].isSurface)
    {
        myConduction = soilConduction(i, myLink);
        if (myStructure.computeHeatAdvective)
        {
            myAdvection = soilAdvection(i, myLink);
            if (myLink->linkedExtra->heatFlux != NULL)
                myLink->linkedExtra->heatFlux->advective = myAdvection;
        }
        if (myStructure.computeHeatLatent)
        {
            myLatent = soilLatentIsothermal(i, myLink);
            if (myLink->linkedExtra->heatFlux != NULL)
                myLink->linkedExtra->heatFlux->isothermLatent = myLatent;
        }
    }

    A[i][myMatrixIndex].index = myLinkIndex;
    A[i][myMatrixIndex].val = myConduction;

    C0[i] += myAdvection + myLatent;

    return (true);
}

double computeHeatStorage()
{ // [J]
    double myHeatStorage = 0.;
    for (long i = 1; i < myStructure.nrNodes; i++)
        myHeatStorage += soilFluxes3D::getHeat(i);
    return myHeatStorage;
}

void computeHeatBalance(double myTimeStep)
{
    // heat sink/source total
    double myHeatSinkSourceSum = 0.;
    for (long i = 1; i < myStructure.nrNodes; i++)
        myHeatSinkSourceSum += myNode[i].extra->Heat->Qh * myTimeStep;

    balanceCurrentTimeStep.sinkSourceHeat = myHeatSinkSourceSum;

    balanceCurrentTimeStep.storageHeat = computeHeatStorage();

    double deltaHeatStorage = balanceCurrentTimeStep.storageHeat - balancePreviousTimeStep.storageHeat;
    balanceCurrentTimeStep.heatMBE = deltaHeatStorage - balanceCurrentTimeStep.sinkSourceHeat;

    double referenceHeat = maxValue(fabs(balanceCurrentTimeStep.sinkSourceHeat), balanceCurrentTimeStep.storageHeat * 1e-5);
    balanceCurrentTimeStep.heatMBR = 1. - balanceCurrentTimeStep.heatMBE / referenceHeat;
}

void storeHeatFlows(long myIndex, TlinkedNode *myLink)
// [W m-2] heat flow between node myNode[myIndex] and link node myLink
{
   if (myLink != NULL && ! myNode[myLink->index].isSurface)
   {
        long myLinkIndex = (*myLink).index;

        int j = 1;
        while ((j < myStructure.maxNrColumns) && (A[myIndex][j].index != NOLINK) && (A[myIndex][j].index != myLinkIndex)) j++;

        if (A[myIndex][j].index == myLinkIndex)
        {
            double myA = (A[myIndex][j].val * A[myIndex][0].val);
            myLink->linkedExtra->heatFlux->diffusive = myA * (myNode[myIndex].extra->Heat->T - myNode[myLinkIndex].extra->Heat->T) * myParameters.heatWeightingFactor;
            myLink->linkedExtra->heatFlux->diffusive += myA * (myNode[myIndex].extra->Heat->oldT - myNode[myLinkIndex].extra->Heat->oldT) * (1. - myParameters.heatWeightingFactor);
        }
        else
            myLink->linkedExtra->heatFlux->diffusive = NODATA;
    }
}

void updateBalanceHeat()
{
    balancePreviousTimeStep.storageHeat = balanceCurrentTimeStep.storageHeat;
    balancePreviousTimeStep.sinkSourceHeat = balanceCurrentTimeStep.sinkSourceHeat;
    balanceCurrentPeriod.sinkSourceHeat += balanceCurrentTimeStep.sinkSourceHeat;

    // update heat fluxes
    for (long i = 1; i < myStructure.nrNodes; i++)
    {
        if (myNode[i].up.index != NOLINK)
            if (myNode[i].up.linkedExtra->heatFlux != NULL)
                storeHeatFlows(i, &(myNode[i].up));

        if (myNode[i].down.index != NOLINK)
            if (myNode[i].down.linkedExtra->heatFlux != NULL)
                storeHeatFlows(i, &(myNode[i].down));

        for (short j = 0; j < myStructure.nrLateralLinks; j++)
            if (myNode[i].lateral[j].index != NOLINK)
                if (myNode[i].lateral[j].linkedExtra->heatFlux != NULL)
                    storeHeatFlows(i, &(myNode[i].lateral[j]));
    }
}

bool heatBalance(double timeStep, int myApprox)
{
    computeHeatBalance(timeStep);
    return ((fabs(1.-balanceCurrentTimeStep.heatMBR) < myParameters.MBRThreshold) || (myApprox == (myParameters.maxApproximationsNumber - 1)));
}

void initializeBalanceHeat()
{
	 balanceCurrentTimeStep.sinkSourceHeat = 0.;
     balancePreviousTimeStep.sinkSourceHeat = 0.;
     balanceCurrentPeriod.sinkSourceHeat = 0.;
     balanceWholePeriod.sinkSourceHeat = 0.;

     balanceCurrentTimeStep.heatMBE = 0.;
     balanceCurrentPeriod.heatMBE = 0.;
     balanceWholePeriod.waterMBE = 0.;

     balanceCurrentTimeStep.heatMBR = 0;
     balanceCurrentPeriod.heatMBR = 0.;
     balanceWholePeriod.waterMBR = 0.;

	 balanceWholePeriod.storageHeat = computeHeatStorage();
	 balanceCurrentTimeStep.storageHeat = balanceWholePeriod.storageHeat;
	 balancePreviousTimeStep.storageHeat = balanceWholePeriod.storageHeat;
	 balanceCurrentPeriod.storageHeat = balanceWholePeriod.storageHeat;
}

void updateBalanceHeatWholePeriod()
{
    /*! update the flows in the balance (balanceWholePeriod) */
    balanceWholePeriod.sinkSourceHeat  += balanceCurrentPeriod.sinkSourceHeat;

    double deltaStoragePeriod = balanceCurrentTimeStep.storageHeat - balanceCurrentPeriod.storageHeat;
    double deltaStorageHistorical = balanceCurrentTimeStep.storageHeat - balanceWholePeriod.storageHeat;

    /*! compute MBE and MBR */
    balanceCurrentPeriod.heatMBE = deltaStoragePeriod - balanceCurrentPeriod.sinkSourceHeat;
    balanceWholePeriod.heatMBE = deltaStorageHistorical - balanceWholePeriod.sinkSourceHeat;
    if ((balanceWholePeriod.storageHeat == 0.) && (balanceWholePeriod.sinkSourceHeat == 0.)) balanceWholePeriod.heatMBR = 1.;
    else if (balanceCurrentTimeStep.storageHeat > fabs(balanceWholePeriod.sinkSourceHeat))
        balanceWholePeriod.heatMBR = balanceCurrentTimeStep.storageHeat / (balanceWholePeriod.storageHeat + balanceWholePeriod.sinkSourceHeat);
    else
        balanceWholePeriod.heatMBR = deltaStorageHistorical / balanceWholePeriod.sinkSourceHeat;

    /*! update storageWater in balanceCurrentPeriod */
    balanceCurrentPeriod.storageHeat = balanceCurrentTimeStep.storageHeat;
}

void restoreHeat()
{
	for (long i = 0; i < myStructure.nrNodes; i++)
        myNode[i].extra->Heat->T = myNode[i].extra->Heat->oldT;
	// ripristinare vecchi vapori al boundary?
}

bool HeatComputation(double myTimeStep)
{

	long i, j;
    bool isValidStep = false;
    double sum, sumFlow0, myDeltaTemp0;
	int myApprox = 0;

	do  {

		if (myApprox == 0)
            for (i = 1; i < myStructure.nrNodes; i++)
			{
                A[i][0].index = i;
                X[i] = myNode[i].extra->Heat->T;
                myNode[i].extra->Heat->oldT = myNode[i].extra->Heat->T;
			}

        for (i = 1; i < myStructure.nrNodes; i++)
			{
                C0[i] = 0.;
                C[i] = soilHeatCapacity(i, computeMean(myNode[i].oldH, myNode[i].H)) * myNode[i].volume_area;

				j = 1;
                if (computeHeatFlux(i, j, &(myNode[i].up))) j++;
                for (short l = 0; l < myStructure.nrLateralLinks; l++)
                    if (computeHeatFlux(i, j, &(myNode[i].lateral[l]))) j++;
                if (computeHeatFlux(i, j, &(myNode[i].down))) j++;

                // closure
                while (j < myStructure.maxNrColumns)
                    A[i][j++].index = NOLINK;

				j = 1;
                sum = 0.;
				sumFlow0 = 0;
                myDeltaTemp0 = 0;

                while ((j < myStructure.maxNrColumns) && (A[i][j].index != NOLINK))
				{
                    sum += A[i][j].val; // * myParameters.heatWeightingFactor);
                    //myDeltaTemp0 = myNode[A[i][j].index].extra->Heat->oldT - myNode[i].extra->Heat->oldT;
                    //sumFlow0 += A[i][j].val * (1. - myParameters.heatWeightingFactor) * myDeltaTemp0;
                    A[i][j++].val *= -1.0; // -(myParameters.heatWeightingFactor);
				}

                /*! sum of diagonal elements */
                A[i][0].val =  C[i] / myTimeStep + sum;

                /*! b vector (constant terms) */
                b[i] = C[i] * myNode[i].extra->Heat->oldT / myTimeStep + myNode[i].extra->Heat->Qh + C0[i]; // + sumFlow0 ;

				// precondizionamento
				if (A[i][0].val > 0)
				{
					b[i] /= A[i][0].val;
					j = 1;
                    while ((j < myStructure.maxNrColumns) && (A[i][j].index != NOLINK))
						A[i][j++].val /= A[i][0].val;
				}
			}

        GaussSeidelRelaxation(myApprox, myParameters.ResidualTolerance, PROCESS_HEAT);

        for (i = 1; i < myStructure.nrNodes; i++)
            myNode[i].extra->Heat->T = X[i];

		// heat balance
        isValidStep = heatBalance(myTimeStep, myApprox);

        } while ((!isValidStep) && (++myApprox < myParameters.maxApproximationsNumber));

    updateBalanceHeat();

	// save old temperatures
    for (long n = 1; n < myStructure.nrNodes; n++)
        myNode[n].extra->Heat->oldT = myNode[n].extra->Heat->T;

    return (isValidStep);
}


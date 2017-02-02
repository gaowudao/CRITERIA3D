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
#include "header/soil.h"
#include "header/water.h"
#include "header/solver.h"
#include "header/criteria3D.h"
#include "header/boundary.h"
#include "header/physics.h"

double previousHeatMBRError;

double getVaporBinaryDiffusivity(double myPressure, double myTemperature)
// [m2 s-1] binar vapor diffusivity (Do) in Bittelli (2008) or vapor diffusion coefficient in air (Dva) in Monteith (1973)
{	return (VaporDiffusivity0 * (p0 / myPressure) * pow(myTemperature / ZEROCELSIUS, 1.75)); }

double getSoilVaporDiffusivity(long i, double myT)
// [m2 s-1] vapor diffusivity
{
	double binaryDiffusivity;	// [m2 s-1]
	double airFilledPorosity;	// [m3 m-3]
	double myPressure;			// [Pa]
	double const beta = 0.9;	// [] following Bittelli et al 2008
	double const emme = 2.3;	// [] idem

    myPressure = PressureFromAltitude(myNode[i].z);
	binaryDiffusivity = getVaporBinaryDiffusivity(myPressure, myT);
	airFilledPorosity = myNode[i].Soil->Theta_s - soil::getThetaMean(i);

	return (binaryDiffusivity * beta * pow(airFilledPorosity, emme));
}

double getSoilRelativeHumidity(double myPsi, double myT)
{	return (exp(MH2O * (myPsi * GRAVITY) / (R * myT))); }

double getIsothermalVaporConductivity(long i, double myT)
{
    // kg s m-3
	double Dv = getSoilVaporDiffusivity(i, myT);
    return (Dv * criteria3D::getNodeVapor(i) * MH2O / (R * myT));
}


double getSpecificHeat(long i)
{   /* Soil specific heat according to Campbell "Soil physics with basic" pp 31-32
	INPUT:
	bulk density [Mg m-3]
	volumetric water content [m3 m-3]
	OUTPUT:
	volumetric specific heat [J m-3 K-1]
	*/

	return (soil::getBulkDensity(i) / 2.65 * VolSpecHeatMineral + soil::theta_from_Se(myNode[i].Se ,i) * VolSpecHeatH2O);
}

double getSpecificHeat(long i, double myH)
{ // [J m-3 K-1]
    double mySpecificHeat;

    double myPressure = PressureFromAltitude(myNode[i].z);

    double myTheta = soil::theta_from_sign_Psi(myH - myNode[i].z, i);

    double airFraction = myNode[i].Soil->Theta_s - myTheta;

    mySpecificHeat = ((1. - myNode[i].Soil->Theta_s) * VolSpecHeatMineral +
                      myTheta * VolSpecHeatH2O +
                      AirVolumetricSpecificHeat(myPressure, myNode[i].T) * airFraction);

    return mySpecificHeat;
}

double getWaterReturnFlowFactor(double myTheta, double myClayFraction, double myTemperature)
{ // Campbell 1994
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


double getNonIsothermalAirVaporConductivity(long i, double myTMean)
{// [kg m-1 s-1 K-1] DTvap in Hammel et al. (1981)
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
    slopesvc = MH2O / (R * myNode[i].T) * (slopesvp - svp / myNode[i].T);

	// relative humidity
    myVapor = criteria3D::getNodeVapor(i);
    myVaporPressure = VaporPressureFromConcentration(myVapor, myTMean);
	hr = myVaporPressure / svp;

    // Stefan correction factor (De Vries, 1957)
    Stefan = myPressure / (myPressure - (hr * myVaporPressure));

	// correction factor for return flow of water
    fw = getWaterReturnFlowFactor(soil::getThetaMean(i), myNode[i].Soil->Clay, myTMean);

    return (Dv * slopesvc * hr * Stefan * fw);

}

double getAirHeatConductivity(long i)
{
    double Kda;						// [J s-1 m-1 K-1] thermal conductivity of dry air
    double Ka;						// [J s-1 m-1 K-1] thermal conductivity of air
    double myDtvap;                 // [kg m-1 s-1 K-1] non isothermal vapor conductivity
    double myLambda;				// [J kg-1] latent heat of vaporization
    double myTCelsiusMean;          // [°C]

    // dry air conductivity
    myTCelsiusMean = myNode[i].T - ZEROCELSIUS;
	Kda = 0.024 + 0.0000773 * myTCelsiusMean - 0.000000026 * myTCelsiusMean * myTCelsiusMean;

    Ka = Kda;

    if (myStructure.computeHeatLatent)
    {
        myLambda = LatentHeatVaporization(myNode[i].T - ZEROCELSIUS);
        myDtvap = getNonIsothermalAirVaporConductivity(i, myNode[i].T);
        Ka += myLambda * myDtvap;
    }

	return (Ka);
}

double getHeatConductivity(long i)
{
  /* Soil thermal conductivity according to Campbell et al. Soil Sci. 158:307-313
    [W m-1 K-1]
  */

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

    myTCelsiusMean = (myNode[i].T + myNode[i].oldT) / 2. - ZEROCELSIUS;

	// water conductivity
	Kw = 0.554 + 0.0024 * myTCelsiusMean - 0.00000987 * myTCelsiusMean * myTCelsiusMean;

	// air conductivity
	Ka = getAirHeatConductivity(i);

	xw = soil::getThetaMean(i);

    fw = getWaterReturnFlowFactor(xw, myNode[i].Soil->Clay, myTCelsiusMean + ZEROCELSIUS);
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

	myKv = getIsothermalVaporConductivity(myIndex, myNode[myIndex].T);
	myLinkKv = getIsothermalVaporConductivity(myLinkIndex, myNode[myLinkIndex].T);

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

    myLambda = LatentHeatVaporization(myNode[myIndex].T - ZEROCELSIUS);
    linkLambda = LatentHeatVaporization(myNode[myLinkIndex].T - ZEROCELSIUS);

	myKv = getMeanIsothermalVaporConductivity(myIndex, myLinkIndex);
    meanLambda = computeMean(myLambda, linkLambda);

	// conversion to J/kg (m2/s2)
    myPsi = (soil::getHMean(myIndex) - myNode[myIndex].z) * GRAVITY;
    myLinkPsi = (soil::getHMean(myLinkIndex) - myNode[myLinkIndex].z) * GRAVITY;

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
    myDeltaT = ((myNode[myLinkIndex].oldT + myNode[myLinkIndex].T)/2. - (myNode[i].oldT + myNode[i].T)/2.);
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

    zeta = (*myLink).area / myDistance;

	myConductivity = getHeatConductivity(myIndex);
	linkConductivity = getHeatConductivity(myLinkIndex);
	meanKh = computeMean(myConductivity, linkConductivity);

    return (zeta * meanKh);
}

bool newHeatLink(long i, int myMatrixIndex, TlinkedNode *myLink)
{
    double myConduction, myAdvection = 0., myLatent = 0.;

	if (myLink == NULL) return (false);
	else
	{
		long myLinkIndex = (*myLink).index;

        myConduction = 0.;
        myAdvection = 0.;
        myLatent = 0.;

        if (!myNode[myLinkIndex].isSurface)
        {
            myConduction = soilConduction(i, myLink);
            if (myStructure.computeHeatAdvective)
            {
                myAdvection = soilAdvection(i, myLink);
                if (myLink->heatFlux != NULL)
                    myLink->heatFlux->advective = myAdvection;
            }
            if (myStructure.computeHeatLatent)
            {
                myLatent = soilLatentIsothermal(i, myLink);
                if (myLink->heatFlux != NULL)
                    myLink->heatFlux->isothermLatent = myLatent;
            }
        }

		A[i][myMatrixIndex].index = myLinkIndex;
		A[i][myMatrixIndex].val = myConduction;

        C0[i] += myAdvection + myLatent;

		return (true);
	}
}

void computeHeatTimeStepErrors()
{
	double deltaHeatStorage = balanceCurrentTimeStep.storageHeat - balancePreviousTimeStep.storageHeat;
    balanceCurrentTimeStep.MBE_Heat = fabs(deltaHeatStorage - balanceCurrentTimeStep.sinkSourceHeat);

	if ((balanceCurrentTimeStep.sinkSourceHeat == 0.) && (deltaHeatStorage == 0.)) balanceCurrentTimeStep.MBR_Heat = 1.;
    else if (fabs(balanceCurrentTimeStep.sinkSourceHeat) < (balanceCurrentTimeStep.storageHeat / 1E5))
        balanceCurrentTimeStep.MBR_Heat = balanceCurrentTimeStep.storageHeat / (balancePreviousTimeStep.storageHeat + balanceCurrentTimeStep.sinkSourceHeat);
	else
        balanceCurrentTimeStep.MBR_Heat = deltaHeatStorage / balanceCurrentTimeStep.sinkSourceHeat;

}

double computeHeatStorage()
{ // [J]
	double myHeatStorage = 0.;
    for (long i = 1; i < myStructure.nrNodes; i++)
        myHeatStorage += criteria3D::getHeat(i);
	return myHeatStorage;
}

void computeBalanceHeat(float myTimeStep)
{
	double myHeatSinkSourceSum = 0.;

	// heat sink/source total
    for (long i = 1; i < myStructure.nrNodes; i++)
        myHeatSinkSourceSum += myNode[i].Qh * myTimeStep;

	balanceCurrentTimeStep.storageHeat = computeHeatStorage();
	balanceCurrentTimeStep.sinkSourceHeat = myHeatSinkSourceSum;

	computeHeatTimeStepErrors();
}

void initializeBalanceHeat()
{
	 balanceCurrentTimeStep.sinkSourceHeat = 0.;
	 balanceCurrentTimeStep.MBE_Heat = 0.;
	 balanceCurrentTimeStep.MBR_Heat = 0;
	 balancePreviousTimeStep.sinkSourceHeat = 0.;
	 balancePreviousTimeStep.MBE_Heat = 0.;
	 balancePreviousTimeStep.MBR_Heat = 0.;
	 balanceCurrentPeriod.sinkSourceHeat = 0.;
	 balanceCurrentPeriod.MBE_Heat = 0.;
	 balanceCurrentPeriod.MBR_Heat = 0.;
	 balanceWholePeriod.sinkSourceHeat = 0.;
	 balanceWholePeriod.MBE_Water = 0.;
	 balanceWholePeriod.MBR_Water = 0.;

     previousHeatMBRError = 1.;

	 balanceWholePeriod.storageHeat = computeHeatStorage();
	 balanceCurrentTimeStep.storageHeat = balanceWholePeriod.storageHeat;
	 balancePreviousTimeStep.storageHeat = balanceWholePeriod.storageHeat;
	 balanceCurrentPeriod.storageHeat = balanceWholePeriod.storageHeat;
}

void updateBalanceHeatWholePeriod()
{
    // aggiorna i flussi del bilancio balanceWholePeriod
    balanceWholePeriod.sinkSourceHeat  += balanceCurrentPeriod.sinkSourceHeat;

    double delta_storage_periodo = balanceCurrentTimeStep.storageHeat - balanceCurrentPeriod.storageHeat;
    double delta_storage_storico = balanceCurrentTimeStep.storageHeat - balanceWholePeriod.storageHeat;

    // calcola MBE_Heat e MBR_Heat
    balanceCurrentPeriod.MBE_Heat = fabs(delta_storage_periodo - balanceCurrentPeriod.sinkSourceHeat);
    if ((balanceWholePeriod.storageHeat == 0.) && (balanceWholePeriod.sinkSourceHeat == 0.)) balanceWholePeriod.MBR_Heat = 1.;
    else if (balanceCurrentTimeStep.storageHeat > fabs(balanceWholePeriod.sinkSourceHeat))
        balanceWholePeriod.MBR_Heat = balanceCurrentTimeStep.storageHeat / (balanceWholePeriod.storageHeat + balanceWholePeriod.sinkSourceHeat);
    else
        balanceWholePeriod.MBR_Heat = delta_storage_storico / balanceWholePeriod.sinkSourceHeat;

    // finito un balanceCurrentPeriod di calcolo - aggiorna storageHeat del balanceCurrentPeriod
    balanceCurrentPeriod.storageHeat = balanceCurrentTimeStep.storageHeat;
}


// [W m-2] heat flow between node myNode[myIndex] and link node myLink
void storeHeatFlows(long myIndex, TlinkedNode *myLink)
{
   if (myLink != NULL && ! myNode[myLink->index].isSurface)
   {
        long myLinkIndex = (*myLink).index;

        int j = 1;
        while ((j < myStructure.maxColumns) && (A[myIndex][j].index != NOLINK) && (A[myIndex][j].index != myLinkIndex)) j++;

        if (A[myIndex][j].index == myLinkIndex)
        {
            double myA = (A[myIndex][j].val * A[myIndex][0].val);
            myLink->heatFlux->diffusive = myA * (myNode[myIndex].T - myNode[myLinkIndex].T) * myParameters.heatWeightingFactor;
            myLink->heatFlux->diffusive += myA * (myNode[myIndex].oldT - myNode[myLinkIndex].oldT) * (1. - myParameters.heatWeightingFactor);
        }
        else
            myLink->heatFlux->diffusive = NODATA;
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
        if (myNode[i].up != NULL && myNode[i].up->heatFlux != NULL) storeHeatFlows(i, myNode[i].up);
        if (myNode[i].down != NULL && myNode[i].down->heatFlux != NULL) storeHeatFlows(i, myNode[i].down);
        for (short j = 0; j < myStructure.nrLateralLinks; j++)
            if (myNode[i].lateral[j] != NULL && myNode[i].lateral[j]->heatFlux != NULL) storeHeatFlows(i, myNode[i].lateral[j]);
    }
}

bool checkBalanceHeat(int myApprox)
{
	double myMBRError, myDeltaMBR;

	myMBRError = fabs(1. - balanceCurrentTimeStep.MBR_Heat);
    myDeltaMBR = fabs(myMBRError - previousHeatMBRError);
    previousHeatMBRError = myMBRError;

	if ((myMBRError < myParameters.MBRThreshold) || (myDeltaMBR < (myParameters.MBRThreshold / 10)))
	{
        updateBalanceHeat();
		return true;
	}
	else
	{
        if (myApprox == (myParameters.maxApproximationsNumber - 1)) updateBalanceHeat();
		return false;
	}
}

void restoreHeat()
{
	for (long i = 0; i < myStructure.nrNodes; i++)
		myNode[i].T = myNode[i].oldT;
	// ripristinare vecchi vapori al boundary?
}

bool computeHeatLoop(float myTimeStep)
{

	long i, j;
	bool isStepOK = false;
	double sumStiffness, sumFlow0, myDeltaTemp0;
    double myVolume;
	int myApprox = 0;

	do  {

		if (myApprox == 0)
            for (i = 1; i < myStructure.nrNodes; i++)
			{
				A[i][0].index = i;
				X[i] = myNode[i].T;
                C[i] = getSpecificHeat(i, myNode[i].H);
			}

        for (i = 1; i < myStructure.nrNodes; i++)
			{

                C0[i] = 0.;

                myVolume = myNode[i].VolumeS0;

				j = 1;
                if (newHeatLink(i, j, myNode[i].up)) j++;
                for (short l = 0; l < myStructure.nrLateralLinks; l++)
                    if (newHeatLink(i, j, myNode[i].lateral[l])) j++;
                if (newHeatLink(i, j, myNode[i].down)) j++;

                // closure
                while (j < myStructure.maxColumns)
                    A[i][j++].index = NOLINK;

				j = 1;
				sumStiffness = 0.;
				sumFlow0 = 0;
                myDeltaTemp0 = 0;

				while ((j < myStructure.maxColumns) && (A[i][j].index != NOLINK))
				{
					sumStiffness += (A[i][j].val * myParameters.heatWeightingFactor);
                    myDeltaTemp0 = myNode[A[i][j].index].oldT - myNode[i].oldT;
					sumFlow0 += A[i][j].val * (1. - myParameters.heatWeightingFactor) * myDeltaTemp0;
					A[i][j++].val *= -(myParameters.heatWeightingFactor);
				}

				// sommatoria sulla diagonale
                A[i][0].val =  getSpecificHeat(i, myNode[i].H) * myVolume / (double)myTimeStep + sumStiffness;

                b[i] = C[i] * myVolume * myNode[i].oldT / (double)myTimeStep + sumFlow0 + myNode[i].Qh + C0[i];

				// precondizionamento
				if (A[i][0].val > 0)
				{
					b[i] /= A[i][0].val;
					j = 1;
					while ((j < myStructure.maxColumns) && (A[i][j].index != NOLINK))
						A[i][j++].val /= A[i][0].val;
				}
			}

        GaussSeidelRelaxation(myApprox, myParameters.ResidualTolerance, PROCESS_HEAT);

        for (i = 1; i < myStructure.nrNodes; i++)
			myNode[i].T = X[i];

		// heat balance
		computeBalanceHeat(myTimeStep);
        isStepOK = checkBalanceHeat(myApprox);

		} while ((!isStepOK) && (++myApprox < myParameters.maxApproximationsNumber));

	// save old temperatures
    for (long n = 1; n < myStructure.nrNodes; n++)
		myNode[n].oldT = myNode[n].T;

	return (isStepOK);
}

bool computeHeat(float myTime)
// ----- Input -----------------------------------------------------------------
// myTime          [s] computation period
// -----------------------------------------------------------------------------
{
    // save old temperatures
    for (long n = 1; n < myStructure.nrNodes; n++)
		myNode[n].oldT = myNode[n].T;

	return (computeHeatLoop(myTime));
}


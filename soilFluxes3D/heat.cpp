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


#include <math.h>
#include <stdlib.h>

#include "../mathFunctions/commonConstants.h"
#include "../mathFunctions/physics.h"
#include "header/types.h"
#include "header/heat.h"
#include "header/soilPhysics.h"
#include "header/balance.h"
#include "header/water.h"
#include "header/solver.h"
#include "header/soilFluxes3D.h"
#include "header/boundary.h"

double CourantHeat, fluxCourant;

double computeHeatStorage()
{ // [J]
    double myHeatStorage = 0.;
    for (long i = 1; i < myStructure.nrNodes; i++)
        myHeatStorage += soilFluxes3D::getHeat(i, getHMean(i) - myNode[i].z);
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

    double referenceHeat = max_value(fabs(balanceCurrentTimeStep.sinkSourceHeat), balanceCurrentTimeStep.storageHeat * 1e-6);
    balanceCurrentTimeStep.heatMBR = 1. - balanceCurrentTimeStep.heatMBE / referenceHeat;
}

float readHeatFlux(TlinkedNode* myLink, int fluxType)
{
    if (myLink == NULL) return NODATA;
    if (myLink->linkedExtra == NULL) return NODATA;
    if (myLink->linkedExtra->heatFlux == NULL) return NODATA;

    if (myStructure.saveHeatFluxes == SAVE_HEATFLUXES_TOTAL)
        return myLink->linkedExtra->heatFlux->fluxes[HEATFLUX_TOTAL];
    else if (myStructure.saveHeatFluxes == SAVE_HEATFLUXES_ALL)
        return myLink->linkedExtra->heatFlux->fluxes[fluxType];
    else
        return NODATA;
}

void saveHeatFlux(TlinkedNode* myLink, int fluxType, double myValue)
{
    if (myLink == NULL) return;
    if (myLink->linkedExtra == NULL) return;
    if (myLink->linkedExtra->heatFlux == NULL) return;

    if (myStructure.saveHeatFluxes == SAVE_HEATFLUXES_TOTAL)
    {
        if (myLink->linkedExtra->heatFlux->fluxes[HEATFLUX_TOTAL] == NODATA)
            myLink->linkedExtra->heatFlux->fluxes[HEATFLUX_TOTAL] = float(myValue);
        else
            myLink->linkedExtra->heatFlux->fluxes[HEATFLUX_TOTAL] += float(myValue);
    }
    else if (myStructure.saveHeatFluxes == SAVE_HEATFLUXES_ALL)
        myLink->linkedExtra->heatFlux->fluxes[fluxType] = float(myValue);

}

void saveDiffusiveHeaFlux(long myIndex, TlinkedNode *myLink)
// [W m-2] heat flow between node myNode[myIndex] and link node myLink
{
   if (myLink != NULL && ! myNode[myLink->index].isSurface)
   {
        long myLinkIndex = (*myLink).index;
        double myValue, myA;

        int j = 1;
        while ((j < myStructure.maxNrColumns) && (A[myIndex][j].index != NOLINK) && (A[myIndex][j].index != myLinkIndex)) j++;

        if (A[myIndex][j].index == myLinkIndex)
        {
            myA = (A[myIndex][j].val * A[myIndex][0].val);
            myValue = myA * (myNode[myIndex].extra->Heat->T - myNode[myLinkIndex].extra->Heat->T) * myParameters.heatWeightingFactor;
            myValue += myA * (myNode[myIndex].extra->Heat->oldT - myNode[myLinkIndex].extra->Heat->oldT) * (1. - myParameters.heatWeightingFactor);
            saveHeatFlux(myLink, HEATFLUX_DIFFUSIVE, myValue);
        }       
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
                saveDiffusiveHeaFlux(i, &(myNode[i].up));

        if (myNode[i].down.index != NOLINK)
            if (myNode[i].down.linkedExtra->heatFlux != NULL)
                saveDiffusiveHeaFlux(i, &(myNode[i].down));

        for (short j = 0; j < myStructure.nrLateralLinks; j++)
            if (myNode[i].lateral[j].index != NOLINK)
                if (myNode[i].lateral[j].linkedExtra->heatFlux != NULL)
                    saveDiffusiveHeaFlux(i, &(myNode[i].lateral[j]));
    }
}

bool heatBalance(double timeStep)
{
    computeHeatBalance(timeStep);
    return ((fabs(1.-balanceCurrentTimeStep.heatMBR) < myParameters.MBRThreshold));
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

     balanceCurrentTimeStep.heatMBR = 1.;
     balanceCurrentPeriod.heatMBR = 1.;
     balanceWholePeriod.heatMBR = 1.;

     balanceWholePeriod.storageHeat = computeHeatStorage();
     balanceCurrentTimeStep.storageHeat = balanceWholePeriod.storageHeat;
     balancePreviousTimeStep.storageHeat = balanceWholePeriod.storageHeat;
     balanceCurrentPeriod.storageHeat = balanceWholePeriod.storageHeat;

     /*! initialize link flow */
     if (myStructure.saveHeatFluxes == SAVE_HEATFLUXES_TOTAL)
         for (long n = 0; n < myStructure.nrNodes; n++)
         {
             saveHeatFlux(&(myNode[n].up), HEATFLUX_TOTAL, 0.);
             saveHeatFlux(&(myNode[n].down), HEATFLUX_TOTAL, 0.);
             for (short i = 0; i < myStructure.nrLateralLinks; i++)
                saveHeatFlux(&(myNode[n].lateral[i]), HEATFLUX_TOTAL, 0.);
         }

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
}


/*!
 * \brief [m3 m-3] vapor volumetric water equivalent
 * \param [m] h
 * \param [K] temperature
 * \param i
 * \return result
 */
double VaporThetaV(double h, double T, long i)
{
    double theta = theta_from_sign_Psi(h, i);
    double vaporConc = VaporFromPsiTemp(h, T);
    return (vaporConc / WATER_DENSITY * (myNode[i].Soil->Theta_s - theta));
}


/*!
 * \brief [m2 s-1] binary vapor diffusivity
 * (Do) in Bittelli (2008) or vapor diffusion coefficient in air (Dva) in Monteith (1973)
 * \param myPressure
 * \param myTemperature
 * \return result
 */
double VaporBinaryDiffusivity(double myTemperature)
{	return (VAPOR_DIFFUSIVITY0 * pow(myTemperature / ZEROCELSIUS, 2)); }

/*!
 * \brief [m2 s-1] vapor diffusivity
 * \param i
 * \param myT
 * \return result
 */
double SoilVaporDiffusivity(double ThetaS, double Theta, double myT)
{
	double binaryDiffusivity;	// [m2 s-1]
	double airFilledPorosity;	// [m3 m-3]
    double const beta = 0.66;	// [] Penman 1940
    double const emme = 1;      // [] idem

    binaryDiffusivity = VaporBinaryDiffusivity(myT);
    airFilledPorosity = ThetaS - Theta;

    return (binaryDiffusivity  * beta * pow(airFilledPorosity, emme));
}

/*!
 * \brief [] soil relative humidity
 * \param [m] h
 * \param [K] myT
 * \return result
 */
double SoilRelativeHumidity(double h, double myT)
{	return (exp(MH2O * h * GRAVITY / (R_GAS * myT))); }

/*!
 * \brief [kg s m-3] isothermal vapor conductivity
 * \param i
 * \param h
 * \param myT
 * \return result
 */
double IsothermalVaporConductivity(long i, double h, double myT)
{
    double theta = theta_from_sign_Psi(h, i);
    double Dv = SoilVaporDiffusivity(myNode[i].Soil->Theta_s, theta, myT);
    double vapor = VaporFromPsiTemp(h, myT);
    return (Dv * vapor * MH2O / (R_GAS * myT));
}

/*!
 * \brief [J m-3 K-1] volumetric specific heat
 * Soil specific heat according to Campbell "Soil physics with basic" pp 31-32
 * \param i
 * \return result
 */
double SoilSpecificHeat(long i)
{   /* Soil specific heat according to Campbell "Soil physics with basic" pp 31-32
	INPUT:
	bulk density [Mg m-3]
	volumetric water content [m3 m-3]
	OUTPUT:
	volumetric specific heat [J m-3 K-1]
	*/

    return (estimateBulkDensity(i) / 2.65 * HEAT_CAPACITY_MINERAL + theta_from_Se(myNode[i].Se ,i) * HEAT_CAPACITY_WATER);
}

/*!
 * \brief [J m-3 K-1] volumetric heat capacity
 * \param i
 * \param h
 * \param T
 * \return result
 */
double SoilHeatCapacity(long i, double h, double T)
{
    double theta = theta_from_sign_Psi(h, i);
    double thetaV = VaporThetaV(h, T, i);
    double bulkDensity = estimateBulkDensity(i);
    return bulkDensity / 2.65 * HEAT_CAPACITY_MINERAL +
            theta * HEAT_CAPACITY_WATER +
            thetaV * HEAT_CAPACITY_AIR;
}

/*!
 * \brief [] water return flow factor
 * Campbell 1994
 * \param myTheta
 * \param myClayFraction
 * \param myTemperature
 * \return result
 */
double WaterReturnFlowFactor(double myTheta, double myClayFraction, double myTemperature)
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
 * \brief compute vapor concentration from matric potential and temperature
 * \param Psi [J kg-1]
 * \param T [K]
 * \return vapor concentration [kg m-3]
 */
double VaporFromPsiTemp(double h, double T)
{
    double mySatVapPressure, mySatVapConcentration, myRelHum;

    mySatVapPressure = SaturationVaporPressure(T - ZEROCELSIUS);
    mySatVapConcentration = VaporConcentrationFromPressure(mySatVapPressure, T);
    myRelHum = SoilRelativeHumidity(h, T);

    return mySatVapConcentration * myRelHum;

}

/*!
 * \brief [kg m-1 s-1 K-1] thermal vapor conductivity
 * \param i
 * \param temperature (K)
 * \param h (m)
 * \return result
 */
double ThermalVaporConductivity(long i, double temperature, double h)
{
    double myPressure;				// [Pa] total air pressure
	double Dv;						// [m2 s-1] vapor diffusivity
	double svp;						// [Pa] saturation vapor pressure
	double slopesvp;				// [Pa K-1] slope of saturation vapor pressure curve
    double slopesvc;                // [kg m-3 K-1] slope of saturation vapor concentration
    double myVapor;					// [kg m-3] vapor concentration
	double myVaporPressure;			// [Pa] vapor partial pressure
	double hr;						// [] relative humidity
    double tempCelsius;             // [°C] temperature
    double theta;                   // [m3 m-3] volumetric water content
    double eta;                     // [] enhancement factor
    double satDegree;               // [] degree of saturation

    tempCelsius = temperature - ZEROCELSIUS;

    myPressure = PressureFromAltitude(myNode[i].z);

    theta = theta_from_sign_Psi(h, i);

	// vapor diffusivity
    Dv = SoilVaporDiffusivity(myNode[i].Soil->Theta_s, theta, temperature);

	// slope of saturation vapor pressure
    svp = SaturationVaporPressure(tempCelsius);
    slopesvp = SaturationSlope(tempCelsius, svp);

    // slope of saturation vapor concentration
    slopesvc = slopesvp * MH2O * AirMolarDensity(myPressure, temperature) / myPressure;

	// relative humidity
    myVapor = VaporFromPsiTemp(h, temperature);
    myVaporPressure = VaporPressureFromConcentration(myVapor, temperature);
	hr = myVaporPressure / svp;

    // enhancement factor (Cass et al. 1984)
    satDegree = theta / myNode[i].Soil->Theta_s;
    eta = 9.5 + 3. * satDegree - 8.5 * exp(-pow((1. + 2.6/sqrt(myNode[i].Soil->clay))*satDegree, 4));

    return (eta * Dv * slopesvc * hr);

}

/*!
 * \brief [W m-1 K-1] air thermal conductivity
 * \param i
 * \param T: temperature [K]
 * \param h: water matric potential [m]
 * \return result
 */
double AirHeatConductivity(long i, double T, double h)
{
    double Kda;						// [W m-1 K-1] thermal conductivity of dry air
    double Ka;						// [W m-1 K-1] thermal conductivity of air
    double myKvt;                   // [kg m-1 s-1 K-1] non isothermal vapor conductivity
    double myLambda;				// [J kg-1] latent heat of vaporization
    double myTCelsiusMean;          // [degC]
    double coeff;                   // [J kg-1]

    // dry air conductivity
    myTCelsiusMean = T - ZEROCELSIUS;
	Kda = 0.024 + 0.0000773 * myTCelsiusMean - 0.000000026 * myTCelsiusMean * myTCelsiusMean;

    Ka = Kda;

    if (myStructure.computeWater)
    {
        myLambda = LatentHeatVaporization(T - ZEROCELSIUS);

        coeff= myLambda;

        // advective heat flux associated with thermal vapor flux
        coeff += T * HEAT_CAPACITY_AIR / WATER_DENSITY;

        myKvt = ThermalVaporConductivity(i, T, h);
        Ka += coeff * myKvt;
    }

	return (Ka);
}

/*!
 * \brief [W m-1 K-1] soil thermal conductivity
 * according to Campbell et al. Soil Sci. 158:307-313
 * \param i
 * \param T: temperature [K]
 * \param h: water matric potential [m]
 * \return result
 */
double SoilHeatConductivity(long i, double T, double h)
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

    myTCelsiusMean = T - ZEROCELSIUS;

	// water conductivity
	Kw = 0.554 + 0.0024 * myTCelsiusMean - 0.00000987 * myTCelsiusMean * myTCelsiusMean;

	// air conductivity
    Ka = AirHeatConductivity(i, T, h);

    xw = theta_from_sign_Psi(h, i);

    fw = WaterReturnFlowFactor(xw, myNode[i].Soil->clay, myTCelsiusMean + ZEROCELSIUS);
	Kf = Ka + fw * (Kw - Ka);

	gc = 1. - 2. * ga;

    ea = (2. / (1 + (Ka / Kf - 1) * ga) + 1 / (1 + (Ka / Kf - 1) * gc)) / 3.;
	ew = (2. / (1 + (Kw / Kf - 1) * ga) + 1 / (1 + (Kw / Kf - 1) * gc)) / 3.;
    es = (2. / (1 + (KH_mineral / Kf - 1) * ga) + 1 / (1 + (KH_mineral / Kf - 1) * gc)) / 3.;

	xs = 1. - myNode[i].Soil->Theta_s;
	xa = myNode[i].Soil->Theta_s - xw;

    myConductivity = (xw * ew * Kw + xa * ea * Ka + xs * es * KH_mineral) / (ew * xw + ea * xa + es * xs);
    return myConductivity;

}

/*!
 * \brief isothermal latent heat flux
 * \param i
 * \param myLink
 * \return isothermal latent heat flux + advective associated [W]
 */
double SoilLatentIsothermal(long i, TlinkedNode *myLink)
{
    double lambda, lambdaLink, avgLambda;       // [J kg-1] latent heat of vaporization
    double myKvi;								// [kg s m-3] vapor conductivity
    double psi, psiLink;                        // [J kg-1] water matric potential
    double deltaPsi;							// [J kg-1] water potential difference
    double myLatentFlux;						// [J m-1 s-1] latent heat flow
    double coeff;                               // [J kg-1]
    double Kvi, KviLink;                        // [kg m-3 s-1] isothermal vapor conductivity
    double havg, havglink;                      // [m] average matric potentials

    long j = (*myLink).index;

    lambda = LatentHeatVaporization(myNode[i].extra->Heat->T - ZEROCELSIUS);
    lambdaLink = LatentHeatVaporization(myNode[j].extra->Heat->T - ZEROCELSIUS);
    avgLambda = computeMean(lambda, lambdaLink);

    havg = computeMean(myNode[i].H, myNode[i].oldH) - myNode[i].z;
    havglink = computeMean(myNode[j].H, myNode[j].oldH) - myNode[j].z;

    Kvi = IsothermalVaporConductivity(i, havg, myNode[i].extra->Heat->T);
    KviLink = IsothermalVaporConductivity(j, havglink, myNode[j].extra->Heat->T);
    myKvi = computeMean(Kvi, KviLink);

    psi = havg * GRAVITY;
    psiLink = havglink * GRAVITY;

    deltaPsi = (psiLink - psi);

    coeff = avgLambda;

    // advective heat flux associated with isothermal vapor flux
    if (myStructure.computeWater)
        coeff += myNode[i].extra->Heat->T * HEAT_CAPACITY_AIR / WATER_DENSITY;

    myLatentFlux = coeff * myKvi * deltaPsi / distance(i, j) * myLink->area;

    return (myLatentFlux);
}

/*!
 * \brief advective liquid water heat flux
 * \param i
 * \param myLink
 * \return advective liquid water heat flux [W]
 */
double SoilHeatAdvection(long i, TlinkedNode *myLink)
{
    double Tadv;
	double myWaterFlow;

    myWaterFlow = (*myLink).linkedExtra->heatFlux->waterFlux;

    if (myWaterFlow < 0.)
        Tadv = myNode[i].extra->Heat->T;
    else
        Tadv = myNode[myLink->index].extra->Heat->T;

    fluxCourant += HEAT_CAPACITY_WATER * myWaterFlow;

    return (fluxCourant * Tadv);
}


double SoilConduction(long i, TlinkedNode *myLink)
{
	double myConductivity, linkConductivity, meanKh;
    double zeta;
    double hAvg, hLinkAvg;

    long j = (*myLink).index;
    double myDistance = distance(i, j);

    zeta = myLink->area / myDistance;

    hAvg = computeMean(myNode[i].H, myNode[i].oldH) - myNode[i].z;
    hLinkAvg = computeMean(myNode[j].H, myNode[j].oldH) - myNode[j].z;

    myConductivity = SoilHeatConductivity(i, myNode[i].extra->Heat->T, hAvg);
    linkConductivity = SoilHeatConductivity(j, myNode[j].extra->Heat->T, hLinkAvg);
    meanKh = computeMean(myConductivity, linkConductivity);

    return (zeta * meanKh);
}

bool computeHeatFlux(long i, int myMatrixIndex, TlinkedNode *myLink, double deltaT)
{
    if (myLink == NULL) return false;
    if ((*myLink).index == NOLINK) return false;

    long myLinkIndex = (*myLink).index;
    double myConduction, myAdvection, myLatent;
    double nodeDistance;

    myConduction = 0.;
    myAdvection = 0.;
    myLatent = 0.;
    fluxCourant = 0.;

    if (!myNode[myLinkIndex].isSurface)
    {
        myConduction = SoilConduction(i, myLink);
        if (myStructure.computeWater)
        {
            myLatent = SoilLatentIsothermal(i, myLink);
            saveHeatFlux(myLink, HEATFLUX_LATENT_ISOTHERMAL, myLatent);

            myAdvection = SoilHeatAdvection(i, myLink);
            saveHeatFlux(myLink, HEATFLUX_ADVECTIVE, myAdvection);
        }
    }

    A[i][myMatrixIndex].index = myLinkIndex;
    A[i][myMatrixIndex].val = myConduction;

    C0[i] += myAdvection + myLatent;

    if (fluxCourant != 0)
    {
        nodeDistance = distance(i, myLinkIndex);
        CourantHeat = max_value(CourantHeat, fabs(fluxCourant) * deltaT / (C[i] * nodeDistance));
    }

    return (true);
}

void saveWaterFluxes()
{
    for (long i = 0; i < myStructure.nrNodes; i++)
        {
            if (&myNode[i].up != NULL)
                if (myNode[i].up.linkedExtra != NULL)
                    myNode[i].up.linkedExtra->heatFlux->waterFlux = float(getWaterFlux(i, &myNode[i].up));

            if (&myNode[i].down != NULL)
                if (myNode[i].down.linkedExtra != NULL)
                    myNode[i].down.linkedExtra->heatFlux->waterFlux = float(getWaterFlux(i, &myNode[i].down));


            for (short j = 0; j < myStructure.nrLateralLinks; j++)
                if (&myNode[i].lateral[j] != NULL)
                    if (myNode[i].lateral[j].linkedExtra != NULL)
                        myNode[i].lateral[j].linkedExtra->heatFlux->waterFlux = float(getWaterFlux(i, &myNode[i].lateral[j]));

        }
}

bool HeatComputation(double myTimeStep)
{

	long i, j;
    double sum = 0;
    double sumFlow0 = 0;
    double myDeltaTemp0;
    double avgh;
    double heatCapacityVar;
    double dtheta, dthetav;

    CourantHeat = 0.;

    for (i = 1; i < myStructure.nrNodes; i++)
    {
        A[i][0].index = i;
        X[i] = myNode[i].extra->Heat->T;
        myNode[i].extra->Heat->oldT = myNode[i].extra->Heat->T;

        avgh = computeMean(myNode[i].oldH, myNode[i].H) - myNode[i].z;
        C[i] = SoilHeatCapacity(i, avgh, myNode[i].extra->Heat->T) * myNode[i].volume_area;
    }

    for (i = 1; i < myStructure.nrNodes; i++)
    {
        C0[i] = 0.;

        // compute heat capacity temporal variation
        // due to changes in water and vapor
        dtheta = theta_from_sign_Psi(myNode[i].H - myNode[i].z, i) -
                theta_from_sign_Psi(myNode[i].oldH - myNode[i].z, i);

        dthetav = VaporThetaV(myNode[i].H - myNode[i].z, myNode[i].extra->Heat->T, i) -
                VaporThetaV(myNode[i].oldH - myNode[i].z, myNode[i].extra->Heat->oldT, i);

        heatCapacityVar = dtheta * HEAT_CAPACITY_WATER * myNode[i].extra->Heat->T;
        heatCapacityVar += dthetav * HEAT_CAPACITY_AIR * myNode[i].extra->Heat->T;
        heatCapacityVar += dthetav * LatentHeatVaporization(myNode[i].extra->Heat->T - ZEROCELSIUS) * WATER_DENSITY;
        heatCapacityVar *= myNode[i].volume_area;

        j = 1;
        if (computeHeatFlux(i, j, &(myNode[i].up), myTimeStep)) j++;
        for (short l = 0; l < myStructure.nrLateralLinks; l++)
            if (computeHeatFlux(i, j, &(myNode[i].lateral[l]), myTimeStep)) j++;
        if (computeHeatFlux(i, j, &(myNode[i].down), myTimeStep)) j++;

        // closure
        while (j < myStructure.maxNrColumns)
            A[i][j++].index = NOLINK;

        j = 1;
        sum = 0.;
        sumFlow0 = 0;
        myDeltaTemp0 = 0;

        while ((j < myStructure.maxNrColumns) && (A[i][j].index != NOLINK))
        {
            sum += A[i][j].val * myParameters.heatWeightingFactor;
            myDeltaTemp0 = myNode[A[i][j].index].extra->Heat->oldT - myNode[i].extra->Heat->oldT;
            sumFlow0 += A[i][j].val * (1. - myParameters.heatWeightingFactor) * myDeltaTemp0;
            A[i][j++].val *= -(myParameters.heatWeightingFactor);
        }

        /*! sum of diagonal elements */
        avgh = computeMean(myNode[i].oldH, myNode[i].H) - myNode[i].z;
        A[i][0].val = SoilHeatCapacity(i, avgh, myNode[i].extra->Heat->T) * myNode[i].volume_area / myTimeStep + sum;

        /*! b vector (constant terms) */
        b[i] = C[i] * myNode[i].extra->Heat->oldT / myTimeStep - heatCapacityVar / myTimeStep + myNode[i].extra->Heat->Qh + C0[i] + sumFlow0;

        // precondizionamento
        if (A[i][0].val > 0)
        {
            b[i] /= A[i][0].val;
            j = 1;
            while ((j < myStructure.maxNrColumns) && (A[i][j].index != NOLINK))
                A[i][j++].val /= A[i][0].val;
        }
    }

    if (CourantHeat > 1.0)
        if (myTimeStep > myParameters.delta_t_min)
        {
            halveTimeStep();
            setForcedHalvedTime(true);
            return (false);
        }

    GaussSeidelRelaxation(0, myParameters.ResidualTolerance, PROCESS_HEAT);

    for (i = 1; i < myStructure.nrNodes; i++)
        myNode[i].extra->Heat->T = X[i];

    heatBalance(myTimeStep);
    updateBalanceHeat();

	// save old temperatures
    for (long n = 1; n < myStructure.nrNodes; n++)
        myNode[n].extra->Heat->oldT = myNode[n].extra->Heat->T;

    return (true);
}

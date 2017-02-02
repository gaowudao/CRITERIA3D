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
#include "header/physics.h"


double PressureFromAltitude(double myHeight)
{	/* from Allen et al., 1994.
    An update for the calculation of reference evapotranspiration.
    ICID Bulletin 43: 65
    INPUT:
    altitude in meters above the sea level [m]
    OUTPUT:
    atmospheric pressure (Pa)
    */

    double myPressure;

    myPressure = p0 * pow(1 + myHeight * LapseRateMoistAir / Tp0, - GRAVITY / (LapseRateMoistAir * RDryAir));
    return myPressure;
}

double MolarDensity(double myPressure, double myT)
// Boyle-Charles law
// mol m-3
{ return 44.65 * (myPressure / p0) * (ZEROCELSIUS / myT);}

double VolumetricLatentHeatVaporization(double myPressure, double myT)
// [J m-3] latent heat of vaporization
{
    double rhoair = MolarDensity(myPressure, myT); // [mol m-3] molar density of air
    return (rhoair * (45144. - 48. * (myT - ZEROCELSIUS)));	// Campbell 1994
}

double VaporPressureFromConcentration(double myConcentration, double myT)
// [Pa] convert vapor partial pressure from concentration in kg m-3
{	return (myConcentration * R * myT / MH2O);}

double VaporConcentrationFromPressure(double myPressure, double myT)
// [kg m-3] convert vapor concentration from pressure (Pa) and temperature (K)
{ return (myPressure * MH2O / (R * myT));}

double AirVolumetricSpecificHeat(double myPressure, double myT)
{ // (J m-3 K-1) volumetric specific heat of air

    double myMolarDensity = MolarDensity(myPressure, myT); // mol m-3
    double mySpHeat = (CpmH20 * myMolarDensity);
    return (mySpHeat);
}

double SaturationVaporPressure(double myTCelsius)
// [Pa] saturation vapor pressure
{	return 611 * exp(17.502 * myTCelsius / (myTCelsius + 240.97));}

double SaturationSlope(double myTCelsius, double mySatVapPressure)
// [Pa K-1] slope of saturation vapor pressure curve
// mySatVapPressure		saturation vapor pressure (Pa)
{	return (17.502 * 240.97 * mySatVapPressure / ((240.97 + myTCelsius) * (240.97 + myTCelsius)));}

double getAirVaporDeficit(double myT, double myVapor)
{
    double myVaporPressure = SaturationVaporPressure(myT - ZEROCELSIUS);
    double mySatVapor = VaporConcentrationFromPressure(myVaporPressure, myT);
    return (mySatVapor - myVapor);
}

double LatentHeatVaporization(double myTCelsius)
// [J kg-1] latent heat of vaporization
{	return (2501000. - 2361. * myTCelsius); }

double AirDensity(double myTemperature, double myRelativeHumidity)
{
    double totalPressure;       // air total pressure (Pa)
    double vaporPressure;		// vapor partial pressure (Pa)
    double satVaporPressure;    // saturation vapor partial pressure (Pa)
    double myDensity;			// air density (kg m-3)

    satVaporPressure = SaturationVaporPressure(myTemperature - ZEROCELSIUS);
    vaporPressure = (satVaporPressure * myRelativeHumidity);
    totalPressure = 101300;

    myDensity = (totalPressure / (RDryAir * myTemperature)) * (1 - (0.378 * vaporPressure / totalPressure));

    return (myDensity);
}

double AerodynamicConductance(double myHeight,
                              double mySoilSurfaceTemperature,
                              double myRoughnessHeight,
                              double myAirTemperature,
                              double myWindSpeed)
{
    // windSpeed						(m/s) wind speed measured at height referenceHeight
    // zRef								(m) reference height
    // cropHeight						(m) crop height (height of roughness element)

    double K;							// (m s-1) aerodynamic conductance
    double psiM, psiH;					// () stability correction factors for momentum and for heat
    const double VONKARMAN = 0.4;		// () Von Karman constant
    double uStar;						// (m s-1) friction velocity
    double zeroPlane;					// (m) zero place displacement
    double roughnessMomentum;           // () surface roughness parameter for momentum
    double roughnessHeat;				// () surface roughness parameter for heat
    // myAirTemperature;                // (K) air temperature
    // myHeight;						// (m) reference measurement height
    double Sp;                          // () stability parameter

    zeroPlane = 0.77 * myRoughnessHeight;
    roughnessMomentum = 0.13 * myRoughnessHeight;
    roughnessHeat = 0.2 * roughnessMomentum;

    psiM = 0.;
    psiH = 0.;

    uStar = VONKARMAN * myWindSpeed / (log((myHeight - zeroPlane) / roughnessMomentum));
    K = VONKARMAN * uStar / (log((myHeight - zeroPlane) / roughnessHeat));

    for (short i = 1; i <= 3; i++)
    {
        uStar = VONKARMAN * myWindSpeed / (log((myHeight - zeroPlane) / roughnessMomentum) + psiM);
        K = VONKARMAN * uStar / (log((myHeight - zeroPlane) / roughnessHeat) + psiH);

        Sp = -VONKARMAN * myHeight * GRAVITY * K * (mySoilSurfaceTemperature - myAirTemperature) / (myAirTemperature * (pow(uStar, 3)));
        if (Sp > 0)
        {// stability
            psiH = 4.7 * Sp;
            psiM = psiH;
        }
        else
        {// unstability
            psiH = -2 * log((1 + sqrt(1 - 16 * Sp)) / 2);
            psiM = 0.6 * psiH;
        }
    }

    return (K);

}

double AerodynamicConductanceOpenwater(double myHeight, double mySurface, double myAirTemperature,
                                      double myRelativeHumidity, double myWindSpeed)
{	// aerodynamic resistance for an open water surface (s m-1)
    // McJannet et al 2008

    // controllare unita' di misura!!

    double myPressure;		// Pa
    double myT;				// K
    double myVolSpecHeat;	// J m-3 K-1
    double mySpecHeat;		// J kg-1 K-1
    double myPsycro;		// Pa K-1
    double myLambda;		// (J kg-1) latent heat of vaporization
    double windFunction;	// (MJ m-2 d-1 kPa-1) wind function (Sweers 1976)
    double myDensity;		// air density (kg m-3)

    myPressure = PressureFromAltitude(myHeight);
    myT = myAirTemperature;
    myDensity = AirDensity(myAirTemperature, myRelativeHumidity);
    myVolSpecHeat = AirVolumetricSpecificHeat(myPressure, myT);
    mySpecHeat = myVolSpecHeat / myDensity;
    myLambda = LatentHeatVaporization(myT - 273.16);

    myPsycro = (mySpecHeat * myPressure) / (RatioMWAir * myLambda);
    windFunction = pow((5. / mySurface), 0.05) * (3.8 + 1.57 * myWindSpeed);

    // conversion to SI unit
    return (1. / (myVolSpecHeat / (myPsycro * windFunction / 86.4)));
}

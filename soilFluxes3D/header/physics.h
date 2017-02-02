#ifndef GRAVITY
    // [m s-2] GRAVITY acceleration
    #define GRAVITY		9.80665
#endif

#ifndef ZEROCELSIUS
    // [K]
    #define	ZEROCELSIUS   273.15
#endif

// [kg mol-1] molecular mass of water
#define	MH2O	0.018
// [kg mol-1] mass of molecular oxygen (O2)
#define	MO2		0.032
// [kg mol-1] mass of molecular nitrogen (N2)
#define	MN2		0.028

#ifndef R
    // [J K-1 mol-1] universal gas constant
    #define	R 8.31447215
#endif

#ifndef STEFANBOLTZMANN
    // [W m-2 K-4] Stefan-Boltzmann constant
    #define STEFANBOLTZMANN  5.670400E-8
#endif

const double	VaporDiffusivity0 =		0.0000212;			// [m2 s-1] vapor diffusivity at standard conditions
const double	minimumSurfaceHeight =	0.02;				// [m]
const double	RDryAir =				287.058;			// [J kg-1 K-1] specific gas constant for dry air
const double	RWetAir =				461.51;				// [J kg-1 K-1] specific gas constant for wet air
const double	LapseRateMoistAir =		0.0065;				// [K m-1] constant lapse rate of moist air
const double	p0 =					101300;				// [Pa] standard atmospheric pressure at sea level
const double	Tp0 =					293.16;				// [K] temperature at reference pressure level (p0)
const double	DryAirMolarMass =		28.97;				// [kg mol-1] standard molar mass of dry air
const double	VolSpecHeatMineral =	2300000;            // [J m-3 K-1] volumetric specific heat of mineral (and organic) soils
const double	VolSpecHeatH2O =		4182000;			// [J m-3 K-1] volumetric specific heat of water
const double	CpmH20 =				29.3;				// [J mol-1 K-1] molar specific heat of air at constant pressure
const double	kH2O =					0.57;				// [W m-1 K-1] thermal conductivity of water
const double	kMineral =				2.5;				// [W m-1 K-1] average thermal conductivity of soil minerals (no quartz)
const double	RatioMWAir =			0.622;				// [] ratio molecular weight of water vapour/dry air

double PressureFromAltitude(double myHeight);
double AirMolarMass(double myVaporPressure);
double AirVolumetricSpecificHeat(double myPressure, double myT);
double SaturationVaporPressure(double myTCelsius);
double SaturationSlope(double myTCelsius, double mySatVapPressure);
double VaporConcentrationFromPressure(double myPressure, double myT);
double VaporPressureFromConcentration(double myConcentration, double myT);
double getAirVaporDeficit(double myT, double myVapor);
double VolumetricLatentHeatVaporization(double myPressure, double myT);
double LatentHeatVaporization(double myTCelsius);
double AerodynamicConductance(double myHeight, double mySoilSurfaceTemperature, double myRoughnessHeight,
                              double myAirTemperature, double myWindSpeed);
double AerodynamicConductanceOpenwater(double myHeight, double mySurface, double myAirTemperature,
                                      double myRelativeHumidity, double myWindSpeed);

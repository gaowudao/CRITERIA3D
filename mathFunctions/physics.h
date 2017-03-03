#ifndef PHYSICS_H
#define PHYSICS_H

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
double Psychro(double myPressure, double myTemp);
double AerodynamicConductance(double myHeight, double mySoilSurfaceTemperature, double myRoughnessHeight,
                              double myAirTemperature, double myWindSpeed);
double AerodynamicConductanceOpenwater(double myHeight, double mySurface, double myAirTemperature,
                                      double myRelativeHumidity, double myWindSpeed);

#endif // PHYSICS_H

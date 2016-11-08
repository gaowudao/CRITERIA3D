#ifndef METEO_H
#define METEO_H

    #ifndef CRIT3DDATE_H
        #include "crit3dDate.h"
    #endif

    #define ALBEDO_WATER 0.05
    #define ALBEDO_SOIL 0.15
    #define ALBEDO_CROP 0.25
    #define ALBEDO_CROP_REFERENCE 0.23

    enum meteoVariable {airTemperature, airTemperatureMin, airTemperatureMax, airTemperatureMean,
                        precipitation, precipitationSum, airHumidity, airHumidityMin, airHumidityMax, airHumidityMean, airDewTemperature,
                        globalIrradiance, directIrradiance, diffuseIrradiance, reflectedIrradiance,
                        globalRadiation, directRadiation, diffuseRadiation, reflectedRadiation,
                        windIntensity, windIntensityMean, windDirection, windDirectionPrevailing,
                        wetnessDuration, atmPressure, atmTransmissivity, potentialEvapotranspiration,
                        realEvaporation, waterTableDepth, noMeteoVar};

    enum surfaceType   {SurfaceTypeWater, SurfaceTypeSoil, SurfaceTypeCrop};

    class Crit3DClimateParameters {
    public:
        Crit3DClimateParameters();
        float tminLapseRate[12];
        float tmaxLapseRate[12];
        float tDewMinLapseRate[12];
        float tDewMaxLapseRate[12];

        float getClimateLapseRate(meteoVariable myVar, Crit3DDate* myDate, int myHour);
    };

    float relHumFromTdew(float dewT, float airT);
    float tDewFromRelHum(float rhAir, float airT);
    double ET0_Penman_hourly(double heigth, double normalizedTransmissivity, double globalSWRadiation,
                    double airTemp, double airHum, double windSpeed10);
    double ET0_Penman_daily(int myDOY, float myLatitude, float myPressure,
                           float myTmin, float myTmax, float myTminDayAfter, float myUmed, float myVmed10, float mySWGlobRad);
    double ET0_Hargreaves(double KT, double myLat, int myDoy, double tmax, double tmin);

#endif // METEO_H

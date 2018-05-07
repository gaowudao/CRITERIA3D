#ifndef METEO_H
#define METEO_H

#include <map>

    #ifndef CRIT3DDATE_H
        #include "crit3dDate.h"
    #endif
    #ifndef CRIT3DCOLOR_H
        #include "color.h"
    #endif

    #define TABLE_METEO_POINTS "point_properties"
    #define FIELD_METEO_POINT "id_point"
    #define FIELD_METEO_DATETIME "date"
    #define FIELD_METEO_VARIABLE "id_variable"
    #define FIELD_METEO_VARIABLE_NAME "variable"

enum meteoVariable {airTemperature, dailyAirTemperatureMin, dailyAirTemperatureMax, dailyAirTemperatureAvg,
                        precipitation, dailyPrecipitation,
                        airHumidity, dailyAirHumidityMin, dailyAirHumidityMax, dailyAirHumidityAvg, airDewTemperature,
                        globalIrradiance, directIrradiance, diffuseIrradiance, reflectedIrradiance, atmTransmissivity,
                        dailyGlobalRadiation, dailyDirectRadiation, dailyDiffuseRadiation, dailyReflectedRadiation,
                        windIntensity, dailyWindIntensityAvg, dailyWindIntensityMax, windDirection, dailyWindDirectionPrevailing,
                        leafWetness, dailyLeafWetness,
                        atmPressure,
                        referenceEvapotranspiration, dailyReferenceEvapotranspiration, actualEvaporation,
                        dailyWaterTableDepth, noMeteoTerrain, noMeteoVar};

    const std::map<std::string, meteoVariable> MapDailyMeteoVar = {
      { "DAILY_TMIN", dailyAirTemperatureMin },
      { "DAILY_TMAX", dailyAirTemperatureMax },
      { "DAILY_TAVG", dailyAirTemperatureAvg },
      { "DAILY_PREC", dailyPrecipitation },
      { "DAILY_RHMIN", dailyAirHumidityMin },
      { "DAILY_RHMAX", dailyAirHumidityMax },
      { "DAILY_RHAVG", dailyAirHumidityAvg },
      { "DAILY_RAD", dailyGlobalRadiation },
      { "DAILY_W_INT_AVG", dailyWindIntensityAvg },
      { "DAILY_W_DIR", dailyWindDirectionPrevailing },
      { "DAILY_W_INT_MAX", dailyWindIntensityMax },
      { "DAILY_ET0", dailyReferenceEvapotranspiration },
      { "DAILY_LEAFW", dailyLeafWetness }
    };

    const std::map<std::string, meteoVariable> MapHourlyMeteoVar = {
      { "TAVG", airTemperature },
      { "PREC", precipitation },
      { "RHAVG", airHumidity },
      { "RAD", globalIrradiance },
      { "W_INT_AVG", windIntensity },
      { "W_DIR", windDirection },
      { "ET0", referenceEvapotranspiration },
      { "LEAFW", leafWetness }
    };

    enum frequencyType {hourly, daily, monthly, noFrequency};

    enum surfaceType   {SurfaceTypeWater, SurfaceTypeSoil, SurfaceTypeCrop};

    class Crit3DClimateParameters
    {
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

    double ET0_Penman_daily(int myDOY, float myLatitude, float myPressure, float myTmin, float myTmax,
                            float myTminDayAfter, float myUmed, float myVmed10, float mySWGlobRad);

    double ET0_Hargreaves(double KT, double myLat, int myDoy, double tmax, double tmin);

    meteoVariable getDefaultMeteoVariable(int myVar);

    bool setColorScale(meteoVariable variable, Crit3DColorScale *colorScale);

    std::string getVariableString(meteoVariable myVar);


#endif // METEO_H

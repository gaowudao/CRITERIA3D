#ifndef SNOWPOINT_H
#define SNOWPOINT_H

#ifndef GIS_H
    #include "gis.h"
#endif
#ifndef METEO_H
    #include "meteo.h"
#endif
#ifndef METEOPOINT_H
    #include "meteoPoint.h"
#endif
#ifndef SOLARRADIATION_H
    #include "solarRadiation.h"
#endif

//Junsei Kondo, Hiromi Yamazawa, Measurement of snow surface emissivity
#define SNOW_EMISSIVITY 0.97f               //[-]
#define SOIL_EMISSIVITY 0.92f               //[-] soil (average)

#define SOIL_SPECIFIC_HEAT 2.1f             //[KJ/kg/°C]

#define DEFAULT_BULK_DENSITY 1300           //[kg/m^3]

#define SOIL_DAMPING_DEPTH 0.3f             //[m]
#define SNOW_DAMPING_DEPTH 0.05f            //[m]

#define SNOW_MINIMUM_HEIGHT 2               //[mm]


struct Tparameters {
    float snowSkinThickness;              //[m]
    float soilAlbedo;                     //[-] bare soil
    float snowVegetationHeight;           //[m] height of vegetation
    float snowWaterHoldingCapacity;       // percentuale di acqua libera che il manto nevoso può trattenere
    float snowMaxWaterContent;            //[m] acqua libera (torrenti, laghetti)
    float tempMaxWithSnow;                //[°C]
    float tempMinWithRain;                //[°C]
};

class Crit3DSnowPoint
{
public:
    Crit3DSnowPoint(struct TradPoint* radpoint, Crit3DMeteoPoint* meteopoint);
    ~Crit3DSnowPoint();

    bool checkValidPoint();
    void computeSnowFall();
    void computeSnowBrooksModel(float myClearSkyTransmissivity);

    float getSnowFall();
    float getSnowMelt();
    float getSnowWaterEquivalent();
    float getIceContent();
    float getLWContent();
    float getInternalEnergy();
    float getSurfaceInternalEnergy();
    float getSnowSurfaceTemp();
    float getAgeOfSnow();

    float getSnowSkinThickness();
    float getSoilAlbedo();
    float getSnowVegetationHeight();
    float getSnowWaterHoldingCapacity();
    float getSnowMaxWaterContent();
    float getTempMaxWithSnow();
    float getTempMinWithRain();

    static float aerodynamicResistanceCampbell77(bool isSnow , float zRefWind, float myWindSpeed, float vegetativeHeight);

private:

    float _snowFall;
    float _snowMelt;
    float _snowWaterEquivalent;
    float _iceContent;
    float _lWContent;
    float _internalEnergy;
    float _surfaceInternalEnergy;
    float _snowSurfaceTemp;
    float _ageOfSnow;

    float _slope;
    struct TradPoint* _radpoint;
    Crit3DMeteoPoint* _meteopoint;
    float _waterContent;
    float _evaporation;
    struct Tparameters* _parameters;

};

#endif // SNOWPOINT_H

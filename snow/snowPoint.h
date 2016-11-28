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

};

#endif // SNOWPOINT_H

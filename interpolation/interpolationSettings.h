#ifndef INTERPOLATIONSETTINGS_H
#define INTERPOLATIONSETTINGS_H

#ifndef INTERPOLATIONCONSTS_H
    #include "interpolationConstants.h"
#endif
#ifndef METEO_H
    #include "meteo.h"
#endif
#ifndef GIS_H
    #include "gis.h"
#endif

class Crit3DProxy
{
private:
    proxyVars::TProxyVar name;
    bool isActive;
    gis::Crit3DRasterGrid* grid;

public:
    void initialize(proxyVars::TProxyVar name_);
    proxyVars::TProxyVar getName();
    void setActive(bool isActive_);
    bool getActive();
};

class Crit3DInterpolationSettings
{
private:
    int interpolationMethod;

    bool useThermalInversion;

    bool useTAD;
    bool useJRC;
    bool useDewPoint;

    bool isKrigingReady;
    bool isRetrendActive;
    float genericPearsonThreshold;
    float maxHeightInversion;

    std::vector <Crit3DProxy> currentProxy;

    bool currentClimateParametersLoaded;
    Crit3DClimateParameters currentClimateParameters;

    Crit3DDate currentDate;
    int currentHour;
    int currentHourFraction;

public:
    Crit3DInterpolationSettings();

    Crit3DProxy getProxy(int pos);
    proxyVars::TProxyVar getProxyName(int pos);
    int getProxyNr();
    bool getProxyActive(int pos);
    void setProxyActive(int pos, bool isActive_);
    void addProxy(proxyVars::TProxyVar myProxyName);

    void setClimateParameters(Crit3DClimateParameters* myParameters);
    void setCurrentDate(Crit3DDate myDate);
    void setCurrentHour(int myHour);
    void setCurrentHourFraction(int myHourFraction);

    void setInterpolationMethod(bool myValue);

    void setUseThermalInversion(bool myValue);

    void setUseTAD(bool myValue);
    void setUseJRC(bool myValue);
    void setUseDewPoint(bool myValue);

    bool getUseTad();
    int getInterpolationMethod();
    float getMaxHeightInversion();

    bool getUseThermalInversion();

    bool getUseTAD();
    bool getUseJRC();
    bool getUseDewPoint();

    float getGenericPearsonThreshold();

    float getCurrentClimateLapseRate(meteoVariable myVar);

};


#endif // INTERPOLATIONSETTINGS_H

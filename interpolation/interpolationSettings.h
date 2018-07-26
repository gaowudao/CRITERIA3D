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
    std::string name;
    bool isActive;
    gis::Crit3DRasterGrid* grid;
    float regressionR2;
    float regressionSlope;

public:
    void initialize(std::string name_);
    std::string getName();
    void setActive(bool isActive_);
    bool getActive();
    void setRegressionR2(float myValue);
    float getRegressionR2();
    void setRegressionSlope(float myValue);
    float getRegressionSlope();
    float getValue(int pos, std::vector <float> proxyValues);
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
    std::string getProxyName(int pos);
    int getProxyNr();
    bool getProxyActive(int pos);
    void setProxyActive(int pos, bool isActive_);
    void addProxy(std::string myProxyName);
    float getProxyValue(int pos, std::vector <float> proxyValues);

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

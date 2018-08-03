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
    std::string gridName;
    gis::Crit3DRasterGrid* grid;

public:
    Crit3DProxy();

    void initialize();
    std::string getName() const;
    void setName(const std::string &value);
    bool getIsActive() const;
    void setIsActive(bool value);
    gis::Crit3DRasterGrid *getGrid() const;
    void setGrid(gis::Crit3DRasterGrid *value);
    std::string getGridName() const;
    void setGridName(const std::string &value);

    TProxyVar getProxyPragaName();
};

class Crit3DProxyInterpolation : public Crit3DProxy
{
private:
    float regressionR2;
    float regressionSlope;

    //orography
    float lapseRateH1;
    float lapseRateH0;
    float inversionLapseRate;
    bool inversionIsSignificative;

public:
    Crit3DProxyInterpolation();

    void setRegressionR2(float myValue);
    float getRegressionR2();
    void setRegressionSlope(float myValue);
    float getRegressionSlope();
    float getValue(int pos, std::vector <float> proxyValues);
    float getLapseRateH1() const;
    void setLapseRateH1(float value);
    float getLapseRateH0() const;
    void setLapseRateH0(float value);
    float getInversionLapseRate() const;
    void setInversionLapseRate(float value);
    bool getInversionIsSignificative() const;
    void setInversionIsSignificative(bool value);

    void initializeOrography();
};

class Crit3DInterpolationSettings
{
private:
    TInterpolationMethod interpolationMethod;

    bool useThermalInversion;

    bool useTAD;
    bool useDewPoint;

    bool isKrigingReady;
    bool precipitationAllZero;
    float genericPearsonThreshold;
    float maxHeightInversion;

    std::vector <Crit3DProxyInterpolation> currentProxy;

    bool currentClimateParametersLoaded;
    Crit3DClimateParameters currentClimateParameters;

    Crit3DDate currentDate;
    int currentHour;
    int currentHourFraction;

public:
    Crit3DInterpolationSettings();

    void initialize();
    void initializeProxy();

    Crit3DProxyInterpolation* getProxy(int pos);
    std::string getProxyName(int pos);
    int getProxyNr();
    bool getProxyActive(int pos);
    void setProxyActive(int pos, bool isActive_);
    void addProxy(Crit3DProxy myProxy);
    float getProxyValue(int pos, std::vector <float> proxyValues);

    void setClimateParameters(Crit3DClimateParameters* myParameters);
    void setCurrentDate(Crit3DDate myDate);
    void setCurrentHour(int myHour);
    void setCurrentHourFraction(int myHourFraction);

    void setInterpolationMethod(TInterpolationMethod myValue);

    void setUseThermalInversion(bool myValue);

    void setUseTAD(bool myValue);
    void setUseJRC(bool myValue);
    void setUseDewPoint(bool myValue);

    bool getUseTad();
    TInterpolationMethod getInterpolationMethod();
    float getMaxHeightInversion();

    bool getUseThermalInversion();

    bool getUseTAD();
    bool getUseJRC();
    bool getUseDewPoint();

    float getGenericPearsonThreshold();

    float getCurrentClimateLapseRate(meteoVariable myVar);

    bool getPrecipitationAllZero() const;
    void setPrecipitationAllZero(bool value);
};


#endif // INTERPOLATIONSETTINGS_H

#ifndef INTERPOLATIONSETTINGS_H
#define INTERPOLATIONSETTINGS_H

#ifndef INTERPOLATIONCONSTS_H
    #include "interpolationConstants.h"
#endif
#ifndef METEO_H
    #include "meteo.h"
#endif

class Crit3DInterpolationSettings
{
private:
    int interpolationMethod;
    int elaboration;

    bool useHeight;
    bool useThermalInversion;
    bool useOrogIndex;
    bool useSeaDistance;
    bool useUrbanFraction;
    bool useAspect;
    bool useTAD;
    bool useJRC;
    bool useDewPoint;
    bool useGenericProxy;

    bool detrendOrographyActive;
    bool detrendUrbanActive;
    bool detrendOrogIndexActive;
    bool detrendSeaDistanceActive;
    bool detrendAspectActive;
    bool detrendGenericProxyActive;

    bool isKrigingReady;
    bool isRetrendActive;
    float genericPearsonThreshold;
    float maxHeightInversion;

    proxyVars::TProxyVar detrendList[PROXY_VAR_NR];

    bool currentClimateParametersLoaded;
    Crit3DClimateParameters currentClimateParameters;

    Crit3DDate currentDate;
    int currentHour;
    int currentHourFraction;

public:
    Crit3DInterpolationSettings();
    bool isCrossValidation;

    proxyVars::TProxyVar getDetrendList(int myPosition);

    void setClimateParameters(Crit3DClimateParameters* myParameters);
    void setCurrentDate(Crit3DDate myDate);
    void setCurrentHour(int myHour);
    void setCurrentHourFraction(int myHourFraction);

    void setInterpolationMethod(bool myValue);
    void setUseHeight(bool myValue);
    void setUseThermalInversion(bool myValue);
    void setUseOrogIndex(bool myValue);
    void setUseSeaDistance(bool myValue);
    void setUrbanFraction(bool myValue);
    void setUseAspect(bool myValue);
    void setUseGenericProxy(bool myValue);
    void setUseTAD(bool myValue);
    void setUseJRC(bool myValue);
    void setUseDewPoint(bool myValue);
    void setIsCrossValidation(bool myValue);

    bool getUseTad();
    int getInterpolationMethod();
    float getMaxHeightInversion();
    bool getUseHeight();
    bool getUseThermalInversion();
    bool getUseOrogIndex();
    bool getUseSeaDistance();
    bool getUseUrbanFraction();
    bool getUseAspect();
    bool getUseGenericProxy();
    bool getUseTAD();
    bool getUseJRC();
    bool getUseDewPoint();
    bool getIsCrossValidation();

    void setDetrendOrographyActive(bool myValue);
    void setDetrendUrbanActive(bool myValue);
    void setDetrendOrogIndexActive(bool myValue);
    void setDetrendSeaDistanceActive(bool myValue);
    void setDetrendAspectActive(bool myValue);
    void setDetrendGenericProxyActive(bool myValue);

    bool getDetrendOrographyActive();
    bool getDetrendUrbanActive();
    bool getDetrendOrogIndexActive();
    bool getDetrendSeaDistanceActive();
    bool getDetrendAspectActive();
    bool getDetrendGenericProxyActive();

    float getGenericPearsonThreshold();

    float getCurrentClimateLapseRate(meteoVariable myVar);
    int getElaboration() const;
    void setElaboration(int value);

};


#endif // INTERPOLATIONSETTINGS_H

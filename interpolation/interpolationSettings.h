#ifndef INTERPOLATIONSETTINGS_H
#define INTERPOLATIONSETTINGS_H

    #ifndef INTERPOLATIONCONSTS_H
        #include "interpolationConstants.h"
    #endif
    #ifndef METEO_H
        #include "meteo.h"
    #endif
    #ifndef METEOGRID_H
        #include "meteoGrid.h"
    #endif

    class Crit3DProxy
    {
    private:
        std::string name;
        bool isSignificant;
        std::string gridName;
        gis::Crit3DRasterGrid* grid;

    public:
        Crit3DProxy();

        std::string getName() const;
        void setName(const std::string &value);
        gis::Crit3DRasterGrid *getGrid() const;
        void setGrid(gis::Crit3DRasterGrid *value);
        std::string getGridName() const;
        void setGridName(const std::string &value);
        bool getIsSignificant() const;
        void setIsSignificant(bool value);

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
        float getValue(unsigned int pos, std::vector <float> proxyValues);
        float getLapseRateH1() const;
        void setLapseRateH1(float value);
        float getLapseRateH0() const;
        void setLapseRateH0(float value);
        float getInversionLapseRate() const;
        void setInversionLapseRate(float value);
        bool getInversionIsSignificative() const;
        void setInversionIsSignificative(bool value);

        void initializeOrography();
        bool getProva() const;
        void setProva(bool value);
    };

    class Crit3DProxyCombination
    {
    private:
        std::vector <int> indexProxy;
        bool useThermalInversion;
        int indexHeight;

    public:
        Crit3DProxyCombination();

        bool getUseThermalInversion() const;
        void setUseThermalInversion(bool value);
        std::vector<int> getIndexProxy() const;
        void setIndexProxy(const std::vector<int> &value);
        int getIndexHeight() const;
        void setIndexHeight(int value);
    };

    class Crit3DInterpolationSettings
    {
    private:
        gis::Crit3DRasterGrid* currentDEM; //for TAD

        TInterpolationMethod interpolationMethod;

        float minRegressionR2;
        bool useThermalInversion;
        bool useTAD;
        bool useLapseRateCode;
        bool useBestDetrending;
        bool useDewPoint;

        gridAggregationMethod meteoGridAggrMethod;

        bool isKrigingReady;
        bool precipitationAllZero;
        float maxHeightInversion;
        float shepardInitialRadius;
        int indexPointCV;
        float topoDist_Kh, topoDist_Kz;

        std::vector <Crit3DProxyInterpolation> currentProxy;
        Crit3DProxyCombination optimalCombination;
        Crit3DProxyCombination selectedCombination;

        bool currentClimateParametersLoaded;
        Crit3DClimateParameters currentClimateParameters;

    public:
        Crit3DInterpolationSettings();

        void initialize();
        void initializeProxy();

        void computeShepardInitialRadius(float area, int nrPoints);

        Crit3DProxyInterpolation* getProxy(int pos);
        std::string getProxyName(int pos);
        int getProxyNr();
        void addProxy(Crit3DProxy myProxy);
        float getProxyValue(unsigned int pos, std::vector <float> proxyValues);
        Crit3DProxyCombination getCombination();

        void setClimateParameters(Crit3DClimateParameters* myParameters);

        void setInterpolationMethod(TInterpolationMethod myValue);
        TInterpolationMethod getInterpolationMethod();

        void setUseThermalInversion(bool myValue);
        bool getUseThermalInversion();

        void setUseTAD(bool myValue);
        bool getUseTAD();

        void setUseDewPoint(bool myValue);
        bool getUseDewPoint();

        float getMaxHeightInversion();

        float getCurrentClimateLapseRate(meteoVariable myVar, Crit3DTime myTime);

        bool getPrecipitationAllZero() const;
        void setPrecipitationAllZero(bool value);
        float getMinRegressionR2() const;
        void setMinRegressionR2(float value);
        bool getUseLapseRateCode() const;
        void setUseLapseRateCode(bool value);
        bool getUseBestDetrending() const;
        void setUseBestDetrending(bool value);
        gridAggregationMethod getMeteoGridAggrMethod() const;
        void setMeteoGridAggrMethod(const gridAggregationMethod &value);
        float getShepardInitialRadius() const;
        void setShepardInitialRadius(float value);
        int getIndexPointCV() const;
        void setIndexPointCV(int value);
        gis::Crit3DRasterGrid *getCurrentDEM() const;
        void setCurrentDEM(gis::Crit3DRasterGrid *value);
        float getTopoDist_Kh() const;
        void setTopoDist_Kh(float value);
        float getTopoDist_Kz() const;
        void setTopoDist_Kz(float value);
        Crit3DProxyCombination getOptimalCombination() const;
        void setOptimalCombination(const Crit3DProxyCombination &value);
        Crit3DProxyCombination getSelectedCombination() const;
        void setSelectedCombination(const Crit3DProxyCombination &value);
    };

#endif // INTERPOLATIONSETTINGS_H

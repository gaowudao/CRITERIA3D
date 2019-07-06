#ifndef RADIATIONSETTINGS_H
#define RADIATIONSETTINGS_H

    #ifndef CRIT3DDATE_H
        #include "crit3dDate.h"
    #endif
    #ifndef RADIATIONDEFINITIONS_H
        #include "radiationDefinitions.h"
    #endif

    struct TtransmissivitySettings
    {
        TtransmissivityAlgorithm model;
        TtransmissivityComputationPeriod periodType;
        bool useTotal;
        float clearSky ;
    } ;

    class Crit3DRadiationSettings
    {
    private:
        TradiationAlgorithm algorithm;
        TtransmissivitySettings transSettings;
        TparameterMode linkeMode;
        TlandUse landUse;
        TparameterMode albedoMode;
        TtiltMode tiltMode;

        bool computeRealData;
        bool usePotentialIfMissing;
        float timeStepIntegration;
        bool computeShadowing;
        float shadowDistanceFactor;
        float linke;
        float albedo;
        float tilt;
        float aspect;

    public:
        Crit3DRadiationSettings();

        gis::Crit3DGisSettings* gisSettings;

        void setGisSettings(const gis::Crit3DGisSettings *myGisSettings);

        bool getComputeRealData();
        bool getUsePotentialIfMissing();
        float getTimeStepIntegration();
        bool getComputeShadowing();
        float getShadowDistanceFactor();
        float getLinke();
        float getAlbedo();
        float getTilt();
        float getAspect();
        TparameterMode getLinkeMode();
        TlandUse getLandUse();
        TparameterMode getAlbedoMode();
        TtiltMode getTiltMode();
        TradiationAlgorithm getAlgorithm();

        TtransmissivityAlgorithm getTransmissivityAlgorithm();
        TtransmissivityComputationPeriod getTransmissivityPeriod();
        Crit3DDate* getWindowDate();
        bool getTransmissivityUseTotal();
        float getTransmissivityClearSky();
        bool getTransmissivityClearSkyComputed();
        float getClearSky();
        float getWindowSolarTime();
        int getWindowNrData();
        void setWindowNrData(int myNrData);
        float getThresholdPotential();
    } ;

#endif // RADIATIONSETTINGS_H

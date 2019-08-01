#ifndef RADIATIONSETTINGS_H
#define RADIATIONSETTINGS_H

    #ifndef CRIT3DDATE_H
        #include "crit3dDate.h"
    #endif
    #ifndef RADIATIONDEFINITIONS_H
        #include "radiationDefinitions.h"
    #endif

    class Crit3DRadiationSettings
    {
    private:
        TradiationAlgorithm algorithm;
        TradiationRealSkyAlgorithm realSkyAlgorithm;
        TparameterMode linkeMode;
        TparameterMode albedoMode;
        TtiltMode tiltMode;

        bool realSky;
        bool shadowing;
        float linke;
        float albedo;
        float tilt;
        float aspect;
        float clearSky;

    public:        
        gis::Crit3DGisSettings* gisSettings;

        Crit3DRadiationSettings();

        void initialize();
        void setGisSettings(const gis::Crit3DGisSettings *myGisSettings);

        TradiationRealSkyAlgorithm getRealSkyAlgorithm() const;
        void setRealSkyAlgorithm(const TradiationRealSkyAlgorithm &value);
        float getClearSky() const;
        void setClearSky(float value);
        bool getRealSky() const;
        void setRealSky(bool value);
        bool getShadowing() const;
        void setShadowing(bool value);
        float getLinke() const;
        void setLinke(float value);
        float getAlbedo() const;
        void setAlbedo(float value);
        float getTilt() const;
        void setTilt(float value);
        float getAspect() const;
        void setAspect(float value);
        TradiationAlgorithm getAlgorithm() const;
        void setAlgorithm(const TradiationAlgorithm &value);
        TparameterMode getLinkeMode() const;
        void setLinkeMode(const TparameterMode &value);
        TparameterMode getAlbedoMode() const;
        void setAlbedoMode(const TparameterMode &value);
        TtiltMode getTiltMode() const;
        void setTiltMode(const TtiltMode &value);
    } ;

    std::string getKeyStringRadAlgorithm(TradiationAlgorithm value);
    std::string getKeyStringRealSky(TradiationRealSkyAlgorithm value);
    std::string getKeyStringParamMode(TparameterMode value);
    std::string getKeyStringTiltMode(TtiltMode value);

#endif // RADIATIONSETTINGS_H

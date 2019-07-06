#ifndef RADIATIONDEFINITIONS_H
#define RADIATIONDEFINITIONS_H

    #ifndef GIS_H
        #include "gis.h"
    #endif

    /*! Surface pressure at sea level (millibars) (used for refraction correction and optical air mass) */
    #define PRESSURE_SEALEVEL 1013
    /*! Ambient default dry-bulb temperature (degrees C) (used for refraction correction) */
    #define TEMPERATURE_DEFAULT 15
    /*! scale height of Rayleigh atmosphere near the Earth surface */
    #define RAYLEIGH_Z0 8434.5

    /*! Eppley shadow band width (cm) */
    #define SBWID 7.6f
    /*! Eppley shadow band radius (cm) */
    #define SBRAD 31.7f
    /*! Drummond factor for partly cloudy skies */
    #define SBSKY 0.04f

    #define CLEAR_SKY_TRANSMISSIVITY_DEFAULT     0.75f

    enum TlandUse{LAND_USE_MOUNTAIN = 0, LAND_USE_RURAL=1, LAND_USE_CITY=2, LAND_USE_INDUSTRIAL=3};
    enum TparameterMode {PARAM_MODE_FIXED = 0, PARAM_MODE_MAP = 1} ;
    enum TtiltMode{TILT_TYPE_HORIZONTAL=0, TILT_TYPE_INCLINED=1,TILT_TYPE_DEM=2};
    enum TradiationAlgorithm{RADIATION_ALGORITHM_RSUN = 0,RADIATION_ALGORITHM_BROOKS = 1};
    enum TtransmissivityAlgorithm{TRANSMISSIVITY_MODEL_HOURLY = 0, TRANSMISSIVITY_MODEL_DAILY = 1, TRANSMISSIVITY_MODEL_SAMANI = 2};
    enum TtransmissivityComputationPeriod{TRANSMISSIVITY_COMPUTATION_DYNAMIC = 0,TRANSMISSIVITY_COMPUTATION_DAILY = 1};
    enum TperiodType {GENERIC_TIME = 0, HOURLY = 1, DAILY = 2, DECADAL = 3, MONTHLY = 4, SEASONAL = 5, ANNUAL = 6, GENERIC_PERIOD = 7};

    struct TsunPosition
    {
        float hourDecimal;
        float rise;                     /*!<  Sunrise time, from midnight, local, WITHOUT refraction [s] */
        float set;                      /*!<  Sunset time, from midnight, local, WITHOUT refraction [s] */
        float azimuth;                  /*!<  Solar azimuth angle [degrees, N=0, E=90, S=180, W = 270] */
        float elevation;                /*!<  Solar elevation, no atmospheric correction */
        float elevationRefr;            /*!<  Solar elevation angle refracted [deg.From horizon] */
        float incidence;                /*!<  Solar incidence angle on Panel [deg] */
        float relOptAirMass;            /*!<  Relative optical airmass [] */
        float relOptAirMassCorr;        /*!<  Pressure-corrected airmass [] */
        float extraIrradianceNormal;    /*!<  Extraterrestrial (top-of-atmosphere) direct normal solar irradiance [W m-2] */
        float extraIrradianceHorizontal;/*!<  Extraterrestrial (top-of-atmosphere) global horizontal solar irradiance [W m-2] */
        bool shadow;                    /*!<  Boolean for "Sun is not visible" */
    };

    struct TradPoint
    {
        double x;
        double y;
        double height;
        double lat;
        double lon;
        double slope;
        double aspect;
        double beam;
        double diffuse;
        double reflected;
        double global;
        double transmissivity;
    };

    class Crit3DTransmissivityPoint
    {
    public:
       gis::Crit3DPoint* point;
       bool isActive;
       float transmissivity;
       float* globalIrradiance;
       Crit3DTransmissivityPoint();
    };

#endif // RADIATIONDEFINITIONS_H

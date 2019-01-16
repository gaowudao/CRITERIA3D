#ifndef GRAPEVINE_H
#define GRAPEVINE_H

#ifndef CRIT3DDATE_H
    #include "crit3dDate.h"
#endif

#ifndef SOIL_H
    #include "soil.h"
#endif

#ifndef _GLIBCXX_STRING
    #include <string>
#endif

#define UPSCALINGFUNC(z,LAI) ((1.0 - exp(-(z)*(LAI))) / (z))

// Tree-plant properties
#define FORM   0.5  // stem form factor
#define RHOF   0.1  // [KgDM m-3] foliage density
#define RHOS   750  // [KgDM m-3] wood-stem density

//Hydraulic properties
#define H50     0.4         //height for 50% maturation of xylem cells (m) [not relevant]
#define KR      4.0E-7      //root specific conductance (m3 MPa-1 s-1 kg-1) [not relevant]
#define KSMAX   2.5E-3      //max. sapwood specific conductivity (m2 MPa-1 s-1) [not relevant]
#define PSITHR  -2.5        //water potential threshold for cavitation (MPa) [not relevant]

// **********************************************************************
//     Assign physical and miscellaneous constants
//     Note: the PARAMETER statement cannot be used, as parms are included in COMMON block
//     -----------------------------------------------------------------------------------
#define      CP_AIR  29.31      // specific heat capacity of air, J mol-1 K-1
#define      FC  0.5        // coeff for conversion of carbon into DM, kgC kgDM-1
#define      GAMMA  66.2    // psychrometer constant, Pa K-1
#define      LATENT  43956  // latent heat of vaporization, J mol-1
#define      H2OMOLECULARWEIGHT  0.018      // molecular weight of H2O, kg mol-1
#define      OSS 21176      // oxygen part pressure in the atmosphere, Pa

//     Define additional photosynthetic parameters
//     -------------------------------------------
#define      HARD    46.39           //activation energy of RD0 (kJ mol-1)
#define      HAVCM   65.33           //activation energy of VCMOP (kJ mol-1)
#define      HAJM    43.9            //activation energy of JMOP (kJ mol-1 e-)
#define      HAKC    79.43           //activation energy of KCT0 (kJ mol-1)
#define      HAKO    36.38           //activation energy of KOT0 (kJ mol-1)
#define      HAGSTAR 37.83           //activation energy of Gamma_star (kJ mol-1)
#define      HD      200             // deactivation energy from Kattge & Knorr 2007 (kJ mol-1)

#define      CRD     18.72           //scaling factor in RD0 response to temperature (-);
#define      CVCM    26.35           //scaling factor in VCMOP response to temperature (-);
#define      CVOM    22.98           //scaling factor in VOMOP response to temperature (-);
#define      CGSTAR  19.02           //scaling factor in Gamma_star response to temperature (-);
#define      CKC     38.05           //scaling factor in KCT0 response to temperature (-);
#define      CKO     20.30           //scaling factor in KOT0 response to temperature (-);
#define      CJM     17.7            //scaling factor in JMOP response to temperature (-);

//     Define additional functional and structural parameters for stand and understorey
//     --------------------------------------------------------------------------------
#define      CONV  0.8    //    dry matter conversion efficiency (growth resp.)(-)
#define      MERCH 0.85   //    merchantable wood as fraction of stem biomass (-)
#define      RADRT 1.E-3  //    root radius (m)
#define      STH0 0.8561  //    intercept in self-thinning eq. (log(TREES) vs log(WST)) (m-2)
#define      STH1 1.9551  //    slope in self-thinning eq. (log(TREES) vs log(WST)) (kgDM-1)
#define      ALLRUND 0.5  //    coeff of allocation to roots in understorey (-)

//     Define soil respiration parameters, partition soil C into young and old components
//     Note: initial steady-state conditions are assumed (Andren & Katterer 1997)
//     Reference respiration rates are for a 'tropical' soil (Andren & Katterer 1997, p. 1231).
//     ----------------------------------------------------------------------------------------
#define      HUMCOEF 0.125 //humification coefficient (-)
#define      R0SLO 0.04556 //resp per unit old soil carbon at ref conditions (kgDM kgDM-1 d-1)
#define      R0SLY 4.228   //resp per unit young soil carbon at ref conditions (kgDM kgDM-1 d-1)
#define      CHLDEFAULT 500

#define      RUEGRASS 1.0   //mais: 1.5-2.0, vine: 0.6-1.0

#define SHADEDGRASS true
#define SUNLITGRASS false

#define minShootLeafNr 1
#define LAIMIN 0.01
#define NOT_INITIALIZED_VINE -1

enum phenoStage {endoDormancy, ecoDormancy , budBurst , flowering , fruitSet, veraison, physiologicalMaturity, vineSenescence};
enum rootDistribution {CARDIOID_DISTRIBUTION, BLACKBODY_DISTRIBUTION , GAMMA_DISTRIBUTION};
enum TfieldOperation {irrigationOperation, grassSowing, grassRemoving, trimming, leafRemoval,
                      clusterThinning, harvesting, tartaricAnalysis};

struct TstateGrowth{
    double cumulatedBiomass;
    double fruitBiomass;
    double shootLeafNumber;
    double meanTemperatureLastMonth;
    double tartaricAcid;
    double leafAreaIndex;
    double fruitBiomassIndex;
    int isHarvested;

    void initialize()
    {
        cumulatedBiomass = 0.;
        fruitBiomass = 0.;
        shootLeafNumber = 0.;
        meanTemperatureLastMonth = 10.;
        tartaricAcid = NODATA;
        isHarvested = 0;
    }
};

struct TstatePheno{
    double chillingState;
    double forceStateBudBurst;
    double forceStateVegetativeSeason;
    double meanTemperature;
    double degreeDaysFromFirstMarch;        // based on 0 Celsius
    double degreeDaysAtFruitSet;            // based on 0 Celsius
    double daysAfterBloom;
    double stage;
    double cumulatedRadiationFromFruitsetToVeraison;

    void initialize()
    {
        chillingState = 86.267 ;
        forceStateVegetativeSeason = 0.;
        forceStateBudBurst = 0.415;
        meanTemperature = 3.93;
        degreeDaysFromFirstMarch = 0.0;
        degreeDaysAtFruitSet = NODATA;
        daysAfterBloom = -1;
        stage = endoDormancy ;
        cumulatedRadiationFromFruitsetToVeraison = 0.;
    }
};

struct ToutputPlant
{
    double transpirationNoStress;
    double grassTranspiration;
    double evaporation;
    double brixBerry;
    double brixMaximum;
    //double* waterExtraction;
};

struct TstatePlant
{
    TstateGrowth stateGrowth;
    TstatePheno statePheno;
    ToutputPlant outputPlant;
};

struct TparameterBindiMiglietta{
    double radiationUseEfficiency ;
    double d,f ;
    double fruitBiomassOffset , fruitBiomassSlope ;
};

struct TparameterBindiMigliettaFix{
    double a,b,c;
    double shadedSurface;
    double extinctionCoefficient ;
    double baseTemperature ;
    double tempMaxThreshold ;

    void initialize()
    {
        a =  -0.28;
        b = 0.04;
        c = -0.015;
        baseTemperature = 10; //Celsius deg
        tempMaxThreshold = 35; //Celsius deg
        extinctionCoefficient = 0.5;
        shadedSurface = 0.8;
    }


};

struct TparameterWangLeuning{
    double sensitivityToVapourPressureDeficit;
    double alpha;
    double psiLeaf;
    double waterStressThreshold;
    double maxCarboxRate;           // Vcmo at optimal temperature
};

struct TparameterWangLeuningFix{
    double optimalTemperatureForPhotosynthesis;
    double minimalStomatalConductance;
    void initialize()
    {
        optimalTemperatureForPhotosynthesis = 298.15; // Celsius deg
        minimalStomatalConductance = 0.008;
    }
};

struct TparameterPhenoVitis{
    double co1;
    double criticalChilling;
    double criticalForceStateFruitSet;
    double criticalForceStateFlowering;
    double criticalForceStateVeraison;
    double criticalForceStatePhysiologicalMaturity;
    double degreeDaysAtVeraison;
};

struct TparameterPhenoVitisFix{
    int startingDay;
    double a,optimalChillingTemp,co2;
    void initialize()
    {
        a = 0.005;
        optimalChillingTemp = 2.8;
        co2 = -0.015;
        startingDay = 244;
    }
};

struct Tvine {
    int id;
    TparameterBindiMiglietta parameterBindiMiglietta;
    TparameterWangLeuning parameterWangLeuning;
    TparameterPhenoVitis parameterPhenoVitis;
};

struct TtrainingSystem {
    int id;
    float shootsPerPlant;
    float rowWidth;
    float rowHeight;
    float rowDistance;
    float plantDistance;
};

struct TvineField {
    int id;
    int soilIndex;
    float shootsPerPlant;
    float plantDensity;
    float maxLAIGrass;
    int trainingSystem;
    float maxIrrigationRate;        //[mm/h]
    Tvine* cultivar;
};

struct TsoilProfileTest {
    double* waterContent;
    double* psi;
    double* temp;
};

struct Vine3D_Nitrogen {
    double interceptLeaf,slopeLeaf,leafNitrogen;
    double leaf , stem , root , shoot;
};


struct Vine3D_SunShade {

    double absorbedPAR ;
    double isothermalNetRadiation;
    double leafAreaIndex ;
    double totalConductanceHeatExchange;
    double aerodynamicConductanceHeatExchange;
    double aerodynamicConductanceCO2Exchange ;
    double leafTemperature ;
    double darkRespiration ;
    double minimalStomatalConductance;
    double maximalCarboxylationRate,maximalElectronTrasportRate ;
    double carbonMichaelisMentenConstant, oxygenMichaelisMentenConstant ;
    double compensationPoint, convexityFactorNonRectangularHyperbola ;
    double quantumYieldPS2 ;
    double assimilation,transpiration,stomatalConductance ;
};

struct Vine3D_DeltaTimeResults {

    double netAssimilation;
    double grossAssimilation ;
    double transpiration ;
    double interceptedWater ;
    double netDryMatter ;
    double absorbedPAR ;
    double respiration ;
    double transpirationGrass;
    double transpirationNoStress;
};

struct Vine3D_Biomass {

    double total ;
    double leaf ;
    double sapwood ;
    double fineRoot ;
    double shoot ;
};


struct TqualityBerry {
    double acidity;
    double pH;
    double sugar;
};

class Vine3D_Grapevine {

private:
    TstatePlant statePlant;

    double myIrradiance ;
    double myInstantTemp ;
    double meanDailyTemperature;
    double chlorophyllContent;
    double myThermalUnit ;
    double leafNumberRate ;
    double stepPhotosynthesis ;
    int myDoy ;
    int myYear ;
    double mySunElevation ;
    double myDiffuseIrradiance;
    double myDirectIrradiance ;
    double myLongWaveIrradiance ;
    double myRelativeHumidity ;
    double vaporPressureDeficit ;
    double myHour ;
    double myCloudiness ;
    double myAirVapourPressure ;
    double myWindSpeed ;
    double emissivitySky ;
    double myPlantHeight ;
    double myLeafWidth ;
    double myAtmosphericPressure ;
    double mySoilTemp ;
    double myPrec ;
    double slopeSatVapPressureVSTemp ;
    double directLightK, diffuseLightK, diffuseLightKPAR, diffuseLightKNIR,directLightKPAR, directLightKNIR;
    double leafNitrogen ;
    bool isAmphystomatic ;
    Vine3D_SunShade shaded ;
    Vine3D_SunShade sunlit ;
    Vine3D_Nitrogen nitrogen ;
    Vine3D_DeltaTimeResults deltaTime ;
    Vine3D_Biomass biomass ;
    double specificLeafArea ;
    double alphaLeuning ;
    double wiltingPoint;
    double psiSoilAverage;
    double psiFieldCapacityAverage;
    double assimilationInstant , transpirationInstant , totalStomatalConductance, totalStomatalConductanceNoStress ;
    double* transpirationInstantLayer;          //molH2O m^-2 s^-1
    double* transpirationLayer;                 //mm
    double* transpirationCumulatedGrass;
    double entropicFactorCarboxyliation,entropicFactorElectronTransporRate ;
    double simulationStepInSeconds ;
    double* psiSoilProfile;
    double* soilWaterContentProfile;
    double* soilWaterContentProfileFC;
    double* soilWaterContentProfileWP;
    double* fractionTranspirableSoilWaterProfile;
    double* stressCoefficientProfile;
    double fractionTranspirableSoilWaterAverage;
    double* soilTempProfile;
    double soilWaterContent;
    double* soilFieldCapacity;
    double* layerRootDensity;
    double* layerDepth;
    double* layerThickness;
    int soilLayersNr;
    double soilTotalDepth;
    double potentialEvapotranspiration;
    double potentialBrix;
    TparameterBindiMigliettaFix parameterBindiMigliettaFix;
    TparameterWangLeuningFix parameterWangLeuningFix;
    TparameterPhenoVitisFix parameterPhenoVitisFix;
    double transpirationInstantNoStress;


private: // functions
    void photosynthesisRadiationUseEfficiency(Tvine* cultivar);
    double getCO2();
    double acclimationFunction(double Ha, double Hd, double leafTemp,double entropicTerm,double optimumTemp);
    double acclimationFunction2(double preFactor, double expFactor, double leafTemp,double optimumTemp);
    void weatherVariables();
    void radiationAbsorption ();
    void aerodynamicalCoupling ();
    void upscale(Tvine* cultivar);
    void photosynthesisAndTranspiration(Tvine* cultivar);
    void carbonWaterFluxes(Tvine* cultivar);
    void carbonWaterFluxesProfile(Tvine *cultivar);
    void carbonWaterFluxesProfileNoStress(Tvine *cultivar);
    void photosynthesisKernel(Tvine *cultivar, double COMP, double GAC, double GHR, double GSCD, double J, double KC, double KO
                              , double RD, double RNI, double STOMWL, double VCMAX, double *ASS, double *GSC, double *TR);
    void photosynthesisKernelSimplified(Tvine *cultivar, double COMP, double GSCD, double J, double KC, double KO
                         , double RD, double STOMWL, double VCmax, double *ASS, double *GSC, double *TR);
    void cumulatedResults();
    double plantRespiration();
    double temperatureMoistureFunction(double temperature);
    void plantInterception(double fieldCoverByPlant);
    double meanLastMonthTemperature(double temperature);
    double* rootDensity(soil::Crit3DSoil* mySoil,
                        int nrLayersSoilProfile, int nrLayersWithRoot, int nrUpperLayersWithoutRoot,
                        rootDistribution type, double *layerDepth, double *layeThickness,
                        double mode, double mean);
    double* waterSuctionDistribution(int nrLayers, double *layerRootDensity, double *psiSoil);

    double chillingRate(double temp, double aParameter, double cParameter);
    double criticalForceState(double chillState,double co1 , double co2);
    double forceStateFunction(double forceState , double temp);
    double forceStateFunction(double forceState , double temp, double degDays);

    void computePhenology(bool computeDaily, bool* isVegSeason, Tvine* cultivar);
    double leafWidth();
    void leafTemperature();
    double getLAIGrass(bool isShadow, double laiMax);
    void getLAIVine(TvineField *vineField);

    double getWaterStressByPsiSoil(double myPsiSoil,double psiSoilStressParameter,double exponentialFactorForPsiRatio);
    double getWaterStressSawFunction(int index, Tvine *cultivar);
    bool getExtractedWaterFromGrassTranspirationandEvaporation(double* myWaterExtractionProfile);
    double getWaterStressSawFunctionAverage(Tvine* cultivar);
    double getGrassTranspiration(double stress, double laiGrassMax, double sensitivityToVPD, double fieldCoverByPlant);
    double getFallowTranspiration(double stress, double laiGrassMax, double sensitivityToVPD);
    double getGrassRootDensity(int layer, float startRootDepth, float totalRootDepth);
    void grassTranspiration(double laiGrassMax, double sensitivityToVPD, double fieldCoverByPlant);
    void fallowTranspiration(double laiGrassMax, double sensitivityToVPD);
    void getFixSimulationParameters();
    double getLaiStressCoefficient();
    void getPotentialBrix();
    void initializeWaterStress(Tvine* cultivar);
    double gompertzDistribution(double stage);
    double getTartaricAcid();
    double soilTemperatureModel();
    void fruitBiomassRateDecreaseDueToRainfall();

public:
    Vine3D_Grapevine();

    //void initializeGrapevineModel(Tvine* vine, double secondsPerStep);
    void setDate (Crit3DTime myTime);
    bool setWeather(double meanDailyTemp, double temp, double irradiance ,
            double prec , double relativeHumidity , double windSpeed, double atmosphericPressure);
    bool setDerivedVariables (double diffuseIrradiance, double directIrradiance,
            double cloudIndex, double sunElevation, double etp);
    bool setSoilProfile(double* myWiltingPoint, double *myFieldCapacity,
                        double *myPsiSoilProfile , double *mySoilWaterContentProfile,
                        double* mySoilWaterContentFC, double* mySoilWaterContentWP);
    bool setStatePlant(TstatePlant myStatePlant, bool isVineyard);
    TstatePlant getStatePlant();
    ToutputPlant getOutputPlant();
    bool getExtractedWater(double* myWaterExtractionProfile);
    void initializeRootProperties (soil::Crit3DSoil* mySoil,
              int nrSoilLayers, float maxRootDepth,
              double* myLayerDepth, double* myLayerThickness,
              int nrSoilLayersWithoutRoot, int nrSoilLayerWithRoot,
              rootDistribution type, double depthMode, double depthMean);
    bool compute(bool computeDaily, int secondsPerStep, TvineField *vineField, double chlorophyll);

    //bool getOutputPlant(int hour, ToutputPlant *outputPlant);
    bool initializeStatePlant(int doy, TvineField *vineField);
    void printResults(std::string fileName, bool* isFirst, Tvine *cultivar);
    double getStressCoefficient();
    double getRealTranspirationGrapevine();
    double getRealTranspirationGrass();
    bool fieldBookAction(TvineField* vineField, TfieldOperation action, float quantity);
    double getRootDensity(int myLayer);

};


#endif // GRAPEVINE_H

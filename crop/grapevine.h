#ifndef GRAPEVINE_H
#define GRAPEVINE_H

#ifndef COMMONCONSTANTS_H
    #include "commonConstants.h"
#endif
#ifndef SOIL_H
    #include "soil.h"
#endif
#ifndef ROOT_H
    #include "root.h"
#endif

#define minShootLeafNr 1
#define LAIMIN 0.01
#define NOT_INITIALIZED_VINE -1

enum phenoStage {endoDormancy, ecoDormancy , budBurst , flowering , fruitSet, veraison, physiologicalMaturity, vineSenescence};
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
    float daysAfterBloom;
    float stage;
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


struct Vintage_Nitrogen {
    double interceptLeaf,slopeLeaf,leafNitrogen;
    double leaf , stem , root , shoot;
};


struct Vintage_SunShade {

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

struct Vintage_DeltaTimeResults {

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

struct Vintage_Biomass {

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

class Vintage_Grapevine {

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
        Vintage_SunShade shaded ;
        Vintage_SunShade sunlit ;
        Vintage_Nitrogen nitrogen ;
        Vintage_DeltaTimeResults deltaTime ;
        Vintage_Biomass biomass ;
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
                            rootDistributionType type, double *layerDepth, double *layeThickness,
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
        Vintage_Grapevine();

        //void initializeGrapevineModel(Tvine* vine, double secondsPerStep);
        void setDate (Crit3DTime myTime);
        bool setWeather(double meanDailyTemp, double temp, double irradiance ,
                double prec , double relativeHumidity , double windSpeed, double atmosphericPressure);
        bool setDerivedVariables (double diffuseIrradiance, double directIrradiance,
                double transmissivity, double sunElevation, float clearSkyTransmissivity,double etp);
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
                  rootDistributionType type, double depthMode, double depthMean);
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

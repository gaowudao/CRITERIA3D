#ifndef MODELS_H
#define MODELS_H

double rainfallCanopyInterceptionHydrall(double laiCanopy,double laiUnderstorey, double canopyCover, double prec);
namespace canopy
{
    double plantCover(double lai, double extinctionCoefficient, double laiMin);
    double waterStorageCapacity(double lai,double leafStorage,double stemStorage);
    double freeRainThroughfall(double rainfall,double cover);
    double rainfallInterceptedByCanopy(double rainfall,double cover);
    double evaporationFromCanopy(double waterFreeEvaporation, double storage, double grossStorage);
    double drainageFromTree(double grossStorage, double storage);
    double stemFlowRate(double maxStemFlowRate, double rainfall);
    bool waterManagementCanopy(double* StoredWater, double* throughfallWater, double rainfall, double waterFreeEvaporation, double lai, double extinctionCoefficient, double leafStorage, double stemStorage, double maxStemFlowRate, double* freeRainfall, double* drainage, double *stemFlow, double laiMin);
}

namespace roughness {
    enum {ASPHALT,BARESOIL,CROP,GRASS,FALLOW,ORCHARD,SHRUBS,FOREST,FOREST_LITTER,FOREST_UNDERSTORY};
    double manningCoefficient(int landType);
}


namespace litter
{
    double evaporationMulching(double storageCapacity);
    void waterManagementLitter(double* storedWater, double* throughfallWater, double rainfall,double waterFreeEvaporation,double storageCapacity);
    double storageCapacity(int doy);


}

namespace roughness {

}
#endif // MODELS_H


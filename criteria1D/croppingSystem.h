#ifndef CROPPINGSYSTEM_H
#define CROPPINGSYSTEM_H

    #include <string>

    class Criteria1D;
    class Crit3DDate;
    class Crit3DCrop;

    #define MIN_EMERGENCE_DAYS 7
    #define MAX_EVAPORATION_DEPTH 0.15

    void initializeCrop(Criteria1D* myCase, int currentDoy);
    bool updateCrop(Criteria1D* myCase, std::string* myError, Crit3DDate myDate, double tmin, double tmax, float waterTableDepth);

    float cropIrrigationDemand(Criteria1D* myCase, float myPrec, float nextPrec);
    bool cropWaterDemand(Criteria1D* myCase);
    bool irrigateCrop(Criteria1D* myCase, double irrigationDemand);

    bool evaporation(Criteria1D* myCase);
    double cropTranspiration(Criteria1D* myCase, bool getWaterStress);

    double getTotalEasyWater(Criteria1D* myCase);
    double getReadilyAvailableWater(Criteria1D* myCase);
    double getSoilWaterDeficit(Criteria1D* myCase);

    bool updateCropWaterStressSensibility(Crit3DCrop* myCrop);

#endif // CROPPINGSYSTEM

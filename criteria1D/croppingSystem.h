#ifndef CROPPINGSYSTEM_H
#define CROPPINGSYSTEM_H

    #include <string>

    class Criteria1D;
    class Crit3DDate;
    class Crit3DCrop;

    #define MIN_EMERGENCE_DAYS 7
    #define MAX_EVAPORATION_DEPTH 0.15

    bool initializeCrop(Criteria1D* myCase);
    bool resetCrop(Criteria1D* myCase);
    bool updateCrop(Criteria1D* myCase, std::string* myError, Crit3DDate myDate,
                    bool isFirstSimulationDay, double tmin, double tmax);

    float cropIrrigationDemand(Criteria1D* myCase, float myPrec, float nextPrec);
    bool cropWaterDemand(Criteria1D* myCase);
    bool irrigateCrop(Criteria1D* myCase, double irrigationDemand);

    bool evaporation(Criteria1D* myCase);
    bool cropTranspiration(Criteria1D* myCase);

    bool isInsideCycle(Crit3DCrop* myCrop, int myDoy);
    bool isSowingDoy(Crit3DCrop* myCrop, int myDoy);

    double getTotalEasyWater(Criteria1D* myCase);
    double getReadilyAvailableWater(Criteria1D* myCase);
    double getSoilWaterDeficit(Criteria1D* myCase);

#endif // CROPPINGSYSTEM

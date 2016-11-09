#ifndef CROP_H
#define CROP_H

    #ifndef _GLIBCXX_STRING
        #include <string>
    #endif
    #ifndef ROOT_H
        #include "root.h"
    #endif

    enum speciesType {HERBACEOUS_ANNUAL, HERBACEOUS_PERENNIAL, HORTICULTURAL, GRASS, GRASS_FIRST_YEAR, FALLOW, FRUIT_TREE};

    class Crit3DCrop
    {
        public:

        std::string idCrop;
        speciesType type;

        // root
        Crit3DRoot roots;

        // crop cycle
        int sowingDoy;
        int doyStartSenescence;
        int plantCycle;
        double LAImin, LAImax, LAIgrass;
        double LAIcurve_a, LAIcurve_b;
        double thermalThreshold;
        double upperThermalThreshold;
        double degreeDaysIncrease, degreeDaysDecrease, degreeDaysEmergence;

        // water need
        double kcMax;
        int degreeDaysMaxSensibility;
        double psiLeaf;                         // [cm]
        double stressTolerance;                 // []
        double frac_read_avail_water_min;
        double frac_read_avail_water_max;

        // irrigation
        int irrigationShift;
        double irrigationVolume;
        int degreeDaysStartIrrigation, degreeDaysEndIrrigation;
        double maxSurfacePuddle;

        // VARIABLES
        double degreeDays;
        bool isLiving;
        bool isEmerged;
        double LAI;
        double waterStressSensibility;
        double lastWaterStress;

        Crit3DCrop();
    };


    speciesType getCropType(std::string cropType);
    bool isPluriannual(speciesType myType);
    bool isGrass(speciesType myType);
    bool isWaterSurplusResistant(Crit3DCrop* myCrop);

#endif // CROP_H

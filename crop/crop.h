#ifndef CROP_H
#define CROP_H

    #ifndef _GLIBCXX_STRING
        #include <string>
    #endif
    #ifndef ROOT_H
        #include "root.h"
    #endif

    class Crit3DDate;

    enum speciesType {HERBACEOUS_ANNUAL, HERBACEOUS_PERENNIAL, HORTICULTURAL, GRASS, FALLOW, FRUIT_TREE};

    class Crit3DCrop
    {
        public:

        std::string idCrop;
        speciesType type;

        /*!
         * \brief roots
         */
        Crit3DRoot roots;


        /*!
         * \brief crop cycle
         */
        int sowingDoy;
        int currentSowingDoy;
        int doyStartSenescence;
        int plantCycle;
        double LAImin, LAImax, LAIgrass;
        double LAIcurve_a, LAIcurve_b;
        double thermalThreshold;
        double upperThermalThreshold;
        double degreeDaysIncrease, degreeDaysDecrease, degreeDaysEmergence;

        /*!
         * \brief water need
         */
        double kcMax;
        int degreeDaysMaxSensibility;
        double psiLeaf;                         /*!< [cm] */
        double stressTolerance;                 /*!< [] */
        double frac_read_avail_water_min;
        double frac_read_avail_water_max;

        /*!
         * \brief irrigation
         */
        int irrigationShift;
        double irrigationVolume;
        int degreeDaysStartIrrigation, degreeDaysEndIrrigation;
        double maxSurfacePuddle;


        /*!
         * \brief variables
         */
        double degreeDays;
        bool isLiving;
        bool isEmerged;
        double LAI;
        double LAIstartSenescence;
        double waterStressSensibility;

        Crit3DCrop();

        bool isWaterSurplusResistant();
        int getDaysFromTypicalSowing(int myDoy);
        int getDaysFromCurrentSowing(int myDoy);
        bool isInsideTypicalCycle(int myDoy);
        bool isPluriannual();
        bool needReset(Crit3DDate myDate, float latitude, float waterTableDepth);
        void resetCrop(int nrLayers);
    };


    speciesType getCropType(std::string cropType);

    double computeDegreeDays(double myTmin, double myTmax, double myLowerThreshold, double myUpperThreshold);


#endif // CROP_H

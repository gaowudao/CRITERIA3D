#ifndef ROOT_H
#define ROOT_H

    #ifndef SOIL_H
        #include "soil.h"
    #endif

    enum rootDistributionType {CYLINDRICAL_DISTRIBUTION, CARDIOID_DISTRIBUTION, GAMMA_DISTRIBUTION};
    enum rootGrowthType {LINEAR, EXPONENTIAL, LOGISTIC};

    class Crit3DCrop;

    class Crit3DRoot
    {
    public:
        rootDistributionType rootShape;
        rootGrowthType growth;

        /*!
         * \brief parameters
         */
        double degreeDaysRootGrowth;        /*!< [°D] */
        double rootDepthMin, rootDepthMax;  /*!< [m] */
        double shapeDeformation;            /*!< [-] */

        //todo:togliere se possibile
        double rootLengthMin;               // [m] non serve per criteria standard in cui è posto uguale a zero
        double rootLength;


        /*!
         * \brief variables
         */
        int firstRootLayer;                 /*!<  [-] */
        int lastRootLayer;                  /*!<  [-] */
        double* rootDensity;                /*!<  [] */
        double* transpiration;              /*!<  [mm] */

        /*!
         * \brief state variables
         */
        double rootDepth;                   /*!<  [m]  current root depth */

        Crit3DRoot();
        Crit3DRoot(rootDistributionType myShape, double myDepthMin, double myDepthMax, double myDegreeDaysRootGrowth);

    };

    namespace root
    {
        int nrAtoms(soil::Crit3DLayer* layers, int nrLayers, double rootDepthMin, double* minThickness, int* atoms);
        double getRootLengthDD(Crit3DRoot* myRoot, double currentDD, double emergenceDD);
        rootDistributionType getRootDistributionType(int rootShape);

        double computeRootLength(Crit3DCrop* myCrop, double soilDepth, double currentDD, double waterTableDepth);
        double computeRootDepth(Crit3DCrop* myCrop, double soilDepth, double currentDD, double waterTableDepth);
        bool computeRootDensity(Crit3DCrop* myCrop, soil::Crit3DLayer* layers, int nrLayers, double soilDepth);
    }

#endif // ROOT_H

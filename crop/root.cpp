#include <math.h>
#include <malloc.h>
#include <iostream>

#include "commonConstants.h"
#include "gammaFunction.h"
#include "root.h"
#include "crop.h"
#include "soil.h"


    Crit3DRoot::Crit3DRoot()
    {
        this->rootShape = CYLINDRICAL_DISTRIBUTION;
        this->growth = LOGISTIC;
        this->shapeDeformation = NODATA;
        this->degreeDaysRootGrowth = 0;
        this->rootDepthMin = NODATA;
        this->rootDepthMax = NODATA;
        this->firstRootLayer = NODATA;
        this->lastRootLayer = NODATA;
        this->rootDepth = 0;
        this->rootLengthMin = 0; // lo possiamo cambiare se volessimo supporre delle arboree giovani tipo
        this->rootDensity = NULL;
        this->transpiration = NULL;
    }

    Crit3DRoot::Crit3DRoot(rootDistributionType myShape, double myDepthMin, double myDepthMax, double myDegreeDaysRootGrowth)
    {
        rootShape = myShape;
        degreeDaysRootGrowth = myDegreeDaysRootGrowth;
        rootDepthMin = myDepthMin;
        rootDepthMax = myDepthMax;
        firstRootLayer = NODATA;
        lastRootLayer = NODATA;
        rootDepth = 0;
        rootLengthMin = 0; // lo possiamo cambiare se volessimo supporre delle arboree giovani tipo
        rootDensity = NULL;
        transpiration = NULL;
    }

namespace root
{
    rootDistributionType getRootDistributionType(int rootShape)
        {
            switch (rootShape)
            {
                case (1):
                    return CYLINDRICAL_DISTRIBUTION;
                    break;
                case (4):
                    return CARDIOID_DISTRIBUTION;
                    break;
                case (5):
                    return GAMMA_DISTRIBUTION;
                default:
                    return GAMMA_DISTRIBUTION;
             }
        }

    double computeRootDepth(Crit3DCrop* myCrop, double soilDepth, double currentDD, double waterTableDepth)
    {
        if (!(myCrop->isLiving))
        {
            myCrop->roots.rootLength = 0.0;
            myCrop->roots.rootDepth = NODATA;
        }
        else
        {
            myCrop->roots.rootLength = computeRootLength(myCrop, soilDepth, currentDD, waterTableDepth);
            myCrop->roots.rootDepth = myCrop->roots.rootDepthMin + myCrop->roots.rootLength;
        }

        return myCrop->roots.rootDepth;
    }


    // TODO this function computes the root length based on thermal units, it could be changed for perennial crops
    double computeRootLength(Crit3DCrop* myCrop, double soilDepth, double currentDD, double waterTableDepth)
    {
        double rootLength = NODATA;

        if (myCrop->roots.rootDepthMax > soilDepth)
        {
            myCrop->roots.rootDepthMax = soilDepth; // attenzione è diverso da criteria
            std::cout << "Warning: input root profile deeper than soil profile\n";
        }

        if (myCrop->isPluriannual())
        {
            rootLength = myCrop->roots.rootDepthMax - myCrop->roots.rootDepthMin;
        }
        else
        {
            if (currentDD <= 0)
                rootLength = myCrop->roots.rootLengthMin;
            else if (currentDD > myCrop->roots.degreeDaysRootGrowth)
                rootLength = myCrop->roots.rootDepthMax - myCrop->roots.rootDepthMin;
            else
            {
                // in order to avoid numerical divergences when calculating density through cardioid and gamma function
                currentDD = maxValue(currentDD, 1.0);
                rootLength = getRootLengthDD(&(myCrop->roots), currentDD, myCrop->degreeDaysEmergence);
            }
        }

        // WATERTABLE
        // le radici nel terreno saturo vanno in asfissia
        // per cui vanno mantenute a distanza nella fase di crescita
        // le radici possono crescere se:
        // la falda è più bassa o si abbassa (max 2 cm al giorno)
        // restano invariate se:
        // 1) non sono più in fase di crescita
        // 2) se sono già dentro la falda
        const float MAX_DAILY_GROWTH = 0.02f;             // [m]
        const float MIN_WATERTABLE_DISTANCE = 0.2f;       // [m]

        if (waterTableDepth != NODATA && waterTableDepth > 0 && myCrop->roots.rootLength != NODATA
                && !myCrop->isWaterSurplusResistant() && rootLength > myCrop->roots.rootLength)
        {
            // check on growth
            if (currentDD > myCrop->roots.degreeDaysRootGrowth)
                rootLength = myCrop->roots.rootLength;
            else
                rootLength = minValue(rootLength, myCrop->roots.rootLength + MAX_DAILY_GROWTH);

            // check on watertable
            float maxLenght = waterTableDepth - myCrop->roots.rootDepthMin - MIN_WATERTABLE_DISTANCE;
            if (rootLength > maxLenght)
            {
                rootLength = maxValue(myCrop->roots.rootLength, maxLenght);
            }
        }

        return rootLength;
    }


    //[m]
    double getRootLengthDD(Crit3DRoot* myRoot, double currentDD, double emergenceDD)
    {
        // this function computes the roots rate of development
        double rootLength = NODATA;
        double maxRootLength = myRoot->rootDepthMax - myRoot->rootDepthMin;

        if (currentDD <= 0) return 0.;
        if (currentDD > myRoot->degreeDaysRootGrowth) return maxRootLength;

        double halfDevelopmentPoint = myRoot->degreeDaysRootGrowth * 0.5 ;

        if (myRoot->growth == LINEAR)
        {
            rootLength = maxRootLength * (currentDD / myRoot->degreeDaysRootGrowth);
        }
        else if (myRoot->growth == LOGISTIC)
        {
            double logMax, logMin,deformationFactor;
            double iniLog = log(9.);
            double filLog = log(1 / 0.99 - 1);
            double k,b;
            k = -(iniLog - filLog) / (emergenceDD - myRoot->degreeDaysRootGrowth);
            b = -(filLog + k * myRoot->degreeDaysRootGrowth);

            logMax = (myRoot->rootDepthMax) / (1 + exp(-b - k * myRoot->degreeDaysRootGrowth));
            logMin = myRoot->rootDepthMax / (1 + exp(-b));
            deformationFactor = (logMax - logMin) / maxRootLength ;
            rootLength = 1.0 / deformationFactor * (myRoot->rootDepthMax / (1.0 + exp(-b - k * currentDD)) - logMin);
        }
        else if (myRoot->growth == EXPONENTIAL)
        {
            // not used in Criteria Bdp
            rootLength = maxRootLength * (1.- exp(-2.*(currentDD/halfDevelopmentPoint)));
        }

        return rootLength;
    }


    int highestCommonFactor(int* vector, int vectorDim)
    {
        // highest common factor (hcf) amongst n integer numbers
        int num1, num2, i, hcf;
        hcf = num1 = vector[0];
        for (int j=0; j<vectorDim-1; j++)
        {

            num1 = hcf;
            num2 = vector[j+1];

            for(i=1; i<=num1 || i<=num2; ++i)
            {
                if(num1%i==0 && num2%i==0)   /* Checking whether i is a factor of both number */
                    hcf=i;
            }
        }
        return hcf;
    }

    int checkTheOrderOfMagnitude(double number,int* order)
    {

        if (number<1)
        {
            number *= 10;
            (*order)--;
            checkTheOrderOfMagnitude(number,order);
        }
        else if (number >= 10)
        {
            number /=10;
            (*order)++;
            checkTheOrderOfMagnitude(number,order);
        }
        return 0;
    }

    int orderOfMagnitude(double number)
    {
        int order = 0;
        number = fabs(number);
        checkTheOrderOfMagnitude(number,&order);
        return order;
    }

    int nrAtoms(soil::Crit3DLayer* layers, int nrLayers, double rootDepthMin, double* minThickness, int* atoms)
    {
        int multiplicationFactor = 1;

        if (rootDepthMin > 0)
            *minThickness = rootDepthMin;
        else
            *minThickness = layers[1].thickness;

        for(int i=1; i<nrLayers; i++)
            *minThickness = minValue(*minThickness, layers[i].thickness);

        double tmp = *minThickness * 1.001;
        if (tmp < 1)
            multiplicationFactor = (int)(pow(10.0,-orderOfMagnitude(tmp)));

        if (*minThickness < 1)
        {
            *minThickness = 1./multiplicationFactor;
        }

        int value;
        int counter = 0;
        for(int i=0; i<nrLayers; i++)
        {
           value = int(round(multiplicationFactor * layers[i].thickness));
           atoms[i] = value;
           counter += value;
        }
        return counter;
    }


    void cardioidDistribution(double shapeFactor, int nrLayersWithRoot,int nrUpperLayersWithoutRoot , int totalLayers,double* densityThinLayers)
    {
        double *lunette =  (double *) calloc(2*nrLayersWithRoot, sizeof(double));
        double *lunetteDensity = (double *) calloc(2*nrLayersWithRoot, sizeof(double));
        for (int i = 0 ; i<nrLayersWithRoot ; i++)
        {
            double sinAlfa, cosAlfa, alfa;
            sinAlfa = 1.0 - (double)(1+i)/((double)(nrLayersWithRoot)) ;
            cosAlfa = maxValue(sqrt(1.0 - pow(sinAlfa,2)), 0.0001);
            alfa = atan(sinAlfa/cosAlfa);
            lunette[i]= ((PI/2) - alfa - sinAlfa*cosAlfa) / PI;
        }

        lunetteDensity[2*nrLayersWithRoot - 1]= lunetteDensity[0] = lunette[0];
        for (int i = 1 ; i<nrLayersWithRoot ; i++)
        {
            lunetteDensity[2*nrLayersWithRoot - i - 1]=lunetteDensity[i]=lunette[i]-lunette[i-1] ;
        }

        // cardioid deformation
        double LiMin,Limax,k,rootDensitySum ;
        LiMin = -log(0.2)/nrLayersWithRoot;
        Limax = -log(0.05)/nrLayersWithRoot;
        k = LiMin + (Limax - LiMin)*(shapeFactor-1); //da controllare
        rootDensitySum = 0 ;
        for (int i = 0 ; i<(2*nrLayersWithRoot) ; i++)
        {
            lunetteDensity[i] *= exp(-k*(i+0.5)); //changed from basic to C
            rootDensitySum += lunetteDensity[i];
        }
        for (int i = 0 ; i<(2*nrLayersWithRoot) ; i++)
        {
            lunetteDensity[i] /= rootDensitySum ;
        }
        for  (int i = 0 ; i<totalLayers ; i++)
        {
            densityThinLayers[i]=0;
        }
        for (int i = 0 ; i<nrLayersWithRoot ; i++)
        {
            densityThinLayers[nrUpperLayersWithoutRoot+i]=lunetteDensity[2*i] + lunetteDensity[2*i+1] ;
        }

        free(lunette);
        free(lunetteDensity);
    }

    void cylindricalDistribution(double deformation, int nrLayersWithRoot,int nrUpperLayersWithoutRoot , int totalLayers,double* densityThinLayers)
    {

       int i;

       double *cylinderDensity =  (double *) calloc(2*nrLayersWithRoot, sizeof(double));
       for (i = 0 ; i<2*nrLayersWithRoot; i++)
       {
           cylinderDensity[i]= 1./(2*nrLayersWithRoot);
       } // not deformed cylinder
       // linear and ovoidal deformation
       double deltaDeformation,rootDensitySum;
       rootDensitySum =0;
       deltaDeformation = deformation - 1;

       for (i = 0 ; i<nrLayersWithRoot ; i++)
       {
           cylinderDensity[i] *= deformation;
           deformation -= deltaDeformation/nrLayersWithRoot;
           rootDensitySum += cylinderDensity[i];
       }
       for (i = nrLayersWithRoot ; i<2*nrLayersWithRoot ; i++)
       {
           deformation -= deltaDeformation / nrLayersWithRoot;
           cylinderDensity[i] *= deformation;
           rootDensitySum += cylinderDensity[i];
       }
       for (i = nrLayersWithRoot ; i<2*nrLayersWithRoot ; i++)
       {
           cylinderDensity[i] /= rootDensitySum;
       }
       for  (i = 0 ; i<totalLayers ; i++)
       {
           densityThinLayers[i] = 0;
       }
       for (i = 0 ; i<nrLayersWithRoot ; i++)
       {
           densityThinLayers[nrUpperLayersWithoutRoot+i] = cylinderDensity[2*i] + cylinderDensity[2*i+1] ;
       }
       free(cylinderDensity);
    }


    bool computeRootDensity(Crit3DCrop* myCrop, soil::Crit3DLayer* layers, int nrLayers, double soilDepth)
    {
        int i, j, layer;

        // Initialize
        for (i=0; i<nrLayers; i++)
            myCrop->roots.rootDensity[i]=0.0;

        if ((! myCrop->isLiving) || (myCrop->roots.rootLength <= 0 )) return true;

        if ((myCrop->roots.rootShape == CARDIOID_DISTRIBUTION) || (myCrop->roots.rootShape == CYLINDRICAL_DISTRIBUTION))
        {
            double minimumThickness;
            int *atoms =  (int *) calloc(nrLayers, sizeof(int));
            int numberOfRootedLayers, numberOfTopUnrootedLayers, totalLayers;
            totalLayers = root::nrAtoms(layers, nrLayers, myCrop->roots.rootDepthMin, &minimumThickness, atoms);
            numberOfTopUnrootedLayers = int(round(myCrop->roots.rootDepthMin / minimumThickness));
            numberOfRootedLayers = int(ceil(minValue(myCrop->roots.rootLength, soilDepth) / minimumThickness));
            double *densityThinLayers =  (double *) calloc(totalLayers+1, sizeof(double));
            densityThinLayers[totalLayers] = 0.;
            for (i=0; i < totalLayers; i++)
                densityThinLayers[i] = 0.;

            if (myCrop->roots.rootShape == CARDIOID_DISTRIBUTION)
            {
                cardioidDistribution(myCrop->roots.shapeDeformation, numberOfRootedLayers,
                                     numberOfTopUnrootedLayers, totalLayers, densityThinLayers);
            }
            else if (myCrop->roots.rootShape == CYLINDRICAL_DISTRIBUTION)
            {
                cylindricalDistribution(myCrop->roots.shapeDeformation, numberOfRootedLayers,
                                        numberOfTopUnrootedLayers, totalLayers, densityThinLayers);
            }

            int counter=0;
            for (layer=0; layer<nrLayers; layer++)
            {
                for (j=counter; j<counter + atoms[layer]; j++)
                {
                    if (j < totalLayers)
                        myCrop->roots.rootDensity[layer] += densityThinLayers[j];
                }
                counter = j;
            }
            free(atoms);
            free(densityThinLayers);
        }
        else if (myCrop->roots.rootShape == GAMMA_DISTRIBUTION)
        {
            double normalizationFactor ;
            double kappa, theta,a,b;
            double mean, mode;
            mean = myCrop->roots.rootLength * 0.5;
            mode = myCrop->roots.rootLength * 0.2;
            theta = mean - mode;
            kappa = mean / theta;
            // complete gamma function
            normalizationFactor = Gamma_Function(kappa);

            for (i=1 ; i<nrLayers ; i++)
            {
                b = maxValue(layers[i].depth + layers[i].thickness*0.5 - myCrop->roots.rootDepthMin,0); // right extreme
                if (b>0)
                {
                    a = maxValue(layers[i].depth - layers[i].thickness*0.5 - myCrop->roots.rootDepthMin,0); //left extreme
                    myCrop->roots.rootDensity[i] = Incomplete_Gamma_Function(b/theta,kappa) - Incomplete_Gamma_Function(a/theta,kappa);
                    myCrop->roots.rootDensity[i] /= normalizationFactor;
                }
            }
        }

        double rootDensitySum = 0. ;
        for (i=0 ; i<nrLayers ; i++)
        {
            myCrop->roots.rootDensity[i] *= layers[i].soilFraction;
            rootDensitySum += myCrop->roots.rootDensity[i];
        }

        if (rootDensitySum > 0.0)
        {
            for (i=0 ; i<nrLayers ; i++)
                myCrop->roots.rootDensity[i] /= rootDensitySum;

            myCrop->roots.firstRootLayer = 0;
            layer = 0;
            while ((myCrop->roots.rootDensity[layer] == 0)
                   && (layer < nrLayers))
            {
                layer++;
                (myCrop->roots.firstRootLayer)++;
            }

            myCrop->roots.lastRootLayer = myCrop->roots.firstRootLayer;
            while ((myCrop->roots.rootDensity[layer] != 0)
                && (layer < nrLayers))
            {
                (myCrop->roots.lastRootLayer) = layer;
                layer++;
            }
        }

        return true;
    }
}

#include <stdio.h>
#include <math.h>

#include "models.h"

double rainfallCanopyInterceptionHydrall(double laiCanopy,double laiUnderstorey, double canopyCover, double prec)
{
    double canopyCapacity;
    double interception;
    double maxInterception;
    double upperBoundPrec = 20;
    if (prec > upperBoundPrec) maxInterception = 0.15 * upperBoundPrec;
    else maxInterception = 0.15 * prec;
    canopyCapacity = 0.07/24. * (laiCanopy*canopyCover + laiUnderstorey);
    if (canopyCapacity > maxInterception) interception = maxInterception ;
    else interception = canopyCapacity;
    printf("%f %f \n", canopyCapacity,maxInterception);
    return interception;
}

namespace roughness {
    //enum {ASPHALT,BARESOIL,CROP,GRASS,FALLOW,ORCHARD,SHRUBS,FOREST,FOREST_LITTER,FOREST_UNDERSTORY};
    double manningCoefficient(int landscapeType)
    {
        // values taken from engineering toolbox + Arcement, Jr. and V.R. Schneider, USGS
        double coefficientManning;
        if (landscapeType == ASPHALT) coefficientManning = 0.016;
        if (landscapeType == BARESOIL) coefficientManning = 0.03;
        if (landscapeType == CROP) coefficientManning = 0.035;
        if (landscapeType == GRASS) coefficientManning = 0.05;
        if (landscapeType == FALLOW) coefficientManning = 0.05;
        if (landscapeType == SHRUBS) coefficientManning = 0.075;
        if (landscapeType == FOREST) coefficientManning = 0.1;
        if (landscapeType == FOREST_LITTER) coefficientManning = 0.15;
        if (landscapeType == FOREST_UNDERSTORY) coefficientManning = 0.2;
        return coefficientManning;
    }



}
namespace canopy {

    double plantCover(double lai, double extinctionCoefficient,double laiMin)
    {
        if (lai < laiMin) lai=laiMin;
        return (1-exp(-extinctionCoefficient*(lai))); //Van Dijk  A.I.J.M, Bruijnzeel L.A., 2001
    }

    double waterStorageCapacity(double lai,double leafStorage,double stemStorage)
    {
        return leafStorage*lai + stemStorage; //Van Dijk  A.I.J.M, Bruijnzeel L.A., 2001
    }

    double freeRainThroughfall(double rainfall,double cover)
    {
        return (rainfall*(1 - cover));
    }

    double rainfallInterceptedByCanopy(double rainfall,double cover)
    {
        return (rainfall*cover);
    }
    double evaporationFromCanopy(double waterFreeEvaporation, double storage,double grossStorage)
    {
        if (grossStorage < 0.1*storage) return grossStorage;

        if (grossStorage >= storage) return waterFreeEvaporation;
        else
        {
            return waterFreeEvaporation*grossStorage/storage ;
        }
    }

    double drainageFromTree(double grossStorage, double storage)
    {
        if (grossStorage > storage) return (grossStorage - storage);
        else return 0;
    }

    double stemFlowRate(double maxStemFlowRate)
    {
        return maxStemFlowRate;
    }

    bool waterManagementCanopy(double* storedWater, double* throughfallWater, double rainfall, double waterFreeEvaporation, double lai, double extinctionCoefficient, double leafStorage, double stemStorage,double maxStemFlowRate, double* freeRainfall, double *drainage, double* stemFlow, double laiMin)
    {
        double actualCover,actualStorage,grossStorage;
        double interception;
        actualCover = plantCover(lai,extinctionCoefficient,laiMin);
        actualStorage = waterStorageCapacity(lai,leafStorage,stemStorage);
        *freeRainfall = freeRainThroughfall(rainfall,actualCover);
        interception = rainfallInterceptedByCanopy(rainfall,actualCover);
        grossStorage = *storedWater + interception;
        *drainage = drainageFromTree(grossStorage,actualStorage);
        *stemFlow = (*drainage)*stemFlowRate(maxStemFlowRate);
        *throughfallWater = *freeRainfall + (*drainage)-(*stemFlow);
        grossStorage -= evaporationFromCanopy(waterFreeEvaporation,actualStorage,grossStorage);
        *storedWater = grossStorage - (*drainage);
        return true;
    }
}


namespace litter {

    double evaporationMulching(double storageCapacity)
    {
        // daily?
        // fraction of evaporation saved because of the litter mulching effect
        return (0.5 * (storageCapacity / 3.138535156));
    }

    void waterManagementLitter(double* storedWater, double* throughfallWater, double rainfall,double waterFreeEvaporation,double storageCapacity)
    {
        double grossStorage;
        *throughfallWater = rainfall/2;
        grossStorage = *storedWater + rainfall/2;
        //printf("gross %f\n",grossStorage);
        if (grossStorage > storageCapacity)
            grossStorage -= waterFreeEvaporation;
        else
            grossStorage -= waterFreeEvaporation * grossStorage / storageCapacity;
        //printf("gross %f\n",grossStorage);

        if (grossStorage > storageCapacity)
        {
            *throughfallWater += grossStorage - storageCapacity;
            *storedWater = storageCapacity;
        }
        else
        {

            *storedWater = grossStorage;
        }


    }

    double storageCapacity(int doy)
    {
        double litterPotentialholding[366]= {3.064	,
                                             3.047030166	,
                                             3.030060333	,
                                             3.013090499	,
                                             2.996120665	,
                                             2.979150832	,
                                             2.962180998	,
                                             2.945211164	,
                                             2.928241331	,
                                             2.911271497	,
                                             2.894301663	,
                                             2.87733183	,
                                             2.860361996	,
                                             2.843392162	,
                                             2.826422329	,
                                             2.809452495	,
                                             2.792482661	,
                                             2.775512828	,
                                             2.758542994	,
                                             2.74157316	,
                                             2.724603327	,
                                             2.707633493	,
                                             2.690663659	,
                                             2.673693826	,
                                             2.656723992	,
                                             2.639754158	,
                                             2.622784325	,
                                             2.605814491	,
                                             2.588844657	,
                                             2.571874824	,
                                             2.55490499	,
                                             2.537935156	,
                                             2.520751423	,
                                             2.50356769	,
                                             2.486383956	,
                                             2.469200223	,
                                             2.45201649	,
                                             2.434832757	,
                                             2.417649023	,
                                             2.40046529	,
                                             2.383281557	,
                                             2.366097824	,
                                             2.34891409	,
                                             2.331730357	,
                                             2.314546624	,
                                             2.297362891	,
                                             2.280179157	,
                                             2.262995424	,
                                             2.245811691	,
                                             2.228627958	,
                                             2.211444224	,
                                             2.194260491	,
                                             2.177076758	,
                                             2.159893025	,
                                             2.142709291	,
                                             2.125525558	,
                                             2.108341825	,
                                             2.091158092	,
                                             2.073974358	,
                                             2.056790625	,
                                             2.05480436	,
                                             2.052818095	,
                                             2.05083183	,
                                             2.048845565	,
                                             2.046859299	,
                                             2.044873034	,
                                             2.042886769	,
                                             2.040900504	,
                                             2.038914239	,
                                             2.036927974	,
                                             2.034941709	,
                                             2.032955444	,
                                             2.030969178	,
                                             2.028982913	,
                                             2.026996648	,
                                             2.025010383	,
                                             2.023024118	,
                                             2.021037853	,
                                             2.019051588	,
                                             2.017065323	,
                                             2.015079057	,
                                             2.013092792	,
                                             2.011106527	,
                                             2.009120262	,
                                             2.007133997	,
                                             2.005147732	,
                                             2.003161467	,
                                             2.001175202	,
                                             1.999188936	,
                                             1.997202671	,
                                             1.995216406	,
                                             1.993121444	,
                                             1.991026482	,
                                             1.988931519	,
                                             1.986836557	,
                                             1.984741595	,
                                             1.982646633	,
                                             1.98055167	,
                                             1.978456708	,
                                             1.976361746	,
                                             1.974266783	,
                                             1.972171821	,
                                             1.970076859	,
                                             1.967981897	,
                                             1.965886934	,
                                             1.963791972	,
                                             1.96169701	,
                                             1.959602047	,
                                             1.957507085	,
                                             1.955412123	,
                                             1.953317161	,
                                             1.951222198	,
                                             1.949127236	,
                                             1.947032274	,
                                             1.944937311	,
                                             1.942842349	,
                                             1.940747387	,
                                             1.938652425	,
                                             1.936557462	,
                                             1.9344625	,
                                             1.932464714	,
                                             1.930466927	,
                                             1.928469141	,
                                             1.926471354	,
                                             1.924473568	,
                                             1.922475781	,
                                             1.920477995	,
                                             1.918480208	,
                                             1.916482422	,
                                             1.914484635	,
                                             1.912486849	,
                                             1.910489063	,
                                             1.908491276	,
                                             1.90649349	,
                                             1.904495703	,
                                             1.902497917	,
                                             1.90050013	,
                                             1.898502344	,
                                             1.896504557	,
                                             1.894506771	,
                                             1.892508984	,
                                             1.890511198	,
                                             1.888513411	,
                                             1.886515625	,
                                             1.884517839	,
                                             1.882520052	,
                                             1.880522266	,
                                             1.878524479	,
                                             1.876526693	,
                                             1.874528906	,
                                             1.872622026	,
                                             1.870715146	,
                                             1.868808266	,
                                             1.866901386	,
                                             1.864994506	,
                                             1.863087626	,
                                             1.861180746	,
                                             1.859273866	,
                                             1.857366986	,
                                             1.855460106	,
                                             1.853553226	,
                                             1.851646346	,
                                             1.849739466	,
                                             1.847832586	,
                                             1.845925706	,
                                             1.844018826	,
                                             1.842111946	,
                                             1.840205066	,
                                             1.838298185	,
                                             1.836391305	,
                                             1.834484425	,
                                             1.832577545	,
                                             1.830670665	,
                                             1.828763785	,
                                             1.826856905	,
                                             1.824950025	,
                                             1.823043145	,
                                             1.821136265	,
                                             1.819229385	,
                                             1.817296043	,
                                             1.815415625	,
                                             1.813535207	,
                                             1.811654788	,
                                             1.80977437	,
                                             1.807893952	,
                                             1.806013533	,
                                             1.804133115	,
                                             1.802252697	,
                                             1.800372278	,
                                             1.79849186	,
                                             1.796611442	,
                                             1.794731023	,
                                             1.792850605	,
                                             1.790970186	,
                                             1.789089768	,
                                             1.78720935	,
                                             1.785328931	,
                                             1.783448513	,
                                             1.781568095	,
                                             1.779687676	,
                                             1.777807258	,
                                             1.77592684	,
                                             1.774046421	,
                                             1.772166003	,
                                             1.770285585	,
                                             1.768405166	,
                                             1.766524748	,
                                             1.76464433	,
                                             1.762763911	,
                                             1.760883493	,
                                             1.759003075	,
                                             1.757122656	,
                                             1.7552687	,
                                             1.753414743	,
                                             1.751560786	,
                                             1.74970683	,
                                             1.747852873	,
                                             1.745998916	,
                                             1.74414496	,
                                             1.742291003	,
                                             1.740437046	,
                                             1.73858309	,
                                             1.736729133	,
                                             1.734875176	,
                                             1.73302122	,
                                             1.731167263	,
                                             1.729313306	,
                                             1.72745935	,
                                             1.725605393	,
                                             1.723751436	,
                                             1.72189748	,
                                             1.720043523	,
                                             1.718189567	,
                                             1.71633561	,
                                             1.714481653	,
                                             1.712627697	,
                                             1.71077374	,
                                             1.708919783	,
                                             1.707065827	,
                                             1.70521187	,
                                             1.703357913	,
                                             1.701503957	,
                                             1.69965	,
                                             1.697761589	,
                                             1.695873177	,
                                             1.693984766	,
                                             1.692096354	,
                                             1.690207943	,
                                             1.688319531	,
                                             1.68643112	,
                                             1.684542708	,
                                             1.682654297	,
                                             1.680765885	,
                                             1.678877474	,
                                             1.676989063	,
                                             1.675100651	,
                                             1.67321224	,
                                             1.671323828	,
                                             1.669435417	,
                                             1.667547005	,
                                             1.665658594	,
                                             1.663770182	,
                                             1.661881771	,
                                             1.659993359	,
                                             1.658104948	,
                                             1.656216536	,
                                             1.654328125	,
                                             1.652439714	,
                                             1.650551302	,
                                             1.648662891	,
                                             1.646774479	,
                                             1.644886068	,
                                             1.642997656	,
                                             1.645563558	,
                                             1.648129461	,
                                             1.650695363	,
                                             1.653261265	,
                                             1.655827167	,
                                             1.65839307	,
                                             1.660958972	,
                                             1.663524874	,
                                             1.666090776	,
                                             1.668656678	,
                                             1.671222581	,
                                             1.673788483	,
                                             1.676354385	,
                                             1.678920287	,
                                             1.68148619	,
                                             1.684052092	,
                                             1.686617994	,
                                             1.689183896	,
                                             1.691749798	,
                                             1.694315701	,
                                             1.696881603	,
                                             1.699447505	,
                                             1.702013407	,
                                             1.704579309	,
                                             1.707145212	,
                                             1.709711114	,
                                             1.712277016	,
                                             1.714842918	,
                                             1.717408821	,
                                             1.719974723	,
                                             1.722540625	,
                                             1.769740443	,
                                             1.81694026	,
                                             1.864140078	,
                                             1.911339896	,
                                             1.958539714	,
                                             2.005739531	,
                                             2.052939349	,
                                             2.100139167	,
                                             2.147338984	,
                                             2.194538802	,
                                             2.24173862	,
                                             2.288938438	,
                                             2.336138255	,
                                             2.383338073	,
                                             2.430537891	,
                                             2.477737708	,
                                             2.524937526	,
                                             2.572137344	,
                                             2.619337161	,
                                             2.666536979	,
                                             2.713736797	,
                                             2.760936615	,
                                             2.808136432	,
                                             2.85533625	,
                                             2.902536068	,
                                             2.949735885	,
                                             2.996935703	,
                                             3.044135521	,
                                             3.091335339	,
                                             3.138535156	,
                                             3.136205933	,
                                             3.133876709	,
                                             3.131547485	,
                                             3.129218262	,
                                             3.126889038	,
                                             3.124559814	,
                                             3.122230591	,
                                             3.119901367	,
                                             3.117572144	,
                                             3.11524292	,
                                             3.112913696	,
                                             3.110584473	,
                                             3.108255249	,
                                             3.105926025	,
                                             3.103596802	,
                                             3.101267578	,
                                             3.098938354	,
                                             3.096609131	,
                                             3.094279907	,
                                             3.091950684	,
                                             3.08962146	,
                                             3.087292236	,
                                             3.084963013	,
                                             3.082633789	,
                                             3.080304565	,
                                             3.077975342	,
                                             3.075646118	,
                                             3.073316895	,
                                             3.070987671	,
                                             3.068658447	,
                                             3.066329224	,
                                             3.064};

        return litterPotentialholding[doy-1];
    }

}

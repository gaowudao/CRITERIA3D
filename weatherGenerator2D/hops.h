#ifndef HOPS_H
#define HOPS_H

#include "meteoPoint.h"

struct Tphenology{
    double sproutingDD;
    double floweringDD;
    double conesDevelopmentDD;
    double conesRipeningDD;
    double conesMaturityDD;
};

struct Tevapotranspiration{
    double sproutingKc;
    double floweringKc;
    double conesDevelopmentKc;
    double conesRipeningKc;
    double conesMaturityKc;
};

class hops
{
    private:
    int nrData;
    int nrStations;

    double baseTemperature = 5;

    void computePhenology();
    void computeEvapotranspration();

    public:
    //
    Tphenology phenology;
    Tevapotranspiration evapotranspiration;
    TObsDataD** obsDataD;
    hops()
    {

    }
    void initializeData(int lengthDataSeries, int nrStations);
    void initializeParametersPhenology(Tphenology pheno);
    void initializeParametersEvapotranspiration(Tevapotranspiration Kc);
    void setObservedData(TObsDataD** observations);
    void compute();
};
#endif // HOPS_H

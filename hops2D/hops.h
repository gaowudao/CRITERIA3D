#ifndef HOPS_H
#define HOPS_H

#include "meteoPoint.h"

struct Tphenology{
    double sproutingDD;
    double floweringDD;
    double conesDevelopmentDD;
    double conesRipeningDD;
    double conesMaturityDD;
    double senescenceDD;
};

struct Tevapotranspiration{
    double sproutingKc;
    double floweringKc;
    double conesDevelopmentKc;
    double conesRipeningKc;
    double conesMaturityKc;
    double senescenceKc;
};

struct ToutputPhenology{
    //double phenologicalStage;
    int doySprouting;
    int doyFlowering;
    int doyConesDevelopment;
    int doyConesRipening;
    int doyConesMaturity;
    int doySenescence;
    int year;
    int nrLackingTemperatureData=0;
    double thermalSum[365];
};

class hops
{
    private:
    int nrData;
    int nrYears=0;
    int nrStations;
    double thermalSum = 0;
    double phenologicalStage = 0;
    double baseTemperature = 5;

    void computeAverageTemperatures();
    void computePhenology();
    void computeEvapotranspration();
    void initializeOutputsPhenology();
    int doyFromDate(int day,int month,int year);

    public:
    //
    Tphenology phenology;
    Tevapotranspiration evapotranspiration;
    ToutputPhenology** outputPhenology;
    TObsDataD** obsDataD;
    hops()
    {
        outputPhenology = nullptr;
        obsDataD = nullptr;
    }
    void initializeData(int lengthDataSeries, int nrStations);
    void initializeParametersPhenology(Tphenology pheno);
    void initializeParametersEvapotranspiration(Tevapotranspiration Kc);
    void setObservedData(TObsDataD** observations);
    void compute();
};
#endif // HOPS_H

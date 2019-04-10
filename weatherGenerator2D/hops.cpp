#include <stdio.h>
#include <math.h>
#include <malloc.h>

#include "meteoPoint.h"
#include "commonConstants.h"
#include "furtherMathFunctions.h"
#include "hops.h"

void hops::initializeData(int lengthDataSeries, int stations)
{
    nrData = lengthDataSeries;
    nrStations = stations;

    // use of PRAGA formats from meteoPoint.h
    obsDataD = (TObsDataD **)calloc(nrStations, sizeof(TObsDataD*));

    for (int i=0;i<nrStations;i++)
    {
        obsDataD[i] = (TObsDataD *)calloc(nrData, sizeof(TObsDataD));
    }
    for(int i=0;i<nrStations;i++)
    {
        for (int j=0;j<nrData;j++)
        {
            obsDataD[i][j].date.day = NODATA;
            obsDataD[i][j].date.month = NODATA;
            obsDataD[i][j].date.year = NODATA;
            obsDataD[i][j].tMin = NODATA;
            obsDataD[i][j].tMax = NODATA;
            obsDataD[i][j].prec = NODATA;
            //printf("%d/%d/%d,%.1f,%.1f,%.1f\n",observations[i][j].date.day,observations[i][j].date.month,observations[i][j].date.year,observations[i][j].tMin,observations[i][j].tMax,observations[i][j].prec);

        }
        //pressEnterToContinue();
    }


}

void hops::setObservedData(TObsDataD** observations)
{
    for(int i=0;i<nrStations;i++)
    {
        for (int j=0;j<nrData;j++)
        {
            obsDataD[i][j].date.day = observations[i][j].date.day ;
            obsDataD[i][j].date.month = observations[i][j].date.month;
            obsDataD[i][j].date.year = observations[i][j].date.year;
            obsDataD[i][j].tMin = observations[i][j].tMin;
            obsDataD[i][j].tMax = observations[i][j].tMax;
            obsDataD[i][j].prec = observations[i][j].prec;
            //printf("%d/%d/%d,%.1f,%.1f,%.1f\n",observations[i][j].date.day,observations[i][j].date.month,observations[i][j].date.year,observations[i][j].tMin,observations[i][j].tMax,observations[i][j].prec);

        }
        //pressEnterToContinue();
    }
}

void hops::initializeParametersPhenology(Tphenology pheno)
{
    phenology.sproutingDD = pheno.sproutingDD;
    phenology.floweringDD = pheno.floweringDD;
    phenology.conesDevelopmentDD = pheno.conesDevelopmentDD;
    phenology.conesRipeningDD = pheno.conesRipeningDD;
    phenology.conesMaturityDD = pheno.conesMaturityDD;
}

void hops::initializeParametersEvapotranspiration(Tevapotranspiration cropCoefficient)
{
    evapotranspiration.sproutingKc = cropCoefficient.sproutingKc;
    evapotranspiration.floweringKc = cropCoefficient.floweringKc;
    evapotranspiration.conesDevelopmentKc = cropCoefficient.conesDevelopmentKc;
    evapotranspiration.conesRipeningKc = cropCoefficient.conesRipeningKc;
    evapotranspiration.conesMaturityKc = cropCoefficient.conesMaturityKc;
}


void hops::compute()
{
    hops::computePhenology();
    hops::computeEvapotranspration();
}

void hops::computePhenology()
{

}

void hops::computeEvapotranspration()
{

}

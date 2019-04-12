#include <stdio.h>
#include <math.h>
#include <malloc.h>

#include "meteoPoint.h"
#include "commonConstants.h"
#include "furtherMathFunctions.h"
#include "hops.h"

int hops::doyFromDate(int day,int month,int year)
{
    int daysOfMonth[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
    if (isLeapYear(year)) (daysOfMonth[1])++;
    if (month == 1)
    {
        return day;
    }
    int doy = 0;
    int counter =0;
    while (counter < (month-1))
    {
        doy += daysOfMonth[counter];
        counter++;
    }
    doy += day;
    return doy;
}

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
    //int minYear,maxYear;
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
        //getchar();
    }
    nrYears = obsDataD[0][nrData-1].date.year - obsDataD[0][0].date.year + 1;
}

void hops::initializeParametersPhenology(Tphenology pheno)
{
    phenology.sproutingDD = pheno.sproutingDD;
    phenology.floweringDD = pheno.floweringDD;
    phenology.conesDevelopmentDD = pheno.conesDevelopmentDD;
    phenology.conesRipeningDD = pheno.conesRipeningDD;
    phenology.conesMaturityDD = pheno.conesMaturityDD;
    phenology.senescenceDD = pheno.senescenceDD;
    //printf("%f\n",phenology.sproutingDD);
    //getchar();
}

void hops::initializeParametersEvapotranspiration(Tevapotranspiration cropCoefficient)
{
    evapotranspiration.sproutingKc = cropCoefficient.sproutingKc;
    evapotranspiration.floweringKc = cropCoefficient.floweringKc;
    evapotranspiration.conesDevelopmentKc = cropCoefficient.conesDevelopmentKc;
    evapotranspiration.conesRipeningKc = cropCoefficient.conesRipeningKc;
    evapotranspiration.conesMaturityKc = cropCoefficient.conesMaturityKc;
    evapotranspiration.senescenceKc = evapotranspiration.sproutingKc;
}

void hops::initializeOutputsPhenology()
{
    outputPhenology = (ToutputPhenology **)calloc(nrStations, sizeof(ToutputPhenology*));
    for  (int i=0; i<nrStations; i++)
        outputPhenology[i] = (ToutputPhenology *)calloc(nrYears, sizeof(ToutputPhenology));
    for (int i=0; i<nrStations; i++)
    {
        for (int k=0; k<nrYears; k++)
        {
            outputPhenology[i][k].doySprouting = NODATA;
            outputPhenology[i][k].doyFlowering = NODATA;
            outputPhenology[i][k].doyConesRipening = NODATA;
            outputPhenology[i][k].doyConesMaturity = NODATA;
            outputPhenology[i][k].doyConesDevelopment = NODATA;
            outputPhenology[i][k].year = NODATA;
            outputPhenology[i][k].nrLackingTemperatureData = 0;
            for (int j=0;j<365;j++)
            {
                outputPhenology[i][k].thermalSum[j] = 0;
            }
        }
    }
}

void hops::compute()
{
    //hops::computeAverageTemperatures();
    hops::computePhenology();
    hops::computeEvapotranspration();
}

void hops::computeAverageTemperatures()
{
    for (int iStation=0; iStation<nrStations; iStation++)
    {
        for (int iDatum = 0; iDatum<nrData; iDatum++)
        {
            if ((fabs(obsDataD[iStation][iDatum].tMax - NODATA) > EPSILON) && (fabs(obsDataD[iStation][iDatum].tMin - NODATA) > EPSILON))
                obsDataD[iStation][iDatum].tAvg = 0.5*(obsDataD[iStation][iDatum].tMax + obsDataD[iStation][iDatum].tMin);
            else
            {
                int iYear;
                iYear = obsDataD[iStation][iDatum].date.year - obsDataD[iStation][0].date.year;
                outputPhenology[iStation][iYear].nrLackingTemperatureData = 1 + outputPhenology[iStation][iYear].nrLackingTemperatureData;
                //printf("%d %d %d\n",iYear,obsDataD[iStation][iDatum].date.year,obsDataD[iStation][0].date.year);
                printf("%d %d nryear\n",outputPhenology[iStation][iYear].nrLackingTemperatureData,iYear);
                if ((fabs(obsDataD[iStation][iDatum-1].tAvg - NODATA) > EPSILON) && iDatum>0)
                    obsDataD[iStation][iDatum].tAvg = obsDataD[iStation][iDatum-1].tAvg;
                else
                    obsDataD[iStation][iDatum].tAvg = 10;
            }
            //printf("%f  %f  %f,",obsDataD[iStation][iDatum].tMax,obsDataD[iStation][iDatum].tAvg,obsDataD[iStation][iDatum].tMin);
            //getchar();
        }
    }
}

void hops::computePhenology()
{
    hops::initializeOutputsPhenology();
    hops::computeAverageTemperatures();
    for (int iStation=0; iStation<nrStations; iStation++)
    {
        int iDatum = 0;
        double thermalSumPrevious=0;
        thermalSum = 0;
        while (iDatum < nrData)
        {
            int dayOfYear;
            int iYear;
            //int test;

            iYear = obsDataD[iStation][iDatum].date.year - obsDataD[iStation][0].date.year;
            dayOfYear = hops::doyFromDate(obsDataD[iStation][iDatum].date.day,obsDataD[iStation][iDatum].date.month,obsDataD[iStation][iDatum].date.year);
            if (dayOfYear == 1)
            {
                thermalSum = 0;
                thermalSumPrevious = 0;
                outputPhenology[iStation][iYear].year = obsDataD[iStation][iDatum].date.year;
            }

            if ((thermalSum >= phenology.sproutingDD) && (thermalSumPrevious < phenology.sproutingDD))
                outputPhenology[iStation][iYear].doySprouting = dayOfYear;
            if (thermalSum >= phenology.floweringDD && thermalSumPrevious < phenology.floweringDD)
                outputPhenology[iStation][iYear].doyFlowering = dayOfYear;
            if (thermalSum >= phenology.conesDevelopmentDD && thermalSumPrevious < phenology.conesDevelopmentDD)
                outputPhenology[iStation][iYear].doyConesDevelopment = dayOfYear;
            if (thermalSum >= phenology.conesRipeningDD && thermalSumPrevious < phenology.conesRipeningDD)
                outputPhenology[iStation][iYear].doyConesRipening = dayOfYear;
            if (thermalSum >= phenology.conesMaturityDD && thermalSumPrevious < phenology.conesMaturityDD)
                outputPhenology[iStation][iYear].doyConesMaturity = dayOfYear;
            if (thermalSum >= phenology.senescenceDD && thermalSumPrevious < phenology.senescenceDD)
                outputPhenology[iStation][iYear].doySenescence = dayOfYear;

            //printf("%d %f %f\n",dayOfYear,thermalSum,outputPhenology[iStation][iYear].thermalSum[dayOfYear]);
            outputPhenology[iStation][iYear].thermalSum[dayOfYear]= thermalSum;
            if (dayOfYear == 360)
            //printf("%d %d %d %d %d %d\n",outputPhenology[iStation][iYear].doySprouting,outputPhenology[iStation][iYear].doyFlowering,outputPhenology[iStation][iYear].doyConesDevelopment,outputPhenology[iStation][iYear].doyConesRipening,outputPhenology[iStation][iYear].doyConesMaturity,outputPhenology[iStation][iYear].doySenescence);
            //getchar();
            thermalSumPrevious = thermalSum;
            thermalSum += maxValue(obsDataD[iStation][iDatum].tAvg - baseTemperature,0);
            iDatum++;
        }
        //printf("pressenter");getchar();
    }
}

void hops::computeEvapotranspration()
{
    float actualKc;
    float* degDays = (float *)calloc(5, sizeof(float));
    float* Kc = (float *)calloc(5, sizeof(float));
    Kc[0] = evapotranspiration.sproutingKc;
    Kc[1] = evapotranspiration.conesDevelopmentKc;
    Kc[2] = evapotranspiration.conesRipeningKc;
    Kc[3] = evapotranspiration.conesMaturityKc;
    Kc[4] = evapotranspiration.senescenceKc;

    degDays[0] = phenology.sproutingDD;
    degDays[1] = phenology.conesDevelopmentDD;
    degDays[2] = phenology.conesRipeningDD;
    degDays[3] = phenology.conesMaturityDD;
    degDays[4] = phenology.senescenceDD;

    for (int iStation=0; iStation<nrStations; iStation++)
    {
        int dayOfYear;
        int iDatum = 0;
        thermalSum = 0;
        while (iDatum < nrData)
        {
            dayOfYear = hops::doyFromDate(obsDataD[iStation][iDatum].date.day,obsDataD[iStation][iDatum].date.month,obsDataD[iStation][iDatum].date.year);
            if (dayOfYear == 1)
            {
                thermalSum = 0;
            }
            actualKc = interpolation::linearInterpolation(thermalSum,degDays,Kc,5);
            printf("day %d thermal sum %f Kc %f \n",dayOfYear, thermalSum, actualKc);
            thermalSum += maxValue(obsDataD[iStation][iDatum].tAvg - baseTemperature,0);
            iDatum++;


        }
    }
    free(degDays);
    free(Kc);
    //printf("the end");
}

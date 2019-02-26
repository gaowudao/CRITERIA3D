#include "disease.h"
#include "powderyMildew.h"
#include "downyMildew.h"
#include "meteo.h"
#include "atmosphere.h"
#include "dataHandler.h"
#include "commonConstants.h"
#include "utilities.h"
#include <iostream>

#define MAXPOINTS 4096
#define VEGETATIVESTART 90
#define VEGETATIVEEND 270

// OIDIO
bool computePowderyMildew(Vine3DProject* myProject)
{
    Tmildew powdery;
    float phenoPhase;
    bool isBudBurst;
    myProject->logInfo("Compute powdery mildew...");

    for (long row = 0; row < myProject->DTM.header->nrRows ; row++)
        for (long col = 0; col < myProject->DTM.header->nrCols; col++)
        {
            if (myProject->DTM.value[row][col] != myProject->DTM.header->flag)
            {
                phenoPhase = myProject->statePlantMaps->stageMap->value[row][col];
                if (phenoPhase != myProject->statePlantMaps->stageMap->header->flag)
                {
                    // initialize output
                    myProject->outputPlantMaps->powderyCOLMap->value[row][col] = 0.0;
                    myProject->outputPlantMaps->powderyINFRMap->value[row][col] = 0.0;
                    myProject->outputPlantMaps->powderyPrimaryInfectionRiskMap->value[row][col] = 0.0;

                    // bud burst
                    if (phenoPhase >= 2.0)
                    {
                        // read state
                        powdery.state.degreeDays = myProject->statePlantMaps->degreeDay10FromBudBurstMap->value[row][col];
                        if (powdery.state.degreeDays == myProject->statePlantMaps->degreeDay10FromBudBurstMap->header->flag)
                            isBudBurst = true;
                        else
                            isBudBurst = false;

                        // Ascospores in Chasmothecia
                        powdery.state.aic = myProject->statePlantMaps->powderyAICMap->value[row][col];
                        powdery.state.currentColonies = myProject->statePlantMaps->powderyCurrentColoniesMap->value[row][col];
                        powdery.state.totalSporulatingColonies = myProject->statePlantMaps->powderySporulatingColoniesMap->value[row][col];

                        // read meteo
                        powdery.input.tavg = myProject->meteoMaps->avgDailyTemperature->value[row][col];
                        powdery.input.relativeHumidity = myProject->meteoMaps->airRelHumidityMap->value[row][col];
                        powdery.input.rain = myProject->meteoMaps->precipitationMap->value[row][col];
                        powdery.input.leafWetness = myProject->meteoMaps->leafWetnessMap->value[row][col];

                        powderyMildew(&powdery, isBudBurst);

                        // save state
                        myProject->statePlantMaps->degreeDay10FromBudBurstMap->value[row][col] = powdery.state.degreeDays;
                        myProject->statePlantMaps->powderyAICMap->value[row][col] = powdery.state.aic;
                        myProject->statePlantMaps->powderyCurrentColoniesMap->value[row][col] = powdery.state.currentColonies;
                        myProject->statePlantMaps->powderySporulatingColoniesMap->value[row][col] = powdery.state.totalSporulatingColonies;

                        // save output
                        myProject->outputPlantMaps->powderyCOLMap->value[row][col] = powdery.output.col;
                        myProject->outputPlantMaps->powderyINFRMap->value[row][col] = powdery.output.infectionRate;
                        myProject->outputPlantMaps->powderyPrimaryInfectionRiskMap->value[row][col] = minValue(5.0 * powdery.output.infectionRisk, 1.0);
                    }
                    else
                    {
                        myProject->statePlantMaps->degreeDay10FromBudBurstMap->value[row][col] = myProject->statePlantMaps->degreeDay10FromBudBurstMap->header->flag;
                        myProject->statePlantMaps->powderyAICMap->value[row][col] = 0.0;
                        myProject->statePlantMaps->powderyCurrentColoniesMap->value[row][col]  = 0.0;
                        myProject->statePlantMaps->powderySporulatingColoniesMap->value[row][col] = 0.0;
                    }
                }
            }
        }
    return true;
}


bool computeDownyMildew(Vine3DProject* myProject, QDate firstDate, QDate lastDate, int lastHour, QString myArea)
{
    using namespace std;

    myProject->logInfo("\nCompute downy mildew...");

    QDate firstJanuary;
    firstJanuary.setDate(lastDate.year(), 1, 1);
    //check last date - at least 3 hours
    if (lastHour < 3)
    {
        lastDate = lastDate.addDays(-1);
        lastHour = 23;
    }

    int lastDoy = firstJanuary.daysTo(lastDate) + 1;
    int firstDoy = firstJanuary.daysTo(firstDate) + 1;

    //check vegetative season
    if ((lastDoy < VEGETATIVESTART) || (firstDoy >= VEGETATIVEEND))
    {
        myProject->logInfo("Out of vegetative season");
        return true;
    }

    //check date
    firstDoy = maxValue(firstDoy, VEGETATIVESTART);
    firstDate = firstJanuary.addDays(firstDoy - 1);
    if (lastDoy > VEGETATIVEEND)
    {
        lastDoy = VEGETATIVEEND;
        lastHour = 23;
    }
    lastDate = firstJanuary.addDays(lastDoy - 1);
    long nrHours = (lastDoy -1)* 24 + lastHour;
    int nrSavingDays = lastDoy - firstDoy + 1;

    QDateTime firstTime, lastTime;
    firstTime.setDate(firstJanuary);
    firstTime.setTime(QTime(1, 0, 0, 0));
    lastTime.setDate(lastDate);
    lastTime.setTime(QTime(lastHour, 0, 0, 0));

    if (!myProject->LoadObsDataFilled(firstTime, lastTime))
    {
        myProject->logError();
        return false;
    }

    int rowPoint[MAXPOINTS], colPoint[MAXPOINTS];

    vector<TdownyMildewInput> input;
    vector<gis::Crit3DRasterGrid*> infectionMap;
    vector<gis::Crit3DRasterGrid*> oilSpotMap;

    TdownyMildew downyMildewCore;

    long n, nrPoints, row, col;
    int doy;
    QString dailyPath, variableMissing;
    float sumOilSpots;

    infectionMap.resize(nrSavingDays);
    oilSpotMap.resize(nrSavingDays);
    for (n = 0; n < nrSavingDays; n++)
    {
        infectionMap[n] = new gis::Crit3DRasterGrid;
        infectionMap[n]->initializeGrid(myProject->DTM);
        oilSpotMap[n] = new gis::Crit3DRasterGrid;
        oilSpotMap[n]->initializeGrid(myProject->DTM);
    }

    Crit3DTime myTime;
    bool missingData = false;
    bool isLastCell = false;
    nrPoints = 0;
    row = 0;
    col = 0;
    while ((! missingData) && (! isLastCell))
    {
        if (myProject->isVineyard(row, col))
        {
            rowPoint[nrPoints] = row;
            colPoint[nrPoints] = col;
            nrPoints++;
        }

        col++;
        if (col == myProject->DTM.header->nrCols)
        {
            row++;
            if (row == myProject->DTM.header->nrRows) isLastCell = true;
            col = 0;
        }

        if ((nrPoints == MAXPOINTS) || (isLastCell && (nrPoints > 0)))
        {
            input.clear();
            input.resize(nrPoints*nrHours);

            myProject->logInfo("Interpolate hourly data... nr points: " + QString::number(nrPoints));
            myTime = getCrit3DTime(firstTime);
            for (long h = 0; h < nrHours; h++)
            {
                if ((myTime.date.day == 1) && (myTime.getHour() == 1))
                    myProject->logInfo("Compute hourly data - month: " + QString::number(myTime.date.month));

                if (! interpolationProjectDtmMain(myProject, airTemperature, myTime, true))
                {
                    missingData = true;
                    variableMissing = "Air temperature";
                    break;
                }
                if (! interpolationProjectDtmMain(myProject, airRelHumidity, myTime, true))
                {
                    missingData = true;
                    variableMissing = "Air humidity";
                    break;
                }
                if (! interpolationProjectDtmMain(myProject, precipitation, myTime, true))
                {
                    missingData = true;
                    variableMissing = "Rainfall";
                    break;
                }

                computeLeafWetnessMap(myProject);

                for (n = 0; n < nrPoints; n++)
                {
                    row = rowPoint[n];
                    col = colPoint[n];
                    input[n*nrHours+h].tair = myProject->meteoMaps->airTemperatureMap->value[row][col];
                    input[n*nrHours+h].relativeHumidity = myProject->meteoMaps->airRelHumidityMap->value[row][col];
                    input[n*nrHours+h].rain = myProject->meteoMaps->precipitationMap->value[row][col];
                    input[n*nrHours+h].leafWetness = myProject->meteoMaps->leafWetnessMap->value[row][col];
                }
                myTime = myTime.addSeconds(3600);
            }

            // Downy cycle computation
            if (! missingData)
            {
                myProject->logInfo("Downy mildew model... nr points: " + QString::number(nrPoints));
                for (n = 0; n < nrPoints; n++)
                {
                    row = rowPoint[n];
                    col = colPoint[n];
                    sumOilSpots = 0;
                    // Downy Mildew initialization
                    downyMildewCore.input = input[n*nrHours];
                    downyMildew(&downyMildewCore, true);

                    for (long h = 1; h < nrHours; h++)
                    {
                        downyMildewCore.input = input[n*nrHours + h];
                        downyMildew(&downyMildewCore, false);

                        if (downyMildewCore.output.oilSpots > 0.0) sumOilSpots += (downyMildewCore.output.oilSpots * 100.0);

                        if (((h % 24)==0) || (h == (nrHours-1)))
                        {
                            if (h == (nrHours-1))
                                doy = lastDoy;
                            else
                                doy = int(h / 24);
                            if (doy >= firstDoy)
                            {
                                infectionMap[doy-firstDoy]->value[row][col] = downyMildewCore.output.infectionRate * 100.0;
                                oilSpotMap[doy-firstDoy]->value[row][col] = sumOilSpots;
                            }
                        }
                    }
                }
            } 
            nrPoints = 0;
        }
    }

    // Save output
    if (! missingData)
    {
        QString fileName, outputFileName;
        QDate myDate;
        std::string myErrorString;
        for (n = 0; n < nrSavingDays; n++)
        {
            myDate = firstDate.addDays(n);
            dailyPath = myProject->getPath() + myProject->dailyOutputPath + myDate.toString("yyyy/MM/dd/");

            fileName = getOutputNameDaily("ARPA", "downyINFR", myArea, "", myDate);
            outputFileName = dailyPath + fileName;
            gis::writeEsriGrid(outputFileName.toStdString(), infectionMap[n], &myErrorString);

            fileName = getOutputNameDaily("ARPA", "downySymptoms", myArea, "", myDate);
            outputFileName = dailyPath + fileName;
            gis::writeEsriGrid(outputFileName.toStdString(), oilSpotMap[n], &myErrorString);
        }
        myProject->logInfo("Downy mildew computed.");
    }

    // Clean memory
    for (n = 0; n < nrSavingDays; n++)
    {
        infectionMap[n]->freeGrid();
        oilSpotMap[n]->freeGrid();
    }
    infectionMap.clear();
    oilSpotMap.clear();
    input.clear();

    if (missingData)
    {
        myProject->logInfo("\nMissing hourly data to compute DownyMildew:"
                + getQDate(myTime.date).toString("yyyy/MM/dd")
                + "\n" + variableMissing);
        return false;
    }
    else
    {
        return true;
    }
}

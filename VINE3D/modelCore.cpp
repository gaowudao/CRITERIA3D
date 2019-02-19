#include <stdio.h>
#include <math.h>
#include <qstring.h>
#include <QDate>

#include "vine3DProject.h"
#include "waterBalance.h"
#include "crit3dDate.h"
#include "meteo.h"
#include "dataHandler.h"
#include "solarRadiation.h"
#include "grapevine.h"
#include "atmosphere.h"
#include "utilities.h"
#include <QDebug>

extern Vine3DProject myProject;

// [Pa] default atmospheric pressure at sea level
#define PRESS 101325

bool setSoilProfileCrop(Vine3DProject* myProject, int row, int col, Crit3DModelCase* modelCase)
{
    double* soilWPProfile = getSoilVarProfile(myProject, row, col, soilWiltingPointPotential);
    double* soilFCProfile = getSoilVarProfile(myProject, row, col, soilFieldCapacityPotential) ;
    double* matricPotentialProfile = getCriteria3DVarProfile(myProject, row, col, waterMatricPotential);
    double* waterContentProfile = getCriteria3DVarProfile(myProject, row, col, waterContent);
    double* waterContentProfileWP = getSoilVarProfile(myProject, row, col, soilWaterContentWP);
    double* waterContentProfileFC = getSoilVarProfile(myProject, row, col, soilWaterContentFC);

    if (! myProject->grapevine.setSoilProfile(modelCase, soilWPProfile, soilFCProfile,
            matricPotentialProfile, waterContentProfile, waterContentProfileFC,
            waterContentProfileWP)) return false;

    free(soilWPProfile);
    free(soilFCProfile);
    free(matricPotentialProfile);
    free(waterContentProfile);
    free(waterContentProfileWP);
    free(waterContentProfileFC);

    return true;
}


bool assignIrrigation(Vine3DProject* myProject, Crit3DTime myTime)
{
    int fieldIndex, nrHours;
    float irrigationRate, rate;
    int hour = myTime.getHour();
    QDate myDate = getQDate(myTime.date);

    for (long row = 0; row < myProject->DTM.header->nrRows ; row++)
        for (long col = 0; col < myProject->DTM.header->nrCols; col++)
            if (myProject->DTM.value[row][col] != myProject->DTM.header->flag)
            {
                //initialize
                myProject->meteoMaps->irrigationMap->value[row][col] = 0.0;

                fieldIndex = myProject->getModelCaseIndex(row, col);
                if (fieldIndex > 0)
                {
                    int idBook = 0;
                    while (myProject->getFieldBookIndex(idBook, myDate, fieldIndex, &idBook))
                    {
                        if (myProject->fieldBook[idBook].operation == irrigationOperation)
                        {
                            nrHours = myProject->fieldBook[idBook].quantity;
                            if (hour >= (24-nrHours))
                            {
                                irrigationRate = myProject->modelCases[fieldIndex].maxIrrigationRate;
                                rate = irrigationRate / myProject->meteoSettings->getHourlyIntervals();
                                myProject->meteoMaps->irrigationMap->value[row][col] = rate;
                            }
                        }
                        idBook++;
                    }
                }
            }
    return true;
}


bool modelDailyCycle(bool isInitialState, Crit3DDate myDate, int nrHours,
                     Vine3DProject* myProject, const QString& myOutputPath,
                     bool isSave, const QString& myArea)
{

    TfieldOperation operation;
    float quantity;
    QDate myQDate = getQDate(myDate);
    Crit3DTime myCurrentTime, myFirstTime, myLastTime;
    int myTimeStep = int(myProject->getTimeStep());
    myFirstTime = Crit3DTime(myDate, myTimeStep);
    myLastTime = Crit3DTime(myDate, nrHours * 3600);
    bool isNewModelCase;
    int modelCaseIndex;
    double* myProfile;


    int checkStressHour;
    if (!myProject->gisSettings.isUTC)
        checkStressHour = 12;
    else
        checkStressHour = 12 - myProject->gisSettings.timeZone;

    for (myCurrentTime = myFirstTime; myCurrentTime <= myLastTime; myCurrentTime = myCurrentTime.addSeconds(myTimeStep))
    {
        myProject->logInfo("\n" + getQDateTime(myCurrentTime).toString("yyyy-MM-dd hh:mm"));
        myProject->grapevine.setDate(myCurrentTime);

        // meteo interpolation
        myProject->logInfo("Interpolate meteo data");
        myProject->initializeMeteoMaps();
        interpolateAndSaveHourlyMeteo(myProject, airTemperature, myCurrentTime, myOutputPath, isSave, myArea);
        interpolateAndSaveHourlyMeteo(myProject, precipitation, myCurrentTime, myOutputPath, isSave, myArea);
        interpolateAndSaveHourlyMeteo(myProject, airRelHumidity, myCurrentTime, myOutputPath, isSave, myArea);
        interpolateAndSaveHourlyMeteo(myProject, windIntensity, myCurrentTime, myOutputPath, isSave, myArea);
        interpolateAndSaveHourlyMeteo(myProject, globalIrradiance, myCurrentTime, myOutputPath, isSave, myArea);
        //ET0
        if (computeET0Map(myProject))
            saveMeteoHourlyOutput(myProject, referenceEvapotranspiration, myOutputPath, myCurrentTime, myArea);

        //Leaf Wetness
        if (computeLeafWetnessMap(myProject))
            saveMeteoHourlyOutput(myProject, leafWetness, myOutputPath, myCurrentTime, myArea);

        if (isInitialState)
            initializeSoilMoisture(myProject, myCurrentTime.date.month);

        //Grapevine
        double vineTranspiration, grassTranspiration;
        myProject->logInfo("Compute grapevine");
        for (long row = 0; row < myProject->DTM.header->nrRows ; row++)
        {
            for (long col = 0; col < myProject->DTM.header->nrCols; col++)
            {
                if (int(myProject->DTM.value[row][col]) != int(myProject->DTM.header->flag))
                {
                    modelCaseIndex = myProject->getModelCaseIndex(row,col);
                    isNewModelCase = (int(myProject->statePlantMaps->fruitBiomassMap->value[row][col])
                                  == int(myProject->statePlantMaps->fruitBiomassMap->header->flag));

                    if (! myProject->grapevine.setWeather(
                                double(myProject->meteoMaps->avgDailyTemperature->value[row][col]),
                                double(myProject->meteoMaps->airTemperatureMap->value[row][col]),
                                double(myProject->meteoMaps->radiationMaps->globalRadiationMap->value[row][col]),
                                double(myProject->meteoMaps->precipitationMap->value[row][col]),
                                double(myProject->meteoMaps->airRelHumidityMap->value[row][col]),
                                double(myProject->meteoMaps->windIntensityMap->value[row][col]),
                                PRESS)) return(false);

                    if (!myProject->grapevine.setDerivedVariables(
                                double(myProject->meteoMaps->radiationMaps->diffuseRadiationMap->value[row][col]),
                                double(myProject->meteoMaps->radiationMaps->beamRadiationMap->value[row][col]),
                                double(myProject->meteoMaps->radiationMaps->transmissivityMap->value[row][col] / CLEAR_SKY_TRANSMISSIVITY_DEFAULT),
                                double(myProject->meteoMaps->radiationMaps->sunElevationMap->value[row][col]))) return (false);

                    myProject->grapevine.resetLayers();

                    if (! setSoilProfileCrop(myProject, row, col, &(myProject->modelCases[modelCaseIndex]))) return false;

                    if ((isInitialState) || (isNewModelCase))
                    {
                        if(!myProject->grapevine.initializeStatePlant(getDoyFromDate(myDate), &(myProject->modelCases[modelCaseIndex])))
                        {
                            myProject->logInfo("It's not possible initialize grapevine in the present growing season.\nIt will be replaced by a complete grass cover.");
                        }
                    }
                    else
                    {
                        if (!setStatePlantfromMap(row, col, myProject)) return(false);
                        myProject->grapevine.setStatePlant(myProject->statePlant, true);
                    }
                    double chlorophyll = NODATA;

                    if (! myProject->grapevine.compute((myCurrentTime == myFirstTime), myTimeStep,
                          &(myProject->modelCases[modelCaseIndex]), chlorophyll))
                          return(false);

                    // check field book (first hour)
                    if (myCurrentTime.getHour() == 1)
                    {
                        int idBook = 0;
                        while (myProject->getFieldBookIndex(idBook, myQDate, modelCaseIndex, &idBook))
                        {
                            operation = myProject->fieldBook[idBook].operation;
                            quantity = myProject->fieldBook[idBook].quantity;
                            myProject->grapevine.fieldBookAction(&(myProject->modelCases[modelCaseIndex]), operation, quantity);
                            idBook++;
                        }
                    }

                    myProject->statePlant = myProject->grapevine.getStatePlant();
                    getStatePlantToMap(row, col, myProject, &(myProject->statePlant));

                    myProfile = myProject->grapevine.getExtractedWater(&(myProject->modelCases[modelCaseIndex]));
                    //layer 0 = surface (only evaporation)
                    for (int layer=0; layer < myProject->WBSettings->nrLayers; layer++)
                    {    myProject->outputPlantMaps->transpirationLayerMaps[layer]->value[row][col] = float(myProfile[layer]);
                            qDebug() << myProfile[layer];
                    }

                    //stress transpiration output
                    if (myCurrentTime.getHour()==checkStressHour)
                        myProject->outputPlantMaps->stressMap->value[row][col] = float(myProject->grapevine.getStressCoefficient());

                    vineTranspiration = myProject->grapevine.getRealTranspirationGrapevine(&(myProject->modelCases[modelCaseIndex]));
                    grassTranspiration = myProject->grapevine.getRealTranspirationGrass(&(myProject->modelCases[modelCaseIndex]));

                    if (myCurrentTime == myFirstTime)
                    {
                        myProject->outputPlantMaps->vineyardTranspirationMap->value[row][col] = float(vineTranspiration);
                        myProject->outputPlantMaps->grassTranspirationMap->value[row][col] = float(grassTranspiration);
                    }
                    else
                    {
                        myProject->outputPlantMaps->vineyardTranspirationMap->value[row][col] += float(vineTranspiration);
                        myProject->outputPlantMaps->grassTranspirationMap->value[row][col] += float(grassTranspiration);
                    }
                }
            }
        }

        //Irrigation
        assignIrrigation(myProject, myCurrentTime);

        //3D soil water balance
        waterBalance(myProject);

        saveMeteoHourlyOutput(myProject, actualEvaporation, myOutputPath, myCurrentTime, myArea);

        if (isInitialState) isInitialState = false;
    }

    return true;
}

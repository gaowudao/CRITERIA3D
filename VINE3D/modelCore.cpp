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

bool firstTimePrint = true;
extern Vine3DProject myProject;

// [Pa] default atmospheric pressure at sea level
#define PRESS 101325

bool setSoilProfileCrop(Vine3DProject* myProject, int row, int col)
{
    double* soilWPProfile = getSoilVarProfile(myProject, row, col, soilWiltingPointPotential);
    double* soilFCProfile = getSoilVarProfile(myProject, row, col, soilFieldCapacityPotential) ;
    double* matricPotentialProfile = getCriteria3DVarProfile(myProject, row, col, waterMatricPotential);
    double* waterContentProfile = getCriteria3DVarProfile(myProject, row, col, waterContent);
    double* waterContentProfileWP = getSoilVarProfile(myProject, row, col, soilWaterContentWP);
    double* waterContentProfileFC = getSoilVarProfile(myProject, row, col, soilWaterContentFC);

    double* soilTempProfile = getCriteria3DVarProfile(myProject, row, col, soilTemperature);
    if (! myProject->grapevine.setSoilProfile(soilWPProfile, soilFCProfile,
            matricPotentialProfile, waterContentProfile, waterContentProfileFC,
            waterContentProfileWP)) return false;

    free(soilWPProfile);
    free(soilFCProfile);
    free(matricPotentialProfile);
    free(waterContentProfile);
    free(waterContentProfileWP);
    free(waterContentProfileFC);
    free(soilTempProfile);

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

                fieldIndex = myProject->getFieldIndex(row, col);
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
                                irrigationRate = myProject->vineFields[fieldIndex].maxIrrigationRate;
                                rate = irrigationRate / myProject->hourlyIntervals;
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
    int myTimeStep = myProject->getTimeStep();
    myFirstTime = Crit3DTime(myDate, myTimeStep);
    myLastTime = Crit3DTime(myDate, nrHours * 3600);
    bool isNewField;
    int fieldIndex;

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
                if (myProject->DTM.value[row][col] != myProject->DTM.header->flag)
                {
                    fieldIndex = myProject->getFieldIndex(row,col);
                    isNewField = (myProject->statePlantMaps->fruitBiomassMap->value[row][col]
                                  == myProject->statePlantMaps->fruitBiomassMap->header->flag);

                    if (! myProject->grapevine.setWeather(
                                myProject->meteoMaps->avgDailyTemperature->value[row][col],
                                myProject->meteoMaps->airTemperatureMap->value[row][col],
                                myProject->meteoMaps->radiationMaps->globalRadiationMap->value[row][col],
                                myProject->meteoMaps->precipitationMap->value[row][col],
                                myProject->meteoMaps->airRelHumidityMap->value[row][col],
                                myProject->meteoMaps->windIntensityMap->value[row][col],
                                PRESS)) return(false);

                    if (!myProject->grapevine.setDerivedVariables(myProject->meteoMaps->radiationMaps->diffuseRadiationMap->value[row][col],
                                myProject->meteoMaps->radiationMaps->beamRadiationMap->value[row][col],
                                myProject->meteoMaps->radiationMaps->transmissivityMap->value[row][col] / CLEAR_SKY_TRANSMISSIVITY_DEFAULT,
                                myProject->meteoMaps->radiationMaps->sunElevationMap->value[row][col],
                                myProject->meteoMaps->ET0Map->value[row][col])) return (false);

                    if (! setSoilProfileCrop(myProject, row, col)) return false;

                    if ((isInitialState) || (isNewField))
                    {
                        if(!myProject->grapevine.initializeStatePlant(getDoyFromDate(myDate), &(myProject->vineFields[fieldIndex])))
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
                          &(myProject->vineFields[fieldIndex]), chlorophyll))
                          return(false);

                    // check field book (first hour)
                    if (myCurrentTime.getHour() == 1)
                    {
                        int idBook = 0;
                        while (myProject->getFieldBookIndex(idBook, myQDate, fieldIndex, &idBook))
                        {
                            operation = myProject->fieldBook[idBook].operation;
                            quantity = myProject->fieldBook[idBook].quantity;
                            myProject->grapevine.fieldBookAction(&(myProject->vineFields[fieldIndex]), operation, quantity);
                            idBook++;
                        }
                    }

                    myProject->statePlant = myProject->grapevine.getStatePlant();
                    getStatePlantToMap(row, col, myProject, &(myProject->statePlant));

                    //pass transpiration to water balance
                    if (myProject->grapevine.getExtractedWater(myProject->currentProfile))
                        passPlantTranspirationProfileToMap(row, col, myProject);

                    //stress transpiration output
                    if (myCurrentTime.getHour()==checkStressHour)
                    {
                        myProject->outputPlantMaps->stressMap->value[row][col] = myProject->grapevine.getStressCoefficient();
                    }

                    vineTranspiration = myProject->grapevine.getRealTranspirationGrapevine();
                    grassTranspiration = myProject->grapevine.getRealTranspirationGrass();

                    if (myCurrentTime == myFirstTime)
                    {
                        myProject->outputPlantMaps->vineyardTranspirationMap->value[row][col] = vineTranspiration;
                        myProject->outputPlantMaps->grassTranspirationMap->value[row][col] = grassTranspiration;
                    }
                    else
                    {
                        myProject->outputPlantMaps->vineyardTranspirationMap->value[row][col] += vineTranspiration;
                        myProject->outputPlantMaps->grassTranspirationMap->value[row][col] += grassTranspiration;
                    }
                }
            }
        }

        //Irrigation
        assignIrrigation(myProject, myCurrentTime);
        waterBalance(myProject);

        saveMeteoHourlyOutput(myProject, actualEvaporation, myOutputPath, myCurrentTime, myArea);

        if (isInitialState) isInitialState = false;
    }

    return true;
}

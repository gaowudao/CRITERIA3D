
#include "waterBalance.h"
#include "crit3dProject.h"
#include "modelCore.h"


bool runModel(bool isInitialState, Crit3DTime myCurrentTime, Crit3DProject* myProject)
{
    myProject->log("Interpolate meteo data");
    myProject->meteoMaps->emptyMeteoMaps();

    /*
     * se ne deve occupare il chiamante
    interpolate(myProject, airTemperature, myCurrentTime);
    interpolate(myProject, precipitation, myCurrentTime);
    interpolate(myProject, airHumidity, myCurrentTime);
    interpolate(myProject, windIntensity, myCurrentTime);
    interpolate(myProject, globalIrradiance, myCurrentTime);
    */

    computeET0Map(myProject);

    if (isInitialState)
        initializeSoilMoisture(myProject, myCurrentTime.date.month);

    // Crop
    myProject->log("Compute grapevine");
    for (long row = 0; row < myProject->dtm->header->nrRows ; row++)
    {
        for (long col = 0; col < myProject->dtm->header->nrCols; col++)
        {
            if (myProject->dtm->value[row][col] != myProject->dtm->header->flag)
            {
                /*
                fieldIndex = myProject->getFieldIndex(row,col);
                isNewField = (myProject->statePlantMaps->fruitBiomassMap->value[row][col]
                              == myProject->statePlantMaps->fruitBiomassMap->header->flag);

                if (! setSoilProfileCrop(myProject, row, col)) return false;

                if ((isInitialState) || (isNewField))
                {
                    if(!myProject->grapevine.initializeStatePlant(getDoyFromDate(myDate), &(myProject->vineFields[fieldIndex])))
                    {
                        myProject->log("It's not possible initialize grapevine in the present growing season.\nIt will be replaced by a complete grass cover.");
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
                */
            }
        }
    }

    waterBalance(myProject);

    //saveMeteoHourlyOutput(myProject, actualEvaporation, myOutputPath, myCurrentTime);

    return true;
}



bool computeET0Map(Crit3DProject* myProject)
{
    float myET0;
    float myGlobalRadiation, myTransmissivity, myClearSkyTransmissivity;
    float myTemperature, myRelHumidity, myWindSpeed;
    float myHeight;

    gis::Crit3DRasterGrid* myMap = myProject->meteoMaps->ET0Map;

    for (long myRow = 0; myRow < myMap->header->nrRows; myRow++)
        for (long myCol = 0; myCol < myMap->header->nrCols; myCol++)
        {
            myMap->value[myRow][myCol] = myMap->header->flag;

            myHeight = myProject->dtm->value[myRow][myCol];

            if (myHeight != myProject->dtm->header->flag)
            {
                myGlobalRadiation = myProject->radiationMaps->globalRadiationMap->value[myRow][myCol];
                myTransmissivity = myProject->radiationMaps->transmissivityMap->value[myRow][myCol];
                myClearSkyTransmissivity = myProject->radiationMaps->clearSkyTransmissivityMap->value[myRow][myCol];
                myTemperature = myProject->meteoMaps->airTemperatureMap->value[myRow][myCol];
                myRelHumidity = myProject->meteoMaps->airHumidityMap->value[myRow][myCol];
                myWindSpeed = myProject->meteoMaps->windIntensityMap->value[myRow][myCol];

                if (myGlobalRadiation != myProject->radiationMaps->globalRadiationMap->header->flag
                        && myTransmissivity != myProject->radiationMaps->transmissivityMap->header->flag
                        && myClearSkyTransmissivity != myProject->radiationMaps->clearSkyTransmissivityMap->header->flag
                        && myTemperature != myProject->meteoMaps->airTemperatureMap->header->flag
                        && myRelHumidity != myProject->meteoMaps->airHumidityMap->header->flag
                        && myWindSpeed != myProject->meteoMaps->windIntensityMap->header->flag)
                {
                    myET0 = float(ET0_Penman_hourly(myHeight, myTransmissivity / myClearSkyTransmissivity,
                                      myGlobalRadiation, myTemperature, myRelHumidity, myWindSpeed));

                    myMap->value[myRow][myCol] = myET0;
                }
            }
        }

    return gis::updateMinMaxRasterGrid(myMap);
}

#include <cmath>
#include <algorithm>
#include "commonConstants.h"
#include "snowPoint.h"
#include "snowParam.h"


Crit3DSnowPoint::Crit3DSnowPoint(struct TradPoint* radpoint, Crit3DMeteoPoint* meteopoint)
{

    _snowFall = NODATA;
    _snowMelt = NODATA;
    _snowWaterEquivalent = NODATA;
    _iceContent = NODATA;
    _lWContent = NODATA;
    _internalEnergy = NODATA;
    _surfaceInternalEnergy = NODATA;
    _snowSurfaceTemp = NODATA;
    _ageOfSnow = NODATA;

    _slope = NODATA;
    _waterContent = NODATA;
    _evaporation = NODATA;

    _radpoint = radpoint;
    _meteopoint = meteopoint;

}

bool Crit3DSnowPoint::checkValidPoint()
{
    if (_radpoint->global != NODATA && _radpoint->beam != NODATA && _snowFall!= NODATA && _snowWaterEquivalent != NODATA && _snowSurfaceTemp != NODATA)
        return true;
    else
        return false;
}


/*
// Update rain and snowfall maps
//---------------------------------
*/


void Crit3DSnowPoint::computeSnowFall()
{

    // mettere nella funzione che la chiama prima il controllo che i 2 valori siano diversi da NODATA, se uno dei 2 è un NODATA, allora SnowFallMap.Value(row, col) = SnowFallMap.header.flag
    // dopo averla vhiamata aggiornare le info  GIS.SetGridInfo Snow.SnowFallMap, Definitions.HOURLY_snowFall, currentDay, getCurrentHour

    float temp = *(_meteopoint->obsDataH->tAir);

    float prec = *(_meteopoint->obsDataH->prec);

    float water = prec;
    if (water > 0)
    {
        if (temp < TEMP_MIN_WITH_RAIN)
            water = 0;
         else if (temp < TEMP_MAX_WITH_SNOW)
            water = water * (temp - TEMP_MIN_WITH_RAIN) / (TEMP_MAX_WITH_SNOW - TEMP_MIN_WITH_RAIN);
     }

    _snowFall = prec - water;
    *(_meteopoint->obsDataH->prec) = water;


}

void Crit3DSnowPoint::computeSnowBrooksModel(float myClearSkyTransmissivity)
{


    float globalRadiation, beamRadiation, vegetationShadowing, solarRadTot;
    float cloudCover;          // [-]        [0-1]
    float prevIceContent, prevLWaterContent; // [mm]
    float currentRatio;

    float dewPoint;  // [°C]
    float AirActualVapDensity;            //[kg/m^3]
    float WaterActualVapDensity;          //[kg/m^3]
    float longWaveAtmEmissivity;          // [-]
    float albedo;                          // [-]

    float freeWaterFlux;                // [mm]
    float avgMass;                      // [kg]

    float QSolar;                   // [kJ/m^2]   integrale della radiazione solare
    float QPrecip;                  // [kJ/m^2]   avvezione (trasferimento di calore dalla precipitazione)
    float QPrecipW;                 // [kJ/m^2]
    float QPrecipS;                 // [kJ/m^2]
    float QLongWave;                // [kJ/m^2]   emissione di radiazione onda lunga (eq. Stefan Boltzmann)
    float QTempGradient;            // [kJ/m^2]   scambio di calore sensibile (dovuto al gradiente di temperatura)
    float QVaporGradient;           // [kJ/m^2]   scambio di calore latente (vapore)
    float QTotal;                   // [kJ/m^2]
    float QWaterHeat;               // [kJ/m^2]
    float QWaterKinetic;            // [kJ/m^2]

    float bulk_density;                  //[kg/m^3]
    float EvapCond;                     // [mm]
    float refreeze;                     // [mm]

    float aerodynamicResistance;
    float myEmissivity;

        /*
         * questo controllo lo mettiamo nella funziona che chiama questa
    'preconditions
    If Not DBPointsManagement.isDBMeteoPointsLoaded Then
        PragaShell.PragaErrMsg "Open DBPoints before."
        Exit Function
    End If

    'controlla se già calcolato snowfall
    'in questo caso sono pronte anche tutte le altre mappe
    If Not Snow.SnowFallMap.isLoaded Or _
        Snow.SnowFallMap.info.date_ <> passaggioDati.currentDay Or _
        Snow.SnowFallMap.info.Hour_ <> passaggioDati.getCurrentHour Then
        PragaShell.PragaErrMsg "Interpolate weather before."
        Exit Function
    End If
*/

     //gestione specchi d'acqua
     bool isWater = false;
     //if criteria3DModule.flgCriteria3D_Ready Then
     float surfaceWaterContent = std::max( _waterContent, 0.0f );    //[mm]
     if ( (surfaceWaterContent / 1000) > SNOW_MAX_WATER_CONTENT )                                   //[m]
        isWater = true;

     //else

//                    If MSoil.SoilMap.isLoaded Then
//                        //corpi d'acqua - calcolo inutile
//                        If MSoil.SoilMapType = Definitions.SOILMAP_TYPE_ER Then
//                            soilIndex = GIS.GetValueFromXY(MSoil.SoilMap, x, y)
//                            If soilIndex <> Definitions.NO_DATA Then
//                                If UCase(MSoil.soilList(soilIndex).Name) = "CORPI D'ACQUA" Then
//                                    IsWater = True
//                                End If
//                            End If
//                        End If
//                    End If


      if ((!isWater) && (checkValidPoint()))
      {
         /*----------------------------------------
         // lettura dati
         //----------------------------------------*/
        float tAir = *(_meteopoint->obsDataH->tAir);
        float prec = *(_meteopoint->obsDataH->prec);
        float relHum = *(_meteopoint->obsDataH->rhAir); //[%]        [0,100]
        float windInt = *(_meteopoint->obsDataH->windInt);


        //Pag. 49 forumula 3.10
        //invertendo si ha cloudCover = 1 - Total transmittance/maximum clear sky transmissivity quindi myClearSkyTransmissivity sarebbe il maximum  ???
        if (_radpoint->transmissivity != NODATA)
            cloudCover = 1 - std::min(_radpoint->transmissivity / myClearSkyTransmissivity, 1.0f);
        else
            cloudCover = 0.1;


        globalRadiation = _radpoint->global;
        beamRadiation = _radpoint->beam;

        //ombreggiamento per vegetazione (<=1m no ombreggiamento, >=5m ombreggiamento completo)
        // non trovo riferimento
        vegetationShadowing = std::max( std::min((SNOW_VEGETATION_HEIGHT - 1) / 4, 1), 0);

        solarRadTot = globalRadiation - beamRadiation * vegetationShadowing;

        float prevSurfacetemp = _snowSurfaceTemp;  //[°C]
        float previousSWE = _snowWaterEquivalent;  //[mm]
        float prevInternalEnergy = _internalEnergy;          //[kJ/m^2]
        float prevSurfaceIntEnergy = _surfaceInternalEnergy; //[kJ/m^2]

        /*----------------------------------------
        //COERENZA
        //----------------------------------------
        // controlli di coerenza per eventuali modifiche manuale su mappa SWE*/

        if (prevSurfacetemp < -30)
            prevSurfacetemp = -30;

        if (previousSWE == 0)
        {
            // neve tolta
            prevIceContent = 0;
            prevLWaterContent = 0;
            _ageOfSnow = 0;
        }
        else
        {
            prevIceContent = _iceContent;
            prevLWaterContent = _lWContent;

            if ( (prevIceContent == 0) && (prevLWaterContent == 0) )
            {
             //neve aggiunta assegna valori standard di neve recente prossima alla fusione
             //una settimana di età
                prevIceContent = previousSWE;
                // Pag. 53 formula 3.23
                prevInternalEnergy = -(previousSWE / 1000) * LATENT_HEAT_FUSION * WATER_DENSITY;
                _ageOfSnow = 7;

                prevSurfacetemp = std::min(prevSurfacetemp, -0.1f);
                prevSurfaceIntEnergy = std::min(prevSurfaceIntEnergy, -0.1f);
            }

            // check on sum
            currentRatio = previousSWE / (prevIceContent + prevLWaterContent);
            if ( abs(currentRatio - 1) > 0.001)
            {
                prevIceContent = prevIceContent * currentRatio;
                prevLWaterContent = prevLWaterContent * currentRatio;
            }

         }


         /*---------------------------------------
         /////// Vapor Density and Roughness Calculations
         ----------------------------------------*/

         // brooks originale
         if ( previousSWE > SNOW_MINIMUM_HEIGHT)
            aerodynamicResistance = Crit3DSnowPoint::aerodynamicResistanceCampbell77(true, 10, windInt, SNOW_VEGETATION_HEIGHT);
         else
            aerodynamicResistance = Crit3DSnowPoint::aerodynamicResistanceCampbell77(false, 10, windInt, SNOW_VEGETATION_HEIGHT);


          // dew_point = Meteo.dewPoint(relHum, tAir)
          float dewPoint = *(_meteopoint->obsDataH->tDew);

          //ok pag.52 (3.20)
          // source: Jensen et al. (1990) and Tetens (1930)
          // saturated vapor density
          AirActualVapDensity = exp((16.78 * dewPoint - 116.9) / (dewPoint + 237.3)) / ((ZEROCELSIUS + dewPoint) * THERMO_WATER_VAPOR_CONST);

          //ok
          //over water (? over snow?)
          WaterActualVapDensity = exp((16.78 * prevSurfacetemp - 116.9) / (prevSurfacetemp + 237.3)) / ((ZEROCELSIUS + prevSurfacetemp) * THERMO_WATER_VAPOR_CONST);

          //over ice
          // ??????? controllare
          // non trovo riferimenti a questa formula
          // perchè amlcune sono define ed altre no?
          // 8.3143 J/(mol*K)   is universal gas constant???
          // 0.018 is [kg] vapor molar mass???
          if (prevInternalEnergy <= 0)
            WaterActualVapDensity = WaterActualVapDensity * exp(0.018 * LATENT_HEAT_FUSION * prevSurfacetemp * 1000 / (8.3143 * pow ((prevSurfacetemp + ZEROCELSIUS) , 2)) );


          /*-----------------------------------------------------------
          ///////////Atmospheric Emissivity Calculations for Longwave Radiation
          //-----------------------------------------------------------
          // Unsworth, M.H. and L.J. Monteith. 1975. Long-wave radiation a the ground. I. Angular distribution of incoming radiation. Quarterly Journal of the Royal Meteorological Society 101(427):13-24.
          //*/

          longWaveAtmEmissivity = (0.72 + 0.005 * tAir) * (1 - 0.84 * cloudCover) + 0.84 * cloudCover;

          /*-----------------------------------------------------------
                    // Age of snow & albedo
          -----------------------------------------------------------*/

          if ( _snowFall > 0 && prec == 0)
            _ageOfSnow = 1 / 24;
          else if (previousSWE > 0)
             _ageOfSnow = _ageOfSnow + 1 / 24;
          else
            _ageOfSnow = 0;

          if ( (previousSWE > 0) || (_snowFall > 0 && prec == 0))
            //arrotondato rispetto alla formula originaria (Gray and O'Neill 1974)
            //  U.S. Army Corps  U.S. Army Corps arrotonda così : albedo = 0.74 * _ageOfSnow^(-0..191)
            // in letteratura ci sono tantissime stime e per il calcolo dell'albedo della snow surface,con dipendenze anche ad es. da solar zenith angle o cloud cover o snowpack thickness o snow density ecc...
            /*
             * O'NEILL, A.D.J. GRAY D.M.1973. Spatial and temporal variations of the albedo of prairie snowpacks. The Role of Snow and Ice in Hydrology: Proceedings of the Banff Syn~posia, 1972. Unesc - WMO -IAHS, Geneva -Budapest-Paris, Vol. 1,  pp. 176-186
             * */
            albedo = std::min(0.9, 0.8 * pow ( _ageOfSnow , -0.15));
          else
            albedo = SOIL_ALBEDO;


          /*----------------------------------------
          ///////////Incoming Energy Fluxes
          //----------------------------------------*/
          // pag. 52 (3.22) considerando i 2 contributi invece che solo uno
          QPrecipW = HEAT_CAPACITY_WATER * WATER_DENSITY * (prec / 1000) * (std::max(0.0f, tAir) - prevSurfacetemp);
          QPrecipS = (HEAT_CAPACITY_SNOW * WATER_DENSITY * (_snowFall / 1000) * (std::min(0.0f, tAir) - prevSurfacetemp));
          QPrecip = QPrecipW + QPrecipS;

          // energia acqua libera (TROY site test)
          QWaterHeat = 0;
          QWaterKinetic = 0;
          freeWaterFlux = 0;
          if (surfaceWaterContent > 0.1)
          {
            //temperatura dell 'acqua: almeno 1 grado
            QWaterHeat = HEAT_CAPACITY_WATER * surfaceWaterContent / 1000 * WATER_DENSITY * (std::max(1.0, (prevSurfacetemp + tAir) / 2.0) - prevSurfacetemp);

            //////////////////////////////////////////////////
            // TO DO
            /*
            nodeIndex = GIS.GetValueFromXY(criteria3DModule.Crit3DIndexMap(0), x, y)
                        If nodeIndex <> criteria3DModule.Crit3DIndexMap(0).header.flag Then
                            //[m3/h]
                            freeWaterFlux = criteria3DModule.getLateralFlow(nodeIndex, False)
                            //[m2]
                            avgExchangeArea = Crit3DIndexMap(0).header.cellSize * (surfaceWaterContent / 1000.0)
                            //[m/s]
                            freeWaterFlux = (freeWaterFlux / avgExchangeArea) / 3600.0
                        End If

                        //controllare formula en. cinetica
                        if (freeWaterFlux > 0.01)
                        {
                            avgMass = (surfaceWaterContent / 1000.0) * WATER_DENSITY; //[kg/m2]
                            QWaterKinetic = 0.5 * avgMass * (freeWaterFlux * freeWaterFlux) / 1000.0; //[kJ/m2]
                        }
             }
             */
             //////////////////////////////////////////////////
             QSolar = (1 - albedo) * (solarRadTot * 3600) / 1000;

             if (previousSWE > SNOW_MINIMUM_HEIGHT)
                myEmissivity = SNOW_EMISSIVITY;
             else
                myEmissivity = SOIL_EMISSIVITY;

             QLongWave = STEFAN_BOLTZMANN * 3.6 * (longWaveAtmEmissivity * pow( (tAir + ZEROCELSIUS), 4.0) - myEmissivity * pow ((prevSurfacetemp + ZEROCELSIUS) , 4.0));

             QTempGradient = HEAT_CAPACITY_AIR * AIR_DENSITY * (tAir - prevSurfacetemp) / (aerodynamicResistance / 3600.0);

             // FT calcolare solo se c'e' manto nevoso
             if (previousSWE > SNOW_MINIMUM_HEIGHT)
                QVaporGradient = (LATENT_HEAT_VAPORIZATION + LATENT_HEAT_FUSION) * (AirActualVapDensity - WaterActualVapDensity) / (aerodynamicResistance / 3600);
             else
                QVaporGradient = 0;

                QTotal = QSolar + QPrecip + QLongWave + QTempGradient + QVaporGradient + QWaterHeat + QWaterKinetic;

             /*------------------------------------------------------
             //Energy Balance
             //------------------------------------------------------*/

             //Evaporation/Condensation
             if (previousSWE > SNOW_MINIMUM_HEIGHT)
             {
                EvapCond = QVaporGradient / ((LATENT_HEAT_FUSION + LATENT_HEAT_VAPORIZATION) * WATER_DENSITY) * 1000;
                if (EvapCond < 0)
                    //controllo aggiunto: può evaporare al massimo la neve presente
                    EvapCond = - std::min(previousSWE + _snowFall, -EvapCond);

              }
              else
                EvapCond = 0;


              // sign of evaporation is negative
              if (EvapCond < 0)
                        _evaporation = -EvapCond;
              else
                        _evaporation = 0;

              // refreeze
              if (previousSWE > SNOW_MINIMUM_HEIGHT)
                refreeze = std::min((prec + prevLWaterContent), std::max(0.0f, -1000 / (LATENT_HEAT_FUSION * WATER_DENSITY) * (prevInternalEnergy + QTotal)))- std::min((_snowFall + prevIceContent + EvapCond), std::max(0.0f, 1000 / (LATENT_HEAT_FUSION * WATER_DENSITY) * (prevInternalEnergy + QTotal)));
              else
                refreeze = 0;


              // Internal energy
              _internalEnergy = prevInternalEnergy + QTotal + (refreeze / 1000) * LATENT_HEAT_FUSION * WATER_DENSITY;

              /*------------------------------------------------------
              ////////////// Snow Pack Mass
              ------------------------------------------------------*/

              //Ice content
              if (_internalEnergy > 0.001)
                _iceContent = 0;
              else
                _iceContent = std::max(prevIceContent + _snowFall + refreeze + EvapCond, 0.0f);


               float waterHoldingCapacity = SNOW_WATER_HOLDING_CAPACITY / (1 - SNOW_WATER_HOLDING_CAPACITY); //[%]

               // Liquid water content
               if (abs(_internalEnergy) < 0.001)
                _lWContent = std::min(waterHoldingCapacity * _iceContent, prevLWaterContent + prec + surfaceWaterContent - refreeze);
               else
                _lWContent = 0;

               if (_lWContent < 0)
                _lWContent = 0;


               // Snow water equivalent
               _snowWaterEquivalent = _iceContent + _lWContent;

               // Snowmelt (or refreeze) - source/sink for Criteria3D
               _snowMelt = previousSWE + _snowFall + EvapCond - _snowWaterEquivalent;

               // Snow surface energy
               if ( abs(_internalEnergy) < 0.001)
               {
                _surfaceInternalEnergy = 0;
               }
               else
               {
                        if (_snowWaterEquivalent > 0)
                            _surfaceInternalEnergy = std::min(0.0f, prevSurfaceIntEnergy + (QTotal + (refreeze / 1000) * LATENT_HEAT_FUSION * WATER_DENSITY) * (std::min(_snowWaterEquivalent / 1000, SNOW_SKIN_THICKNESS) / SNOW_DAMPING_DEPTH + std::max(SNOW_SKIN_THICKNESS - (_snowWaterEquivalent / 1000), 0.0f) / SOIL_DAMPING_DEPTH));
                        else
                            _surfaceInternalEnergy = prevSurfaceIntEnergy + (QTotal + (refreeze / 1000) * LATENT_HEAT_FUSION * WATER_DENSITY) * (SNOW_SKIN_THICKNESS / SOIL_DAMPING_DEPTH);
               }


               bulk_density = DEFAULT_BULK_DENSITY;
               /*
                    If MSoil.SoilMap.isLoaded Then
                        'bulk density superficiale (1 cm)
                        'da reperire dato reale di bulk_density, ora messo a valore di default
                         bulk_density = DEFAULT_BULK_DENSITY
'                        If MSoil.readPointSoilHorizon(x, y, 1, soilIndex, nrHorizon, myHorizon) Then
'                             If myHorizon.bulkDensity <> Definitions.NO_DATA Then
'                                 bulk_density = myHorizon.bulkDensity * 1000         '[kg/m^3]
'                             End If
'                        End If
                    End If
                 */
                _snowSurfaceTemp = _surfaceInternalEnergy / (WATER_DENSITY * HEAT_CAPACITY_SNOW * std::min(_snowWaterEquivalent / 1000, SNOW_SKIN_THICKNESS) + SOIL_SPECIFIC_HEAT * std::max(0.0f, SNOW_SKIN_THICKNESS - _snowWaterEquivalent / 1000) * bulk_density);
           }
           else
           {
                    //snowfall diventa snowmelt negli specchi d'acqua
            _snowMelt = _snowFall;
            _iceContent = NODATA;
            _lWContent = NODATA;
            _snowWaterEquivalent = NODATA;
            _surfaceInternalEnergy = NODATA;
            _snowSurfaceTemp = NODATA;
           }
    }

    /*------------------------------------------------------
    //////////Update maps
    //------------------------------------------------------
    GIS.SetGridInfo Snow.SnowMeltMap, HOURLY_SNOWMELT, currentDay, getCurrentHour
    GIS.SetGridInfo Snow.SnowWaterEquivalentMap, HOURLY_SWE, currentDay, getCurrentHour
    */


}


float Crit3DSnowPoint::getSnowFall()
{
    return _snowFall;
}

float Crit3DSnowPoint::getSnowMelt()
{
    return _snowMelt;
}

float Crit3DSnowPoint::getSnowWaterEquivalent()
{
    return _snowWaterEquivalent;
}

float Crit3DSnowPoint::getIceContent()
{
    return _iceContent;
}

float Crit3DSnowPoint::getLWContent()
{
    return _lWContent;
}

float Crit3DSnowPoint::getInternalEnergy()
{
    return _internalEnergy;
}

float Crit3DSnowPoint::getSurfaceInternalEnergy()
{
    return _surfaceInternalEnergy;
}

float Crit3DSnowPoint::getSnowSurfaceTemp()
{
    return _snowSurfaceTemp;
}

float Crit3DSnowPoint::getAgeOfSnow()
{
    return _ageOfSnow;
}


float Crit3DSnowPoint::aerodynamicResistanceCampbell77(bool isSnow , float zRefWind, float myWindSpeed, float vegetativeHeight)
{

    //vegetativeHeight          //  [m]) height of the vegetative
    //zRefWind                   //  [m] measurement height for wind
    //myWindSpeed               //  [m * s-1] wind speed measured at reference height
    float zeroPlane;            //  [m] zero-plane displacement (snow = 0m, vegetative cover d = 0.64 times the height of the vegetative)
    float momentumRoughness;    //  [m] momentum roughness parameter (for snow = 0.001m, for vegetative cover zm = 0.13 times the height of the vegetation)
    float log2;                 //equivalente a vegetativeHeight = 1; log2 = (Zt - d + Zh)/Zh , Zt: measurement height for temperature  sempre pari a 2m (i sensori di temperatura sono piazzati a 2metri)
    float log1;

    // check on wind speed [m/s]
    myWindSpeed = std::max(myWindSpeed, 0.2f);

    if (isSnow)
    {
        zeroPlane = 0;
        momentumRoughness = 0.001;
        log2 = 9.2;
    }
    else
    {
        //check on vegetativeHeight  [m]
        vegetativeHeight = std::max(vegetativeHeight, 0.1f);

        //pag 51: d is the height of the zero-plane displacement (for snow d = 0 m and for
        //vegetative cover d = 0.64 times the height of the vegetative) quindi
        //era: zeroPlane = 0.65 * vegetativeHeight ?!
        zeroPlane = 0.64 * vegetativeHeight;

        momentumRoughness = 0.13 * vegetativeHeight;
        log2 = 4;
    }

    if (zeroPlane > zRefWind)
        zeroPlane = zRefWind;

    //formula 3.18 pag 51
    log1 = log((zRefWind - zeroPlane + momentumRoughness) / momentumRoughness);

    float aerodResis = log1 * log2 / (VON_KARMAN_CONST * VON_KARMAN_CONST * myWindSpeed);

    return aerodResis;

}

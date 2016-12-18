/*!
    CRITERIA3D

    \copyright 2010-2016 Fausto Tomei, Gabriele Antolini,
    Alberto Pistocchi, Marco Bittelli, Antonio Volta, Laura Costantini

    You should have received a copy of the GNU General Public License
    along with Nome-Programma.  If not, see <http://www.gnu.org/licenses/>.

    This file is part of CRITERIA3D.
    CRITERIA3D has been developed under contract issued by A.R.P.A. Emilia-Romagna

    CRITERIA3D is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    CRITERIA3D is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with CRITERIA3D.  If not, see <http://www.gnu.org/licenses/>.

    contacts:
    fausto.tomei@gmail.com
    ftomei@arpae.it
*/


#include "commonConstants.h"
#include "gis.h"
#include "radiationSettings.h"

Crit3DRadiationSettings::Crit3DRadiationSettings()
{
    computeRealData = true;
    usePotentialIfMissing = false;
    timeStepIntegration = 1;
    computeShadowing = true;
    shadowDistanceFactor = 1;
    linkeMode = PARAM_MODE_FIXED;
    linke = 4.0;
    landUse = LAND_USE_RURAL;
    albedoMode = PARAM_MODE_FIXED;
    albedo = (float)0.2;
    tiltMode = TILT_TYPE_DEM;
    tilt = NODATA;
    aspect = NODATA;

    algorithm = RADIATION_ALGORITHM_RSUN;
    transSettings.model = TRANSMISSIVITY_MODEL_HOURLY;
    transSettings.periodType = TRANSMISSIVITY_COMPUTATION_DYNAMIC;
    transSettings.useTotal = false;
    transSettings.clearSky = CLEAR_SKY_TRANSMISSIVITY_DEFAULT;
}

void Crit3DRadiationSettings::setGisSettings(const gis::Crit3DGisSettings* mySettings)
{
    gisSettings = new gis::Crit3DGisSettings();
    gisSettings->utmZone = mySettings->utmZone;
    gisSettings->isNorthernEmisphere = mySettings->isNorthernEmisphere;
    gisSettings->timeZone = mySettings->timeZone;
    gisSettings->isUTC = mySettings->isUTC;
}

bool Crit3DRadiationSettings::getComputeRealData()
{ return computeRealData;}

bool Crit3DRadiationSettings::getUsePotentialIfMissing()
{ return usePotentialIfMissing;}

float Crit3DRadiationSettings::getTimeStepIntegration()
{ return timeStepIntegration;}

bool Crit3DRadiationSettings::getComputeShadowing()
{ return computeShadowing;}

float Crit3DRadiationSettings::getShadowDistanceFactor()
{ return shadowDistanceFactor;}

TparameterMode Crit3DRadiationSettings::getLinkeMode()
{ return linkeMode;}

float Crit3DRadiationSettings::getLinke()
{ return linke;}

TlandUse Crit3DRadiationSettings::getLandUse()
{ return landUse;}

TparameterMode Crit3DRadiationSettings::getAlbedoMode()
{ return albedoMode;}

float Crit3DRadiationSettings::getAlbedo()
{ return albedo;}

TtiltMode Crit3DRadiationSettings::getTiltMode()
{ return tiltMode;}

TradiationAlgorithm Crit3DRadiationSettings::getAlgorithm()
{ return algorithm;}

TtransmissivityAlgorithm Crit3DRadiationSettings::getTransmissivityAlgorithm()
{ return transSettings.model;}

TtransmissivityComputationPeriod Crit3DRadiationSettings::getTransmissivityPeriod()
{ return transSettings.periodType;}

bool Crit3DRadiationSettings::getTransmissivityUseTotal()
{ return transSettings.useTotal;}

float Crit3DRadiationSettings::getTransmissivityClearSky()
{ return transSettings.clearSky;}

float Crit3DRadiationSettings::getTilt()
{ return tilt;}

float Crit3DRadiationSettings::getAspect()
{ return aspect;}

float Crit3DRadiationSettings::getClearSky()
{ return transSettings.clearSky;}

int Crit3DRadiationSettings::getTimeZone()
{ return timeZone;}

void Crit3DRadiationSettings::setTimeZone(int myZone)
{ timeZone = myZone;}

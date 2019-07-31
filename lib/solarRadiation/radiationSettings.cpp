/*!
    \name Solar Radiation
    \copyright 2011 Gabriele Antolini, Fausto Tomei
    \note  This library uses G_calc_solar_position() by Markus Neteler

    This library is part of CRITERIA3D.
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

    \authors
    Gabriele Antolini gantolini@arpae.it
    Fausto Tomei ftomei@arpae.it
*/

#include "commonConstants.h"
#include "radiationSettings.h"


Crit3DRadiationSettings::Crit3DRadiationSettings()
{
    initialize();
}

void Crit3DRadiationSettings::initialize()
{
    computeRealData = true;
    computeShadowing = true;
    linkeMode = PARAM_MODE_FIXED;
    linke = 4.f;
    albedoMode = PARAM_MODE_FIXED;
    albedo = 0.2f;
    tiltMode = TILT_TYPE_DEM;
    tilt = NODATA;
    aspect = NODATA;

    algorithm = RADIATION_ALGORITHM_RSUN;
    //transSettings.model = TRANSMISSIVITY_MODEL_HOURLY;
    //transSettings.periodType = TRANSMISSIVITY_COMPUTATION_DYNAMIC;
    transSettings.useTotal = false;
    transSettings.clearSky = CLEAR_SKY_TRANSMISSIVITY_DEFAULT;
}

void Crit3DRadiationSettings::setGisSettings(const gis::Crit3DGisSettings* mySettings)
{
    gisSettings = new gis::Crit3DGisSettings();
    gisSettings->utmZone = mySettings->utmZone;
    gisSettings->timeZone = mySettings->timeZone;
    gisSettings->isUTC = mySettings->isUTC;
}

bool Crit3DRadiationSettings::getComputeRealData()
{ return computeRealData;}

bool Crit3DRadiationSettings::getComputeShadowing()
{ return computeShadowing;}

TparameterMode Crit3DRadiationSettings::getLinkeMode()
{ return linkeMode;}

float Crit3DRadiationSettings::getLinke()
{ return linke;}

TparameterMode Crit3DRadiationSettings::getAlbedoMode()
{ return albedoMode;}

float Crit3DRadiationSettings::getAlbedo()
{ return albedo;}

TtiltMode Crit3DRadiationSettings::getTiltMode()
{ return tiltMode;}

TradiationAlgorithm Crit3DRadiationSettings::getAlgorithm()
{ return algorithm;}

//TtransmissivityAlgorithm Crit3DRadiationSettings::getTransmissivityAlgorithm()
//{ return 0; }// transSettings.model;}

//TtransmissivityComputationPeriod Crit3DRadiationSettings::getTransmissivityPeriod()
//{ return transSettings.periodType;}

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

std::string getKeyStringRadAlgorithm(TradiationAlgorithm value)
{
    std::map<std::string, TradiationAlgorithm>::const_iterator it;
    std::string key = "";

    for (it = radAlgorithmToString.begin(); it != radAlgorithmToString.end(); ++it)
    {
        if (it->second == value)
        {
            key = it->first;
            break;
        }
    }
    return key;
}

std::string getKeyStringParamMode(TparameterMode value)
{
    std::map<std::string, TparameterMode>::const_iterator it;
    std::string key = "";

    for (it = paramModeToString.begin(); it != paramModeToString.end(); ++it)
    {
        if (it->second == value)
        {
            key = it->first;
            break;
        }
    }
    return key;
}

std::string getKeyStringParamMode(TtiltMode value)
{
    std::map<std::string, TtiltMode>::const_iterator it;
    std::string key = "";

    for (it = tiltModeToString.begin(); it != tiltModeToString.end(); ++it)
    {
        if (it->second == value)
        {
            key = it->first;
            break;
        }
    }
    return key;
}

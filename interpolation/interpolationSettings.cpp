/*!
    \copyright 2016 Fausto Tomei, Gabriele Antolini,
    Alberto Pistocchi, Marco Bittelli, Antonio Volta, Laura Costantini

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

#include "crit3dDate.h"
#include "interpolationSettings.h"
#include "interpolation.h"
#include "commonConstants.h"


Crit3DInterpolationSettings::Crit3DInterpolationSettings()
{
    interpolationMethod = interpolationMethod::idw;
    useThermalInversion = true;
    useTAD = false;
    useJRC = false;
    useDewPoint = true;
    isKrigingReady = false;
    genericPearsonThreshold = float(PEARSONSTANDARDTHRESHOLD);
    maxHeightInversion = 1000.;

    /*
    detrendList[0] = proxyVars::height;
    detrendList[1] = proxyVars::urbanFraction;
    detrendList[2] = proxyVars::orogIndex;
    detrendList[3] = proxyVars::seaDistance;
    detrendList[4] = proxyVars::aspect;
    detrendList[5] = proxyVars::generic;
    */

    currentClimateParametersLoaded = false;
    currentDate = getNullDate();
    currentHour = NODATA;
    currentHourFraction = NODATA;
}

float Crit3DInterpolationSettings::getCurrentClimateLapseRate(meteoVariable myVar)
{
    if (currentClimateParametersLoaded && (currentDate != getNullDate()) && (currentHour != NODATA))
        return (currentClimateParameters.getClimateLapseRate(myVar, &currentDate, currentHour));
    else
        // TODO migliorare
        return -0.006f;
}

void Crit3DInterpolationSettings::setClimateParameters(Crit3DClimateParameters* myParameters)
{
    currentClimateParametersLoaded = true;
    currentClimateParameters = *(myParameters);
}

void Crit3DInterpolationSettings::setCurrentDate(Crit3DDate myDate)
{ currentDate = myDate;}

void Crit3DInterpolationSettings::setCurrentHour(int myHour)
{ currentHour = myHour;}

void Crit3DInterpolationSettings::setCurrentHourFraction(int myHourFraction)
{ currentHourFraction = myHourFraction;}

float Crit3DInterpolationSettings::getGenericPearsonThreshold()
{ return genericPearsonThreshold;}

int Crit3DInterpolationSettings::getInterpolationMethod()
{ return interpolationMethod;}

bool Crit3DInterpolationSettings::getUseTad()
{ return useTAD;}

float Crit3DInterpolationSettings::getMaxHeightInversion()
{ return maxHeightInversion;}

void Crit3DInterpolationSettings::setInterpolationMethod(bool myValue)
{ interpolationMethod = myValue;}

void Crit3DInterpolationSettings::setUseThermalInversion(bool myValue)
{ useThermalInversion = myValue;}

void Crit3DInterpolationSettings::setUseTAD(bool myValue)
{ useTAD = myValue;}

void Crit3DInterpolationSettings::setUseJRC(bool myValue)
{ useJRC = myValue;}

void Crit3DInterpolationSettings::setUseDewPoint(bool myValue)
{ useDewPoint = myValue;}

bool Crit3DInterpolationSettings::getUseThermalInversion()
{ return (useThermalInversion);}

bool Crit3DInterpolationSettings::getUseJRC()
{ return (useJRC);}

bool Crit3DInterpolationSettings::getUseDewPoint()
{ return (useDewPoint);}

int Crit3DInterpolationSettings::getProxyNr()
{ return (int)currentProxy.size();}

Crit3DProxy Crit3DInterpolationSettings::getProxy(int pos)
{ return currentProxy.at(pos);}

std::string Crit3DProxy::getName()
{ return name;}

void Crit3DProxy::setActive(bool isActive_)
{ isActive = isActive_;}

bool Crit3DProxy::getActive()
{ return isActive;}

void Crit3DProxy::initialize(std::string name_)
{
    name = name_;
    isActive = false;
    this->grid = NULL;
}

void Crit3DProxy::setRegressionR2(float myValue)
{ regressionR2 = myValue;}

float Crit3DProxy::getRegressionR2()
{ return regressionR2;}

void Crit3DProxy::setRegressionSlope(float myValue)
{ regressionSlope = myValue;}

float Crit3DProxy::getRegressionSlope()
{ return regressionSlope;}

float Crit3DProxy::getValue(int pos, std::vector <float> proxyValues)
{
    if (pos < proxyValues.size())
        return proxyValues.at(pos);
    else
        return NODATA;
}

void Crit3DInterpolationSettings::addProxy(std::string myProxyName)
{
    Crit3DProxy myProxy;
    myProxy.initialize(myProxyName);
    currentProxy.push_back(myProxy);
}

std::string Crit3DInterpolationSettings::getProxyName(int pos)
{ return currentProxy.at(pos).getName();}

void Crit3DInterpolationSettings::setProxyActive(int pos, bool isActive_)
{ currentProxy.at(pos).setActive(isActive_);}

bool Crit3DInterpolationSettings::getProxyActive(int pos)
{ return currentProxy.at(pos).getActive();}

float Crit3DInterpolationSettings::getProxyValue(int pos, std::vector <float> proxyValues)
{
    if (pos < currentProxy.size())
        return currentProxy.at(pos).getValue(pos, proxyValues);
    else
        return NODATA;
}

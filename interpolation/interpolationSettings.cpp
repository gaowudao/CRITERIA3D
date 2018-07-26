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

Crit3DProxyInterpolation Crit3DInterpolationSettings::getProxy(int pos)
{ return currentProxy.at(pos);}

Crit3DProxy::Crit3DProxy(std::string name_)
{
    name = name_;
    isActive = false;
    this->grid = NULL;
}

Crit3DProxyInterpolation::Crit3DProxyInterpolation(std::string name_) : Crit3DProxy( name_)
{
    regressionR2 = NODATA;
    regressionSlope = NODATA;
}

void Crit3DProxyInterpolation::setRegressionR2(float myValue)
{ regressionR2 = myValue;}

float Crit3DProxyInterpolation::getRegressionR2()
{ return regressionR2;}

void Crit3DProxyInterpolation::setRegressionSlope(float myValue)
{ regressionSlope = myValue;}

float Crit3DProxyInterpolation::getRegressionSlope()
{ return regressionSlope;}

float Crit3DProxyInterpolation::getValue(int pos, std::vector <float> proxyValues)
{
    if (pos < proxyValues.size())
        return proxyValues.at(pos);
    else
        return NODATA;
}

void Crit3DInterpolationSettings::addProxy(std::string myProxyName)
{
    Crit3DProxyInterpolation myProxy = Crit3DProxyInterpolation(myProxyName);
    currentProxy.push_back(myProxy);
}

std::string Crit3DInterpolationSettings::getProxyName(int pos)
{ return currentProxy.at(pos).name;}

void Crit3DInterpolationSettings::setProxyActive(int pos, bool isActive_)
{ currentProxy.at(pos).isActive = isActive_;}

bool Crit3DInterpolationSettings::getProxyActive(int pos)
{ return currentProxy.at(pos).isActive;}

float Crit3DInterpolationSettings::getProxyValue(int pos, std::vector <float> proxyValues)
{
    if (pos < currentProxy.size())
        return currentProxy.at(pos).getValue(pos, proxyValues);
    else
        return NODATA;
}

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
    useHeight = true;
    useThermalInversion = true;
    useOrogIndex = false;
    useSeaDistance = false;
    useUrbanFraction = false;
    useGenericProxy = false;
    useTAD = false;
    useJRC = false;
    useDewPoint = true;
    isKrigingReady = false;
    isRetrendActive = true;
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

void Crit3DInterpolationSettings::setUseHeight(bool myValue)
{ useHeight = myValue;}

void Crit3DInterpolationSettings::setUseThermalInversion(bool myValue)
{ useThermalInversion = myValue;}

void Crit3DInterpolationSettings::setUseAspect(bool myValue)
{ useAspect = myValue;}

void Crit3DInterpolationSettings::setUseOrogIndex(bool myValue)
{ useOrogIndex = myValue;}

void Crit3DInterpolationSettings::setUseSeaDistance(bool myValue)
{ useSeaDistance = myValue;}

void Crit3DInterpolationSettings::setUrbanFraction(bool myValue)
{ useUrbanFraction = myValue;}

void Crit3DInterpolationSettings::setUseGenericProxy(bool myValue)
{ useGenericProxy = myValue;}

void Crit3DInterpolationSettings::setUseTAD(bool myValue)
{ useTAD = myValue;}

void Crit3DInterpolationSettings::setUseJRC(bool myValue)
{ useJRC = myValue;}

void Crit3DInterpolationSettings::setUseDewPoint(bool myValue)
{ useDewPoint = myValue;}

bool Crit3DInterpolationSettings::getUseHeight()
{ return (useHeight);}

bool Crit3DInterpolationSettings::getUseThermalInversion()
{ return (useThermalInversion);}

bool Crit3DInterpolationSettings::getUseOrogIndex()
{ return (useOrogIndex);}

bool Crit3DInterpolationSettings::getUseSeaDistance()
{ return (useSeaDistance);}

bool Crit3DInterpolationSettings::getUseUrbanFraction()
{ return (useUrbanFraction);}

bool Crit3DInterpolationSettings::getUseAspect()
{ return (useAspect);}

bool Crit3DInterpolationSettings::getUseGenericProxy()
{ return (useGenericProxy);}

bool Crit3DInterpolationSettings::getUseJRC()
{ return (useJRC);}

bool Crit3DInterpolationSettings::getUseDewPoint()
{ return (useDewPoint);}

void Crit3DInterpolationSettings::setDetrendOrographyActive(bool myValue)
{ detrendOrographyActive = myValue;}

void Crit3DInterpolationSettings::setDetrendUrbanActive(bool myValue)
{ detrendUrbanActive = myValue;}

void Crit3DInterpolationSettings::setDetrendOrogIndexActive(bool myValue)
{ detrendOrogIndexActive = myValue;}

void Crit3DInterpolationSettings::setDetrendSeaDistanceActive(bool myValue)
{ detrendSeaDistanceActive = myValue;}

void Crit3DInterpolationSettings::setDetrendAspectActive(bool myValue)
{ detrendAspectActive = myValue;}

void Crit3DInterpolationSettings::setDetrendGenericProxyActive(bool myValue)
{ detrendGenericProxyActive = myValue;}

bool Crit3DInterpolationSettings::getDetrendOrographyActive()
{ return detrendOrographyActive;}

bool Crit3DInterpolationSettings::getDetrendUrbanActive()
{ return detrendUrbanActive;}

bool Crit3DInterpolationSettings::getDetrendOrogIndexActive()
{ return detrendOrogIndexActive;}

bool Crit3DInterpolationSettings::getDetrendSeaDistanceActive()
{ return detrendSeaDistanceActive;}

bool Crit3DInterpolationSettings::getDetrendAspectActive()
{ return detrendAspectActive;}

bool Crit3DInterpolationSettings::getDetrendGenericProxyActive()
{ return detrendGenericProxyActive;}

int Crit3DInterpolationSettings::getProxyNr()
{ return (int)currentProxy.size();}

Crit3DProxy Crit3DInterpolationSettings::getProxy(int pos)
{ return currentProxy.at(pos);}

proxyVars::TProxyVar Crit3DProxy::getName()
{ return name;}

void Crit3DProxy::setActive(bool isActive_)
{ isActive = isActive_;}

void Crit3DProxy::initialize(proxyVars::TProxyVar name_)
{
    name = name_;
    isActive = false;
    this->grid = NULL;
}

void Crit3DInterpolationSettings::addProxy(proxyVars::TProxyVar myProxyName)
{
    Crit3DProxy myProxy;
    myProxy.initialize(myProxyName);
    currentProxy.push_back(myProxy);
}

proxyVars::TProxyVar Crit3DInterpolationSettings::getProxyName(int pos)
{ return currentProxy.at(pos).getName();}

void Crit3DInterpolationSettings::setProxyActive(int pos, bool isActive_)
{ currentProxy.at(pos).setActive(isActive_);}

bool Crit3DInterpolationSettings::getProxyActive(proxyVars::TProxyVar myProxyName)
{
    switch (myProxyName)
    {
        case (proxyVars::height):
            return getUseHeight();
            break;

        case (proxyVars::orogIndex):
            return getUseOrogIndex();
            break;

        case (proxyVars::urbanFraction):
            return getUseUrbanFraction();
            break;

        case (proxyVars::seaDistance):
            return getUseSeaDistance();
            break;

        case (proxyVars::aspect):
            return getUseAspect();
            break;

        case (proxyVars::generic):
            return getUseGenericProxy();
            break;

        default:
            return false;
            break;
    }
}


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

#include <bitset>

#include "crit3dDate.h"
#include "interpolationSettings.h"
#include "interpolation.h"
#include "commonConstants.h"
#include "cmath"


bool Crit3DInterpolationSettings::getPrecipitationAllZero() const
{
    return precipitationAllZero;
}

void Crit3DInterpolationSettings::setPrecipitationAllZero(bool value)
{
    precipitationAllZero = value;
}

float Crit3DInterpolationSettings::getMinRegressionR2() const
{
    return minRegressionR2;
}

void Crit3DInterpolationSettings::setMinRegressionR2(float value)
{
    minRegressionR2 = value;
}

bool Crit3DInterpolationSettings::getUseLapseRateCode() const
{
    return useLapseRateCode;
}

void Crit3DInterpolationSettings::setUseLapseRateCode(bool value)
{
    useLapseRateCode = value;
}

bool Crit3DInterpolationSettings::getUseBestDetrending() const
{
    return useBestDetrending;
}

void Crit3DInterpolationSettings::setUseBestDetrending(bool value)
{
    useBestDetrending = value;
}

gridAggregationMethod Crit3DInterpolationSettings::getMeteoGridAggrMethod() const
{
    return meteoGridAggrMethod;
}

void Crit3DInterpolationSettings::setMeteoGridAggrMethod(const gridAggregationMethod &value)
{
    meteoGridAggrMethod = value;
}

float Crit3DInterpolationSettings::getShepardInitialRadius() const
{
    return shepardInitialRadius;
}

void Crit3DInterpolationSettings::setShepardInitialRadius(float value)
{
    shepardInitialRadius = value;
}

int Crit3DInterpolationSettings::getIndexPointCV() const
{
    return indexPointCV;
}

void Crit3DInterpolationSettings::setIndexPointCV(int value)
{
    indexPointCV = value;
}

gis::Crit3DRasterGrid *Crit3DInterpolationSettings::getCurrentDEM() const
{
    return currentDEM;
}

void Crit3DInterpolationSettings::setCurrentDEM(gis::Crit3DRasterGrid *value)
{
    currentDEM = value;
}

float Crit3DInterpolationSettings::getTopoDist_Kh() const
{
    return topoDist_Kh;
}

void Crit3DInterpolationSettings::setTopoDist_Kh(float value)
{
    topoDist_Kh = value;
}

float Crit3DInterpolationSettings::getTopoDist_Kz() const
{
    return topoDist_Kz;
}

void Crit3DInterpolationSettings::setTopoDist_Kz(float value)
{
    topoDist_Kz = value;
}

Crit3DInterpolationSettings::Crit3DInterpolationSettings()
{
    initialize();
}

void Crit3DInterpolationSettings::initializeProxy()
{
    currentProxy.clear();
    selectedCombination.getIndexProxy().clear();
    optimalCombination.getIndexProxy().clear();
}

void Crit3DInterpolationSettings::initialize()
{
    delete currentDEM;
    interpolationMethod = idw;
    useThermalInversion = true;
    useTAD = false;
    useDewPoint = true;
    useBestDetrending = false;
    useLapseRateCode = false;
    minRegressionR2 = float(PEARSONSTANDARDTHRESHOLD);
    meteoGridAggrMethod = aggrAvg;
    initializeProxy();

    isKrigingReady = false;
    precipitationAllZero = false;
    maxHeightInversion = 1000.;
    shepardInitialRadius = NODATA;
    indexPointCV = NODATA;

    currentClimateParametersLoaded = false;
}

float Crit3DInterpolationSettings::getCurrentClimateLapseRate(meteoVariable myVar, Crit3DTime myTime)
{
    Crit3DDate myDate = myTime.date;
    int myHour = myTime.getHour();
    if (currentClimateParametersLoaded && (myDate != getNullDate()) && (myHour != NODATA))
        return (currentClimateParameters.getClimateLapseRate(myVar, myDate, myHour));
    else
        // TODO migliorare
        return -0.006f;
}

void Crit3DInterpolationSettings::setClimateParameters(Crit3DClimateParameters* myParameters)
{
    currentClimateParametersLoaded = true;
    currentClimateParameters = *(myParameters);
}

TInterpolationMethod Crit3DInterpolationSettings::getInterpolationMethod()
{ return interpolationMethod;}

bool Crit3DInterpolationSettings::getUseTAD()
{ return useTAD;}

float Crit3DInterpolationSettings::getMaxHeightInversion()
{ return maxHeightInversion;}

void Crit3DInterpolationSettings::setInterpolationMethod(TInterpolationMethod myValue)
{ interpolationMethod = myValue;}

void Crit3DInterpolationSettings::setUseThermalInversion(bool myValue)
{ useThermalInversion = myValue;}

void Crit3DInterpolationSettings::setUseTAD(bool myValue)
{ useTAD = myValue;}

void Crit3DInterpolationSettings::setUseDewPoint(bool myValue)
{ useDewPoint = myValue;}

bool Crit3DInterpolationSettings::getUseThermalInversion()
{ return (useThermalInversion);}

bool Crit3DInterpolationSettings::getUseDewPoint()
{ return (useDewPoint);}

int Crit3DInterpolationSettings::getProxyNr()
{ return (int)currentProxy.size();}

Crit3DProxyInterpolation* Crit3DInterpolationSettings::getProxy(int pos)
{ return &(currentProxy.at(pos));}

std::string Crit3DProxy::getName() const
{
    return name;
}

void Crit3DProxy::setName(const std::string &value)
{
    name = value;
}

gis::Crit3DRasterGrid *Crit3DProxy::getGrid() const
{
    return grid;
}

void Crit3DProxy::setGrid(gis::Crit3DRasterGrid *value)
{
    grid = value;
}

std::string Crit3DProxy::getGridName() const
{
    return gridName;
}

TProxyVar Crit3DProxy::getProxyPragaName()
{
    if (ProxyVarNames.find(name) == ProxyVarNames.end())
        return TProxyVar::noProxy;
    else
        return ProxyVarNames.at(name);
}

void Crit3DProxy::setGridName(const std::string &value)
{
    gridName = value;
}

bool Crit3DProxy::getIsSignificant() const
{
    return isSignificant;
}

void Crit3DProxy::setIsSignificant(bool value)
{
    isSignificant = value;
}

Crit3DProxy::Crit3DProxy()
{
    name = "";
    isSignificant = false;
    grid = new gis::Crit3DRasterGrid();
}

float Crit3DProxyInterpolation::getLapseRateH1() const
{
    return lapseRateH1;
}

void Crit3DProxyInterpolation::setLapseRateH1(float value)
{
    lapseRateH1 = value;
}

float Crit3DProxyInterpolation::getLapseRateH0() const
{
    return lapseRateH0;
}

void Crit3DProxyInterpolation::setLapseRateH0(float value)
{
    lapseRateH0 = value;
}

float Crit3DProxyInterpolation::getInversionLapseRate() const
{
    return inversionLapseRate;
}

void Crit3DProxyInterpolation::setInversionLapseRate(float value)
{
    inversionLapseRate = value;
}

bool Crit3DProxyInterpolation::getInversionIsSignificative() const
{
    return inversionIsSignificative;
}

void Crit3DProxyInterpolation::setInversionIsSignificative(bool value)
{
    inversionIsSignificative = value;
}

Crit3DProxyInterpolation::Crit3DProxyInterpolation()
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

float Crit3DProxyInterpolation::getValue(unsigned int pos, std::vector <float> proxyValues)
{
    if (pos < proxyValues.size())
        return proxyValues.at(pos);
    else
        return NODATA;
}

void Crit3DProxyInterpolation::initializeOrography()
{
    setLapseRateH0(0.);
    setLapseRateH1(NODATA);
    setInversionLapseRate(NODATA);
    setRegressionSlope(NODATA);
    setRegressionR2(NODATA);
    setInversionIsSignificative(false);

    return;
}

void Crit3DInterpolationSettings::addProxy(Crit3DProxy myProxy)
{
    Crit3DProxyInterpolation myInterpolationProxy;
    myInterpolationProxy.setName(myProxy.getName());
    myInterpolationProxy.setGridName(myProxy.getGridName());
    myInterpolationProxy.setGrid(myProxy.getGrid());
    currentProxy.push_back(myInterpolationProxy);

    selectedCombination.getIndexProxy().push_back((int)currentProxy.size()-1);
}

std::string Crit3DInterpolationSettings::getProxyName(int pos)
{ return currentProxy.at(pos).getName();}

float Crit3DInterpolationSettings::getProxyValue(unsigned int pos, std::vector <float> proxyValues)
{
    if (pos < currentProxy.size())
        return currentProxy.at(pos).getValue(pos, proxyValues);
    else
        return NODATA;
}

Crit3DProxyCombination Crit3DInterpolationSettings::getCombination()
{
    if (getUseBestDetrending())
        return selectedCombination;
    else
        return optimalCombination;
}

void Crit3DInterpolationSettings::computeShepardInitialRadius(float area, int nrPoints)
{
    setShepardInitialRadius(sqrt((SHEPARD_AVG_NRPOINTS * area) / ((float)PI * nrPoints)));
}

std::vector<int> Crit3DProxyCombination::getIndexProxy() const
{
    return indexProxy;
}

void Crit3DProxyCombination::setIndexProxy(const std::vector<int> &value)
{
    indexProxy = value;
}

bool Crit3DProxyCombination::getUseThermalInversion() const
{
    return useThermalInversion;
}

void Crit3DProxyCombination::setUseThermalInversion(bool value)
{
    useThermalInversion = value;
}

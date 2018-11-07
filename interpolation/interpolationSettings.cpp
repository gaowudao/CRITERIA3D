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

#include <string>

#include "crit3dDate.h"
#include "interpolationSettings.h"
#include "interpolation.h"
#include "furtherMathFunctions.h"
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

Crit3DProxyCombination Crit3DInterpolationSettings::getOptimalCombination() const
{
    return optimalCombination;
}

Crit3DProxyCombination* Crit3DInterpolationSettings::getOptimalCombinationRef()
{
    return &optimalCombination;
}

void Crit3DInterpolationSettings::setOptimalCombination(const Crit3DProxyCombination &value)
{
    optimalCombination = value;
}

Crit3DProxyCombination Crit3DInterpolationSettings::getSelectedCombination() const
{
    return selectedCombination;
}

Crit3DProxyCombination* Crit3DInterpolationSettings::getSelectedCombinationRef()
{
    return &selectedCombination;
}

void Crit3DInterpolationSettings::setSelectedCombination(const Crit3DProxyCombination &value)
{
    selectedCombination = value;
}

void Crit3DInterpolationSettings::setValueSelectedCombination(int index, bool isActive)
{
    selectedCombination.setValue(index, isActive);
}

int Crit3DInterpolationSettings::getIndexHeight() const
{
    return indexHeight;
}

void Crit3DInterpolationSettings::setIndexHeight(int value)
{
    indexHeight = value;
}

void Crit3DInterpolationSettings::setCurrentCombination(Crit3DProxyCombination *value)
{
    currentCombination = value;
}

Crit3DProxyCombination *Crit3DInterpolationSettings::getCurrentCombination() const
{
    return currentCombination;
}

std::vector<Crit3DProxy> Crit3DInterpolationSettings::getCurrentProxy() const
{
    return currentProxy;
}

void Crit3DInterpolationSettings::setCurrentProxy(const std::vector<Crit3DProxy> &value)
{
    currentProxy = value;
}

Crit3DInterpolationSettings::Crit3DInterpolationSettings()
{
    initialize();
}

void Crit3DInterpolationSettings::initializeProxy()
{
    currentProxy.clear();
    selectedCombination.getIsActive().clear();
    optimalCombination.getIsActive().clear();
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
    indexHeight = NODATA;

    isKrigingReady = false;
    precipitationAllZero = false;
    maxHeightInversion = 1000.;
    shepardInitialRadius = NODATA;
    indexPointCV = NODATA;

    currentClimateParametersLoaded = false;
    if (currentCombination == NULL)
        currentCombination = new Crit3DProxyCombination();
}

std::string getKeyStringInterpolationMethod(TInterpolationMethod value)
{
    std::map<std::string, TInterpolationMethod>::const_iterator it;
    std::string key = "";

    for (it = interpolationMethodNames.begin(); it != interpolationMethodNames.end(); ++it)
    {
        if (it->second == value)
        {
            key = it->first;
            break;
        }
    }
    return key;
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
{
    useThermalInversion = myValue;
    selectedCombination.setUseThermalInversion(myValue);
}

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

Crit3DProxy* Crit3DInterpolationSettings::getProxy(int pos)
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

TProxyVar getProxyPragaName(std::string name_)
{
    if (ProxyVarNames.find(name_) == ProxyVarNames.end())
        return TProxyVar::noProxy;
    else
        return ProxyVarNames.at(name_);
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

bool Crit3DProxy::getForQualityControl() const
{
    return forQualityControl;
}

void Crit3DProxy::setForQualityControl(bool value)
{
    forQualityControl = value;
}

std::string Crit3DProxy::getProxyTable() const
{
    return proxyTable;
}

void Crit3DProxy::setProxyTable(const std::string &value)
{
    proxyTable = value;
}

std::string Crit3DProxy::getProxyField() const
{
    return proxyField;
}

void Crit3DProxy::setProxyField(const std::string &value)
{
    proxyField = value;
}

Crit3DProxy::Crit3DProxy()
{
    name = "";
    gridName = "";
    grid = new gis::Crit3DRasterGrid();
    isSignificant = false;
    forQualityControl = false;

    regressionR2 = NODATA;
    regressionSlope = NODATA;
    lapseRateH0 = NODATA;
    lapseRateH1 = NODATA;
    inversionLapseRate = NODATA;
    inversionIsSignificative = false;

    proxyTable = "";
    proxyField = "";
}

float Crit3DProxy::getLapseRateH1() const
{
    return lapseRateH1;
}

void Crit3DProxy::setLapseRateH1(float value)
{
    lapseRateH1 = value;
}

float Crit3DProxy::getLapseRateH0() const
{
    return lapseRateH0;
}

void Crit3DProxy::setLapseRateH0(float value)
{
    lapseRateH0 = value;
}

float Crit3DProxy::getInversionLapseRate() const
{
    return inversionLapseRate;
}

void Crit3DProxy::setInversionLapseRate(float value)
{
    inversionLapseRate = value;
}

bool Crit3DProxy::getInversionIsSignificative() const
{
    return inversionIsSignificative;
}

void Crit3DProxy::setInversionIsSignificative(bool value)
{
    inversionIsSignificative = value;
}

void Crit3DProxy::setRegressionR2(float myValue)
{ regressionR2 = myValue;}

float Crit3DProxy::getRegressionR2()
{ return regressionR2;}

void Crit3DProxy::setRegressionSlope(float myValue)
{ regressionSlope = myValue;}

float Crit3DProxy::getRegressionSlope()
{ return regressionSlope;}

float Crit3DProxy::getValue(unsigned int pos, std::vector <float> proxyValues)
{
    if (pos < proxyValues.size())
        return proxyValues.at(pos);
    else
        return NODATA;
}

void Crit3DProxy::initializeOrography()
{
    setLapseRateH0(0.);
    setLapseRateH1(NODATA);
    setInversionLapseRate(NODATA);
    setRegressionSlope(NODATA);
    setRegressionR2(NODATA);
    setInversionIsSignificative(false);

    return;
}

void Crit3DInterpolationSettings::addProxy(Crit3DProxy myProxy, bool isActive_)
{
    currentProxy.push_back(myProxy);

    if (getProxyPragaName(myProxy.getName()) == height)
        setIndexHeight((int)currentProxy.size()-1);

    selectedCombination.addValue(isActive_);
    optimalCombination.addValue(isActive_);
}

bool Crit3DProxy::check(std::string *error)
{
    if (getName() == "")
    {
        *error = "no name for proxy";
        return false;
    }

    if (getGridName() == "")
    {
        *error = "no grid name for proxy " + getName();
        return false;
    }

    if (getGridName() == "")
    {
        gis::Crit3DRasterGrid* grid_ = new gis::Crit3DRasterGrid();
        if (! gis::readEsriGrid(getGridName(), grid_, error))
        {
            *error = "error loading grid for proxy " + getName();
            grid_ = NULL;
            return false;
        }
        grid_ = NULL;
    }

    return true;
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

void Crit3DInterpolationSettings::computeShepardInitialRadius(float area, int nrPoints)
{
    setShepardInitialRadius(sqrt((SHEPARD_AVG_NRPOINTS * area) / ((float)PI * nrPoints)));
}

std::deque<bool> Crit3DProxyCombination::getIsActive() const
{
    return isActive;
}

void Crit3DProxyCombination::setIsActive(const std::deque<bool> &value)
{
    isActive = value;
}

Crit3DProxyCombination::Crit3DProxyCombination()
{
    setUseThermalInversion(false);
}

void Crit3DProxyCombination::addValue(bool isActive_)
{
    isActive.push_back(isActive_);
}

void Crit3DProxyCombination::setValue(int index, bool isActive_)
{
    isActive.at(index) = isActive_;
}

bool Crit3DProxyCombination::getValue(int index)
{
    return isActive.at(index);
}

bool Crit3DProxyCombination::getUseThermalInversion() const
{
    return useThermalInversion;
}

void Crit3DProxyCombination::setUseThermalInversion(bool value)
{
    useThermalInversion = value;
}

bool Crit3DInterpolationSettings::getCombination(int combinationInteger, Crit3DProxyCombination* outCombination)
{
    *outCombination = selectedCombination;
    std::string binaryString = decimal_to_binary(combinationInteger, getProxyNr()+1);

    int indexHeight = getIndexHeight();

    // avoid combinations with inversion (last index) and without orography
    if (combinationInteger % 2 == 1)
        if (indexHeight == NODATA || binaryString[indexHeight] == '0')
            return false;

    for (int i=0; i < binaryString.length()-1; i++)
        outCombination->setValue(i, binaryString[i] == '1' && selectedCombination.getValue(i));

    outCombination->setUseThermalInversion(binaryString.at(binaryString.length()-1) == '1' && selectedCombination.getUseThermalInversion());

    return true;
}


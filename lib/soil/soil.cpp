/*!
    CRITERIA3D

    \copyright 2016 Fausto Tomei, Gabriele Antolini,
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

#include <math.h>

#include "soil.h"
#include "commonConstants.h"
#include <iostream> //debug

namespace soil
{
    Crit3DHorizonDbData::Crit3DHorizonDbData()
    {
        this->horizonNr = NODATA;
        this->upperDepth = NODATA;
        this->lowerDepth = NODATA;
        this->sand = NODATA;
        this->silt = NODATA;
        this->clay = NODATA;
        this->coarseFragments = NODATA;
        this->organicMatter = NODATA;
        this->bulkDensity = NODATA;
        this->thetaSat = NODATA;
        this->kSat = NODATA;
    }

    Crit3DFittingOptions::Crit3DFittingOptions()
    {
        this->waterRetentionCurve = MODIFIEDVANGENUCHTEN;
        this->useWaterRetentionData = false;
        this->airEntryFixed = false;
        this->mRestriction = false;
    }

    Crit3DLayer::Crit3DLayer()
    {
        this->depth = NODATA;
        this->thickness = NODATA;
        this->soilFraction = NODATA;
        this->waterContent = NODATA;
        this->SAT = NODATA;
        this->FC = NODATA;
        this->WP = NODATA;
        this->HH = NODATA;
        this->critical = NODATA;
        this->maxInfiltration = NODATA;
        this->flux = NODATA;

        this->horizon = nullptr;
    }

    Crit3DTexture::Crit3DTexture()
    {
        this->sand = NODATA;
        this->silt = NODATA;
        this->clay = NODATA;
        this->classUSDA = NODATA;
        this->classNL = NODATA;
        this->classNameUSDA = "UNDEFINED";
    }

    Crit3DTexture::Crit3DTexture (float mySand, float mySilt, float myClay)
    {
        this->sand = mySand;
        this->silt = mySilt;
        this->clay = myClay;
        this->classUSDA = getUSDATextureClass(sand, silt, clay);
        this->classNL = getNLTextureClass(sand, silt, clay);
    }

    Crit3DVanGenuchten::Crit3DVanGenuchten()
    {
        this->alpha = NODATA;
        this->n = NODATA;
        this->m = NODATA;
        this->he = NODATA;
        this->sc = NODATA;
        this->thetaR = NODATA;
        this->thetaS = NODATA;
        this->refThetaS = NODATA;
    }

    Crit3DDriessen::Crit3DDriessen()
    {
        this->k0 = NODATA;
        this->gravConductivity = NODATA;
        this->maxSorptivity = NODATA;
    }

    Crit3DWaterConductivity::Crit3DWaterConductivity()
    {
        this->kSat = NODATA;
        this->l = NODATA;
    }


    Crit3DHorizon::Crit3DHorizon()
    {
        this->upperDepth = NODATA;
        this->lowerDepth = NODATA;

        this->coarseFragments = NODATA;
        this->organicMatter = NODATA;
        this->bulkDensity = NODATA;

        this->fieldCapacity = NODATA;
        this->wiltingPoint = NODATA;
        this->waterContentFC = NODATA;
        this->waterContentWP = NODATA;

        this->PH = NODATA;
        this->CEC = NODATA;
    }

    Crit3DSoil::Crit3DSoil()
    {
        this->totalDepth = 0;
        this->nrHorizons = 0;
    }

    void Crit3DSoil::initialize(std::string soilCode, int nrHorizons)
    {
        this->cleanSoil();
        this->code = soilCode;
        this->nrHorizons = unsigned(nrHorizons);
        this->horizon.resize(this->nrHorizons);
        this->totalDepth = 0;
    }

    void Crit3DSoil::addHorizon(int nHorizon, Crit3DHorizon* newHorizon)
    {
        this->horizon.insert(this->horizon.begin() + nHorizon, *newHorizon);
        this->nrHorizons = nrHorizons + 1;
    }

    void Crit3DSoil::deleteHorizon(int nHorizon)
    {
        this->horizon.erase(this->horizon.begin() + nHorizon);
        this->nrHorizons = nrHorizons - 1;
    }

    void Crit3DSoil::cleanSoil()
    {
        this->horizon.clear();
        this->code = "";
        this->nrHorizons = 0;
        this->totalDepth = 0;
    }


    int getUSDATextureClass(Crit3DTexture texture)
    {
        return getUSDATextureClass(texture.sand, texture.silt, texture.clay);
    }

    // [%]
    int getUSDATextureClass(float sand, float silt, float clay)
    {
        if (int(sand) == int(NODATA) || int(silt) == int(NODATA) || int(clay) == int(NODATA))
            return NODATA;

        if (fabs(double(sand + clay + silt) - 100) > 2.0)
            return NODATA;

        int myClass = NODATA;
        /*! clay */
        if (clay >= 40) myClass = 12;
        /*! silty clay */
        if ((silt >= 40) && (clay >= 40)) myClass = 11;
        /*! sandy clay */
        if ((clay >= 35) && (sand >= 45)) myClass = 10;
        /*! silty loam */
        if (((clay < 27.5f) && (silt >= 50) & (silt <= 80)) || ((clay >= 12.5f) && (silt >= 80))) myClass = 4;
        /*! silt */
        if ((clay < 12.5f) && (silt >= 80)) myClass = 6;
        /*! silty clay loam */
        if ((clay < 40) && (sand < 20) && (clay >= 27.5f)) myClass = 8;
        /*! loamy sand */
        if (((clay < 20) && (sand >= 52.5f)) ||
           ((clay < 7.5f) && (silt < 50) && (sand >= 42.5f) && (sand <= 52.5f))) myClass = 3;
        /*! sandy loam */
        if ((sand >= 70) && (clay <= (sand - 70))) myClass = 2;
        /*! sand */
        if ((sand >= 85) && (clay <= (2 * sand -170))) myClass = 1;
        /*! sandy clay loam */
        if ((clay >= 20) && (clay < 35) && (sand >= 45) && (silt < 27.5f)) myClass = 7;
        /*! loam */
        if ((clay >= 7.5f) && (clay < 27.5f) && (sand < 52.5f)  && (silt >= 27.5f) & (silt < 50)) myClass = 5;
        /*! clay loam */
        if ((clay >= 27.5f) && (clay < 40) && (sand >= 20) && (sand < 45)) myClass = 9;

        return myClass;
    }


    int getNLTextureClass(Crit3DTexture texture)
    {
        return getNLTextureClass(texture.sand, texture.silt, texture.clay);
    }

    /*!
     * \brief NL texture (used by Driessen) different from USDA only in clay zone
     * \param sand
     * \param silt
     * \param clay
     * \return result
     */
    int getNLTextureClass(float sand, float silt, float clay)
    {
        if (int(sand) == int(NODATA) || int(silt) == int(NODATA) || int(clay) == int(NODATA))
            return NODATA;

        if (fabs(double(sand + clay + silt) - 100) > 2.0)
            return NODATA;

        /*! heavy clay */
        if (clay >= 65) return 12;

        if (clay > 40)
        {
            /*! silty clay */
            if (silt > 40)return 11;
            /*! light clay */
            else return 10;
        }
        else return getUSDATextureClass(sand, silt, clay);
    }


    double getSpecificDensity(double organicMatter)
    {
        const double MINIMUM_ORGANIC_MATTER = 0.01;

        if (int(organicMatter) == int(NODATA))
        {
            organicMatter = MINIMUM_ORGANIC_MATTER;
        }

        /*! [Driessen] */
        return 1.0 / ((1.0 - organicMatter) / QUARTZ_DENSITY + organicMatter / 1.43);
    }


    // estimate bulk density from total porosity
    double estimateBulkDensity(Crit3DHorizon* horizon, double totalPorosity, bool increaseWithDepth = false)
    {
        if (int(totalPorosity) == int(NODATA))
            totalPorosity = horizon->vanGenuchten.refThetaS;

        double specificDensity = getSpecificDensity(horizon->organicMatter);
        double refBulkDensity = (1 - totalPorosity) * specificDensity;

        // increase/decrease with depth, reference theta sat at 33cm
        if (increaseWithDepth)
        {
            double depth = (horizon->upperDepth + horizon->lowerDepth) * 0.5;
            double depthCoeff = (depth - 0.33) * 0.1;
            refBulkDensity *= (1.0 + depthCoeff);
        }

        return refBulkDensity;
    }


    double estimateTotalPorosity(Crit3DHorizon* horizon, double bulkDensity)
    {
        if (int(bulkDensity) == int(NODATA)) return NODATA;

        double specificDensity = getSpecificDensity(horizon->organicMatter);
        return 1 - (bulkDensity /specificDensity);
    }


    double estimateSaturatedConductivity(Crit3DHorizon* horizon, double bulkDensity)
    {
        if (int(bulkDensity) == int(NODATA)) return NODATA;

        double refTotalPorosity = horizon->vanGenuchten.refThetaS;
        double specificDensity = getSpecificDensity(horizon->organicMatter);
        double refBulkDensity = (1 - refTotalPorosity) * specificDensity;
        double ratio = 1 - bulkDensity / refBulkDensity;

        double refKsat = horizon->waterConductivity.kSat;
        double factor = pow(10, 2 * ratio);
        return refKsat * factor;
    }


    int getHorizonIndex(Crit3DSoil* soil, double depth)
    {
       for (unsigned int index = 0; index < soil->nrHorizons; index++)
       {
           if (depth >= soil->horizon[index].upperDepth && depth <= (soil->horizon[index].lowerDepth + EPSILON))
               return(int(index));
       }

       return NODATA;
    }


    /*!
     * \brief Field Capacity water potential as clay function
     * \param horizon
     * \param unit [KPA | METER | CM]
     * \note author: Franco Zinoni
     * \return Water potential at field capacity [unit]
     */
    double getFieldCapacity(Crit3DHorizon* horizon, soil::units unit)
    {
        double fcMin = -10;                 /*!< [kPa] clay < 20% sandy soils */
        double fcMax = -33;                 /*!< [kPa] clay > 50% clay soils */

        const double CLAYMIN = 20;
        const double CLAYMAX = 50;

        double fieldCapacity;

        if (double(horizon->texture.clay) <= CLAYMIN)
            fieldCapacity = fcMin;
        else if (double(horizon->texture.clay) >= CLAYMAX)
            fieldCapacity = fcMax;
        else
        {
            double clayFactor = (double(horizon->texture.clay) - CLAYMIN) / (CLAYMAX - CLAYMIN);
            fieldCapacity = (fcMin + (fcMax - fcMin) * clayFactor);
        }

        if (unit == KPA)
            return fieldCapacity;
        else if (unit == METER)
            return kPaToMeters(fieldCapacity);
        else if (unit == CM)
            return kPaToCm(fieldCapacity);
        else
            return fieldCapacity;
    }


    /*!
     * \brief [m] WP = Wilting Point
     * \param unit
     * \return wilting point
     */
    double getWiltingPoint(soil::units unit)
    {           
        if (unit == KPA)
            return -1600;
        else if (unit == METER)
            return kPaToMeters(-1600);
        else if (unit == CM)
            return kPaToCm(-1600);
        else
            return(-1600);
    }


    double kPaToMeters(double value)
    {
        return (value / GRAVITY);
    }

    double metersTokPa(double value)
    {
        return (value * GRAVITY);
    }

    double kPaToCm(double value)
    {
        return kPaToMeters(value) * 100;
    }

    double cmTokPa(double value)
    {
        return metersTokPa(value / 100);
    }


    /*!
     * \brief Compute degree of saturation from volumetric water content
     * \param theta [m^3 m-3] volumetric water content
     * \param horizon pointer to Crit3DHorizon class
     * \return [-] degree of saturation
     */
    double SeFromTheta(double theta, Crit3DHorizon* horizon)
    {
        // check range
        if (theta >= horizon->vanGenuchten.thetaS) return 1.;
        if (theta <= horizon->vanGenuchten.thetaR) return 0.;

        return (theta - horizon->vanGenuchten.thetaR) / (horizon->vanGenuchten.thetaS - horizon->vanGenuchten.thetaR);
    }


    /*!
     * \brief Compute water potential from volumetric water content
     * \param theta  [m^3 m-3] volumetric water content
     * \param horizon pointer to Crit3DHorizon class
     * \return [same unit of vanGenuchten.he] water potential
     */
    double psiFromTheta(double theta, Crit3DHorizon* horizon)

    {
        double Se = SeFromTheta(theta, horizon);
        double temp = pow(1.0 / (Se * horizon->vanGenuchten.sc), 1.0 / horizon->vanGenuchten.m) - 1.0;
        double psi = (1.0 / horizon->vanGenuchten.alpha) * pow(temp, 1.0/ horizon->vanGenuchten.n);
        return psi;
    }


    /*!
     * \brief Compute water content from signed water potential
     * \param signPsi water potential       [kPa]
     * \param horizon
     * \return volumetric water content     [m^3 m-3]
     */
    double thetaFromSignPsi(double signPsi, Crit3DHorizon* horizon)
    {
        if (signPsi >= 0.0) return horizon->vanGenuchten.thetaS;

        double psi = fabs(signPsi);
        if (psi <=  horizon->vanGenuchten.he) return horizon->vanGenuchten.thetaS;

        //[-] degree of saturation
        double Se = pow(1.0 + pow(horizon->vanGenuchten.alpha * psi, horizon->vanGenuchten.n),
                        - horizon->vanGenuchten.m) / horizon->vanGenuchten.sc;

        double theta = (Se * (horizon->vanGenuchten.thetaS - horizon->vanGenuchten.thetaR) + horizon->vanGenuchten.thetaR);
        return theta;
    }


    /*!
     * \brief Compute hydraulic conductivity
     * \param Se [-] degree of saturation
     * \param horizon pointer to Crit3DHorizon class
     * \note Mualem equation for modified Van Genuchten curve: K(Se) = Ksat * Se^(L) * {1-[1-Se^(1/m)]^m}^2
     * \warning very low values are possible (es: 10^12)
     * \return hydraulic conductivity
     *  unit is the same of horizon->waterConductivity.kSat usually [cm day^-1]
     */
    double waterConductivity(double Se, Crit3DHorizon* horizon)
    {
        if (Se >= 1.) return(horizon->waterConductivity.kSat);

        double myTmp = NODATA;

        double myNumerator = 1. - pow(1. - pow(Se * horizon->vanGenuchten.sc, 1.0 / horizon->vanGenuchten.m), horizon->vanGenuchten.m);
        myTmp = myNumerator / (1. - pow(1. - pow(horizon->vanGenuchten.sc, 1.0 / horizon->vanGenuchten.m), horizon->vanGenuchten.m));

        return (horizon->waterConductivity.kSat * pow(Se, horizon->waterConductivity.l) * pow(myTmp , 2.0));
    }

    /*!
     * \brief getWaterContentFromPsi
     * \param psi [kPa]
     * \param layer pointer to Crit3DLayer class
     * \return water content at water potential psi [mm]
     */
    double getWaterContentFromPsi(double psi, Crit3DLayer* layer)
    {
        double theta = soil::thetaFromSignPsi(-psi, layer->horizon);
        return theta * layer->thickness * layer->soilFraction * 1000;
    }


    /*!
     * \brief getWaterContentFromAW
     * \param availableWater [-] (0: wilting point, 1: field capacity)
     * \param layer pointer to Crit3DLayer class
     * \return  water content at specific available water [mm]
     */
    double getWaterContentFromAW(double availableWater, Crit3DLayer* layer)
    {
        if (availableWater < 0)
            return (layer->WP);

        else if (availableWater > 1)
            return (layer->FC);

        else
            return (layer->WP + availableWater * (layer->FC - layer->WP));
    }


    /*!
     * \brief getVolumetricWaterContent
     * \param layer pointer to Crit3DLayer class
     * \return current volumetric water content of layer [-]
     */
    double getVolumetricWaterContent(Crit3DLayer* layer)
    {
        // layer->thickness in [m]
        double theta = layer->waterContent / (layer->thickness * layer->soilFraction * 1000);
        return theta;
    }


    /*!
     * \brief getWaterPotential
     * \param layer pointer to Crit3DLayer class
     * \return current water potential of layer [kPa]
     */
    double getWaterPotential(Crit3DLayer* layer)
    {
        double theta = getVolumetricWaterContent(layer);
        return psiFromTheta(theta, layer->horizon);
    }


    /*!
     * \brief getWaterConductivity
     * \param layer pointer to Crit3DLayer class
     * \return current hydraulic conductivity of layer
     *  unit is the same of horizon->waterConductivity.kSat - usually [cm day^-1]
     */
    double getWaterConductivity(Crit3DLayer* layer)
    {
        double theta = getVolumetricWaterContent(layer);
        double degreeOfSaturation = SeFromTheta(theta, layer->horizon);
        return waterConductivity(degreeOfSaturation, layer->horizon);
    }


    // It assumes that dbData are loaded
    bool setHorizon(Crit3DHorizon* horizon, Crit3DTextureClass* textureClassList,
                    Crit3DFittingOptions* fittingOptions, std::string* error)
    {
        *error = "";

        // depth [cm]->[m]
        if (horizon->dbData.upperDepth != NODATA && horizon->dbData.lowerDepth != NODATA)
        {
            horizon->upperDepth = horizon->dbData.upperDepth / 100;
            horizon->lowerDepth = horizon->dbData.lowerDepth / 100;
        }
        else
        {
            *error += "wrong depth";
            return false;
        }

        // coarse fragments [%] -> 0-1
        if (horizon->dbData.coarseFragments != NODATA
                && horizon->dbData.coarseFragments >= 0
                && horizon->dbData.coarseFragments < 100)
        {
            horizon->coarseFragments = horizon->dbData.coarseFragments / 100;
        }
        else
        {
            // default: no coarse fragment
            horizon->coarseFragments = 0.0;
        }

        // organic matter [%] -> 0-1
        if (horizon->dbData.organicMatter != NODATA
                && horizon->dbData.organicMatter > 0
                && horizon->dbData.organicMatter < 100)
        {
            horizon->organicMatter = horizon->dbData.organicMatter / 100;
        }
        else
        {
            // default: 0.5%
            horizon->organicMatter = 0.005;
        }

        // sand, silt, clay [%]
        horizon->texture.sand = horizon->dbData.sand;
        horizon->texture.silt = horizon->dbData.silt;
        horizon->texture.clay = horizon->dbData.clay;
        if (horizon->texture.sand <= 1 && horizon->texture.silt <= 1 && horizon->texture.clay <= 1)
        {
            horizon->texture.sand *= 100;
            horizon->texture.silt *= 100;
            horizon->texture.clay *= 100;
        }

        // texture
        horizon->texture.classUSDA = soil::getUSDATextureClass(horizon->texture);
        if (horizon->texture.classUSDA == NODATA)
        {
            *error = "sand+silt+clay <> 100";
            return false;
        }

        horizon->texture.classNL = soil::getNLTextureClass(horizon->texture);
        horizon->texture.classNameUSDA = textureClassList[horizon->texture.classUSDA].classNameUSDA;

        // assign default parameters from texture class
        horizon->vanGenuchten = textureClassList[horizon->texture.classUSDA].vanGenuchten;
        horizon->waterConductivity = textureClassList[horizon->texture.classUSDA].waterConductivity;
        horizon->Driessen = textureClassList[horizon->texture.classNL].Driessen;

        // bulk density [g cm-3]
        horizon->bulkDensity = NODATA;
        if (horizon->dbData.bulkDensity > 0 && horizon->dbData.bulkDensity < QUARTZ_DENSITY)
        {
            horizon->bulkDensity = horizon->dbData.bulkDensity;
        }

        // check theta sat [m3 m-3]
        if (horizon->dbData.thetaSat != NODATA && horizon->dbData.thetaSat > 0 && horizon->dbData.thetaSat < 1)
        {
            horizon->vanGenuchten.thetaS = horizon->dbData.thetaSat;
        }
        else if(horizon->bulkDensity != NODATA)
        {
            horizon->vanGenuchten.thetaS = soil::estimateTotalPorosity(horizon, horizon->bulkDensity);
        }

        if (horizon->bulkDensity == NODATA)
        {
            horizon->bulkDensity = soil::estimateBulkDensity(horizon, horizon->vanGenuchten.thetaS, false);
        }

        // Ksat = saturated water conductivity [cm day-1]
        if (horizon->dbData.kSat != NODATA && horizon->dbData.kSat > 0)
        {
            // check ksat value
            if (horizon->dbData.kSat < (horizon->waterConductivity.kSat / 100))
            {
                horizon->waterConductivity.kSat /= 100;
                *error = "Ksat is out of class limits.";
            }
            else if (horizon->dbData.kSat > (horizon->waterConductivity.kSat * 100))
            {
                horizon->waterConductivity.kSat *= 100;
                *error = "Ksat is out of class limits.";
            }
            else
            {
                horizon->waterConductivity.kSat = horizon->dbData.kSat;
            }
        }
        else
        {
            horizon->waterConductivity.kSat = soil::estimateSaturatedConductivity(horizon, horizon->bulkDensity);
        }

        // update with coarse fragment
        horizon->vanGenuchten.thetaS *= (1.0 - horizon->coarseFragments);
        horizon->vanGenuchten.thetaR *= (1.0 - horizon->coarseFragments);

        horizon->CEC = 50.0;
        horizon->PH = 7.7;

        horizon->fieldCapacity = soil::getFieldCapacity(horizon, soil::KPA);
        horizon->wiltingPoint = soil::getWiltingPoint(soil::KPA);
        horizon->waterContentFC = soil::thetaFromSignPsi(horizon->fieldCapacity, horizon);
        horizon->waterContentWP = soil::thetaFromSignPsi(horizon->wiltingPoint, horizon);

        return true;
    }
}

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

namespace soil
{
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
    }

    Crit3DTexture::Crit3DTexture()
    {
        this->sand = NODATA;
        this->silt = NODATA;
        this->clay = NODATA;
        this->classUSDA = NODATA;
        this->classNL = NODATA;
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
        this->id = NODATA;
        this->totalDepth = 0;
        this->nrHorizons = 0;
    }

    Crit3DSoil::Crit3DSoil(int idSoil, int nrHorizons)
    {
        this->initialize(idSoil, nrHorizons);
    }

    void Crit3DSoil::initialize(int idSoil, int nrHorizons)
    {
        this->cleanSoil();
        this->id = idSoil;
        this->nrHorizons = nrHorizons;
        this->horizon = new Crit3DHorizon[this->nrHorizons];
        this->totalDepth = 0;
    }

    void Crit3DSoil::cleanSoil()
    {
        if (this->nrHorizons > 0)
            delete (this->horizon);

        this->id = NODATA;
        this->nrHorizons = 0;
        this->totalDepth = 0;
    }

    // [%]
    int getUSDATextureClass(float sand, float silt, float clay)
    {
        if ((sand == NODATA) || (silt == NODATA) || (clay == NODATA)) return NODATA;

        float sum = sand + clay + silt;
        if (fabs(sum - 100.0) > 2.0) return NODATA;

        int myClass = NODATA;
        /*! clay */
        if (clay >= 40) myClass = 12;
        /*! silty clay */
        if ((silt >= 40) && (clay >= 40)) myClass = 11;
        /*! sandy clay */
        if ((clay >= 35) && (sand >= 45)) myClass = 10;
        /*! silty loam */
        if (((clay < 27.5) && (silt >= 50) & (silt <= 80)) || ((clay >= 12.5) && (silt >= 80))) myClass = 4;
        /*! silt */
        if ((clay < 12.5) && (silt >= 80)) myClass = 6;
        /*! silty clay loam */
        if ((clay < 40) && (sand < 20) && (clay >= 27.5)) myClass = 8;
        /*! loamy sand */
        if (((clay < 20) && (sand >= 52.5)) ||
           ((clay < 7.5) && (silt < 50) && (sand >= 42.5) && (sand <= 52.5))) myClass = 3;
        /*! sandy loam */
        if ((sand >= 70) && (clay <= (sand - 70))) myClass = 2;
        /*! sand */
        if ((sand >= 85) && (clay <= (2 * sand -170))) myClass = 1;
        /*! sandy clay loam */
        if ((clay >= 20) && (clay < 35) && (sand >= 45) && (silt < 27.5)) myClass = 7;
        /*! loam */
        if ((clay >= 7.5) && (clay < 27.5) && (sand < 52.5)  && (silt >= 27.5) & (silt < 50)) myClass = 5;
        /*! clay loam */
        if ((clay >= 27.5) && (clay < 40) && (sand >= 20) && (sand < 45)) myClass = 9;

        return myClass;
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
        if ((sand == NODATA) || (silt == NODATA) || (clay == NODATA)) return NODATA;

        float sum = sand + clay + silt;
        if (fabs(sum - 100.0) > 2.0) return NODATA;

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
        if (organicMatter == NODATA) organicMatter = 0.01;

        /*! [Driessen] */
        return 1.0 / ((1.0 - organicMatter) / 2.65 + organicMatter / 1.43);
    }


    double estimateBulkDensity(Crit3DHorizon* mySoil, double totalPorosity)
    {
        if (totalPorosity == NODATA)
            totalPorosity = mySoil->vanGenuchten.refThetaS;

        double specificDensity = getSpecificDensity(mySoil->organicMatter);
        double refBulkDensity = (1 - totalPorosity) * specificDensity;

        return refBulkDensity;

        /* VALUTARE se inserire (introdotto in praga)
        // increase/decrease with depth, reference thetaSat at 33cm

        double depth = (mySoil->upperDepth + mySoil->lowerDepth) * 0.5;
        double depthCoeff = (depth - 0.33) * 0.1;
        return refBulkDensity * (1.0 + depthCoeff);
        */
    }


    double estimateTotalPorosity(Crit3DHorizon* mySoil, double bulkDensity)
    {
        if (bulkDensity == NODATA) return(NODATA);
        double specificDensity = getSpecificDensity(mySoil->organicMatter);
        return (1.0 - (bulkDensity /specificDensity));
    }


    double estimateSaturatedConductivity(Crit3DHorizon* mySoil, double bulkDensity)
    {
        if (bulkDensity == NODATA) return(NODATA);
        double refTotalPorosity = mySoil->vanGenuchten.refThetaS;
        double specificDensity = getSpecificDensity(mySoil->organicMatter);
        double refBulkDensity = (1.0 - refTotalPorosity) * specificDensity;
        double ratio = 1 - bulkDensity / refBulkDensity;

        double refKsat = mySoil->waterConductivity.kSat;
        double factor = pow(10.0, 2.0 * ratio);
        return refKsat * factor;
    }


    int getHorizonIndex(Crit3DSoil* soil, double depth)
    {
       for (int index = 0; index < soil->nrHorizons; index++)
           if (depth >= soil->horizon[index].upperDepth &&
               depth <= (soil->horizon[index].lowerDepth + EPSILON))
               return(index);
       return(NODATA);
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

        const double CLAYMIN = 20.f;
        const double CLAYMAX = 50.f;

        double fieldCapacity;

        if (horizon->texture.clay <= CLAYMIN)
            fieldCapacity = fcMin;
        else if (horizon->texture.clay >= CLAYMAX)
            fieldCapacity = fcMax;
        else
        {
            double clayFactor = (horizon->texture.clay - CLAYMIN) / (CLAYMAX - CLAYMIN);
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
            return -1600.f;
        else if (unit == METER)
            return kPaToMeters(-1600.f);
        else if (unit == CM)
            return kPaToCm(-1600.f);
        else
            return(-1600.f);
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
        return kPaToMeters(value) * 100.f;
    }

    double cmTokPa(double value)
    {
        return metersTokPa(value / 100.f);
    }

    double getThetaFC(Crit3DHorizon* horizon)
    {
        double psi = getFieldCapacity(horizon, soil::KPA);
        return thetaFromSignPsi(psi, horizon);
    }

    double getThetaWP(Crit3DHorizon* horizon)
    {
        double psi = getWiltingPoint(soil::KPA);
        return thetaFromSignPsi(psi, horizon);
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

        return (theta - horizon->vanGenuchten.thetaR)/(horizon->vanGenuchten.thetaS - horizon->vanGenuchten.thetaR);
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
        return theta * layer->thickness * layer->soilFraction * 1000.f;
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
        double theta = layer->waterContent / (layer->thickness * layer->soilFraction * 1000.f);
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

}

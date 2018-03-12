/*!
    \name Solar Radiation
    \copyright 2011 Fausto Tomei, Gabriele Antolini
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

    contacts:
    fausto.tomei@gmail.com
    ftomei@arpae.it
    gantolini@arpae.it
*/

#include "commonConstants.h"
#include "sunPosition.h"
#include "solarRadiation.h"
#include <cmath>


Crit3DRadiationMaps::Crit3DRadiationMaps()
{
    latMap = new gis::Crit3DRasterGrid;
    lonMap = new gis::Crit3DRasterGrid;
    slopeMap = new gis::Crit3DRasterGrid;
    aspectMap = new gis::Crit3DRasterGrid;
    linkeMap = new gis::Crit3DRasterGrid;
    albedoMap = new gis::Crit3DRasterGrid;

    clearSkyTransmissivityMap = new gis::Crit3DRasterGrid;
    transmissivityMap = new gis::Crit3DRasterGrid;

    beamRadiationMap = new gis::Crit3DRasterGrid;
    diffuseRadiationMap = new gis::Crit3DRasterGrid;
    reflectedRadiationMap = new gis::Crit3DRasterGrid;
    globalRadiationMap = new gis::Crit3DRasterGrid;
    sunAzimuthMap = new gis::Crit3DRasterGrid;
    sunElevationMap = new gis::Crit3DRasterGrid;
    sunIncidenceMap = new gis::Crit3DRasterGrid;
    sunRiseMap = new gis::Crit3DRasterGrid;
    sunSetMap = new gis::Crit3DRasterGrid;
    sunShadowMap = new gis::Crit3DRasterGrid;

    isLoaded = false;
}

Crit3DRadiationMaps::Crit3DRadiationMaps(const gis::Crit3DRasterGrid& myDtm, const gis::Crit3DGisSettings& myGisSettings)
{
    latMap = new gis::Crit3DRasterGrid;
    lonMap = new gis::Crit3DRasterGrid;
    slopeMap = new gis::Crit3DRasterGrid;
    aspectMap = new gis::Crit3DRasterGrid;
    albedoMap = new gis::Crit3DRasterGrid;
    linkeMap = new gis::Crit3DRasterGrid;

    gis::computeSlopeAspectMaps(myDtm, slopeMap, aspectMap);
    gis::computeLatLonMaps(myDtm, latMap, lonMap, myGisSettings);
    linkeMap->initializeGrid(myDtm);
    albedoMap->initializeGrid(myDtm);

    clearSkyTransmissivityMap = new gis::Crit3DRasterGrid;
    transmissivityMap = new gis::Crit3DRasterGrid;
    /*! initialize with default value */
    clearSkyTransmissivityMap->initializeGrid(myDtm, CLEAR_SKY_TRANSMISSIVITY_DEFAULT);
    transmissivityMap->initializeGrid(myDtm, CLEAR_SKY_TRANSMISSIVITY_DEFAULT);

    beamRadiationMap = new gis::Crit3DRasterGrid;
    diffuseRadiationMap = new gis::Crit3DRasterGrid;
    reflectedRadiationMap = new gis::Crit3DRasterGrid;
    globalRadiationMap = new gis::Crit3DRasterGrid;
    sunAzimuthMap = new gis::Crit3DRasterGrid;
    sunElevationMap = new gis::Crit3DRasterGrid;
    sunIncidenceMap = new gis::Crit3DRasterGrid;
    sunRiseMap = new gis::Crit3DRasterGrid;
    sunSetMap = new gis::Crit3DRasterGrid;
    sunShadowMap = new gis::Crit3DRasterGrid;

    beamRadiationMap->initializeGrid(myDtm);
    diffuseRadiationMap->initializeGrid(myDtm);
    reflectedRadiationMap->initializeGrid(myDtm);
    globalRadiationMap->initializeGrid(myDtm);
    sunAzimuthMap->initializeGrid(myDtm);
    sunElevationMap->initializeGrid(myDtm);
    sunIncidenceMap->initializeGrid(myDtm);
    sunRiseMap->initializeGrid(myDtm);
    sunSetMap->initializeGrid(myDtm);
    sunShadowMap->initializeGrid(myDtm);

    isLoaded = true;
}

Crit3DRadiationMaps::~Crit3DRadiationMaps()
{
    this->clean();
}


void Crit3DRadiationMaps::clean()
{
    latMap->freeGrid();
    lonMap->freeGrid();
    slopeMap->freeGrid();
    aspectMap->freeGrid();
    beamRadiationMap->freeGrid();
    diffuseRadiationMap->freeGrid();
    reflectedRadiationMap->freeGrid();
    globalRadiationMap->freeGrid();
    albedoMap->freeGrid();
    clearSkyTransmissivityMap->freeGrid();
    linkeMap->freeGrid();
    sunAzimuthMap->freeGrid();
    sunElevationMap->freeGrid();
    sunIncidenceMap->freeGrid();
    sunRiseMap->freeGrid();
    sunSetMap->freeGrid();
    sunShadowMap->freeGrid();
    transmissivityMap->freeGrid();

    delete latMap;
    delete lonMap;
    delete slopeMap;
    delete aspectMap;
    delete beamRadiationMap;
    delete diffuseRadiationMap;
    delete reflectedRadiationMap;
    delete globalRadiationMap;
    delete albedoMap;
    delete clearSkyTransmissivityMap;
    delete linkeMap;
    delete sunAzimuthMap;
    delete sunElevationMap;
    delete sunIncidenceMap;
    delete sunRiseMap;
    delete sunSetMap;
    delete sunShadowMap;
    delete transmissivityMap;

    isLoaded = false;
}


Crit3DTransmissivityPoint::Crit3DTransmissivityPoint()
{
    isActive = false;
    point = new gis::Crit3DPoint();
    point->utm.x = NODATA;
    point->utm.y = NODATA;
    point->z = NODATA;
}


float getSinDecimalDegree(float angle)
{
    while (angle > 360) angle -= 360 ;
    while (angle < -360) angle +=360 ;
    return (float)sin(angle * DEG_TO_RAD);
}

float getCosDecimalDegree(float angle)
{
    while (angle > 360) angle -= 360 ;
    while (angle < -360) angle +=360 ;
    return (float)cos(angle * DEG_TO_RAD);
}


namespace radiation
{
    Crit3DRadiationSettings radiationSettings;
    std::vector <Crit3DTransmissivityPoint> transmissivityPointList;

    double linkeMountain[13] = {1.5, 1.6, 1.8, 1.9, 2.0, 2.3, 2.3, 2.3, 2.1, 1.8, 1.6, 1.5, 1.9};
    double linkeRural[13] = {2.1, 2.2, 2.5, 2.9, 3.2, 3.4, 3.5, 3.3, 2.9, 2.6, 2.3, 2.2, 2.75};
    double linkeCity[13] = {3.1, 3.2, 3.5, 4.0, 4.2, 4.3, 4.4, 4.3, 4.0, 3.6, 3.3, 3.1, 3.75};
    double linkeIndustrial[13] = {4.1, 4.3, 4.7, 5.3, 5.5, 5.7, 5.8, 5.7, 5.3, 4.9, 4.5, 4.2, 5.0};

    float linkeSinusoidal(float amplitude , float phase , float average , int doy)
    {
        return float(amplitude * (1.0 - cos(doy / 365.0 * 2.0 * PI - phase)) + average);
    }

    void setRadiationSettings(Crit3DRadiationSettings* mySettings)
    {
        radiationSettings = *(mySettings);
    }

    /*!
     * \brief Average monthly values of the Linke turbidity coefficient for a mild Climate
     * \param landUse
     * \param myMonth
     * \return result
     */
    float linkeMonthly(int landUse,int myMonth)
    {
        switch(landUse)
        {
            case LAND_USE_MOUNTAIN:
                return float(linkeMountain[myMonth - 1]);
                break ;
            case LAND_USE_RURAL:
                return float(linkeRural[myMonth - 1]);
                break;
            case LAND_USE_CITY:
                return float(linkeCity[myMonth - 1]);
                break;
            case LAND_USE_INDUSTRIAL:
                return float(linkeIndustrial[myMonth - 1]);
                break;
        }
        return NODATA;
    }


    float readAlbedo(int myRow, int myCol, const gis::Crit3DRasterGrid& albedoMap)
    {
        float myAlbedo = NODATA;
        switch (radiationSettings.getAlbedoMode())
        {
            case PARAM_MODE_FIXED:
                myAlbedo = radiationSettings.getAlbedo();
                break;
            case PARAM_MODE_MAP:
                myAlbedo = albedoMap.value[myRow][myCol];
                break;
        }
        return (myAlbedo);
    }


    float readAlbedo()
    {
        return radiationSettings.getAlbedo();
    }

    float readLinke(int myRow, int myCol, const gis::Crit3DRasterGrid& linkeMap)
    {
        float output = NODATA;
        switch(radiationSettings.getLinkeMode())
        {
            case PARAM_MODE_FIXED:
                output = radiationSettings.getLinke();
                break;

            case PARAM_MODE_MAP:
                output = linkeMap.value[myRow][myCol];
                break;
        }
        return output;
    }

    float readLinke()
    {
        float output = NODATA;
        switch(radiationSettings.getLinkeMode())
        {
            case PARAM_MODE_FIXED:
                output = radiationSettings.getLinke();
                break;

            case PARAM_MODE_MAP:
                 output = radiationSettings.getLinke();
                 break;
        }
        return output;
    }

    float readSlope()
    {
        float output = NODATA;
        switch (radiationSettings.getTiltMode())
        {
            case TILT_TYPE_HORIZONTAL:
                output = radiationSettings.getTilt();
                break;
            case TILT_TYPE_INCLINED:
                output = radiationSettings.getTilt();
                break;
            case TILT_TYPE_DEM:
                output = NODATA;
                break;
        }
        return output;
    }

    float readAspect(gis::Crit3DRasterGrid* aspectMap, int myRow,int myCol)
    {
        float output = NODATA;

        switch (radiationSettings.getTiltMode())
        {
            case TILT_TYPE_HORIZONTAL:
                output = radiationSettings.getAspect();
                break;
            case TILT_TYPE_INCLINED:
                output = radiationSettings.getAspect();
                break;
            case TILT_TYPE_DEM:
                output = aspectMap->value[myRow][myCol];
                break;
        }
        return output;
    }

    float readSlope(gis::Crit3DRasterGrid* slopeMap,int myRow,int myCol)
    {
        float output = NODATA;

        switch (radiationSettings.getTiltMode())
        {
            case TILT_TYPE_HORIZONTAL:
                output = radiationSettings.getTilt();
                break;
            case TILT_TYPE_INCLINED:
                output = radiationSettings.getTilt();
                break;
            case TILT_TYPE_DEM:
                output = slopeMap->value[myRow][myCol];
                break;
        }
        return output;
    }

    /*!
     * \brief Clear sky beam irradiance on a horizontal surface [W m-2]
     * \param myLinke
     * \param mySunPosition a pointer to a TsunPosition
     * \return result
     */
    float clearSkyBeamHorizontal(float myLinke, TsunPosition* mySunPosition)
    {
        //tl:        air mass 2 Linke atmospheric turbidity factor (Kasten, 1996)    []
        //rayl:      Rayleigh optical thickness (Kasten, 1996)                       []
        //am:        relative optical air mass corrected for pressure                []
        float turbidityLinke, rayleighThickness;
        float airMass ;

        airMass = mySunPosition->relOptAirMassCorr ;
        turbidityLinke = float(0.8662 * myLinke);
        if (airMass <= 20)
            rayleighThickness = float(1.0 / (6.6296 + 1.7513 * airMass - 0.1202 * pow(airMass,2)
                                            + 0.0065 * pow(airMass,3) - 0.00013 * pow(airMass,4)));
        else
            rayleighThickness = float(1.0 / (10.4 + 0.718 * airMass));

        return mySunPosition->extraIrradianceNormal * getSinDecimalDegree(mySunPosition->elevationRefr) * exp(-rayleighThickness * airMass * turbidityLinke);
    }


    /*!
     * \brief Beam irradiance on an inclined surface                         [W m-2]
     * \param beamIrradianceHor
     * \param mySunPosition a pointer to a TsunPosition
     * \return result
     */
    float clearSkyBeamInclined(float beamIrradianceHor, TsunPosition* mySunPosition )
    {
        /*! Bh: clear sky beam irradiance on a horizontal surface */
        return (beamIrradianceHor * getSinDecimalDegree(mySunPosition->incidence) / getSinDecimalDegree(mySunPosition->elevationRefr)) ;
    }

    /*!
     * \brief Diffuse irradiance on a horizontal surface (Scharmer and Greif, 2000   [W m-2]
     * \param myLinke
     * \param mySunPosition a pointer to a TsunPosition
     * \return result
     */
    float clearSkyDiffuseHorizontal(float myLinke, TsunPosition* mySunPosition)
    {
        double diffuseSolarAltitudeFunction; /*!< diffuse solar altitude function (Scharmer and Greif, 2000)     []   */
        double transmissionFunction;         /*!< ransmission function                                           []   */
        double A1b, a1, a2, A3;
        double sinElev;

        sinElev = maxValue(getSinDecimalDegree(mySunPosition->elevation), float(0.00001));
        transmissionFunction = -0.015843 + myLinke * (0.030543 + 0.0003797 * myLinke);
        A1b = 0.26463 + myLinke * (-0.061581 + 0.0031408 * myLinke);

        if ((A1b * transmissionFunction) < 0.0022)
            a1 = 0.0022 / transmissionFunction;
        else
            a1 = A1b;

        a2 = 2.0402 + myLinke * (0.018945 - 0.011161 * myLinke);
        A3 = -1.3025 + myLinke * (0.039231 + 0.0085079 * myLinke);
        diffuseSolarAltitudeFunction = a1 + a2 * sinElev + A3 * sinElev * sinElev;

        return float(mySunPosition->extraIrradianceNormal * diffuseSolarAltitudeFunction * transmissionFunction);
    }


    /*!
     * \brief Diffuse irradiance on an inclined surface (Muneer, 1990)               [W m-2]
     * \param beamIrradianceHor
     * \param diffuseIrradianceHor
     * \param mySunPosition a pointer to a TsunPosition
     * \param myPoint
     * \return result
     */
    float clearSkyDiffuseInclined(float beamIrradianceHor, float diffuseIrradianceHor, TsunPosition* mySunPosition, TradPoint* myPoint)
    {
        //Bh                     beam irradiance on a horizontal surface                                     [W m-2]
        //Dh                     diffuse irradiance on a horizontal surface

        double cosSlope, sinSlope;
        double slopeRad, aspectRad, elevRad, azimRad;
        double sinElev;
        double Kb;        /*!< amount of beam irradiance available [] */
        double Fg, r_sky, Fx, Aln;
        double n;
        sinElev = maxValue(getSinDecimalDegree(mySunPosition->elevation), 0.001);
        cosSlope = getCosDecimalDegree(float(myPoint->slope));
        sinSlope = getSinDecimalDegree(float(myPoint->slope));
        slopeRad = myPoint->slope * DEG_TO_RAD;
        aspectRad = myPoint->aspect * DEG_TO_RAD;
        elevRad = mySunPosition->elevation * DEG_TO_RAD;
        azimRad = mySunPosition->azimuth * DEG_TO_RAD;

        Kb = beamIrradianceHor / (mySunPosition->extraIrradianceNormal * sinElev);
        Fg = sinSlope - slopeRad * cosSlope - PI * getSinDecimalDegree(float(myPoint->slope / 2.0))
                                                 * getSinDecimalDegree(float(myPoint->slope / 2.0));
        r_sky = (1.0 + cosSlope) / 2.0;
        if ((((mySunPosition->shadow) || ((mySunPosition)->incidence * DEG_TO_RAD) <= 0.1)) && (elevRad >= 0.0))
        {
            (n = 0.252271) ;
            Fx = r_sky + Fg * n ;
        }
        else
        {
            n = 0.00263 - Kb * (0.712 + 0.6883 * Kb);
            //FT attenzione: crea discontinuita'
            if (elevRad >= 0.1) (Fx = (n * Fg + r_sky) * (1 - Kb) + Kb * getSinDecimalDegree(mySunPosition->incidence) / sinElev);
            //elevRad < 0.1
            else
            {
                Aln = azimRad - aspectRad;
                if (Aln > (2.0 * PI))
                    Aln -= (float)(2.0 * PI);
                else if (Aln < 0.0)
                    Aln += (float)(2.0 * PI);
                Fx = (n * Fg + r_sky) * (1.0 - Kb) + Kb * sinSlope * getCosDecimalDegree(float(Aln * RAD_TO_DEG)) / (0.1 - 0.008 * elevRad);
            }
        }
        return float(diffuseIrradianceHor * Fx);
    }


    float getReflectedIrradiance(float beamIrradianceHor, float diffuseIrradianceHor, float myAlbedo, float mySlope)
    {
        if (mySlope > 0)
            //Muneer 1997
            return (float)(myAlbedo * (beamIrradianceHor + diffuseIrradianceHor) * (1 - getCosDecimalDegree(mySlope)) / 2.);
        else
            return 0;
    }


    bool isIlluminated(float myTime, float riseTime, float setTime, float sunElevation)
    {
        bool output = false ;
        if ((riseTime != NODATA) && (setTime != NODATA) && (sunElevation != NODATA))
            output =  ((myTime >= riseTime) && (myTime <= setTime) && (sunElevation > 0));
        return output;
    }


    bool computeShadow(TradPoint* myPoint, TsunPosition* mySunPosition, const gis::Crit3DRasterGrid& myDtm)
    {
        float sunMaskStepX, sunMaskStepY;
        float sunMaskStepZ, maxDeltaH;
        float x, y, z;
        int row, col;
        bool shadowComputed;
        float cosElev, sinElev, tgElev;
        float step, stepCount, maxDistCount;

        /*INPUT
        azimuth
        elevationRefr
        supponiamo di avere gia' controllato se siamo dopo l'alba e prima del tramonto
        inizializzazione a sole visibile*/

        shadowComputed = false;
        x = float(myPoint->x);
        y = float(myPoint->y);
        z = float(myPoint->height);
        sunMaskStepX = float(radiationSettings.getShadowDistanceFactor() *
                             getSinDecimalDegree(mySunPosition->azimuth) * myDtm.header->cellSize);
        sunMaskStepY = float(radiationSettings.getShadowDistanceFactor() *
                             getCosDecimalDegree(mySunPosition->azimuth) * myDtm.header->cellSize);

        cosElev = getCosDecimalDegree(mySunPosition->elevation);
        sinElev = getSinDecimalDegree(mySunPosition->elevation);
        tgElev = sinElev / cosElev;
        sunMaskStepZ = float(myDtm.header->cellSize * radiationSettings.getShadowDistanceFactor() * tgElev);
        maxDeltaH = float(myDtm.header->cellSize * radiationSettings.getShadowDistanceFactor() * 2.0);

        if (sunMaskStepZ == 0)
            maxDistCount = myDtm.maximum - z;
        else
            maxDistCount = (myDtm.maximum - z) / sunMaskStepZ;
        stepCount = 0;
        step = 1;

        bool output= false;
        do
        {
            x += sunMaskStepX * step;
            y += sunMaskStepY * step;
            z += sunMaskStepZ * step;
            stepCount += step;

            if (stepCount > maxDistCount) shadowComputed = true ;

            gis::getRowColFromXY(myDtm, x, y, &row, &col);
            if (! gis::isOutOfGridRowCol(row, col, myDtm))
            {
                float matrixElement = myDtm.value[row][col];
                if (matrixElement != myDtm.header->flag)
                {
                    if (matrixElement > z)
                    {
                        //shadow - exit
                        shadowComputed = true ;
                        output = true ;
                    }
                    else
                    {
                        step = (z - matrixElement) / maxDeltaH;
                        if (step < 1) step = 1;
                    }
                }
                else shadowComputed = true;
            }
            else
                shadowComputed = true;

        } while(!shadowComputed);
        return output;
    }


    void separateTransmissivity(float myClearSkyTransmissivity, float transmissivity, float *td,float *Tt)
    {
        float maximumDiffuseTransmissivity;

        //in attesa di studi mirati (Bristow and Campbell, 1985)
        maximumDiffuseTransmissivity = float(0.6 / (myClearSkyTransmissivity - 0.4));
        *Tt = float(maxValue(minValue(transmissivity, myClearSkyTransmissivity), 0.00001));
        *td = (*Tt) * (1 - exp(maximumDiffuseTransmissivity - (maximumDiffuseTransmissivity * myClearSkyTransmissivity) / (*Tt)));

        /*! FT 0.12 stimato da Settefonti agosto 2007 */
        if ((*Tt) > 0.6) *td = maxValue((*td), float(0.12));
    }


bool computeRadiationPointRsun(float myTemperature, float myPressure, Crit3DTime myTime,
            float myLinke,float myAlbedo, float myClearSkyTransmissivity, float myTransmissivity ,
            TsunPosition* mySunPosition, TradPoint* myPoint, const gis::Crit3DRasterGrid& myDtm)
    {
        int myYear, myMonth, myDay;
        int myHour, myMinute, mySecond;
        float Bhc, Bh;
        float Dhc, dH;
        float Ghc, Gh;
        //float td, Tt;
        float normalizedTransmittance;
        float globalTransmittance;  /*!<   real sky global irradiation coefficient (global transmittance) */
        float diffuseTransmittance; /*!<   real sky mypoint.diffuse irradiation coefficient (mypoint.diffuse transmittance) */
        //float beamTransmittance;    /*! real sky mypoint.beam irradiation coefficient (mypoint.beam transmittance) */
        float dhsOverGhs;           /*!<  ratio horizontal mypoint.diffuse over horizontal global */
        bool isPointIlluminated;

        Crit3DTime localTime;
        localTime = myTime;
        if (radiationSettings.gisSettings->isUTC)
        {
            localTime = myTime.addSeconds(float(radiationSettings.gisSettings->timeZone * 3600));
        }

        myYear = localTime.date.year;
        myMonth =  localTime.date.month;
        myDay =  localTime.date.day;
        myHour = localTime.getHour();
        myMinute = localTime.getMinutes();
        mySecond = int(localTime.getSeconds());

        /*! Sun position */
        if (! computeSunPosition(float(myPoint->lon), float(myPoint->lat), radiationSettings.gisSettings->timeZone,
            myYear, myMonth, myDay, myHour, myMinute, mySecond,
            myTemperature, myPressure, float(myPoint->aspect), float(myPoint->slope), mySunPosition))
            return false;

        /*! Shadowing */
        isPointIlluminated = isIlluminated(localTime.time, (*mySunPosition).rise, (*mySunPosition).set, (*mySunPosition).elevationRefr);
        if (radiationSettings.getComputeShadowing())
        {
            if (gis::isOutOfGridXY(myPoint->x, myPoint->y, myDtm.header))
                (*mySunPosition).shadow = ! isPointIlluminated;
            else
            {
                if (isPointIlluminated)
                    (*mySunPosition).shadow = computeShadow(myPoint, mySunPosition, myDtm);
                else
                    (*mySunPosition).shadow = true;
            }
        }

        /*! Radiation */
        if (isPointIlluminated)
        {
            Bhc = clearSkyBeamHorizontal(myLinke, mySunPosition);
            Dhc = clearSkyDiffuseHorizontal(myLinke, mySunPosition);
            Ghc = Dhc + Bhc;
            if (radiationSettings.getComputeRealData())
            {
                if (myTransmissivity != NODATA)
                {
                    if (radiationSettings.getTransmissivityUseTotal())
                    {
                        Gh = mySunPosition->extraIrradianceHorizontal * myTransmissivity;
                        separateTransmissivity (myClearSkyTransmissivity, myTransmissivity, &diffuseTransmittance, &globalTransmittance);
                        dH = mySunPosition->extraIrradianceHorizontal * diffuseTransmittance;
                    }
                    else
                    {
                        normalizedTransmittance = myTransmissivity / myClearSkyTransmissivity;
                        Gh = Ghc * normalizedTransmittance;
                        separateTransmissivity (myClearSkyTransmissivity, myTransmissivity, &diffuseTransmittance, &globalTransmittance);
                        dhsOverGhs = diffuseTransmittance / globalTransmittance;
                        dH = dhsOverGhs * Gh;
                    }
                }
                else
                {
                    Gh = Ghc;
                    dH = Dhc;
                }
            }
            else
            {
                Gh = Ghc;
                dH = Dhc;
            }

            if ((!(*mySunPosition).shadow) && ((*mySunPosition).incidence > 0.))
                Bh = Gh - dH;
            else
            {
                Bh = 0;
                Gh = dH;
            }
            if (radiationSettings.getTiltMode() == TILT_TYPE_HORIZONTAL)
            {
                (*myPoint).beam = Bh;
                (*myPoint).diffuse = dH;
                (*myPoint).reflected = 0;
                (*myPoint).global = Gh;
            }
            else
            {
                if ((!(*mySunPosition).shadow) && ((*mySunPosition).incidence > 0.))
                    myPoint->beam = clearSkyBeamInclined(Bh, mySunPosition);
                else
                    myPoint->beam = 0;

                myPoint->diffuse = clearSkyDiffuseInclined(Bh, dH, mySunPosition, myPoint);
                myPoint->reflected = getReflectedIrradiance(Bh, dH, myAlbedo, float(myPoint->slope));
                myPoint->global = myPoint->beam + myPoint->diffuse + myPoint->reflected;
            }
        }
        else
        {
            myPoint->beam = 0;
            myPoint->diffuse = 0;
            myPoint->reflected = 0;
            myPoint->global = 0;
        }

        return true;
    }


    int estimateTransmissivityWindow(const gis::Crit3DRasterGrid& myDtm,
                                     const Crit3DRadiationMaps& myRadiationMaps,
                                     gis::Crit3DPoint* myPoint, Crit3DTime myTime, int timeStepSecond)
    {
        double latDegrees, lonDegrees;
        TradPoint myRadPoint;
        TsunPosition mySunPosition;
        float myLinke, myAlbedo;
        float  myClearSkyTransmissivity;
        float sumPotentialRadThreshold = 0.;
        float sumPotentialRad = 0.;
        Crit3DTime backwardTime;
        Crit3DTime forwardTime;
        int myWindowSteps;
        int myRow, myCol;

        /*! assegna altezza e coordinate stazione */
        myRadPoint.x = myPoint->utm.x;
        myRadPoint.y = myPoint->utm.y;
        myRadPoint.height = myPoint->z;

        if (myPoint->z == NODATA) myRadPoint.height = gis::getValueFromXY(myDtm, myRadPoint.x, myRadPoint.y);

        gis::getRowColFromXY(myDtm, myRadPoint.x, myRadPoint.y, &myRow, &myCol);
        myRadPoint.aspect = readAspect(myRadiationMaps.aspectMap, myRow, myCol);
        myRadPoint.slope = readSlope(myRadiationMaps.slopeMap, myRow, myCol);

        gis::getLatLonFromUtm(*(radiationSettings.gisSettings), myPoint->utm.x, myPoint->utm.y, &latDegrees, &lonDegrees);
        myRadPoint.lat = (float)(latDegrees);
        myRadPoint.lon = (float)(lonDegrees);

        myLinke = readLinke();
        myAlbedo = readAlbedo();
        myClearSkyTransmissivity = radiationSettings.getClearSky();

        // noon
        Crit3DTime noonTime = myTime;
        noonTime.time = 12*3600;
        if (radiationSettings.gisSettings->isUTC)
        {
            noonTime = noonTime.addSeconds(-float(radiationSettings.gisSettings->timeZone * 3600));
        }

        // Threshold: half potential radiation at noon
        computeRadiationPointRsun(TEMPERATURE_DEFAULT, PRESSURE_DEFAULT, noonTime, myLinke, myAlbedo, myClearSkyTransmissivity, myClearSkyTransmissivity, &mySunPosition, &myRadPoint, myDtm);
        sumPotentialRadThreshold = float(myRadPoint.global * 0.5);

        computeRadiationPointRsun(TEMPERATURE_DEFAULT, PRESSURE_DEFAULT, myTime, myLinke, myAlbedo, myClearSkyTransmissivity, myClearSkyTransmissivity, &mySunPosition, &myRadPoint, myDtm);
        sumPotentialRad = float(myRadPoint.global);

        int backwardTimeStep,forwardTimeStep;
        backwardTimeStep = forwardTimeStep = 0;
        myWindowSteps = 1;
        backwardTime = forwardTime = myTime;

        while (sumPotentialRad < sumPotentialRadThreshold)
        {
            myWindowSteps += 2;

            backwardTimeStep -= timeStepSecond ;
            forwardTimeStep += timeStepSecond;
            backwardTime = myTime.addSeconds(float(backwardTimeStep));
            forwardTime = myTime.addSeconds(float(forwardTimeStep));

            computeRadiationPointRsun(TEMPERATURE_DEFAULT, PRESSURE_DEFAULT, backwardTime, myLinke, myAlbedo, myClearSkyTransmissivity, myClearSkyTransmissivity, &mySunPosition, &myRadPoint, myDtm);
            sumPotentialRad+= float(myRadPoint.global);

            computeRadiationPointRsun(TEMPERATURE_DEFAULT, PRESSURE_DEFAULT, forwardTime, myLinke, myAlbedo, myClearSkyTransmissivity, myClearSkyTransmissivity, &mySunPosition, &myRadPoint, myDtm);
            sumPotentialRad+= float(myRadPoint.global);
        }

        return myWindowSteps;
    }


    bool isGridPointComputable(int row, int col, const gis::Crit3DRasterGrid& myDtm, Crit3DRadiationMaps* radiationMaps)
    {
        float mySlope, myAspect;
        bool output = false;
        if (myDtm.value[row][col] != myDtm.header->flag)
        {
            if ((radiationMaps->latMap->value[row][col] != radiationMaps->latMap->header->flag)
                    && (radiationMaps->lonMap->value[row][col] != radiationMaps->lonMap->header->flag))
            {
                mySlope = readSlope(radiationMaps->slopeMap, row, col);
                myAspect = readAspect(radiationMaps->aspectMap, row, col);
                if ((mySlope != NODATA) && (myAspect != NODATA))  output = true;
            }
        }
        return output;
    }


   bool computeRadiationGridRsun(const gis::Crit3DRasterGrid& myDtm,
                                 Crit3DRadiationMaps* radiationMaps,
                                 const Crit3DTime& UTCTime)

    {
        int myRow, myCol;
        TsunPosition mySunPosition;
        TradPoint myRadPoint;

        for (myRow=0;myRow< myDtm.header->nrRows ; myRow++ )
        {
            for (myCol=0;myCol < myDtm.header->nrCols; myCol++)
            {
                if(isGridPointComputable(myRow, myCol, myDtm, radiationMaps))
                {
                    gis::getUtmXYFromRowCol(myDtm, myRow, myCol, &(myRadPoint.x), &(myRadPoint.y));
                    myRadPoint.height = myDtm.value[myRow][myCol];
                    myRadPoint.lat = radiationMaps->latMap->value[myRow][myCol];
                    myRadPoint.lon = radiationMaps->lonMap->value[myRow][myCol];
                    myRadPoint.slope = readSlope(radiationMaps->slopeMap, myRow, myCol);
                    myRadPoint.aspect = readAspect(radiationMaps->aspectMap, myRow, myCol);
                    //CHIAMATA A SINGLE POINT
                    if (!computeRadiationPointRsun(TEMPERATURE_DEFAULT, PRESSURE_DEFAULT, UTCTime,
                                    readLinke(myRow, myCol, *(radiationMaps->linkeMap)), readAlbedo(myRow, myCol, *(radiationMaps->albedoMap)), radiationSettings.getClearSky(), radiationMaps->transmissivityMap->value[myRow][myCol], &mySunPosition, &myRadPoint, myDtm))
                        return false;

                    radiationMaps->sunAzimuthMap->value[myRow][myCol] = mySunPosition.azimuth;
                    radiationMaps->sunElevationMap->value[myRow][myCol] = mySunPosition.elevationRefr;
                    radiationMaps->sunIncidenceMap->value[myRow][myCol] = mySunPosition.incidence;

                    radiationMaps->sunShadowMap->value[myRow][myCol] = float((mySunPosition.shadow) ?  0 : 1);

                    radiationMaps->beamRadiationMap->value[myRow][myCol] = float(myRadPoint.beam);
                    radiationMaps->diffuseRadiationMap->value[myRow][myCol] = float(myRadPoint.diffuse);
                    radiationMaps->reflectedRadiationMap->value[myRow][myCol] = float(myRadPoint.reflected);
                    radiationMaps->globalRadiationMap->value[myRow][myCol] = float(myRadPoint.global);
                }
            }
        }

        gis::updateMinMaxRasterGrid(radiationMaps->sunAzimuthMap);
        gis::updateMinMaxRasterGrid(radiationMaps->sunElevationMap);
        gis::updateMinMaxRasterGrid(radiationMaps->sunIncidenceMap);
        gis::updateMinMaxRasterGrid(radiationMaps->sunRiseMap);
        gis::updateMinMaxRasterGrid(radiationMaps->sunSetMap);
        gis::updateMinMaxRasterGrid(radiationMaps->sunShadowMap);
        gis::updateMinMaxRasterGrid(radiationMaps->transmissivityMap);
        gis::updateMinMaxRasterGrid(radiationMaps->diffuseRadiationMap);
        gis::updateMinMaxRasterGrid(radiationMaps->reflectedRadiationMap);
        gis::updateMinMaxRasterGrid(radiationMaps->beamRadiationMap);
        gis::updateMinMaxRasterGrid(radiationMaps->globalRadiationMap);

        return true;
    }


    bool computeSunPosition(float lon, float lat, int myTimezone,
                            int myYear,int myMonth, int myDay,
                            int myHour, int myMinute, int mySecond,
                            float temp, float pressure, float aspect, float slope, TsunPosition *mySunPosition)
    {
        float etrTilt; /*!<  Extraterrestrial (top-of-atmosphere) global irradiance on a tilted surface (W m-2) */
        float cosZen; /*!<  Cosine of refraction corrected solar zenith angle */
        float sbcf; /*!<  Shadow-band correction factor */
        float prime; /*!<  Factor that normalizes Kt, Kn, etc. */
        float unPrime; /*!<  Factor that denormalizes Kt', Kn', etc. */
        float  zenRef; /*!<  Solar zenith angle, deg. from zenith, refracted */
        int chk;
        float sunCosIncidenceCompl;/*!<  cosine of (90 - incidence) */
        float sunRiseMinutes; /*!<  sunrise time [minutes from midnight] */
        float sunSetMinutes; /*!<  sunset time [minutes from midnight] */

        chk = RSUN_compute_solar_position(lon, lat, myTimezone, myYear, myMonth, myDay, myHour, myMinute, mySecond, temp, pressure, aspect, slope, float(SBWID), float(SBRAD), float(SBSKY));
        if (chk > 0)
        {
           //setErrorMsg InfoMsg.Err_SunPositionDLL(chk) + vbCrLf & "for point Lat: " & CStr(Lat) & "Lon: " & CStr(Lon)
            return false;
        }

         RSUN_get_results(&((*mySunPosition).relOptAirMass), &((*mySunPosition).relOptAirMassCorr), &((*mySunPosition).azimuth), &sunCosIncidenceCompl, &cosZen, &((*mySunPosition).elevation), &((*mySunPosition).elevationRefr), &((*mySunPosition).extraIrradianceHorizontal), &((*mySunPosition).extraIrradianceNormal), &etrTilt, &prime, &sbcf, &sunRiseMinutes, &sunSetMinutes, &unPrime, &zenRef);

        (*mySunPosition).incidence = float(maxValue(0, RAD_TO_DEG * ((PI / 2.0) - acos(sunCosIncidenceCompl))));
        (*mySunPosition).rise = float(sunRiseMinutes * 60.0);
        (*mySunPosition).set = float(sunSetMinutes * 60.0);
        return true;
    }

    bool computeRadiationPointBrooks(TradPoint* myPoint, Crit3DDate* myDate, float currentSolarTime,
        float myClearSkyTransmissivity, float myTransmissivity)
    {
        int myDoy;
        double timeAdjustment;   /*!<  hours */
        double timeEq;           /*!<  hours */
        double solarTime;        /*!<  hours */
        double correctionLong;   /*!<  hours */

        double solarDeclination; /*!<  radians */
        double elevationAngle;   /*!<  radians */
        double incidenceAngle;   /*!<  radians */
        double azimuthSouth;     /*!<  radians */
        double azimuthNorth;     /*!<  radians */

        double coeffBH; /*!<  [-] */

        double extraTerrestrialRad; /*!<  [W/m2] */
        double radDiffuse; /*!<  [W/m2] */
        double radBeam; /*!<  [W/m2] */
        double radReflected; /*!<  [W/m2] */
        double radTotal; /*!<  [W/m2] */

        myDoy = getDoyFromDate(*myDate);

        /*! conversione in radianti per il calcolo */
        myPoint->aspect *= (float)DEG_TO_RAD;
        myPoint->lat *= (float)DEG_TO_RAD;

        timeAdjustment = (279.575 + 0.986 * myDoy) * DEG_TO_RAD;

        timeEq = (-104.7 * sin(timeAdjustment) + 596.2 * sin(2 * timeAdjustment)
                  + 4.3 * sin(3 * timeAdjustment) - 12.7 * sin(4 * timeAdjustment)
                  - 429.3 * cos(timeAdjustment) - 2 * cos(2 * timeAdjustment)
                  + 19.3 * cos(3 * timeAdjustment)) / 3600.0 ;

        solarDeclination = 0.4102 * sin(2.0 * PI / 365.0 * (myDoy - 80));

        //controllare i segni:
        correctionLong = ((radiationSettings.gisSettings->timeZone * 15) - myPoint->lon) / 15.0;

        solarTime = currentSolarTime - correctionLong + timeEq;
        if (solarTime > 24)
        {
            solarTime -= 24;
        }
        else if (solarTime < 0)
        {
            solarTime += 24;
        }

        elevationAngle = asin(sin(myPoint->lat) * sin(solarDeclination)
                                 + cos(myPoint->lat) * cos(solarDeclination)
                                 * cos((PI / 12) * (solarTime - 12)));

        extraTerrestrialRad = maxValue(0, SOLAR_CONSTANT * sin(elevationAngle));

        azimuthSouth = acos((sin(elevationAngle)
                                * sin(myPoint->lat) - sin(solarDeclination))
                               / (cos(elevationAngle) * cos(myPoint->lat)));

        azimuthNorth = (solarTime>12) ? PI + azimuthSouth : PI - azimuthSouth;
        incidenceAngle = maxValue(0, asin(getSinDecimalDegree(float(myPoint->slope)) *
                                        cos(elevationAngle) * cos(azimuthNorth - float(myPoint->aspect))
                                        + getCosDecimalDegree(float(myPoint->slope)) * sin(elevationAngle)));

        float Tt = myClearSkyTransmissivity;
        float td = float(0.1);
        if (radiationSettings.getComputeRealData())
        {
            if (myTransmissivity != NODATA)
            {
                separateTransmissivity (myClearSkyTransmissivity, myTransmissivity, &td, &Tt);
            }
        }

        coeffBH = maxValue(0, (Tt - td));

        radDiffuse = extraTerrestrialRad * td;

        if (radiationSettings.getTiltMode() == TILT_TYPE_HORIZONTAL)
        {
            radBeam = extraTerrestrialRad * coeffBH;
            radReflected = 0;
        }
        else
        {
            radBeam = extraTerrestrialRad * coeffBH * maxValue(0, sin(incidenceAngle) / sin(elevationAngle));
            //aggiungere Snow albedo!
            //Muneer 1997
            radReflected = extraTerrestrialRad * Tt * 0.2 * (1 - getCosDecimalDegree(float(myPoint->slope))) / 2.0;
        }
        radTotal = radDiffuse + radBeam + radReflected;
        myPoint->global = float(radTotal);
        myPoint->beam = float(radBeam);
        myPoint->diffuse = float(radDiffuse);
        myPoint->reflected = float(radReflected);

        return true;
    }


    void getTime(double decimalTime, int* myHour, int* myMinute, int* mySecond)
    {
       int mySign = 1;
       if (decimalTime < 0.0)
       {
          mySign = -1;
          decimalTime *= -1;
       }

       *myHour = mySign * (int)floor(decimalTime);
       *myMinute = mySign * (int)floor((decimalTime - (*myHour)) * 60);
       *mySecond =mySign * (int)floor((decimalTime - (*myHour) - (*myMinute) / 60) * 3600);
    }

    bool preConditionsRadiationGrid(Crit3DRadiationMaps* radiationMaps)
    {
        if (! radiationMaps->latMap->isLoaded || ! radiationMaps->lonMap->isLoaded) return false;

        if (! radiationMaps->slopeMap->isLoaded || ! radiationMaps->aspectMap->isLoaded) return false;

        return true;
    }


    bool computeRadiationGridPresentTime(const gis::Crit3DRasterGrid& myDtm,
                                         Crit3DRadiationMaps* radiationMaps,
                                         const Crit3DTime& myCrit3DTime)
    {
        if (! preConditionsRadiationGrid(radiationMaps))
            return false;        

        if (radiationSettings.getAlgorithm() == RADIATION_ALGORITHM_RSUN)
        {
            return computeRadiationGridRsun(myDtm, radiationMaps, myCrit3DTime);
        }
        else if (radiationSettings.getAlgorithm() == RADIATION_ALGORITHM_BROOKS)
        {
            // to do
            return false;
        }
        else
            return false;
    }


    float computePointTransmissivity(const gis::Crit3DPoint& myPoint, Crit3DTime UTCTime, float* measuredRad,
                                     int windowWidth, int timeStepSecond, const gis::Crit3DRasterGrid& myDtm)
    {
        if (windowWidth % 2 != 1) return NODATA;

        int intervalCenter = (windowWidth-1)/2;

        if (measuredRad[intervalCenter] == NODATA) return NODATA;

        double latDegrees, lonDegrees;
        float ratioTransmissivity;
        TradPoint myRadPoint;
        TsunPosition mySunPosition;
        float myLinke, myAlbedo;
        float  myClearSkyTransmissivity;
        float myTransmissivity;

        Crit3DTime backwardTime;
        Crit3DTime forwardTime;

        /*! assign topographic height and coordinates */
        myRadPoint.x = myPoint.utm.x;
        myRadPoint.y = myPoint.utm.y;
        myRadPoint.height = myPoint.z;
        if (myPoint.z == NODATA && myPoint.utm.isInsideGrid(*(myDtm.header)))
            myRadPoint.height = gis::getValueFromXY(myDtm, myRadPoint.x, myRadPoint.y);

        /*! suppose radiometers are horizontal */
        myRadPoint.aspect = 0.;
        myRadPoint.slope = 0.;

        gis::getLatLonFromUtm(*(radiationSettings.gisSettings), myPoint.utm.x, myPoint.utm.y, &latDegrees, &lonDegrees);
        myRadPoint.lat = (float)(latDegrees);
        myRadPoint.lon = (float)(lonDegrees);

        myLinke = readLinke();
        myAlbedo = readAlbedo();
        myClearSkyTransmissivity = radiationSettings.getClearSky();

        int backwardTimeStep,forwardTimeStep;
        backwardTimeStep = forwardTimeStep = 0;
        backwardTime = forwardTime = UTCTime;

        float sumMeasuredRad = measuredRad[intervalCenter];

        computeRadiationPointRsun(TEMPERATURE_DEFAULT, PRESSURE_DEFAULT, UTCTime, myLinke, myAlbedo,
               myClearSkyTransmissivity, myClearSkyTransmissivity, &mySunPosition, &myRadPoint, myDtm);

        float sumPotentialRad = float(myRadPoint.global);

        for (int windowIndex = (intervalCenter - 1); windowIndex >= 0; windowIndex--)
        {
            backwardTimeStep -= timeStepSecond;
            forwardTimeStep += timeStepSecond;
            backwardTime = UTCTime.addSeconds(float(backwardTimeStep));
            forwardTime = UTCTime.addSeconds(float(forwardTimeStep));

            if (measuredRad[windowIndex] != NODATA)
            {
                sumMeasuredRad += measuredRad[windowIndex];
                computeRadiationPointRsun(TEMPERATURE_DEFAULT, PRESSURE_DEFAULT, backwardTime, myLinke, myAlbedo, myClearSkyTransmissivity, myClearSkyTransmissivity, &mySunPosition, &myRadPoint, myDtm);
                sumPotentialRad += float(myRadPoint.global);
            }
            if (measuredRad[windowWidth-windowIndex-1] != NODATA)
            {
                sumMeasuredRad+= measuredRad[windowWidth-windowIndex-1];
                computeRadiationPointRsun(TEMPERATURE_DEFAULT, PRESSURE_DEFAULT, forwardTime, myLinke, myAlbedo, myClearSkyTransmissivity, myClearSkyTransmissivity, &mySunPosition, &myRadPoint, myDtm);
                sumPotentialRad+= float(myRadPoint.global);
            }
        }

        ratioTransmissivity = maxValue(sumMeasuredRad/sumPotentialRad, float(0.0));
        myTransmissivity = ratioTransmissivity * myClearSkyTransmissivity;

        /*! transmissivity can't be over 0.85 */
        return minValue(myTransmissivity, float(0.85));
    }
}


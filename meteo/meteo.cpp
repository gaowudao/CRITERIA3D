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

#include <math.h>

#include "commonConstants.h"
#include "physics.h"
#include "meteo.h"


Crit3DClimateParameters::Crit3DClimateParameters()
{
    for (int i=0; i<12; i++)
    {
        tminLapseRate[i] = NODATA;
        tmaxLapseRate[i] = NODATA;
        tDewMinLapseRate[i] = NODATA;
        tDewMaxLapseRate[i] = NODATA;
    }
}


float computeTminHourlyWeight(int myHour)
{
    if (myHour >= 6 && myHour <= 14)
        return (1 - ((float)myHour - 6) / 8);
    else if (myHour > 14)
        return (1 - min_value((24 - (float)myHour) + 6, 12) / 12);
    else
        return (1 - (6 - (float)myHour) / 12);
}


float Crit3DClimateParameters::getClimateLapseRate(meteoVariable myVar, Crit3DDate* myDate, int myHour)
{
    int indexMonth = myDate->month - 1;

    if (myVar == dailyAirTemperatureMin)
        return tminLapseRate[indexMonth];
    else if (myVar == dailyAirTemperatureMax)
        return tmaxLapseRate[indexMonth];
    else if (myVar == dailyAirTemperatureAvg)
        return (tmaxLapseRate[indexMonth] + tminLapseRate[indexMonth]) / 2;
    else
    {
        float lapseTmin, lapseTmax;
        if (myVar == airTemperature)
        {
            lapseTmin = tminLapseRate[indexMonth];
            lapseTmax = tmaxLapseRate[indexMonth];
        }
        else if (myVar == airDewTemperature)
        {
            lapseTmin = tDewMinLapseRate[indexMonth];
            lapseTmax = tDewMaxLapseRate[indexMonth];
        }
        else
            return NODATA;

        float tminWeight = computeTminHourlyWeight(myHour);
        return (lapseTmin * tminWeight + lapseTmax * (1 - tminWeight));
    }
}


float tDewFromRelHum(float rhAir, float airT)
{
    if (rhAir == NODATA || airT == NODATA)
        return NODATA;

    rhAir = minValue(100, rhAir);

    double mySaturatedVaporPres = exp((16.78 * airT - 116.9) / (airT + 237.3));
    double actualVaporPres = rhAir / 100. * mySaturatedVaporPres;
    return float((log(actualVaporPres) * 237.3 + 116.9) / (16.78 - log(actualVaporPres)));
}

float relHumFromTdew(float DewT, float airT)
{
    if (DewT == NODATA || airT == NODATA)
        return NODATA;
    else
    {
        double d = 237.3;
        double c = 17.2693882;
        double esp = 1 / (airT + d);
        double myValue = pow((exp((c * DewT) - ((c * airT / (airT + d))) * (DewT + d))), esp);
        myValue *= 100.;
        if (myValue > 100.)
            return 100;
        else if (myValue == 0.)
            return 1.;
        else
            return float(myValue);
    }
}

double dailyExtrRadiation(double myLat, int myDoy)
{
    /*!
    2011 GA
    da quaderno FAO
    MJ m-2 d-1
    */

    double OmegaS;                               /*!< [rad] sunset hour angle */
    double Phi;                                  /*!< [rad] latitude in radiants */
    double delta;                                /*!< [rad] solar declination */
    double dr;                                   /*!< [-] inverse Earth-Sun relative distance */

    Phi = PI / 180 * myLat;
    delta = 0.4093 * sin((2 * PI / 365) * myDoy - 1.39);
    dr = 1 + 0.033 * cos(2 * PI * myDoy / 365);
    OmegaS = acos(-tan(Phi) * tan(delta));

    return SOLAR_CONSTANT * DAY_SECONDS / 1000000 * dr / PI * (OmegaS * sin(Phi) * sin(delta) + cos(Phi) * cos(delta) * sin(OmegaS));
}


/*!
 * \brief [] net surface emissivity
 * \param myVP
 * \return result
 */
double emissivityFromVaporPressure(double myVP)
{
    return 0.34 - 0.14 * sqrt(myVP);
}


/*!
 * \brief 2016 GA. comments: G is ignored for now (if heat is active, should be added)
 * \param myDOY [] day of year
 * \param myElevation
 * \param myLatitude [°] latitude in decimal degrees
 * \param myTmin [°C] daily minimum temperature
 * \param myTmax [°C] daily maximum temperature
 * \param myTminDayAfter
 * \param myUmed [%] daily average relative humidity
 * \param myVmed10 [m s-1] daily average wind intensity
 * \param mySWGlobRad [MJ m-2 d-1] daily global short wave radiation
 * \return result
 */
double ET0_Penman_daily(int myDOY, float myElevation, float myLatitude,
                        float myTmin, float myTmax, float myTminDayAfter,
                        float myUmed, float myVmed10, float mySWGlobRad)
{
        double MAXTRANSMISSIVITY = 0.75;

        double myPressure;                   /*!<  [kPa] atmospheric pressure */
        double myDailySB;                    /*!<  [MJ m-2 d-1 K-4] Stefan Boltzmann constant */
        double myPsychro;                    /*!<  [kPa °C-1] psychrometric instrument constant */
        double myTmed;                       /*!<  [°C] daily average temperature */
        double myTransmissivity;             /*!<  [] global atmospheric trasmissivity */
        double myVapPress;                   /*!<  [kPa] actual vapor pressure */
        double mySatVapPress;                /*!<  [kPa] actual average vapor pressure */
        double myExtraRad;                   /*!<  [MJ m-2 d-1] extraterrestrial radiation */
        double mySWNetRad;                   /*!<  [MJ m-2 d-1] net short wave radiation */
        double myLWNetRad;                   /*!<  [MJ m-2 d-1] net long wave emitted radiation */
        double myNetRad;                     /*!<  [MJ m-2 d-1] net surface radiation */
        double delta;                        /*!<  [kPa °C-1] slope of vapour pressure curve */
        double vmed2;                        /*!<  [m s-1] average wind speed estimated at 2 meters */
        double EvapDemand;                   /*!<  [mm d-1] evaporative demand of atmosphere */
        double myEmissivity;                 /*!<  [] surface emissivity */
        double myLambda;                     /*!<  [MJ kg-1] latent heat of vaporization */


        if (myTmin == NODATA || myTmax == NODATA || myVmed10 == NODATA || myUmed == NODATA || myUmed < 0 || myUmed > 100 || myTminDayAfter == NODATA)
            return NODATA;

        myTmed = 0.5 * (myTmin + myTmax);

        myExtraRad = dailyExtrRadiation(myLatitude, myDOY);
        if (myExtraRad > 0)
            myTransmissivity = minValue(MAXTRANSMISSIVITY, mySWGlobRad / myExtraRad);
        else
            myTransmissivity = 0;

        myPressure = 101.3 * pow(((293 - 0.0065 * myElevation) / 293), 5.26);

        myPsychro = Psychro(myPressure, myTmed);

        /*!
        \brief
        differs from the one presented in the FAO Irrigation and Drainage Paper N° 56.
        Analysis with several climatic data sets proved that more accurate estimates of ea can be
        obtained using es(Tmed) than with the equation reported in the FAO paper if only mean
        relative humidity is available (G. Van Halsema and G. Muñoz, Personal communication).
        */

        /*! Monteith and Unsworth (2008) */
        mySatVapPress = 0.61078 * exp(17.27 * myTmed / (myTmed + 237.3));
        myVapPress = mySatVapPress * myUmed / 100;
        delta = SaturationSlope(myTmed, mySatVapPress) / 1000;    /*!<  to kPa */

        myDailySB = STEFAN_BOLTZMANN * DAY_SECONDS / 1000000;       /*!<   to MJ */
        myEmissivity = emissivityFromVaporPressure(myVapPress);
        myLWNetRad = myDailySB * (pow(myTmax + 273, 4) + pow(myTmin + 273, 4) / 2) * myEmissivity * (1.35 * (myTransmissivity / MAXTRANSMISSIVITY) - 0.35);

        mySWNetRad = mySWGlobRad * (1 - ALBEDO_CROP_REFERENCE);
        myNetRad = (mySWNetRad - myLWNetRad);

        myLambda = LatentHeatVaporization(myTmed) / 1000000; /*!<  to MJ */

        vmed2 = myVmed10 * 0.748;

        EvapDemand = 900 / (myTmed + 273) * vmed2 * (mySatVapPress - myVapPress);

        return (delta * myNetRad + myPsychro * EvapDemand / myLambda) / (delta + myPsychro * (1 + 0.34 * vmed2));

}



/*!
 * \brief ET0_Penman_hourly http://www.cimis.water.ca.gov/cimis/infoEtoPmEquation.jsp
 * \param heigth elevation above mean sea level (meters)
 * \param normalizedTransmissivity normalized tramissivity [0-1] ()
 * \param globalSWRadiation net Short Wave radiation (W m-2)
 * \param airTemp air temperature (C)
 * \param airHum relative humidity (%)
 * \param windSpeed10 wind speed at 2 meters (m s-1)
 * \return result
 */
double ET0_Penman_hourly(double heigth, double normalizedTransmissivity, double globalSWRadiation,
                double airTemp, double airHum, double windSpeed10)
{
    double mySigma;                              /*!<  Steffan-Boltzman constant J m-2 h-1 K-4 */
    double es;                                   /*!<  saturation vapor pressure (kPa) at the mean hourly air temperature in C */
    double ea;                                   /*!<  actual vapor pressure (kPa) at the mean hourly air temperature in C */
    double emissivity;                           /*!<  net emissivity of the surface */
    double netRadiation;                         /*!<  net radiation (J m-2 h-1) */
    double netLWRadiation;                       /*!<  net longwave radiation (J m-2 h-1) */
    double g;                                    /*!<  soil heat flux density (J m-2 h-1) */
    double Cd;                                   /*!<  bulk surface resistance and aerodynamic resistance coefficient */
    double tAirK;                                /*!<  air temperature (Kelvin) */
    double windSpeed2;                           /*!<  wind speed at 2 meters (m s-1) */
    double delta;                                /*!<  slope of saturation vapor pressure curve (kPa C-1) at mean air temperature */
    double pressure;                             /*!<  barometric pressure (kPa) */
    double lambda;                               /*!<  latent heat of vaporization in (J kg-1) */
    double gamma;                                /*!<  psychrometric constant (kPa C-1) */
    double firstTerm, secondTerm, denominator;


    es = SaturationVaporPressure(airTemp) / 1000.;
    ea = airHum * es / 100.0;
    emissivity = emissivityFromVaporPressure(ea);
    tAirK = airTemp + ZEROCELSIUS;
    mySigma = STEFAN_BOLTZMANN * HOUR_SECONDS;
    netLWRadiation = minValue(normalizedTransmissivity, 1) * emissivity * mySigma * (pow(tAirK, 4));

    /*!   from [W m-2] to [J h-1 m-2] */
    netRadiation = ALBEDO_CROP_REFERENCE * (3600 * globalSWRadiation) - netLWRadiation;

    /*!   values for grass */
    if (netRadiation > 0)
    {   g = 0.1 * netRadiation;
        Cd = 0.24;
    }
    else
    {
        g = 0.5 * netRadiation;
        Cd = 0.96;
    }

    delta = SaturationSlope(airTemp, es) / 1000;    /*!<  to kPa */

    pressure = PressureFromAltitude(heigth)/1000.;

    gamma = Psychro(pressure, airTemp);
    lambda = LatentHeatVaporization(airTemp);

    windSpeed2 = windSpeed10 * 0.748;

    denominator = delta + gamma * (1 + Cd * windSpeed2);
    firstTerm = delta * (netRadiation - g) / (lambda * denominator);
    secondTerm = (gamma * (37 / tAirK) * windSpeed2 * (es - ea)) / denominator;

    return maxValue(firstTerm + secondTerm, 0);
}


/*!
 * \brief computes [mm d-1] potential evapotranspiration. Trange minimum: 0.25°C  equivalent to 8.5% transimissivity
 * \param KT [-] Samani empirical coefficient
 * \param myLat [degrees] Latitude
 * \param myDoy [-] Day number (Jan 1st = 1)
 * \param tmax [°C] daily maximum air temperature
 * \param tmin [°C] daily minimum air temperature
 * \return result
 */
double ET0_Hargreaves(double KT, double myLat, int myDoy, double tmax, double tmin)
{
    double tavg, deltaT, extraTerrRadiation;

    if (tmax == NODATA || tmin == NODATA || KT == NODATA || myLat == NODATA || myDoy == NODATA)
        return NODATA;

    extraTerrRadiation = dailyExtrRadiation(myLat, myDoy);
    deltaT = maxValue(fabs(tmax - tmin), 0.25);

    tavg = (tmax + tmin) * 0.5;

    return 0.0135 * (tavg + 17.78) * KT * (extraTerrRadiation / 2.456) * sqrt(deltaT);
}


meteoVariable getDefaultMeteoVariable(int id)
{
    if (id == 101) return(airTemperature);
    else if (id == 102) return(precipitation);
    else if (id == 103) return(airHumidity);
    else if (id == 104) return(globalIrradiance);
    else if (id == 105) return(windIntensity);
    else if (id == 106) return(windDirection);

    else if (id == 151) return(dailyAirTemperatureMin);
    else if (id == 152) return(dailyAirTemperatureMax);
    else if (id == 153) return(dailyAirTemperatureAvg);
    else if (id == 154) return(dailyPrecipitation);
    else if (id == 155) return(dailyAirHumidityMin);
    else if (id == 156) return(dailyAirHumidityMax);
    else if (id == 157) return(dailyAirHumidityAvg);
    else if (id == 158) return(dailyGlobalRadiation);
    else if (id == 159) return(dailyWindIntensityAvg);
    else if (id == 160) return(dailyWindDirectionPrevailing);
	else if (id == 161) return(dailyWindIntensityMax);

    else
        return(noMeteoVar);
}


bool setColorScale(meteoVariable variable, Crit3DColorScale *colorScale)
{
    if (colorScale == NULL) return false;

    switch(variable)
    {
        case airTemperature: case dailyAirTemperatureAvg: case dailyAirTemperatureMax: case dailyAirTemperatureMin:
            setTemperatureScale(colorScale);
            break;
        case airHumidity: case dailyAirHumidityAvg: case dailyAirHumidityMax: case dailyAirHumidityMin: case leafWetness: case dailyLeafWetness:
            setRelativeHumidityScale(colorScale);
            break;
        case precipitation: case dailyPrecipitation: case referenceEvapotranspiration: case dailyReferenceEvapotranspiration:
            setPrecipitationScale(colorScale);
            break;
        case globalIrradiance: case dailyGlobalRadiation:
            setRadiationScale(colorScale);
            break;
        case windIntensity: case dailyWindIntensityAvg: case dailyWindIntensityMax:
            setWindIntensityScale(colorScale);
            break;
        case noMeteoTerrain:
            setDefaultDTMScale(colorScale);
        default:
            setDefaultDTMScale(colorScale);
    }

    return true;
}


std::string getVariableString(meteoVariable myVar)
{
    if (myVar == airTemperature || myVar == dailyAirTemperatureAvg)
        return "Air temperature °C";
    else if (myVar == airHumidity || myVar == dailyAirHumidityAvg)
        return "Relative humidity %";
    else if ((myVar == dailyPrecipitation ||  myVar == precipitation))
        return "Precipitation mm";
    else if (myVar == dailyAirTemperatureMax)
        return "Max. air temperature °C";
    else if (myVar == dailyAirTemperatureMin)
        return "Min. air temperature °C";
    else if (myVar == dailyGlobalRadiation)
        return "Solar radiation MJ m-2";
    else if (myVar == dailyAirHumidityMax)
        return "Max. relative humidity %";
    else if (myVar == dailyAirHumidityMin)
        return "Min. relative humidity %";
    else if (myVar == globalIrradiance)
        return "Solar irradiance W m-2";
    else if (myVar == windIntensity)
        return "Wind intensity m s-1";
	else if (myVar == dailyWindIntensityAvg)
        return "Average wind intensity m s-1";
    else if (myVar == dailyWindIntensityMax)
        return "Maximum wind intensity m s-1";
    else if (myVar == dailyWindDirectionPrevailing)
        return "Prevailing wind direction °";
    else if (myVar == referenceEvapotranspiration || myVar == dailyReferenceEvapotranspiration)
        return "Reference evapotranspiration mm";
    else if (myVar == leafWetness || myVar == dailyLeafWetness)
        return "Leaf wetness h";								
    else if (myVar == noMeteoTerrain)
        return "Digital Terrain Model m";

    else
        return "None";
}



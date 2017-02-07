/*!

    CRITERIA 3D
    \copyright (C) 2011 Fausto Tomei, Gabriele Antolini, Alberto Pistocchi,
    Antonio Volta, Giulia Villani, Marco Bittelli

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
    ftomei@arpa.emr.it
    fausto.tomei@gmail.com
    gantolini@arpa.emr.it
    alberto.pistocchi@gecosistema.it
    marco.bittelli@unibo.it
*/

#ifndef TYPESEXTRA_H
#define TYPESEXTRA_H

struct TboundaryHeat{
    double temperature;                     /*!< [K] temperature of the boundary (ex. air temperature) */
    double relativeHumidity;                /*!< [] relative humidity */
    double vaporConcentration;              /*!< [kg m-3] water vapour concentration */
    double invariantFluxes;                 /*!< [W] fluxes which are not to be updated inside */
    double height;                          /*!< [m] reference height */
    double roughnessHeight;                 /*!< [m] surface roughness height */
    double windSpeed;                       /*!< [m s-1] wind speed */
    double rainTemperature;                 /*!< [K] temperature of rain water */
    double globalIrradiance;                /*!< [W m-2] global shortwave irradiance */
    double netIrradiance;                   /*!< [W m-2] net irradiance */
    double sensibleFlux;                    /*!< [W m-2] boundary sensible heat flow density */
    double latentFlux;                      /*!< [W m-2] boundary latent heat flow density */
    double radiativeFlux;                   /*!< [W m-2] boundary net radiative flow density */
    double aerodynamicConductance;          /*!< [m s-1] aerodynamic conductance for heat */
    double soilConductance;                 /*!< [m s-1] soil conductance */
    double albedo;                          /*!< [] albedo */
};

struct TgroundWater{                        /*!< [K] ground water temperature */
    double temperature;

    void initialize()
    {
         temperature = NODATA;
    }
};

struct THeatFlux{
    double isothermLatent;  			// [W]
    double advective;                   // [W]
    double diffusive;                   // [W]
    double thermLatent;                 // [W]
    } ;

struct TCrit3DNodeHeat{
    double T;					/*!< [K] node temperature */
    double oldT;				/*!< [K] old node temperature */
    double Kh;					/*!< [W m^-1 K^-1] thermal conductivity */
    double vapor;				/*!< [kg] water vapor concentration */
    double Qh;					/*!< [W] heat sink/source */
};

struct TCrit3DLinkedNodeExtra{
    THeatFlux* heatFlux;
};

struct TCrit3DnodeExtra{
    TCrit3DNodeHeat *Heat;      /*!< heat pointer */
   } ;

void initializeExtra(TCrit3DnodeExtra *myNodeExtra, bool computeHeat, bool computeSolutes);
void initializeLinkExtra(TCrit3DLinkedNodeExtra* myLinkedNodeExtra, bool computeHeat, bool computeSolutes);

#endif // TYPESEXTRA_H

# CRITERIA-3D
CRITERIA-3D is a three-dimensional water balance for small catchments.

It includes a numerical solution for three-dimensional water and heat flow in the soil, coupled surface and subsurface flow, meteorological data interpolation, radiation budget, crop development and crop water uptake. It needs hourly meteo data as input. 
See [CRITERIA3D](https://github.com/ARPA-SIMC/CRITERIA3D/blob/master/DOC/CRITERIA3D.pdf) for more information. 

![](https://github.com/ARPA-SIMC/CRITERIA3D/blob/master/DOC/img/CRITERIA3D.png)

## How to compile CRITERIA-3D
Dependencies:
- Qt libraries: Qt 5.x or following is needed.
- [MapGraphics library](https://github.com/raptorswing/MapGraphics) (modified version) you have to build with Qt the project: mapGraphics/MapGraphics.pro
- [Qwt library](http://qwt.sourceforge.net/): you have to download and compile the qwt library, then check the correct path in: bin/CRITERIA3D/CRITERIA3D.pro and agrolib/soilWidget/soilWidget.pro

When all the dependencies are installed you can build the project: bin/Makeall_CRITERIA3D

## soilFluxes3D library 
#### agrolib/soilFluxed3D
SoilFluxes3D is a numerical solution for flow equations of water and heat in the soil, in a three-dimensional domain.
Surface water flow is described by the two-dimensional parabolic approximation of the St. Venant equation, using Manning’s equation of motion; subsurface water flow is described by the three-dimensional Richards’ equation for the unsaturated zone and by three-dimensional Darcy’s law for the saturated zone, using an integrated finite difference formulation.

Water fluxes equations may be coupled with the heat flux equations, which include diffusive, latent and advective terms. Atmospheric data (net irradiance, air temperature and relative humidity, wind speed) could be used as top boundary conditions.

## HEAT1D 
#### bin/Makeall_HEAT1D
HEAT1D is a graphical interface for testing the soilFluxex3D library in a 1D domain. Users can set fixed or variable atmospheric boundary conditions and soil parameters. Output graph results include soil temperature, soil water content, heat fluxes (diffusive and latent), liquid (isothermal and thermal) and vapor (isothermal and thermal) water fluxes.  
Graphs are produced using the Qwt plot library (http://qwt.sourceforge.net) which should be compiled before usage.

## Soil Widget 
#### test/Makeall_SoilWidget

Soil widget is a tool to show/edit soil data, including water retention and hydrologic conductivity curves, using modified Van Genuchten - Mualem model.
Click here to download the last release of [soilWidget](https://github.com/ARPA-SIMC/CRITERIA3D/releases/tag/SoilWidget)

![](https://github.com/ARPA-SIMC/CRITERIA3D/blob/master/DOC/img/soilWidget.png)




## License
CRITERIA-3D has been developed under contract issued by 
ARPAE Hydro-Meteo-Climate Service, Emilia-Romagna, Italy: http://www.arpae.it/sim/

The executables (CRITERIA3D, VINE3D, HEAT1D, etc.) are released under the GNU GPL license, libreries (agrolib/) are released under the GNU LGPL license.

## Authors
Fausto Tomei <ftomei@arpae.it>

Gabriele Antolini	 <gantolini@arpae.it>

Laura Costantini  <laura.costantini0@gmail.com>

Antonio Volta		<antonio.volta@agromet.it>

## Contributions
Marco Bittelli   <marco.bittelli@unibo.it>

Vittorio Marletto <vmarletto@arpae.it>

Alberto Pistocchi	 <alberto.pistocchi@jrc.ec.europa.eu>

Margot Van Soetendaal <margot@farnet.eu>

Tomaso Tonelli <ttonelli@arpae.it>

Franco Zinoni <fzinoni@arpae.it>

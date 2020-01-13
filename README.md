<<<<<<< HEAD
# CRITERIA-3D
CRITERIA3D is a three-dimensional water balance for small catchments.

It includes a numerical solution for three-dimensional water and heat flow in the soil, coupled surface and subsurface flow, meteorological data interpolation, radiation budget, crop development and crop water uptake. It needs hourly meteo data as input. 

![](https://github.com/ARPA-SIMC/CRITERIA3D/blob/master/DOC/img/CRITERIA3D.png)

## How to compile CRITERIA-3D
Dependencies:
- Qt libraries: Qt 5.x or following is needed.
- MapGraphics library (https://github.com/raptorswing/MapGraphics): you have to compile the project mapGraphics/MapGraphics.pro
- qwt library (http://qwt.sourceforge.net/): you have to dowload and compile qwt, then check the correct path in bin/CRITERIA3D/CRITERIA3D.pro

When the dependencies are installed you can compile the project bin/Makeall_CRITERIA3D
=======
# PRAGA
Program for Agrometeorological Analysis
>>>>>>> 606d64c736232b2384413741361222d0ebc82034

![](https://github.com/ftomei/PRAGA/blob/master/DOC/img/PRAGA_screenshot.png)

<<<<<<< HEAD
Water fluxes equations may be coupled with the heat flux equations, which include diffusive, latent and advective terms. Atmospheric data (net irradiance, air temperature and relative humidity, wind speed) could be used as top boundary conditions. See [CRITERIA3D](http://www.sciencedirect.com/science/article/pii/S0309170809001754) for more information.

## HEAT1D
HEAT1D is a graphical interface for testing the soilFluxex3D library in a 1D domain. Users can set fixed or variable atmospheric boundary conditions and soil parameters. Output graph results include soil temperature, soil water content, heat fluxes (diffusive and latent), liquid (isothermal and thermal) and vapor (isothermal and thermal) water fluxes.  
Graphs are produced using the qwt plot library (http://qwt.sourceforge.net/) which should be compiled before usage.

## Soil Widget
Soil widget is a tool to show/edit soil data, including water retention and hydrologic conductivity curves, using modified Van Genuchten - Mualem model.

[-->Download soilWidget](https://github.com/ARPA-SIMC/CRITERIA3D/releases/tag/v0.1)
![](https://github.com/ARPA-SIMC/CRITERIA3D/blob/master/DOC/img/soilWidget.png)

## TestCriteria1D
Test program for the 1D water balance using criteriaModel, crop and soil libraries.  
To compile it, use the project `test/Makeall_CRITERIA1D`. An example project is available in `DATA/PROJECT/kiwifruit` directory.

## License
CRITERIA-3D has been developed under contract issued by 
ARPAE Hydro-Meteo-Climate Service, Emilia-Romagna, Italy: http://www.arpae.it/sim/

The executables (CRITERIA3D, VINE3D, HEAT1D, etc.) are released under the GNU GPL license, libreries are released under the GNU LGPL license.
=======
PRAGA is a geographical application for agrometeorological and climate analysis.  
See [PRAGA wiki](https://github.com/ftomei/PRAGA/wiki/PRAGA-wiki) for more informations.

# License
PRAGA has been developed under contract issued by 
ARPAE Climate Observatory, Emilia-Romagna, Italy: http://www.arpae.it/sim/

The executable is released under the GNU GPL license, libreries are released under the GNU LGPL license.
>>>>>>> 606d64c736232b2384413741361222d0ebc82034

## Authors
Fausto Tomei <ftomei@arpae.it>

Gabriele Antolini	 <gantolini@arpae.it>

Laura Costantini  <laura.costantini0@gmail.com>

Antonio Volta		<antonio.volta@agromet.it>

## Contributions
Marco Bittelli   <marco.bittelli@unibo.it>

Vittorio Marletto <vmarletto@arpae.it>

<<<<<<< HEAD
Alberto Pistocchi	 <alberto.pistocchi@jrc.ec.europa.eu>

Tomaso Tonelli <ttonelli@arpae.it>

Margot Van Soetendaal <margot@farnet.eu>

Franco Zinoni <fzinoni@arpae.it>
=======
Andrea Pasquali <apasquali@arpae.it>
>>>>>>> 606d64c736232b2384413741361222d0ebc82034

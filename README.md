
# CRITERIA3D
CRITERIA3D is a geographical application that three-dimensional water balance for small catchments. 
It includes a coupled surface and subsurface flow and it needs hourly meteo data as input.
It includes meteorological data interpolation, radiation budget, numerical solution for three-dimensional water and heat flow in the soil, snow accumulation and melt, crop development and crop water uptake. 

# How to compile CRITERIA3D
Dependencies:
- Qt libraries: Qt 5.x or following is needed.
- MapGraphics library (https://github.com/raptorswing/MapGraphics): you have only to compile the project mapGraphics/MapGraphics.pro
- qwt library: you have to dowload and install qwt, then check the correct path in CRITERIA3D.pro 
When the dependencies are installed you can compile the project bin/Makeall_CRITERIA3D

## soilFluxes3D library
SoilFluxes3D is a numerical solution for flow equations of water and heat in the soil, in a three-dimensional domain.
Surface water flow is described by the two-dimensional parabolic approximation of the St. Venant equation, using Manning’s equation of motion; subsurface water flow is described by the three-dimensional Richards’ equation for the unsaturated zone and by three-dimensional Darcy’s law for the saturated zone, using an integrated finite difference formulation.

Water fluxes equations may be coupled with the heat flux equations, which include diffusive, latent and advective terms. Atmospheric data (net irradiance, air temperature and relative humidity, wind speed) could be used as top boundary conditions. See [CRITERIA3D](http://www.sciencedirect.com/science/article/pii/S0309170809001754) for more information.

## HEAT1D
HEAT1D is a graphical interface for testing the soilFluxex3D library in a 1D domain. Users can set fixed or variable atmospheric boundary conditions and soil parameters. Output graph results include soil temperature, soil water content, heat fluxes (diffusive and latent), liquid (isothermal and thermal) and vapor (isothermal and thermal) water fluxes.  
Graphs are produced using the Qwt plot library (http://qwt.sourceforge.net/) which should be compiled before usage.

## Soil Widget
Soil widget is a tool to show/edit soil data, including water retention and hydrologic conductivity curves, using modified Van Genuchten - Mualem model.

[-->Download soilWidget](https://github.com/ARPA-SIMC/CRITERIA3D/releases/tag/v0.1)
![](https://github.com/ARPA-SIMC/CRITERIA3D/blob/master/DOC/img/soilWidget.png)

## TestCriteria1D
Test program for the 1D water balance using criteriaModel, crop and soil libraries.  
To compile it, use the project `test/Makeall_CRITERIA1D`. An example project is available in `DATA/PROJECT/kiwifruit` directory.

# License
CRITERIA3D has been developed under contract issued by 
ARPAE Hydro-Meteo-Climate Service, Emilia-Romagna, Italy: http://www.arpae.it/sim/

Executables (CRITERIA3D, VINE3D, HEAT1D, etc.) are released under the GNU GPL license, libreries are released under the GNU LGPL license.

# Authors
Fausto Tomei <ftomei@arpae.it>

Gabriele Antolini	 <gantolini@arpae.it>

Laura Costantini  <laura.costantini0@gmail.com>

Antonio Volta		<antonio.volta@agromet.it>

## Contributions

Marco Bittelli   <marco.bittelli@unibo.it>

Vittorio Marletto <vmarletto@arpae.it>

Alberto Pistocchi	 <alberto.pistocchi@jrc.ec.europa.eu>

Tomaso Tonelli <ttonelli@arpae.it>

Margot Van Soetendaal <margot@farnet.eu>

Franco Zinoni <fzinoni@arpae.it>

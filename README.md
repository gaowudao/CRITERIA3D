# CRITERIA3D distribution
CRITERIA3D distribution provides a set of libraries and executables for agro-meteorological and climate analysis.
It includes a numerical solution for three-dimensional water and heat flow in the soil, meteorological data interpolation, daily weather generator, radiation budget, snow accumulation and melt, plant development and plant water uptake.

## soilFluxes3D library
SoilFluxes3D is a numerical solution for flow equations of water and heat in the soil, in a three-dimensional domain.

Surface water flow is described by the two-dimensional parabolic approximation of the St. Venant equation, using Manning’s equation of motion; subsurface water flow is described by the three-dimensional Richards’ equation for the unsaturated zone and by three-dimensional Darcy’s law for the saturated zone, using an integrated finite difference formulation.

Water fluxes equations may be coupled with the heat flux equations, which include diffusive, latent and advective terms. Atmospheric data (net irradiance, air temperature and relative humidity, wind speed) could be used as top boundary conditions.
http://www.sciencedirect.com/science/article/pii/S0309170809001754

## PRAGA (PRogram for AGrometeorological Analysis)
PRAGA is a Graphical User Interface to manage the libraries in the distribution for agro-meteorological and climate analysis.

It uses a modified version of MapGraphics library:
https://github.com/raptorswing/MapGraphics

## HEAT1D
HEAT1D is a graphical interface for testing the soilFluxex3D library in a 1D domain. Users can set fixed or variable atmospheric boundary conditions and soil parameters. Output graph results include soil temperature, soil water content, heat fluxes (diffusive and latent), liquid (isothermal and thermal) and vapor (isothermal and thermal) water fluxes. Graphs are produced using the Qwt plot library (http://qwt.sourceforge.net/) which should be compiled before usage.


# License
CRITERIA3D has been developed under contract issued by 
ARPAE Hydro Meteo Climate Service, Emilia-Romagna, Italy: http://www.arpae.it/sim/

CRITERIA3D is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

# Authors
**Fausto Tomei** <ftomei@arpae.it> - <fausto.tomei@gmail.com>

**Gabriele Antolini**	 <gantolini@arpae.it>

Laura Costantini  <laura.costantini0@gmail.com>

Antonio Volta		<antonio.volta@agromet.it>

Alberto Pistocchi	 <alberto.pistocchi@jrc.ec.europa.eu>

Marco Bittelli   <marco.bittelli@unibo.it>

Margot Van Soetendaal <margot@farnet.eu>

Vittorio Marletto <vmarletto@arpae.it>

Franco Zinoni <fzinoni@arpae.it>

Tomaso Tonelli <ttonelli@arpae.it>



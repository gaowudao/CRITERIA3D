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


## License
CRITERIA-3D has been developed under contract issued by 
ARPAE Hydro-Meteo-Climate Service, Emilia-Romagna, Italy: http://www.arpae.it/sim/

The executables (CRITERIA3D, VINE3D, HEAT1D, etc.) are released under the GNU GPL license, libreries are released under the GNU LGPL license.

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

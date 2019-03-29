#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <malloc.h>
#include <time.h>
#include <iostream>

#include "wg2D.h"
#include "commonConstants.h"
#include "furtherMathFunctions.h"
#include "statistics.h"
#include "eispack.h"
#include "gammaFunction.h"




void weatherGenerator2D::temperatureCompute()
{
    // step 1 of temperature WG2D
    // step 2 of temperature WG2D
    // step 3 of temperature WG2D
    // step 4 of temperature WG2D
    // step 5 of temperature WG2D
}

void weatherGenerator2D::computeTemperatureParameters()
{
    temperatureCoefficients = (TtemperatureCoefficients *)calloc(nrStations, sizeof(TtemperatureCoefficients));
}

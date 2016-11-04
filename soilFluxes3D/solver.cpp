/*-----------------------------------------------------------------------------------

    CRITERIA 3D
    Copyright (C) 2011 Fausto Tomei, Gabriele Antolini, Alberto Pistocchi,
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
-----------------------------------------------------------------------------------*/

#include <stdio.h>
#include <math.h>
#include <malloc.h>

#include "header/types.h"
#include "header/solver.h"


double distance(unsigned long i, unsigned long j)
{
    return (sqrt(square(fabs(myNode[i].x - myNode[j].x))
        + square(fabs(myNode[i].y - myNode[j].y))
        + square(fabs(myNode[i].z - myNode[j].z))));
}


double distance2D(unsigned long i, unsigned long j)
{
    return (sqrt(square(fabs(myNode[i].x - myNode[j].x)) + square(fabs(myNode[i].y - myNode[j].y))));
}


double logarithmicMean(double k1, double k2)
{
    if (k1 == k2)
        return(k1);
    else
        return((k1 - k2) / log(k1/k2));
}

double computeMean(double k1, double k2)
{
    if (myParameters.conductivityMean == MEAN_LOGARITHMIC)
        return(logarithmicMean(k1, k2));
    else if (myParameters.conductivityMean == MEAN_GEOMETRIC)
        return(sqrt(k1 * k2));
    else
        //default: logarithmic
        return(logarithmicMean(k1, k2));
}


TlinkedNode* getLink(long i, long j)
{
    if (myNode[i].up.index == j) return &(myNode[i].up);

    if (myNode[i].down.index == j) return &(myNode[i].down);

    for (short l = 0; l < myStructure.nrLateralLinks; l++)
         if (myNode[i].lateral[l].index == j) return &(myNode[i].lateral[l]);

    return(NULL);
}


int calcola_iterazioni_max(int num_approssimazione)
{
 float max_iterazioni = (float)myParameters.iterazioni_max / (float)myParameters.maxApproximationsNumber * (float)(num_approssimazione + 1);
 return(maxValue(20, int(max_iterazioni)));
}


double GaussSeidelIterationWater(int direction)
 {
    double psi = 0.0, newX = 0.0;
    double norm = 0.0, infinityNorm = 0.0;
    short j;
    long i, firstIndex, lastIndex;

    if (direction == UP)
    {
        firstIndex = 0;
        lastIndex = myStructure.nrNodes;
    }
    else
    {
        firstIndex = myStructure.nrNodes -1;
        lastIndex = -1;
    }

    i = firstIndex;
    while (i != lastIndex)
    {
        newX = b[i];
        j = 1;
        while ((A[i][j].index != NOLINK) && (j < myStructure.maxNrColumns))
        {
            newX -= A[i][j].val * X[A[i][j].index];
            j++;
        }

        // surface check
        if (myNode[i].isSurface)
            if (newX < myNode[i].z)
                newX = myNode[i].z;

        // water potential [m]
        psi = fabs(newX - myNode[i].z);

        // infinity norm (normalized if psi > 1m)
        if (psi > 1.0)
            norm = (fabs(newX - X[i])) / psi;
        else
            norm = fabs(newX - X[i]);

        if (norm > infinityNorm) infinityNorm = norm;

        X[i] = newX;

        (direction == UP)? i++ : i--;
    }
    return(infinityNorm);
 }


bool GaussSeidelRelaxation (int approximation, double residualTolerance, int process)
{
    const double MAX_NORM = 1.0;
    double norm = MAX_NORM;
    double bestNorm = MAX_NORM;
    int iteration = 0;

    int maxIterationsNr = calcola_iterazioni_max(approximation);

    while ((norm > residualTolerance) && (iteration < maxIterationsNr))
	{
        if (process == PROCESS_WATER)
        {
            if (iteration%2 == 0)
                norm = GaussSeidelIterationWater(DOWN);
            else
                norm = GaussSeidelIterationWater(UP);

            if (norm > (bestNorm * 10.0))
                return(false);                    //not converging
            else if (norm < bestNorm)
                bestNorm = norm;
        }

        iteration++;
	}

	return(true);
}

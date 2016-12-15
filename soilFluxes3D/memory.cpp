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

#include <stdio.h>
#include <malloc.h>
#include "header/types.h"


void cleanArrays()
{
    /*! free matrix A */
    if (A != NULL)
    {
            for (long i=0; i < myStructure.nrNodes; i++)
                if (A[i] != NULL) free(A[i]);
            free(A);
            A = NULL;
    }

    /*! free arrays */
    if (b != NULL){ free(b); b = NULL; }
    if (C != NULL){ free(C); C = NULL; }
    if (C0 != NULL){ free(C0); C0 = NULL; }
    if (X != NULL) { free(X); X = NULL; }
    }


void cleanNodes()
{
    if (myNode != NULL)
    {
        for (long i = 0; i < myStructure.nrNodes; i++)
        {
			if (myNode[i].boundary != NULL) free(myNode[i].boundary);
			free(myNode[i].lateral);
        }
        free(myNode);
        myNode = NULL;
    }
}


/*!
 * \brief initialize matrix and arrays
 * \return OK/ERROR
 */
int initializeArrays()
{
    long i, j, n;

    /*! clean previous arrays */
    cleanArrays();

    /*! matrix solver: rows */
    A = (TmatrixElement **) calloc(myStructure.nrNodes, sizeof(TmatrixElement *));

    /*! matrix solver: columns */
    for (i = 0; i < myStructure.nrNodes; i++)
            A[i] = (TmatrixElement *) calloc(myStructure.maxNrColumns, sizeof(TmatrixElement));

    /*! initialize matrix solver */
    for (i = 0; i < myStructure.nrNodes; i++)
        for (j = 0; j < (myStructure.nrLateralLinks + 2); j++)
        {
            A[i][j].index   = NOLINK;
            A[i][j].val     = 0.;
        }

    b = (double *) calloc(myStructure.nrNodes, sizeof(double));
    for (n = 0; n < myStructure.nrNodes; n++) b[n] = 0.;

    X = (double *) calloc(myStructure.nrNodes, sizeof(double));

    /*! mass diagonal matrix */
    C = (double *) calloc(myStructure.nrNodes, sizeof(double));
    for (n = 0; n < myStructure.nrNodes; n++) C[n] = 0.;

    /*! mass diagonal matrix */
    C0 = (double *) calloc(myStructure.nrNodes, sizeof(double));
    for (n = 0; n < myStructure.nrNodes; n++) C0[n] = 0.;

    if (A == NULL) return(MEMORY_ERROR);
    else return(CRIT3D_OK);
}

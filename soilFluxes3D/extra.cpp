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
    along with CRITERIA3D.  If not, see <http://www.gnu.org/licenses/>

    contacts:
    ftomei@arpa.emr.it
    fausto.tomei@gmail.com
    gantolini@arpa.emr.it
    alberto.pistocchi@gecosistema.it
    marco.bittelli@unibo.it
*/

#include <math.h>
#include <stdio.h>

#include "commonConstants.h"
#include "header/extra.h"
#include "header/types.h"


void initializeExtraHeat(TCrit3DNodeHeat* myNodeExtraHeat)
{
    (*myNodeExtraHeat).T = ZEROCELSIUS + 20;
    (*myNodeExtraHeat).oldT = ZEROCELSIUS + 20;
    (*myNodeExtraHeat).Qh = 0;
}

void initializeExtra(TCrit3DnodeExtra *myNodeExtra, bool computeHeat, bool computeSolutes)
{
    if (computeHeat)
    {
        (*myNodeExtra).Heat = new(TCrit3DNodeHeat);
        initializeExtraHeat(myNodeExtra->Heat);
    }
    else (*myNodeExtra).Heat = NULL;

    if (computeSolutes)
    {
        //TODO
    }
}

void initializeLinkExtra(TCrit3DLinkedNodeExtra* myLinkedNodeExtra, bool computeHeat, bool computeSolutes)
{
    if (computeHeat)
    {
        myLinkedNodeExtra->heatFlux = new(THeatFlux);

        (*myLinkedNodeExtra).heatFlux->waterFlux = 0.;

        if (myStructure.saveHeatFluxes == SAVE_HEATFLUXES_ALL)
        {
            (*myLinkedNodeExtra).heatFlux->fluxes = new float(4);
            (*myLinkedNodeExtra).heatFlux->fluxes[HEATFLUX_DIFFUSIVE] = NODATA;
            (*myLinkedNodeExtra).heatFlux->fluxes[HEATFLUX_LATENT_ISOTHERMAL] = NODATA;
            (*myLinkedNodeExtra).heatFlux->fluxes[HEATFLUX_LATENT_THERMAL] = NODATA;
            (*myLinkedNodeExtra).heatFlux->fluxes[HEATFLUX_ADVECTIVE] = NODATA;
        }
        else if (myStructure.saveHeatFluxes = SAVE_HEATFLUXES_TOTAL)
        {
            (*myLinkedNodeExtra).heatFlux->fluxes = new float(1);
            (*myLinkedNodeExtra).heatFlux->fluxes[HEATFLUX_TOTAL] = NODATA;
        }
        else
        {
            (*myLinkedNodeExtra).heatFlux->fluxes = NULL;
        }

    }
    else (*myLinkedNodeExtra).heatFlux = NULL;

    if (computeSolutes)
    {
        // TODO
    }
}



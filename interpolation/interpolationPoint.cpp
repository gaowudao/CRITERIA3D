/*-----------------------------------------------------------------------------------
    CRITERIA3D

    Copyright 2016 Fausto Tomei, Gabriele Antolini,
    Alberto Pistocchi, Marco Bittelli, Antonio Volta, Laura Costantini

    You should have received a copy of the GNU General Public License
    along with Nome-Programma.  If not, see <http://www.gnu.org/licenses/>.

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

    Contacts:
    fausto.tomei@gmail.com
    ftomei@arpae.it
-----------------------------------------------------------------------------------*/


#include "interpolationPoint.h"
#include "gis.h"
#include "commonConstants.h"

Crit3DInterpolationDataPoint::Crit3DInterpolationDataPoint()
{
    index = NODATA;
    isActive = false;
    distance = NODATA;
    deltaZ = NODATA;
    value = NODATA;
    orogIndex = NODATA;
    urbanFraction = NODATA;
    seaDistance = NODATA;
    aspect = NODATA;
    genericProxyValue = NODATA;

    point = new gis::Crit3DPoint();
}

void Crit3DInterpolationDataPoint::setOrogIndex(float myValue)
{ orogIndex = myValue;}

void Crit3DInterpolationDataPoint::setUrbanFraction(float myValue)
{ urbanFraction = myValue;}

void Crit3DInterpolationDataPoint::setSeaDistance(float myValue)
{ seaDistance = myValue;}

void Crit3DInterpolationDataPoint::setAspect(float myValue)
{ aspect = myValue;}

void Crit3DInterpolationDataPoint::setGenericProxy(float myValue)
{ genericProxyValue = myValue;}

float Crit3DInterpolationDataPoint::getOrogIndex()
{ return orogIndex;}

float Crit3DInterpolationDataPoint::getUrbanFraction()
{ return urbanFraction;}

float Crit3DInterpolationDataPoint::getSeaDistance()
{ return seaDistance;}

float Crit3DInterpolationDataPoint::getAspect()
{ return aspect;}

float Crit3DInterpolationDataPoint::getGenericProxy()
{ return genericProxyValue;}

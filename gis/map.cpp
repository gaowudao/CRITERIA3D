/*!
    \copyright 2010-2016 Fausto Tomei, Gabriele Antolini,
    Alberto Pistocchi, Marco Bittelli, Antonio Volta, Laura Costantini

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
    fausto.tomei@gmail.com
    ftomei@arpae.it
*/


#include "commonConstants.h"
#include "map.h"
#include "math.h"

namespace gis
{
    Crit3DUtmWindow::Crit3DUtmWindow() {}

    Crit3DUtmWindow::Crit3DUtmWindow(const Crit3DUtmPoint& v0, const Crit3DUtmPoint& v1)
    {
        this->v0 = v0;
        this->v1 = v1;
    }

    double Crit3DUtmWindow::width()
    {
        return fabs(this->v1.x - this->v0.x);
    }

    double Crit3DUtmWindow::height()
    {
        return fabs(this->v1.y - this->v0.y);
    }

    Crit3DPixelWindow::Crit3DPixelWindow() {}

    Crit3DPixelWindow::Crit3DPixelWindow(const Crit3DPixel& v0, const Crit3DPixel& v1)
    {
        this->v0 = v0;
        this->v1 = v1;
    }

    int Crit3DPixelWindow::width()
    {
        return abs(this->v1.x - this->v0.x);
    }

    int Crit3DPixelWindow::height()
    {
        return abs(this->v1.y - this->v0.y);
    }

    Crit3DGeoMap::Crit3DGeoMap()
    {
        this->isDrawing = false;
        this->isChanged = false;
        this->isSelecting = false;

        this->degreeToPixelX = 1.0;
        this->pixelToDegreeX = 1.0 / this->degreeToPixelX;

        this->degreeToPixelY = 1.0;
        this->pixelToDegreeY = 1.0 / this->degreeToPixelY;
    }

    void Crit3DGeoMap::setResolution(double dx, double dy)
    {
        this->pixelToDegreeX= dx;
        this->degreeToPixelX = 1.0 / this->pixelToDegreeX;

        this->pixelToDegreeY = dy;
        this->degreeToPixelY = 1.0 / this->pixelToDegreeY;
    }
}

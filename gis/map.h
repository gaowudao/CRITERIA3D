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


#ifndef MAP_H
#define MAP_H

    #ifndef GIS_H
        #include "gis.h"
    #endif

    namespace gis
    {
        enum mapActionType {actionDrag, actionSelect, operationSum, actionZoom, actionNone};

        class Crit3DMapPoint
        {
        public:
            Crit3DUtmPoint utm;
            Crit3DPixel pixel;

            Crit3DMapPoint();
            Crit3DMapPoint(const Crit3DUtmPoint& utm, const Crit3DPixel& pixel);
        };

        class Crit3DUtmWindow
        {
        public:
            Crit3DUtmPoint v0;
            Crit3DUtmPoint v1;

            Crit3DUtmWindow();
            Crit3DUtmWindow(const Crit3DUtmPoint& v0, const Crit3DUtmPoint& v1);
            double width();
            double height();
        };

        class Crit3DPixelWindow
        {
        public:
            Crit3DPixel v0;
            Crit3DPixel v1;

            Crit3DPixelWindow();
            Crit3DPixelWindow(const Crit3DPixel& v0, const Crit3DPixel& v1);
            int width();
            int height();
        };


        class Crit3DGeoMap
        {
        public:
            bool isDrawing;
            bool isChanged;
            bool isSelecting;

            Crit3DGeoPoint referencePoint;
            Crit3DGeoPoint bottomLeft;
            Crit3DGeoPoint topRight;

            double degreeToPixelX;
            double pixelToDegreeX;
            double degreeToPixelY;
            double pixelToDegreeY;

            Crit3DGeoMap();

            void setResolution(double dx, double dy);
        };
    }



#endif // MAP_H

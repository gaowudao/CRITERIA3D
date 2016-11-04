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


        class Crit3DMapArea
        {
        public:
            bool isCenterDefined;
            bool isDrawing;
            bool isChanged;
            bool isSelecting;

            Crit3DUtmPoint center;
            Crit3DUtmPoint previousCenter;
            Crit3DPixel dragPosition;

            double metreToPixel;
            double pixelToMetre;

            mapActionType activeAction;

            Crit3DMapArea();

            void setUtmCenter(double x, double y);
            void setResolution(double pixelToMetre);
        };
    }



#endif // MAP_H

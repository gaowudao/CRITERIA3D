/*!
    \file gis.h

    \abstract Gis structures: UTM point and raster

    This file is part of CRITERIA3D.

    CRITERIA3D has been developed under contract issued by A.R.P.A.E. Emilia-Romagna.

    \copyright
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

    \authors
    Fausto Tomei ftomei@arpae.it
    Gabriele Antolini gantolini@arpae.it
*/

#ifndef GIS_H
#define GIS_H

    #ifndef VECTOR_H
        #include <vector>
    #endif
    #ifndef _GLIBCXX_STRING
        #include <string>
    #endif
    #ifndef COLOR_H
        #include "color.h"
    #endif

#include <cmath>

    enum operationType {operationMin, operationMax, operationSum, operationSubtract, operationProduct, operationDivide};

    namespace gis
    {
        class  Crit3DPixel {
        public:
            int x;
            int y;
            Crit3DPixel();
            Crit3DPixel(int _x, int _y);
        };

        class Crit3DGridHeader;

        class  Crit3DUtmPoint {
        public:
            double x;
            double y;

            Crit3DUtmPoint();
            Crit3DUtmPoint(double x, double y);

            bool isInsideGrid(const Crit3DGridHeader& myGridHeader) const;
        };

        class  Crit3DGeoPoint {
        public:
            double latitude;
            double longitude;

            Crit3DGeoPoint();
            Crit3DGeoPoint(double lat, double lon);
        };


        class  Crit3DPoint {
        public:
            Crit3DUtmPoint utm;
            double z;

            Crit3DPoint();
            Crit3DPoint(double, double, double);
        };

        class Crit3DGridHeader
        {
        public:
            long nrRows;
            long nrCols;
            double cellSize;
            float flag;
            Crit3DUtmPoint* llCorner;

            Crit3DGridHeader();

            friend bool operator == (const Crit3DGridHeader& myHeader1, const Crit3DGridHeader& myHeader2);
        };

        class Crit3DRasterGrid
        {
        public:
            Crit3DGridHeader* header;
            Crit3DColorScale* colorScale;
            float** value;
            float minimum, maximum;
            bool isLoaded;
            std::string timeString;

            Crit3DUtmPoint* utmPoint(long myRow, long myCol);

            void freeGrid();
            void emptyGrid();

            Crit3DRasterGrid();
            ~Crit3DRasterGrid();

            void setConstantValue(float initValue);

            bool initializeGrid();
            bool initializeGrid(float initValue);
            bool initializeGrid(const Crit3DRasterGrid& initGrid);
            bool initializeGrid(const Crit3DGridHeader& initHeader);
            bool initializeGrid(const Crit3DRasterGrid& initGrid, float initValue);

            bool setConstantValueWithBase(float initValue, const Crit3DRasterGrid& initGrid);
            float getValueFromRowCol(long myRow, long myCol) const;
            Crit3DPoint mapCenter();
        };


        class Crit3DGisSettings
        {
        public:
            bool isNorthernEmisphere;
            int utmZone;
            bool isUTC;
            int timeZone;

            Crit3DGisSettings();
        };

        class Crit3DEllipsoid
        {
        public:
            double equatorialRadius;
            double eccentricitySquared;

            Crit3DEllipsoid();
        };

        float computeDistance(float x1, float y1, float x2, float y2);
        double computeDistancePoint(Crit3DUtmPoint* p0, Crit3DUtmPoint *p1);
        bool updateMinMaxRasterGrid(Crit3DRasterGrid* myGrid);
        bool updateColorScale(Crit3DRasterGrid* myGrid, long row0, long row1, long col0, long col1);
        bool getRowColFromXY(const Crit3DRasterGrid &myGrid, double myX, double myY, long* myRow, long* myCol);
        bool isOutOfGridRowCol(long myRow, long myCol, const Crit3DRasterGrid &myGrid);
        void getUtmXYFromRowColSinglePrecision(const Crit3DRasterGrid& myGrid, long myRow, long myCol,float* myX,float* myY);
        void getUtmXYFromRowCol(const Crit3DRasterGrid& myGrid, long myRow, long myCol ,double* myX, double* myY);
        void getUtmXYFromRowCol(const Crit3DGridHeader& myHeader,long myRow, long myCol, double* myX, double* myY);
        void getLatLonFromRowCol(const Crit3DGridHeader& myHeader, long myRow, long myCol, double* lat, double* lon);
        float getValueFromXY(const Crit3DRasterGrid& myGrid, double x, double y);

        bool isOutOfGridXY(double x, double y, Crit3DGridHeader* header);
        Crit3DGeoPoint* getRasterGeoCenter(Crit3DGridHeader* header);
        double getRasterMaxSize(Crit3DGridHeader* header);

        bool isMinimum(const Crit3DRasterGrid& myGrid, long row, long col);
        bool isMinimumOrNearMinimum(const Crit3DRasterGrid& myGrid, long row, long col);
        bool isBoundary(const Crit3DRasterGrid& myGrid, long row, long col);
        bool isStrictMaximum(const Crit3DRasterGrid& myGrid, long row, long col);

        bool getNorthernEmisphere();
        void getLatLonFromUtm(const Crit3DGisSettings& gisSettings, double utmX,double utmY, double *myLat, double *myLon);
        void getLatLonFromUtm(const Crit3DGisSettings& gisSettings, const Crit3DUtmPoint& utmPoint, Crit3DGeoPoint *geoPoint);

        void latLonToUtm(double lat, double lon,double *utmEasting,double *utmNorthing,int *zoneNumber);
        void latLonToUtmForceZone(int zoneNumber, double lat, double lon, double *utmEasting, double *utmNorthing);
        void utmToLatLon(int zoneNumber, bool north, double utmEasting, double utmNorthing, double *lat, double *lon);
        bool isValidUtmTimeZone(int utmZone, int timeZone);

        bool readEsriGrid(std::string myFileName, Crit3DRasterGrid* myGrid, std::string* myError);
        bool writeEsriGrid(std::string myFileName, Crit3DRasterGrid* myGrid, std::string* myError);

        bool mapAlgebra(Crit3DRasterGrid* myMap1, Crit3DRasterGrid* myMap2, Crit3DRasterGrid *myMapOut, operationType myOperation);
        bool mapAlgebra(Crit3DRasterGrid* myMap1, float myValue, Crit3DRasterGrid *myMapOut, operationType myOperation);
        bool prevailingMap(const Crit3DRasterGrid& inputMap,  Crit3DRasterGrid *outputMap);
        float prevailingValue(const std::vector<float> valueList);

        bool computeLatLonMaps(const gis::Crit3DRasterGrid& myGrid,
                               gis::Crit3DRasterGrid* latMap, gis::Crit3DRasterGrid* lonMap,
                               const gis::Crit3DGisSettings& gisSettings);

        bool computeSlopeAspectMaps(const gis::Crit3DRasterGrid& myDtm,
                               gis::Crit3DRasterGrid* slopeMap, gis::Crit3DRasterGrid* aspectMap);

        bool getGeoExtentsFromUTMHeader(const Crit3DGisSettings& mySettings,
                                        Crit3DGridHeader *utmHeader, Crit3DGridHeader *latLonHeader);
    }


#endif // GIS_H

#ifndef NETCDFHANDLER_H
#define NETCDFHANDLER_H

    #include "gis.h"
    #include <string>

    class NetCDFHandler
    {
    public:
        gis::Crit3DRasterGrid dataGrid;
        gis::Crit3DLatLonHeader latLonHeader;

        bool isLatLon;
        bool isStandardTime;
        bool isLoaded;

        NetCDFHandler();

        bool readProperties(std::string fileName, std::stringstream *buffer);
        bool isPointInside(gis::Crit3DGeoPoint geoPoint, int utmZone);
    private:

        int nrX, nrY, nrLat, nrLon, nrTime;
        int idTime, idX, idY, idLat, idLon;
        float *x, *y;

        double *doubleT;
        float *floatT;

        int timeType;

        void initialize();
    };


#endif // NETCDFHANDLER_H

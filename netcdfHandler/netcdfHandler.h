#ifndef NETCDFHANDLER_H
#define NETCDFHANDLER_H

    #include "gis.h"
    #include <string>

    class NetCDFHandler
    {
    public:
        gis::Crit3DRasterGrid dataGrid;

        bool isLatLon;

        NetCDFHandler();

        bool readProperties(std::string fileName, std::stringstream *buffer);
    private:

        int nrX, nrY, nrLat, nrLon, nrTime;
        int idTime, idX, idY, idLat, idLon;
        float *x, *y;

        void initialize();
    };


#endif // NETCDFHANDLER_H

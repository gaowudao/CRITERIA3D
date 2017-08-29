#ifndef NETCDFHANDLER_H
#define NETCDFHANDLER_H

    #include <string>

    class NetCDFHandler
    {
    public:

        NetCDFHandler();

        bool readProperties(std::string fileName, std::stringstream *buffer);
    private:

        int nrX, nrY, nrLat, nrLon, nrTime;
        int idTime, idX, idY, idLat, idLon;
        bool isUTM;

        void initialize();
    };


#endif // NETCDFHANDLER_H

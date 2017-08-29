#ifndef NETCDFHANDLER_H
#define NETCDFHANDLER_H

    #include <string>

    class NetCDFHandler
    {
    public:

        NetCDFHandler();

        bool readProperties(std::string fileName, std::stringstream *buffer);
    };


#endif // NETCDFHANDLER_H

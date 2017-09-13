#ifndef NETCDFHANDLER_H
#define NETCDFHANDLER_H

    #include "gis.h"
    #include <ctime>
    #include <string>

    class NetCDFVariable
    {
    public:
        std::string name;
        std::string longName;
        int id;
        int type;

        NetCDFVariable();
        NetCDFVariable(char *myName, int myId, int myType);

        std::string getVarName();
    };

    class NetCDFHandler
    {
    public:
        gis::Crit3DRasterGrid dataGrid;
        gis::Crit3DLatLonHeader latLonHeader;

        std::vector<NetCDFVariable> variables;

        bool isLatLon;
        bool isStandardTime;
        bool isLoaded;

        NetCDFHandler();

        void initialize(int myUtmZone);

        bool readProperties(std::string fileName, std::stringstream *buffer);
        bool isPointInside(gis::Crit3DGeoPoint geoPoint);
        int getDimensionIndex(char* dimName);
        bool setVarLongName(char* varName, char* varLongName);
        std::string getDateTimeStr(int timeIndex);

        bool exportDataSeries(int idVar, gis::Crit3DGeoPoint geoPoint, time_t firstTime, time_t lastTime, std::stringstream *buffer);

        time_t NetCDFHandler::getTime(int timeIndex);
        inline time_t getFirstTime() {return getTime(0);}
        inline time_t getLastTime() {return getTime(nrTime-1);}

        inline int getNrVariables() {return variables.size();}

    private:

        int utmZone;
        int nrX, nrY, nrLat, nrLon, nrTime;
        int idTime, idX, idY, idLat, idLon;
        float *x, *y;
        double *time;
        int timeType;

        std::vector<NetCDFVariable> dimensions;
    };


#endif // NETCDFHANDLER_H

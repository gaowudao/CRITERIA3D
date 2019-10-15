#ifndef NETCDFHANDLER_H
#define NETCDFHANDLER_H

    #ifndef GIS_H
        #include "gis.h"
    #endif
    #ifndef _CTIME_
        #include <ctime>
    #endif

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
        int ncId;

        gis::Crit3DRasterGrid dataGrid;
        gis::Crit3DGridHeader latLonHeader;

        std::vector<NetCDFVariable> variables;

        bool isLatLon;
        bool isStandardTime;
        bool isHourly;
        Crit3DDate firstDate;
        bool isLoaded;

        NetCDFHandler();

        void clear();
        void initialize(int _utmZone);


        bool isPointInside(gis::Crit3DGeoPoint geoPoint);

        bool setVarLongName(std::string varName, std::string varLongName);

        int getDimensionIndex(char* dimName);
        std::string getDateTimeStr(int timeIndex);
        std::string getVarName(int idVar);

        Crit3DTime getTime(int timeIndex);

        inline Crit3DTime getFirstTime() { return getTime(0); }
        inline Crit3DTime getLastTime() { return getTime(nrTime-1); }
        inline bool isTimeReadable() { return (getFirstTime() != NO_DATETIME); }
        inline unsigned int getNrVariables() { return unsigned(variables.size()); }

        bool readProperties(std::string fileName, std::stringstream *buffer);
        bool exportDataSeries(int idVar, gis::Crit3DGeoPoint geoPoint, Crit3DTime firstTime, Crit3DTime lastTime, std::stringstream *buffer);

    private:

        int utmZone;
        int nrX, nrY, nrLat, nrLon, nrTime;
        int idTime, idX, idY, idLat, idLon;
        bool isLatDecreasing;
        float *x, *y;
        float *lat, *lon;
        double *time;
        int timeType;

        std::vector<NetCDFVariable> dimensions;
    };


#endif // NETCDFHANDLER_H

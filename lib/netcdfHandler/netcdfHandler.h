#ifndef NETCDFHANDLER_H
#define NETCDFHANDLER_H

    #ifndef GIS_H
        #include "gis.h"
    #endif
    #ifndef _CTIME_
        #include <ctime>
    #endif
    #include <sstream>

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
    private:
        int utmZone;
        int nrX, nrY, nrLat, nrLon, nrTime;
        int idTime, idX, idY, idLat, idLon;

        float *x, *y;
        float *lat, *lon;
        double *time;
        bool isLatDecreasing;

        bool isStandardTime;
        bool isHourly;
        bool isDaily;
        Crit3DDate firstDate;

        std::stringstream metadata;
        std::vector<NetCDFVariable> dimensions;

    public:
        int ncId;
        bool isLatLon;

        std::vector<NetCDFVariable> variables;
        gis::Crit3DRasterGrid dataGrid;
        gis::Crit3DGridHeader latLonHeader;

        NetCDFHandler();

        void clear();
        void initialize(int _utmZone);

        bool isLoaded();
        bool isPointInside(gis::Crit3DGeoPoint geoPoint);

        bool setVarLongName(std::string varName, std::string varLongName);

        int getDimensionIndex(char* dimName);
        std::string getDateTimeStr(int timeIndex);
        std::string getVarName(int idVar);
        std::string getMetadata();

        Crit3DTime getTime(int timeIndex);
        NetCDFVariable getVariable(int idVar);

        inline Crit3DTime getFirstTime() { return getTime(0); }
        inline Crit3DTime getLastTime() { return getTime(nrTime-1); }
        inline bool isTimeReadable() { return (getFirstTime() != NO_DATETIME); }
        inline unsigned int getNrVariables() { return unsigned(variables.size()); }

        bool readProperties(std::string fileName);
        bool exportDataSeries(int idVar, gis::Crit3DGeoPoint geoPoint, Crit3DTime firstTime, Crit3DTime lastTime, std::stringstream *buffer);

        bool createNewFile(std::string fileName);

    };


#endif // NETCDFHANDLER_H

#ifndef METEOPOINT_H
#define METEOPOINT_H

    #include <string>

    #ifndef CRIT3DDATE_H
        #include "crit3dDate.h"
    #endif
    #ifndef METEO_H
        #include "meteo.h"
    #endif
    #ifndef GIS_H
        #include "gis.h"
    #endif

    namespace quality
    {
        enum type {missing_data, wrong_syntactic, wrong_spatial, wrong_variable, accepted};
    }

    struct TObsDataH {
        Crit3DDate date;
        float* tAir;
        float* prec;
        float* rhAir;
        float* tDew;
        float* irradiance;
        float* et0;
        float* windInt;
        int* leafW;
        float* transmissivity;
    };

    struct TObsDataD {
        Crit3DDate date;
        float tMin;
        float tMax;
        float tAvg;
        float prec;
        float rhMin;
        float rhMax;
        float rhAvg;
        float globRad;
        float et0;
        float windIntAvg;
        float windDirPrev;
        float windIntMax;
        float leafW;
        float waterTable;       // [m]
    };

    struct TObsDataM {
        int _month;
        int _year;
        float tMin;
        float tMax;
        float tAvg;
        float prec;
        float et0;
        float globRad;
    };

    class Crit3DMeteoPoint {
    public:
        std::string name;
        std::string id;
        std::string dataset;
        std::string state;
        std::string region;
        std::string province;
        std::string municipality;

        std::vector<gis::Crit3DPoint> aggregationPoints;
        long aggregationPointsMaxNr;

        gis::Crit3DPoint point;
        double latitude;
        double longitude;
        double area;
        int latInt;
        int lonInt;

        bool isUTC;
        int hourlyFraction;
        long nrObsDataDaysH;
        long nrObsDataDaysD;
        long nrObsDataDaysM;
        TObsDataH *obsDataH;
        TObsDataD *obsDataD;
        TObsDataM *obsDataM;
        quality::type myQuality;
        float currentValue;
        float residual;   
        bool active;
        bool selected;

        Crit3DMeteoPoint();

        void initializeObsDataH(int hourlyFraction, int numberOfDays, const Crit3DDate& firstDate);
        void emptyVarObsDataH(meteoVariable myVar, const Crit3DDate& myDate);
        void emptyVarObsDataH(meteoVariable myVar, const Crit3DDate& date1, const Crit3DDate& date2);
        void cleanObsDataH();
        bool isDateLoadedH(const Crit3DDate& myDate);
        bool isDateIntervalLoadedH(const Crit3DDate& date1, const Crit3DDate& date2);
        bool isDateIntervalLoadedH(const Crit3DTime& time1, const Crit3DTime& time2);
        float obsDataConsistencyH(meteoVariable myVar, const Crit3DTime& time1, const Crit3DTime& time2);

        void initializeObsDataD(int numberOfDays, const Crit3DDate& firstDate);
        void emptyVarObsDataD(meteoVariable myVar, const Crit3DDate& date1, const Crit3DDate& date2);
        void cleanObsDataD();
        bool isDateLoadedD(const Crit3DDate& myDate);
        bool isDateIntervalLoadedD(const Crit3DDate& date1, const Crit3DDate& date2);

        void initializeObsDataM(int numberOfMonths, int month, int year);
        void cleanObsDataM();

        float getMeteoPointValueH(const Crit3DDate& myDate, int myHour, int myMinutes, meteoVariable myVar);
        bool setMeteoPointValueH(const Crit3DDate& myDate, int myHour, int myMinutes, meteoVariable myVar, float myValue);
        float getMeteoPointValueD(const Crit3DDate& myDate, meteoVariable myVar);
        bool setMeteoPointValueD(const Crit3DDate& myDate, meteoVariable myVar, float myValue);

        float getMeteoPointValue(const Crit3DTime& myTime, meteoVariable myVar, frequencyType frequency);

    };


#endif // METEOPOINT_H

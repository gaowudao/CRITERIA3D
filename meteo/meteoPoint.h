#ifndef METEOPOINT_H
#define METEOPOINT_H

    #ifndef _GLIBCXX_STRING
        #include <string>
    #endif
    #ifndef CRIT3DDATE_H
        #include "crit3dDate.h"
    #endif
    #ifndef QUALITY_H
        #include "quality.h"
    #endif
    #ifndef GIS_H
        #include "gis.h"
    #endif

    struct TObsDataH {
        Crit3DDate date;
        float* tAir;
        float* prec;
        float* rhAir;
        float* tDew;
        float* irradiance;
        float* et0;
        float* windInt;
        float* pressure;
        int* wetDuration;
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
        float waterTableDepth;
    };

    class Crit3DMeteoPoint {
    public:
        std::string name;
        std::string id;
        std::string dataset;

        gis::Crit3DPoint point;
        double latitude;
        double longitude;

        bool isUTC;
        int hourlyFraction;
        long nrObsDataDaysH;
        long nrObsDataDaysD;
        TObsDataH *obsDataH;
        TObsDataD *obsDataD;
        quality::qualityValue myQuality;
        float residual;

        Crit3DMeteoPoint();
        Crit3DMeteoPoint(std::string, std::string, bool isUTC);

        void initializeObsDataH(int hourlyFraction, int numberOfDays, const Crit3DDate& firstDate);
        void emptyVarObsDataH(meteoVariable myVar, const Crit3DDate& myDate);
        void emptyVarObsDataH(meteoVariable myVar, const Crit3DDate& myDateIni, const Crit3DDate& myDateFin);
        void cleanObsDataH();
        bool isDateLoadedH(const Crit3DDate& myDate);
        bool isDateIntervalLoadedH(const Crit3DDate& myDateIni, const Crit3DDate& myDateFin);
        bool isDateIntervalLoadedH(const Crit3DTime& myTimeIni, const Crit3DTime& myTimeFin);
        float obsDataConsistencyH(meteoVariable myVar, const Crit3DTime& myTimeIni, const Crit3DTime& myTimeFin);
        float getMeteoPointValueH(const Crit3DDate& myDate, int myHour, int myMinutes, meteoVariable myVar);
        bool setMeteoPointValueH(const Crit3DDate& myDate, int myHour, int myMinutes, meteoVariable myVar, float myValue);

        void initializeObsDataD(int numberOfDays, const Crit3DDate& firstDate);
        void emptyVarObsDataD(meteoVariable myVar, const Crit3DDate& myDateIni, const Crit3DDate& myDateFin);
        void cleanObsDataD();
        bool isDateLoadedD(const Crit3DDate& myDate);
        bool isDateIntervalLoadedD(const Crit3DDate& myDateIni, const Crit3DDate& myDateFin);
        float getMeteoPointValueD(const Crit3DDate& myDate, meteoVariable myVar);
        bool setMeteoPointValueD(const Crit3DDate& myDate, meteoVariable myVar, float myValue);
    };


#endif // METEOPOINT_H

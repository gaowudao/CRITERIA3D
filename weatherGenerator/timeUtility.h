#ifndef TIMEUTILITY
#define TIMEUTILITY

    #ifndef CRIT3DDATE_H
        #include "crit3dDate.h"
    #endif

    #define NRDAYSTOLERANCE  15

    int numMonthsInPeriod(int m1, int m2);

    int getNumDaysInMonth(int myMonth, int myYear);

    bool getDoyFromSeason(QString season, int myPredictionYear, int* wgDoy1, int* wgDoy2);

    int checkLastYearDate(Crit3DDate inputFirstDate, Crit3DDate inputLastDate, int dataLenght, int myPredictionYear, int* wgDoy1);

    void fixWgDoy(int wgDoy1, int wgDoy2, int predictionYear, int myYear, int* fixwgDoy1, int* fixwgDoy2);


#endif // TIMEUTILITY

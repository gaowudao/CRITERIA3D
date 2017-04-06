#ifndef TIMEUTILITY
#define TIMEUTILITY

    #ifndef QSTRING_H
        #include <QString>
    #endif
    #ifndef CRIT3DDATE_H
        #include "crit3dDate.h"
    #endif

    // currentMETEO should include the last 9 months before wgDoy1, accept input data with max last 15 days missing
    #define NRDAYSTOLERANCE  15

    int numMonthsInPeriod(int myMonth1, int myMonth2);
    int getNumDaysInMonth(int myMonth, int myYear);

    bool getDoyFromSeason(QString season, int myPredictionYear, int* wgDoy1, int* wgDoy2);
    int checkLastYearDate(Crit3DDate inputFirstDate, Crit3DDate inputLastDate, int dataLenght, int myPredictionYear, int* wgDoy1);
    void fixWgDoy(int wgDoy1, int wgDoy2, int predictionYear, int myYear, int* fixwgDoy1, int* fixwgDoy2);
    QString getDateString(Crit3DDate myDate);

#endif // TIMEUTILITY

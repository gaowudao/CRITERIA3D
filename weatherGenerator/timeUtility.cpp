#include <QDebug>
#include <QDate>
#include <QString>

#include "crit3dDate.h"
#include "timeUtility.h"


int numMonthsInPeriod(int m1, int m2)
{
    int numMonthsInPeriod = 0;

    if (m2 >= m1)
        // regular period
        numMonthsInPeriod = (m2 - m1 + 1);
    else
        // irregular period (between years)
        numMonthsInPeriod = (12 - m1 + 1) + m2;

    return numMonthsInPeriod;
}


// check if currentMETEO includes the last 9 months before wgDoy1
// if it does returns the number of days equals to 9 months before wgDoy1
int checkLastYearDate(Crit3DDate inputFirstDate, Crit3DDate inputLastDate, int dataLenght, int myPredictionYear, int* wgDoy1)
{
    Crit3DDate predictionFirstDate = getDateFromDoy (myPredictionYear, *wgDoy1);

    if (inputLastDate.addDays(NRDAYSTOLERANCE+1) <  predictionFirstDate)
    {
        qDebug() << "\nObserved days missing are more than NRDAYSTOLERANCE" << NRDAYSTOLERANCE << endl;
        return(-1);
    }

    int predictionMonth = predictionFirstDate.month;
    int monthIndex = 0;
    int totalDay = 0;
    for (unsigned int i = 0; i < 9; i++)
    {
        monthIndex = (predictionMonth-1)- i;
        if (monthIndex <= 0)
        {
            monthIndex = monthIndex + 12 ;
            myPredictionYear = myPredictionYear - 1;
        }
        totalDay += getDaysInMonth(monthIndex, myPredictionYear);
    }

    // shift wgDoy1 if there are missing data
    if (inputLastDate < predictionFirstDate)
    {
        int delta = difference(inputLastDate, predictionFirstDate) - 1;
        *wgDoy1 -= delta;
        totalDay -= delta;
    }
    // use or not the observed data in the forecast period
    else if (USEDATA)
    {
        if (inputLastDate > predictionFirstDate.addDays(84))
        {
            qDebug() << "Check your XML: you have already all observed data" << endl;
            return(-1);
        }
        if (isLeapYear(predictionFirstDate.year))
            *wgDoy1 = (*wgDoy1 + (difference(predictionFirstDate, inputLastDate)) + 1 ) % 366;
        else
            *wgDoy1 = (*wgDoy1 + (difference(predictionFirstDate, inputLastDate)) + 1 ) % 365;

        totalDay += (difference(predictionFirstDate, inputLastDate)) + 1 ;
    }

    if ( difference(inputFirstDate, predictionFirstDate) < totalDay || dataLenght < (totalDay-NRDAYSTOLERANCE) )
    {
        // currentMETEO does not include 9 months before wgDoy1 or more than NRDAYSTOLERANCE days missing
        return(-1);
    }

    return totalDay ;
}


bool getDoyFromSeason(QString season, int myPredictionYear, int* wgDoy1, int* wgDoy2)
{
    QString period[12] = {"JFM","FMA","MAM","AMJ","MJJ","JJA","JAS","ASO","SON","OND","NDJ","DJF"};
    unsigned int i = 0;
    int found = 0;
    int month1, month2 = 0;

    for (i = 0; i<12; i++)
    {
        if (season.compare(period[i]) == 0)
        {
            found = 1;
            break;
        }
    }
    if (found == 0)
    {
        qDebug() << "Wrong season" ;
        return false;
    }

    month1 = i + 1;        // first month of my season
    month2 = (i + 3) % 12; // last month of my season
    if (month2 == 0) month2 = 12;

    Crit3DDate predictionFirstDate;
    predictionFirstDate.year = myPredictionYear;
    predictionFirstDate.month = month1;
    predictionFirstDate.day = 1;

    *wgDoy1 = getDoyFromDate(predictionFirstDate);

    // season between 2 years
    if (season.compare(period[10]) == 0 || season.compare(period[11]) == 0)
    {
        myPredictionYear = myPredictionYear + 1 ;
    }

    Crit3DDate predictionLastDate;
    predictionLastDate.year = myPredictionYear;
    predictionLastDate.month = month2;
    predictionLastDate.day = getDaysInMonth(month2, myPredictionYear);

    *wgDoy2 = getDoyFromDate(predictionLastDate);

    return true;
}


void fixWgDoy(int wgDoy1, int wgDoy2, int predictionYear, int myYear, int* fixwgDoy1, int* fixwgDoy2)
{
    // check if wgDoy1 and wgDoy2 have been computed starting from a leap year and adjust them for standard years
    if (wgDoy1 < wgDoy2)
    {
        if (isLeapYear(predictionYear) && !isLeapYear(myYear))
        {
            // if wgDoy1 or wgDoy2 > 29th Feb.
            if (wgDoy1 >= 60)
                *fixwgDoy1 = wgDoy1-1;

            if (wgDoy1 >= 60)
                *fixwgDoy2 = wgDoy2-1;
        }
        else if ( !isLeapYear(predictionYear) && isLeapYear(myYear))
        {
            // if wgDoy1 or wgDoy2 > 29th Feb.
            if (wgDoy1 >= 60)
                *fixwgDoy1 = wgDoy1+1;

            if (wgDoy1 >= 60)
                *fixwgDoy2 = wgDoy2+1;
        }
        else
        {
            *fixwgDoy1 = wgDoy1;
            *fixwgDoy2 = wgDoy2;
        }
    }
    else
    {
        if (isLeapYear(predictionYear) && !isLeapYear(myYear))
        {
            // if wgDoy1 > 29th Feb.
            if (wgDoy1 >= 60)
                *fixwgDoy1 = wgDoy1-1;
        }

        else if (isLeapYear(predictionYear+1) && !isLeapYear(myYear))
        {
            // if wgDoy2 > 29th Feb.
            if (wgDoy1 >= 60)
                *fixwgDoy2 = wgDoy2-1;
        }
        else
        {
            *fixwgDoy1 = wgDoy1;
            *fixwgDoy2 = wgDoy2;
        }
    }
}

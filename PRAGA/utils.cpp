#include "commonConstants.h"
#include "utils.h"


Crit3DDate getCrit3DDate(const QDate& d)
{
    Crit3DDate myDate = Crit3DDate(d.day(), d.month(), d.year());
    return myDate;
}

Crit3DTime getCrit3DTime(const QDateTime& t)
{
    Crit3DTime myTime;

    myTime.date.day = t.date().day();
    myTime.date.month = t.date().month();
    myTime.date.year = t.date().year();
    myTime.time = t.time().hour()*3600 + t.time().minute()*60 + t.time().second();

    return myTime;
}

QDate getQDate(const Crit3DDate& d)
{
    QDate myDate = QDate(d.year, d.month, d.day);
    return myDate;
}

QDateTime getQDateTime(const Crit3DTime& t)
{
    QDateTime myTime;
    QDate myQDate = QDate(t.date.year, t.date.month, t.date.day);
    myTime.setDate(myQDate);
    QTime myQTime = QTime(t.getHour(), t.getMinutes(), t.getSeconds(), 0);
    myTime.setTime(myQTime);

    return myTime;
}


bool getValue(QVariant myRs, float* myValue)
{
    if (myRs.isNull())
        *myValue = NODATA;
    else
        *myValue = myRs.toFloat();

    return (*myValue != NODATA);
}

bool getValue(QVariant myRs, double* myValue)
{
    if (myRs.isNull())
        *myValue = NODATA;
    else
        *myValue = myRs.toDouble();

    return (*myValue != NODATA);
}


bool getValue(QVariant myRs, int* myValue)
{
    if (myRs.isNull())
        *myValue = NODATA;
    else
        *myValue = myRs.toInt();

    return (*myValue != NODATA);
}

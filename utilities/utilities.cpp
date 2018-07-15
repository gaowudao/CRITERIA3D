#include <QDateTime>
#include <QVariant>
#include <QString>

#include "commonConstants.h"
#include "crit3dDate.h"
#include "utilities.h"


bool getValue(QVariant myRs, int* myValue)
{
    if (! myRs.isValid() || myRs.isNull())
    {
        *myValue = NODATA;
    }
    else
    {
        if (myRs == "" || myRs == "NULL")
            *myValue = NODATA;
        else
            *myValue = myRs.toInt();
    }

    return (*myValue != NODATA);
}


bool getValue(QVariant myRs, float* myValue)
{
    if (myRs.isNull())
        *myValue = NODATA;
    else
    {
        if (myRs == "")
             *myValue = NODATA;
        else
            *myValue = myRs.toFloat();
    }

    return (*myValue != NODATA);
}


bool getValue(QVariant myRs, double* myValue)
{
    if (myRs.isNull())
        *myValue = NODATA;
    else
    {
        if (myRs == "")
             *myValue = NODATA;
        else
            *myValue = myRs.toDouble();
    }

    return (*myValue != NODATA);
}

bool getValue(QVariant myRs, QDate* myValue)
{
    if (myRs.isNull())
        return false;
    else
    {
        if (myRs == "")
             return false;
        else
            *myValue = myRs.toDate();
    }

    return true;
}

bool getValue(QVariant myRs, QDateTime* myValue)
{
    if (myRs.isNull())
        return false;
    else
    {
        if (myRs == "")
             return false;
        else
            *myValue = myRs.toDateTime();
    }

    return true;
}


bool getValue(QVariant myRs, QString* myValue)
{
    if (myRs.isNull())
        *myValue = "";
    else
        *myValue = myRs.toString();

    return (*myValue != "");
}


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


Crit3DTime getCrit3DTime(const QDate& t, int hour)
{
    Crit3DTime myTime;

    myTime.date.day = t.day();
    myTime.date.month = t.month();
    myTime.date.year = t.year();
    myTime.time = hour * 3600;

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


QString getFileName(QString filePath)
{
    QString c;
    QString fileName = "";
    for (int i = filePath.length()-1; i >= 0; i--)
    {
        c = filePath.mid(i,1);
        if ((c != "\\") && (c != "/"))
            fileName = c + fileName;
        else
            return fileName;
    }
    return fileName;
}



QString getPath(QString filePath)
{
    QString fileName = getFileName(filePath);
    return filePath.left(filePath.length() - fileName.length());
}

int decadeFromDate(QDate date)
{

    int day = date.day();
    int decade;
    if ( day <= 10)
    {
        decade = 1;
    }
    else if ( day <= 20)
    {
        decade = 2;
    }
    else
    {
        decade = 3;
    }
    decade = decade + (3 * (date.month() - 1));
    return decade;

}

int getSeasonFromDate(QDate date)
{

    int month = date.month();
    switch (month) {
    case 3: case 4: case 5:
        return 1;
    case 6: case 7: case 8:
        return 2;
    case 9: case 10: case 11:
        return 3;
    case 12: case 1: case 2:
        return 4;
    default:
        return NODATA;
    }
}




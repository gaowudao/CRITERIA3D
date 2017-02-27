#include <QDate>
#include <QVariant>
#include <string>

#include "commonConstants.h"
#include "dataHandler.h"
#include "crit3dDate.h"
#include "Criteria1D.h"

bool getValue(QVariant myRs, int* myValue)
{
    if (myRs.isNull())
        *myValue = NODATA;
    else
        *myValue = myRs.toInt();

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
        *myValue = myRs.toDouble();

    return (*myValue != NODATA);
}


bool getValue(QVariant myRs, QString* myValue)
{
    if (myRs.isNull())
        *myValue = "";
    else
        *myValue = myRs.toString();

    return (*myValue != "");
}

Crit3DDate getCrit3DDate(const QDate& myDate)
{
    Crit3DDate date;

    date.day = myDate.day();
    date.month = myDate.month();
    date.year = myDate.year();
    return date;
}

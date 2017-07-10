#ifndef UTILS_H
#define UTILS_H

#ifndef CRIT3DDATE_H
    #include "crit3dDate.h"
#endif
#include <QDate>
#include <QVariant>

Crit3DDate getCrit3DDate(const QDate &myDate);
Crit3DTime getCrit3DTime(const QDateTime &myTime);
QDate getQDate(const Crit3DDate &myDate);
QDateTime getQDateTime(const Crit3DTime &myCrit3DTime);

bool getValue(QVariant myRs, int* myValue);
bool getValue(QVariant myRs, float* myValue);
bool getValue(QVariant myRs, double* myValue);


#endif // UTILS_H

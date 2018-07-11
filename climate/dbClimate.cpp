#include "dbClimate.h"


QString Crit3DClimate::climateElab() const
{
    return _climateElab;
}

void Crit3DClimate::setClimateElab(const QString &climateElab)
{
    _climateElab = climateElab;
}

int Crit3DClimate::yearStart() const
{
    return _yearStart;
}

void Crit3DClimate::setYearStart(int yearStart)
{
    _yearStart = yearStart;
}

int Crit3DClimate::yearEnd() const
{
    return _yearEnd;
}

void Crit3DClimate::setYearEnd(int yearEnd)
{
    _yearEnd = yearEnd;
}

meteoVariable Crit3DClimate::variable() const
{
    return _variable;
}

void Crit3DClimate::setVariable(const meteoVariable &variable)
{
    _variable = variable;
}

QString Crit3DClimate::genericPeriod() const
{
    return _genericPeriod;
}

void Crit3DClimate::setGenericPeriod(const QString &genericPeriod)
{
    _genericPeriod = genericPeriod;
}

QDate Crit3DClimate::genericPeriodDateStart() const
{
    return _genericPeriodDateStart;
}

void Crit3DClimate::setGenericPeriodDateStart(const QDate &genericPeriodDateStart)
{
    _genericPeriodDateStart = genericPeriodDateStart;
}

QDate Crit3DClimate::genericPeriodDateEnd() const
{
    return _genericPeriodDateEnd;
}

void Crit3DClimate::setGenericPeriodDateEnd(const QDate &genericPeriodDateEnd)
{
    _genericPeriodDateEnd = genericPeriodDateEnd;
}

int Crit3DClimate::nYears() const
{
    return _nYears;
}

void Crit3DClimate::setNYears(int nYears)
{
    _nYears = nYears;
}

QString Crit3DClimate::elab1() const
{
    return _elab1;
}

void Crit3DClimate::setElab1(const QString &elab1)
{
    _elab1 = elab1;
}

float Crit3DClimate::param1() const
{
    return _param1;
}

void Crit3DClimate::setParam1(float param1)
{
    _param1 = param1;
}

bool Crit3DClimate::param1IsClimate() const
{
    return _param1IsClimate;
}

void Crit3DClimate::setParam1IsClimate(bool param1IsClimate)
{
    _param1IsClimate = param1IsClimate;
}

QString Crit3DClimate::param1ClimateField() const
{
    return _param1ClimateField;
}

void Crit3DClimate::setParam1ClimateField(const QString &param1ClimateField)
{
    _param1ClimateField = param1ClimateField;
}

QString Crit3DClimate::elab2() const
{
    return _elab2;
}

void Crit3DClimate::setElab2(const QString &elab2)
{
    _elab2 = elab2;
}

float Crit3DClimate::param2() const
{
    return _param2;
}

void Crit3DClimate::setParam2(float param2)
{
    _param2 = param2;
}

period Crit3DClimate::periodType() const
{
    return _periodType;
}

void Crit3DClimate::setPeriodType(const period &periodType)
{
    _periodType = periodType;
}

QSqlDatabase Crit3DClimate::db() const
{
    return _db;
}

void Crit3DClimate::setDb(const QSqlDatabase &db)
{
    _db = db;
}

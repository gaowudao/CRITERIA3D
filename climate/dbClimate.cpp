#include "dbClimate.h"


QString Climate::climateElab() const
{
    return _climateElab;
}

void Climate::setClimateElab(const QString &climateElab)
{
    _climateElab = climateElab;
}

int Climate::yearStart() const
{
    return _yearStart;
}

void Climate::setYearStart(int yearStart)
{
    _yearStart = yearStart;
}

int Climate::yearEnd() const
{
    return _yearEnd;
}

void Climate::setYearEnd(int yearEnd)
{
    _yearEnd = yearEnd;
}

meteoVariable Climate::variable() const
{
    return _variable;
}

void Climate::setVariable(const meteoVariable &variable)
{
    _variable = variable;
}

QString Climate::genericPeriod() const
{
    return _genericPeriod;
}

void Climate::setGenericPeriod(const QString &genericPeriod)
{
    _genericPeriod = genericPeriod;
}

QDate Climate::genericPeriodDateStart() const
{
    return _genericPeriodDateStart;
}

void Climate::setGenericPeriodDateStart(const QDate &genericPeriodDateStart)
{
    _genericPeriodDateStart = genericPeriodDateStart;
}

QDate Climate::genericPeriodDateEnd() const
{
    return _genericPeriodDateEnd;
}

void Climate::setGenericPeriodDateEnd(const QDate &genericPeriodDateEnd)
{
    _genericPeriodDateEnd = genericPeriodDateEnd;
}

int Climate::nYears() const
{
    return _nYears;
}

void Climate::setNYears(int nYears)
{
    _nYears = nYears;
}

QString Climate::elab1() const
{
    return _elab1;
}

void Climate::setElab1(const QString &elab1)
{
    _elab1 = elab1;
}

float Climate::param1() const
{
    return _param1;
}

void Climate::setParam1(float param1)
{
    _param1 = param1;
}

bool Climate::param1IsClimate() const
{
    return _param1IsClimate;
}

void Climate::setParam1IsClimate(bool param1IsClimate)
{
    _param1IsClimate = param1IsClimate;
}

QString Climate::param1ClimateField() const
{
    return _param1ClimateField;
}

void Climate::setParam1ClimateField(const QString &param1ClimateField)
{
    _param1ClimateField = param1ClimateField;
}

QString Climate::elab2() const
{
    return _elab2;
}

void Climate::setElab2(const QString &elab2)
{
    _elab2 = elab2;
}

float Climate::param2() const
{
    return _param2;
}

void Climate::setParam2(float param2)
{
    _param2 = param2;
}

period Climate::periodType() const
{
    return _periodType;
}

void Climate::setPeriodType(const period &periodType)
{
    _periodType = periodType;
}

QSqlDatabase Climate::db() const
{
    return _db;
}

void Climate::setDb(const QSqlDatabase &db)
{
    _db = db;
}

#include <QString>
#include <QDate>
#include <QtSql>

#include "commonConstants.h"
#include "crit3dClimate.h"


Crit3DClimate::Crit3DClimate()
{
    _climateElab = "";
    _yearStart = NODATA;
    _yearEnd = NODATA;
    _periodType = noPeriodType;
    _variable = noMeteoVar;
    _periodStr = "";
    _genericPeriodDateStart.setDate(1800,1,1);
    _genericPeriodDateEnd.setDate(1800,1,1);
    _nYears = NODATA;
    _elab1 = "";
    _param1 = NODATA;
    _param1IsClimate = false;
    _param1ClimateField = "";
    _param1ClimateIndex = NODATA;
    _elab2 = "";
    _param2 = NODATA;

    _currentVar = noMeteoVar;
    _currentElab1 = "";
    _currentYearStart = NODATA;
    _currentYearEnd = NODATA;
    _currentPeriodType = noPeriodType;

    elabSettings = new Crit3DElaborationSettings();
    listElab = new Crit3DClimateList();
}

void Crit3DClimate::resetParam()
{
    _climateElab = "";
    _yearStart = NODATA;
    _yearEnd = NODATA;
    _periodType = noPeriodType;
    _variable = noMeteoVar;
    _periodStr = "";
    _genericPeriodDateStart.setDate(1800,1,1);
    _genericPeriodDateEnd.setDate(1800,1,1);
    _nYears = NODATA;
    _elab1 = "";
    _param1 = NODATA;
    _param1IsClimate = false;
    _param1ClimateField = "";
    _param1ClimateIndex = NODATA;
    _elab2 = "";
    _param2 = NODATA;

}

Crit3DClimate::~Crit3DClimate()
{
}

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

QString Crit3DClimate::periodStr() const
{
    return _periodStr;
}

void Crit3DClimate::setPeriodStr(const QString &periodStr)
{
    _periodStr = periodStr;
}

Crit3DElaborationSettings *Crit3DClimate::getElabSettings() const
{
    return elabSettings;
}

void Crit3DClimate::setElabSettings(Crit3DElaborationSettings *value)
{
    elabSettings = value;
}

meteoVariable Crit3DClimate::getCurrentVar() const
{
    return _currentVar;
}

void Crit3DClimate::setCurrentVar(const meteoVariable &currentVar)
{
    _currentVar = currentVar;
}

QString Crit3DClimate::getCurrentElab1() const
{
    return _currentElab1;
}

void Crit3DClimate::setCurrentElab1(const QString &currentElab1)
{
    _currentElab1 = currentElab1;
}

int Crit3DClimate::getCurrentYearStart() const
{
    return _currentYearStart;
}

void Crit3DClimate::setCurrentYearStart(int currentYearStart)
{
    _currentYearStart = currentYearStart;
}

int Crit3DClimate::getCurrentYearEnd() const
{
    return _currentYearEnd;
}

void Crit3DClimate::setCurrentYearEnd(int currentYearEnd)
{
    _currentYearEnd = currentYearEnd;
}

period Crit3DClimate::getCurrentPeriodType() const
{
    return _currentPeriodType;
}

void Crit3DClimate::setCurrentPeriodType(const period &currentPeriodType)
{
    _currentPeriodType = currentPeriodType;
}

Crit3DClimateList *Crit3DClimate::getListElab() const
{
    return listElab;
}

void Crit3DClimate::setListElab(Crit3DClimateList *value)
{
    listElab = value;
}

int Crit3DClimate::getParam1ClimateIndex() const
{
    return _param1ClimateIndex;
}

void Crit3DClimate::setParam1ClimateIndex(int param1ClimateIndex)
{
    _param1ClimateIndex = param1ClimateIndex;
}

Crit3DMeteoSettings *Crit3DClimate::getMeteoSettings() const
{
    return meteoSettings;
}

void Crit3DClimate::setMeteoSettings(Crit3DMeteoSettings *value)
{
    meteoSettings = value;
}

QSqlDatabase Crit3DClimate::db() const
{
    return _db;
}

void Crit3DClimate::setDb(const QSqlDatabase &db)
{
    _db = db;
}



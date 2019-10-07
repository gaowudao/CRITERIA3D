#include <QString>
#include <QDate>
#include "crit3dAnomalyList.h"
#include "commonConstants.h"
#include "climate.h"

Crit3DAnomalyList::Crit3DAnomalyList()
{

}

Crit3DAnomalyList::~Crit3DAnomalyList()
{

}

bool Crit3DAnomalyList::isMeteoGrid() const
{
    return _isMeteoGrid;
}

void Crit3DAnomalyList::setIsMeteoGrid(bool isMeteoGrid)
{
    _isMeteoGrid = isMeteoGrid;
}

QStringList Crit3DAnomalyList::listAnomaly() const
{
    return _listAll;
}

void Crit3DAnomalyList::setListAnomaly(const QStringList &listAnomaly)
{
    _listAll = listAnomaly;
}

void Crit3DAnomalyList::reset()
{
    _listElab1.clear();
    _listElab2.clear();
    _listDateStart.clear();
    _listDateEnd.clear();
    _listNYears.clear();
    _listParam1.clear();
    _listParam1ClimateField.clear();
    _listParam1IsClimate.clear();
    _listParam2.clear();
    _listPeriodStr.clear();
    _listPeriodType.clear();
    _listVariable.clear();
    _listYearEnd.clear();
    _listYearStart.clear();
}

std::vector<int> Crit3DAnomalyList::listYearStart() const
{
    return _listYearStart;
}

void Crit3DAnomalyList::setListYearStart(const std::vector<int> &listYearStart)
{
    _listYearStart = listYearStart;
}

void Crit3DAnomalyList::insertYearStart(int yearStart)
{
    _listYearStart.push_back(yearStart);
}

std::vector<int> Crit3DAnomalyList::listYearEnd() const
{
    return _listYearEnd;
}

void Crit3DAnomalyList::setListYearEnd(const std::vector<int> &listYearEnd)
{
    _listYearEnd = listYearEnd;
}

void Crit3DAnomalyList::insertYearEnd(int yearEnd)
{
    _listYearEnd.push_back(yearEnd);
}

std::vector<meteoVariable> Crit3DAnomalyList::listVariable() const
{
    return _listVariable;
}

void Crit3DAnomalyList::setListVariable(const std::vector<meteoVariable> &listVariable)
{
    _listVariable = listVariable;
}

void Crit3DAnomalyList::insertVariable(meteoVariable variable)
{
    _listVariable.push_back(variable);
}

std::vector<QString> Crit3DAnomalyList::listPeriodStr() const
{
    return _listPeriodStr;
}

void Crit3DAnomalyList::setListPeriodStr(const std::vector<QString> &listPeriodStr)
{
    _listPeriodStr = listPeriodStr;
}

void Crit3DAnomalyList::insertPeriodStr(QString period)
{
    _listPeriodStr.push_back(period);
}

std::vector<period> Crit3DAnomalyList::listPeriodType() const
{
    return _listPeriodType;
}

void Crit3DAnomalyList::setListPeriodType(const std::vector<period> &listPeriodType)
{
    _listPeriodType = listPeriodType;
}

void Crit3DAnomalyList::insertPeriodType(period period)
{
    _listPeriodType.push_back(period);
}

std::vector<QDate> Crit3DAnomalyList::listDateStart() const
{
    return _listDateStart;
}

void Crit3DAnomalyList::setListDateStart(const std::vector<QDate> &listDateStart)
{
    _listDateStart = listDateStart;
}

void Crit3DAnomalyList::insertDateStart(QDate dateStart)
{
    _listDateStart.push_back(dateStart);
}

std::vector<QDate> Crit3DAnomalyList::listDateEnd() const
{
    return _listDateEnd;
}

void Crit3DAnomalyList::setListDateEnd(const std::vector<QDate> &listDateEnd)
{
    _listDateEnd = listDateEnd;
}

void Crit3DAnomalyList::insertDateEnd(QDate dateEnd)
{
    _listDateEnd.push_back(dateEnd);
}

std::vector<int> Crit3DAnomalyList::listNYears() const
{
    return _listNYears;
}

void Crit3DAnomalyList::setListNYears(const std::vector<int> &listNYears)
{
    _listNYears = listNYears;
}

void Crit3DAnomalyList::insertNYears(int nYears)
{
    _listNYears.push_back(nYears);
}

std::vector<QString> Crit3DAnomalyList::listElab1() const
{
    return _listElab1;
}

void Crit3DAnomalyList::setListElab1(const std::vector<QString> &listElab1)
{
    _listElab1 = listElab1;
}

void Crit3DAnomalyList::insertElab1(QString elab1)
{
    _listElab1.push_back(elab1);
}

std::vector<float> Crit3DAnomalyList::listParam1() const
{
    return _listParam1;
}

void Crit3DAnomalyList::setListParam1(const std::vector<float> &listParam1)
{
    _listParam1 = listParam1;
}

void Crit3DAnomalyList::insertParam1(float param1)
{
    _listParam1.push_back(param1);
}

std::vector<bool> Crit3DAnomalyList::listParam1IsClimate() const
{
    return _listParam1IsClimate;
}

void Crit3DAnomalyList::setListParam1IsClimate(const std::vector<bool> &listParam1IsClimate)
{
    _listParam1IsClimate = listParam1IsClimate;
}

void Crit3DAnomalyList::insertParam1IsClimate(bool param1IsClimate)
{
    _listParam1IsClimate.push_back(param1IsClimate);
}

std::vector<QString> Crit3DAnomalyList::listParam1ClimateField() const
{
    return _listParam1ClimateField;
}

void Crit3DAnomalyList::setListParam1ClimateField(const std::vector<QString> &listParam1ClimateField)
{
    _listParam1ClimateField = listParam1ClimateField;
}

void Crit3DAnomalyList::insertParam1ClimateField(QString param1ClimateField)
{
    _listParam1ClimateField.push_back(param1ClimateField);
}

std::vector<QString> Crit3DAnomalyList::listElab2() const
{
    return _listElab2;
}

void Crit3DAnomalyList::setListElab2(const std::vector<QString> &listElab2)
{
    _listElab2 = listElab2;
}

void Crit3DAnomalyList::insertElab2(QString elab2)
{
    _listElab2.push_back(elab2);
}

std::vector<float> Crit3DAnomalyList::listParam2() const
{
    return _listParam2;
}

void Crit3DAnomalyList::setListParam2(const std::vector<float> &listParam2)
{
    _listParam2 = listParam2;
}

void Crit3DAnomalyList::insertParam2(float param2)
{
    _listParam2.push_back(param2);
}

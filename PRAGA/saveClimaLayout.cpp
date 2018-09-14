#include "saveClimaLayout.h"

QString SaveClimaLayout::getFirstYear() const
{
    return firstYear;
}

void SaveClimaLayout::setFirstYear(const QString &value)
{
    firstYear = value;
}

QString SaveClimaLayout::getLastYear() const
{
    return lastYear;
}

void SaveClimaLayout::setLastYear(const QString &value)
{
    lastYear = value;
}

QString SaveClimaLayout::getVariable() const
{
    return variable;
}

void SaveClimaLayout::setVariable(const QString &value)
{
    variable = value;
}

QString SaveClimaLayout::getPeriod() const
{
    return period;
}

void SaveClimaLayout::setPeriod(const QString &value)
{
    period = value;
}

QString SaveClimaLayout::getGenericPeriodStart() const
{
    return genericPeriodStart;
}

void SaveClimaLayout::setGenericPeriodStart(const QString &value)
{
    genericPeriodStart = value;
}

QString SaveClimaLayout::getGenericPeriodEnd() const
{
    return genericPeriodEnd;
}

void SaveClimaLayout::setGenericPeriodEnd(const QString &value)
{
    genericPeriodEnd = value;
}

QString SaveClimaLayout::getGenericNYear() const
{
    return genericNYear;
}

void SaveClimaLayout::setGenericNYear(const QString &value)
{
    genericNYear = value;
}

QString SaveClimaLayout::getSecondElab() const
{
    return secondElab;
}

void SaveClimaLayout::setSecondElab(const QString &value)
{
    secondElab = value;
}

QString SaveClimaLayout::getElab2Param() const
{
    return elab2Param;
}

void SaveClimaLayout::setElab2Param(const QString &value)
{
    elab2Param = value;
}

QString SaveClimaLayout::getElab() const
{
    return elab;
}

void SaveClimaLayout::setElab(const QString &value)
{
    elab = value;
}

QString SaveClimaLayout::getElab1Param() const
{
    return elab1Param;
}

void SaveClimaLayout::setElab1Param(const QString &value)
{
    elab1Param = value;
}


SaveClimaLayout::SaveClimaLayout()
{

    listLayout.addWidget(&listview);

    saveList.setText("Save list");
    loadList.setText("Load list");
    saveButtonLayout.addWidget(&saveList);
    saveButtonLayout.addWidget(&loadList);


    mainLayout.addLayout(&listLayout);
    mainLayout.addLayout(&saveButtonLayout);

    setLayout(&mainLayout);

}

void SaveClimaLayout::addElab()
{

    qInfo() << "firstYear " << firstYear;
    qInfo() << "lastYear " << lastYear;

    qInfo() << "variable " << variable;

    qInfo() << "period " << period;
    qInfo() << "genericPeriodStart " << genericPeriodStart;
    qInfo() << "genericPeriodEnd " << genericPeriodEnd;
    qInfo() << "genericPeriodNYear " << genericNYear;
    qInfo() << "secondElab " << secondElab;
    qInfo() << "elab2Param " << elab2Param;
    qInfo() << "elab " << elab;
    qInfo() << "elab1Param " << elab1Param;
}


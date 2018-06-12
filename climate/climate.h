#ifndef CLIMATE_H
#define CLIMATE_H


#include <QString>
#include <QDate>

bool elaborationPointsCycle(std::string *myError, bool pointOrGrid, QString variable, int firstYear, int lastYear, QDate firstDate, QDate lastDate, int nYears,
    QString elab1, bool param1IsClimate, QString param1ClimateField, float param1, QString elab2, float param2, bool isAnomaly,
    int nYearsMin, int firstYearClimate, int lastYearClimate);

#endif // CLIMATE_H

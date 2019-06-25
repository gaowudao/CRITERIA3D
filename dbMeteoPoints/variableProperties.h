#ifndef VARIABLEPROPERTIES_H
#define VARIABLEPROPERTIES_H

#ifndef METEOPOINT_H
    #include "meteo.h"
#endif
#include <QString>
#include <QtSql>

bool loadVariableProperties(QString *myError, QSqlDatabase db, std::map<int, meteoVariable> mapIdMeteoVar);
int getIdfromMeteoVar(meteoVariable meteoVar, std::map<int, meteoVariable> mapIdMeteoVar);

#endif // VARIABLEPROPERTIES_H

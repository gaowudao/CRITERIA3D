#ifndef DBCLIMATE_H
#define DBCLIMATE_H

#include <QtSql>

bool saveDailyElab(QSqlDatabase db, std::string *myError, QString id, int day, float result, QString elab);
bool saveDecadalElab(QSqlDatabase db, std::string *myError, QString id, int decades, float result, QString elab);
bool saveMonthlyElab(QSqlDatabase db, std::string *myError, QString id, int months, float result, QString elab);
bool saveSeasonalElab(QSqlDatabase db, std::string *myError, QString id, int season, float result, QString elab);
bool saveAnnualElab(QSqlDatabase db, std::string *myError, QString id, float result, QString elab);
bool saveGenericElab(QSqlDatabase db, std::string *myError, QString id, float result, QString elab);



#endif // DBCLIMATE_H

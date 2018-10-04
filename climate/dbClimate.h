#ifndef DBCLIMATE_H
#define DBCLIMATE_H

#include <QtSql>

bool writeDailyElab(QSqlDatabase db, std::string *myError, QString id, int day, float result, QString elab);
bool writeDecadalElab(QSqlDatabase db, std::string *myError, QString id, int decades, float result, QString elab);
bool writeMonthlyElab(QSqlDatabase db, std::string *myError, QString id, int months, float result, QString elab);
bool writeSeasonalElab(QSqlDatabase db, std::string *myError, QString id, int season, float result, QString elab);
bool writeAnnualElab(QSqlDatabase db, std::string *myError, QString id, float result, QString elab);
bool writeGenericElab(QSqlDatabase db, std::string *myError, QString id, float result, QString elab);



#endif // DBCLIMATE_H

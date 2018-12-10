#ifndef DBCLIMATE_H
#define DBCLIMATE_H

#include <QtSql>

bool saveDailyElab(QSqlDatabase db, std::string *myError, QString id, std::vector<float> allResults, QString elab);
bool saveDecadalElab(QSqlDatabase db, std::string *myError, QString id, std::vector<float> allResults, QString elab);
bool saveMonthlyElab(QSqlDatabase db, std::string *myError, QString id, std::vector<float> allResults, QString elab);
bool saveSeasonalElab(QSqlDatabase db, std::string *myError, QString id, std::vector<float> allResults, QString elab);
bool saveAnnualElab(QSqlDatabase db, std::string *myError, QString id, float result, QString elab);
bool saveGenericElab(QSqlDatabase db, std::string *myError, QString id, float result, QString elab);

bool selectAllElab(QSqlDatabase db, std::string *myError, QString table, QStringList* listElab);
bool selectVarElab(QSqlDatabase db, std::string *myError, QString table, QString variable, QStringList* listElab);
bool showClimateTables(QSqlDatabase db, std::string *myError, QStringList* climateTables);

bool deleteElab(QSqlDatabase db, std::string *myError, QString table, QString elab);

QList<float> readElab(QSqlDatabase db, QString table, std::string *myError, QString id, QString elab);
QList<QString> getIdListFromElab(QSqlDatabase db, QString table, std::string *myError, QString elab);
QList<QString> getIdListFromElab(QSqlDatabase db, QString table, std::string *myError, QString elab, int index);


#endif // DBCLIMATE_H

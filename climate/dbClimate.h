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

bool deleteDailyElab(QSqlDatabase db, std::string *myError, int day, QString elab);
bool deleteDecadalElab(QSqlDatabase db, std::string *myError, int decade, QString elab);
bool deleteMonthlyElab(QSqlDatabase db, std::string *myError, int month, QString elab);
bool deleteSeasonalElab(QSqlDatabase db, std::string *myError, int season, QString elab);
bool deleteAnnualElab(QSqlDatabase db, std::string *myError, QString elab);
bool deleteGenericElab(QSqlDatabase db, std::string *myError, QString elab);

QList<float> readElab(QSqlDatabase db, QString table, std::string *myError, QString id, QString elab);
QList<QString> getIdListFromElab(QSqlDatabase db, QString table, std::string *myError, QString elab);
QList<QString> getIdListFromElab(QSqlDatabase db, QString table, std::string *myError, QString elab, int index);


#endif // DBCLIMATE_H

#ifndef DBARKIMET_H
#define DBARKIMET_H

#include <QString>
#include "variableslist.h"
#include "dbMeteoPoints.h"

#define PREC_ID 250
#define RAD_ID 706

class DbArkimet : public DbMeteoPoints
{
    public:
        explicit DbArkimet(QString dbName);
        void dbManager();
        QString queryString;

        QString getVarName(int id);
        QList<int> getDailyVar();
        QList<int> getHourlyVar();
        int getId(QString VarName);
        QList<VariablesList> getHourlyVarFields(QList<int> id);
        void initStationsDailyTables(QDate startDate, QDate endDate, QStringList stations);
        void initStationsHourlyTables(QDate startDate, QDate endDate, QStringList stations);
        void createTmpTable();
        void deleteTmpTable();
        void insertDailyValue(QString station, QString date, int varType, double varValue, QString flag);
        void insertOrUpdate(QString date, QString id_point, int id_variable, QString variable_name, double value, int frequency, QString flag);
        bool saveHourlyData();
        int arkIdmap(int arkId);
        void appendQueryHourly(QString dateTime, QString idPoint, QString idVariable, QString varName, QString value, QString frequency, bool isFirstData);
    signals:

    protected slots:

};


#endif // DBARKIMET_H

#ifndef DBARKIMET_H
#define DBARKIMET_H

#include <QString>
#include <QtSql>
#include "variableslist.h"
#include "dbMeteoPoints.h"
#include "meteoPoint.h"


#define PREC_ID 250
#define RAD_ID 706
#define DAILY_TO_INTEGRAL_RAD 86400

class DbArkimet : public DbMeteoPoints
{

    public:
        explicit DbArkimet(QString dbName);
        ~DbArkimet();
        void dbManager();

        QString getVarName(int id);
        QList<int> getDailyVar();
        QList<int> getHourlyVar();
        int getId(QString VarName);
        bool fillPointProperties(Crit3DMeteoPoint* pointProp);
        QList<Crit3DMeteoPoint> getPropertiesFromDb();
        QList<VariablesList> getHourlyVarFields(QList<int> id);
        void initStationsDailyTables(Crit3DDate dataStartInput, Crit3DDate dataEndInput, QStringList stations);
        void initStationsHourlyTables(Crit3DTime dataStartInput, Crit3DTime dataEndInput, QStringList stations);
        void createTmpTable();
        void deleteTmpTable();
        void insertDailyValue(QString station, QString date, int varType, double varValue, QString flag);
        void insertOrUpdate(QString date, QString id_point, int id_variable, QString variable_name, double value, int frequency, QString flag);
        void saveHourlyData();
        int arkIdmap(int arkId);
    private:
        QSqlDatabase _db;
    signals:

    protected slots:

};


#endif // DBARKIMET_H

#ifndef DBARKIMET_H
#define DBARKIMET_H

#include <QString>
#include <QtSql>
#include "variableslist.h"
#include "dbMeteoPoints.h"


struct TPointProperties {
    int id;
    QString name;
    double lat;
    double lon;
    double utm_x;
    double utm_y;
    float altitude;

    int latInt;
    int lonInt;
    QString network;
    QString state;
    QString region;
    QString province;
    QString municipality;
};

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
        bool fillPointProperties(TPointProperties* pointProp);
        QList<VariablesList> getHourlyVarFields(QList<int> id);
        void initStationsDailyTables(Crit3DDate dataStartInput, Crit3DDate dataEndInput, QList<int> stations);
        void initStationsHourlyTables(Crit3DTime dataStartInput, Crit3DTime dataEndInput, QList<int> stations);
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

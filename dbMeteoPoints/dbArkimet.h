#ifndef DBARKIMET_H
#define DBARKIMET_H

#include <QString>
#include <QtSql>
#include "variableslist.h"
#include "dbMeteoPoints.h"
#ifndef METEOPOINT_H
    #include "meteoPoint.h"
#endif


#define PREC_ID 250
#define RAD_ID 706
#define DAILY_TO_INTEGRAL_RAD 86400

class DbArkimet : public DbMeteoPoints
{

    public:
        explicit DbArkimet(QString dbName);
        void dbManager();

        QString getVarName(int id);
        QList<int> getDailyVar();
        QList<int> getHourlyVar();
        int getId(QString VarName);
        bool fillPointProperties(Crit3DMeteoPoint* pointProp);
        QList<Crit3DMeteoPoint> getPropertiesFromDb();
        void getDataFromDailyDb(Crit3DDate dateStart, Crit3DDate dateEnd, QList<Crit3DMeteoPoint> &meteoPointsList);
        void getDataFromHourlyDb(Crit3DDate dateStart, Crit3DDate dateEnd, QList<Crit3DMeteoPoint> &meteoPointsList);
        QList<VariablesList> getHourlyVarFields(QList<int> id);
        void initStationsDailyTables(Crit3DDate dateStartInput, Crit3DDate dateEndInput, QStringList stations);
        void initStationsHourlyTables(Crit3DTime dateStartInput, Crit3DTime dateEndInput, QStringList stations);
        void createTmpTable();
        void deleteTmpTable();
        void insertDailyValue(QString station, QString date, int varType, double varValue, QString flag);
        void insertOrUpdate(QString date, QString id_point, int id_variable, QString variable_name, double value, int frequency, QString flag);
        void saveHourlyData();
        int arkIdmap(int arkId);
    signals:

    protected slots:

};


#endif // DBARKIMET_H

#ifndef DBAGGREGATIONSHANDLER_H
#define DBAGGREGATIONSHANDLER_H

#include <QString>
#include <QtSql>
#ifndef METEOPOINT_H
    #include "meteo.h"
#endif
#ifndef VARIABLEPROPERTIES_H
    #include "variableProperties.h"
#endif

class Crit3DAggregationsDbHandler
{
public:
    Crit3DAggregationsDbHandler(QString dbname);
    ~Crit3DAggregationsDbHandler();
    QSqlDatabase db() const;
    QString error() const;

    bool writeAggregationZonesTable(QString name, QString filename, QString field);
    bool getAggregationZonesReference(QString name, QString* filename, QString* field);
    void initAggregatedTables(int numZones, QString aggrType, QString periodType, QDateTime startDate, QDateTime endDate);
    void createTmpAggrTable();
    void deleteTmpAggrTable();
    bool insertTmpAggr(QDateTime startDate, QDateTime endDate, int idVariable, std::vector< std::vector<float> > aggregatedValues, int nZones);

private:
    QSqlDatabase _db;
    std::map<int, meteoVariable> _mapIdMeteoVar;
    QString _error;
};

#endif // DBAGGREGATIONSHANDLER_H




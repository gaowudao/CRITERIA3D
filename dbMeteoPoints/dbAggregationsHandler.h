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

private:
    QSqlDatabase _db;
    std::map<int, meteoVariable> _mapIdMeteoVar;
    QString _error;
};

#endif // DBAGGREGATIONSHANDLER_H




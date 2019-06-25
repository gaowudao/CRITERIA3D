#include "dbAggregationsHandler.h"
#include "commonConstants.h"
#include "utilities.h"

Crit3DAggregationsDbHandler::Crit3DAggregationsDbHandler(QString dbname)
{
    _error = "";
    _mapIdMeteoVar.clear();

    if(_db.isOpen())
    {
        qDebug() << _db.connectionName() << "is already open";
        _db.close();
    }

    _db = QSqlDatabase::addDatabase("QSQLITE", QUuid::createUuid().toString());
    _db.setDatabaseName(dbname);

    if (!_db.open())
       _error = _db.lastError().text();
}

Crit3DAggregationsDbHandler::~Crit3DAggregationsDbHandler()
{
    if ((_db.isValid()) && (_db.isOpen()))
    {
        _db.close();
        _db.removeDatabase(_db.connectionName());
    }
}

QSqlDatabase Crit3DAggregationsDbHandler::db() const
{
    return _db;
}


QString Crit3DAggregationsDbHandler::error() const
{
    return _error;
}

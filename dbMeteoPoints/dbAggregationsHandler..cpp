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

bool Crit3DAggregationsDbHandler::writeAggregationZonesTable(QString name, QString filename, QString field)
{

    QSqlQuery qry(_db);

    qry.prepare( "INSERT INTO aggregation_zones (name, filename, shape_field)"
                                      " VALUES (:name, :filename, :shape_field)" );

    qry.bindValue(":name", name);
    qry.bindValue(":filename", filename);
    qry.bindValue(":shape_field", field);

    if( !qry.exec() )
    {
        _error = qry.lastError().text();
        return false;
    }
    else
        return true;

}

bool Crit3DAggregationsDbHandler::getAggregationZonesReference(QString name, QString* filename, QString* field)
{

    QSqlQuery qry(_db);

    qry.prepare( "SELECT * FROM aggregation_zones WHERE name = :name");
    qry.bindValue(":name", name);

    if( !qry.exec() )
    {
        _error = qry.lastError().text();
        return false;
    }
    else
    {
        if (qry.next())
        {
            getValue(qry.value("filename"), filename);
            getValue(qry.value("shape_field"), field);
            return true;
        }
        else
        {
            _error = "name not found";
            return false;
        }
    }
}

void Crit3DAggregationsDbHandler::initAggregatedTables(int numZones, QString aggrType, QString periodType, QDate startDate, QDate endDate)
{

    for (int i = 0; i < numZones; i++)
    {
        QString statement = QString("CREATE TABLE IF NOT EXISTS `%1_%2_%3` "
                                    "(date_time TEXT, id_variable INTEGER, value REAL, PRIMARY KEY(date_time,id_variable))").arg(i).arg(aggrType).arg(periodType);

        QSqlQuery qry(statement, _db);
        if( !qry.exec() )
        {
            _error = qry.lastError().text();
        }
        statement = QString("DELETE FROM `%1_%2_%3` WHERE date_time >= DATE('%2') AND date_time < DATE('%3', '+1 day')")
                        .arg(i).arg(aggrType).arg(periodType).arg(startDate.toString("yyyy-MM-dd")).arg(endDate.toString("yyyy-MM-dd"));

        qry = QSqlQuery(statement, _db);
        if( !qry.exec() )
        {
            _error = qry.lastError().text();
        }
    }

}

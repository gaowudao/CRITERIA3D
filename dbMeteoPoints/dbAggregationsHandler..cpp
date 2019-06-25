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

void Crit3DAggregationsDbHandler::initAggregatedTables(int numZones, QString aggrType, QString periodType, QDateTime startDate, QDateTime endDate)
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
                        .arg(i).arg(aggrType).arg(periodType).arg(startDate.toString("yyyy-MM-dd hh:mm:ss")).arg(endDate.toString("yyyy-MM-dd hh:mm:ss"));

        qry = QSqlQuery(statement, _db);
        if( !qry.exec() )
        {
            _error = qry.lastError().text();
        }
    }

}

void Crit3DAggregationsDbHandler::createTmpAggrTable()
{
    this->deleteTmpAggrTable();

    QSqlQuery qry(_db);
    qry.prepare("CREATE TABLE TmpAggregationData (date_time TEXT, zone TEXT, id_variable INTEGER, value REAL)");
    if( !qry.exec() )
    {
        qDebug() << qry.lastError();
    }
}

void Crit3DAggregationsDbHandler::deleteTmpAggrTable()
{
    QSqlQuery qry(_db);

    qry.prepare( "DROP TABLE TmpAggregationData" );

    qry.exec();
}


bool Crit3DAggregationsDbHandler::insertTmpAggr(QDateTime startDate, QDateTime endDate, int idVariable, std::vector< std::vector<float> > aggregatedValues, int nZones)
{

    int nrDays = int(startDate.daysTo(endDate) + 1);
    QSqlQuery qry(_db);
    qry.prepare( "INSERT INTO `TmpAggregationData` (date_time, zone, id_variable, value)"
                                          " VALUES (?, ?, ?, ?)" );
    //QString dateTime;
    QVariantList dateTimeList;
    QVariantList zoneList;
    QVariantList idVariableList;
    QVariantList valueList;


    for (int day = 0; day < nrDays; day++)
    {

        // LC NB le zone partono da 1, a 0 è NODATA
        for (int zone = 1; zone < nZones; zone++)
        {
            float value = aggregatedValues[day][zone];
            if (value != NODATA)
            {
                dateTimeList << (startDate.addDays(day)).toString("yyyy-MM-dd hh:mm:ss");
                zoneList << zone;
                idVariableList << idVariable;
                valueList << value;

            }
        }
    }

    qry.addBindValue(dateTimeList);
    qry.addBindValue(zoneList);
    qry.addBindValue(idVariableList);
    qry.addBindValue(valueList);

    if( !qry.execBatch() )
    {
        _error = qry.lastError().text();
        return false;
    }
    else
        return true;

}

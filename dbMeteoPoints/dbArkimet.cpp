#include "commonConstants.h"
#include "dbArkimet.h"

#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QString>


DbArkimet::DbArkimet(QString dbName) : DbMeteoPoints(dbName)
{
}

QList<VariablesList> DbArkimet::getHourlyVarFields(QList<int> id)
{
    QList<VariablesList> variableList;

    QString idlist = QString("(%1").arg(id[0]);

    for (int i = 1; i < id.size(); i++)
    {
        idlist = idlist % QString(",%1").arg(id[i]);
    }
    idlist = idlist % QString(")");

    QString statement = QString("SELECT * FROM variable_properties WHERE id_arkimet IN %1").arg(idlist);

    QSqlQuery qry(statement, _db);

    if( !qry.exec() )
        qDebug() << qry.lastError();
    else
    {
        while (qry.next())
        {
            variableList.append(VariablesList(qry.value(0).toInt(), qry.value(1).toInt(), qry.value(2).toString(), qry.value(5).toInt() ));
        }
    }

    return variableList;
}


QString DbArkimet::getVarName(int id)
{
    QString varName = NULL;
    QSqlQuery qry(_db);

    qry.prepare( "SELECT variable FROM variable_properties WHERE id_arkimet = :id" );
    qry.bindValue(":id", id);

    if( !qry.exec() )
    {
        qDebug() << qry.lastError();
    }
    else
    {
        qDebug( "getVarName Selected!" );

        if (qry.next())
            varName = qry.value(0).toString();

        else
            qDebug( "Error: dataset not found" );
    }

    return varName;
}


int DbArkimet::getId(QString VarName)
{
    int id = 0;
    QSqlQuery qry(_db);

    qry.prepare( "SELECT id_arkimet FROM variable_properties WHERE variable = :VarName" );
    qry.bindValue(":VarName", VarName);

    if( !qry.exec() )
    {
        qDebug() << qry.lastError();
    }
    else
    {
        if (qry.next())
            id = qry.value(0).toInt();

        else
            qDebug( "Error: dataset not found" );
    }

    return id;
}


QList<int> DbArkimet::getDailyVar()
{
    QList<int> dailyVarList;
    QSqlQuery qry(_db);

    qry.prepare( "SELECT id_arkimet FROM variable_properties WHERE aggregation_time = 86400" );

    if( !qry.exec() )
    {
        qDebug() << qry.lastError();
    }
    else
    {
        qDebug( "getDailyVar Selected!" );

        while (qry.next())
        {
            int id = qry.value(0).toInt();
            dailyVarList << id;
        }
    }

    return dailyVarList;
}


QList<int> DbArkimet::getHourlyVar()
{
    QList<int> hourlyVarList;
    QSqlQuery qry(_db);

    qry.prepare( "SELECT id_arkimet FROM variable_properties WHERE aggregation_time < 86400" );

    if( !qry.exec() )
    {
        qDebug() << qry.lastError();
    }
    else
    {
        while (qry.next())
        {
            int id = qry.value(0).toInt();
            hourlyVarList << id;
        }
    }

    return hourlyVarList;
}



void DbArkimet::initStationsDailyTables(QDate startDate, QDate endDate, QStringList stations)
{

    for (int i = 0; i < stations.size(); i++)
    {
        QString statement = QString("CREATE TABLE IF NOT EXISTS `%1_D` (date_time TEXT, id_variable INTEGER, value REAL, PRIMARY KEY(date_time,id_variable))").arg(stations[i]);
        //qDebug() << "initStationsDailyTables - Create " << statement;

        QSqlQuery qry(statement, _db);
        qry.exec();

        statement = QString("DELETE FROM `%1_D` WHERE date_time >= DATE('%2') AND date_time < DATE('%3', '+1 day')")
                        .arg(stations[i]).arg(startDate.toString("yyyy-MM-dd")).arg(endDate.toString("yyyy-MM-dd"));

        qry = QSqlQuery(statement, _db);
        qry.exec();
    }

}


void DbArkimet::initStationsHourlyTables(QDate startDate, QDate endDate, QStringList stations)
{
    for (int i = 0; i < stations.size(); i++)
    {
        QString statement = QString("CREATE TABLE IF NOT EXISTS `%1_H` (date_time TEXT, id_variable INTEGER, value REAL, PRIMARY KEY(date_time,id_variable))").arg(stations[i]);

        QSqlQuery qry(statement, _db);
        qry.exec();

        statement = QString("DELETE FROM `%1_H` WHERE date_time >= DATE('%2') AND date_time < DATE('%3', '+1 day')")
                        .arg(stations[i]).arg(startDate.toString("yyyy-MM-dd")).arg(endDate.toString("yyyy-MM-dd"));

        qry = QSqlQuery(statement, _db);
        qry.exec();
    }
}


void DbArkimet::createTmpTable()
{
    this->deleteTmpTable();

    QSqlQuery qry(_db);
    qry.prepare("CREATE TABLE TmpHourlyData (date_time TEXT, id_point INTEGER, id_variable INTEGER, variable_name TEXT, value REAL, frequency INTEGER, PRIMARY KEY(date_time,id_point,variable_name))");
    if( !qry.exec() )
    {
        qDebug() << qry.lastError();
    }
}


void DbArkimet::deleteTmpTable()
{
    QSqlQuery qry(_db);

    qry.prepare( "DROP TABLE TmpHourlyData" );

    if( !qry.exec() )
    {
        //qDebug() << "DROP TABLE TmpHourlyData" << qry.lastError();
    }
}


void DbArkimet::insertDailyValue(QString station, QString date, int varType, double varValue, QString flag)
{
    if (flag.left(1) == "1" || flag.left(1) == "054") {
        // invalid data
        varValue = NODATA;
    }

    if (varValue !=NODATA)
    {
        double valueRound = static_cast<double>(static_cast<int>(varValue*10+0.5))/10.0;
        QString statement = QString("INSERT INTO `%1_D` VALUES('%2', '%3', '%4')").arg(station).arg(date).arg(varType).arg(valueRound);

        QSqlQuery qry = QSqlQuery(statement, _db);
        qry.exec();
    }

}


void DbArkimet::saveHourlyData()
{
    QStringList stations;

    QString statement = QString("SELECT DISTINCT id_point FROM TmpHourlyData");
    QSqlQuery qry = QSqlQuery(statement, _db);
    qry.exec();

    while (qry.next())
    {
        stations.append(qry.value(0).toString());
    }

    statement = QString("INSERT INTO `%1_H` ");
    statement = statement % "SELECT date_time, id_variable, value FROM TmpHourlyData WHERE id_point = %1 AND frequency = 3600";

    foreach (QString station, stations) {
        qry = QSqlQuery(statement.arg(station), _db);
        qry.exec();
    }

    statement = QString("DELETE FROM TmpHourlyData WHERE frequency = 3600");
    qry = QSqlQuery(statement, _db);
    qry.exec();

    statement = QString("INSERT INTO `%1_H` ");
    statement = statement % "SELECT date_time, id_variable, value FROM TmpHourlyData WHERE ";
    statement = statement % "id_point = %1 AND variable_name = 'W_DIR' AND strftime('%M', date_time) = '00'";

    foreach (QString station, stations) {
        qry = QSqlQuery(statement.arg(station), _db);
        qry.exec();
    }

    //rad:TO DO prevailing HH:30 data
    statement = QString("INSERT INTO `%1_H` ");
    statement = statement % "SELECT DATETIME(date_time, '+30 minutes'), id_variable, value FROM TmpHourlyData WHERE ";
    statement = statement % "id_point = %1 AND variable_name = 'RAD' AND strftime('%M', date_time) = '30'";

    foreach (QString station, stations) {
        qry = QSqlQuery(statement.arg(station), _db);
        qry.exec();
    }

    statement = QString("DELETE FROM TmpHourlyData WHERE variable_name IN ('RAD', 'W_DIR')");
    qry = QSqlQuery(statement, _db);
    qry.exec();

    // re-query stations
    stations.clear();
    statement = QString("SELECT DISTINCT id_point FROM TmpHourlyData");
    qry = QSqlQuery(statement, _db);
    qry.exec();
    while (qry.next())
    {
        stations.append(qry.value(0).toString());
    }

    // la media funziona su tutte le var che hanno AVG nel nome (Temperature, RH, Wind intensity)
    // i minuti del campo date_time vengono portati a 60 dalla funzione datetime quindi tutto si uniforma all'ora di riferimento
    statement = QString("INSERT INTO `%1_H` ");
    statement = statement % "SELECT aggregate_date, id_variable, aggregate_value FROM (";
    statement = statement % "SELECT aggregate_date, id_variable, SUM(value) AS aggregate_value, SUM(frequency) AS aggregate_frequency FROM (";
    statement = statement % "SELECT datetime(date_time, '+' || (SELECT CASE WHEN strftime('%M', date_time) = '00' THEN 0 ELSE 60 - strftime('%M', date_time) END) || ' minutes') AS aggregate_date, ";
    statement = statement % "id_variable, value, frequency FROM TmpHourlyData WHERE id_point = %1 AND variable_name like '%PREC%'";
    statement = statement % ") group by aggregate_date, id_variable) WHERE aggregate_frequency = 3600";
    statement = statement % " UNION ALL ";
    statement = statement % "SELECT aggregate_date, id_variable, aggregate_value FROM (";
    statement = statement % "SELECT aggregate_date, id_variable, AVG(value) AS aggregate_value FROM (";
    statement = statement % "SELECT datetime(date_time, '+' || (SELECT CASE WHEN strftime('%M', date_time) = '00' THEN 0 ELSE 60 - strftime('%M', date_time) END) || ' minutes') AS aggregate_date, ";
    statement = statement % "id_variable, value FROM TmpHourlyData WHERE id_point = %1 AND variable_name like '%AVG%'";
    statement = statement % ") group by aggregate_date, id_variable)";

    QString delStationStatement = QString("DELETE FROM TmpHourlyData WHERE id_point = :id_point");

    foreach (QString station, stations) {

        qry = QSqlQuery(statement.arg(station), _db);
        if( !qry.exec() )
        {
            qDebug() << "error in hourly insert " << station << qry.lastError();
        }

        qry = QSqlQuery(delStationStatement, _db);
        qry.bindValue(":id_point", station);
        if( !qry.exec() )
        {
            qDebug() << "error in delete " << station << qry.lastError();
        }
    }
}


void DbArkimet::insertOrUpdate(QString date, QString id_point, int id_variable, QString variable_name, double value, int frequency, QString flag)
{
    if (flag.left(1) == "1" || flag.left(1) == "054") {
        // invalid data
        value = NODATA;
    }

    if (value !=NODATA)
    {
        double valueRound = static_cast<double>(static_cast<int>(value*10+0.5))/10.0;
        QString statement = QString("REPLACE INTO TmpHourlyData SELECT '%1', %2, %3, '%4', %5, %6 WHERE %6 > (SELECT COALESCE((");
        statement = statement % "SELECT frequency FROM TmpHourlyData WHERE date_time = '%1' AND id_point = %2 AND variable_name = '%4'), 0))";
        statement = statement.arg(date).arg(id_point).arg(id_variable).arg(variable_name).arg(valueRound).arg(frequency);

        QSqlQuery qry = QSqlQuery(statement, _db);
        qry.exec();
    }
}


int DbArkimet::arkIdmap(int arkId)
{
    switch(arkId)
    {
        case 158:
            return 51;
        case 159:
            return 52;
        case 139:
            return 53;
        case 164:
            return 54;
        case 166:
            return 55;
        case 165:
            return 56;
        case 78:
            return 101;
        case 160:
            return 102;
        case 140:
            return 103;
        case 409:
            return 104;
        case 69:
            return 105;
        case 431:
            return 106;
        case 232:
            return 151;
        case 233:
            return 152;
        case 231:
            return 153;
        case 250:
            return 154;
        case 241:
            return 155;
        case 242:
            return 156;
        case 240:
            return 157;
        case 706:
            return 158;
        case 227:
            return 159;
        case 230:
            return 160;
        default:
            qDebug() << "value of arkId unknown";
    }
    return NODATA;
}

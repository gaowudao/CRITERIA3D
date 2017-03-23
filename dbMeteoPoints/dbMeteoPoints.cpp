#include "dbMeteoPoints.h"
#include "commonConstants.h"

#include <QDebug>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>

#include <QString>
#include <QtNetwork>

DbMeteoPoints::DbMeteoPoints(QString dbName)
{
    _db = QSqlDatabase::addDatabase("QSQLITE");
    _db.setDatabaseName(dbName);

    if (!_db.open())
    {
       qDebug() << "Error: connection with database fail";
    }
    else
    {
       qDebug() << "Database: connection ok";
    }
}

DbMeteoPoints::~DbMeteoPoints()
{
    _db.close();
}

QString DbMeteoPoints::getDatasetURL(QString dataset)
{

    QSqlQuery qry(_db);
    QString url = NULL;

    qry.prepare( "SELECT URL FROM datasets WHERE dataset = :dataset");
    qry.bindValue(":dataset", dataset);

    if( !qry.exec() )
        qDebug() << qry.lastError();
    else
    {
        qDebug( "Selected!" );

        if (qry.next())
            url = qry.value(0).toString();

        else
            qDebug( "Error: dataset not found" );

    }

    return url;

}

QStringList DbMeteoPoints::getDatasetsActive()
{
    QStringList activeList;
    QSqlQuery qry(_db);

    qry.prepare( "SELECT dataset FROM datasets WHERE active = 1" );

    if( !qry.exec() )
        qDebug() << qry.lastError();
    else
    {
        qDebug( "Selected!" );

        while (qry.next())
        {
            QString active = qry.value(0).toString();
            activeList << active;

        }

    }

    return activeList;

}

QString DbMeteoPoints::getVarName(int id)
{

    QString varName = NULL;
    QSqlQuery qry(_db);

    qry.prepare( "SELECT variable FROM variable_properties WHERE id_arkimet = :id" );
    qry.bindValue(":id", id);

    if( !qry.exec() )
        qDebug() << qry.lastError();
    else
    {
        qDebug( "Selected!" );

        if (qry.next())
            varName = qry.value(0).toString();

        else
            qDebug( "Error: dataset not found" );

    }

    return varName;
}


int DbMeteoPoints::getId(QString VarName)
{

    int id = 0;
    QSqlQuery qry(_db);

    qry.prepare( "SELECT id_arkimet FROM variable_properties WHERE variable = :VarName" );
    qry.bindValue(":VarName", VarName);

    if( !qry.exec() )
        qDebug() << qry.lastError();
    else
    {
        qDebug( "Selected!" );

        if (qry.next())
            id = qry.value(0).toInt();

        else
            qDebug( "Error: dataset not found" );

    }

    return id;
}

QList<int> DbMeteoPoints::getDailyVar()
{
    QList<int> dailyVarList;
    QSqlQuery qry(_db);

    qry.prepare( "SELECT id_arkimet FROM variable_properties WHERE aggregation_time = 86400" );

    if( !qry.exec() )
        qDebug() << qry.lastError();
    else
    {
        qDebug( "Selected!" );

        while (qry.next())
        {
            int id = qry.value(0).toInt();
            dailyVarList << id;

        }

    }

    return dailyVarList;

}

QList<int> DbMeteoPoints::getHourlyVar()
{
    QList<int> hourlyVarList;
    QSqlQuery qry(_db);

    qry.prepare( "SELECT id_arkimet FROM variable_properties WHERE aggregation_time < 86400" );

    if( !qry.exec() )
        qDebug() << qry.lastError();
    else
    {
        qDebug( "Selected!" );

        while (qry.next())
        {
            int id = qry.value(0).toInt();
            hourlyVarList << id;

        }

    }

    return hourlyVarList;

}

bool DbMeteoPoints::fillPointProperties(TPointProperties* pointProp)
{

//    ////////////debug//////
//    qDebug() << "pointProp->id" << pointProp->id << endl;
//    qDebug() << "pointProp->name" << pointProp->name << endl;
//    qDebug() << "pointProp->network" << pointProp->network << endl;
//    qDebug() << "pointProp->lat" << pointProp->lat << endl;
//    qDebug() << "pointProp->lon" << pointProp->lon << endl;
//    qDebug() << "pointProp->latInt" << pointProp->latInt << endl;
//    qDebug() << "pointProp->lonInt" << pointProp->lonInt << endl;
//    qDebug() << "pointProp->utm_x" << pointProp->utm_x << endl;
//    qDebug() << "pointProp->utm_y" << pointProp->utm_y << endl;
//    qDebug() << "pointProp->altitude" << pointProp->altitude << endl;
//    qDebug() << "pointProp->state" << pointProp->state << endl;
//    qDebug() << "pointProp->region" << pointProp->region << endl;
//    qDebug() << "pointProp->province" << pointProp->province << endl;
//    qDebug() << "pointProp->municipality" << pointProp->municipality << endl;
//    ///
    bool success = false;
    QSqlQuery qry(_db);

    qry.prepare( "INSERT INTO point_properties (id_point, name, network, latitude, longitude, latInt, lonInt, utm_x, utm_y, altitude, state, region, province, municipality)"
                                      " VALUES (:id_point, :name, :network, :latitude, :longitude, :latInt, :lonInt, :utm_x, :utm_y, :altitude, :state, :region, :province, :municipality)" );


    qry.bindValue(":id_point", pointProp->id);
    qry.bindValue(":name", pointProp->name);
    qry.bindValue(":network", pointProp->network);
    qry.bindValue(":latitude", pointProp->lat);
    qry.bindValue(":longitude", pointProp->lon);
    qry.bindValue(":latInt", pointProp->latInt);
    qry.bindValue(":lonInt", pointProp->lonInt);
    qry.bindValue(":utm_x", pointProp->utm_x);
    qry.bindValue(":utm_y", pointProp->utm_y);
    qry.bindValue(":altitude", pointProp->altitude);
    qry.bindValue(":state", pointProp->state);
    qry.bindValue(":region", pointProp->region);
    qry.bindValue(":province", pointProp->province);
    qry.bindValue(":municipality", pointProp->municipality);


    if( !qry.exec() )
    {
        success = false;
        qDebug() << qry.lastError();
    }
    else
    {
        qDebug( "successfully inserted" );
        success = true;
    }
    //qDebug() << qry.lastQuery();

    return success;

}

void DbMeteoPoints::initStationsDailyTables(Crit3DDate dataStartInput, Crit3DDate dataEndInput, QList<int> stations)
{
    QString startDate = QString::fromStdString(dataStartInput.toStdString());
    QString endDate = QString::fromStdString(dataEndInput.toStdString());

    foreach(int station, stations)
    {
        QString statement = QString("CREATE TABLE IF NOT EXISTS `%1_D` (date_time TEXT, id_variable INTEGER, value REAL, PRIMARY KEY(date_time,id_variable))").arg(station);
        qDebug() << "initStationsDailyTables - Create " << statement;

        QSqlQuery qry(statement, _db);
        qry.exec();

        statement = QString("DELETE FROM `%1_D` WHERE date_time >= DATE('%2') AND date_time < DATE('%3', '+1 day')").arg(station).arg(startDate).arg(endDate);
        qDebug() << "initStationsDailyTables - Delete " << statement;

        qry = QSqlQuery(statement, _db);
        qry.exec();
    }

}

void DbMeteoPoints::initStationsHourlyTables(Crit3DTime dataStartInput, Crit3DTime dataEndInput, QList<int> stations)
{
    QString startDate = QString::fromStdString(dataStartInput.toStdString());
    QString endDate = QString::fromStdString(dataEndInput.toStdString());

    foreach(int station, stations)
    {
        QString statement = QString("CREATE TABLE IF NOT EXISTS `%1_H` (date_time TEXT, id_variable INTEGER, value REAL, PRIMARY KEY(date_time,id_variable))").arg(station);
        qDebug() << "initStationsHourlyTables - Create " << statement;

        QSqlQuery qry(statement, _db);
        qry.exec();

        statement = QString("DELETE FROM `%1_H` WHERE date_time >= DATE('%2') AND date_time < DATE('%3', '+1 day')").arg(station).arg(startDate).arg(endDate);
        qDebug() << "initStationsHourlyTables - Delete " << statement;

        qry = QSqlQuery(statement, _db);
        qry.exec();
    }

}

void DbMeteoPoints::createTmpTable(QString tmpTable)
{
    QString statement = QString("CREATE TABLE IF NOT EXISTS `%1` (date_time TEXT, id_point INTEGER, id_variable INTEGER, variable_name TEXT, value REAL, frequency INTEGER, PRIMARY KEY(date_time,id_variable))").arg(tmpTable);
    qDebug() << "createTmpTable - Create " << statement;

    QSqlQuery qry(statement, _db);
    qry.exec();

    statement = QString("DELETE FROM `%1`").arg(tmpTable);
    qDebug() << "createTmpTable - Delete all records" << statement;

    qry = QSqlQuery(statement, _db);
    qry.exec();


}

void DbMeteoPoints::insertVarValue(QString station, QString date, int varType, double varValue, QString flag)
{
    if (flag.left(1) == "1" || flag.left(1) == "054") {
        // dato invalidato o non plausibile
        varValue = NODATA;
    }

    QString statement = QString("INSERT INTO `%1_D` VALUES('%2', '%3', '%4')").arg(station).arg(date).arg(varType).arg(varValue);

    QSqlQuery qry = QSqlQuery(statement, _db);
    qry.exec();

}

int DbMeteoPoints::arkIdmap(int arkId)
{

    switch(arkId)
    {
        case 139:
            return 51;
        case 158:
            return 52;
        case 159:
            return 53;
        case 164:
            return 54;
        case 165:
            return 55;
        case 166:
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

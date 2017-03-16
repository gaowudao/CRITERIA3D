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

    QSqlQuery qry;
    QString url = NULL;

    qry.prepare( "SELECT URL FROM datasets WHERE dataset = :dataset" );
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
    QSqlQuery qry;

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
    QSqlQuery qry;

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
    QSqlQuery qry;

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
    QSqlQuery qry;

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
    QSqlQuery qry;

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
    QSqlQuery qry;

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

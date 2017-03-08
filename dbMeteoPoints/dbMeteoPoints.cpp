#include "dbMeteoPoints.h"
#include "commonConstants.h"

#include <QtSql>
#include <QDebug>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>

#include <QString>
#include <QtNetwork>

QString getDatasetURL(QString dbName, QString dataset)
{

    QSqlDatabase db = QSqlDatabase::addDatabase( "QSQLITE" );
    QString url = NULL;

    db.setDatabaseName( dbName );

    if( !db.open() )
    {
        qDebug() << db.lastError();
        qFatal( "Failed to connect." );
    }

    qDebug( "Connected!" );

    QSqlQuery qry;

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

    db.close();
    return url;

}

QStringList getDatasetsActive(QString dbName)
{
    QStringList activeList;
    QSqlDatabase db = QSqlDatabase::addDatabase( "QSQLITE" );

    db.setDatabaseName( dbName );

    if( !db.open() )
    {
        qDebug() << db.lastError();
        qFatal( "Failed to connect." );
    }

    qDebug( "Connected!" );

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

    db.close();
    return activeList;

}

QString getVarName(QString dbName, int id)
{
    QSqlDatabase db = QSqlDatabase::addDatabase( "QSQLITE" );
    QString varName = NULL;

    db.setDatabaseName( dbName );

    if( !db.open() )
    {
        qDebug() << db.lastError();
        qFatal( "Failed to connect." );
    }

    qDebug( "Connected!" );

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

    db.close();
    return varName;
}


int getId(QString dbName, QString VarName)
{
    QSqlDatabase db = QSqlDatabase::addDatabase( "QSQLITE" );
    int id = 0;

    db.setDatabaseName( dbName );

    if( !db.open() )
    {
        qDebug() << db.lastError();
        qFatal( "Failed to connect." );
    }

    qDebug( "Connected!" );

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

    db.close();
    return id;
}

void fillPointProperties(QString dbName, TPointProperties* pointProp)
{
    QSqlDatabase db = QSqlDatabase::addDatabase( "QSQLITE" );

    db.setDatabaseName( dbName );

    if( !db.open() )
    {
        qDebug() << db.lastError();
        qFatal( "Failed to connect." );
    }

    qDebug( "Connected!" );

    QSqlQuery qry;

    qry.prepare( "INSERT INTO point_properties (id_point, id_network, latitude, longitude, latInt, lonInt, utm_x, utm_y, altitude, state, region, province, municipality) VALUES (:id_point, :dataset, :id_network, :latitude, :longitude, :latInt, :lonInt, :utm_x, :utm_y, :altitude, :state, :region, :province, :municipality)" );

    qry.bindValue(":id_point", pointProp->id);
    qry.bindValue(":id_network", pointProp->network);
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
        qDebug() << qry.lastError();
    else
        qDebug( "successfully inserted" );


    db.close();

}

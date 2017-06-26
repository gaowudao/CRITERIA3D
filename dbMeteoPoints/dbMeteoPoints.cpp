#include "dbMeteoPoints.h"
#include "commonConstants.h"

#include <QDebug>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>

#include <QString>
#include <QStringBuilder>


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

QString DbMeteoPoints::getDbName()
{
    return _db.databaseName();
}


QString DbMeteoPoints::getDatasetURL(QString dataset)
{
    QSqlQuery qry(_db);
    QString url = NULL;

    qry.prepare( "SELECT URL FROM datasets WHERE dataset = :dataset");
    qry.bindValue(":dataset", dataset);

    if( !qry.exec() )
    {
        qDebug() << qry.lastError();
    }
    else
    {
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
    {
        qDebug() << qry.lastError();
    }
    else
    {
        qDebug( "getDatasetsActive Selected!" );

        while (qry.next())
        {
            QString active = qry.value(0).toString();
            activeList << active;

        }

    }

    return activeList;

}

void DbMeteoPoints::setDatasetsActive(QString active)
{

    QString statement = QString("UPDATE datasets SET active = 1 where dataset IN ( %1 )").arg(active);

    QSqlQuery qry(statement, _db);

    if( !qry.exec() )
        qDebug() << qry.lastError();
    else
    {
        qDebug( "set active!" );
    }


}

QStringList DbMeteoPoints::getDatasetsList()
{
    QStringList datasetList;
    QSqlQuery qry(_db);

    qry.prepare( "SELECT * FROM datasets" );

    if( !qry.exec() )
    {
        qDebug() << qry.lastError();
    }
    else
    {
        qDebug( "getDatasetsList Selected!" );

        while (qry.next())
        {
            QString dataset = qry.value(0).toString();
            datasetList << dataset;

        }

    }

    return datasetList;

}


QDateTime DbMeteoPoints::getLastDay(char dayHour)
{

    QSqlQuery qry(_db);
    QStringList tables;
    QDateTime lastDay(QDate(1800, 1, 1), QTime(0, 0, 0));

    qry.prepare( "SELECT name FROM sqlite_master WHERE type='table' AND name like :dayHour ESCAPE '^'");
    qry.bindValue(":dayHour",  "%^_" + QString(dayHour)  + "%");


    if( !qry.exec() )
    {
        qDebug() << qry.lastError();
    }
    else
    {
        while (qry.next())
        {
            QString table = qry.value(0).toString();
            tables << table;

        }

    }

    foreach (QString table, tables)
    {

        QString statement = QString( "SELECT date_time FROM `%1` ORDER BY datetime(date_time) DESC Limit 1").arg(table);
        if( !qry.exec(statement) )
        {
            qDebug() << qry.lastError();
        }
        else
        {

            if (qry.next())
            {
                QString dateStr = qry.value(0).toString();
                QDateTime date = QDateTime::fromString(dateStr,"yyyy-MM-dd HH:mm:ss");
                if (date > lastDay)
                {
                    lastDay = date;
                }

            }

        }

    }

    return lastDay;
}


QDateTime DbMeteoPoints::getFirstDay(char dayHour)
{

    QSqlQuery qry(_db);
    QStringList tables;
    QDateTime firstDay(QDate::currentDate(), QTime(0, 0, 0));


    qry.prepare( "SELECT name FROM sqlite_master WHERE type='table' AND name like :dayHour ESCAPE '^'");
    qry.bindValue(":dayHour",  "%^_" + QString(dayHour)  + "%");


    if( !qry.exec() )
    {
        qDebug() << qry.lastError();
    }
    else
    {
        while (qry.next())
        {
            QString table = qry.value(0).toString();
            tables << table;

        }

    }

    foreach (QString table, tables)
    {

        QString statement = QString( "SELECT date_time FROM `%1` ORDER BY datetime(date_time) ASC Limit 1").arg(table);
        if( !qry.exec(statement) )
        {
            qDebug() << qry.lastError();
        }
        else
        {

            if (qry.next())
            {
                QString dateStr = qry.value(0).toString();
                QDateTime date = QDateTime::fromString(dateStr,"yyyy-MM-dd HH:mm:ss");
                if (date < firstDay)
                {
                    firstDay = date;
                }
            }
        }
    }
    return firstDay;
}


void DbMeteoPoints::getDataFromDailyDb(Crit3DDate dateStart, Crit3DDate dateEnd, QList<Crit3DMeteoPoint> &meteoPointsList)
{

    int numberOfDays = difference(dateStart, dateEnd)+1;
    QString startDate = QString::fromStdString(dateStart.toStdString());
    QString endDate = QString::fromStdString(dateEnd.toStdString());

    QSqlQuery qry(_db);

    for (int i = 0; i < meteoPointsList.size(); i++)
    {

        meteoPointsList[i].initializeObsDataD(numberOfDays, dateStart);
        QString statement = QString( "SELECT * FROM `%1_D` WHERE date_time >= DATE('%2') AND date_time < DATE('%3', '+1 day')")
                                   .arg(QString::fromStdString(meteoPointsList[i].id)).arg(startDate).arg(endDate);
        if( !qry.exec(statement) )
        {
            qDebug() << qry.lastError();
        }
        else
        {
            while (qry.next())
            {
                QString dateStr = qry.value(0).toString();
                QDateTime qDateT = QDateTime::fromString(dateStr,"yyyy-MM-dd HH:mm:ss");
                Crit3DDate date(qDateT.date().day(), qDateT.date().month(), qDateT.date().year());

                int idVar = qry.value(1).toInt();
                meteoVariable meteoVar = getDefaultMeteoVariable(idVar);

                float value = qry.value(2).toFloat();
                meteoPointsList[i].setMeteoPointValueD(date, meteoVar, value);
            }
        }
    }
}


QList<Crit3DMeteoPoint> DbMeteoPoints::getPropertiesFromDb()
{
    QList<Crit3DMeteoPoint> meteoPointsList;
    Crit3DMeteoPoint meteoPoint;
    QSqlQuery qry(_db);

    qry.prepare( "SELECT id_point, name, dataset, latitude, longitude, latInt, lonInt, utm_x, utm_y, altitude, state, region, province, municipality, is_utc from point_properties" );

    if( !qry.exec() )
    {
        qDebug() << qry.lastError();
    }
    else
    {
        while (qry.next())
        {
            meteoPoint.id = qry.value(0).toString().toStdString();
            meteoPoint.name = qry.value(1).toString().toStdString();
            meteoPoint.dataset = qry.value(2).toString().toStdString();
            meteoPoint.latitude = qry.value(3).toDouble();
            meteoPoint.longitude = qry.value(4).toDouble();
            meteoPoint.latInt = qry.value(5).toInt();
            meteoPoint.lonInt = qry.value(6).toInt();
            meteoPoint.point.utm.x = qry.value(7).toDouble();
            meteoPoint.point.utm.y = qry.value(8).toDouble();
            meteoPoint.point.z = qry.value(9).toDouble();
            meteoPoint.state = qry.value(10).toString().toStdString();
            meteoPoint.region = qry.value(11).toString().toStdString();
            meteoPoint.province = qry.value(12).toString().toStdString();
            meteoPoint.municipality = qry.value(13).toString().toStdString();
            meteoPoint.isUTC = qry.value(14).toBool();

            meteoPointsList << meteoPoint;
        }
    }

    return meteoPointsList;
}


bool DbMeteoPoints::fillPointProperties(Crit3DMeteoPoint *myPoint)
{

    QSqlQuery qry(_db);

    qry.prepare( "INSERT INTO point_properties (id_point, name, dataset, latitude, longitude, latInt, lonInt, utm_x, utm_y, altitude, state, region, province, municipality)"
                                      " VALUES (:id_point, :name, :dataset, :latitude, :longitude, :latInt, :lonInt, :utm_x, :utm_y, :altitude, :state, :region, :province, :municipality)" );

    qry.bindValue(":id_point", QString::fromStdString(myPoint->id));
    qry.bindValue(":name", QString::fromStdString(myPoint->name));
    qry.bindValue(":dataset", QString::fromStdString(myPoint->dataset));
    qry.bindValue(":latitude", myPoint->latitude);
    qry.bindValue(":longitude", myPoint->longitude);
    qry.bindValue(":latInt", myPoint->latInt);
    qry.bindValue(":lonInt", myPoint->lonInt);
    qry.bindValue(":utm_x", myPoint->point.utm.x);
    qry.bindValue(":utm_y", myPoint->point.utm.y);
    qry.bindValue(":altitude", myPoint->point.z);
    qry.bindValue(":state", QString::fromStdString(myPoint->state));
    qry.bindValue(":region", QString::fromStdString(myPoint->region));
    qry.bindValue(":province", QString::fromStdString(myPoint->province));
    qry.bindValue(":municipality", QString::fromStdString(myPoint->municipality));

    if( !qry.exec() )
    {
        qDebug() << qry.lastError();
        return false;
    }
    else
        return true;

}


void DbMeteoPoints::getDataFromHourlyDb(Crit3DDate dateStart, Crit3DDate dateEnd, QList<Crit3DMeteoPoint> &meteoPointsList)
{

    int numberOfDays = difference(dateStart, dateEnd)+1;
    int myHourlyFraction = 1;
    QString startDate = QString::fromStdString(dateStart.toStdString());
    QString endDate = QString::fromStdString(dateEnd.toStdString());

    QSqlQuery qry(_db);

    for (int i = 0; i < meteoPointsList.size(); i++)
    {
        meteoPointsList[i].initializeObsDataH(myHourlyFraction, numberOfDays, dateStart);
        QString statement = QString( "SELECT * FROM `%1_H` WHERE date_time >= DATE('%2') AND date_time < DATE('%3', '+1 day')")
                                        .arg(QString::fromStdString(meteoPointsList[i].id)).arg(startDate).arg(endDate);
        if( !qry.exec(statement) )
        {
            qDebug() << qry.lastError();
        }
        else
        {
            while (qry.next())
            {
                QString dateStr = qry.value(0).toString();
                QDateTime qDateT = QDateTime::fromString(dateStr,"yyyy-MM-dd HH:mm:ss");
                Crit3DDate date(qDateT.date().day(), qDateT.date().month(), qDateT.date().year());

                int idVar = qry.value(1).toInt();
                meteoVariable meteoVar = getDefaultMeteoVariable(idVar);

                float value = qry.value(2).toFloat();
                meteoPointsList[i].setMeteoPointValueH(date, qDateT.time().hour(), qDateT.time().minute(), meteoVar, value);
            }
        }
    }
}




#include "dbMeteoPoints.h"
#include "commonConstants.h"

#include <QDebug>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>

#include <QString>
#include <QStringBuilder>

//#include <QtNetwork>

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
    {
        qDebug() << qry.lastError();
    }
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
    {
        qDebug() << qry.lastError();
    }
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
        qDebug( "Selected!" );

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

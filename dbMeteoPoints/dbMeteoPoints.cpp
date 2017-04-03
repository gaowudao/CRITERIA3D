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


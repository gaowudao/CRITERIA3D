#include "dbMeteoPoints.h"
#include "commonConstants.h"

#include <QtSql>
#include <QDebug>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>

#include <QString>
#include <QtNetwork>

QString getURL(QString dbName, QString dataset)
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

    qry.prepare( "SELECT URL FROM datasets WHERE dataset = :dataset" );
    qry.bindValue(":dataset", dataset);

    if( !qry.exec() )
        qDebug() << qry.lastError();
    else
    {
        qDebug( "Selected!" );

        if (qry.next())
        {
            QString url = qry.value(0).toString();
            db.close();

            return url;
        }
        else
            qDebug( "Error: dataset not found" );

    }

    db.close();
    return QString::null;


}

#include "unitDb.h"



UnitDb::UnitDb(QString dbname)
{
    error = "";

    if(db.isOpen())
    {
        qDebug() << db.connectionName() << "is already open";
        db.close();
    }

    db = QSqlDatabase::addDatabase("QSQLITE", QUuid::createUuid().toString());
    db.setDatabaseName(dbname);

    if (!db.open())
    {
       error = db.lastError().text();
    }
    else
    {
        createUnitTable();
    }

}

UnitDb::~UnitDb()
{
    if ((db.isValid()) && (db.isOpen()))
    {
        QString connection = db.connectionName();
        db.close();
        db = QSqlDatabase();
        QSqlDatabase::removeDatabase(connection);
    }
}

void UnitDb::createUnitTable()
{

    QSqlQuery qry(db);
    qry.prepare("CREATE TABLE units (ID_CASE TEXT, ID_CROP TEXT, ID_METEO TEXT, ID_SOIL NUMERIC)");
    if( !qry.exec() )
    {
        error = qry.lastError().text();
    }
}

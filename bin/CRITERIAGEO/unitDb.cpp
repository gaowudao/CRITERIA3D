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
        createUnitsTable();
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

void UnitDb::createUnitsTable()
{

    QSqlQuery qry(db);
    qry.prepare("CREATE TABLE units (ID_CASE TEXT, ID_CROP TEXT, ID_METEO TEXT, ID_SOIL NUMERIC, PRIMARY KEY(ID_CASE))");
    if( !qry.exec() )
    {
        error = qry.lastError().text();
    }
}

bool UnitDb::writeUnitsTable(QString idCase, QString idCrop, QString idMeteo, float idSoil)
{

    QSqlQuery qry(db);

    qry.prepare( "INSERT INTO units (ID_CASE, ID_CROP, ID_METEO, ID_SOIL)"
                                      " VALUES (:idCase, :idCrop, :idMeteo, :idSoil)" );

    qry.bindValue(":idCase", idCase);
    qry.bindValue(":idCrop", idCrop);
    qry.bindValue(":idMeteo", idMeteo);
    qry.bindValue(":idSoil", idSoil);

    if( !qry.exec() )
    {
        error = qry.lastError().text();
        return false;
    }
    else
        return true;

}

bool UnitDb::writeListToUnitsTable(QStringList idCase, QStringList idCrop, QStringList idMeteo, QList<float> idSoil)
{

    QSqlQuery qry(db);

    qry.prepare( "INSERT INTO units (ID_CASE, ID_CROP, ID_METEO, ID_SOIL)"
                                      " VALUES (?, ?, ?, ?)" );

    for (int i = 0; i < idCase.size(); i++)
    {
        qry.addBindValue(idCase[i]);
        qry.addBindValue(idCrop[i]);
        qry.addBindValue(idMeteo[i]);
        qry.addBindValue(idSoil[i]);

        if( !qry.exec() )
        {
            error = qry.lastError().text();
            return false;
        }
    }
    return true;

}

QString UnitDb::getError() const
{
    return error;
}

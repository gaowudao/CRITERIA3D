#include "dbClimate.h"

bool writeDailyElab(QSqlDatabase db, std::string *myError, QString id, int day, float result, QString elab)
{
    QSqlQuery qry(db);
    if (db.driverName() == "QSQLITE")
    {
        qry.prepare("CREATE TABLE IF NOT EXISTS `climate_daily` (days INTEGER, id_point TEXT, elab TEXT, value REAL);");
        if( !qry.exec() )
        {
            *myError = qry.lastError().text().toStdString();
            return false;
        }

    }
    else if (db.driverName() == "QMYSQL")
    {
        qry.prepare("CREATE TABLE IF NOT EXISTS `climate_daily` (days smallint(5), id_point varchar(20), elab  	varchar(100), value float(5,2) );");
        if( !qry.exec() )
        {
            *myError = qry.lastError().text().toStdString();
            return false;
        }
    }
    qry.prepare( "INSERT INTO `climate_daily` (days, id_point, elab, value)"
                                      " VALUES (:days, :id_point, :elab, :value)" );

    qry.bindValue(":days", day);
    qry.bindValue(":id_point", id);
    qry.bindValue(":elab", elab);
    qry.bindValue(":value", result);

    if( !qry.exec() )
    {
        *myError = qry.lastError().text().toStdString();
        return false;
    }

    return true;
}


bool writeDecadalElab(QSqlDatabase db, std::string *myError, QString id, int decades, float result, QString elab)
{
    QSqlQuery qry(db);
    if (db.driverName() == "QSQLITE")
    {
        qry.prepare("CREATE TABLE IF NOT EXISTS `climate_decadal` (decades INTEGER, id_point TEXT, elab TEXT, value REAL);");
        if( !qry.exec() )
        {
            *myError = qry.lastError().text().toStdString();
            return false;
        }

    }
    else if (db.driverName() == "QMYSQL")
    {
        qry.prepare("CREATE TABLE IF NOT EXISTS `climate_decadal` (decades smallint(5), id_point varchar(20), elab  varchar(100), value float(5,2) );");
        if( !qry.exec() )
        {
            *myError = qry.lastError().text().toStdString();
            return false;
        }
    }
    qry.prepare( "INSERT INTO `climate_decadal` (decades, id_point, elab, value)"
                                      " VALUES (:decades, :id_point, :elab, :value)" );

    qry.bindValue(":decades", decades);
    qry.bindValue(":id_point", id);
    qry.bindValue(":elab", elab);
    qry.bindValue(":value", result);

    if( !qry.exec() )
    {
        *myError = qry.lastError().text().toStdString();
        return false;
    }

    return true;
}

bool writeMonthlyElab(QSqlDatabase db, std::string *myError, QString id, int months, float result, QString elab)
{
    QSqlQuery qry(db);
    if (db.driverName() == "QSQLITE")
    {
        qry.prepare("CREATE TABLE IF NOT EXISTS `climate_monthly` (months INTEGER, id_point TEXT, elab TEXT, value REAL);");
        if( !qry.exec() )
        {
            *myError = qry.lastError().text().toStdString();
            return false;
        }

    }
    else if (db.driverName() == "QMYSQL")
    {
        qry.prepare("CREATE TABLE IF NOT EXISTS `climate_monthly` (months smallint(5), id_point varchar(20), elab  varchar(100), value float(5,2) );");
        if( !qry.exec() )
        {
            *myError = qry.lastError().text().toStdString();
            return false;
        }
    }
    qry.prepare( "INSERT INTO `climate_monthly` (months, id_point, elab, value)"
                                      " VALUES (:months, :id_point, :elab, :value)" );

    qry.bindValue(":months", months);
    qry.bindValue(":id_point", id);
    qry.bindValue(":elab", elab);
    qry.bindValue(":value", result);

    if( !qry.exec() )
    {
        *myError = qry.lastError().text().toStdString();
        return false;
    }

    return true;
}

bool writeSeasonalElab(QSqlDatabase db, std::string *myError, QString id, int season, float result, QString elab)
{
    QSqlQuery qry(db);
    if (db.driverName() == "QSQLITE")
    {
        qry.prepare("CREATE TABLE IF NOT EXISTS `climate_seasonal` (season INTEGER, id_point TEXT, elab TEXT, value REAL);");
        if( !qry.exec() )
        {
            *myError = qry.lastError().text().toStdString();
            return false;
        }

    }
    else if (db.driverName() == "QMYSQL")
    {
        qry.prepare("CREATE TABLE IF NOT EXISTS `climate_seasonal` (season smallint(5), id_point varchar(20), elab  varchar(100), value float(5,2) );");
        if( !qry.exec() )
        {
            *myError = qry.lastError().text().toStdString();
            return false;
        }
    }
    qry.prepare( "INSERT INTO `climate_seasonal` (season, id_point, elab, value)"
                                      " VALUES (:season, :id_point, :elab, :value)" );

    qry.bindValue(":season", season);
    qry.bindValue(":id_point", id);
    qry.bindValue(":elab", elab);
    qry.bindValue(":value", result);

    if( !qry.exec() )
    {
        *myError = qry.lastError().text().toStdString();
        return false;
    }

    return true;
}


bool writeAnnualElab(QSqlDatabase db, std::string *myError, QString id, float result, QString elab)
{
    QSqlQuery qry(db);
    if (db.driverName() == "QSQLITE")
    {
        qry.prepare("CREATE TABLE IF NOT EXISTS `climate_annual` (id_point TEXT, elab TEXT, value REAL);");
        if( !qry.exec() )
        {
            *myError = qry.lastError().text().toStdString();
            return false;
        }

    }
    else if (db.driverName() == "QMYSQL")
    {
        qry.prepare("CREATE TABLE IF NOT EXISTS `climate_annual` (id_point varchar(20), elab  varchar(100), value float(5,2) );");
        if( !qry.exec() )
        {
            *myError = qry.lastError().text().toStdString();
            return false;
        }
    }
    qry.prepare( "INSERT INTO `climate_annual` (id_point, elab, value)"
                                      " VALUES (:id_point, :elab, :value)" );

    qry.bindValue(":id_point", id);
    qry.bindValue(":elab", elab);
    qry.bindValue(":value", result);

    if( !qry.exec() )
    {
        *myError = qry.lastError().text().toStdString();
        return false;
    }

    return true;
}

bool writeGenericElab(QSqlDatabase db, std::string *myError, QString id, float result, QString elab)
{
    QSqlQuery qry(db);
    if (db.driverName() == "QSQLITE")
    {
        qry.prepare("CREATE TABLE IF NOT EXISTS `climate_generic` (id_point TEXT, elab TEXT, value REAL);");
        if( !qry.exec() )
        {
            *myError = qry.lastError().text().toStdString();
            return false;
        }

    }
    else if (db.driverName() == "QMYSQL")
    {
        qry.prepare("CREATE TABLE IF NOT EXISTS `climate_generic` (id_point varchar(20), elab  varchar(100), value float(5,2) );");
        if( !qry.exec() )
        {
            *myError = qry.lastError().text().toStdString();
            return false;
        }
    }
    qry.prepare( "INSERT INTO `climate_generic` (id_point, elab, value)"
                                      " VALUES (:id_point, :elab, :value)" );

    qry.bindValue(":id_point", id);
    qry.bindValue(":elab", elab);
    qry.bindValue(":value", result);

    if( !qry.exec() )
    {
        *myError = qry.lastError().text().toStdString();
        return false;
    }

    return true;
}




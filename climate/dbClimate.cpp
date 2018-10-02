#include "dbClimate.h"

namespace dbClimatePoint
{
    void createDailyTable(QSqlDatabase db)
    {
        QSqlQuery qry(db);
        qry.prepare("CREATE TABLE IF NOT EXISTS `climate_daily` (days INTEGER, id_point TEXT, elab TEXT, value REAL);");
        if( !qry.exec() )
        {
            qDebug() << qry.lastError();
        }
    }

    void createDecadalTable(QSqlDatabase db)
    {
        QSqlQuery qry(db);
        qry.prepare("CREATE TABLE IF NOT EXISTS `climate_decadal` (decades INTEGER, id_point TEXT, elab TEXT, value REAL);");
        if( !qry.exec() )
        {
            qDebug() << qry.lastError();
        }
    }

    void createMonthlyTable(QSqlDatabase db)
    {
        QSqlQuery qry(db);
        qry.prepare("CREATE TABLE IF NOT EXISTS `climate_monthly` (months INTEGER, id_point TEXT, elab TEXT, value REAL);");
        if( !qry.exec() )
        {
            qDebug() << qry.lastError();
        }
    }

    void createSeasonalTable(QSqlDatabase db)
    {
        QSqlQuery qry(db);
        qry.prepare("CREATE TABLE IF NOT EXISTS `climate_seasonal` (season INTEGER, id_point TEXT, elab TEXT, value REAL);");
        if( !qry.exec() )
        {
            qDebug() << qry.lastError();
        }
    }

    void createAnnualTable(QSqlDatabase db)
    {
        QSqlQuery qry(db);
        qry.prepare("CREATE TABLE IF NOT EXISTS `climate_annual` (id_point TEXT, elab TEXT, value REAL);");
        if( !qry.exec() )
        {
            qDebug() << qry.lastError();
        }
    }

    void createGenericPeriodTable(QSqlDatabase db)
    {
        QSqlQuery qry(db);
        qry.prepare("CREATE TABLE IF NOT EXISTS `climate_generic` (id_point TEXT);");
        if( !qry.exec() )
        {
            qDebug() << qry.lastError();
        }
    }

    void writeDailyResult(QSqlDatabase db, QString id, int day, float result, QString elab)
    {
        QSqlQuery qry(db);
        qry.prepare( "INSERT INTO `climate_daily` (days, id_point, elab, value)"
                                          " VALUES (:days, :id_point, :elab, :value)" );

        qry.bindValue(":days", day);
        qry.bindValue(":id_point", id);
        qry.bindValue(":elab", elab);
        qry.bindValue(":value", result);

        if( !qry.exec() )
        {
            qDebug() << qry.lastError();
        }
    }
}


//namespace dbClimateGrid
//{

//}

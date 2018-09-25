#include "dbClimate.h"

namespace dbClimatePoint
{
    void createDailyTable(QSqlDatabase db)
    {
        QSqlQuery qry(db);
        qry.prepare("CREATE TABLE IF NOT EXISTS `climate_daily` (id_point TEXT, days INTEGER);");
        if( !qry.exec() )
        {
            qDebug() << qry.lastError();
        }
    }

    void createDecadalTable(QSqlDatabase db)
    {
        QSqlQuery qry(db);
        qry.prepare("CREATE TABLE IF NOT EXISTS `climate_decadal` (id_point TEXT, decades INTEGER);");
        if( !qry.exec() )
        {
            qDebug() << qry.lastError();
        }
    }

    void createMonthlyTable(QSqlDatabase db)
    {
        QSqlQuery qry(db);
        qry.prepare("CREATE TABLE IF NOT EXISTS `climate_monthly` (id_point TEXT, months INTEGER);");
        if( !qry.exec() )
        {
            qDebug() << qry.lastError();
        }
    }

    void createSeasonalTable(QSqlDatabase db)
    {
        QSqlQuery qry(db);
        qry.prepare("CREATE TABLE IF NOT EXISTS `climate_seasonal` (id_point TEXT, season INTEGER);");
        if( !qry.exec() )
        {
            qDebug() << qry.lastError();
        }
    }

    void createAnnualTable(QSqlDatabase db)
    {
        QSqlQuery qry(db);
        qry.prepare("CREATE TABLE IF NOT EXISTS `climate_annual` (id_point TEXT);");
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
}



namespace dbClimateGrid
{

}

#include "dbClimate.h"
#include "utilities.h"

// LC saveDailyElab is a bit more efficient respect saveDailyElabSingleValue, making prepare just once.
// Anyway all the process for each id_point is only 2seconds.
bool saveDailyElab(QSqlDatabase db, std::string *myError, QString id, std::vector<float> allResults, QString elab)
{
    QSqlQuery qry(db);
    if (db.driverName() == "QSQLITE")
    {
        qry.prepare("CREATE TABLE IF NOT EXISTS `climate_daily` (days INTEGER, id_point TEXT, elab TEXT, value REAL, PRIMARY KEY(days,id_point,elab));");
        if( !qry.exec() )
        {
            *myError = qry.lastError().text().toStdString();
            return false;
        }

    }
    else if (db.driverName() == "QMYSQL")
    {
        qry.prepare("CREATE TABLE IF NOT EXISTS `climate_daily` (days smallint(5), id_point varchar(20), elab varchar(100), value float(5,2), PRIMARY KEY(days,id_point,elab) );");
        if( !qry.exec() )
        {
            *myError = qry.lastError().text().toStdString();
            return false;
        }
    }
    qry.prepare( "INSERT INTO `climate_daily` (days, id_point, elab, value)"
                                      " VALUES (?, ?, ?, ?)" );



    for (unsigned int i = 0; i < allResults.size(); i++)
    {
        qry.addBindValue(i+1);
        qry.addBindValue(id);
        qry.addBindValue(elab);
        qry.addBindValue(allResults.at(i));

        if( !qry.exec() )
        {
            *myError = qry.lastError().text().toStdString();
            return false;
        }
    }


    return true;
}

bool saveDailyElabSingleValue(QSqlDatabase db, std::string *myError, QString id, int day, float result, QString elab)
{
    QSqlQuery qry(db);
    if (db.driverName() == "QSQLITE")
    {
        qry.prepare("CREATE TABLE IF NOT EXISTS `climate_daily` (days INTEGER, id_point TEXT, elab TEXT, value REAL, PRIMARY KEY(days,id_point,elab));");
        if( !qry.exec() )
        {
            *myError = qry.lastError().text().toStdString();
            return false;
        }

    }
    else if (db.driverName() == "QMYSQL")
    {
        qry.prepare("CREATE TABLE IF NOT EXISTS `climate_daily` (days smallint(5), id_point varchar(20), elab  	varchar(100), value float(5,2), PRIMARY KEY(days,id_point,elab) );");
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


bool saveDecadalElab(QSqlDatabase db, std::string *myError, QString id, std::vector<float> allResults, QString elab)
{
    QSqlQuery qry(db);
    if (db.driverName() == "QSQLITE")
    {
        qry.prepare("CREATE TABLE IF NOT EXISTS `climate_decadal` (decades INTEGER, id_point TEXT, elab TEXT, value REAL, PRIMARY KEY(decades,id_point,elab));");
        if( !qry.exec() )
        {
            *myError = qry.lastError().text().toStdString();
            return false;
        }

    }
    else if (db.driverName() == "QMYSQL")
    {
        qry.prepare("CREATE TABLE IF NOT EXISTS `climate_decadal` (decades smallint(5), id_point varchar(20), elab  varchar(100), value float(5,2), PRIMARY KEY(decades,id_point,elab) );");
        if( !qry.exec() )
        {
            *myError = qry.lastError().text().toStdString();
            return false;
        }
    }
    qry.prepare( "INSERT INTO `climate_decadal` (decades, id_point, elab, value)"
                                      " VALUES (?, ?, ?, ?)" );

    for (unsigned int i = 0; i < allResults.size(); i++)
    {
        qry.addBindValue(i+1);
        qry.addBindValue(id);
        qry.addBindValue(elab);
        qry.addBindValue(allResults.at(i));

        if( !qry.exec() )
        {
            *myError = qry.lastError().text().toStdString();
            return false;
        }
    }

    return true;
}

bool saveDecadalElabSingleValue(QSqlDatabase db, std::string *myError, QString id, int decades, float result, QString elab)
{
    QSqlQuery qry(db);
    if (db.driverName() == "QSQLITE")
    {
        qry.prepare("CREATE TABLE IF NOT EXISTS `climate_decadal` (decades INTEGER, id_point TEXT, elab TEXT, value REAL, PRIMARY KEY(decades,id_point,elab));");
        if( !qry.exec() )
        {
            *myError = qry.lastError().text().toStdString();
            return false;
        }

    }
    else if (db.driverName() == "QMYSQL")
    {
        qry.prepare("CREATE TABLE IF NOT EXISTS `climate_decadal` (decades smallint(5), id_point varchar(20), elab  varchar(100), value float(5,2), PRIMARY KEY(decades,id_point,elab) );");
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

bool saveMonthlyElab(QSqlDatabase db, std::string *myError, QString id, std::vector<float> allResults, QString elab)
{
    QSqlQuery qry(db);
    if (db.driverName() == "QSQLITE")
    {
        qry.prepare("CREATE TABLE IF NOT EXISTS `climate_monthly` (months INTEGER, id_point TEXT, elab TEXT, value REAL, PRIMARY KEY(months,id_point,elab));");
        if( !qry.exec() )
        {
            *myError = qry.lastError().text().toStdString();
            return false;
        }

    }
    else if (db.driverName() == "QMYSQL")
    {
        qry.prepare("CREATE TABLE IF NOT EXISTS `climate_monthly` (months smallint(5), id_point varchar(20), elab  varchar(100), value float(5,2), PRIMARY KEY(months,id_point,elab) );");
        if( !qry.exec() )
        {
            *myError = qry.lastError().text().toStdString();
            return false;
        }
    }
    qry.prepare( "INSERT INTO `climate_monthly` (months, id_point, elab, value)"
                                      " VALUES (?, ?, ?, ?)" );

    for (unsigned int i = 0; i < allResults.size(); i++)
    {
        qry.addBindValue(i+1);
        qry.addBindValue(id);
        qry.addBindValue(elab);
        qry.addBindValue(allResults.at(i));

        if( !qry.exec() )
        {
            *myError = qry.lastError().text().toStdString();
            return false;
        }
    }

    return true;
}

bool saveMonthlyElabSingleValue(QSqlDatabase db, std::string *myError, QString id, int months, float result, QString elab)
{
    QSqlQuery qry(db);
    if (db.driverName() == "QSQLITE")
    {
        qry.prepare("CREATE TABLE IF NOT EXISTS `climate_monthly` (months INTEGER, id_point TEXT, elab TEXT, value REAL, PRIMARY KEY(months,id_point,elab));");
        if( !qry.exec() )
        {
            *myError = qry.lastError().text().toStdString();
            return false;
        }

    }
    else if (db.driverName() == "QMYSQL")
    {
        qry.prepare("CREATE TABLE IF NOT EXISTS `climate_monthly` (months smallint(5), id_point varchar(20), elab  varchar(100), value float(5,2), PRIMARY KEY(months,id_point,elab) );");
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

bool saveSeasonalElabSingleValue(QSqlDatabase db, std::string *myError, QString id, int season, float result, QString elab)
{
    QSqlQuery qry(db);
    if (db.driverName() == "QSQLITE")
    {
        qry.prepare("CREATE TABLE IF NOT EXISTS `climate_seasonal` (season INTEGER, id_point TEXT, elab TEXT, value REAL, PRIMARY KEY(season,id_point,elab));");
        if( !qry.exec() )
        {
            *myError = qry.lastError().text().toStdString();
            return false;
        }

    }
    else if (db.driverName() == "QMYSQL")
    {
        qry.prepare("CREATE TABLE IF NOT EXISTS `climate_seasonal` (season smallint(5), id_point varchar(20), elab  varchar(100), value float(5,2), PRIMARY KEY(season,id_point,elab) );");
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

bool saveSeasonalElab(QSqlDatabase db, std::string *myError, QString id, std::vector<float> allResults, QString elab)
{
    QSqlQuery qry(db);
    if (db.driverName() == "QSQLITE")
    {
        qry.prepare("CREATE TABLE IF NOT EXISTS `climate_seasonal` (season INTEGER, id_point TEXT, elab TEXT, value REAL, PRIMARY KEY(season,id_point,elab));");
        if( !qry.exec() )
        {
            *myError = qry.lastError().text().toStdString();
            return false;
        }

    }
    else if (db.driverName() == "QMYSQL")
    {
        qry.prepare("CREATE TABLE IF NOT EXISTS `climate_seasonal` (season smallint(5), id_point varchar(20), elab  varchar(100), value float(5,2), PRIMARY KEY(season,id_point,elab) );");
        if( !qry.exec() )
        {
            *myError = qry.lastError().text().toStdString();
            return false;
        }
    }
    qry.prepare( "INSERT INTO `climate_seasonal` (season, id_point, elab, value)"
                                      " VALUES (?, ?, ?,?)" );

    for (unsigned int i = 0; i < allResults.size(); i++)
    {
        qry.addBindValue(i+1);
        qry.addBindValue(id);
        qry.addBindValue(elab);
        qry.addBindValue(allResults.at(i));

        if( !qry.exec() )
        {
            *myError = qry.lastError().text().toStdString();
            return false;
        }
    }

    return true;
}


bool saveAnnualElab(QSqlDatabase db, std::string *myError, QString id, float result, QString elab)
{
    QSqlQuery qry(db);
    if (db.driverName() == "QSQLITE")
    {
        qry.prepare("CREATE TABLE IF NOT EXISTS `climate_annual` (id_point TEXT, elab TEXT, value REAL, PRIMARY KEY(id_point,elab));");
        if( !qry.exec() )
        {
            *myError = qry.lastError().text().toStdString();
            return false;
        }

    }
    else if (db.driverName() == "QMYSQL")
    {
        qry.prepare("CREATE TABLE IF NOT EXISTS `climate_annual` (id_point varchar(20), elab  varchar(100), value float(5,2), PRIMARY KEY(id_point,elab) );");
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

bool saveGenericElab(QSqlDatabase db, std::string *myError, QString id, float result, QString elab)
{
    QSqlQuery qry(db);
    if (db.driverName() == "QSQLITE")
    {
        qry.prepare("CREATE TABLE IF NOT EXISTS `climate_generic` (id_point TEXT, elab TEXT, value REAL, PRIMARY KEY(id_point,elab));");
        if( !qry.exec() )
        {
            *myError = qry.lastError().text().toStdString();
            return false;
        }

    }
    else if (db.driverName() == "QMYSQL")
    {
        qry.prepare("CREATE TABLE IF NOT EXISTS `climate_generic` (id_point varchar(20), elab  varchar(100), value float(5,2), PRIMARY KEY(id_point,elab) );");
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

bool selectVarElab(QSqlDatabase db, std::string *myError, QString table, QString variable, QStringList* listElab)
{
    QSqlQuery qry(db);
    QString elab;

    bool found = false;
    QString statement = QString("SELECT DISTINCT elab from `%1` WHERE `elab` LIKE '%'||:variable||'%'").arg(table);

    qry.prepare(statement);

    qry.bindValue(":variable", variable);

    if( !qry.exec() )
    {
        *myError = qry.lastError().text().toStdString();
        return false;
    }
    else
    {

        while (qry.next())
        {
            if (getValue(qry.value("elab"), &elab))
            {
                listElab->append(elab);
                found = true;
            }
            else
            {
                *myError = qry.lastError().text().toStdString();
            }
        }
    }

    return found;
}

bool showClimateTables(QSqlDatabase db, std::string *myError, QStringList* climateTables)
{
    QSqlQuery qry(db);
    QString table;

    if (db.driverName() == "QSQLITE")
    {
        qry.prepare("SELECT * FROM sqlite_master");
        if( !qry.exec() )
        {
            *myError = qry.lastError().text().toStdString();
            return false;
        }
        else
        {
            while (qry.next())
            {
                if (getValue(qry.value("name"), &table))
                {
                    if( qry.value("type").toString() == "table" && table != "sqlite_sequence" ) //The "sqlite_sequence" table is an internal table used to help implement AUTOINCREMENT
                    {
                       climateTables->append(table);
                    }

                }
                else
                {
                    *myError = qry.lastError().text().toStdString();
                    return false;
                }
            }
        }
    }
    else if (db.driverName() == "QMYSQL")
    {
        qry.prepare("SHOW TABLES LIKE 'climate_%'");
        if( !qry.exec() )
        {
            *myError = qry.lastError().text().toStdString();
            return false;
        }
        else
        {
            while (qry.next())
            {
                if (getValue(qry.value(0), &table))
                {
                    climateTables->append(table);
                }
                else
                {
                    *myError = qry.lastError().text().toStdString();
                    return false;
                }
            }
        }
    }

    return true;
}


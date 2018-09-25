#ifndef DBCLIMATE_H
#define DBCLIMATE_H

#include <QtSql>


namespace dbClimatePoint
{
    void createDailyTable(QSqlDatabase db);
    void createDecadalTable(QSqlDatabase db);
    void createMonthlyTable(QSqlDatabase db);
    void createSeasonalTable(QSqlDatabase db);
    void createAnnualTable(QSqlDatabase db);
    void createGenericPeriodTable(QSqlDatabase db);

}

namespace dbClimateGrid
{


}
#endif // DBCLIMATE_H

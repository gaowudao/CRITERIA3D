#include <QApplication>

#include "meteoWidget.h"
#include "meteoPoint.h"
#include "utilities.h"
#include "console.h"

#include "dbMeteoPointsHandler.h"
#include <iostream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFileDialog openDBDialog;

    QString dbData = openDBDialog.getOpenFileName(nullptr, ("Open mp database"), "", ("SQLite files (*.db)"));

    if (dbData == "")
    {
        return -1;
    }

    Crit3DMeteoPointsDbHandler mpHandler(dbData);
    mpHandler.loadVariableProperties();

    // test fill meteoPoint with random series
    Crit3DMeteoPoint meteoPoint;
    Crit3DMeteoPoint meteoPointSecond;

    meteoPoint.setId("10141");
    meteoPointSecond.setId("10");

    QDateTime first = mpHandler.getFirstDate(daily);
    QDateTime last = mpHandler.getLastDate(daily);
    QDate lastDateYear = first.date().addDays(365);
    if (last.date() < lastDateYear)
    {
        mpHandler.loadDailyData(getCrit3DDate(first.date()), getCrit3DDate(last.date()), &meteoPoint);
        mpHandler.loadDailyData(getCrit3DDate(first.date()), getCrit3DDate(last.date()), &meteoPointSecond);
    }
    else
    {
        mpHandler.loadDailyData(getCrit3DDate(first.date()), getCrit3DDate(lastDateYear), &meteoPoint);
        mpHandler.loadDailyData(getCrit3DDate(first.date()), getCrit3DDate(lastDateYear), &meteoPointSecond);
    }

    first = mpHandler.getFirstDate(hourly);
    last = mpHandler.getLastDate(hourly);
    lastDateYear = first.date().addDays(1000);
    if (last.date() < lastDateYear)
    {
        mpHandler.loadHourlyData(getCrit3DDate(first.date()), getCrit3DDate(last.date()), &meteoPoint);
        mpHandler.loadHourlyData(getCrit3DDate(first.date()), getCrit3DDate(last.date()), &meteoPointSecond);
    }
    else
    {
        mpHandler.loadHourlyData(getCrit3DDate(first.date()), getCrit3DDate(lastDateYear), &meteoPoint);
        mpHandler.loadHourlyData(getCrit3DDate(first.date()), getCrit3DDate(lastDateYear), &meteoPointSecond);
    }

    #ifdef _WIN32
        attachOutputToConsole();
    #endif

    std::cout << "\n...Create window\n" << std::flush;
    Crit3DMeteoWidget w;

    std::cout << "...Show window\n" << std::flush;
    w.show();

    std::cout << "...draw data p1\n" << std::flush;
    w.draw(meteoPoint);

    std::cout << "...draw data p2\n" << std::flush;
    w.draw(meteoPointSecond);

    return a.exec();
}
 

#include <QApplication>

#include "meteoWidget.h"
#include "meteoPoint.h"
#include "utilities.h"
#include "console.h"

#include <QRandomGenerator> // test
#include <iostream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // test fill meteoPoint with random series
    Crit3DMeteoPoint meteoPoint;
    Crit3DMeteoPoint meteoPointSecond;
    Crit3DDate firstDate = Crit3DDate(1, 1, 2018);
    Crit3DDate lastDate = Crit3DDate(31, 12, 2018);
    float tmin;
    float tmax;
    float tavg;
    float prec;
    float airRel;
    meteoPoint.initializeObsDataD(365,firstDate);
    meteoPoint.setId("1");
    meteoPointSecond.initializeObsDataD(365,firstDate);
    meteoPointSecond.setId("2");
    for (Crit3DDate date = firstDate; date <= lastDate; ++date)
    {
        tmin = QRandomGenerator::global()->generateDouble()*10+2;
        tmax = QRandomGenerator::global()->generateDouble()*10+5;
        prec = QRandomGenerator::global()->generateDouble()*10+1;
        airRel = 5;
        tavg = (tmin+tmax)/2;

        if (date.month == 1 && date.day == 1)
        {
            meteoPoint.setMeteoPointValueD(date, dailyAirTemperatureAvg, 3);
            meteoPoint.setMeteoPointValueD(date, dailyPrecipitation, 5);
            meteoPointSecond.setMeteoPointValueD(date, dailyAirTemperatureAvg, 4);
            meteoPointSecond.setMeteoPointValueD(date, dailyPrecipitation, 6);
        }
        else if (date.month == 1 && date.day == 2)
        {
            meteoPoint.setMeteoPointValueD(date, dailyAirTemperatureAvg, 5);
            meteoPointSecond.setMeteoPointValueD(date, dailyAirTemperatureAvg, 7);
        }
        else if (date.month == 3 && date.day == 1)
        {
            meteoPoint.setMeteoPointValueD(date, dailyAirTemperatureAvg, 7);
            meteoPoint.setMeteoPointValueD(date, dailyPrecipitation, 15);

            meteoPointSecond.setMeteoPointValueD(date, dailyAirTemperatureAvg, 2);
            meteoPointSecond.setMeteoPointValueD(date, dailyPrecipitation, 25);
        }
        else
        {
            meteoPoint.setMeteoPointValueD(date, dailyAirTemperatureMin, tmin);
            meteoPoint.setMeteoPointValueD(date, dailyAirTemperatureMax, tmax);
            meteoPoint.setMeteoPointValueD(date, dailyAirTemperatureAvg, tavg);
            meteoPoint.setMeteoPointValueD(date, dailyPrecipitation, prec);
            meteoPoint.setMeteoPointValueD(date, dailyAirRelHumidityAvg, airRel);

            meteoPointSecond.setMeteoPointValueD(date, dailyAirTemperatureMin, tmin*0.8);
            meteoPointSecond.setMeteoPointValueD(date, dailyAirTemperatureMax, tmax*0.8);
            meteoPointSecond.setMeteoPointValueD(date, dailyAirTemperatureAvg, tavg*0.8);
            meteoPointSecond.setMeteoPointValueD(date, dailyPrecipitation, prec*0.8);
        }

    }
    meteoPoint.initializeObsDataH(1, 15, firstDate);
    Crit3DTime firstDateTime(firstDate, 0);
    lastDate = firstDate.addDays(15);
    Crit3DTime lastDateTime(lastDate, 0);
    for (Crit3DTime date = firstDateTime; date <= lastDateTime; date=date.addSeconds(3600))
    {
        for (int i = 0; i<24; i++)
        {
            prec = QRandomGenerator::global()->generateDouble()*10+1;
            tavg = 10;
            airRel = 2;
            meteoPoint.setMeteoPointValueH(date.date, i, 0, airTemperature, tavg);
            meteoPoint.setMeteoPointValueH(date.date, i, 0, precipitation, prec);
            meteoPoint.setMeteoPointValueH(date.date, i, 0, airRelHumidity, airRel);
        }
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
 

#include <QApplication>

#include "meteoWidget.h"
#include "meteoPoint.h"
#include "utilities.h"
#include <QRandomGenerator> // test

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // test fill meteoPoint with random series
    Crit3DMeteoPoint meteoPoint;
    Crit3DDate firstDate = Crit3DDate(1, 1, 2018);
    Crit3DDate lastDate = Crit3DDate(31, 12, 2018);
    float tmin;
    float tmax;
    float tavg;
    float prec;
    meteoPoint.initializeObsDataD(365,firstDate);

    for (Crit3DDate date = firstDate; date <= lastDate; ++date)
    {
        tmin = QRandomGenerator::global()->generateDouble()*10+2;
        tmax = QRandomGenerator::global()->generateDouble()*10+5;
        prec = QRandomGenerator::global()->generateDouble()*10+1;
        tavg = (tmin+tmax)/2;
        meteoPoint.setMeteoPointValueD(date, dailyAirTemperatureMin, tmin);
        meteoPoint.setMeteoPointValueD(date, dailyAirTemperatureMax, tmax);
        meteoPoint.setMeteoPointValueD(date, dailyAirTemperatureAvg, tavg);
        meteoPoint.setMeteoPointValueD(date, dailyPrecipitation, prec);
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
            meteoPoint.setMeteoPointValueH(date.date, i, 0, airTemperature, tavg);
            meteoPoint.setMeteoPointValueH(date.date, i, 0, precipitation, prec);
        }
    }
    Crit3DMeteoWidget w;
    w.draw(meteoPoint);
    w.show();

    return a.exec();
}
 

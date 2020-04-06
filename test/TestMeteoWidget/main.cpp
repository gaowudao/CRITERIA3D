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
    Crit3DMeteoWidget w;
    QVector<Crit3DMeteoPoint> mpVector;
    frequencyType freq = daily;
    mpVector.append(meteoPoint);
    w.draw(mpVector, freq);
    w.show();

    return a.exec();
}
 

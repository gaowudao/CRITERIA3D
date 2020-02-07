#include "commonConstants.h"
#include "dbMeteoGrid.h"
#include "meteo.h"
#include <iostream>
#include <QFileDialog>
#include <QApplication>


static Crit3DMeteoGridDbHandler* meteoGridDbHandler;


void logInfo(QString myStr)
{
     std::cout << myStr.toStdString() << std::endl;
}


bool loadMeteoGridDB(QString* errorString)
{
    QString xmlName = QFileDialog::getOpenFileName(nullptr, "Open XML grid", "", "XML files (*.xml)");
    if (xmlName == "") return false;

    meteoGridDbHandler = new Crit3DMeteoGridDbHandler();

    // todo
    //meteoGridDbHandler->meteoGrid()->setGisSettings(this->gisSettings);

    if (! meteoGridDbHandler->parseXMLGrid(xmlName, errorString)) return false;

    if (! meteoGridDbHandler->openDatabase(errorString))return false;

    if (! meteoGridDbHandler->loadCellProperties(errorString)) return false;

    if (! meteoGridDbHandler->updateGridDate(errorString)) return false;

    logInfo("Meteo Grid = " + xmlName);

    return true;
}



int main(int argc, char *argv[])
{
    QApplication myApp(argc, argv);
    QString myError, myMeteoPoint;
    meteoVariable variable;
    QDate firstDay(1961,1,1);
    QDate lastDay(1990,12,31);
    QDate firstDateDB(1,1,1);
    std::vector<float> minDailyTemperature;
    std::vector<float> maxDailyTemperature;
    std::vector<float> cumDailyPrecipitation;

    QString errorString;
    if (! loadMeteoGridDB(&errorString))
    {
        std::cout << errorString.toStdString();
        return -1;
    }

    variable = dailyAirTemperatureMin;
    //minDailyTemperature = meteoGridDbHandler->loadGridDailyVar(&myError,myMeteoPoint,variable,firstDay,lastDay,&firstDateDB);
    variable = dailyAirTemperatureMax;
    //maxDailyTemperature = meteoGridDbHandler->loadGridDailyVar(&myError,myMeteoPoint,variable,firstDay,lastDay,&firstDateDB);
    variable = dailyPrecipitation;
    //maxDailyTemperature = meteoGridDbHandler->loadGridDailyVar(&myError,myMeteoPoint,variable,firstDay,lastDay,&firstDateDB);

    return 0;
}




#ifndef DATAHANDLER_H
#define DATAHANDLER_H

    #ifndef QVARIANT_H
        #include <QVariant>
    #endif
    #ifndef QDATETIME_H
        #include <QDateTime>
    #endif
    #ifndef QSTRING_H
        #include <QString>
    #endif
    #ifndef CRIT3DDATE_H
        #include "crit3dDate.h"
    #endif
    #ifndef METEO_H
        #include "meteo.h"
    #endif
    #ifndef PLANT_H
        #include "plant.h"
    #endif
    #ifndef GIS_H
        #include "gis.h"
    #endif

    #define OUTPUT_WEATHER 1
    #define OUTPUT_PLANT 2
    #define OUTPUT_SOIL 3

    float getTimeStepFromHourlyInterval(int myHourlyIntervals);
    meteoVariable getMeteoVariable(int myVar);
    int getMeteoVarIndex(meteoVariable myVar);

    QString getOutputNameDaily(QString varName, QString strArea, QString notes, QDate myDate);
    QString getOutputNameHourly(meteoVariable myVar, Crit3DTime myTime, QString myArea);
    QString getOutputNameHourly(meteoVariable myVar, QDateTime myTime, QString myArea);
    QString getVarNameFromMeteoVariable(meteoVariable myVar);
    QString getVarNameFromPlantVariable(plantVariable myVar);
    meteoVariable getMeteoVariableFromVarName(QString myVar);
    float readDataHourly(meteoVariable myVar, QString hourlyPath, QDateTime myTime, QString myArea, int row, int col);
    bool readHourlyMap(meteoVariable myVar, QString hourlyPath, QDateTime myTime, QString myArea, gis::Crit3DRasterGrid* myGrid);

#endif // DATAHANDLER_H

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

    Crit3DDate getCrit3DDate(const QDate &myDate);
    Crit3DDate getCrit3DDate(const QDateTime &myTime);
    Crit3DTime getCrit3DTime(const QDateTime& myQTime);
    QDate getQDate(Crit3DDate myDate);
    QDateTime getQDateTime(Crit3DTime myCrit3DTime);
    float getTimeStepFromHourlyInterval(int myHourlyIntervals);
    QString getQStringFromCrit3DTime(const Crit3DTime& myCrit3DTime);

    bool getValue(QVariant myRs, int* myValue);
    bool getValue(QVariant myRs, float* myValue);
    bool getValue(QVariant myRs, double* myValue);
    meteoVariable getMeteoVariable(int myVar);
    int getMeteoVarIndex(meteoVariable myVar);

    QString getFileNameFromString(QString myString);
    QString getPathFromString(QString myString);
    QString getOutputNameDaily(QString producer, QString varName, QString strArea, QString notes, QDate myDate);
    QString getOutputNameHourly(meteoVariable myVar, Crit3DTime myTime, QString myArea);
    QString getOutputNameHourly(meteoVariable myVar, QDateTime myTime, QString myArea);
    QString getVarNameFromMeteoVariable(meteoVariable myVar);
    QString getVarNameFromPlantVariable(plantVariable myVar);
    meteoVariable getMeteoVariableFromVarName(QString myVar);
    float readDataHourly(meteoVariable myVar, QString hourlyPath, QDateTime myTime, QString myArea, int row, int col);
    bool readHourlyMap(meteoVariable myVar, QString hourlyPath, QDateTime myTime, QString myArea, gis::Crit3DRasterGrid* myGrid);

#endif // DATAHANDLER_H

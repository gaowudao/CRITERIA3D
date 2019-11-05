#ifndef UTILITIES_H
#define UTILITIES_H

    #ifndef METEO_H
        #include "meteo.h"
    #endif

    class QDate;
    class QDateTime;
    class QVariant;
    class QString;
    class QStringList;
    class QSqlDatabase;

    Crit3DDate getCrit3DDate(const QDate &myDate);
    Crit3DTime getCrit3DTime(const QDateTime &myTime);
    Crit3DTime getCrit3DTime(const QDate& t, int hour);

    QStringList getFields(QSqlDatabase* db_, QString tableName);

    QDate getQDate(const Crit3DDate &myDate);
    QDateTime getQDateTime(const Crit3DTime &myCrit3DTime);
    int decadeFromDate(QDate date);
    void intervalDecade(int decade, int year, int* dayStart, int* dayEnd, int* month);
    int getSeasonFromDate(QDate date);
    int getSeasonFromString(QString season);
    QString getStringSeasonFromDate(QDate date);
    bool getPeriodDates(QString periodSelected, int year, QDate myDate, QDate* startDate, QDate* endDate);

    bool getValue(QVariant myRs, int* myValue);
    bool getValue(QVariant myRs, float* myValue);
    bool getValue(QVariant myRs, double* myValue);
    bool getValue(QVariant myRs, QDate* myValue);
    bool getValue(QVariant myRs, QDateTime* myValue);
    bool getValue(QVariant myRs, QString* myValue);

    QString getFilePath(QString fileNameComplete);
    QString getFileName(QString fileNameComplete);

    std::vector <float> StringListToFloat(QStringList myList);
    QStringList FloatVectorToStringList(std::vector <float> myVector);

    bool removeDirectory(QString myPath);

    QString getVarNameDaily(meteoVariable myVar);
    QString getVarNameHourly(meteoVariable myVar);
    QString getOutputNameDaily(meteoVariable dailyVar, QDate myDate);
    QString getOutputNameHourly(meteoVariable myVar, QDateTime myTime);

#endif // UTILITIES_H

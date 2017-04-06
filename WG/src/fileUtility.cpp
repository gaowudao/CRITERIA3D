#include "fileUtility.h"
#include <QDebug>
#include <QFile>
#include <QTextStream>

#include "commonConstants.h"
#include "Crit3DDate.h"
#include "weatherGenerator.h"


// read input data from csv
bool readCsv (QString namefile, char separator, float noData, TinputObsData* inputData)
{

    QFile file(namefile);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "\nERROR!\n" << namefile << file.errorString();
        return false;
    }

    QStringList listData;
    QStringList listTMin;
    QStringList listTMax;
    QStringList listPrecip;

    int indexLine = 0;
    int indexDate = 0;
    Crit3DDate tempDate;

    QString noDataString = QString::number(noData);

    // header
    file.readLine();

    while (!file.atEnd()) {
        QByteArray line = file.readLine();

        //check format
        if (line.split(separator).count() < 5)
        {
            qDebug() << "Error!" << "\nfile =" << namefile << "\nline =" << indexLine+2;;
            qDebug() << "missing data / invalid format / invalid separator";
            qDebug() << "required separator =" << separator <<"\n";
            return false;
        }

        listData.append(line.split(separator)[0]);

        // save the first date into the struct and check it is a valid date
        if (indexLine == 0)
        {
            inputData->inputFirstDate.year = listData[indexLine].mid(0,4).toInt();
            if (inputData->inputFirstDate.year == 0)
            {
                qDebug() << "Invalid date format ";
                return false;
            }
            inputData->inputFirstDate.month = listData[indexLine].mid(5,2).toInt();
            if (inputData->inputFirstDate.month == 0 || inputData->inputFirstDate.month > 12 )
            {
                qDebug() << "Invalid date format ";
                return false;
            }
            inputData->inputFirstDate.day = listData[indexLine].mid(8,2).toInt();
            if (inputData->inputFirstDate.day == 0 || inputData->inputFirstDate.day > 31)
            {
                qDebug() << "Invalid date format ";
                return false;
            }
        }
        else
        {
            tempDate.year = listData[indexLine].mid(0,4).toInt();
            tempDate.month = listData[indexLine].mid(5,2).toInt();
            tempDate.day = listData[indexLine].mid(8,2).toInt();

            indexDate = difference(inputData->inputFirstDate , tempDate );

            // check LACK of data
            if (indexDate != indexLine)
            {
                // insert nodata row
                listData.removeLast();
                for (int i = indexLine; i < indexDate ; i++)
                {
                    listData.append(noDataString);
                    listTMin.append(noDataString);
                    listTMax.append(noDataString);
                    listPrecip.append(noDataString);
                    indexLine++;
                }
                listData.append(line.split(separator)[0]);
            }
        }

        if (line.split(separator)[1] == "" || line.split(separator)[1] == " " || line.split(separator)[1] == noDataString )
            listTMin.append(QString::number(NODATA));
        else
            listTMin.append(line.split(separator)[1]);

        if (line.split(separator)[2] == "" || line.split(separator)[2] == " " || line.split(separator)[2] == noDataString)
            listTMax.append(QString::number(NODATA));
        else
            listTMax.append(line.split(separator)[2]);

        if (line.split(separator)[4] == "" || line.split(separator)[4] == " " || line.split(separator)[4] == noDataString)
            listPrecip.append(QString::number(NODATA));
        else
            listPrecip.append(line.split(separator)[4]);

        indexLine++;
    }

    // save and check the last date
    inputData->inputLastDate = tempDate;
    if (inputData->inputLastDate.year == 0)
    {
        qDebug() << "Invalid date format ";
        return false;
    }
    if (inputData->inputLastDate.month == 0 || inputData->inputLastDate.month > 12 )
    {
        qDebug() << "Invalid date format ";
        return false;
    }
    if (inputData->inputLastDate.day == 0 || inputData->inputLastDate.day > 31)
    {
        qDebug() << "Invalid date format ";
        return false;
    }

    if (listData.length() != listTMin.length() || (listData.length()!= listTMax.length() ) || (listData.length() != listPrecip.length()) )
    {
        qDebug() << "list data - different size";
        return false;
    }

    inputData->dataLenght = listData.length();

    inputData->inputTMin = (float*)malloc( inputData->dataLenght *sizeof(float));
    inputData->inputTMax = (float*)malloc( inputData->dataLenght *sizeof(float));
    inputData->inputPrecip = (float*)malloc( inputData->dataLenght *sizeof(float));

    for (int i = 0; i < inputData->dataLenght; i++)
    {
        inputData->inputTMin[i] = listTMin[i].toFloat();
        inputData->inputTMax[i] = listTMax[i].toFloat();
        inputData->inputPrecip[i] = listPrecip[i].toFloat();

        // check tmin <= tmax
        if ((inputData->inputTMin[i] != noData) &&  (inputData->inputTMax[i] != noData)
             && (inputData->inputTMin[i] > inputData->inputTMax[i]))
        {
            qDebug() << "WARNING: TMIN > TMAX: " << listData[i];
            // switch
            inputData->inputTMin[i] = listTMax[i].toFloat();
            inputData->inputTMax[i] = listTMin[i].toFloat();
        }

    }
    return true;
}


// write output
bool writeCsv (QString namefile, char separator, ToutputDailyMeteo* mydailyData)
{

    QFile file(namefile);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        qDebug() << file.errorString();
        return false;
    }

    QTextStream stream( &file );

    QString myDate, tMin, tMax, tAvg, prec;
    QString month, day;

    stream << "date" << separator << "tmin" << separator << "tmax" << separator << "tavg"
           << separator << "prec" << separator << "etp" << separator << "watertable\n";

    for (int i=0; i < mydailyData->dataLenght; i++)
    {
        if (mydailyData[i].date.month < 10)
            month = "0" + QString::number(mydailyData[i].date.month);
        else
            month = QString::number(mydailyData[i].date.month);

        if (mydailyData[i].date.day < 10)
            day = "0" + QString::number(mydailyData[i].date.day);
        else
            day = QString::number(mydailyData[i].date.day);

        myDate = QString::number(mydailyData[i].date.year) + "-" + month + "-" + day;
        tMin = QString::number(mydailyData[i].minTemp, 'f', 1);
        tMax = QString::number(mydailyData[i].maxTemp, 'f', 1);
        tAvg = QString::number((mydailyData[i].minTemp + mydailyData[i].maxTemp)*0.5, 'f', 1);
        prec = QString::number(mydailyData[i].prec, 'f', 1);

        stream << myDate << separator << tMin << separator << tMax << separator << tAvg
               << separator << prec << separator << separator << "\n";

    }

    return true;
}

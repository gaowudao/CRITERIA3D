#ifndef DBMETEOGRID_H
#define DBMETEOGRID_H

#include <QString>
#include <QtSql>
#include <QDomElement>

#ifndef METEOGRID_H
    #include "meteoGrid.h"
#endif


struct TXMLConnection
{
    QString provider;
    QString server;
    QString name;
    QString user;
    QString password;
};

struct TXMLfield
{
    QString name;
    QString pragaName;
};

struct TXMLvar
{
    int varCode;
    QString varPragaName;
};

struct TXMLCellsProperties
{
    QString table;
    std::vector<TXMLfield> cellPropertiesField;
};

struct TXMLTable
{
     QString indexTableName;
     QString fieldTime;
     QString fieldVarCode;
     QString fieldValue;
     QString postFix;
     std::vector<TXMLvar> varcode;
};


class DbMeteoGrid
{

    public:

        DbMeteoGrid();

        QString fileName() const;

        TXMLConnection connection() const;

        MeteoGrid gridStructure() const;

        TXMLCellsProperties cellsProperties() const;

        TXMLTable tableDaily() const;

        TXMLTable tableHourly() const;

        bool existHourlyData() const;

        QString tableDailyPrefix() const;

        QString tableDailyPostfix() const;

        QString tableHourlyPrefix() const;

        QString tableHourlyPostfix() const;

        QString tableDailyModel() const;

        QString tableHourlyModel() const;

        bool parseXMLFile(QString xmlFileName, QDomDocument* xmlDoc);

        bool parseXMLGrid(QString xmlFileName);

private:

        QString _fileName;
        TXMLConnection _connection;
        MeteoGrid _gridStructure;
        TXMLCellsProperties _cellsProperties;

        TXMLTable _tableDaily;
        TXMLTable _tableHourly;

        bool _existHourlyData;

        QString _tableDailyPrefix;
        QString _tableDailyPostfix;
        QString _tableHourlyPrefix;
        QString _tableHourlyPostfix;

        QString _tableDailyModel;
        QString _tableHourlyModel;


};

#endif // DBMETEOGRID_H

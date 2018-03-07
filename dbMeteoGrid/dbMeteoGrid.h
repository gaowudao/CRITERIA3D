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

};


struct TXMLCellsProperties
{

};

struct TXMLTableDaily
{

};

struct TXMLTableHourly
{

};

class DbMeteoGrid
{

    public:

        DbMeteoGrid();
        bool parseXMLFile(QString xmlFileName, QDomDocument* xmlDoc);
        bool parseXMLGrid(QString xmlFileName);

    private:

        QString _fileName;
        TXMLConnection _connection;
        MeteoGrid _gridStructure;
        TXMLCellsProperties _cellsProperties;

        TXMLTableDaily _tableDaily;
        TXMLTableHourly _tableHourly;

        bool _existHourlyData;

        QString _tableDailyPrefix;
        QString _tableDailyPostfix;
        QString _tableHourlyPrefix;
        QString _tableHourlyPostfix;

        QString _tableDailyModel;
        QString _tableHourlyModel;


};

#endif // DBMETEOGRID_H

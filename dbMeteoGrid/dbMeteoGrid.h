#ifndef DBMETEOGRID_H
#define DBMETEOGRID_H

#include <QString>
#include <QtSql>
#include <QDomElement>
#include <QMap>

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

struct TXMLTable
{
     QString indexTableName;
     QString fieldTime;
     QString fieldVarCode;
     QString fieldValue;
     QString postFix;
     std::vector<TXMLvar> varcode;
};


class Crit3DMeteoGridDbHandler
{

    public:

        Crit3DMeteoGridDbHandler();

        ~Crit3DMeteoGridDbHandler();

        QSqlDatabase db() const;

        void setDb(const QSqlDatabase &db);

        QString fileName() const;

        TXMLConnection connection() const;

        Crit3DMeteoGridStructure gridStructure() const;

        Crit3DMeteoGrid *meteoGrid() const;

        void setMeteoGrid(Crit3DMeteoGrid *meteoGrid);

        QDate firstDate() const;

        void setFirstDate(const QDate &firstDate);

        QDate lastDate() const;

        void setLastDate(const QDate &lastDate);

        TXMLTable tableDaily() const;

        TXMLTable tableHourly() const;

        QString tableDailyPrefix() const;

        QString tableDailyPostfix() const;

        QString tableHourlyPrefix() const;

        QString tableHourlyPostfix() const;

        QString tableDailyModel() const;

        QString tableHourlyModel() const;

        bool openDatabase(std::string *myError);

        void closeDatabase();

        bool parseXMLFile(QString xmlFileName, QDomDocument* xmlDoc);

        bool parseXMLGrid(QString xmlFileName, std::string *myError);

        int getDailyVarCode(meteoVariable meteoGridDailyVar);

        meteoVariable getDailyVarEnum(int varCode);

        int getHourlyVarCode(meteoVariable meteoGridHourlyVar);

        meteoVariable getHourlyVarEnum(int varCode);

        bool loadCellProperties(std::string *myError);

        bool updateGridDate(std::string *myError);

        bool loadGridDailyData(std::string *myError, QString meteoPoint, QDate first, QDate last);

        bool loadGridHourlyData(std::string *myError, QString meteoPoint, QDateTime first, QDateTime last);


private:

        QString _fileName;
        QSqlDatabase _db;
        TXMLConnection _connection;
        Crit3DMeteoGridStructure _gridStructure;
        Crit3DMeteoGrid* _meteoGrid;

        QDate _firstDate;
        QDate _lastDate;

        TXMLTable _tableDaily;
        TXMLTable _tableHourly;

        QMap<meteoVariable, int> _gridDailyVar;
        QMap<meteoVariable, int> _gridHourlyVar;

        QString _tableDailyPrefix;
        QString _tableDailyPostfix;
        QString _tableHourlyPrefix;
        QString _tableHourlyPostfix;

        QString _tableDailyModel;
        QString _tableHourlyModel;


};

#endif // DBMETEOGRID_H

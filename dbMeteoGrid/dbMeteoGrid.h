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


struct TXMLvar
{
    QString varField;
    int varCode;
    QString varPragaName;
};

struct TXMLTable
{

     QString fieldTime;
     QString fieldVarCode;
     QString fieldValue;
     QString prefix;
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

        QString tableDailyModel() const;

        QString tableHourlyModel() const;

        bool openDatabase(std::string *myError);

        void closeDatabase();

        bool parseXMLFile(QString xmlFileName, QDomDocument* xmlDoc);

        void initMapMySqlVarType();

        bool checkXML(std::string *myError);

        bool parseXMLGrid(QString xmlFileName, std::string *myError);

        int getDailyVarCode(meteoVariable meteoGridDailyVar);

        QString getDailyVarField(meteoVariable meteoGridDailyVar);

        meteoVariable getDailyVarEnum(int varCode);

        meteoVariable getDailyVarFieldEnum(QString varField);

        int getHourlyVarCode(meteoVariable meteoGridHourlyVar);

        QString getHourlyVarField(meteoVariable meteoGridHourlyVar);

        meteoVariable getHourlyVarEnum(int varCode);

        meteoVariable getHourlyVarFieldEnum(QString varField);

        std::string getDailyPragaName(meteoVariable meteoVar);

        std::string getHourlyPragaName(meteoVariable meteoVar);

        bool loadCellProperties(std::string *myError);

        bool updateGridDate(std::string *myError);

        bool loadGridDailyData(std::string *myError, QString meteoPoint, QDate first, QDate last);

        bool loadGridDailyDataFixedFields(std::string *myError, QString meteoPoint, QDate first, QDate last);

        bool loadGridHourlyData(std::string *myError, QString meteoPoint, QDateTime first, QDateTime last);

        bool loadGridHourlyDataFixedFields(std::string *myError, QString meteoPoint, QDateTime first, QDateTime last);

        QList<float> loadGridDailyVar(std::string *myError, QString meteoPoint, meteoVariable variable, QDate first, QDate last, QDate *firstDateDB);

        QList<float> loadGridDailyVarFixedFields(std::string *myError, QString meteoPoint, meteoVariable variable, QDate first, QDate last, QDate* firstDateDB);

        QList<float> loadGridHourlyVar(std::string *myError, QString meteoPoint, meteoVariable variable, QDateTime first, QDateTime last, QDateTime* firstDateDB);

        QList<float> loadGridHourlyVarFixedFields(std::string *myError, QString meteoPoint, meteoVariable variable, QDateTime first, QDateTime last, QDateTime* firstDateDB);

        bool saveCellGridDailyData(std::string *myError, QString meteoPointID, int row, int col, QDate firstDate, QDate lastDate, QList<meteoVariable> meteoVariableList);

        bool saveCellGridDailyDataFF(std::string *myError, QString meteoPointID, int row, int col, QDate firstDate, QDate lastDate);

        bool saveCellCurrrentGridDaily(std::string *myError, QString meteoPointID, QDate date, int varCode, float value);

        bool saveCellCurrentGridDailyFF(std::string *myError, QString meteoPointID, QDate date, QString varPragaName, float value);


        bool saveCellGridHourlyData(std::string *myError, QString meteoPointID, int row, int col, QDateTime firstDate, QDateTime lastDate, QList<meteoVariable> meteoVariableList);

        bool saveCellGridHourlyDataFF(std::string *myError, QString meteoPointID, int row, int col, QDateTime firstDate, QDateTime lastDate);

        bool saveCellCurrentGridHourly(std::string *myError, QString meteoPointID, QDateTime dateTime, int varCode, float value);

        bool saveCellCurrentGridHourlyFF(std::string *myError, QString meteoPointID, QDateTime dateTime, QString varPragaName, float value);




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

        QMap<meteoVariable, QString> _gridDailyVarField;
        QMap<meteoVariable, QString> _gridHourlyVarField;

        QString _tableDailyModel;
        QString _tableHourlyModel;

        QMap<QString, QString> _mapDailyMySqlVarType;
        QMap<QString, QString> _mapHourlyMySqlVarType;


};

#endif // DBMETEOGRID_H

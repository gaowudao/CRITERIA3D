#ifndef DBMETEOPOINTS_H
#define DBMETEOPOINTS_H

#include <QString>
#include <QtSql>

#ifndef CRIT3DDATE_H
    #include "crit3dDate.h"
#endif
#ifndef METEOPOINT_H
    #include "meteoPoint.h"
#endif
#ifndef INTERPOLATIONSETTINGS_H
    #include "interpolationSettings.h"
#endif

class Crit3DProxyMeteoPoint : public Crit3DProxy
{
private:
    std::string proxyTable;
    std::string proxyField;

public:
    Crit3DProxyMeteoPoint();
    Crit3DProxyMeteoPoint(Crit3DProxy myProxy);
    std::string getProxyField() const;
    void setProxyField(const std::string &value);
    std::string getProxyTable() const;
    void setProxyTable(const std::string &value);
};

class Crit3DMeteoPointsDbHandler : public QObject
{
    Q_OBJECT

private:
    std::vector <Crit3DProxyMeteoPoint> ProxyMeteoPoint;

public:
    explicit Crit3DMeteoPointsDbHandler(QString dbName);
        ~Crit3DMeteoPointsDbHandler();
        void dbManager();
        int getIdfromMeteoVar(meteoVariable meteoVar);
        QString getDatasetURL(QString dataset);
        QString getDbName();
        QStringList getDatasetsList();
        QStringList getDatasetsActive();
        void setDatasetsActive(QString active);
        QDateTime getLastDay(frequencyType frequency);
        QDateTime getFirstDay(frequencyType frequency);

        void initializeProxy();
        void addProxy(Crit3DProxy myProxy, std::string tableName_, std::string fieldName_);
        bool readPointProxyValues(Crit3DMeteoPoint* myPoint);

        bool writePointProperties(Crit3DMeteoPoint* pointProp);
        QList<Crit3DMeteoPoint> getPropertiesFromDb(const gis::Crit3DGisSettings& gisSettings);
        bool getDailyData(Crit3DDate dateStart, Crit3DDate dateEnd, Crit3DMeteoPoint *meteoPoint);
        std::vector<float> getDailyVar(std::string *myError, meteoVariable variable, Crit3DDate dateStart, Crit3DDate dateEnd, QDate* firstDateDB, Crit3DMeteoPoint *meteoPoint);
        bool getHourlyData(Crit3DDate dateStart, Crit3DDate dateEnd, Crit3DMeteoPoint *meteoPoint);
        std::vector<float> getHourlyVar(std::string *myError, meteoVariable variable, Crit3DDate dateStart, Crit3DDate dateEnd, QDateTime* firstDateDB, Crit3DMeteoPoint *meteoPoint);
        void closeDatabase();
    protected:
        QSqlDatabase _db;
    signals:

    protected slots:
};


#endif // DBMETEOPOINTS_H

#ifndef DBMETEOPOINTS_H
#define DBMETEOPOINTS_H

    #ifndef CRIT3DDATE_H
        #include "crit3dDate.h"
    #endif
    #ifndef METEOPOINT_H
        #include "meteoPoint.h"
    #endif
    #ifndef INTERPOLATIONSETTINGS_H
        #include "interpolationSettings.h"
    #endif

    #ifndef QSQLDATABASE_H
        #include <QSqlDatabase>
    #endif
    #ifndef QOBJECT_H
        #include <QObject>
    #endif


    class Crit3DMeteoPointsDbHandler : public QObject
    {
        Q_OBJECT

    public:
        QString error;

        explicit Crit3DMeteoPointsDbHandler(QString dbname_);
        explicit Crit3DMeteoPointsDbHandler(QString provider_, QString host_, QString dbname_, int port_, QString user_, QString pass_);

            ~Crit3DMeteoPointsDbHandler();
            void dbManager();
            QString getDatasetURL(QString dataset);
            QString getDbName();
            QStringList getDatasetsList();
            QStringList getDatasetsActive();
            void setDatasetsActive(QString active);
            QDateTime getLastDate(frequencyType frequency);
            QDateTime getFirstDay(frequencyType frequency);

            bool readPointProxyValues(Crit3DMeteoPoint* myPoint, Crit3DInterpolationSettings* interpolationSettings);

            bool writePointProperties(Crit3DMeteoPoint* pointProp);
            QList<Crit3DMeteoPoint> getPropertiesFromDb(const gis::Crit3DGisSettings& gisSettings, QString *errorString);
            bool loadDailyData(Crit3DDate dateStart, Crit3DDate dateEnd, Crit3DMeteoPoint *meteoPoint);
            std::vector<float> loadDailyVar(QString *myError, meteoVariable variable, Crit3DDate dateStart, Crit3DDate dateEnd, QDate* firstDateDB, Crit3DMeteoPoint *meteoPoint);
            bool loadHourlyData(Crit3DDate dateStart, Crit3DDate dateEnd, Crit3DMeteoPoint *meteoPoint);
            std::vector<float> loadHourlyVar(QString *myError, meteoVariable variable, Crit3DDate dateStart, Crit3DDate dateEnd, QDateTime* firstDateDB, Crit3DMeteoPoint *meteoPoint);
            void closeDatabase();
            QSqlDatabase getDb() const;
            void setDb(const QSqlDatabase &db);
            bool loadVariableProperties();
            int getIdfromMeteoVar(meteoVariable meteoVar);
            std::map<int, meteoVariable> getMapIdMeteoVar() const;

    protected:
            QSqlDatabase _db;
            std::map<int, meteoVariable> _mapIdMeteoVar;
    signals:

    protected slots:
    };


#endif // DBMETEOPOINTS_H
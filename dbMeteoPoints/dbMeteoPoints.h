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

class DbMeteoPoints : public QObject
{
    Q_OBJECT
    public:
        explicit DbMeteoPoints(QString dbName);
        ~DbMeteoPoints();
        void dbManager();
        QString getDatasetURL(QString dataset);
        QString getDbName();
        QStringList getDatasetsList();
        QStringList getDatasetsActive();
        void setDatasetsActive(QString active);
        QDateTime getLastDay(char dayHour);
        QDateTime getFirstDay(char dayHour);
        bool fillPointProperties(Crit3DMeteoPoint* pointProp);
        QList<Crit3DMeteoPoint> getPropertiesFromDb();
        void getDailyData(Crit3DDate dateStart, Crit3DDate dateEnd, Crit3DMeteoPoint *meteoPoint);
        void getHourlyData(Crit3DDate dateStart, Crit3DDate dateEnd, Crit3DMeteoPoint *meteoPoint);
    protected:
        QSqlDatabase _db;
    signals:

    protected slots:

};


#endif // DBMETEOPOINTS_H

#ifndef DBMETEOPOINTS_H
#define DBMETEOPOINTS_H

#include <QString>
#include <QtSql>

struct TPointProperties {
    int id;
    QString name;
    QString network;
    double lat;
    double lon;
    int latInt;
    int lonInt;
    double utm_x;
    double utm_y;
    float altitude;
    QString state;
    QString region;
    QString province;
    QString municipality;
};



class DbMeteoPoints : public QObject
{
    Q_OBJECT
    public:
        explicit DbMeteoPoints(QString dbName);
        ~DbMeteoPoints();
        void dbManager();
        QString getDatasetURL(QString dataset);
        QStringList getDatasetsActive();
        QString getVarName(int id);
        QList<int> getDailyVar();
        QList<int> getHourlyVar();
        int getId(QString VarName);
        bool fillPointProperties(TPointProperties* pointProp);
    private:
        QSqlDatabase _db;
    signals:

    protected slots:

};


#endif // DBMETEOPOINTS_H

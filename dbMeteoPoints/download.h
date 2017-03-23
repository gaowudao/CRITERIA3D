#ifndef DOWNLOAD_H
#define DOWNLOAD_H

#include <QNetworkRequest>
#include <QtNetwork>
#include "dbMeteoPoints.h"
#ifndef CRIT3DDATE_H
    #include "crit3dDate.h"
#endif
#ifndef GIS_H
    #include "gis.h"
#endif


class Download : public QObject
{
    Q_OBJECT
    public:
        explicit Download(QString dbName, QObject* parent = 0);
        ~Download();
        void getPointProperties(QStringList datasetList);
        void downloadMetadata(QJsonObject obj);
        void downloadDailyVar(Crit3DDate dateStart, Crit3DDate dateEnd, QStringList datasets, QList<int> stations, QList<int> variables, bool precSelection);
        void downloadHourlyVar(Crit3DTime dateStartTime, Crit3DTime dateEndTime, QStringList datasets, QList<int> stations, QList<int> variables);
        void debugFromFile(); //cancellare

    private:
        //QNetworkAccessManager* _manager;
        QStringList _datasetsList;
        DbMeteoPoints* _dbMeteo;

        static const QByteArray _authorization;
//    signals:
//        void finished(QNetworkReply*);
//    protected slots:
//        void getPointPropertiesReady(QNetworkReply* reply);
};

#endif // DOWNLOAD_H

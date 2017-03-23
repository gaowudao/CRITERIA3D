#include "download.h"
#include "dbMeteoPoints.h"
#include "commonConstants.h"
#include "variableslist.h"

#include <QEventLoop>

#include <QDebug>
#include <QObject>
#include <QFile> //debug
#include <QTextStream> //debug

#include <QStringBuilder>

const QByteArray Download::_authorization = QString("Basic " + QString("ugo:Ul1ss&").toLocal8Bit().toBase64()).toLocal8Bit();

Download::Download(QString dbName, QObject* parent) : QObject(parent)
{
    _dbMeteo = new DbMeteoPoints(dbName);
}

Download::~Download()
{
    delete _dbMeteo;
}

void Download::getPointProperties(QStringList datasetList)
{

    QEventLoop loop;

    QNetworkAccessManager* manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));

    _datasetsList = datasetList;

    QNetworkRequest request;
    request.setUrl(QUrl("http://meteozen.metarpa/simcstations/api/stations"));
    request.setRawHeader("Authorization", _authorization);

    QNetworkReply* reply = manager->get(request);  // GET

    loop.exec();

    if (reply->error() != QNetworkReply::NoError)
            qDebug( "Error!" );
    else
    {

        QString data = (QString) reply->readAll();

        QJsonParseError *error = new QJsonParseError();
        QJsonDocument doc = QJsonDocument::fromJson(data.toUtf8(), error);
        //qDebug() << data.length();

        qDebug() << "err: " << error->errorString() << " -> " << error->offset;

        ///////////////////debug/////////////
//        QString filename = "debug.txt";
//            QFile file(filename);
//            if (file.open(QIODevice::ReadWrite)) {
//                QTextStream stream(&file);
//                stream << data << endl;
//                stream.flush();
//                file.close();
//            }
        ///////////////////
        // check validity of the document
        if(!doc.isNull() && doc.isArray())
        {
            QJsonArray jsonArr = doc.array();

            for(int index=0; index<jsonArr.size(); ++index)
            {
                QJsonObject obj = jsonArr[index].toObject();
                foreach(QString item, _datasetsList)
                {
                    QJsonValue jsonNetwork = obj.value("network");

                    if (jsonNetwork.isUndefined())
                            qDebug() << "Key id does not exist";
                    else if (!jsonNetwork.isString())
                        qDebug() << "Value not string";
                    else
                    {
                        if (jsonNetwork == item)
                        {
                            qDebug() << jsonNetwork;
                            this->downloadMetadata(obj);
                            //qDebug() << jsonNetwork.toString();
                        }
                    }
                }
            }
        }
         else
            qDebug() << "Invalid JSON...\n" << endl;
    }

    delete reply;
    delete manager;
}

// da cancellare, solo x test senza bisogno della rete
void Download::debugFromFile()
{

    QString data;
    _datasetsList << "agrmet" << "cer" ;

    qDebug() << "_datasetsList" << _datasetsList << endl;
        QString filename = "stations_aut.txt";
        QFile file(filename);
        if (file.open(QIODevice::ReadWrite))
        {
                QTextStream in(&file);
                data = in.readAll();
                file.close();
         }

        QJsonParseError *error = new QJsonParseError();
        QJsonDocument doc = QJsonDocument::fromJson(data.toUtf8(), error);
        qDebug() << data.length();

        qDebug() << "err: " << error->errorString() << " -> " << error->offset;


        if(!doc.isNull() && doc.isArray())
        {
            QJsonArray jsonArr = doc.array();
            qDebug() << "elems: " << jsonArr.size() << endl;

            for(int index=0; index<jsonArr.size(); index++)
            {
                QJsonObject obj = jsonArr[index].toObject();

                foreach(QString item, _datasetsList)
                {
                    QJsonValue jsonNetwork = obj.value("network");

                    if (jsonNetwork.isUndefined())
                            qDebug() << "Key id does not exist" << endl;
                    else if (!jsonNetwork.isString())
                        qDebug() << "Value not string" << endl;
                    else
                    {
                        if (jsonNetwork == item)
                        {
                            qDebug() << "jsonNetwork metadata: " << jsonNetwork << endl;
                            this->downloadMetadata(obj);
                            //qDebug() << jsonNetwork.toString();
                        }
                    }
                }
            }
        }
         else
            qDebug() << "Invalid JSON..." << endl;
 }


void Download::downloadMetadata(QJsonObject obj)
{
    qDebug() << "downloadMetadata";

    TPointProperties* pointProp = new TPointProperties();

    QJsonValue jsonId = obj.value("id");
    if (jsonId.isNull())
          qDebug() << "Id is null" << endl;
    pointProp->id = jsonId.toInt();

    QJsonValue jsonName = obj.value("name");
    if (jsonName.isNull())
          qDebug() << "name is null" << endl;
    pointProp->name = jsonName.toString();

    QJsonValue jsonNetwork = obj.value("network");
    pointProp->network = jsonNetwork.toString();

    QJsonValue jsonGeometry = obj.value("geometry").toObject().value("coordinates");
    QJsonValue jsonLon = jsonGeometry.toArray()[0];
    if (jsonLon.isNull() || jsonLon.toInt() < -180 || jsonLon.toInt() > 180)
        qDebug() << "invalid Longitude" << endl;
    pointProp->lon = jsonLon.toDouble();

    QJsonValue jsonLat = jsonGeometry.toArray()[1];
    if (jsonLat.isNull() || jsonLat.toInt() < -90 || jsonLat.toInt() > 90)
        qDebug() << "invalid Latitude" << endl;
    pointProp->lat = jsonLat.toDouble();

    QJsonValue jsonLatInt = obj.value("lat");
    if (jsonLatInt.isNull())
        jsonLatInt = NODATA;
    pointProp->latInt = jsonLatInt.toInt();

    QJsonValue jsonLonInt = obj.value("lon");
    if (jsonLonInt.isNull())
        jsonLonInt = NODATA;
    pointProp->lonInt = jsonLonInt.toInt();

    QJsonValue jsonAltitude = obj.value("height");
    pointProp->altitude = jsonAltitude.toString().toFloat();

    QJsonValue jsonState = obj.value("country").toObject().value("name");
    pointProp->state = jsonState.toString();

    if (obj.value("region").isNull())
        pointProp->region = "";
    else
    {
        QJsonValue jsonRegion = obj.value("region").toObject().value("name");
        pointProp->region = jsonRegion.toString();
    }

    if (obj.value("province").isNull())
        pointProp->province = "";
    else
    {
        QJsonValue jsonProvince = obj.value("province").toObject().value("name");
        pointProp->province = jsonProvince.toString();
    }

    if (obj.value("municipality").isNull())
        pointProp->municipality = "";
    else
    {
        QJsonValue jsonMunicipality = obj.value("municipality").toObject().value("name");
        pointProp->municipality = jsonMunicipality.toString();
    }

    double utmx, utmy;
    int utmZone = 32; // dove far inserire la utmZone? c'è funzione che data lat,lon restituisce utm zone?
    gis::latLonToUtmForceZone(utmZone, pointProp->lat, pointProp->lon, &utmx, &utmy);
    pointProp->utm_x = utmx;
    pointProp->utm_y = utmy;

    _dbMeteo->fillPointProperties(pointProp);

}


void Download::downloadDailyVar(Crit3DDate dateStart, Crit3DDate dateEnd, QStringList datasets, QList<int> stations, QList<int> variables, bool precSelection)
{

    // create station tables
    _dbMeteo->initStationsDailyTables(dateStart, dateEnd, stations);

    QString area;

    area = QString(";area: VM2,%1").arg(stations[0]);

    for (int i = 1; i < stations.size(); i++)
    {
        area = area % QString(" or VM2,%1").arg(stations[i]);
    }

    QString product;

    // LC variables non sarà mai vuoto, altrimenti scaricherebbe tutte le variabili, anche le orarie e semiorarie. Nella funzione che chiama downloadDailyVar passare la lista
    // di tutte le variabili giornaliere. Stessa cosa per le stazioni, l'array lo riempie opportunamente la funzione che chiama downloadDailyVar

    product = QString(";product: VM2,%1").arg(variables[0]);

    for (int i = 1; i < variables.size(); i++)
    {
        product = product % QString(" or VM2,%1").arg(variables[i]);
    }

//    if (!variables.empty()) {

//        product = QString(";product: VM2,%1").arg(variables[0]);

//        for (int i = 1; i < variables.size(); i++)
//        {
//            product = product % QString(" or VM2,%1").arg(variables[i]);
//        }
//    }

    for (Crit3DDate i = dateStart.addDays(180); dateEnd >= dateStart; i = dateStart.addDays(180))
    {

        if (i > dateEnd)
            i = dateEnd;

        // reftime
        QString refTime = QString("reftime:>=%1,<=%2").arg(QString::fromStdString(dateStart.toStdString())).arg(QString::fromStdString(i.toStdString()));

        foreach (QString dataset, datasets) {

            QEventLoop loop;

            QNetworkAccessManager* manager = new QNetworkAccessManager(this);
            connect(manager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));

            QUrl url = QUrl(QString("%1/query?query=%2%3%4&style=postprocess").arg(_dbMeteo->getDatasetURL(dataset)).arg(refTime).arg(area).arg(product));

            qDebug() << url ;

            QNetworkRequest request;
            request.setUrl(url);
            request.setRawHeader("Authorization", _authorization);

            QNetworkReply* reply = manager->get(request);  // GET
            loop.exec();

            if (reply->error() != QNetworkReply::NoError)
                    qDebug( "Error!" );
            else
            {

                for (QString line = QString(reply->readLine()); !(line.isNull() || line.isEmpty());  line = QString(reply->readLine()))
                {

                    QStringList fields = line.split(",");

                    QString date = QString("%1-%2-%3 %4:%5:00").arg(fields[0].left(4))
                                                               .arg(fields[0].mid(4, 2))
                                                               .arg(fields[0].mid(6, 2))
                                                               .arg(fields[0].mid(8, 2))
                                                               .arg(fields[0].mid(10, 2));
                    QString station = fields[1];
                    int arkId = fields[2].toInt();
                    double varValue = fields[3].toDouble();
                    QString flag = fields[6];


                    if (arkId == PREC_ID) {

                        if ((precSelection && fields[0].mid(8,2) == "08") || (!precSelection && fields[0].mid(8,2) == "00"))
                        {
                                continue;
                        }
                        else if (!precSelection && fields[0].mid(8,2) == "08")
                        {
                            date = QString("%1-%2-%3 00:%4:00").arg(fields[0].left(4))
                                                              .arg(fields[0].mid(4, 2))
                                                              .arg(fields[0].mid(6, 2))
                                                              .arg(fields[0].mid(10, 2));
                        }

                    }

//                    conversion from average daily radiation to integral radiation
                    if (arkId == RAD_ID)
                    {
                        varValue *= DAILY_TO_INTEGRAL_RAD;
                    }

                    int id = _dbMeteo->arkIdmap(arkId);

                    if (!(station.isEmpty() || station.isEmpty()))
                    {
                        _dbMeteo->insertVarValue(station, date, id, varValue, flag);
                    }

                    dateStart = i.addDays(1);

                }
            }

            delete reply;
            delete manager;
        }

    }


}

//TO DO
void Download::downloadHourlyVar(Crit3DTime dateStartTime, Crit3DTime dateEndTime, QStringList datasets, QList<int> stations, QList<int> variables)
{
    // create station tables
    _dbMeteo->initStationsHourlyTables(dateStartTime, dateEndTime, stations);

    QString area;

    area = QString(";area: VM2,%1").arg(stations[0]);

    for (int i = 1; i < stations.size(); i++)
    {
        area = area % QString(" or VM2,%1").arg(stations[i]);
    }

    QString product;

    QList<VariablesList> variableList = _dbMeteo->getHourlyVarFields(variables);

//    qDebug() << "arkId[0] = " << variableList.at(0).arkId();
//    qDebug() << "id[0] = " << variableList.at(0).id();
//    qDebug() << "varName[0] = " << variableList.at(0).varName();
//    qDebug() << "frequency[0] = " << variableList.at(0).frequency();

    product = QString(";product: VM2,%1").arg(variables[0]);

    for (int i = 1; i < variables.size(); i++)
    {
        product = product % QString(" or VM2,%1").arg(variables[i]);
    }

    int nrData = (difference(dateStartTime.date.addDays(1), dateEndTime.date) * 24 * stations.size());

    int nrSteps = qMax(1, (nrData / datasets.size()) / 3000);
    int stepDate = qMax(1, difference(dateStartTime.date.addDays(1), dateEndTime.date) / nrSteps);

    dateStartTime = dateStartTime.addSeconds(-1800);
    dateEndTime.date = dateEndTime.date.addDays(1);
    dateEndTime = dateEndTime.addSeconds(-3600);



    Crit3DTime i = dateStartTime;

    for (i.date = dateStartTime.date.addDays(stepDate); dateEndTime >= dateStartTime; i.date = dateStartTime.date.addDays(stepDate))
    {


        if (i > dateEndTime)
            i = dateEndTime;
       else
            i = i.addSeconds(-1800);


        // reftime
        QString refTime = QString("reftime:>=%1,<=%2").arg(QString::fromStdString(dateStartTime.toStdString())).arg(QString::fromStdString(i.toStdString()));
        qDebug() << refTime;

        _dbMeteo->createTmpTable("TmpHourlyData");

        foreach (QString dataset, datasets)
        {

            QEventLoop loop;

            QNetworkAccessManager* manager = new QNetworkAccessManager(this);
            connect(manager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));

            QUrl url = QUrl(QString("%1/query?query=%2%3%4&style=postprocess").arg(_dbMeteo->getDatasetURL(dataset)).arg(refTime).arg(area).arg(product));

            qDebug() << url ;

            QNetworkRequest request;
            request.setUrl(url);
            request.setRawHeader("Authorization", _authorization);

            QNetworkReply* reply = manager->get(request);  // GET
            loop.exec();

            if (reply->error() != QNetworkReply::NoError)
                    qDebug( "Error!" );
            else
            {

                for (QString line = QString(reply->readLine()); !(line.isNull() || line.isEmpty());  line = QString(reply->readLine()))
                {

                    QStringList fields = line.split(",");

                    QString date = QString("%1-%2-%3 %4:%5:00").arg(fields[0].left(4))
                                                               .arg(fields[0].mid(4, 2))
                                                               .arg(fields[0].mid(6, 2))
                                                               .arg(fields[0].mid(8, 2))
                                                               .arg(fields[0].mid(10, 2));
                    QString station = fields[1];
                    int arkId = fields[2].toInt();
                    double varValue = fields[3].toDouble();
                    QString flag = fields[6];

                    dateStartTime = i.addSeconds(1800);

                }
            }

            delete reply;
            delete manager;
         }

    }
}

#include "download.h"
#include "dbMeteoPoints.h"
#include "commonConstants.h"

#include <QDebug>
#include <QObject>
#include <QFile> //debug
#include <QTextStream> //debug



Download::Download(QString dbName, QObject* parent)
{

    _dbMeteo = new DbMeteoPoints(dbName);

    _manager = new QNetworkAccessManager(this);
    connect(_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
}

Download::~Download()
{
    delete _manager;
    delete _dbMeteo;
}

void Download::getPointProperties(QStringList datasetList)
{

    _datasetsList = datasetList;

    QNetworkRequest request;
    QNetworkReply* reply;

    QString url = "http://meteozen.metarpa/simcstations/api/stations";

    QUrl stations(url);
    // HTTP Basic authentication header value: base64(username:password)
    QString concatenated = "ugo:Ul1ss&";  //username:password
    QByteArray data = concatenated.toLocal8Bit().toBase64();
    QString headerData = "Basic " + data;
    qDebug() << "headerData" << headerData;
    request.setUrl(stations);
    request.setRawHeader("Authorization", headerData.toLocal8Bit());

    reply = _manager->get(request);  // GET

}

// DA CONTROLLARE
void Download::replyFinished(QNetworkReply* reply)
{

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


void Download::downloadDailyVar(Crit3DDate dataStartInput, Crit3DDate dataEndInput, QStringList dataset, QList<int> station, QList<int> variable)
{

    // x ogni dataset chiamare la getDatasetURL, a cui concatenare
    // sURL = "http://arkioss.metarpa:8090/dataset/cer" & "/query?query=" & reftime & ";area: " & stationlist & ";product:" & variablelist & "&style=postprocess"
    // se station è array vuoto -> tutte le stazioni (omettere nell'url area: in modo le prenda tutte), se variable è array vuoto -> tutte le variabili giornaliere getDailyVar

}

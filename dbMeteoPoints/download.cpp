#include "download.h"
#include "dbMeteoPoints.h"
#include "commonConstants.h"

#include <QDebug>
#include <QtNetwork>
//#include <QNetworkRequest>


void getPointProperties(QString dbName, QStringList datasetsList )
{

    foreach(QString item, datasetsList)
    {
        // si connette al dbName e prende il giusto URL dalla tabella datasets

        QString url = getURL(dbName,item);

        if (url.isNull()) {
            qDebug () << item << " not found";
            continue;
        }

        qDebug () << url;
        //QUrl datasetUrl(url);
        //QNetworkRequest request(datasetUrl);
        //request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

        // fa la chiamata http con basic autor.

        // parsing JSON e salvataggio nella tabella Point Properties
    }
}

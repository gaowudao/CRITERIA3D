#include "queryArcgis.h"
#include "ServiceError.h"
#include <iostream>

#include <QString>
#include <QDebug>

#include "project.h"

queryArcgis::queryArcgis(QString myURL, MOSESProject *project, QObject *parent) : QObject(parent)
{
    this->isError = false;
    this->error = "";
    this->myProject = project;

    this->myQueryTask = new EsriRuntimeQt::QueryTask(myURL, this);
    connect(this->myQueryTask, SIGNAL(queryTaskComplete(EsriRuntimeQt::FeatureSet*)), this, SLOT(onQueryTaskComplete(EsriRuntimeQt::FeatureSet*)));
    connect(this->myQueryTask, SIGNAL(queryTaskError(EsriRuntimeQt::ServiceError)), this, SLOT(onQueryTaskError(const EsriRuntimeQt::ServiceError)));
}

queryArcgis::~queryArcgis()
{
}

void queryArcgis::executeQuery()
{
    EsriRuntimeQt::Query query;

    // Before initializing ArcGIS Runtime, first set the
    // ArcGIS Runtime license setting required for your application.
    EsriRuntimeQt::ArcGISRuntime::setClientId("BeCHnTS3B0PhVtCw");

    query.setOutFields(QStringList() << "ID_CASE" << "ECM_CODE" << "ID_SOIL" << "ID_METEO" << "ID_FORECAST");

    query.setReturnGeometry(false);
    query.setReturnDistinctValues(true);

    query.setWhere("1=1");

    //qDebug() << "\nSELECT DISTINCT" << query.outFields();
    //qDebug() << "FROM" << this->myQueryTask->url();

    myQueryTask->executeAndWait(query);
}


void queryArcgis::onQueryTaskComplete(EsriRuntimeQt::FeatureSet* featureSet)
{
    int nr = featureSet->graphics().length();

    if (!featureSet || nr < 1)
    {
      this->isError = true;
      this->error = "No records.";
      return;
    }

    myProject->initializeUnit(nr);

    // attributes of all states obtained after executing the query.
    int i = 0;
    foreach (EsriRuntimeQt::Graphic* state, featureSet->graphics())
    {
        if (!state) continue;

        myProject->unit[i].idCase = state->attributeValue("ID_CASE").toString();
        myProject->unit[i].idCropMoses = state->attributeValue("ECM_CODE").toString();
        myProject->unit[i].idMeteo = state->attributeValue("ID_METEO").toString();
        myProject->unit[i].idForecast = state->attributeValue("ID_FORECAST").toString();
        myProject->unit[i].idSoilNumber = state->attributeValue("ID_SOIL").toInt();

        i++;
    }

      featureSet->deleteLater();
}


void queryArcgis::onQueryTaskError(const EsriRuntimeQt::ServiceError &error)
{
   this->isError = true;
   this->error = error.message();
}

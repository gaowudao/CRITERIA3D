#ifndef QUERY_CROP
#define QUERY_CROP

#include <QObject>
#include "ArcGISRuntime.h"
#include "QueryTask.h"

class MOSESProject;

class queryArcgis : public QObject
{
    Q_OBJECT

    public:
        bool isError;
        QString error;
        MOSESProject *myProject;

        EsriRuntimeQt::QueryTask *myQueryTask;
        queryArcgis(QString myURL, MOSESProject *myProject, QObject *parent=0);
        ~queryArcgis();

        void executeQuery();

    public slots:
        void onQueryTaskComplete(EsriRuntimeQt::FeatureSet* featureSet);
        void onQueryTaskError(const EsriRuntimeQt::ServiceError &error);

    private:
        void Init();
};
#endif // QUERY_CROP


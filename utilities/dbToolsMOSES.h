#ifndef DBTOOLSMOSES_H
#define DBTOOLSMOSES_H

    #include <string>
    class QSqlDatabase;
    class QSqlQuery;
    class QString;
    class Crit3DMeteoPoint;

    float getMOSESIrriRatio(QSqlDatabase* dbCrop, QString idCrop, QString *myError);
    bool readMOSESDailyData(QSqlQuery *query, Crit3DMeteoPoint *meteoPoint, QString *myError);

#endif // DBTOOLSMOSES_H

#ifndef DBTOOLSMOSES_H
#define DBTOOLSMOSES_H

    class QSqlQuery;
    class QString;
    class Crit3DMeteoPoint;

    bool readMOSESDailyData(QSqlQuery *query, Crit3DMeteoPoint *meteoPoint, QString *myError);

#endif // DBTOOLSMOSES_H

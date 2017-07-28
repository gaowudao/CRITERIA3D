#ifndef DBTOOLSMOSES_H
#define DBTOOLSMOSES_H

    #include <string>
    class QSqlDatabase;
    class QSqlQuery;
    class QString;
    class Crit3DMeteoPoint;

    float getMOSESIrriRatio(QSqlDatabase* dbCrop, QString idCrop, std::string *myError);
    QString getMOSESIdCrop(QSqlDatabase* dbCrop, QString idCropMOSES, std::string *myError);
    bool readMOSESDailyData(QSqlQuery *query, Crit3DMeteoPoint *meteoPoint, std::string *myError);

#endif // DBTOOLSMOSES_H

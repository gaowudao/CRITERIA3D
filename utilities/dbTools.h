#ifndef DBTOOLS_H
#define DBTOOLS_H

    #include <string>
    #include "soil.h"
    class Crit3DCrop;
    class QSqlDatabase;
    class QString;

    // CROP
    QString getCropFromClass(QSqlDatabase* dbCrop, QString cropClassTable, QString cropClassField, QString idCropClass, QString *myError);
    QString getCropFromId(QSqlDatabase* dbCrop, QString cropClassTable, QString cropIdField, int cropId, QString *myError);
    float getIrriRatioFromClass(QSqlDatabase* dbCrop, QString cropClassTable, QString cropClassField, QString idCrop, QString *myError);
    float getIrriRatioFromId(QSqlDatabase* dbCrop, QString cropClassTable, QString cropIdField, int cropId, QString *myError);

    bool loadCropParameters(QString idCrop, Crit3DCrop* myCrop, QSqlDatabase* dbCrop, QString *myError);

    // SOIL
    QString getIdSoilString(QSqlDatabase* dbSoil, int idSoilNumber, QString *myError);
    bool loadSoil(QSqlDatabase* dbSoil, QString soilCode, soil::Crit3DSoil *mySoil,
                  soil::Crit3DSoilClass *soilTexture, QString *myError);
    bool loadVanGenuchtenParameters(soil::Crit3DSoilClass *soilTexture, QSqlDatabase* dbParameters, QString *myError);
    bool loadDriessenParameters(soil::Crit3DSoilClass *soilTexture, QSqlDatabase* dbParameters, QString *myError);

#endif // DBTOOLS_H

#ifndef DBTOOLS_H
#define DBTOOLS_H

    #include <string>
    #include "soil.h"
    class Crit3DCrop;
    class QSqlDatabase;
    class QString;

    bool loadCropParameters(QString idCrop, Crit3DCrop* myCrop, QSqlDatabase* dbCrop, std::string *myError);
    bool loadVanGenuchtenParameters(soil::Crit3DSoilClass *soilTexture, QSqlDatabase* dbParameters, std::string *myError);
    bool loadDriessenParameters(soil::Crit3DSoilClass *soilTexture, QSqlDatabase* dbParameters, std::string *myError);
    QString getIdSoil(QSqlDatabase* dbSoil, int idSoilNumber, std::string *myError);
    bool loadSoil(QSqlDatabase* dbSoil, QString soilCode, soil::Crit3DSoil *mySoil, soil::Crit3DSoilClass *soilTexture, std::string *myError);

#endif // DBTOOLS_H

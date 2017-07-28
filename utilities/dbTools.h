#ifndef DBTOOLS_H
#define DBTOOLS_H

    #include <string>
    class Crit3DCrop;
    class QSqlDatabase;
    class Crit3DSoilClass;

    bool loadCropParameters(std::string idCropString, Crit3DCrop* myCrop, QSqlDatabase* dbCrop, std::string *myError);
    bool loadVanGenuchtenParameters(Crit3DSoilClass *soilTexture, QSqlDatabase* dbParameters, std::string *myError);
    bool loadDriessenParameters(Crit3DSoilClass *soilTexture, QSqlDatabase* dbParameters, std::string *myError);


#endif // DBTOOLS_H

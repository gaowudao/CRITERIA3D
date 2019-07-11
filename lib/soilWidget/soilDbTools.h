#ifndef SOILDBTOOLS_H
#define SOILDBTOOLS_H

    #ifndef _VECTOR_
        #include <vector>
    #endif
    #ifndef SOIL_H
        #include "soil.h"
    #endif

    class QString;
    class QSqlDatabase;

    bool loadSoilData(QSqlDatabase* dbSoil, QString soilCode, soil::Crit3DSoil *mySoil, QString *myError);

    bool loadSoil(QSqlDatabase* dbSoil, QString soilCode, soil::Crit3DSoil* mySoil,
                  soil::Crit3DSoilClass* soilClassList, QString* error);

    bool loadVanGenuchtenParameters(QSqlDatabase* dbSoil, soil::Crit3DSoilClass* soilClassList, QString *error);

    bool loadDriessenParameters(QSqlDatabase* dbSoil, soil::Crit3DSoilClass* soilClassList, QString *error);

    QString getIdSoilString(QSqlDatabase* dbSoil, int idSoilNumber, QString *myError);

    bool openDbSoil(QString dbName, QSqlDatabase* dbSoil, QString* error);

    bool loadAllSoils(QString dbSoilName, std::vector <soil::Crit3DSoil> *soilList, soil::Crit3DSoilClass *soilClassList, QString* error);


#endif // SOILDBTOOLS_H

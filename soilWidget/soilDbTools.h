#ifndef SOILDBTOOLS_H
#define SOILDBTOOLS_H

    #ifndef SOIL_H
        #include "soil.h"
    #endif
    #ifndef QSTRING_H
        #include <QString>
    #endif
    #ifndef _VECTOR_
        #include <vector>
    #endif
    class QSqlDatabase;

    bool loadSoil(QSqlDatabase* dbSoil, QString soilCode, soil::Crit3DSoil *mySoil,
                  soil::Crit3DSoilClass *soilTexture, QString *myError);

    bool loadDbSoilData(QString dbSoilName, QString soilCode, soil::Crit3DSoil *mySoil, QString *myError);

    bool loadVanGenuchtenParameters(soil::Crit3DSoilClass *soilClassList,
                                    QSqlDatabase* dbSoil, QString *myError);

    bool loadDriessenParameters(soil::Crit3DSoilClass *soilTexture,
                                QSqlDatabase* dbSoil, QString *myError);

    QString getIdSoilString(QSqlDatabase* dbSoil, int idSoilNumber, QString *myError);

    bool openDbSoil(QString dbName, QSqlDatabase* dbSoil, QString* error);

    bool loadAllSoils(QString dbSoilName, std::vector <soil::Crit3DSoil> *soilList, soil::Crit3DSoilClass *soilClassList, QString* error);


#endif // SOILDBTOOLS_H

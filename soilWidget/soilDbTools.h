#ifndef SOILDBTOOLS_H
#define SOILDBTOOLS_H

    #ifndef SOIL_H
        #include "soil.h"
    #endif
    class QSqlDatabase;
    class QString;

    bool loadSoil(QSqlDatabase* dbSoil, QString soilCode, soil::Crit3DSoil *mySoil,
                  soil::Crit3DSoilClass *soilTexture, QString *myError);

    bool loadVanGenuchtenParameters(soil::Crit3DSoilClass *soilTexture,
                                    QSqlDatabase* dbParameters, QString *myError);

    bool loadDriessenParameters(soil::Crit3DSoilClass *soilTexture,
                                QSqlDatabase* dbParameters, QString *myError);

    QString getIdSoilString(QSqlDatabase* dbSoil, int idSoilNumber, QString *myError);


#endif // SOILDBTOOLS_H

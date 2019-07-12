#include "soilDbTools.h"
#include "commonConstants.h"
#include "utilities.h"

#include <math.h>

#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QUuid>
#include <QVariant>
#include <QDebug>


bool openDbSoil(QString dbName, QSqlDatabase* dbSoil, QString* error)
{
    *dbSoil = QSqlDatabase::addDatabase("QSQLITE", QUuid::createUuid().toString());
    dbSoil->setDatabaseName(dbName);

    if (!dbSoil->open())
    {
       *error = "Connection with database fail";
       return false;
    }

    return true;
}


bool loadVanGenuchtenParameters(QSqlDatabase* dbSoil, soil::Crit3DTextureClass* soilClassList, QString *error)
{
    QString queryString = "SELECT id_texture, alpha, n, he, theta_r, theta_s, k_sat, l ";
    queryString        += "FROM van_genuchten ORDER BY id_texture";

    QSqlQuery query = dbSoil->exec(queryString);
    query.last();
    int tableSize = query.at() + 1;     //SQLITE doesn't support SIZE

    if (tableSize == 0)
    {
        *error = "Table van_genuchten\n" + query.lastError().text();
        return false;
    }
    else if (tableSize != 12)
    {
        *error = "Table van_genuchten: wrong number of soil textures (must be 12)";
        return false;
    }

    //read values
    int id, j;
    float myValue;
    double m;
    query.first();
    do
    {
        id = query.value(0).toInt();

        //check data
        for (j = 0; j <= 7; j++)
            if (! getValue(query.value(j), &myValue))
            {
                *error = "Table van_genuchten: missing data in soil texture:" + QString::number(id);
                return false;
            }

        soilClassList[id].vanGenuchten.alpha = query.value(1).toDouble();    //[kPa^-1]
        soilClassList[id].vanGenuchten.n = query.value(2).toDouble();
        soilClassList[id].vanGenuchten.he = query.value(3).toDouble();       //[kPa]

        m = 1.0 - 1.0 / soilClassList[id].vanGenuchten.n;
        soilClassList[id].vanGenuchten.m = m;
        soilClassList[id].vanGenuchten.sc = pow(1.0 + pow(soilClassList[id].vanGenuchten.alpha
                                        * soilClassList[id].vanGenuchten.he, soilClassList[id].vanGenuchten.n), -m);

        soilClassList[id].vanGenuchten.thetaR = query.value(4).toDouble();

        //reference theta at saturation
        soilClassList[id].vanGenuchten.refThetaS = query.value(5).toDouble();
        soilClassList[id].vanGenuchten.thetaS = soilClassList[id].vanGenuchten.refThetaS;

        soilClassList[id].waterConductivity.kSat = query.value(6).toDouble();
        soilClassList[id].waterConductivity.l = query.value(7).toDouble();

    } while(query.next());

    return true;
}


bool loadDriessenParameters(QSqlDatabase* dbSoil, soil::Crit3DTextureClass* soilClassList, QString *error)
{
    QString queryString = "SELECT id_texture, k_sat, grav_conductivity, max_sorptivity";
    queryString += " FROM driessen ORDER BY id_texture";

    QSqlQuery query = dbSoil->exec(queryString);
    query.last();
    int tableSize = query.at() + 1;     //SQLITE doesn't support SIZE

    if (tableSize == 0)
    {
        *error = "Table soil_driessen\n" + query.lastError().text();
        return false;
    }
    else if (tableSize != 12)
    {
        *error = "Table soil_driessen: wrong number of soil textures (must be 12)";
        return false;
    }

    //read values
    int id, j;
    float myValue;
    query.first();
    do
    {
        id = query.value(0).toInt();
        //check data
        for (j = 0; j <= 3; j++)
            if (! getValue(query.value(j), &myValue))
            {
                *error = "Table soil_driessen: missing data in soil texture:" + QString::number(id);
                return false;
            }

        soilClassList[id].Driessen.k0 = query.value("k_sat").toDouble();
        soilClassList[id].Driessen.gravConductivity = query.value("grav_conductivity").toDouble();
        soilClassList[id].Driessen.maxSorptivity = query.value("max_sorptivity").toDouble();

    } while(query.next());

    return true;
}


bool loadSoilData(QSqlDatabase* dbSoil, QString soilCode, soil::Crit3DSoil* mySoil, QString *error)
{
    QString queryString = "SELECT * FROM horizons ";
    queryString += "WHERE soil_code='" + soilCode + "' ORDER BY horizon_nr";

    QSqlQuery query = dbSoil->exec(queryString);
    query.last();

    if (! query.isValid())
    {
        if (query.lastError().number() > 0)
            *error = "dbSoil error: " + query.lastError().text();
        else
            *error = "Missing soil:" + soilCode;
        return false;
    }

    int nrHorizons = query.at() + 1;     //SQLITE doesn't support SIZE
    mySoil->initialize(1, nrHorizons);

    int i = 0;
    float sand, silt, clay;
    double organicMatter, coarseFragments, lowerDepth, upperDepth, bulkDensity, theta_sat, ksat;

    query.first();
    do
    {
        // horizon number
        mySoil->horizon[i].dbData.horizonNr = query.value("horizon_nr").toInt();

        // upper and lower depth [cm]
        getValue(query.value("upper_depth"), &upperDepth);
        getValue(query.value("lower_depth"), &lowerDepth);
        mySoil->horizon[i].dbData.upperDepth = upperDepth;
        mySoil->horizon[i].dbData.lowerDepth = lowerDepth;

        // sand silt clay [%]
        getValue(query.value("sand"), &sand);
        getValue(query.value("silt"), &silt);
        getValue(query.value("clay"), &clay);
        mySoil->horizon[i].dbData.sand = sand;
        mySoil->horizon[i].dbData.silt = silt;
        mySoil->horizon[i].dbData.clay = clay;

        // coarse fragments and organic matter [%]
        getValue(query.value("coarse_fragment"), &coarseFragments);
        getValue(query.value("organic_matter"), &organicMatter);
        mySoil->horizon[i].dbData.coarseFragments = coarseFragments;
        mySoil->horizon[i].dbData.organicMatter = organicMatter;

        // bulk density [g/cm3]
        getValue(query.value("bulk_density"), &bulkDensity);
        mySoil->horizon[i].dbData.bulkDensity = bulkDensity;

        // theta sat [m3/m3]
        getValue(query.value("theta_sat"), &theta_sat);
        mySoil->horizon[i].dbData.thetaSat = theta_sat;

        // saturated conductivity [cm day-1]
        getValue(query.value("k_sat"), &ksat);
        mySoil->horizon[i].dbData.kSat = ksat;

        i++;

    } while(query.next());

    // read water retention data
    /*
    queryString = "SELECT * FROM water_retention ";
    queryString += "WHERE soil_code='" + soilCode + "' ORDER BY horizon_nr";
    query = dbSoil.exec(queryString);
    query.last();
    */

    return true;
}


bool loadSoil(QSqlDatabase* dbSoil, QString soilCode, soil::Crit3DSoil* mySoil,
              soil::Crit3DTextureClass* soilClassList, QString* error)
{
    if (!loadSoilData(dbSoil, soilCode, mySoil, error))
    {
        return false;
    }

    // TODO check nr horizons and depth

    std::string errorString;
    *error = "";
    for (int i = 0; i < mySoil->nrHorizons; i++)
    {
        if (! soil::setHorizon(&(mySoil->horizon[i]), soilClassList, &errorString))
        {
            *error += "horizon nr." + QString::number(mySoil->horizon[i].dbData.horizonNr) + ": "
                    + QString::fromStdString(errorString) + "\n";
        }
    }

    int lastHorizon = mySoil->nrHorizons -1;
    mySoil->totalDepth = mySoil->horizon[lastHorizon].lowerDepth;

    return true;
}



/*
 * if ((mySoil->horizon[i].upperDepth == NODATA) || (mySoil->horizon[i].lowerDepth == NODATA)
            || (mySoil->horizon[i].lowerDepth < mySoil->horizon[i].upperDepth)
            || ((idHorizon == 1) && (mySoil->horizon[i].upperDepth > 0))
            || ((idHorizon > 1) && (fabs(mySoil->horizon[i].upperDepth - mySoil->horizon[i-1].lowerDepth) > EPSILON)))
        {
            *myError = "Wrong soil: " + idSoilStr + " - wrong depth horizon: " + QString::number(idHorizon);
            return false;
        }
 */



QString getIdSoilString(QSqlDatabase* dbSoil, int idSoilNumber, QString *myError)
{
    *myError = "";
    QString queryString = "SELECT * FROM soils WHERE id_soil='" + QString::number(idSoilNumber) +"'";

    QSqlQuery query = dbSoil->exec(queryString);
    query.last();

    if (! query.isValid())
    {
        if (query.lastError().number() > 0)
            *myError = query.lastError().text();
        return "";
    }

    QString idSoilStr;
    getValue(query.value("soil_code"), &idSoilStr);

    return idSoilStr;
}


bool getSoilList(QSqlDatabase* dbSoil, QStringList* soilList, QString* error)
{
    // query soil list
    QString queryString = "SELECT soil_code FROM soils";
    QSqlQuery query = dbSoil->exec(queryString);

    query.first();
    if (! query.isValid())
    {
        if (query.lastError().number() > 0)
        {
            *error = query.lastError().text();
        }
        else
        {
            *error = "Error in reading table soils";
        }
        return false;
    }

    QString soilCode;
    do
    {
        getValue(query.value("soil_code"), &soilCode);
        if (soilCode != "")
        {
            soilList->append(soilCode);
        }
    }
    while(query.next());

    return true;
}


bool loadAllSoils(QString dbSoilName, std::vector <soil::Crit3DSoil> *soilList, soil::Crit3DTextureClass *soilClassList, QString* error)
{
    soilList->clear();

    QSqlDatabase* dbSoil = new QSqlDatabase();
    if (! openDbSoil(dbSoilName, dbSoil, error))
        return false;

    if (! loadVanGenuchtenParameters(dbSoil, soilClassList, error))
    {
        return false;
    }

    // query soil list
    QString queryString = "SELECT id_soil, soil_code FROM soils";
    QSqlQuery query = dbSoil->exec(queryString);

    query.first();
    if (! query.isValid())
    {
        if (query.lastError().number() > 0)
        {
            *error = query.lastError().text();
        }
        else
        {
            *error = "Error in reading table soils";
        }
        return false;
    }

    // load soil properties
    QString soilCode;
    int idSoil;
    QString wrongSoils = "";
    do
    {
        getValue(query.value("id_soil"), &idSoil);
        getValue(query.value("soil_code"), &soilCode);
        if (idSoil != NODATA && soilCode != "")
        {
            soil::Crit3DSoil *mySoil = new soil::Crit3DSoil;
            if (loadSoil(dbSoil, soilCode, mySoil, soilClassList, error))
            {
                mySoil->id = idSoil;
                mySoil->code = soilCode.toStdString();
                soilList->push_back(*mySoil);
            }
            else
            {
                wrongSoils += soilCode + ": " + *error + "\n";
            }
        }
    }
    while(query.next());

    dbSoil->close();

    if (soilList->size() == 0)
    {
       *error = "Missing soil properties";
       return false;
    }
    else if (wrongSoils != "")
    {
        *error = wrongSoils;
    }

    return true;
}

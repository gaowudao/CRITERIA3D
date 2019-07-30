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


bool loadVanGenuchtenParameters(QSqlDatabase* dbSoil, soil::Crit3DTextureClass* textureClassList, QString *error)
{
    QString queryString = "SELECT id_texture, texture, alpha, n, he, theta_r, theta_s, k_sat, l ";
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
        for (j = 0; j <= 8; j++)
            if (! getValue(query.value(j), &myValue))
            {
                *error = "Table van_genuchten: missing data in soil texture:" + QString::number(id);
                return false;
            }

        textureClassList[id].classNameUSDA = query.value(1).toString().toStdString();
        textureClassList[id].vanGenuchten.alpha = query.value(2).toDouble();    //[kPa^-1]
        textureClassList[id].vanGenuchten.n = query.value(3).toDouble();
        textureClassList[id].vanGenuchten.he = query.value(4).toDouble();       //[kPa]

        m = 1.0 - 1.0 / textureClassList[id].vanGenuchten.n;
        textureClassList[id].vanGenuchten.m = m;
        textureClassList[id].vanGenuchten.sc = pow(1.0 + pow(textureClassList[id].vanGenuchten.alpha
                                        * textureClassList[id].vanGenuchten.he, textureClassList[id].vanGenuchten.n), -m);

        textureClassList[id].vanGenuchten.thetaR = query.value(5).toDouble();

        //reference theta at saturation
        textureClassList[id].vanGenuchten.refThetaS = query.value(6).toDouble();
        textureClassList[id].vanGenuchten.thetaS = textureClassList[id].vanGenuchten.refThetaS;

        textureClassList[id].waterConductivity.kSat = query.value(7).toDouble();
        textureClassList[id].waterConductivity.l = query.value(8).toDouble();

    } while(query.next());

    return true;
}


bool loadDriessenParameters(QSqlDatabase* dbSoil, soil::Crit3DTextureClass* textureClassList, QString *error)
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

        textureClassList[id].Driessen.k0 = query.value("k_sat").toDouble();
        textureClassList[id].Driessen.gravConductivity = query.value("grav_conductivity").toDouble();
        textureClassList[id].Driessen.maxSorptivity = query.value("max_sorptivity").toDouble();

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
              soil::Crit3DTextureClass* textureClassList, QString* error)
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
        if (! soil::setHorizon(&(mySoil->horizon[i]), textureClassList, &errorString))
        {
            *error += "horizon nr." + QString::number(mySoil->horizon[i].dbData.horizonNr) + ": "
                    + QString::fromStdString(errorString) + "\n";
        }
    }

    int lastHorizon = mySoil->nrHorizons -1;
    mySoil->totalDepth = mySoil->horizon[lastHorizon].lowerDepth;

    return true;
}

/* check depth
 * if ((mySoil->horizon[i].upperDepth == NODATA) || (mySoil->horizon[i].lowerDepth == NODATA)
            || (mySoil->horizon[i].lowerDepth < mySoil->horizon[i].upperDepth)
            || ((idHorizon == 1) && (mySoil->horizon[i].upperDepth > 0))
            || ((idHorizon > 1) && (fabs(mySoil->horizon[i].upperDepth - mySoil->horizon[i-1].lowerDepth) > EPSILON)))
        {
            *myError = "Wrong soil: " + idSoilStr + " - wrong depth horizon: " + QString::number(idHorizon);
            return false;
        }
 */

bool updateSoilData(QSqlDatabase* dbSoil, QString soilCode, soil::Crit3DSoil* mySoil, QString *error)
{

    QSqlQuery qry(*dbSoil);
    if (soilCode.isEmpty())
    {
        *error = "soilCode missing";
        return false;
    }

    // delete all row from table horizons of soil:soilCode
    qry.prepare( "DELETE FROM `horizons` WHERE soil_code = :soil_code");
    qry.bindValue(":soil_code", soilCode);

    if( !qry.exec() )
    {
        *error = qry.lastError().text();
        return false;
    }

    // insert new rows
    qry.prepare( "INSERT INTO `horizons` (soil_code, horizon_nr, upper_depth, lower_depth, coarse_fragment, organic_matter, sand, silt, clay, bulk_density, theta_sat, k_sat)"
                                              " VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)" );

    QVariantList soil_code;
    QVariantList horizon_nr;
    QVariantList upper_depth;
    QVariantList lower_depth;
    QVariantList coarse_fragment;
    QVariantList organic_matter;
    QVariantList sand;
    QVariantList silt;
    QVariantList clay;
    QVariantList bulk_density;
    QVariantList theta_sat;
    QVariantList k_sat;

    for (int i=0; i<mySoil->nrHorizons; i++)
    {
        soil_code << soilCode;
        horizon_nr << i+1;
        upper_depth << mySoil->horizon[i].dbData.upperDepth;
        lower_depth << mySoil->horizon[i].dbData.lowerDepth;
        coarse_fragment << mySoil->horizon[i].dbData.coarseFragments;
        organic_matter << mySoil->horizon[i].dbData.organicMatter;
        sand << mySoil->horizon[i].dbData.sand;
        silt << mySoil->horizon[i].dbData.silt;
        clay << mySoil->horizon[i].dbData.clay;
        bulk_density << mySoil->horizon[i].dbData.bulkDensity;
        theta_sat << mySoil->horizon[i].dbData.thetaSat;
        k_sat << mySoil->horizon[i].dbData.kSat;
    }

    qry.addBindValue(soil_code);
    qry.addBindValue(horizon_nr);
    qry.addBindValue(upper_depth);
    qry.addBindValue(lower_depth);
    qry.addBindValue(coarse_fragment);
    qry.addBindValue(organic_matter);
    qry.addBindValue(sand);
    qry.addBindValue(silt);
    qry.addBindValue(clay);
    qry.addBindValue(bulk_density);
    qry.addBindValue(theta_sat);
    qry.addBindValue(k_sat);

    if( !qry.execBatch() )
    {
        *error = qry.lastError().text();
        return false;
    }
    else
        return true;
}

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


bool loadAllSoils(QString dbSoilName, std::vector <soil::Crit3DSoil> *soilList, soil::Crit3DTextureClass *textureClassList, QString* error)
{
    QSqlDatabase* dbSoil = new QSqlDatabase();
    if (! openDbSoil(dbSoilName, dbSoil, error)) return false;

    bool result = loadAllSoils(dbSoil, soilList, textureClassList, error);
    dbSoil->close();

    return result;
}


bool loadAllSoils(QSqlDatabase* dbSoil, std::vector <soil::Crit3DSoil> *soilList, soil::Crit3DTextureClass *textureClassList, QString* error)
{
    soilList->clear();

    if (! loadVanGenuchtenParameters(dbSoil, textureClassList, error))
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
            if (loadSoil(dbSoil, soilCode, mySoil, textureClassList, error))
            {
                mySoil->id = idSoil;
                mySoil->code = soilCode.toStdString();
                soilList->push_back(*mySoil);
            }
            if (*error != "")
            {
                wrongSoils += soilCode + ": " + *error + "\n";
            }
        }
    }
    while(query.next());

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

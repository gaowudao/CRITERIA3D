#include <QString>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QtMath>

#include "commonConstants.h"
#include "dbTools.h"
#include "crop.h"
#include "utilities.h"



bool loadCropParameters(QString idCrop, Crit3DCrop* myCrop, QSqlDatabase* dbCrop, QString *myError)
{
    QString idCropString = idCrop;

    QString queryString = "SELECT * FROM crop WHERE id_crop = '" + idCrop + "'";

    QSqlQuery query = dbCrop->exec(queryString);
    query.last();

    if (! query.isValid())
    {
        if (query.lastError().number() > 0)
            *myError = "Error in reading crop parameters of " + idCropString + "\n" + query.lastError().text();
        else
            *myError = "Missing crop: " + idCropString;
        return(false);
    }

    myCrop->idCrop = idCropString.toStdString();
    myCrop->type = getCropType(query.value("type").toString().toStdString());
    myCrop->plantCycle = query.value("plant_cycle_max_duration").toInt();
    getValue(query.value("sowing_doy"), &(myCrop->sowingDoy));

    // LAI
    myCrop->LAImax = query.value("lai_max").toDouble();
    myCrop->LAImin = query.value("lai_min").toDouble();
    myCrop->LAIcurve_a = query.value("lai_curve_factor_a").toDouble();
    myCrop->LAIcurve_b = query.value("lai_curve_factor_b").toDouble();

    // THERMAL THRESHOLDS
    myCrop->thermalThreshold = query.value("thermal_threshold").toDouble();
    myCrop->upperThermalThreshold = query.value("upper_thermal_threshold").toDouble();
    myCrop->degreeDaysIncrease = query.value("degree_days_lai_increase").toInt();
    myCrop->degreeDaysDecrease = query.value("degree_days_lai_decrease").toInt();
    myCrop->degreeDaysEmergence = query.value("degree_days_emergence").toInt();

    // ROOT
    myCrop->roots.rootShape = root::getRootDistributionType(query.value("root_shape").toInt());
    myCrop->roots.shapeDeformation = query.value("root_shape_deformation").toDouble();
    myCrop->roots.rootDepthMin = query.value("root_depth_zero").toDouble();
    myCrop->roots.rootDepthMax = query.value("root_depth_max").toDouble();
    getValue(query.value("degree_days_root_increase"), &(myCrop->roots.degreeDaysRootGrowth));

    // WATER NEEDS
    myCrop->kcMax = query.value("kc_max").toDouble();
    // [cm]
    if (! getValue(query.value("psi_leaf"), &(myCrop->psiLeaf)))
        myCrop->psiLeaf = 16000;

    myCrop->stressTolerance = query.value("stress_tolerance").toDouble();

    // fraction of Readily Available Water
    if (! getValue(query.value("raw_fraction"), &(myCrop->fRAW)))
    {
        // old version
        if (! getValue(query.value("frac_read_avail_water_max"), &(myCrop->fRAW)))
        {
            *myError = "Missing RAW_fraction for crop: " + idCropString;
            return(false);
        }
    }

    // IRRIGATION
    getValue(query.value("irrigation_shift"), &(myCrop->irrigationShift));
    getValue(query.value("degree_days_start_irrigation"), &(myCrop->degreeDaysStartIrrigation));
    getValue(query.value("degree_days_end_irrigation"), &(myCrop->degreeDaysEndIrrigation));
    getValue(query.value("doy_start_irrigation"), &(myCrop->doyStartIrrigation));
    getValue(query.value("doy_end_irrigation"), &(myCrop->doyEndIrrigation));

    // key value for irrigation
    if (! getValue(query.value("irrigation_volume"), &(myCrop->irrigationVolume)))
        myCrop->irrigationVolume = 0;

    // LAI grass
    if (! getValue(query.value("lai_grass"), &(myCrop->LAIgrass)))
        myCrop->LAIgrass = 0;

    // max surface puddle
    if (! getValue(query.value("max_height_surface_puddle"), &(myCrop->maxSurfacePuddle)))
        myCrop->maxSurfacePuddle = 0;

    return true;
}


bool loadVanGenuchtenParameters(soil::Crit3DSoilClass *soilTexture, QSqlDatabase* dbParameters, QString *myError)
{
    QString queryString = "SELECT id_texture, alpha, n, he, theta_r, theta_s, k_sat, l ";
    queryString        += "FROM soil_vangenuchten ORDER BY id_texture";

    QSqlQuery query = dbParameters->exec(queryString);
    query.last();
    int tableSize = query.at() + 1;     //SQLITE doesn't support SIZE

    if (tableSize == 0)
    {
        *myError = "Table soil_vangenuchten\n" + query.lastError().text();
        return(false);
    }
    else if (tableSize != 12)
    {
        *myError = "Table soil_vangenuchten: wrong number of soil textures (must be 12)";
        return(false);
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
                *myError = "Table soil_van_genuchten: missing data in soil texture:" + QString::number(id);
                return(false);
            }

        soilTexture[id].vanGenuchten.alpha = query.value(1).toDouble();    //[kPa^-1]
        soilTexture[id].vanGenuchten.n = query.value(2).toDouble();
        soilTexture[id].vanGenuchten.he = query.value(3).toDouble();       //[kPa]

        m = 1.0 - 1.0 / soilTexture[id].vanGenuchten.n;
        soilTexture[id].vanGenuchten.m = m;
        soilTexture[id].vanGenuchten.sc = pow(1.0 + pow(soilTexture[id].vanGenuchten.alpha
                                        * soilTexture[id].vanGenuchten.he, soilTexture[id].vanGenuchten.n), -m);

        soilTexture[id].vanGenuchten.thetaR = query.value(4).toDouble();

        //reference theta at saturation
        soilTexture[id].vanGenuchten.refThetaS = query.value(5).toDouble();
        soilTexture[id].vanGenuchten.thetaS = soilTexture[id].vanGenuchten.refThetaS;

        soilTexture[id].waterConductivity.kSat = query.value(6).toDouble();
        soilTexture[id].waterConductivity.l = query.value(7).toDouble();


    } while(query.next());

    return(true);
}


bool loadDriessenParameters(soil::Crit3DSoilClass *soilTexture, QSqlDatabase* dbParameters, QString *myError)
{
    QString queryString = "SELECT id_texture, k_sat, grav_conductivity, max_sorptivity";
    queryString += " FROM soil_driessen ORDER BY id_texture";

    QSqlQuery query = dbParameters->exec(queryString);
    query.last();
    int tableSize = query.at() + 1;     //SQLITE doesn't support SIZE

    if (tableSize == 0)
    {
        *myError = "Table soil_driessen\n" + query.lastError().text();
        return(false);
    }
    else if (tableSize != 12)
    {
        *myError = "Table soil_driessen: wrong number of soil textures (must be 12)";
        return(false);
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
                *myError = "Table soil_driessen: missing data in soil texture:" + QString::number(id);
                return(false);
            }

        soilTexture[id].Driessen.k0 = query.value("k_sat").toDouble();
        soilTexture[id].Driessen.gravConductivity = query.value("grav_conductivity").toDouble();
        soilTexture[id].Driessen.maxSorptivity = query.value("max_sorptivity").toDouble();

    } while(query.next());

    return(true);
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


bool loadSoil(QSqlDatabase* dbSoil, QString soilCode, soil::Crit3DSoil *mySoil, soil::Crit3DSoilClass *soilTexture, QString *myError)
{
    QString idSoilStr = soilCode;

    QString queryString = "SELECT * FROM horizons ";
    queryString += "WHERE soil_code='" + soilCode + "' ORDER BY horizon_nr";

    QSqlQuery query = dbSoil->exec(queryString);
    query.last();

    if (! query.isValid())
    {
        if (query.lastError().number() > 0)
            *myError = "dbSoil error: " + query.lastError().text();
        else
            *myError = "Missing soil:" + idSoilStr;
        return false;
    }

    int nrHorizons = query.at() + 1;     //SQLITE doesn't support SIZE
    mySoil->initialize(1, nrHorizons);

    int idTextureUSDA, idTextureNL, idHorizon;
    float sand, silt, clay;
    double organicMatter, coarseFragments, lowerDepth, upperDepth, bulkDensity, theta_sat, ksat;
    int i = 0;

    query.first();
    idHorizon = query.value("horizon_nr").toInt();
    if (idHorizon != 1)
    {
        *myError = "Wrong soil: " + idSoilStr + " - horizons must start from 1";
        return false;
    }
    do
    {
        // horizon depth
        idHorizon = query.value("horizon_nr").toInt();
        getValue(query.value("upper_depth"), &upperDepth);
        getValue(query.value("lower_depth"), &lowerDepth);

        // [cm]->[m]
        mySoil->horizon[i].upperDepth = upperDepth / 100.;
        mySoil->horizon[i].lowerDepth = lowerDepth / 100.;

        if ((mySoil->horizon[i].upperDepth == NODATA) || (mySoil->horizon[i].lowerDepth == NODATA)
            || (mySoil->horizon[i].lowerDepth < mySoil->horizon[i].upperDepth)
            || ((idHorizon == 1) && (mySoil->horizon[i].upperDepth > 0))
            || ((idHorizon > 1) && (fabs(mySoil->horizon[i].upperDepth - mySoil->horizon[i-1].lowerDepth) > EPSILON)))
        {
            *myError = "Wrong soil: " + idSoilStr + " - wrong depth horizon: " + QString::number(idHorizon);
            return false;
        }

        // coarse fragments %
        getValue(query.value("coarse_fragment"), &coarseFragments);
        if (int(coarseFragments) == int(NODATA))
            mySoil->horizon[i].coarseFragments = 0.0;    //default = no coarse fragment
        else
            //[0,1]
            mySoil->horizon[i].coarseFragments = coarseFragments / 100.0;

        // sand silt clay [-]
        getValue(query.value("sand"), &sand);
        if (sand < 1.f) sand *= 100.f;
        getValue(query.value("silt"), &silt);
        if (silt < 1.f) silt *= 100.f;
        getValue(query.value("clay"), &clay);
        if (clay < 1.f) clay *= 100.f;

        // texture
        idTextureUSDA = soil::getUSDATextureClass(sand, silt, clay);
        if (idTextureUSDA == NODATA)
        {
                *myError = "Wrong soil: " + idSoilStr
                    + " - sand+silt+clay <> 1 - horizon nr: "
                    + QString::number(idHorizon);
                return false;
        }
        idTextureNL =  soil::getNLTextureClass(sand, silt, clay);

        mySoil->horizon[i].texture.sand = sand;
        mySoil->horizon[i].texture.silt = silt;
        mySoil->horizon[i].texture.clay = clay;
        mySoil->horizon[i].texture.classUSDA = idTextureUSDA;
        mySoil->horizon[i].texture.classNL = idTextureNL;
        mySoil->horizon[i].vanGenuchten = soilTexture[idTextureUSDA].vanGenuchten;
        mySoil->horizon[i].waterConductivity = soilTexture[idTextureUSDA].waterConductivity;
        mySoil->horizon[i].Driessen = soilTexture[idTextureNL].Driessen;

        // organic matter (%)
        getValue(query.value("organic_matter"), &organicMatter);
        if ((int(organicMatter) == int(NODATA)) || (organicMatter == 0.0))
            organicMatter = 0.005;      // default: 0.5%
        else
            organicMatter /= 100.0;     // [-]
        mySoil->horizon[i].organicMatter = organicMatter;

        // bulk density and porosity
        getValue(query.value("bulk_density"), &bulkDensity);
        if ((int(bulkDensity) != int(NODATA)) && (bulkDensity <= 0 || bulkDensity >= 2.7))
        {
                *myError = "Wrong soil: " + idSoilStr
                    + " - wrong bulk density - horizon nr: "
                    + QString::number(idHorizon);
                return false;
        }

        getValue(query.value("theta_sat"), &theta_sat);
        if (theta_sat <= 0) theta_sat = NODATA;

        if ((int(theta_sat) == int(NODATA)) && (bulkDensity != int(NODATA)))
            theta_sat = soil::estimateTotalPorosity(&(mySoil->horizon[i]), bulkDensity);

        if (int(theta_sat) != int(NODATA))
            mySoil->horizon[i].vanGenuchten.thetaS = theta_sat;

        if (int(bulkDensity) == int(NODATA))
            bulkDensity = soil::estimateBulkDensity(&(mySoil->horizon[i]), theta_sat);

        mySoil->horizon[i].bulkDensity = bulkDensity;

        // SATURATED CONDUCTIVITY (cm/day)
        getValue(query.value("ksat"), &ksat);
        if ((int(ksat) == int(NODATA)) || (ksat <= 0))
            ksat = soil::estimateSaturatedConductivity(&(mySoil->horizon[i]), bulkDensity);

        mySoil->horizon[i].waterConductivity.kSat = ksat;

        mySoil->horizon[i].fieldCapacity = soil::getFieldCapacity(&(mySoil->horizon[i]), soil::KPA);
        mySoil->horizon[i].wiltingPoint = soil::getWiltingPoint(soil::KPA);
        mySoil->horizon[i].waterContentFC = soil::getThetaFC(&(mySoil->horizon[i]));
        mySoil->horizon[i].waterContentWP = soil::getThetaWP(&(mySoil->horizon[i]));

        i++;

    } while(query.next());

    mySoil->totalDepth = mySoil->horizon[nrHorizons-1].lowerDepth;

    if (mySoil->totalDepth < 0.3)
    {
        *myError = "Wrong soil: " + idSoilStr + " - soil depth < 30cm";
        return false;
    }

    return true;
}


QString getCropFromClass(QSqlDatabase* dbCrop, QString cropClassTable, QString cropClassField, QString idCropClass, QString *myError)
{
    *myError = "";
    QString queryString = "SELECT * FROM " + cropClassTable + " WHERE " + cropClassField + " = '" + idCropClass + "'";

    QSqlQuery query = dbCrop->exec(queryString);
    query.last();

    if (! query.isValid())
    {
        if (query.lastError().number() > 0)
            *myError = query.lastError().text();
        return "";
    }

    QString myCrop;
    getValue(query.value("id_crop"), &myCrop);

    return myCrop;
}


QString getCropFromId(QSqlDatabase* dbCrop, QString cropClassTable, QString cropIdField, int cropId, QString *myError)
{
    *myError = "";
    QString queryString = "SELECT * FROM " + cropClassTable + " WHERE " + cropIdField + " = " + QString::number(cropId);

    QSqlQuery query = dbCrop->exec(queryString);
    query.last();

    if (! query.isValid())
    {
        if (query.lastError().number() > 0)
            *myError = query.lastError().text();
        return "";
    }

    QString myCrop;
    getValue(query.value("id_crop"), &myCrop);

    return myCrop;
}


float getIrriRatioFromClass(QSqlDatabase* dbCrop, QString cropClassTable, QString cropClassField, QString idCrop, QString *myError)
{
    *myError = "";

    QString queryString = "SELECT irri_ratio FROM " + cropClassTable + " WHERE " + cropClassField + " = '" + idCrop + "'";

    QSqlQuery query = dbCrop->exec(queryString);
    query.last();

    if (! query.isValid())
    {
        if (query.lastError().number() > 0)
            *myError = query.lastError().text();
        return(NODATA);
    }

    float myRatio = 0;

    if (getValue(query.value("irri_ratio"), &(myRatio)))
        return myRatio;
    else
        return NODATA;
}


float getIrriRatioFromId(QSqlDatabase* dbCrop, QString cropClassTable, QString cropIdField, int cropId, QString *myError)
{
    *myError = "";

    QString queryString = "SELECT irri_ratio FROM " + cropClassTable + " WHERE " + cropIdField + " = " + QString::number(cropId);

    QSqlQuery query = dbCrop->exec(queryString);
    query.last();

    if (! query.isValid())
    {
        if (query.lastError().number() > 0)
            *myError = query.lastError().text();
        return(NODATA);
    }

    float myRatio = 0;

    if (getValue(query.value("irri_ratio"), &(myRatio)))
        return myRatio;
    else
        return NODATA;
}



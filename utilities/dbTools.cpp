#include <QString>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

#include "dbtools.h"
#include "crop.h"
#include "utilities.h"


bool loadCropParameters(std::string idCropString, Crit3DCrop* myCrop, QSqlDatabase* dbCrop, std::string *myError)
{
    QString idCrop = QString::fromStdString(idCropString);

    QString queryString = "SELECT * FROM crop WHERE id_crop = '" + idCrop + "'";

    QSqlQuery query = dbCrop->exec(queryString);
    query.last();

    if (! query.isValid())
    {
        if (query.lastError().number() > 0)
            *myError = "Error in reading crop parameters of " + idCropString + "\n" + query.lastError().text().toStdString();
        else
            *myError = "Missing crop: " + idCropString;
        return(false);
    }

    myCrop->idCrop = idCropString;
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
    myCrop->degreeDaysMaxSensibility = query.value("degree_days_max_sensibility").toInt();
    myCrop->psiLeaf = query.value("psi_leaf").toDouble();
    myCrop->stressTolerance = query.value("stress_tolerance").toDouble();
    myCrop->frac_read_avail_water_min = query.value("frac_read_avail_water_min").toDouble();
    myCrop->frac_read_avail_water_max = query.value("frac_read_avail_water_max").toDouble();

    // IRRIGATION
    getValue(query.value("irrigation_shift"), &(myCrop->irrigationShift));
    getValue(query.value("degree_days_start_irrigation"), &(myCrop->degreeDaysStartIrrigation));
    getValue(query.value("degree_days_end_irrigation"), &(myCrop->degreeDaysEndIrrigation));

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



bool loadVanGenuchtenParameters(soil::Crit3DSoilClass *soilTexture, QSqlDatabase* dbParameters, std::string *myError)
{
    QString queryString = "SELECT id_texture, alpha, n, he, theta_r, theta_s, k_sat, l ";
    queryString        += "FROM soil_vangenuchten ORDER BY id_texture";

    QSqlQuery query = dbParameters->exec(queryString);
    query.last();
    int tableSize = query.at() + 1;     //SQLITE doesn't support SIZE

    if (tableSize == 0)
    {
        *myError = "Table soil_vangenuchten\n" + query.lastError().text().toStdString();
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
                *myError = "Table soil_van_genuchten: missing data in soil texture:" + QString::number(id).toStdString();
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


bool loadDriessenParameters(soil::Crit3DSoilClass *soilTexture, QSqlDatabase* dbParameters, std::string *myError)
{
    QString queryString = "SELECT id_texture, k_sat, grav_conductivity, max_sorptivity";
    queryString += " FROM soil_driessen ORDER BY id_texture";

    QSqlQuery query = dbParameters->exec(queryString);
    query.last();
    int tableSize = query.at() + 1;     //SQLITE doesn't support SIZE

    if (tableSize == 0)
    {
        *myError = "Table soil_driessen\n" + query.lastError().text().toStdString();
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
                *myError = "Table soil_driessen: missing data in soil texture:" + QString::number(id).toStdString();
                return(false);
            }

        soilTexture[id].Driessen.k0 = query.value(1).toFloat();
        soilTexture[id].Driessen.gravConductivity = query.value(2).toFloat();
        soilTexture[id].Driessen.maxSorptivity = query.value(3).toFloat();

    } while(query.next());

    return(true);
}

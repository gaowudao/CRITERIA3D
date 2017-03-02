#include <QSqlQuery>
#include <QSqlError>
#include <QDate>
#include <QVariant>
#include <QDebug>
#include <iostream>

#include "commonConstants.h"
#include "crit3dDate.h"
#include "dataHandler.h"
#include "Criteria1D.h"
#include "meteo.h"
#include "root.h"


Criteria1DUnit::Criteria1DUnit()
{
    this->idCase = "";
    this->idCrop = "";
    this->idSoil = "";
    this->idMeteo = "";

    this->idForecast = "";
    this->idSoilNumber = NODATA;
    this->idCropMoses = "";
}


Criteria1D::Criteria1D()
{
    this->idCase = "";
    this->outputString = "";

    this->layer = NULL;
    this->nrLayers = 0;
    this->layerThickness = 0.02;          // [m] thickness = 2cm

    this->maxSimulationDepth = 2.0;       // [m]

    this->isGeometricLayer = false;

    this->depthPloughedSoil = 0.5;        // [m]
    this->initialAW[0] = 0.85;            // [-] of available Water (ploughed soil)
    this->initialAW[1] = 0.8;             // [-] of available Water (deep soil)

    this->optimizeIrrigation = false;

    this->isSeasonalForecast = false;
    this->firstSeasonMonth = NODATA;
    this->nrSeasonalForecasts = 0;
    this->seasonalForecasts = NULL;

    this->isShortTermForecast = false;
    this->daysOfForecast = NODATA;
}


void Criteria1DOutput::initializeDaily()
{
    this->dailyPrec = 0.0;
    this->dailyDrainage = 0.0;
    this->dailySurfaceRunoff = 0.0;
    this->dailySubsurfaceRunoff = 0.0;
    this->dailyIrrigation = 0.0;
    this->dailyEt0 = 0.0;
    this->dailyEvaporation = 0.0;
    this->dailyMaxTranspiration = 0.0;
    this->dailyMaxEvaporation = 0.0;
    this->dailyTranspiration = 0.0;
    this->dailyCropAvailableWater = 0.0;
}

Criteria1DOutput::Criteria1DOutput()
{
    this->initializeDaily();
}


bool Criteria1D::loadVanGenuchtenParameters(QString *myError)
{
    QString queryString = "SELECT id_texture, alpha, n, he, theta_r, theta_s, k_sat, l";
    queryString += " FROM soil_vangenuchten";
    queryString += " ORDER BY id_texture";

    QSqlQuery query = this->dbParameters.exec(queryString);
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

        this->soilTexture[id].vanGenuchten.alpha = query.value(1).toDouble();    //[kPa^-1]
        this->soilTexture[id].vanGenuchten.n = query.value(2).toDouble();
        this->soilTexture[id].vanGenuchten.he = query.value(3).toDouble();       //[kPa]

        m = 1.0 - 1.0 / this->soilTexture[id].vanGenuchten.n;
        this->soilTexture[id].vanGenuchten.m = m;
        this->soilTexture[id].vanGenuchten.sc = pow(1.0 + pow(this->soilTexture[id].vanGenuchten.alpha
                                        * this->soilTexture[id].vanGenuchten.he, this->soilTexture[id].vanGenuchten.n), -m);

        this->soilTexture[id].vanGenuchten.thetaR = query.value(4).toDouble();

        //reference theta at saturation
        this->soilTexture[id].vanGenuchten.refThetaS = query.value(5).toDouble();
        this->soilTexture[id].vanGenuchten.thetaS = this->soilTexture[id].vanGenuchten.refThetaS;

        this->soilTexture[id].waterConductivity.kSat = query.value(6).toDouble();
        this->soilTexture[id].waterConductivity.l = query.value(7).toDouble();


    } while(query.next());

    return(true);

}


bool Criteria1D::loadDriessenParameters(QString *myError)
{
    QString queryString = "SELECT id_texture, k_sat, grav_conductivity, max_sorptivity";
    queryString += " FROM soil_driessen";
    queryString += " ORDER BY id_texture";

    QSqlQuery query = this->dbParameters.exec(queryString);
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
        this->soilTexture[id].Driessen.k0 = query.value(1).toFloat();
        this->soilTexture[id].Driessen.gravConductivity = query.value(2).toFloat();
        this->soilTexture[id].Driessen.maxSorptivity = query.value(3).toFloat();

    } while(query.next());

    return(true);
}


bool Criteria1D::loadSoil(QString soilCode, QString *myError)
{
    QString queryString = "SELECT * FROM horizons ";
    queryString += "WHERE soil_code='" + soilCode + "' ORDER BY horizon_nr";

    QSqlQuery query = this->dbSoil.exec(queryString);
    query.last();

    if (! query.isValid())
    {
        if (query.lastError().number() > 0)
            *myError = "dbSoil error: " + query.lastError().text();
        else
            *myError = "Missing soil:" + soilCode;
        return(false);
    }

    int nrHorizons = query.at() + 1;     //SQLITE doesn't support SIZE
    mySoil.cleanSoil();
    mySoil = soil::Crit3DSoil(1, nrHorizons);

    int idTextureUSDA, idTextureNL, idHorizon;
    double sand, silt, clay, organicMatter, coarseFragments;
    double lowerDepth, upperDepth, bulkDensity, theta_sat, ksat;
    int i = 0;
    query.first();
    do
    {
        // horizon depth
        idHorizon = query.value("horizon_nr").toInt();
        getValue(query.value("upper_depth"), &upperDepth);
        getValue(query.value("lower_depth"), &lowerDepth);

        // [cm]->[m]
        mySoil.horizon[i].upperDepth = upperDepth / 100.;
        mySoil.horizon[i].lowerDepth = lowerDepth / 100.;

        if ((mySoil.horizon[i].upperDepth == NODATA) || (mySoil.horizon[i].lowerDepth == NODATA)
            || (mySoil.horizon[i].lowerDepth < mySoil.horizon[i].upperDepth)
            || ((idHorizon == 1) && (mySoil.horizon[i].upperDepth > 0))
            || ((idHorizon > 1) && (mySoil.horizon[i].upperDepth > mySoil.horizon[i-1].lowerDepth)))
        {
            *myError = "Wrong depth in soil:" + soilCode + " horizon nr:" + QString::number(idHorizon);
            return(false);
        }

        // coarse fragments %
        getValue(query.value("coarse_fragment"), &coarseFragments);
        if (coarseFragments == NODATA)
            mySoil.horizon[i].coarseFragments = 0.;    //default = no coarse fragment
        else
            //[0,1]
            mySoil.horizon[i].coarseFragments = coarseFragments / 100.0;

        // sand silt clay [-]
        getValue(query.value("sand"), &sand);
        if (sand < 1) sand *= 100.0;
        getValue(query.value("silt"), &silt);
        if (silt < 1) silt *= 100.0;
        getValue(query.value("clay"), &clay);
        if (clay < 1) clay *= 100.0;

        // texture
        idTextureUSDA = soil::getUSDATextureClass(sand, silt, clay);
        if (idTextureUSDA == NODATA)
        {
                *myError = "Texture wrong! sand+silt+clay <> 1 in soil:"
                        + soilCode + " horizon nr:" + QString::number(idHorizon);
                return (false);
        }
        idTextureNL =  soil::getNLTextureClass(sand, silt, clay);

        mySoil.horizon[i].texture.sand = sand;
        mySoil.horizon[i].texture.silt = silt;
        mySoil.horizon[i].texture.clay = clay;
        mySoil.horizon[i].texture.classUSDA = idTextureUSDA;
        mySoil.horizon[i].texture.classNL = idTextureNL;
        mySoil.horizon[i].vanGenuchten = this->soilTexture[idTextureUSDA].vanGenuchten;
        mySoil.horizon[i].waterConductivity = this->soilTexture[idTextureUSDA].waterConductivity;
        mySoil.horizon[i].Driessen = this->soilTexture[idTextureNL].Driessen;

        // organic matter (%)
        getValue(query.value("organic_matter"), &organicMatter);
        if ((organicMatter == NODATA) || (organicMatter == 0.0))
            organicMatter = 0.005;       //default: 0.5%
        else
            organicMatter /= 100.0;     //[-]
        mySoil.horizon[i].organicMatter = organicMatter;

        // bulk density and porosity
        getValue(query.value("bulk_density"), &bulkDensity);
        if (bulkDensity <= 0) bulkDensity = NODATA;
        getValue(query.value("theta_sat"), &theta_sat);
        if (theta_sat <= 0) theta_sat = NODATA;

        if ((theta_sat == NODATA) && (bulkDensity != NODATA))
            theta_sat = soil::estimateTotalPorosity(&(mySoil.horizon[i]), bulkDensity);

        if (theta_sat != NODATA)
            mySoil.horizon[i].vanGenuchten.thetaS = theta_sat;

        if (bulkDensity == NODATA)
            bulkDensity = soil::estimateBulkDensity(&(mySoil.horizon[i]), theta_sat);

        mySoil.horizon[i].bulkDensity = bulkDensity;

        // SATURATED CONDUCTIVITY (cm/day)
        getValue(query.value("ksat"), &ksat);
        if ((ksat == NODATA) || (ksat <= 0))
            ksat = soil::estimateSaturatedConductivity(&(mySoil.horizon[i]), bulkDensity);
        /*else
        {
            if ((ksat < (mySoil.horizon[i].waterConductivity.kSat / 10.)) || (ksat > (mySoil.horizon[i].waterConductivity.kSat * 10.)))
                logInfo("WARNING: Ksat out of class limit, in soil " + idSoil + " horizon " + QString::number(i));
        }*/

        mySoil.horizon[i].waterConductivity.kSat = ksat;

        mySoil.horizon[i].fieldCapacity = soil::getFieldCapacity(&(mySoil.horizon[i]), soil::KPA);
        mySoil.horizon[i].wiltingPoint = soil::getWiltingPoint(soil::KPA);
        mySoil.horizon[i].waterContentFC = soil::getThetaFC(&(mySoil.horizon[i]));
        mySoil.horizon[i].waterContentWP = soil::getThetaWP(&(mySoil.horizon[i]));

        i++;
    } while(query.next());

    mySoil.totalDepth = mySoil.horizon[nrHorizons-1].lowerDepth;
    if (mySoil.totalDepth < 0.5)
    {
            *myError = "Texture wrong! soil depth < 50cm:" + soilCode;
            return (false);
    }

    // set layers
    this->nrLayers = ceil(mySoil.totalDepth / layerThickness) + 1;
    if (this->layer != NULL) free(this->layer);
    this->layer = (soil::Crit3DLayer *) calloc(nrLayers, sizeof(soil::Crit3DLayer));

    // initialize layers
    layer[0].depth = 0.0;
    layer[0].thickness = 0.0;
    double soilFraction, hygroscopicHumidity;
    int horizonIndex;
    double depth = this->layerThickness / 2.0;
    for (i = 1; i < this->nrLayers; i++)
    {
        horizonIndex = soil::getHorizonIndex(&(mySoil), depth);
        layer[i].horizon = &(mySoil.horizon[horizonIndex]);
        soilFraction = (1.0 - layer[i].horizon->coarseFragments);
        layer[i].soilFraction = soilFraction;       // [-]
        layer[i].depth = depth;                     // [m]
        layer[i].thickness = this->layerThickness;  // [m]

        //[mm]
        layer[i].SAT = mySoil.horizon[horizonIndex].vanGenuchten.thetaS * soilFraction * this->layerThickness * 1000.0;

        layer[i].FC = mySoil.horizon[horizonIndex].waterContentFC * soilFraction * this->layerThickness * 1000.0;
        layer[i].critical = layer[i].FC;

        layer[i].WP = mySoil.horizon[horizonIndex].waterContentWP * soilFraction * this->layerThickness * 1000.0;

        // hygroscopic humidity: -2000 kPa
        hygroscopicHumidity = soil::thetaFromSignPsi(-2000, &(mySoil.horizon[horizonIndex]));
        layer[i].HH = hygroscopicHumidity * soilFraction * this->layerThickness * 1000.0;

        depth += this->layerThickness;
    }

    return(true);
}


bool Criteria1D::loadMeteo(QString idMeteo, QString idForecast, QString *myError)
{
    QString queryString = "SELECT * FROM meteo_locations";
    queryString += " WHERE id_meteo='" + idMeteo + "'";
    QSqlQuery query = this->dbMeteo.exec(queryString);
    query.last();
    if (! query.isValid())
    {
        if (query.lastError().number() > 0)
            *myError = "dbMeteo error: " + query.lastError().text();
        else
            *myError = "Missing meteo location:" + idMeteo;
        return(false);
    }
    QString tableName = query.value("table_name").toString();

    double myLat, myLon;
    if (getValue(query.value(("latitude")), &myLat))
        this->meteoPoint.latitude = myLat;
    else
    {
        *myError = "Missing latitude in idMeteo: " + idMeteo;
        return false;
    }

    if (getValue(query.value(("longitude")), &myLon))
        this->meteoPoint.longitude = myLon;

    queryString = "SELECT * FROM " + tableName + " ORDER BY [date]";
    query = this->dbMeteo.exec(queryString);
    query.last();

    if (! query.isValid())
    {
        if (query.lastError().number() > 0)
            *myError = "dbMeteo error: " + query.lastError().text();
        else
            *myError = "Missing meteo table:" + tableName;
        return false;
    }

    query.first();
    QDate firstDate = query.value("date").toDate();
    query.last();
    QDate lastDate = query.value("date").toDate();

    int nrDays = firstDate.daysTo(lastDate) + 1;

    // Short term forecast
    if (this->isShortTermForecast)
        nrDays += this->daysOfForecast;

    this->meteoPoint.initializeObsDataD(nrDays, getCrit3DDate(firstDate));

    //std::cout << "Read weather data...";
    if (! readMeteoData(&query, myError)) return false;

    // SHORT TERM FORECAST
    if (this->isShortTermForecast)
    {
        QString queryString = "SELECT * FROM meteo_locations";
        queryString += " WHERE id_meteo='" + idForecast + "'";
        QSqlQuery query = this->dbForecast.exec(queryString);
        query.last();
        if (! query.isValid())
        {
            if (query.lastError().number() > 0)
                *myError = "dbForecast error: " + query.lastError().text();
            else
                *myError = "Missing forecast location:" + idForecast;
            return false;
        }
        QString tableName = query.value("table_name").toString();

        query.clear();
        queryString = "SELECT * FROM " + tableName + " ORDER BY [date]";
        query = this->dbForecast.exec(queryString);
        query.last();

        if (! query.isValid())
        {
            if (query.lastError().number() > 0)
                *myError = "dbForecast error: " + query.lastError().text();
            else
                *myError = "Missing forecast table:" + tableName;
            return false;
        }

        if (! readMeteoData(&query, myError)) return false;
        // TODO check on last date
        // creare un reduceNrObsDataDaysD in meteo.pro
    }

    return true;
}


bool Criteria1D::readMeteoData(QSqlQuery * query, QString *myError)
{
    float tmin, tmax, tmed, prec, et0;
    QDate myDate, previousDate;

    previousDate.setDate(1900,1,1);
    query->first();

    do
    {
        myDate = query->value("date").toDate();

        if (! myDate.isValid())
        {
            *myError = "Wrong date format: " + query->value("date").toString();
            return false;
        }

        if (myDate != previousDate)
        {
            // mandatory
            getValue(query->value("tmin"), &tmin);
            getValue(query->value("tmax"), &tmax);
            if ((tmin == NODATA) || (tmax == NODATA))
            {
                *myError = "Missing temperature: " + myDate.toString();
                return false;
            }
            getValue(query->value("prec"), &prec);
            if (prec == NODATA)
            {
                *myError = "Missing precipitation: " + myDate.toString();
                return false;
            }

            // facoltativi
            getValue(query->value("tavg"), &tmed);
            getValue(query->value("etp"), &et0);
            if (tmed == NODATA) tmed = (tmin + tmax) * 0.5;

            this->meteoPoint.setMeteoPointValueD(getCrit3DDate(myDate), airTemperatureMin, (float)tmin);
            this->meteoPoint.setMeteoPointValueD(getCrit3DDate(myDate), airTemperatureMax, (float)tmax);
            this->meteoPoint.setMeteoPointValueD(getCrit3DDate(myDate), airTemperatureMean, (float)tmed);
            this->meteoPoint.setMeteoPointValueD(getCrit3DDate(myDate), precipitation, (float)prec);
            this->meteoPoint.setMeteoPointValueD(getCrit3DDate(myDate), potentialEvapotranspiration, (float)et0);
        }

        previousDate = myDate;

    } while(query->next());

    return true;
}


bool Criteria1D::loadCropParameters(QString idCrop, QString *myError)
{
    QString queryString = "SELECT * FROM crop WHERE id_crop = '" + idCrop + "'";

    QSqlQuery query = this->dbParameters.exec(queryString);
    query.last();

    if (! query.isValid())
    {
        if (query.lastError().number() > 0)
            *myError = "dbParameters error: " + query.lastError().text();
        else
            *myError = "Missing crop: " + idCrop;
        return(false);
    }

    myCrop.idCrop = idCrop.toStdString();

    myCrop.type = getCropType(query.value("type").toString().toStdString());

    myCrop.plantCycle = query.value("plant_cycle_max_duration").toInt();
    getValue(query.value("sowing_doy"), &(myCrop.sowingDoy));

    myCrop.thermalThreshold = query.value("thermal_threshold").toDouble();
    myCrop.upperThermalThreshold = query.value("upper_thermal_threshold").toDouble();
    myCrop.LAImax = query.value("lai_max").toDouble();
    myCrop.LAImin = query.value("lai_min").toDouble();
    myCrop.LAIcurve_a = query.value("lai_curve_factor_a").toDouble();
    myCrop.LAIcurve_b = query.value("lai_curve_factor_b").toDouble();
    myCrop.degreeDaysIncrease = query.value("degree_days_lai_increase").toInt();
    myCrop.degreeDaysDecrease = query.value("degree_days_lai_decrease").toInt();
    myCrop.degreeDaysEmergence = query.value("degree_days_emergence").toInt();

    myCrop.roots.rootShape = root::getRootDistributionType(query.value("root_shape").toInt());
    myCrop.roots.shapeDeformation = query.value("root_shape_deformation").toDouble();
    myCrop.roots.rootDepthMin = query.value("root_depth_zero").toDouble();
    myCrop.roots.rootDepthMax = query.value("root_depth_max").toDouble();

    getValue(query.value("degree_days_root_increase"), &(myCrop.roots.degreeDaysRootGrowth));

    myCrop.kcMax = query.value("kc_max").toDouble();

    //water need
    myCrop.degreeDaysMaxSensibility = query.value("degree_days_max_sensibility").toInt();
    myCrop.psiLeaf = query.value("psi_leaf").toDouble();
    myCrop.stressTolerance = query.value("stress_tolerance").toDouble();
    myCrop.frac_read_avail_water_min = query.value("frac_read_avail_water_min").toDouble();
    myCrop.frac_read_avail_water_max = query.value("frac_read_avail_water_max").toDouble();

    getValue(query.value("irrigation_shift"), &(myCrop.irrigationShift));
    getValue(query.value("degree_days_start_irrigation"), &(myCrop.degreeDaysStartIrrigation));
    getValue(query.value("degree_days_end_irrigation"), &(myCrop.degreeDaysEndIrrigation));

    //key value for irrigation
    if (! getValue(query.value("irrigation_volume"), &(myCrop.irrigationVolume)))
        myCrop.irrigationVolume = 0;

    //LAI grass
    if (! getValue(query.value("lai_grass"), &(myCrop.LAIgrass)))
        myCrop.LAIgrass = 0;

    //max surface puddle
    if (! getValue(query.value("max_height_surface_puddle"), &(myCrop.maxSurfacePuddle)))
        myCrop.maxSurfacePuddle = 0;

    return true;
}


bool Criteria1D::createOutputTable(QString* myError)
{
    QString queryString = "DROP TABLE '" + this->idCase + "'";
    QSqlQuery myQuery = this->dbOutput.exec(queryString);

    if (myQuery.lastError().number() > 0)
    {
        *myError = "Error in dropping table: " + this->idCase + "\n" + myQuery.lastError().text();
    }

    queryString = "CREATE TABLE '" + this->idCase + "'"
            + " ( DATE TEXT,"
            + " PREC REAL, IRRIGATION REAL, RAW REAL, DRAINAGE REAL, RUNOFF REAL, ET0 REAL,"
            + " EVAP_MAX REAL, TRANSP_MAX, EVAP REAL, TRANSP REAL,"
            + " LAI REAL, ROOTDEPTH REAL )";
    myQuery = this->dbOutput.exec(queryString);

    if (myQuery.lastError().number() > 0)
    {
        *myError = "Error in creating table: " + this->idCase + "\n" + myQuery.lastError().text();
        return false;
    }

    return true;
}


void Criteria1D::prepareOutput(Crit3DDate myDate, bool isFirst)
{
    if (isFirst)
        this->outputString = "INSERT INTO '" + this->idCase + "'"
            + " (DATE, PREC, IRRIGATION, RAW, DRAINAGE, RUNOFF, ET0,"
            + " EVAP_MAX, TRANSP_MAX, EVAP, TRANSP, LAI, ROOTDEPTH) "
            + " VALUES ";
    else
        this->outputString += ",";

    this->outputString
            += "('" + QString::fromStdString(myDate.toStdString()) + "'"
            + "," + QString::number(this->output.dailyPrec)
            + "," + QString::number(this->output.dailyIrrigation)
            + "," + QString::number(this->output.dailyCropAvailableWater)
            + "," + QString::number(this->output.dailyDrainage)
            + "," + QString::number(this->output.dailySurfaceRunoff)
            + "," + QString::number(this->output.dailyEt0)
            + "," + QString::number(this->output.dailyMaxEvaporation)
            + "," + QString::number(this->output.dailyMaxTranspiration)
            + "," + QString::number(this->output.dailyEvaporation)
            + "," + QString::number(this->output.dailyTranspiration)
            + "," + QString::number(this->myCrop.LAI)
            + "," + QString::number(this->myCrop.roots.rootDepth)
            + ")";
}


bool Criteria1D::saveOutput(QString* myError)
{
    QSqlQuery myQuery = this->dbOutput.exec(this->outputString);

    if (myQuery.lastError().type() != QSqlError::NoError)
    {
        *myError = "Error in saving output:\n" + myQuery.lastError().text();
        return false;
    }

    return true;
}





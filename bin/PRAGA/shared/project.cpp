#include <QMessageBox>

#include "project.h"
#include "formInfo.h"
#include "commonConstants.h"
#include "spatialControl.h"
#include "radiationSettings.h"
#include "solarRadiation.h"
#include "interpolationCmd.h"
#include "interpolation.h"
#include "transmissivity.h"
#include "utilities.h"

#include <iostream> //debug
#include <QtSql>


Project::Project()
{
    inizializeDBConnection();
    modality = MODE_GUI;
    requestedExit = false;
    path = "";
    logFileName = "";
    errorString = "";
    meteoPoints = nullptr;
    nrMeteoPoints = 0;
    meteoSettings = new Crit3DMeteoSettings();
    quality = new Crit3DQuality();
    checkSpatialQuality = true;
    currentVariable = noMeteoVar;
    currentFrequency = noFrequency;
    currentDate.setDate(1800,1,1);
    previousDate = currentDate;
    currentHour = 12;
    meteoPointsColorScale = new Crit3DColorScale();
    meteoPointsDbHandler = nullptr;
    meteoGridDbHandler = nullptr;
    radiationMaps = nullptr;

}

void Project::inizializeDBConnection()
{
    dbProvider = "QSQLITE";
    dbHostname = "";
    dbName = "";
    dbPort = NODATA;
    dbUsername = "";
    dbPassword = "";
}

bool Project::openDBConnection()
{
    dbConnection.close();

    dbConnection = QSqlDatabase::addDatabase(dbProvider);
    dbConnection.setHostName(dbHostname);
    dbConnection.setDatabaseName(dbName);
    dbConnection.setPort(dbPort);
    dbConnection.setUserName(dbUsername);
    dbConnection.setPassword(dbPassword);
    if (! dbConnection.open())
    {
        logError("Open DB failed: " + dbHostname + "//" + dbName +"\n" + dbConnection.lastError().text());
        dbConnection.close();
        return(false);
    }

    return (true);
}


void Project::setProxyDEM()
{
    int index = interpolationSettings.getIndexHeight();
    int indexQuality = qualityInterpolationSettings.getIndexHeight();

    // if no elevation proxy defined nothing to do
    if (index == NODATA && indexQuality == NODATA) return;

    Crit3DProxy* proxyHeight;

    if (index != NODATA)
    {
        proxyHeight = interpolationSettings.getProxy(index);

        // if no alternative DEM defined and project DEM loaded, use it for elevation proxy
        if (proxyHeight->getGridName() == "" && DTM.isLoaded)
            proxyHeight->setGrid(&DTM);
    }

    if (indexQuality != NODATA)
    {
        proxyHeight = qualityInterpolationSettings.getProxy(indexQuality);
        if (proxyHeight->getGridName() == "" && DTM.isLoaded)
            proxyHeight->setGrid(&DTM);
    }
}

bool Project::checkProxy(std::string name_, std::string gridName_, std::string table_, std::string field_, QString *error)
{
    if (name_ == "")
    {
        *error = "no name";
        return false;
    }

    bool isHeight = (getProxyPragaName(name_) == height);

    if (gridName_ == "" && !isHeight && (table_ == "" && field_ == ""))
    {
        *error = "error reading grid, table or field for proxy " + QString::fromStdString(name_);
        return false;
    }

    return true;

}
void Project::addProxy(std::string name_, std::string gridName_, std::string table_, std::string field_,
                                bool isForQuality_, bool isActive_)
{
    Crit3DProxy myProxy;

    myProxy.setName(name_);
    myProxy.setGridName(gridName_);
    myProxy.setProxyTable(table_);
    myProxy.setProxyField(field_);
    myProxy.setForQualityControl(isForQuality_);

    interpolationSettings.addProxy(myProxy, isActive_);
    if (isForQuality_)
        qualityInterpolationSettings.addProxy(myProxy, isActive_);

    if (getProxyPragaName(name_) == height) setProxyDEM();
}

bool Project::loadParameters(QString parametersFileName)
{
    if (! QFile(parametersFileName).exists())
    {
        logError("Missing file: " + parametersFileName);
        return false;
    }

    parameters = new QSettings(parametersFileName, QSettings::IniFormat);

    //interpolation settings
    interpolationSettings.initialize();
    qualityInterpolationSettings.initialize();

    QString gridName;
    std::string proxyName, proxyGridName, proxyTable, proxyField;
    bool isActive, forQuality;

    Q_FOREACH (QString group, parameters->childGroups())
    {
        //meteo settings
        if (group == "meteo")
        {
            parameters->beginGroup(group);

            if (parameters->contains("min_percentage") && !parameters->value("min_percentage").toString().isEmpty())
            {
                meteoSettings->setMinimumPercentage(parameters->value("min_percentage").toFloat());
            }
            if (parameters->contains("prec_threshold") && !parameters->value("prec_threshold").toString().isEmpty())
            {
                meteoSettings->setRainfallThreshold(parameters->value("prec_threshold").toFloat());
            }
            if (parameters->contains("thom_threshold") && !parameters->value("thom_threshold").toString().isEmpty())
            {
                meteoSettings->setThomThreshold(parameters->value("thom_threshold").toFloat());
            }
            if (parameters->contains("samani_coefficient") && !parameters->value("samani_coefficient").toString().isEmpty())
            {
                meteoSettings->setTransSamaniCoefficient(parameters->value("samani_coefficient").toFloat());
            }
            if (parameters->contains("hourly_intervals") && !parameters->value("hourly_intervals").toString().isEmpty())
            {
                meteoSettings->setHourlyIntervals(parameters->value("hourly_intervals").toInt());
            }
            if (parameters->contains("wind_intensity_default") && !parameters->value("wind_intensity_default").toString().isEmpty())
            {
                meteoSettings->setWindIntensityDefault(parameters->value("wind_intensity_default").toInt());
            }

            parameters->endGroup();
        }

        //interpolation
        if (group == "interpolation")
        {
            parameters->beginGroup(group);

            if (parameters->contains("algorithm"))
            {
                std::string algorithm = parameters->value("algorithm").toString().toStdString();
                if (interpolationMethodNames.find(algorithm) == interpolationMethodNames.end())
                {
                    errorString = "Unknown interpolation method";
                    return false;
                }
                else
                    interpolationSettings.setInterpolationMethod(interpolationMethodNames.at(algorithm));
            }

            if (parameters->contains("aggregationMethod"))
            {
                std::string aggrMethod = parameters->value("aggregationMethod").toString().toStdString();
                if (aggregationMethodToString.find(aggrMethod) == aggregationMethodToString.end())
                {
                    errorString = "Unknown aggregation method";
                    return false;
                }
                else
                    interpolationSettings.setMeteoGridAggrMethod(aggregationMethodToString.at(aggrMethod));
            }

            if (parameters->contains("thermalInversion"))
            {
                interpolationSettings.setUseThermalInversion(parameters->value("thermalInversion").toBool());
                qualityInterpolationSettings.setUseThermalInversion(parameters->value("thermalInversion").toBool());
            }

            if (parameters->contains("topographicDistance"))
                interpolationSettings.setUseTAD(parameters->value("topographicDistance").toBool());

            if (parameters->contains("lapseRateCode"))
            {
                interpolationSettings.setUseLapseRateCode(parameters->value("lapseRateCode").toBool());
                qualityInterpolationSettings.setUseLapseRateCode(parameters->value("lapseRateCode").toBool());
            }

            if (parameters->contains("optimalDetrending"))
                interpolationSettings.setUseBestDetrending(parameters->value("optimalDetrending").toBool());

            if (parameters->contains("minRegressionR2"))
            {
                interpolationSettings.setMinRegressionR2(parameters->value("minRegressionR2").toFloat());
                qualityInterpolationSettings.setMinRegressionR2(parameters->value("minRegressionR2").toFloat());
            }

            if (parameters->contains("useDewPoint"))
                interpolationSettings.setUseDewPoint(parameters->value("useDewPoint").toBool());

            parameters->endGroup();

        }

        if (group == "quality")
        {
            parameters->beginGroup(group);
            if (parameters->contains("reference_height") && !parameters->value("reference_height").toString().isEmpty())
            {
                quality->setReferenceHeight(parameters->value("reference_height").toFloat());
            }
            if (parameters->contains("delta_temperature_suspect") && !parameters->value("delta_temperature_suspect").toString().isEmpty())
            {
                quality->setDeltaTSuspect(parameters->value("delta_temperature_suspect").toFloat());
            }
            if (parameters->contains("delta_temperature_wrong") && !parameters->value("delta_temperature_wrong").toString().isEmpty())
            {
                quality->setDeltaTWrong(parameters->value("delta_temperature_wrong").toFloat());
            }
            if (parameters->contains("relhum_tolerance") && !parameters->value("relhum_tolerance").toString().isEmpty())
            {
                quality->setRelHumTolerance(parameters->value("relhum_tolerance").toFloat());
            }

            parameters->endGroup();
        }

        //proxy variables (for interpolation)
        if (group.startsWith("proxy"))
        {
            proxyName = group.right(group.size()-6).toStdString();

            parameters->beginGroup(group);
            gridName = parameters->value("raster").toString();
            if (gridName.left(1) == ".")
                proxyGridName = path.toStdString() + gridName.toStdString();
            else
                proxyGridName = gridName.toStdString();
            proxyTable = parameters->value("table").toString().toStdString();
            proxyField = parameters->value("field").toString().toStdString();
            isActive = parameters->value("active").toBool();
            forQuality = parameters->value("useForSpatialQualityControl").toBool();
            parameters->endGroup();

            if (checkProxy(proxyName, proxyGridName, proxyTable, proxyField, &errorString))
                addProxy(proxyName, proxyGridName, proxyTable, proxyField, forQuality, isActive);
            else
                return false;

        }
    }

    // check proxy grids for detrending
    if (!loadProxyGrids())
    {
        errorString = "Error loading proxy grids";
        return false;
    }

    return true;
}


bool Project::loadCommonSettings(QString settingsFileName)
{
    this->path = getFilePath(settingsFileName);

    if (! QFile(settingsFileName).exists())
    {
        logError("Missing settings file: " + settingsFileName);
        return false;
    }

    projectSettings = new QSettings(settingsFileName, QSettings::IniFormat);

    projectSettings->beginGroup("path");
    QString myPath = projectSettings->value("path").toString();
    projectSettings->endGroup();

    if (! myPath.isEmpty())
    {
        if (myPath.right(1) != "/")
            myPath += "/";

        if (myPath.left(2) == "./")
        {
            this->path += myPath.right(myPath.length()-2);
        }
        else if(myPath.left(2) == "..")
        {
            this->path += myPath;
        }
        else
        {
            this->path = myPath;
        }
    }

    inizializeDBConnection();
    projectSettings->beginGroup("database");
        if (projectSettings->contains("driver") && !projectSettings->value("driver").toString().isEmpty()) dbProvider = projectSettings->value("driver").toString();
        if (projectSettings->contains("host") && !projectSettings->value("host").toString().isEmpty()) dbHostname = projectSettings->value("host").toString();
        if (projectSettings->contains("port") && !projectSettings->value("port").toString().isEmpty()) dbPort = projectSettings->value("port").toInt();
        if (projectSettings->contains("dbname") && !projectSettings->value("dbname").toString().isEmpty()) dbName = projectSettings->value("dbname").toString();
        if (projectSettings->contains("username") && !projectSettings->value("username").toString().isEmpty()) dbUsername = projectSettings->value("username").toString();
        if (projectSettings->contains("password") && !projectSettings->value("password").toString().isEmpty()) dbPassword = projectSettings->value("password").toString();
    projectSettings->endGroup();

    projectSettings->beginGroup("location");
        float latitude = projectSettings->value("lat").toFloat();
        float longitude = projectSettings->value("lon").toFloat();
        int utmZone = projectSettings->value("utm_zone").toInt();
        int isUtc = projectSettings->value("is_utc").toBool();
    projectSettings->endGroup();

    if (latitude != 0 && longitude != 0)
    {
        gisSettings.startLocation.latitude = latitude;
        gisSettings.startLocation.longitude = longitude;
        gisSettings.utmZone = utmZone;
        gisSettings.isUTC = isUtc;
    }

    return true;
}


bool Project::getMeteoPointSelected(int i)
{
    if (meteoPointsSelected.isEmpty()) return true;

    for (int j = 0; j < meteoPointsSelected.size(); j++)
    {
        if (abs(meteoPoints[i].latitude - meteoPointsSelected[j].latitude) < EPSILON
            && abs(meteoPoints[i].longitude - meteoPointsSelected[j].longitude) < EPSILON)
            return true;
    }

    return false;
}

void Project::setPath(QString myPath)
{
    this->path = myPath;
}

QString Project::getPath()
{
    return this->path;
}

void Project::setFrequency(frequencyType frequency)
{
    this->currentFrequency = frequency;
}

frequencyType Project::getFrequency()
{
    return this->currentFrequency;
}

void Project::setCurrentVariable(meteoVariable variable)
{
    this->currentVariable = variable;
}

meteoVariable Project::getCurrentVariable()
{
    return this->currentVariable;
}

void Project::setCurrentDate(QDate myDate)
{
    if (myDate != this->currentDate)
    {
        this->previousDate = this->currentDate;
        this->currentDate = myDate;
    }
}

QDate Project::getCurrentDate()
{
    return this->currentDate;
}

void Project::setCurrentHour(int myHour)
{
    this->currentHour = myHour;
}

int Project::getCurrentHour()
{
    return this->currentHour;
}

Crit3DTime Project::getCurrentTime()
{
    return getCrit3DTime(this->currentDate, this->currentHour);
}

void Project::getMeteoPointsRange(float *minimum, float *maximum)
{
    *minimum = NODATA;
    *maximum = NODATA;

    if (currentFrequency == noFrequency || currentVariable == noMeteoVar)
        return;

    float v;
    for (int i = 0; i < nrMeteoPoints; i++)
    {
        v = meteoPoints[i].currentValue;

        if (int(v) != int(NODATA) && meteoPoints[i].quality == quality::accepted)
        {
            if (int(*minimum) == int(NODATA))
            {
                *minimum = v;
                *maximum = v;
            }
            else if (v < *minimum) *minimum = v;
            else if (v > *maximum) *maximum = v;
        }
    }
}

void Project::closeMeteoPointsDB()
{

    if (meteoPointsDbHandler != nullptr)
    {
        delete meteoPointsDbHandler;
    }

    if (meteoPoints != nullptr)
    {
        delete [] meteoPoints;
    }

    meteoPointsDbHandler = nullptr;
    meteoPoints = nullptr;

    meteoPointsSelected.clear();
    nrMeteoPoints = 0;
}

void Project::closeMeteoGridDB()
{

    if (meteoGridDbHandler != nullptr)
    {
        delete meteoGridDbHandler;
    }

    meteoGridDbHandler = nullptr;

}


/*!
 * \brief loadDEM
 * \param myFileName the name of the DEM file
 * \return true if file is ok, false otherwise
 */
bool Project::loadDEM(QString myFileName)
{
    this->logInfoInfo("Read Digital Elevation Model...");

    std::string error, fileName;
    if (myFileName.right(4).left(1) == ".")
    {
        myFileName = myFileName.left(myFileName.length()-4);
    }
    fileName = myFileName.toStdString();

    if (! gis::readEsriGrid(fileName, &DTM, &error))
    {
        this->logInfoError("Wrong DEM file.\n" + QString::fromStdString(error));
        return false;
    }

    setColorScale(noMeteoTerrain, DTM.colorScale);

    // initialize radition maps: slope, aspect, lat/lon
    if (radiationMaps != nullptr)
    {
        radiationMaps->clean();
    }
    radiationMaps = new Crit3DRadiationMaps(DTM, gisSettings);

    //reset aggregationPoints meteoGrid
    if (meteoGridDbHandler != nullptr)
    {
        meteoGridDbHandler->meteoGrid()->setIsAggregationDefined(false);
        // TODO
        // ciclo sulle celle della meteo grid -> clean vettore aggregation points
    }

    setProxyDEM();

    //set interpolation settings DEM
    interpolationSettings.setCurrentDEM(&DTM);
    qualityInterpolationSettings.setCurrentDEM(&DTM);

    //check points position with respect to DEM
    checkMeteoPointsDEM();

    this->logInfoInfo("DEM = " + myFileName);
    return true;
}


bool Project::loadMeteoPointsDB(QString dbName)
{
    if (dbName == "") return false;

    closeMeteoPointsDB();

    meteoPointsDbHandler = new Crit3DMeteoPointsDbHandler(dbName);
    if (meteoPointsDbHandler->error != "")
    {
        logError(meteoPointsDbHandler->error);
        closeMeteoPointsDB();
        return false;
    }

    if (! meteoPointsDbHandler->loadVariableProperties())
    {
        logError(meteoPointsDbHandler->error);
        closeMeteoPointsDB();
        return false;
    }
    QList<Crit3DMeteoPoint> listMeteoPoints = meteoPointsDbHandler->getPropertiesFromDb(gisSettings, &errorString);

    nrMeteoPoints = listMeteoPoints.size();
    if (nrMeteoPoints == 0)
    {
        errorString = "Error in reading the point properties:\n" + errorString;
        this->logInfoError();
        this->closeMeteoPointsDB();
        return false;
    }

    meteoPoints = new Crit3DMeteoPoint[unsigned(nrMeteoPoints)];

    for (int i=0; i < nrMeteoPoints; i++)
        meteoPoints[i] = listMeteoPoints[i];

    listMeteoPoints.clear();

    // find last date
    findLastMeteoDate();

    // load proxy values for detrending
    if (! readProxyValues())
    {
        logInfo("Error reading proxy values");
    }

    //position with respect to DEM
    if (DTM.isLoaded)
        checkMeteoPointsDEM();

    return true;
}

bool Project::loadMeteoGridDB(QString xmlName)
{
    if (xmlName == "")
        return false;

    meteoGridDbHandler = new Crit3DMeteoGridDbHandler();
    meteoGridDbHandler->meteoGrid()->setGisSettings(this->gisSettings);


    if (! meteoGridDbHandler->parseXMLGrid(xmlName, &errorString))
        return false;

    if (! this->meteoGridDbHandler->openDatabase(&errorString))
        return false;

    if (! this->meteoGridDbHandler->loadCellProperties(&errorString))
        return false;

    if (! this->meteoGridDbHandler->updateGridDate(&errorString))
        return false;

    return true;
}

bool Project::loadAggregationdDB(QString dbName)
{
    if (dbName == "") return false;

    aggregationDbHandler = new Crit3DAggregationsDbHandler(dbName);
    if (aggregationDbHandler->error() != "")
    {
        logError(aggregationDbHandler->error());
        return false;
    }
    if (aggregationDbHandler->loadVariableProperties())
    {
        return false;
    }
    return true;
}

bool Project::loadMeteoPointsData(QDate firstDate, QDate lastDate, bool showInfo)
{
    //check
    if (firstDate == QDate(1800,1,1) || lastDate == QDate(1800,1,1)) return false;

    bool isData = false;
    FormInfo myInfo;
    int step = 0;

    QString infoStr = "Load data: " + firstDate.toString();

    if (firstDate != lastDate)
        infoStr += " - " + lastDate.toString();

    if (showInfo)
        step = myInfo.start(infoStr, nrMeteoPoints);

    for (int i=0; i < nrMeteoPoints; i++)
    {
        if (showInfo)
        {
            if ((i % step) == 0) myInfo.setValue(i);
        }

        if (meteoPointsDbHandler->loadDailyData(getCrit3DDate(firstDate), getCrit3DDate(lastDate), &(meteoPoints[i])))
        {
            isData = true;
        }

        if (meteoPointsDbHandler->loadHourlyData(getCrit3DDate(firstDate), getCrit3DDate(lastDate), &(meteoPoints[i])))
        {
            isData = true;
        }
    }
    if (showInfo) myInfo.close();

    return isData;
}


void Project::loadMeteoGridData(QDate firstDate, QDate lastDate, bool showInfo)
{
    if (this->meteoGridDbHandler != nullptr)
    {
        QDateTime t1 = QDateTime(firstDate);
        QDateTime t2 = QDateTime(lastDate.addDays(1));

        this->loadMeteoGridDailyData(firstDate, lastDate, showInfo);

        this->loadMeteoGridHourlyData(t1, t2, showInfo);
    }
}


bool Project::loadMeteoGridDailyData(QDate firstDate, QDate lastDate, bool showInfo)
{
    std::string id;
    int count = 0;

    FormInfo myInfo;
    int infoStep = 1;

    if (showInfo)
    {
        QString infoStr = "Load grid daily data: " + firstDate.toString();
        if (firstDate != lastDate) infoStr += " - " + lastDate.toString();
        infoStep = myInfo.start(infoStr, this->meteoGridDbHandler->gridStructure().header().nrRows);
    }

    for (int row = 0; row < this->meteoGridDbHandler->gridStructure().header().nrRows; row++)
    {
        if (showInfo && (row % infoStep) == 0)
            myInfo.setValue(row);

        for (int col = 0; col < this->meteoGridDbHandler->gridStructure().header().nrCols; col++)
        {
            if (this->meteoGridDbHandler->meteoGrid()->getMeteoPointActiveId(row, col, &id))
            {
                if (!this->meteoGridDbHandler->gridStructure().isFixedFields())
                {
                    if (this->meteoGridDbHandler->loadGridDailyData(&errorString, QString::fromStdString(id), firstDate, lastDate))
                    {
                        count = count + 1;
                    }
                }
                else
                {
                    if (this->meteoGridDbHandler->loadGridDailyDataFixedFields(&errorString, QString::fromStdString(id), firstDate, lastDate))
                    {
                        count = count + 1;
                    }
                }
            }
        }
    }

    if (showInfo) myInfo.close();

    if (count == 0)
    {
        errorString = "No Data Available";
        return false;
    }
    else
        return true;
}


bool Project::loadMeteoGridHourlyData(QDateTime firstDate, QDateTime lastDate, bool showInfo)
{
    std::string id;
    int count = 0;
    FormInfo myInfo;
    int infoStep = 1;

    if (showInfo)
    {
        QString infoStr = "Load grid hourly data: " + firstDate.toString("yyyy-MM-dd:hh") + " - " + lastDate.toString("yyyy-MM-dd:hh");
        infoStep = myInfo.start(infoStr, this->meteoGridDbHandler->gridStructure().header().nrRows);
    }

    for (int row = 0; row < this->meteoGridDbHandler->gridStructure().header().nrRows; row++)
    {
        if (showInfo && (row % infoStep) == 0)
            myInfo.setValue(row);

        for (int col = 0; col < this->meteoGridDbHandler->gridStructure().header().nrCols; col++)
        {
            if (this->meteoGridDbHandler->meteoGrid()->getMeteoPointActiveId(row, col, &id))
            {
                if (!this->meteoGridDbHandler->gridStructure().isFixedFields())
                {
                    if (this->meteoGridDbHandler->loadGridHourlyData(&errorString, QString::fromStdString(id), firstDate, lastDate))
                    {
                        count = count + 1;
                    }
                }
                else
                {
                    if (this->meteoGridDbHandler->loadGridHourlyDataFixedFields(&errorString, QString::fromStdString(id), firstDate, lastDate))
                    {
                        count = count + 1;
                    }
                }
            }
        }
    }

    if (showInfo) myInfo.close();

    if (count == 0)
    {
        errorString = "No Data Available";
        return false;
    }
    else
        return true;
}


void Project::findLastMeteoDate()
{
    QDate lastDateD = meteoPointsDbHandler->getLastDate(daily).date();
    QDate lastDateH = meteoPointsDbHandler->getLastDate(hourly).date();

    QDate lastDate = (lastDateD > lastDateH) ? lastDateD : lastDateH;

    setCurrentDate(lastDate);
    setCurrentHour(12);
}


void Project::checkMeteoPointsDEM()
{
    for (int i=0; i < nrMeteoPoints; i++)
        meteoPoints[i].isInsideDem = ! gis::isOutOfGridXY(meteoPoints[i].point.utm.x, meteoPoints[i].point.utm.y, DTM.header);
}


bool Project::readPointProxyValues(Crit3DMeteoPoint* myPoint, QSqlDatabase* myDb)
{
    if (myPoint == nullptr) return false;

    QSqlQuery qry(*myDb);

    QString proxyField;
    QString proxyTable;
    QString statement;
    int nrProxy;
    Crit3DProxy* myProxy;

    nrProxy = interpolationSettings.getProxyNr();
    myPoint->proxyValues.resize(unsigned(nrProxy));

    for (unsigned int i=0; i < unsigned(nrProxy); i++)
    {
        myPoint->proxyValues.at(i) = NODATA;

        // read only for active proxies
        if (interpolationSettings.getSelectedCombination().getValue(signed(i)))
        {
            myProxy = interpolationSettings.getProxy(signed(i));
            proxyField = QString::fromStdString(myProxy->getProxyField());
            proxyTable = QString::fromStdString(myProxy->getProxyTable());
            if (proxyField != "" && proxyTable != "")
            {
                statement = QString("SELECT %1 FROM %2 WHERE id_point = '%3'").arg(proxyField).arg(proxyTable).arg(QString::fromStdString((*myPoint).id));
                if(qry.exec(statement))
                {
                    qry.last();
                    if (qry.value(proxyField) != "")
                        myPoint->proxyValues.at(i) = qry.value(proxyField).toFloat();
                }
            }

            if (int(myPoint->proxyValues.at(i)) == int(NODATA))
            {
                gis::Crit3DRasterGrid* proxyGrid = myProxy->getGrid();
                if (proxyGrid == nullptr || ! proxyGrid->isLoaded)
                    return false;
                else
                {
                    float myValue = gis::getValueFromXY(*proxyGrid, myPoint->point.utm.x, myPoint->point.utm.y);
                    if (int(myValue) != int(proxyGrid->header->flag))
                        myPoint->proxyValues.at(unsigned(i)) = myValue;
                }
            }
        }
    }

    return true;
}

bool Project::loadProxyGrids()
{
    std::string* myError = new std::string();
    Crit3DProxy *myProxy;
    gis::Crit3DRasterGrid *myGrid;
    std::string gridName;

    for (int i=0; i < interpolationSettings.getProxyNr(); i++)
    {
        myProxy = interpolationSettings.getProxy(i);
        myGrid = myProxy->getGrid();
        gridName = myProxy->getGridName();

        if (interpolationSettings.getSelectedCombination().getValue(i) || myProxy->getForQualityControl())
        {
            if (myGrid == nullptr && gridName != "")
            {
                myGrid = new gis::Crit3DRasterGrid();
                if (!gis::readEsriGrid(gridName, myGrid, myError))
                    return false;
            }
        }
    }

    return true;
}

bool Project::readProxyValues()
{
    if (dbProvider == "QSQLITE")
    {
        if (meteoPointsDbHandler == nullptr)
            return false;

        QSqlDatabase myDb = this->meteoPointsDbHandler->getDb();
        for (int i = 0; i < this->nrMeteoPoints; i++)
            if (! readPointProxyValues(&(this->meteoPoints[i]), &myDb))
                return false;
    }
    else
    {
        if (dbConnection.isOpen())
            for (int i = 0; i < this->nrMeteoPoints; i++)
                if (! readPointProxyValues(&(this->meteoPoints[i]), &(this->dbConnection)))
                    return false;
    }

    return true;
}

bool Project::updateProxy()
{
    if (! loadProxyGrids()) return false;
    if (! readProxyValues()) return false;
    return true;
}

bool Project::writeTopographicDistanceMaps()
{
    if (nrMeteoPoints == 0)
    {
        errorString = "Open a meteo points DB before.";
        return false;
    }

    if (! DTM.isLoaded)
    {
        errorString = "Load a DEM before.";
        return false;
    }

    QString mapsFolder = this->path + "DATA/GEO/TAD/";
    if (! QDir(mapsFolder).exists())
        QDir().mkdir(mapsFolder);

    FormInfo myInfo;
    int infoStep;
    QString infoStr = "Computing topographic distance maps...";
    infoStep = myInfo.start(infoStr, nrMeteoPoints);

    std::string myError;
    std::string fileName;
    gis::Crit3DRasterGrid myMap;
    for (int i=0; i < nrMeteoPoints; i++)
    {
        myInfo.setValue(i);

        if (meteoPoints[i].active)
        {
            if (gis::topographicDistanceMap(meteoPoints[i].point, DTM, &myMap))
            {
                fileName = mapsFolder.toStdString() + "TAD_" + meteoPoints[i].id;
                if (! gis::writeEsriGrid(fileName, &myMap, &myError))
                {
                    errorString = QString::fromStdString(myError);
                    return false;
                }
            }
        }
    }

    myInfo.close();

    return true;
}


bool Project::loadTopographicDistanceMaps()
{
    if (nrMeteoPoints == 0)
    {
        errorString = "Open a meteo points DB before.";
        return false;
    }

    QString mapsFolder = this->path + "DATA/GEO/TAD/";
    if (! QDir(mapsFolder).exists()) return false;

    FormInfo myInfo;
    int infoStep;
    QString infoStr = "Loading topographic distance maps...";
    infoStep = myInfo.start(infoStr, nrMeteoPoints);

    std::string myError;
    std::string fileName;


    for (int i=0; i < nrMeteoPoints; i++)
    {
        myInfo.setValue(i);

        if (meteoPoints[i].active)
        {
            fileName = mapsFolder.toStdString() + "TAD_" + meteoPoints[i].id;
            meteoPoints[i].topographicDistance = new gis::Crit3DRasterGrid();
            if (! gis::readEsriGrid(fileName, meteoPoints[i].topographicDistance, &myError))
            {
                errorString = QString::fromStdString(myError);
                return false;
            }
        }
    }

    myInfo.close();

    return true;
}


bool Project::interpolationDem(meteoVariable myVar, const Crit3DTime& myTime, gis::Crit3DRasterGrid *myRaster, bool showInfo)
{
    std::vector <Crit3DInterpolationDataPoint> interpolationPoints;

    // check quality and pass data to interpolation
    if (!checkAndPassDataToInterpolation(quality, myVar, meteoPoints, nrMeteoPoints, myTime,
                                         &qualityInterpolationSettings, &interpolationSettings, interpolationPoints,
                                         checkSpatialQuality))
    {
        errorString = "No data available";
        return false;
    }

    // Proxy vars regression and detrend
    if (showInfo)
    {
        FormInfo myInfo;
        myInfo.start("Preparing interpolation...", 0);
    }

    //detrending and checking precipitation
    if (! preInterpolation(interpolationPoints, &interpolationSettings, meteoPoints, nrMeteoPoints, myVar, myTime))
    {
        errorString = "Interpolation: error in function preInterpolation";
        return false;
    }

    // Interpolate
    if (! interpolationRaster(interpolationPoints, &interpolationSettings, myRaster, DTM, myVar, showInfo))
    {
        errorString = "Interpolation: error in function interpolateGridDtm";
        return false;
    }

    Crit3DTime t = myTime;
    myRaster->timeString = t.toStdString();
    return true;
}


bool Project::interpolateDemRadiation(const Crit3DTime& myTime, gis::Crit3DRasterGrid *myRaster, bool showInfo)
{
    std::vector <Crit3DInterpolationDataPoint> interpolationPoints;

    radSettings.setGisSettings(&gisSettings);

    gis::Crit3DPoint mapCenter = DTM.mapCenter();

    int intervalWidth = radiation::estimateTransmissivityWindow(&radSettings, DTM, &mapCenter, myTime, int(HOUR_SECONDS));

    // almost a meteoPoint with transmissivity data
    if (! computeTransmissivity(&radSettings, meteoPoints, nrMeteoPoints, intervalWidth, myTime, DTM))
        if (! computeTransmissivityFromTRange(meteoPoints, nrMeteoPoints, myTime))
        {
            errorString = "Function interpolateRasterRadiation: it is not possible to compute transmissivity.";
            return false;
        }

    if (! checkAndPassDataToInterpolation(quality, atmTransmissivity, meteoPoints, nrMeteoPoints,
                                        myTime, &qualityInterpolationSettings,
                                        &interpolationSettings, interpolationPoints, checkSpatialQuality))
    {
        errorString = "Function interpolateRasterRadiation: not enough transmissivity data.";
        return false;
    }

    preInterpolation(interpolationPoints, &interpolationSettings, meteoPoints, nrMeteoPoints, atmTransmissivity, myTime);

    if (! interpolationRaster(interpolationPoints, &interpolationSettings, this->radiationMaps->transmissivityMap, DTM, atmTransmissivity, showInfo))
    {
        errorString = "Function interpolateRasterRadiation: error interpolating transmissivity.";
        return false;
    }

    if (! radiation::computeRadiationGridPresentTime(&radSettings, this->DTM, this->radiationMaps, myTime))
    {
        errorString = "Function interpolateRasterRadiation: error computing solar radiation";
        return false;
    }

    if (myRaster != this->radiationMaps->globalRadiationMap)
    {
        myRaster->copyGrid(*(this->radiationMaps->globalRadiationMap));
    }

    return true;
}


bool Project::interpolationDemMain(meteoVariable myVar, const Crit3DTime& myTime, gis::Crit3DRasterGrid *myRaster, bool showInfo)
{
    if (! DTM.isLoaded)
    {
        errorString = "Load a DEM before.";
        return false;
    }

    if (nrMeteoPoints == 0)
    {
        errorString = "Open a meteo points DB before.";
        return false;
    }

    if (myVar == noMeteoVar)
    {
        errorString = "Select a variable before.";
        return false;
    }

    myRaster->initializeGrid(this->DTM);

    if (myVar == globalIrradiance)
    {
        Crit3DTime measureTime = myTime.addSeconds(-1800);
        return interpolateDemRadiation(measureTime, myRaster, showInfo);
    }
    else
    {
        return interpolationDem(myVar, myTime, myRaster, showInfo);
    }
}


float Project::meteoDataConsistency(meteoVariable myVar, const Crit3DTime& timeIni, const Crit3DTime& timeFin)
{
    float dataConsistency = 0.0;
    for (int i = 0; i < nrMeteoPoints; i++)
        dataConsistency = maxValue(dataConsistency, meteoPoints[i].obsDataConsistencyH(myVar, timeIni, timeFin));

    return dataConsistency;
}


QString Project::getCompleteFileName(QString fileName, QString secondaryPath)
{
    if (getFilePath(fileName) == "" || fileName.left(1) == ".")
    {
        QString completeFileName = this->getPath() + secondaryPath + fileName;
        return QDir().cleanPath(completeFileName);
    }
    else
    {
        return fileName;
    }
}


/* ---------------------------------------------
 * LOG functions
 * --------------------------------------------*/

bool Project::setLogFile(QString fileNameWithPath)
{
    QString filePath = getFilePath(fileNameWithPath);
    QString fileName = getFileName(fileNameWithPath);

    if (!QDir(filePath).exists())
    {
         QDir().mkdir(filePath);
    }

    QString myDate = QDateTime().currentDateTime().toString("yyyy-MM-dd hh mm ");
    fileName = myDate + fileName;

    logFileName = filePath + fileName;

    logFile.open(logFileName.toStdString().c_str());
    if (logFile.is_open())
    {
        logInfo("LogFile: " + logFileName);
        return true;
    }
    else
    {
        logError("Unable to open file: " + logFileName);
        return false;
    }
}


void Project::logInfo(QString myStr)
{
    // standard output in all modalities
    std::cout << myStr.toStdString() << std::endl;

    if (logFile.is_open())
    {
        logFile << myStr.toStdString() << std::endl;
    }
}

void Project::logError(QString myStr)
{
    errorString = myStr;
    logError();
}


void Project::logError()
{
    if (logFile.is_open())
    {
        logFile << "ERROR! " << errorString.toStdString() << std::endl;
    }

    if (modality == MODE_GUI)
    {
        QMessageBox::critical(nullptr, "ERROR!", errorString);
    }
    else
    {
        std::cout << "ERROR! " << errorString.toStdString() << std::endl;
    }
}



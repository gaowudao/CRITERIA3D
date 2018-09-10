#include <QLabel>
#include <QtDebug>
#include <QMessageBox>

#include "project.h"
#include "formRunInfo.h"
#include "commonConstants.h"
#include "spatialControl.h"
#include "radiationSettings.h"
#include "solarRadiation.h"
#include "interpolation.h"
#include "transmissivity.h"
#include "climate.h"
#include "utilities.h"

#include <iostream> //debug

bool Project::getIsElabMeteoPointsValue() const
{
    return isElabMeteoPointsValue;
}

void Project::setIsElabMeteoPointsValue(bool value)
{
    isElabMeteoPointsValue = value;
}

Project::Project()
{
    path = "";
    logFileName = "";
    errorString = "";

    meteoPoints = NULL;
    nrMeteoPoints = 0;
    quality = new Crit3DQuality();
    checkSpatialQuality = true;
    currentVariable = noMeteoVar;
    currentFrequency = noFrequency;
    currentDate.setDate(1800,1,1);
    previousDate = currentDate;
    currentHour = 12;
    meteoPointsColorScale = new Crit3DColorScale();
    meteoPointsDbHandler = NULL;
    meteoGridDbHandler = NULL;
    clima = NULL;
    climaFromDb = NULL;
    referenceClima = NULL;

    radiationMaps = NULL;

    // default: Bologna
    startLocation.latitude = 44.5;
    startLocation.longitude = 11.35;
}

bool Project::readSettings()
{
    //interpolation settings
    interpolationSettings.initialize();
    qualityInterpolationSettings.initialize();

    Q_FOREACH (QString group, settings->childGroups())
    {
        //interpolation
        if (group == "interpolation")
        {
            settings->beginGroup(group);

            if (settings->contains("algorithm"))
            {
                std::string algorithm = settings->value("algorithm").toString().toStdString();
                if (interpolationMethodNames.find(algorithm) == interpolationMethodNames.end())
                {
                    errorString = "Unknown interpolation method";
                    return false;
                }
                else
                    interpolationSettings.setInterpolationMethod(interpolationMethodNames.at(algorithm));
            }

            if (settings->contains("gridAggregationMethod"))
            {
                std::string aggrMethod = settings->value("gridAggregationMethod").toString().toStdString();
                if (aggregationMethodNames.find(aggrMethod) == aggregationMethodNames.end())
                {
                    errorString = "Unknown aggregation method";
                    return false;
                }
                else
                    interpolationSettings.setMeteoGridAggrMethod(aggregationMethodNames.at(aggrMethod));
            }

            if (settings->contains("thermalInversion"))
                interpolationSettings.setUseThermalInversion(settings->value("thermalInversion").toBool());

            if (settings->contains("topographicDistance"))
                interpolationSettings.setUseTAD(settings->value("topographicDistance").toBool());

            if (settings->contains("lapseRateCode"))
                interpolationSettings.setUseLapseRateCode(settings->value("lapseRateCode").toBool());

            if (settings->contains("optimalDetrending"))
                interpolationSettings.setUseBestDetrending(settings->value("optimalDetrending").toBool());

            if (settings->contains("minRegressionR2"))
                interpolationSettings.setMinRegressionR2(settings->value("minRegressionR2").toFloat());

            if (settings->contains("useDewPoint"))
                interpolationSettings.setUseDewPoint(settings->value("useDewPoint").toFloat());

            settings->endGroup();
        }
        if (group == "quality")
        {
            settings->beginGroup(group);
            if (settings->contains("reference_height") && !settings->value("reference_height").toString().isEmpty())
            {
                qInfo() << "value: " << settings->value("reference_height");
                quality->setReferenceHeight(settings->value("reference_height").toFloat());
            }
            if (settings->contains("delta_temperature_suspect") && !settings->value("delta_temperature_suspect").toString().isEmpty())
            {
                quality->setDeltaTSuspect(settings->value("delta_temperature_suspect").toFloat());
            }
            if (settings->contains("delta_temperature_wrong") && !settings->value("delta_temperature_wrong").toString().isEmpty())
            {
                quality->setDeltaTWrong(settings->value("delta_temperature_wrong").toFloat());
            }
            if (settings->contains("relhum_tolerance") && !settings->value("relhum_tolerance").toString().isEmpty())
            {
                quality->setRelHumTolerance(settings->value("relhum_tolerance").toFloat());
            }

            settings->endGroup();

        }
    }

    return true;
}

bool Project::initializeSettings(QString currentPath)
{
    this->path = currentPath;
    QString pathFileName = currentPath + "praga.ini";

    if (QFile(pathFileName).exists())
    {
        QSettings pathSetting(pathFileName, QSettings::IniFormat);

        pathSetting.beginGroup("path");
        QString pragaPath = pathSetting.value("PragaPath").toString();
        pathSetting.endGroup();
        if (! pragaPath.isEmpty())
        {
            if (pragaPath.right(1) != "/" || pragaPath.right(1) != "\\" ) { pragaPath += "/"; }

            this->path = pragaPath;
        }

        pathSetting.beginGroup("location");
        float latitude = pathSetting.value("lat").toFloat();
        float longitude = pathSetting.value("lon").toFloat();
        pathSetting.endGroup();

        if (latitude != 0 && longitude != 0)
        {
            this->startLocation.latitude = latitude;
            this->startLocation.longitude = longitude;
        }
    }

    QString settingsFileName = this->path + "DATA/settings/parameters.ini";
    if (! QFile(settingsFileName).exists())
    {
        logError("Missing file: " + settingsFileName);
        return false;
    }
    else
    {
        this->settings = new QSettings(settingsFileName, QSettings::IniFormat);
        return readSettings();
    }


}



/*!
 * \brief loadDEM
 * \param fileName the name of the file
 * \param raster a Crit3DRasterGrid pointer
 * \return true if file is ok, false otherwise
 */
bool Project::loadDEM(QString myFileName)
{
    std::string* myError = new std::string();
    std::string fileName = myFileName.left(myFileName.length()-4).toStdString();

    if (! gis::readEsriGrid(fileName, &DTM, myError))
    {
        qDebug("Load raster failed!");
        return (false);
    }

    setColorScale(noMeteoTerrain, DTM.colorScale);

    // initialize radition maps: slope, aspect, lat/lon
    if (radiationMaps != NULL)
        radiationMaps->clean();
    radiationMaps = new Crit3DRadiationMaps(DTM, gisSettings);

    //reset aggregationPoints meteoGrid
    if (meteoGridDbHandler != NULL)
    {
        meteoGridDbHandler->meteoGrid()->setIsAggregationDefined(false);
        // TODO
        // ciclo sulle celle della meteo grid -> clean vettore aggregation points
    }

    //set interpolation settings DEM
    interpolationSettings.setCurrentDEM(&DTM);
    qualityInterpolationSettings.setCurrentDEM(&DTM);

    return (true);
}

bool loadProxyGrid(Crit3DProxy* myProxy)
{
    std::string* myError = new std::string();
    std::string gridName = myProxy->getGridName();
    gis::Crit3DRasterGrid* myGrid = myProxy->getGrid();
    return (gis::readEsriGrid(gridName, myGrid, myError));
}

bool Project::checkProxySetting(QString group, std::string* name, std::string* grdName, std::string* table, std::string* field)
{
    std::string grid;

    *name = group.right(group.size()-6).toStdString();
    settings->beginGroup(group);
    grid = settings->value("raster").toString().toStdString();
    *grdName = path.toStdString() + grid;
    *table = settings->value("table").toString().toStdString();
    *field = settings->value("field").toString().toStdString();
    settings->endGroup();

    return (*name != "" && (grid != "" || (*table != "" && *field != "")));
}

bool Project::readProxies()
{
    std::string proxyName, proxyGridName, proxyTable, proxyField;
    int proxyNr = 0;
    bool isGridLoaded;

    interpolationSettings.initializeProxy();
    meteoPointsDbHandler->initializeProxy();

    Q_FOREACH (QString group, settings->childGroups())
    {
        //proxy variables (for interpolation)
        if (group.startsWith("proxy"))
        {
            isGridLoaded = false;

            if (! checkProxySetting(group, &proxyName, &proxyGridName, &proxyTable, &proxyField))
            {
                errorString = "error parsing proxy " + proxyName;
                return false;
            }

            Crit3DProxy myProxy;

            myProxy.setName(proxyName);
            myProxy.setGridName(proxyGridName);

            if (myProxy.getProxyPragaName() == height)
            {
                if (DTM.isLoaded)
                {
                    isGridLoaded = true;
                    myProxy.setGrid(&DTM);
                }
            }
            else
                isGridLoaded = loadProxyGrid(&myProxy);

            interpolationSettings.addProxy(myProxy);
            if (isGridLoaded && meteoPointsDbHandler != NULL)
            {
                meteoPointsDbHandler->addProxy(myProxy, proxyTable, proxyField);
                proxyNr++;
            }

            if ( !isGridLoaded && (proxyTable == "" || proxyField == ""))
                return false;
        }
    }

    return true;
}

bool Project::getMeteoPointSelected(int i)
{
    if (meteoPointsSelected.isEmpty()) return true;

    for (int j = 0; j < meteoPointsSelected.size(); j++)
    {
        if (meteoPoints[i].latitude == meteoPointsSelected[j].latitude && meteoPoints[i].longitude == meteoPointsSelected[j].longitude)
            return true;
    }

    return false;
}


bool Project::downloadDailyDataArkimet(QStringList variables, bool prec0024, QDate startDate, QDate endDate, bool showInfo)
{
    const int MAXDAYS = 30;

    QString id, dataset;
    QStringList datasetList;
    QList<QStringList> idList;

    QList<int> arkIdAirTemp;
    arkIdAirTemp << 231 << 232 << 233;
    int arkIdPrec = 250;
    QList<int> arkIdRH;
    arkIdRH << 240 << 241 << 242;
    int arkIdRadiation = 706;
    QList<int> arkIdWind;
    arkIdWind << 227 << 230;

    QList<int> arkIdVar;
    for( int i=0; i < variables.size(); i++ )
    {
        if (variables[i] == "Air Temperature")
            arkIdVar.append(arkIdAirTemp);
        if (variables[i] == "Precipitation")
            arkIdVar.append(arkIdPrec);
        if (variables[i] == "Air Humidity")
            arkIdVar.append(arkIdRH);
        if (variables[i] == "Radiation")
            arkIdVar.append(arkIdRadiation);
        if (variables[i] == "Wind")
            arkIdVar.append(arkIdWind);
    }

    Download* myDownload = new Download(meteoPointsDbHandler->getDbName());

    int index, nrPoints = 0;
    for( int i=0; i < nrMeteoPoints; i++ )
    {
        if (getMeteoPointSelected(i))
        {
            nrPoints ++;

            id = QString::fromStdString(meteoPoints[i].id);
            dataset = QString::fromStdString(meteoPoints[i].dataset);

            if (!datasetList.contains(dataset))
            {
                datasetList << dataset;
                QStringList myList;
                myList << id;
                idList.append(myList);
            }
            else
            {
                index = datasetList.indexOf(dataset);
                idList[index].append(id);
            }
        }
    }

    formRunInfo myInfo;
    QString infoStr;

    int nrDays = startDate.daysTo(endDate) + 1;
    if (showInfo) myInfo.start(infoStr, nrPoints*nrDays);

    int currentPoints = 0.;
    for( int i=0; i < datasetList.size(); i++ )
    {
        QDate date1 = startDate;
        QDate date2 = std::min(date1.addDays(MAXDAYS-1), endDate);

        while (date1 <= endDate)
        {
            if (showInfo)
            {
                myInfo.setText("Download data from: " + date1.toString("yyyy-MM-dd") + " to: " + date2.toString("yyyy-MM-dd") + " dataset:" + datasetList[i]);
                currentPoints += idList[i].size() * (date1.daysTo(date2) + 1);
                myInfo.setValue(currentPoints);
            }

            myDownload->downloadDailyData(date1, date2, datasetList[i], idList[i], arkIdVar, prec0024);

            date1 = date2.addDays(1);
            date2 = std::min(date1.addDays(MAXDAYS-1), endDate);
        }
    }

    if (showInfo) myInfo.close();
    return true;
}


bool Project::downloadHourlyDataArkimet(QStringList variables, QDate startDate, QDate endDate, bool showInfo)
{
    const int MAXDAYS = 7;

    QList<int> arkIdAirTemp;
    arkIdAirTemp << 78 << 158;
    QList<int> arkIdPrec;
    arkIdPrec << 160;
    QList<int> arkIdRH;
    arkIdRH << 139 << 140;
    QList<int> arkIdRadiation;
    arkIdRadiation << 164 << 409;
    QList<int> arkIdWind;
    arkIdWind << 69 << 165 << 166 << 431;

    QList<int> arkIdVar;
    for( int i=0; i < variables.size(); i++ )
    {
        if (variables[i] == "Air Temperature")
            arkIdVar.append(arkIdAirTemp);
        if (variables[i] == "Precipitation")
            arkIdVar.append(arkIdPrec);
        if (variables[i] == "Air Humidity")
            arkIdVar.append(arkIdRH);
        if (variables[i] == "Radiation")
            arkIdVar.append(arkIdRadiation);
        if (variables[i] == "Wind")
            arkIdVar.append(arkIdWind);
    }

    int index, nrPoints = 0;
    QString id, dataset;
    QStringList datasetList;
    QList<QStringList> idList;

    for( int i=0; i < nrMeteoPoints; i++ )
    {
        if (getMeteoPointSelected(i))
        {
            nrPoints ++;

            id = QString::fromStdString(meteoPoints[i].id);
            dataset = QString::fromStdString(meteoPoints[i].dataset);

            if (!datasetList.contains(dataset))
            {
                datasetList << dataset;
                QStringList myList;
                myList << id;
                idList.append(myList);
            }
            else
            {
                index = datasetList.indexOf(dataset);
                idList[index].append(id);
            }
        }
    }

    Download* myDownload = new Download(meteoPointsDbHandler->getDbName());

    formRunInfo myInfo;
    QString infoStr;

    int nrDays = startDate.daysTo(endDate) + 1;
    if (showInfo) myInfo.start(infoStr, nrPoints*nrDays);

    int currentPoints = 0.;
    for( int i=0; i < datasetList.size(); i++ )
    {
        QDate date1 = startDate;
        QDate date2 = std::min(date1.addDays(MAXDAYS-1), endDate);

        while (date1 <= endDate)
        {
            if (showInfo)
            {
                myInfo.setText("Download data from: " + date1.toString("yyyy-MM-dd") + " to:" + date2.toString("yyyy-MM-dd") + " dataset:" + datasetList[i]);
                currentPoints += idList[i].size() * (date1.daysTo(date2) + 1);
                myInfo.setValue(currentPoints);
            }

            myDownload->downloadHourlyData(date1, date2, datasetList[i], idList[i], arkIdVar);

            date1 = date2.addDays(1);
            date2 = std::min(date1.addDays(MAXDAYS-1), endDate);
        }
    }

    if (showInfo) myInfo.close();
    return true;
}


void Project::setCurrentDate(QDate myDate)
{
    if (myDate != this->currentDate)
    {
        this->previousDate = this->currentDate;
        this->currentDate = myDate;
    }
}

void Project::setCurrentHour(int myHour)
{
    this->currentHour = myHour;
}

void Project::setFrequency(frequencyType myFrequency)
{
    this->currentFrequency = myFrequency;
}

QDate Project::getCurrentDate()
{
    return this->currentDate;
}

Crit3DTime Project::getCurrentTime()
{
    return getCrit3DTime(this->currentDate, this->currentHour);
}

int Project::getCurrentHour()
{
    return this->currentHour;
}

frequencyType Project::getFrequency()
{
    return this->currentFrequency;
}

meteoVariable Project::getCurrentVariable()
{
    return this->currentVariable;
}


bool Project::loadlastMeteoData()
{
    QDate lastDateD = meteoPointsDbHandler->getLastDay(daily).date();
    QDate lastDateH = meteoPointsDbHandler->getLastDay(hourly).date();

    /*
     *
    */
/*
    QDate firstDateD = meteoPointsDbHandler->getFirstDay(daily).date();
    QDate firstDateH = meteoPointsDbHandler->getFirstDay(hourly).date();


    int numberOfDaysD = firstDateD.daysTo(lastDateD) + 1;
    int numberOfDaysH = firstDateH.daysTo(lastDateH) + 1;
    int myHourlyFraction = 1;

    for (int i=0; i < nrMeteoPoints; i++)
    {
        meteoPoints[i].initializeObsDataD(numberOfDaysD, getCrit3DDate(firstDateD));
        meteoPoints[i].initializeObsDataH(myHourlyFraction, numberOfDaysH, getCrit3DDate(firstDateH));
    }
*/
    /*
     *
    */

    QDate lastDate = (lastDateD > lastDateH) ? lastDateD : lastDateH;

    this->setCurrentDate(lastDate);
    this->setCurrentHour(12);

    return this->loadMeteoPointsData (lastDate, lastDate, true);
}


bool Project::updateMeteoPointsData()
{
    return loadMeteoPointsData(currentDate, currentDate, true);
}


bool Project::loadMeteoPointsData(QDate firstDate, QDate lastDate, bool showInfo)
{
    //check
    if (firstDate == QDate(1800,1,1) || lastDate == QDate(1800,1,1)) return false;

    bool isData = false;
    formRunInfo myInfo;
    int step;

    QString infoStr = "Load data: " + firstDate.toString();

    if (firstDate != lastDate)
        infoStr += " - " + lastDate.toString();

    if (showInfo)
        step = myInfo.start(infoStr, nrMeteoPoints);

    for (int i=0; i < nrMeteoPoints; i++)
    {

        if (showInfo)
            if ((i % step) == 0) myInfo.setValue(i);

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

        if (v != NODATA && meteoPoints[i].quality == quality::accepted)
        {
            if (*minimum == NODATA)
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

    if (meteoPointsDbHandler != NULL)
    {
        delete meteoPointsDbHandler;
    }

    if (meteoPoints != NULL)
    {
        delete [] meteoPoints;
    }

    meteoPointsDbHandler = NULL;
    meteoPoints = NULL;

    meteoPointsSelected.clear();
    nrMeteoPoints = 0;
    setIsElabMeteoPointsValue(false);
}

void Project::closeMeteoGridDB()
{

    if (meteoGridDbHandler != NULL)
    {
        delete meteoGridDbHandler;
    }

    meteoGridDbHandler = NULL;

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

    QList<Crit3DMeteoPoint> listMeteoPoints = meteoPointsDbHandler->getPropertiesFromDb(this->gisSettings);

    nrMeteoPoints = listMeteoPoints.size();
    if (nrMeteoPoints == 0)
    {
        logError("Missing meteo point properties.");
        closeMeteoPointsDB();
        return false;
    }

    meteoPoints = new Crit3DMeteoPoint[nrMeteoPoints];

    for (int i=0; i < nrMeteoPoints; i++)
    {
        meteoPoints[i] = listMeteoPoints[i];
    }

    listMeteoPoints.clear();

    //interpolation proxies
    if (! readProxies())
    {
        logError("Error loading interpolation proxies");
        return false;
    }
    readProxyValues();

    return true;
}


bool Project::loadMeteoGridDB(QString xmlName)
{

    if (xmlName == "")
        return false;

    this->meteoGridDbHandler = new Crit3DMeteoGridDbHandler();
    this->meteoGridDbHandler->meteoGrid()->setGisSettings(this->gisSettings);


    if (! this->meteoGridDbHandler->parseXMLGrid(xmlName, &errorString))
        return false;

    if (! this->meteoGridDbHandler->openDatabase(&errorString))
        return false;

    if (! this->meteoGridDbHandler->loadCellProperties(&errorString))
        return false;

    if (! this->meteoGridDbHandler->updateGridDate(&errorString))
        return false;




// test
//    this->meteoGridDbHandler->saveCellCurrrentGridDaily(&errorString, "lauraD", QDate(1985,01,01), 231, 30);
//    this->meteoGridDbHandler->saveCellCurrrentGridDaily(&errorString, "lauraD", QDate(1985,01,01), 231, NODATA);
//    this->meteoGridDbHandler->saveCellCurrrentGridDaily(&errorString, "lauraD", QDate(1985,01,02), 231, 40);
//    this->meteoGridDbHandler->saveCellCurrentGridDailyFF(&errorString, "lauraD", QDate(1985,01,01), "DAILY_TAVG", 30);
//    this->meteoGridDbHandler->saveCellCurrentGridDailyFF(&errorString, "lauraD", QDate(1985,01,01), "DAILY_W_INT_AVG", 10);
//    this->meteoGridDbHandler->saveCellCurrentGridDailyFF(&errorString, "lauraD", QDate(1985,01,01), "DAILY_W_INT_AVG", 20);
//    this->meteoGridDbHandler->saveCellCurrentGridDailyFF(&errorString, "lauraD", QDate(1985,01,02), "DAILY_TAVG", NODATA);
//    this->meteoGridDbHandler->saveCellCurrentGridDailyFF(&errorString, "lauraD", QDate(1985,01,03), "DAILY_TAVG", 40);
//    this->meteoGridDbHandler->saveCellCurrentGridHourlyFF(&errorString, "lauraH", QDateTime(QDate(1991,01,01),QTime(9,0,0)), "TAVG", 30);
//    this->meteoGridDbHandler->saveCellCurrentGridHourlyFF(&errorString, "lauraH", QDateTime(QDate(1991,01,01),QTime(9,0,0)), "TAVG", NODATA);
//    this->meteoGridDbHandler->saveCellCurrentGridHourlyFF(&errorString, "lauraH", QDateTime(QDate(1991,01,01),QTime(10,0,0)), "TAVG", 20);

//    this->meteoGridDbHandler->saveCellCurrentGridHourly(&errorString, "lauraH", QDateTime(QDate(1991,01,01),QTime(9,0,0)), 78, 30);
//    this->meteoGridDbHandler->saveCellCurrentGridHourly(&errorString, "lauraH", QDateTime(QDate(1991,01,01),QTime(9,0,0)), 78, NODATA);
//    this->meteoGridDbHandler->saveCellCurrentGridHourly(&errorString, "lauraH", QDateTime(QDate(1991,01,01),QTime(10,0,0)), 78, 20);

//    QDateTime firstDateDB;
//    std::vector<float> hourlyVarList = this->meteoGridDbHandler->loadGridHourlyVar(&errorString, "01019", precipitation, QDateTime(QDate(1991,01,01),QTime(9,0,0)), QDateTime(QDate(1991,01,2),QTime(9,0,0)), &firstDateDB);
//    if (hourlyVarList.size() == 0)
//        return false;

    /*
    QDate lastDate = this->meteoGridDbHandler->lastDate();
    if (! this->loadMeteoGridDailyData(lastDate, lastDate) )
        return false;
*/

    //test
//    QDate firstDateDB;
//    std::vector<float> dailyVarList = this->meteoGridDbHandler->loadGridDailyVar(&errorString, "01010", dailyPrecipitation, QDate(1991,01,01), QDate(1991,01,10), &firstDateDB);
//    std::vector<float> dailyVarList = this->meteoGridDbHandler->loadGridDailyVarFixedFields(&errorString, "01011", dailyAirTemperatureMin, QDate(2018,04,15), QDate(2018,04,17), &firstDateDB);
//    if (dailyVarList.size() == 0)
//        return false;

//    if (! this->meteoGridDbHandler->loadGridHourlyData(&errorString, "01019", QDateTime(QDate(1991,01,01),QTime(9,0,0)), QDateTime(QDate(1991,01,2),QTime(9,0,0))))
//        return false;
//    QDateTime firstDateDB;
//    std::vector<float> hourlyVarList = this->meteoGridDbHandler->loadGridHourlyVarFixedFields(&errorString, "01019", airTemperature, QDateTime(QDate(2018,04,15),QTime(20,0,0)), QDateTime(QDate(2018,04,15),QTime(23,0,0)), &firstDateDB);
//        if (hourlyVarList.size() == 0)
//            return false;

//    loadMeteoGridData(QDate(2018,04,29), QDate(2018,04,30), 1);
//    QList<meteoVariable> meteoVariableList = { dailyAirTemperatureMin , dailyAirTemperatureMax , dailyAirTemperatureAvg , dailyAirRelHumidityMax };
//    this->meteoGridDbHandler->saveCellGridDailyData(&errorString, "write00097", 23, 2, QDate(2018,04,29), QDate(2018,04,30), meteoVariableList);


//    loadMeteoGridData(QDate(2018,04,17), QDate(2018,04,30), 1);
//    QList<meteoVariable> meteoVariableList = { airTemperature , precipitation , airHumidity , globalIrradiance };
//    this->meteoGridDbHandler->saveCellGridHourlyData(&errorString, "Hwrite00097", 23, 2, QDateTime(QDate(2018,04,29),QTime(9,0,0)), QDateTime(QDate(2018,04,30),QTime(9,0,0)), meteoVariableList);
//    this->meteoGridDbHandler->saveCellGridDailyDataFF(&errorString, "DW00097", 23, 2, QDate(2018,04,29), QDate(2018,04,30));
//    this->meteoGridDbHandler->saveCellGridHourlyDataFF(&errorString, "HW00097", 23, 2, QDateTime(QDate(2018,04,17),QTime(20,0,0)), QDateTime(QDate(2018,04,18),QTime(9,0,0)));
    return true;
}


void Project::loadMeteoGridData(QDate firstDate, QDate lastDate, bool showInfo)
{
    if (this->meteoGridDbHandler != NULL)
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

    formRunInfo myInfo;
    int infoStep;

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
    formRunInfo myInfo;
    int infoStep;

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


bool Project::readProxyValues()
{
    if (nrMeteoPoints == 0)
        return false;

    for (int i = 0; i < nrMeteoPoints; i++)
        meteoPointsDbHandler->readPointProxyValues(&meteoPoints[i]);

    return true;
}

bool Project::interpolationRaster(meteoVariable myVar, const Crit3DTime& myTime, gis::Crit3DRasterGrid *myRaster)
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
    if (! preInterpolation(interpolationPoints, &interpolationSettings, meteoPoints, nrMeteoPoints, myVar, myTime))
    {
        errorString = "Interpolation: error in function preInterpolation";
        return false;
    }

    // Interpolate
    if (! interpolationDem(interpolationPoints, &interpolationSettings, myRaster, DTM, myVar))
    {
       errorString = "Interpolation: error in function interpolateGridDtm";
        return false;
    }

    Crit3DTime t = myTime;
    myRaster->timeString = t.toStdString();
    return true;
}

bool Project::interpolateRasterRadiation(const Crit3DTime& myTime, gis::Crit3DRasterGrid *myRaster)
{
    radSettings.setGisSettings(&gisSettings);

    gis::Crit3DPoint mapCenter = DTM.mapCenter();

    int intervalWidth = radiation::estimateTransmissivityWindow(&radSettings, DTM, *radiationMaps, &mapCenter, myTime, int(HOUR_SECONDS));

    // almost a meteoPoint with transmissivity data
    if (! computeTransmissivity(&radSettings, meteoPoints, nrMeteoPoints, intervalWidth, myTime, DTM))
        if (! computeTransmissivityFromTRange(meteoPoints, nrMeteoPoints, myTime))
        {
            errorString = "Function interpolateRasterRadiation: it is not possible to compute transmissivity.";
            return false;
        }

    if (checkAndPassDataToInterpolation(quality, atmTransmissivity, meteoPoints, nrMeteoPoints,
                                        myTime, &qualityInterpolationSettings,
                                        &interpolationSettings, interpolationPointList, checkSpatialQuality))
    {
        errorString = "Function interpolateRasterRadiation: not enough transmissivity data.";
        return false;
    }

    if (preInterpolation(interpolationPointList, &interpolationSettings, meteoPoints, nrMeteoPoints, atmTransmissivity, myTime))
        if (! interpolationDem(interpolationPointList, &interpolationSettings, this->radiationMaps->transmissivityMap, this->DTM, atmTransmissivity), &interpolationSettings)
        {
            errorString = "Function interpolateRasterRadiation: error interpolating transmissivity.";
            return false;
        }

    if (! radiation::computeRadiationGridPresentTime(&radSettings, this->DTM, this->radiationMaps, myTime))
    {
        errorString = "Function interpolateRasterRadiation: error computing solar radiation";
        return false;
    }

    myRaster->copyGrid(*(this->radiationMaps->globalRadiationMap));

    return true;
}

bool Project::interpolationRasterMain(meteoVariable myVar, const Crit3DTime& myTime, gis::Crit3DRasterGrid *myRaster)
{
    if (myVar == noMeteoVar)
    {
        errorString = "No variable selected";
        return false;
    }

    if (! DTM.isLoaded)
    {
        errorString = "No DEM loaded";
        return false;
    }

    if (nrMeteoPoints == 0)
    {
        errorString = "No points available";
        return false;
    }

    myRaster->initializeGrid(this->DTM);

    if (myVar == globalIrradiance)
    {
        Crit3DTime measureTime = myTime.addSeconds(-1800);
        return interpolateRasterRadiation(measureTime, myRaster);
    }
    else
    {
        return interpolationRaster(myVar, myTime, myRaster);
    }
}


bool Project::interpolationMeteoGrid(meteoVariable myVar, frequencyType myFrequency, const Crit3DTime& myTime,
                              gis::Crit3DRasterGrid *myRaster)
{

    if (meteoGridDbHandler != NULL)
    {
        if (!interpolationRaster(myVar, myTime, myRaster))
        {
            return false;
        }
        meteoGridDbHandler->meteoGrid()->aggregateMeteoGrid(myVar, myFrequency, myTime.date, myTime.getHour(), myTime.getMinutes(), &(this->DTM), this->dataRaster, this->grdAggrMethod);
        meteoGridDbHandler->meteoGrid()->fillMeteoRaster();
    }
    else
    {
        errorString = "No grid";
        return false;
    }
    return true;
}

bool Project::saveGrid(meteoVariable myVar, frequencyType myFrequency, const Crit3DTime& myTime, bool showInfo)
{
    std::string id;
    formRunInfo myInfo;
    int infoStep;


    if (myFrequency == daily)
    {
        if (showInfo)
        {
            QString infoStr = "Save grid daily data";
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
                        this->meteoGridDbHandler->saveCellCurrrentGridDaily(&errorString, QString::fromStdString(id), QDate(myTime.date.year, myTime.date.month, myTime.date.day), this->meteoGridDbHandler->getDailyVarCode(myVar), this->meteoGridDbHandler->meteoGrid()->meteoPoint(row,col).currentValue);
                    }
                    else
                    {
                        this->meteoGridDbHandler->saveCellCurrentGridDailyFF(&errorString, QString::fromStdString(id), QDate(myTime.date.year, myTime.date.month, myTime.date.day), QString::fromStdString(this->meteoGridDbHandler->getDailyPragaName(myVar)), this->meteoGridDbHandler->meteoGrid()->meteoPoint(row,col).currentValue);
                    }
                }
            }
        }
    }
    else if (myFrequency == hourly)
    {
        if (showInfo)
        {
            QString infoStr = "Save grid hourly data";
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
                        this->meteoGridDbHandler->saveCellCurrentGridHourly(&errorString, QString::fromStdString(id), QDateTime(QDate(myTime.date.year, myTime.date.month, myTime.date.day), QTime(myTime.getHour(), myTime.getMinutes(), myTime.getSeconds())), this->meteoGridDbHandler->getHourlyVarCode(myVar), this->meteoGridDbHandler->meteoGrid()->meteoPoint(row,col).currentValue);
                    }
                    else
                    {
                        this->meteoGridDbHandler->saveCellCurrentGridHourlyFF(&errorString, QString::fromStdString(id), QDateTime(QDate(myTime.date.year, myTime.date.month, myTime.date.day), QTime(myTime.getHour(), myTime.getMinutes(), myTime.getSeconds())), QString::fromStdString(this->meteoGridDbHandler->getHourlyPragaName(myVar)), this->meteoGridDbHandler->meteoGrid()->meteoPoint(row,col).currentValue);
                    }

                }
            }
        }

    }

    if (showInfo) myInfo.close();

    return true;
}


float Project::meteoDataConsistency(meteoVariable myVar, const Crit3DTime& timeIni, const Crit3DTime& timeFin)
{
    float dataConsistency = 0.0;
    for (int i = 0; i < nrMeteoPoints; i++)
        dataConsistency = maxValue(dataConsistency, meteoPoints[i].obsDataConsistencyH(myVar, timeIni, timeFin));

    return dataConsistency;
}


bool Project::elaborationCheck(bool isMeteoGrid, bool isAnomaly)
{

    if (isMeteoGrid)
    {
        if (this->meteoGridDbHandler == NULL)
        {
            errorString = "Load grid";
            return false;
        }
        else
        {
            if (this->clima == NULL)
            {
                this->clima = new Crit3DClimate();
            }
        }
    }
    else
    {
        if (this->meteoPointsDbHandler == NULL)
        {
            errorString = "Load meteo Points";
            return false;
        }
        else
        {
            if (this->clima == NULL)
            {
                this->clima = new Crit3DClimate();
            }
        }
    }
    if (isAnomaly)
    {
        if (this->referenceClima == NULL)
        {
            this->referenceClima = new Crit3DClimate();
        }
    }

    return true;
}


bool Project::elaboration(bool isMeteoGrid, bool isAnomaly)
{

    if (isMeteoGrid)
    {
        if (!isAnomaly)
        {
            if (!elaborationPointsCycleGrid(isAnomaly, true))
            {
                return false;
            }
            meteoGridDbHandler->meteoGrid()->fillMeteoRasterElabValue();
        }
        else
        {
            if (!elaborationPointsCycleGrid(isAnomaly, true))
            {
                return false;
            }
            meteoGridDbHandler->meteoGrid()->fillMeteoRasterAnomalyValue();
        }
        meteoGridDbHandler->meteoGrid()->setIsElabValue(true);
    }
    else
    {
        if (!isAnomaly)
        {
            if (!elaborationPointsCycle(isAnomaly, true))
            {
                return false;
            }
        }
        else
        {
            if (!elaborationPointsCycle(isAnomaly, true))
            {
                return false;
            }
        }

        setIsElabMeteoPointsValue(true);
    }

    return true;
}


bool Project::elaborationPointsCycle(bool isAnomaly, bool showInfo)
{

    bool isMeteoGrid = 0; // meteoPoint
    float currentParameter1;
    int validCell = 0;
    int myClimateIndex;

    formRunInfo myInfo;
    int infoStep;
    QString infoStr;

    if (showInfo)
    {
        if (isAnomaly)
        {
            infoStr = "Anomaly - Meteo Points";
        }
        else
        {
            infoStr = "Elaboration - Meteo Points";
        }

        infoStep = myInfo.start(infoStr, nrMeteoPoints);
    }


    QDate startDate(clima->yearStart(), clima->genericPeriodDateStart().month(), clima->genericPeriodDateStart().day());
    QDate endDate(clima->yearEnd(), clima->genericPeriodDateEnd().month(), clima->genericPeriodDateEnd().day());

    if (clima->nYears() > 0)
    {
        endDate.setDate(clima->yearEnd() + clima->nYears(), clima->genericPeriodDateEnd().month(), clima->genericPeriodDateEnd().day());
    }
    else if (clima->nYears() < 0)
    {
        startDate.setDate(clima->yearStart() + clima->nYears(), clima->genericPeriodDateStart().month(), clima->genericPeriodDateStart().day());
    }

    if (clima->param1IsClimate())
    {
        parserElaboration(clima);
        myClimateIndex = getClimateIndexFromDate(currentDate, clima->periodType());
     }


    for (int i = 0; i < nrMeteoPoints; i++)
    {
        if (showInfo && (i % infoStep) == 0)
                    myInfo.setValue(i);

        if (clima->param1IsClimate())
        {
//            if ( ClimateReadPoint(PragaClimate.Point(i).TableName, climateElab, myPeriodType, myClimateIndex, PragaClimate.Point(i)))
//            {

//                currentParameter1 = passaggioDati.GetClimateData(myPeriodType, PragaClimate.Point(i), myClimateIndex);
//            }
//            else
//            {
//                currentParameter1 = NODATA;
//            }
             clima->setParam1(currentParameter1);
        }


        if (clima->elab1() == "phenology")
        {
            //Then currentPheno.setPhenoPoint i;  // TODO
        }

        if ( elaborationOnPoint(&errorString, meteoPointsDbHandler, NULL, &meteoPoints[i], clima, isMeteoGrid, startDate, endDate, isAnomaly, true))
        {
            validCell = validCell + 1;
        }


    }
    if (showInfo) myInfo.close();

    if (validCell == 0)
    {
        errorString = "no valid cells available";
        return false;
    }
    else
    {
        return true;
    }

}


bool Project::elaborationPointsCycleGrid(bool isAnomaly, bool showInfo)
{

    bool isMeteoGrid = 1; // grid
    float currentParameter1;
    int validCell = 0;
    int myClimateIndex;

    std::string id;

    formRunInfo myInfo;
    int infoStep;
    QString infoStr;

    if (showInfo)
    {
        if (isAnomaly)
        {
            infoStr = "Anomaly - Meteo Grid";
        }
        else
        {
            infoStr = "Elaboration - Meteo Grid";
        }

        infoStep = myInfo.start(infoStr, this->meteoGridDbHandler->gridStructure().header().nrRows);
    }

    QDate startDate(clima->yearStart(), clima->genericPeriodDateStart().month(), clima->genericPeriodDateStart().day());
    QDate endDate(clima->yearEnd(), clima->genericPeriodDateEnd().month(), clima->genericPeriodDateEnd().day());

    if (clima->nYears() > 0)
    {
        endDate.setDate(clima->yearEnd() + clima->nYears(), clima->genericPeriodDateEnd().month(), clima->genericPeriodDateEnd().day());
    }
    else if (clima->nYears() < 0)
    {
        startDate.setDate(clima->yearStart() + clima->nYears(), clima->genericPeriodDateStart().month(), clima->genericPeriodDateStart().day());
    }


    if (clima->param1IsClimate())
    {

        parserElaboration(clima);
        myClimateIndex = getClimateIndexFromDate(currentDate, clima->periodType());

     }


     for (int row = 0; row < meteoGridDbHandler->gridStructure().header().nrRows; row++)
     {
         if (showInfo && (row % infoStep) == 0)
             myInfo.setValue(row);

         for (int col = 0; col < meteoGridDbHandler->gridStructure().header().nrCols; col++)
         {

            if (meteoGridDbHandler->meteoGrid()->getMeteoPointActiveId(row, col, &id))
            {

                if (clima->param1IsClimate())
                {
//                    if (Climate.ClimateReadPoint(PragaClimate.Point(row, col).TableName, _
//                        climateElab, myPeriodType, myClimateIndex,PragaClimate.Point(row, col)) )
//                    {

//                      currentParameter1 = passaggioDati.GetClimateData(myPeriodType, PragaClimate.Point(row, col), myClimateIndex)
//                    }
//                    else
//                    {
//                        currentParameter1 = NODATA;
//                    }
                    clima->setParam1(currentParameter1);
                }


                //Crit3DMeteoPoint meteoPoint = meteoGridDbHandler->meteoGrid()->meteoPoint(row,col);
                Crit3DMeteoPoint* meteoPoint = meteoGridDbHandler->meteoGrid()->meteoPointPointer(row,col);
                if  ( elaborationOnPoint(&errorString, NULL, meteoGridDbHandler, meteoPoint, clima, isMeteoGrid, startDate, endDate, isAnomaly, true))
                {
                    validCell = validCell + 1;
                }

            }

        }
    }

    if (showInfo) myInfo.close();

    if (validCell == 0)
    {
        errorString = "no valid cells available";
        return false;
    }
    else
    {
        return true;
    }

}

/*-------------------

   LOG functions

 -------------------*/

bool Project::setLogFile()
{
    if (!QDir(this->path + "log").exists())
         QDir().mkdir(this->path + "log");

    QString myDate = QDateTime().currentDateTime().toString("yyyy-MM-dd hh.mm");
    QString fileName = "Praga_" + myDate + ".txt";

    this->logFileName = this->path + "log\\" + fileName;

    this->logFile.open(this->logFileName.toStdString().c_str());
    return (this->logFile.is_open());
}

void Project::logError(QString myStr)
{
    errorString = myStr.toStdString();
    logError();
}

void Project::logError()
{
    if (logFile.is_open())
        logFile << "----ERROR!----\n" << this->errorString << std::endl;
    else
        QMessageBox::critical(NULL, "Error!", QString::fromStdString(errorString));
}

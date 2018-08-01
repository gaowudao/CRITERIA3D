#include <QLabel>
#include <QtDebug>
#include <QMessageBox>

#include "project.h"
#include "formRunInfo.h"
#include "utilities.h"
#include "commonConstants.h"
#include "quality.h"
#include "radiationSettings.h"
#include "solarRadiation.h"
#include "interpolation.h"
#include "transmissivity.h"
#include "climate.h"
#include "dbTools.h"

#include <iostream> //debug

Project::Project()
{
    path = "";
    logFileName = "";
    errorString = "";

    meteoPoints = NULL;
    nrMeteoPoints = 0;
    quality = new Crit3DQuality();
    currentVariable = noMeteoVar;
    currentFrequency = noFrequency;
    currentDate.setDate(1800,1,1);
    previousDate = currentDate;
    currentHour = 12;
    meteoPointsColorScale = new Crit3DColorScale();
    meteoPointsDbHandler = NULL;
    meteoGridDbHandler = NULL;
    clima = NULL;
    referenceClima = NULL;
    grdAggrMethod = gridAggregationMethod::aggrAvg;

    radiationMaps = NULL;

    // default: Bologna
    startLocation.latitude = 44.5;
    startLocation.longitude = 11.35;
}

bool Project::readSettings()
{
    Q_FOREACH (QString group, settings->childGroups())
    {
        //proxy variables (for interpolation)
        QString proxyName;
        std::string proxyGridName;
        std::string proxyField;
        int proxyPos = 0;
        if (group.startsWith("proxy"))
        {
            proxyName = group.right(group.size()-6);
            settings->beginGroup(group);
            proxyGridName = settings->value("raster").toString().toStdString();
            proxyField = settings->value("field").toString().toStdString();
            settings->endGroup();

            this->myInterpolationSettings.addProxy(proxyName.toStdString(), proxyGridName);
            Crit3DProxyInterpolation proxyToAdd = myInterpolationSettings.getProxy(proxyPos);
            this->meteoPointsDbHandler->addProxy(&proxyToAdd, proxyField);
            proxyPos++;
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
 * \brief loadRaster
 * \param fileName the name of the file
 * \param raster a Crit3DRasterGrid pointer
 * \return true if file is ok, false otherwise
 */
bool Project::loadRaster(QString myFileName)
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

    return (true);
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

        if (meteoPointsDbHandler->getDailyData(getCrit3DDate(firstDate), getCrit3DDate(lastDate), &(meteoPoints[i])))
        {
            isData = true;
        }
        if (meteoPointsDbHandler->getHourlyData(getCrit3DDate(firstDate), getCrit3DDate(lastDate), &(meteoPoints[i])))
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

        if (v != NODATA && meteoPoints[i].myQuality == quality::accepted)
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
    QList<Crit3DMeteoPoint> listMeteoPoints = meteoPointsDbHandler->getPropertiesFromDb();

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

        //check data
        if ((meteoPoints[i].latitude == NODATA || meteoPoints[i].longitude == NODATA)
            && (meteoPoints[i].point.utm.x != NODATA && meteoPoints[i].point.utm.y != NODATA))
        {
            gis::getLatLonFromUtm(this->gisSettings, meteoPoints[i].point.utm.x, meteoPoints[i].point.utm.y,
                                    &meteoPoints[i].latitude, &meteoPoints[i].longitude);
        }
    }

    listMeteoPoints.clear();

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
        QString infoStr = "Load grid hourly data: " + firstDate.toString() + " - " + lastDate.toString();
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


bool Project::interpolateRaster(meteoVariable myVar, frequencyType myFrequency, const Crit3DTime& myTime,
                            gis::Crit3DRasterGrid *myRaster)
{    
    // check quality and pass data to interpolation
    if (!quality->checkAndPassDataToInterpolation(myVar, myFrequency, this->meteoPoints, this->nrMeteoPoints, myTime, &this->myInterpolationSettings))
    {
        errorString = "No data";
        return false;
    }

    if (! checkInterpolationRaster(this->DTM, &errorString))
        return false;

    myRaster->initializeGrid(this->DTM);

    if (myVar == globalIrradiance)
    {
        Crit3DTime measureTime = myTime.addSeconds(-1800);
        return interpolateRasterRadiation(measureTime, myRaster, &errorString);
    }
    else
    {
        return interpolationRaster(myVar, &myInterpolationSettings, myTime, this->DTM, myRaster, &errorString);
    }
}


bool Project::interpolateGrid(meteoVariable myVar, frequencyType myFrequency, const Crit3DTime& myTime)
{

    if (this->meteoGridDbHandler != NULL)
    {
        if (!interpolateRaster(myVar, myFrequency, myTime, &(this->dataRaster)))
        {
            return false;
        }
        this->meteoGridDbHandler->meteoGrid()->aggregateMeteoGrid(myVar, myFrequency, myTime.date, myTime.getHour(), myTime.getMinutes(), &(this->DTM), this->dataRaster, this->grdAggrMethod);
        this->meteoGridDbHandler->meteoGrid()->fillMeteoRaster();
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


bool Project::interpolateRasterRadiation(const Crit3DTime& myTime, gis::Crit3DRasterGrid *myRaster, std::string *myError)
{
    Crit3DRadiationSettings radSettings;

    radSettings.setGisSettings(&(gisSettings));

    radiation::setRadiationSettings(&(radSettings));

    gis::Crit3DPoint mapCenter = DTM.mapCenter();

    int intervalWidth = radiation::estimateTransmissivityWindow(DTM, *radiationMaps, &mapCenter, myTime, int(HOUR_SECONDS));

    // almost a meteoPoint with transmissivity data
    if (! computeTransmissivity(this->meteoPoints, this->nrMeteoPoints, intervalWidth, myTime, this->DTM))
        if (! computeTransmissivityFromTRange(this->meteoPoints, this->nrMeteoPoints, myTime))
        {
            *myError = "Function interpolateRasterRadiation: it is not possible to compute transmissivity.";
            return false;
        }

    if (!quality->checkAndPassDataToInterpolation(atmTransmissivity, hourly, this->meteoPoints, this->nrMeteoPoints, myTime, &this->myInterpolationSettings))
    {
        *myError = "Function interpolateRasterRadiation: not enough transmissivity data.";
        return false;
    }

    if (preInterpolation(atmTransmissivity))
        if (! interpolateGridDtm(this->radiationMaps->transmissivityMap, this->DTM, atmTransmissivity))
        {
            *myError = "Function interpolateRasterRadiation: error interpolating transmissivity.";
            return false;
        }

    if (! radiation::computeRadiationGridPresentTime(this->DTM, this->radiationMaps, myTime))
    {
        *myError = "Function interpolateRasterRadiation: error computing solar radiation";
        return false;
    }

    myRaster->copyGrid(*(this->radiationMaps->globalRadiationMap));

    return true;
}


float Project::meteoDataConsistency(meteoVariable myVar, const Crit3DTime& timeIni, const Crit3DTime& timeFin)
{
    float dataConsistency = 0.0;
    for (int i = 0; i < nrMeteoPoints; i++)
        dataConsistency = maxValue(dataConsistency, meteoPoints[i].obsDataConsistencyH(myVar, timeIni, timeFin));

    return dataConsistency;
}


bool Project::loadModelParameters(QString dbName)
{
    QSqlDatabase dbParameters;
    std::string myError;

    Criteria3Dproject.isParametersLoaded = false;

    dbParameters = QSqlDatabase::addDatabase("QSQLITE", QUuid::createUuid().toString());
    dbParameters.setDatabaseName(dbName);

    if (!dbParameters.open())
    {
       logError("Connection with database fail");
       return false;
    }

    // TODO Load crop parameters

    Criteria3Dproject.isParametersLoaded = true;
    return true;
}


bool Project::loadSoilData(QString dbName)
{
    Criteria3Dproject.soilList.clear();

    std::string myError;
    QSqlDatabase dbSoil;

    dbSoil = QSqlDatabase::addDatabase("QSQLITE", QUuid::createUuid().toString());
    dbSoil.setDatabaseName(dbName);

    if (!dbSoil.open())
    {
       logError("Connection with database fail");
       return false;
    }

    if (! loadVanGenuchtenParameters(Criteria3Dproject.soilClass, &dbSoil, &myError))
    {
        logError(QString::fromStdString(myError));
        return false;
    }

    // query soil list
    QString queryString = "SELECT id_soil, soil_code FROM soils";
    QSqlQuery query = dbSoil.exec(queryString);

    query.first();
    if (! query.isValid())
    {
        if (query.lastError().number() > 0)
            logError(query.lastError().text());
        else
            logError("Error in reading table soils");
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
            if (loadSoil(&dbSoil, soilCode, mySoil, Criteria3Dproject.soilClass, &myError))
            {
                mySoil->id = idSoil;
                Criteria3Dproject.soilList.push_back(*mySoil);
            }
            else
            {
                wrongSoils += " " + soilCode;
            }
        }
    } while(query.next());

    if (Criteria3Dproject.soilList.size() == 0)
    {
       logError("Missing soil properties.");
       return false;
    }
    else if (wrongSoils != "")
    {
        logError("Wrong soils:" + wrongSoils);
    }

    return true;
}


bool Project::loadSoilMap(QString myFileName)
{
    std::string myError;
    std::string fileName = myFileName.left(myFileName.length()-4).toStdString();

    if (! gis::readEsriGrid(fileName, &(Criteria3Dproject.soilMap), &myError))
    {
        logError("Load soil map failed!");
        return (false);
    }

    return (true);
}


bool Project::initializeCriteria3D()
{
    if (!Criteria3Dproject.initializeProject(&DTM, radiationMaps))
    {
        logError(QString::fromStdString(Criteria3Dproject.error));
        return false;
    }

    return true;
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
            if (isAnomaly)
            {
                if (this->referenceClima == NULL)
                {
                    errorString = "Load clima";
                    return false;
                }
            }
            else
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
            if (isAnomaly)
            {
                if (this->referenceClima == NULL)
                {
                    errorString = "Load clima";
                    return false;
                }
            }
            else
            {
                this->clima = new Crit3DClimate();
            }
        }
    }

    return true;
}


bool Project::elaboration(bool isMeteoGrid, bool isAnomaly)
{

    if (isMeteoGrid)
    {
        if (!elaborationPointsCycleGrid(&errorString, this->meteoGridDbHandler, this->referenceClima, this->clima, this->currentDate, isAnomaly))
        {
            return false;
        }
    }
    else
    {
        if (!elaborationPointsCycle(&errorString, this->meteoPointsDbHandler, this->meteoPoints, this->nrMeteoPoints, this->referenceClima, this->clima, this->currentDate, isAnomaly))
        {
            return false;
        }
    }
    return true;
}




//-------------------
//
//   LOG functions
//
//-------------------

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

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
    colorScalePoints = new Crit3DColorScale();
    meteoPointsDbHandler = NULL;
    meteoGridDbHandler = NULL;

    radiationMaps = new Crit3DRadiationMaps();
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

    gis::updateMinMaxRasterGrid(&DTM);
    this->DTM.isLoaded = true;

    // initialize slope, aspect, lat/lon
    radiationMaps->clean();
    radiationMaps = new Crit3DRadiationMaps(DTM, gisSettings);

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
    const int MAXDAYS = 1;

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

        if (meteoPointsDbHandler->getDailyData(getCrit3DDate(firstDate), getCrit3DDate(lastDate), &(meteoPoints[i]))) isData = true;
        if (meteoPointsDbHandler->getHourlyData(getCrit3DDate(firstDate), getCrit3DDate(lastDate), &(meteoPoints[i]))) isData = true;
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
        meteoPointsDbHandler->closeDatabase();
        delete [] meteoPoints;
    }

    meteoPointsSelected.clear();
    nrMeteoPoints = 0;
}


bool Project::loadMeteoPointsDB(QString dbName)
{
    if (dbName == "") return false;

    closeMeteoPointsDB();

    meteoPointsDbHandler = new Crit3DMeteoPointsDbHandler(dbName);
    QList<Crit3DMeteoPoint> listMeteoPoints = meteoPointsDbHandler->getPropertiesFromDb();

    nrMeteoPoints = listMeteoPoints.size();
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
    this->meteoGridDbHandler->meteoGrid()->setUtmZone(this->gisSettings.utmZone);
    this->meteoGridDbHandler->meteoGrid()->setIsNorthernEmisphere(this->gisSettings.isNorthernEmisphere);


    if (! this->meteoGridDbHandler->parseXMLGrid(xmlName, &errorString))
        return false;

    if (! this->meteoGridDbHandler->openDatabase(&errorString))
        return false;

    if (! this->meteoGridDbHandler->loadCellProperties(&errorString))
        return false;

    if (! this->meteoGridDbHandler->updateGridDate(&errorString))
        return false;

    this->meteoGridDbHandler->meteoGrid()->createRasterGrid();

    return true;
}


bool Project::interpolateRaster(meteoVariable myVar, frequencyType myFrequency, const Crit3DTime& myTime,
                            gis::Crit3DRasterGrid *myRaster)
{
    if (!quality->checkData(myVar, myFrequency, this->meteoPoints, this->nrMeteoPoints, myTime))
    {
        errorString = "No data";
        return false;
    }

    if (! checkInterpolationRaster(this->DTM, &errorString))
        return false;

    myRaster->initializeGrid(this->DTM);

    // TO DO - gestione dei settings
    Crit3DInterpolationSettings interpolationSettings;

    if (myVar == globalIrradiance)
    {
        Crit3DTime measureTime = myTime.addSeconds(-1800);
        return interpolateRasterRadiation(measureTime, myRaster, &errorString);
    }
    else
    {
        return interpolationRaster(myVar, &interpolationSettings, myTime, this->DTM, myRaster, &errorString);
    }
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

    if (!quality->checkData(atmTransmissivity, hourly, this->meteoPoints, this->nrMeteoPoints, myTime))
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

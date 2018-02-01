#include <QLabel>
#include <QtDebug>
#include <QMessageBox>

#include "project.h"
#include "forminfo.h"
#include "utilities.h"
#include "commonConstants.h"
#include "quality.h"
#include "radiationSettings.h"
#include "solarRadiation.h"
#include "interpolation.h"
#include "transmissivity.h"


Project::Project()
{
    meteoPoints = NULL;
    nrMeteoPoints = 0;
    quality = new Crit3DQuality();
    currentVariable = noMeteoVar;
    currentFrequency = noFrequency;
    currentDate.setDate(1800,1,1);
    previousDate = currentDate;
    currentHour = 12;
    colorScalePoints = new Crit3DColorScale();
    dbMeteoPoints = NULL;
    currentRaster = &DTM;

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
    this->currentRaster = &DTM;

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

    Download* myDownload = new Download(dbMeteoPoints->getDbName());

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

    formInfo myInfo;
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
                myInfo.setText("Load data from: " + date1.toString("yyyy-MM-dd") + " to: " + date2.toString("yyyy-MM-dd") + " dataset:" + datasetList[i]);
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

    Download* myDownload = new Download(dbMeteoPoints->getDbName());

    formInfo myInfo;
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
                myInfo.setText("Load data from: " + date1.toString("yyyy-MM-dd") + " to:" + date2.toString("yyyy-MM-dd") + " dataset:" + datasetList[i]);
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

void Project::setCurrentHour(short myHour)
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

short Project::getCurrentHour()
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
    QDate lastDateD = dbMeteoPoints->getLastDay(daily).date();
    QDate lastDateH = dbMeteoPoints->getLastDay(hourly).date();

    QDate lastDate = (lastDateD > lastDateH) ? lastDateD : lastDateH;

    setCurrentDate(lastDate);
    setCurrentHour(12);

    return loadMeteoPointsData (lastDate, lastDate, true);
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
    formInfo myInfo;
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

        if (dbMeteoPoints->getDailyData(getCrit3DDate(firstDate), getCrit3DDate(lastDate), &(meteoPoints[i]))) isData = true;
        if (dbMeteoPoints->getHourlyData(getCrit3DDate(firstDate), getCrit3DDate(lastDate), &(meteoPoints[i]))) isData = true;
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
        v = meteoPoints[i].value;

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
    if (dbMeteoPoints != NULL)
    {
        dbMeteoPoints->closeDatabase();
        delete [] meteoPoints;
    }

    meteoPointsSelected.clear();
    nrMeteoPoints = 0;
}


bool Project::loadMeteoPointsDB(QString dbName)
{
    if (dbName == "") return false;

    closeMeteoPointsDB();

    dbMeteoPoints = new DbMeteoPoints(dbName);
    QList<Crit3DMeteoPoint> listMeteoPoints = dbMeteoPoints->getPropertiesFromDb();

    nrMeteoPoints = listMeteoPoints.size();
    meteoPoints = new Crit3DMeteoPoint[nrMeteoPoints];
    for (int i=0; i < nrMeteoPoints; i++)
        meteoPoints[i] = listMeteoPoints[i];

    listMeteoPoints.clear();

    return true;
}


bool Project::interpolation(meteoVariable myVar, frequencyType myFrequency, const Crit3DTime& myTime, std::string *myError)
{

    if (!quality->checkData(myVar, myFrequency, this->meteoPoints, this->nrMeteoPoints, myTime))
    {
        *myError = "No data";
        return false;
    }

    if (! checkInterpolationRaster(this->DTM, myError))
        return false;

    this->dataRaster.initializeGrid(this->DTM);

    // TO DO - gestione dei settings
    Crit3DInterpolationSettings interpolationSettings;

    if (myVar == globalIrradiance)
    {
        if (! interpolateRadiation(myTime, myError))
            return false;
    }
    else
    {
        if (! interpolationRaster(myVar, &interpolationSettings, &(this->dataRaster), this->DTM, myTime, myError))
            return false;
    }

    return true;
}


bool Project::interpolateRadiation(const Crit3DTime& myTime, std::string *myError)
{
    Crit3DRadiationSettings radSettings;

    radSettings.setGisSettings(&(gisSettings));

    radiation::setRadiationSettings(&(radSettings));

    int intervalWidth = radiation::estimateTransmissivityWindow(DTM, *radiationMaps, &(DTM.mapCenter()), myTime, int(HOUR_SECONDS));
    float dt = (intervalWidth-1) * 0.5 * HOUR_SECONDS;

    Crit3DTime timeIni = myTime.addSeconds(-dt);
    Crit3DTime timeFin = myTime.addSeconds(dt);

    // almost a meteo point with 50% of data
    if (! (meteoDataConsistency(globalIrradiance, timeIni, timeFin) > 0.5))
    {
        *myError = "Radiation data not available.";
        return false;
    }

    // almost a meteo point with transmissivity
    if(! (computeTransmissivity(meteoPoints, nrMeteoPoints, intervalWidth, myTime, DTM) > 0))
    {
        *myError = "It is not possible to compute transmissivity.";
        return false;
    }

    return true;
}

    /*
        QDate transmissivityDate = getQDate(myCrit3DTime.date);
        QDate yesterday = transmissivityDate.addDays(-1);
        if ((transmissivityDate == myProject->lastDateTransmissivity)
            || ((yesterday == myProject->lastDateTransmissivity) && (myCrit3DTime.getHour() == 0))) //midnight
            transComputed = true;
        else
        {
            Crit3DTime timeIniTemp = myCrit3DTime;
            Crit3DTime timeFinTemp = myCrit3DTime;
            timeIniTemp.time = myTimeStep;
            timeFinTemp.time = DAY_SECONDS;
            bool tempLoaded = false;

            tempLoaded = (myProject->meteoDataConsistency(airTemperature, timeIniTemp, timeFinTemp) > 0.5);
            if (! tempLoaded && isLoadData) tempLoaded = (myProject->loadObsDataAllPointsVar(airTemperature, transmissivityDate, transmissivityDate));
            if (tempLoaded && isLoadData) tempLoaded = (myProject->meteoDataConsistency(airTemperature, timeIniTemp, timeFinTemp) > 0.5);
            if (tempLoaded)
                if (computeTransmissivityFromTRange(myProject->meteoPoints, myProject->nrMeteoPoints, myCrit3DTime))
                {
                    transComputed= true;
                    myProject->lastDateTransmissivity = transmissivityDate;
                }
        }
    }

    if (! transComputed)
    {
        myProject->projectError = "Function computeRadiationProjectDtm: transmissivity data unavailable";
        return false;
    }

    if (! myProject->qualityParameters.checkData(atmTransmissivity, hourly, myProject->meteoPoints, myProject->nrMeteoPoints, myCrit3DTime))
    {
        myProject->projectError = "Function computeRadiationProjectDtm: no transmissivity data available";
        return false;
    }

    if (preInterpolation(atmTransmissivity))
        if (! interpolateGridDtm(myProject->meteoMaps->radiationMaps->transmissivityMap, myProject->dtm, atmTransmissivity))
        {
            myProject->projectError = "Function computeRadiationProjectDtm: error interpolating transmissivity";
            return false;
        }

    if (radiation::computeRadiationGridPresentTime(myProject->dtm, myProject->meteoMaps->radiationMaps, myCrit3DTime))
        myResult = setRadiationScale(myProject->meteoMaps->radiationMaps->globalRadiationMap->colorScale);
    else
        myProject->projectError = "Function computeRadiationProjectDtm: error computing irradiance";

    return myResult;
}
*/



float Project::meteoDataConsistency(meteoVariable myVar, const Crit3DTime& timeIni, const Crit3DTime& timeFin)
{
    float dataConsistency = 0.0;
    for (int i = 0; i < nrMeteoPoints; i++)
        dataConsistency = maxValue(dataConsistency, meteoPoints[i].obsDataConsistencyH(myVar, timeIni, timeFin));

    return dataConsistency;
}


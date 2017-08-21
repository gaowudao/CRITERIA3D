#include <QLabel>
#include <QtDebug>
#include <QMessageBox>

#include "project.h"
#include "forminfo.h"
#include "utilities.h"
#include "commonConstants.h"
#include "quality.h"

#include <netcdf.h>


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
}



int provaNetcdf()
{
    int ncid, retValue;
    QString fileName = "simple_xy.nc";

    /* Open the file. NC_NOWRITE tells netCDF we want read-only access
    * to the file.*/
   if ((retValue = nc_open(fileName.toStdString().data(), NC_NOWRITE, &ncid)))
      qDebug() << nc_strerror(retValue);

   /* Close the file, freeing all resources. */
   if ((retValue = nc_close(ncid)))
      qDebug() << nc_strerror(retValue);

   return 0;
}


/*!
 * \brief loadRaster
 * \param fileName the name of the file
 * \param raster a Crit3DRasterGrid pointer
 * \return true if everything is ok, false otherwise
 */
bool Project::loadRaster(QString myFileName)
{
    std::string* myError = new std::string();
    std::string fileName = myFileName.left(myFileName.length()-4).toStdString();

    if (gis::readEsriGrid(fileName, &(this->DTM), myError))
    {
        gis::updateMinMaxRasterGrid(&(this->DTM));
        this->DTM.isLoaded = true;
        this->currentRaster = &DTM;

        this->dataRaster.initializeGrid(this->DTM);

        gis::Crit3DGridHeader myLatLonHeader;
        gis::getGeoExtentsFromUTMHeader(this->gisSettings, this->DTM.header, &myLatLonHeader);

        this->colMatrix.initializeGrid(myLatLonHeader);
        this->rowMatrix.initializeGrid(myLatLonHeader);

        double lat, lon, x, y;
        long utmRow, utmCol;
        for (long row = 0; row < myLatLonHeader.nrRows; row++)
            for (long col = 0; col < myLatLonHeader.nrCols; col++)
            {
                gis::getLatLonFromRowCol(myLatLonHeader, row, col, &lat, &lon);
                gis::latLonToUtmForceZone(this->gisSettings.utmZone, lat, lon, &x, &y);
                gis::getRowColFromXY(this->DTM, x, y, &utmRow, &utmCol);
                if (this->DTM.getValueFromRowCol(utmRow, utmCol) != this->DTM.header->flag)
                {
                    this->rowMatrix.value[row][col] = utmRow;
                    this->colMatrix.value[row][col] = utmCol;
                }
            }

        qDebug("Raster Ok.");
        return (true);
    }
    else
    {
        qDebug("Load raster failed!");
        return (false);
    }
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
    arkIdPrec << 159 << 160;
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








#include "project.h"
#include "forminfo.h"
#include <QLabel>
#include <QtDebug>
#include <QMessageBox>


Project::Project()
{
    currentVariable = dailyAirTemperatureAvg;
    currentFrequency = daily;
    currentDate.setDate(1800,1,1);
    previousDate = currentDate;
    currentHour = 12;
    colorScalePoints = new gis::Crit3DColorScale();
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
        this->DTM.isLoaded = true;
        gis::updateMinMaxRasterGrid(&(this->DTM));
        // colorscale
        gis::setDefaultDTMScale(this->DTM.colorScale);

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


bool Project::downloadDailyDataArkimet(QStringList variables, bool prec24, Crit3DDate dateStart, Crit3DDate dateEnd)
{
    QStringList id, dataset;

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

    bool skip = false;
    for( int i=0; i < meteoPoints.size(); i++ )
    {
        if (!meteoPointsSelected.isEmpty())
        {
            skip = true;
            for (int j = 0; j < meteoPointsSelected.size(); j++)
            {
                if (meteoPoints[i].latitude == meteoPointsSelected[j].latitude && meteoPoints[i].longitude == meteoPointsSelected[j].longitude)
                    skip = false;
            }
        }

        if (!skip)
        {
            /*id = QString::fromStdString(meteoPoints[i].id);
            dataset = QString::fromStdString(meteoPoints[i].dataset);
            name = QString::fromStdString(meteoPoints[i].name);

            info.setText(id + "\n" + name);

            if (! myDownload->downloadDailyDataSinglePoint(dateStart, dateEnd, dataset, id, arkIdVar, prec24))
                return false;
            */
            id << QString::fromStdString(meteoPoints[i].id);
            if (!dataset.contains(QString::fromStdString(meteoPoints[i].dataset)))
                dataset << QString::fromStdString(meteoPoints[i].dataset);
        }
    }

    if (! myDownload->downloadDailyVar(dateStart, dateEnd, dataset, id, arkIdVar, prec24)) return false;

    return true;
}



bool Project::downloadHourlyDataArkimet(QStringList variables, Crit3DDate dateStart, Crit3DDate dateEnd)
{
    Crit3DTime dateTimeStart(dateStart, 0);
    Crit3DTime dateTimeEnd(dateEnd, 0);
    QStringList datasets;
    QStringList id;
    bool skip = 0;

    QList<int> arkIdVar;

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

    for( int i=0; i < meteoPoints.size(); i++ )
    {
        if (!meteoPointsSelected.isEmpty())
        {
            skip = 1;
            for (int j = 0; j < meteoPointsSelected.size(); j++)
            {
                if (meteoPoints[i].latitude == meteoPointsSelected[j].latitude && meteoPoints[i].longitude == meteoPointsSelected[j].longitude)
                    skip = 0;
            }
        }
        if (!skip)
        {
            if (!datasets.contains(QString::fromStdString(meteoPoints[i].dataset)))
                datasets << QString::fromStdString(meteoPoints[i].dataset);
            id << QString::fromStdString(meteoPoints[i].id);
        }
    }

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
    qDebug() << "dateTimeStart" << QString::fromStdString(dateTimeStart.toStdString());
    qDebug() << "dateTimeEnd" << QString::fromStdString(dateTimeEnd.toStdString());
    qDebug() << "datasets" << datasets;
    qDebug() << "id" << id;
    qDebug() << "arkIdVar" << arkIdVar;

    Download* myDownload = new Download(dbMeteoPoints->getDbName());
    return myDownload->downloadHourlyVar(dateTimeStart, dateTimeEnd, datasets, id, arkIdVar);
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

short Project::getCurrentHour()
{
    return this->currentHour;
}

frequencyType Project::getFrequency()
{
    return this->currentFrequency;
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


bool Project::loadMeteoPointsData(QDate firstDate, QDate lastDate, bool showInfo)
{
    //check
    if (firstDate == QDate(1800,1,1)
        || lastDate == QDate(1800,1,1)) return false;

    bool isData = false;
    formInfo myInfo;
    int step;

    QString infoStr = "Load data: " + firstDate.toString();

    if (firstDate != lastDate)
        infoStr += " - " + lastDate.toString();

    if (showInfo)
        step = myInfo.start(infoStr, meteoPoints.size());

    for (int i=0; i < meteoPoints.size(); i++)
    {
        if (showInfo)
            if ((i % step) == 0) myInfo.setValue(i);

        if (dbMeteoPoints->getDailyData(getCrit3DDate(firstDate), getCrit3DDate(lastDate), &(meteoPoints[i]))) isData = true;
        if (dbMeteoPoints->getHourlyData(getCrit3DDate(firstDate), getCrit3DDate(lastDate), &(meteoPoints[i]))) isData = true;
    }

    if (showInfo) myInfo.close();

    return isData;
}





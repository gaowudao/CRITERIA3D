#include "project.h"
#include <QtDebug>


Project::Project()
{

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

    gis::Crit3DRasterGrid UTMRaster;
    if (gis::readEsriGrid(fileName, &(UTMRaster), myError))
    {
        gis::Crit3DGridHeader myLatLonHeader;
        gis::getGeoExtentsFromUTMHeader(this->gisSettings, UTMRaster.header, &myLatLonHeader);

        this->DTM.initializeGrid(myLatLonHeader);

        // transform
        double lat, lon, x, y;
        for (long row = 0; row < this->DTM.header->nrRows; row++)
            for (long col = 0; col < this->DTM.header->nrCols; col++)
            {
                gis::getUtmXYFromRowCol(this->DTM, row, col, &(lon), &(lat));
                gis::latLonToUtmForceZone(this->gisSettings.utmZone, lat, lon, &x, &y);
                this->DTM.value[row][col] = gis::getValueFromXY(UTMRaster, x, y);
            }

        this->DTM.isLoaded = true;
        gis::updateMinMaxRasterGrid(&(this->DTM));

        // colorscale
        gis::setDefaultDTMScale(this->DTM.colorScale);
        qDebug("Raster Ok.");
        return (true);
    }
    else
    {
        qDebug("Load raster failed!");
        return (false);
    }
}


void Project::downloadArkimetDailyVar(QStringList variables, bool precSelection)
{
    Crit3DDate dateStart(startDate.day(), startDate.month(), startDate.year());
    Crit3DDate dateEnd(endDate.day(), endDate.month(), endDate.year());
    QStringList datasets;
    QStringList id;

    QList<int> arkIdVar;

    QList<int> arkIdAirTemp;
    arkIdAirTemp << 231 << 232 << 233;
    int arkIdPrec = 250;
    QList<int> arkIdRH;
    arkIdRH << 240 << 241 << 242;
    int arkIdRadiation = 706;
    QList<int> arkIdWind;
    arkIdWind << 227 << 230;

    for( int i=0; i < meteoPoints.size(); i++ )
    {
        if (!datasets.contains(QString::fromStdString(meteoPoints[i].dataset)))
            datasets << QString::fromStdString(meteoPoints[i].dataset);
        id << QString::fromStdString(meteoPoints[i].id);
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

    pointProperties->downloadDailyVar(dateStart, dateEnd, datasets, id, arkIdVar, precSelection);

}

void Project::downloadArkimetHourlyVar(QStringList variables)
{

    Crit3DDate dateStart(startDate.day(), startDate.month(), startDate.year());
    Crit3DDate dateEnd(endDate.day(), endDate.month(), endDate.year());

    Crit3DTime dateTimeStart(dateStart, 0);
    Crit3DTime dateTimeEnd(dateEnd, 0);
    QStringList datasets;
    QStringList id;

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
        if (!datasets.contains(QString::fromStdString(meteoPoints[i].dataset)))
            datasets << QString::fromStdString(meteoPoints[i].dataset);
        id << QString::fromStdString(meteoPoints[i].id);
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

    pointProperties->downloadHourlyVar(dateTimeStart, dateTimeEnd, datasets, id, arkIdVar);

}

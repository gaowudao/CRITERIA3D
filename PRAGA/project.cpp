#include "project.h"
#include <QtDebug>


Project::Project()
{
    nrMeteoPoints = 0;
    //meteoPoints = NULL;
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

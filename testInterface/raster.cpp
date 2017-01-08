#include <QDebug>
#include "raster.h"


gis::Crit3DRasterGrid *DTM;


/*!
 * \brief loadRaster
 * \param fileName the name of the file
 * \param raster a Crit3DRasterGrid pointer
 * \return true if everything is ok, false otherwise
 */
bool loadRaster(QString myFileName, gis::Crit3DRasterGrid *outputRaster)
{
    std::string* myError = new std::string();
    std::string fileName = myFileName.left(myFileName.length()-4).toStdString();

    gis::Crit3DRasterGrid utmRaster;
    if (gis::readEsriGrid(fileName, &(utmRaster), myError))
    {
        gis::Crit3DGisSettings mySettings;
        gis::Crit3DGridHeader myLatLonHeader;
        gis::getGeoExtentsFromUTMHeader(mySettings, utmRaster.header, &myLatLonHeader);

        outputRaster->initializeGrid(myLatLonHeader);

        // transform
        double lat, lon, x, y;
        for (long row = 0; row < outputRaster->header->nrRows; row++)
            for (long col = 0; col < outputRaster->header->nrCols; col++)
            {
                gis::getUtmXYFromRowCol(*(outputRaster), row, col, &(lon), &(lat));
                gis::latLonToUtmForceZone(mySettings.utmZone, lat, lon, &x, &y);
                outputRaster->value[row][col] = gis::getValueFromXY(utmRaster, x, y);
            }

        outputRaster->isLoaded = true;
        gis::updateMinMaxRasterGrid(outputRaster);

        // colorscale
        gis::setDefaultDTMScale(outputRaster->colorScale);
        qDebug() << "Raster Ok.";
        return (true);
    }
    else
    {
        qDebug() << "Load raster failed!";
        return (false);
    }
}

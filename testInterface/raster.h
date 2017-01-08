#ifndef RASTER_H
#define RASTER_H

#include "gis.h"
#include <QString>

/*!
 * \brief loadRaster
 * \param fileName the name of the file
 * \param raster a Crit3DRasterGrid pointer
 * \return true if everything is ok, false otherwise
 */
bool loadRaster(QString myFileName, gis::Crit3DRasterGrid *outputRaster);

#endif // RASTER_H

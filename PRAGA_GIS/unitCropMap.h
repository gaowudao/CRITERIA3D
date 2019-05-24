#ifndef UNITCROPMAP_H
#define UNITCROPMAP_H

#include "shapeHandler.h"
#include "gis.h"

enum opType{MAJORITY, MIN, MAX, AVG};

bool zonalStatistics(Crit3DShapeHandler* shapeRef, Crit3DShapeHandler* shapeVal, std::string valField, DBFFieldType fieldType, int cellSize, opType type);
gis::Crit3DRasterGrid shapeToRaster(Crit3DShapeHandler shape, int cellSize);
void fillRasterWithShapeNumber(gis::Crit3DRasterGrid* raster, Crit3DShapeHandler shape);

#endif // UNITCROPMAP_H

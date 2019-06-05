#ifndef ZONALSTATISTIC_H
#define ZONALSTATISTIC_H

    #include "shapeHandler.h"
    #include "gis.h"

    enum opType{MAJORITY, MIN, MAX, AVG};

    bool zonalStatisticsShape(Crit3DShapeHandler* shapeRef, Crit3DShapeHandler* shapeVal, gis::Crit3DRasterGrid* rasterRef, gis::Crit3DRasterGrid* rasterVal, std::string valField, int cellSize, opType type, std::string *error);

#endif // ZONALSTATISTIC_H

#ifndef ZONALSTATISTIC_H
#define ZONALSTATISTIC_H

    #include "shapeHandler.h"
    #include "gis.h"

    enum opType{MAJORITY, MIN, MAX, AVG};

    bool zonalStatisticsShape(Crit3DShapeHandler* shapeRef, Crit3DShapeHandler* shapeVal, std::string valField, int cellSize, opType type, std::string *error);

#endif // ZONALSTATISTIC_H

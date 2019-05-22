#ifndef UNITCROPMAP_H
#define UNITCROPMAP_H

#include "shapeHandler.h"

enum opType{MAJORITY, MIN, MAX, AVG};

bool zonalStatistics(Crit3DShapeHandler* shapeRef, Crit3DShapeHandler* shapeVal, std::string valField, DBFFieldType fieldType, int cellSize, opType type);

#endif // UNITCROPMAP_H

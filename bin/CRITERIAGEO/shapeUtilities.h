#ifndef SHAPEUTILITIES_H
#define SHAPEUTILITIES_H

#ifndef SHAPEHANDLER_H
    #include "shapeHandler.h"
#endif

#include <QString>

QString cloneShapeFile(std::string completeShapePath, QString fileName);
bool deleteRecords(Crit3DShapeHandler *shapeHandler, QString newFile);

#endif // SHAPEUTILITIES_H

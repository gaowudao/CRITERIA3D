#ifndef EXTRACTUCMLIST_H
#define EXTRACTUCMLIST_H

#ifndef SHAPEHANDLER_H
    #include "shapeHandler.h"
#endif
#include <QString>

bool extractUCMListToDb(Crit3DShapeHandler* shapeHandler, QString dbName, std::string *error, bool showInfo);
bool createShapeFromCSV(Crit3DShapeHandler* shapeHandler, Crit3DShapeHandler* shapeFromCSV, QString fileCSV, std::string *error);

#endif // EXTRACTUCMLIST_H

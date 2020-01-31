#ifndef EXTRACTUCMLIST_H
#define EXTRACTUCMLIST_H

#ifndef SHAPEHANDLER_H
    #include "shapeHandler.h"
#endif
#include <QString>

bool extractUCMListToDb(Crit3DShapeHandler* shapeHandler, QString dbName, std::string *error, bool showInfo);

#endif // EXTRACTUCMLIST_H

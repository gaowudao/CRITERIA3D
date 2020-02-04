#ifndef EXTRACTUCMLIST_H
#define EXTRACTUCMLIST_H

#ifndef SHAPEHANDLER_H
    #include "shapeHandler.h"
#endif
#include <QString>
#include <QMap>

const QMap<QString, QString> MapCSVShapeFields = {
    { "CROP", "CROPTYPE" },
    { "readilyAvailableWater", "RAW" },
    { "rootDepth", "rootDepth" },
    { "forecast7daysPrec", "FcstPrec7d" },
    { "forecast7daysMaxTransp", "FcstETc7d" },
    { "forecast7daysIRR", "FcstIrr7d" },
    { "previousAllSeasonIRR", "PrevIrrTot" }
};

bool extractUCMListToDb(Crit3DShapeHandler* shapeHandler, QString dbName, std::string *error);
bool createShapeFromCSV(Crit3DShapeHandler* shapeHandler, Crit3DShapeHandler* shapeFromCSV, QString fileCSV, std::string *error);

#endif // EXTRACTUCMLIST_H

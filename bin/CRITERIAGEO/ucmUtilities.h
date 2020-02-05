#ifndef UCMUTILITIES_H
#define UCMUTILITIES_H

    #ifndef SHAPEHANDLER_H
        #include "shapeHandler.h"
    #endif
    #include <QString>
    #include <QMap>

    /*
    const QMap<QString, QString> MapCSVShapeFields = {
        { "CROP", "CROPTYPE" },
        { "readilyAvailableWater", "RAW" },
        { "rootDepth", "rootDepth" },
        { "forecast7daysPrec", "FcstPrec7d" },
        { "forecast7daysMaxTransp", "FcstETc7d" },
        { "forecast7daysIRR", "FcstIrr7d" },
        { "previousAllSeasonIRR", "PrevIrrTot" }
    };
    */

    bool UCMListToDb(Crit3DShapeHandler* shapeHandler, QString dbName, std::string *error, bool showInfo);
    bool shapeFromCSV(Crit3DShapeHandler* shapeHandler, Crit3DShapeHandler* outputShape, QString fileCSV, QString fileCSVRef, std::string *error);

#endif // UCMUTILITIES_H

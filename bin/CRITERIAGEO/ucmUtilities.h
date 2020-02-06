#ifndef UCMUTILITIES_H
#define UCMUTILITIES_H

    #ifndef SHAPEHANDLER_H
        #include "shapeHandler.h"
    #endif
    #include <QString>
    #include <QMap>

    bool UCMListToDb(Crit3DShapeHandler* shapeHandler, QString dbName, std::string *error, bool showInfo);
    bool shapeFromCSV(Crit3DShapeHandler* shapeHandler, Crit3DShapeHandler* outputShape, QString fileCSV, QString fileCSVRef, QString outputName, std::string *error);

#endif // UCMUTILITIES_H

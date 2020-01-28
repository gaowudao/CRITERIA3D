#ifndef UNITCROPMAP_H
#define UNITCROPMAP_H

    #include "shapeHandler.h"
    #include <QString>

    bool unitCropMap(Crit3DShapeHandler *ucm, Crit3DShapeHandler *crop, Crit3DShapeHandler *soil,
                 Crit3DShapeHandler *meteo, std::string idSoil, std::string idMeteo, double cellSize,
                 QString fileName, std::string *error, bool showInfo);

#endif // UNITCROPMAP_H

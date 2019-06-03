#ifndef UNITCROPMAP_H
#define UNITCROPMAP_H

    #include "shapeHandler.h"

    bool unitCropMap(Crit3DShapeHandler *ucm, Crit3DShapeHandler *crop, Crit3DShapeHandler *soil, Crit3DShapeHandler *meteo, std::string idSoil, std::string idMeteo, int cellSize, std::string *error);

#endif // UNITCROPMAP_H

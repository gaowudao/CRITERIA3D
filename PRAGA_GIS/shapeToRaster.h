#ifndef SHAPETORASTER_H
#define SHAPETORASTER_H

    #include "shapeHandler.h"
    #include "gis.h"

    gis::Crit3DRasterGrid initializeRasterFromShape(Crit3DShapeHandler *shape, int cellSize);
    void fillRasterWithShapeNumber(gis::Crit3DRasterGrid* raster, Crit3DShapeHandler* shape);
    void fillRasterWithField(gis::Crit3DRasterGrid* raster, Crit3DShapeHandler* shape, std::string valField);


#endif // SHAPETORASTER_H

#ifndef SCENE_H
#define SCENE_H

    #include <Qt3DCore/QEntity>
    #include "gis.h"

    Qt3DCore::QEntity *createScene(gis::Crit3DRasterGrid *dtm, gis::Crit3DRasterGrid *indexGrid);

#endif // SCENE_H

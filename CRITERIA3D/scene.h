#ifndef SCENE_H
#define SCENE_H

    #include <Qt3DCore/QEntity>
    #include "gis.h"
    #include "crit3dProject.h"


    Qt3DCore::QEntity *createScene(Crit3DProject *myProject, float magnify);

#endif // SCENE_H

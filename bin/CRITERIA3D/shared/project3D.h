#ifndef PROJECT3D_H
#define PROJECT3D_H

    #ifndef PROJECT_H
        #include "project.h"
    #endif
    #ifndef METEOMAPS_H
        #include "meteoMaps.h"
    #endif
    #ifndef SOIL3D_H
        #include "soil3D.h"
    #endif

    class Project3D : public Project
    {

    public:
        Crit3DMeteoMaps* meteoMaps;
        gis::Crit3DRasterGrid soilMap;
        Crit3DSoilFluxesSettings* wb3DSettings;

        Project3D();


    };


#endif // PROJECT3D_H

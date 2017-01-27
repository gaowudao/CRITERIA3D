#ifndef CRIT3DPROJECT_H
#define CRIT3DPROJECT_H

#ifndef GIS_H
    #include "gis.h"
#endif
#ifndef QSTRING_H
    #include <QString>
#endif

class Crit3DProject
{
public:
    Crit3DProject();

    gis::Crit3DRasterGrid DTM;
    gis::Crit3DRasterGrid rowMatrix;
    gis::Crit3DRasterGrid colMatrix;
    gis::Crit3DGisSettings gisSettings;

    bool loadRaster(QString myFileName);
};

#endif // CRIT3DPROJECT_H

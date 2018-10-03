#ifndef INTERPOLATIONCMD_H
#define INTERPOLATIONCMD_H

#ifndef GIS_H
    #include "gis.h"
#endif
#ifndef METEO_H
    #include "meteo.h"
#endif
#ifndef INTERPOLATIONSETTINGS_H
    #include "interpolationSettings.h"
#endif
#ifndef INTERPOLATIONPOINT_H
    #include "interpolationPoint.h"
#endif

bool loadProxyGrid(Crit3DProxy* myProxy);

bool interpolationRaster(std::vector <Crit3DInterpolationDataPoint> &myPoints, Crit3DInterpolationSettings* mySettings,
                        gis::Crit3DRasterGrid* myGrid, const gis::Crit3DRasterGrid& myDTM, meteoVariable myVar, bool showInfo);

#endif // INTERPOLATIONCMD_H

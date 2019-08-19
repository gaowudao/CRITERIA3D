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

class Crit3DProxyGridSeries
{
private:
    std::vector <std::string> gridName;
    std::vector <int> gridYear;
    std::vector<gis::Crit3DRasterGrid *> grid;

public:
    Crit3DProxyGridSeries();

    void initialize();
    void addGridToSeries(std::string name_, int year_);
    std::vector<gis::Crit3DRasterGrid *> getGrid() const;
    void setGrid(const std::vector<gis::Crit3DRasterGrid *> &value);
};

bool interpolationRaster(std::vector <Crit3DInterpolationDataPoint> &myPoints, Crit3DInterpolationSettings* mySettings,
                        gis::Crit3DRasterGrid* myGrid, const gis::Crit3DRasterGrid& raster, meteoVariable myVar, bool showInfo);

#endif // INTERPOLATIONCMD_H

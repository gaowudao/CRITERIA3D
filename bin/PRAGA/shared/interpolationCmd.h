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
#ifndef QSTRING_H
    #include <QString>
#endif

class Crit3DProxyGridSeries
{
private:
    std::vector <QString> gridName;
    std::vector <unsigned> gridYear;
    QString proxyName;

public:
    Crit3DProxyGridSeries();
    Crit3DProxyGridSeries(QString name_);

    void initialize();
    void addGridToSeries(QString name_, unsigned year_);
    std::vector<QString> getGridName() const;
    std::vector<unsigned> getGridYear() const;
    QString getProxyName() const;
};

bool interpolationRaster(std::vector <Crit3DInterpolationDataPoint> &myPoints, Crit3DInterpolationSettings* mySettings,
                        gis::Crit3DRasterGrid* myGrid, const gis::Crit3DRasterGrid& raster, meteoVariable myVar, bool showInfo);

#endif // INTERPOLATIONCMD_H

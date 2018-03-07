#ifndef METEOGRID_H
#define METEOGRID_H

#include <QString>
#include <QDate>
#ifndef CRIT3DDATE_H
    #include "crit3dDate.h"
#endif
#ifndef METEO_H
    #include "meteo.h"
#endif
#ifndef METEOPOINT_H
    #include "meteoPoint.h"
#endif
#ifndef GIS_H
    #include "gis.h"
#endif


class MeteoGrid
{
    public:

        gis::Crit3DRasterGrid gridStructure;
        Crit3DMeteoPoint* meteoPoints;

        bool isAggregationDefined;
        bool isDateTypeSpecified;
        QDate firstDate;
        QDate lastDate;
        int nrVarsArray;

        MeteoGrid();
};

#endif // METEOGRID_H


#ifndef METEOGRID_H
#define METEOGRID_H

#include <QString>
#include <QDate>


#ifndef METEOPOINT_H
    #include "meteoPoint.h"
#endif
#ifndef GRIDSTRUCTURE_H
    #include "gridStructure.h"
#endif



class MeteoGrid
{
    public:
        MeteoGrid();

    private:

        GridStructure gridStructure;
        Crit3DMeteoPoint* _meteoPoints;

        bool _isAggregationDefined;
        bool _isDateTypeSpecified;
        QDate _firstDate;
        QDate _lastDate;
        int _nrVarsArray;

};

#endif // METEOGRID_H


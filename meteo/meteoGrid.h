#ifndef METEOGRID_H
#define METEOGRID_H

    #ifndef METEOPOINT_H
        #include "meteoPoint.h"
    #endif

    class Crit3DMeteoGridStructure
    {
        public:
            Crit3DMeteoGridStructure();

        private:
            std::string _name;
            gis::Crit3DGridHeader header;
            int _dataType;

            bool _isRegular;
            bool _isTIN;
            bool _isUTM;
            bool _isLoaded;

            bool _isFixedFields;
            bool _isHourlyDataAvailable;
            bool _isDailyDataAvailable;

    };


    class Crit3DMeteoGrid
    {
    public:
        Crit3DMeteoGrid();

    private:

        Crit3DMeteoGridStructure gridStructure;
        Crit3DMeteoPoint* _meteoPoints;

        bool _isAggregationDefined;
        bool _isDateTypeSpecified;
        Crit3DDate _firstDate;
        Crit3DDate _lastDate;
        int _nrVarsArray;
    };


#endif // METEOGRID_H

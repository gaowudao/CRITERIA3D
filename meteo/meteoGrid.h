#ifndef METEOGRID_H
#define METEOGRID_H

#include <vector>

    #ifndef METEOPOINT_H
        #include "meteoPoint.h"
    #endif

    class Crit3DMeteoGridStructure
    {
        public:
            Crit3DMeteoGridStructure();

            std::string name() const;
            void setName(const std::string &name);

            gis::Crit3DGridHeader header() const;
            void setHeader(const gis::Crit3DGridHeader &header);

            int dataType() const;
            void setDataType(int dataType);

            bool isRegular() const;
            void setIsRegular(bool isRegular);

            bool isTIN() const;
            void setIsTIN(bool isTIN);

            bool isUTM() const;
            void setIsUTM(bool isUTM);

            bool isLoaded() const;
            void setIsLoaded(bool isLoaded);

            bool isFixedFields() const;
            void setIsFixedFields(bool isFixedFields);

            bool isHourlyDataAvailable() const;
            void setIsHourlyDataAvailable(bool isHourlyDataAvailable);

            bool isDailyDataAvailable() const;
            void setIsDailyDataAvailable(bool isDailyDataAvailable);

    private:
            std::string _name;
            gis::Crit3DGridHeader _header;

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
            gis::Crit3DRasterGrid dataMeteoGrid;

            Crit3DMeteoGrid();
            ~Crit3DMeteoGrid();

            Crit3DMeteoGridStructure gridStructure() const;
            void setGridStructure(const Crit3DMeteoGridStructure &gridStructure);

            std::vector<std::vector<Crit3DMeteoPoint *> > meteoPoints() const;
            void setMeteoPoints(const std::vector<std::vector<Crit3DMeteoPoint *> > &meteoPoints);

            int utmZone() const;
            void setUtmZone(int utmZone);

            bool isNorthernEmisphere() const;
            void setIsNorthernEmisphere(bool isNorthernEmisphere);

            void initMeteoPoints(int nRow, int nCol);

            void fillMeteoPoint(int row, int col, std::string code, std::string name, int height, bool active);

            bool findCellFromId(int* row, int* col, std::string code);

            bool findFirstActiveCell(std::string* id, int* row, int* col);

            bool isActiveCellFromId(std::string id);

            bool isAggregationDefined() const;
            void setIsAggregationDefined(bool isAggregationDefined);

            bool isDateTypeSpecified() const;
            void setIsDateTypeSpecified(bool isDateTypeSpecified);

            Crit3DDate firstDate() const;
            void setFirstDate(const Crit3DDate &firstDate);

            Crit3DDate lastDate() const;
            void setLastDate(const Crit3DDate &lastDate);

            int nrVarsArray() const;
            void setNrVarsArray(int nrVarsArray);

            bool createRasterGrid();

    private:

            Crit3DMeteoGridStructure _gridStructure;
            std::vector<std::vector<Crit3DMeteoPoint*> > _meteoPoints;
            int _utmZone;
            bool _isNorthernEmisphere;

            bool _isAggregationDefined;
            bool _isDateTypeSpecified;
            Crit3DDate _firstDate;
            Crit3DDate _lastDate;
            int _nrVarsArray;
    };


#endif // METEOGRID_H

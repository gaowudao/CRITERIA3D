#ifndef GRIDSTRUCTURE_H
#define GRIDSTRUCTURE_H

#include <QObject>

class GridHeader
{
    public:
        GridHeader();

    private:
        int _nrRows;
        int _nrCols;
        //latLonInfo As TLatLonInfo
        double _xllCorner;
        double _yllCorner;
        double _cellSize;
        double _flag;
};

class GridStructure
{
    public:
        GridStructure();

    private:
        QString _name;
        int _dataType;
        GridHeader header;

        float _xWidth;
        float _yWidth;
        bool _isRegular;
        bool _isTIN;
        bool _isUTM;
        bool _isLoaded;

        bool _isFixedFields;
        bool _isHourlyDataAvailable;
        bool _isDailyDataAvailable;

};

#endif // GRIDSTRUCTURE_H

#include "meteoGrid.h"
#include "commonConstants.h"
#include <iostream> //debug


Crit3DMeteoGridStructure::Crit3DMeteoGridStructure()
{

}

std::string Crit3DMeteoGridStructure::name() const
{
    return _name;
}

void Crit3DMeteoGridStructure::setName(const std::string &name)
{
    _name = name;
}

gis::Crit3DGridHeader Crit3DMeteoGridStructure::header() const
{
    return _header;
}

void Crit3DMeteoGridStructure::setHeader(const gis::Crit3DGridHeader &header)
{
    _header = header;
}

int Crit3DMeteoGridStructure::dataType() const
{
    return _dataType;
}

void Crit3DMeteoGridStructure::setDataType(int dataType)
{
    _dataType = dataType;
}

bool Crit3DMeteoGridStructure::isRegular() const
{
    return _isRegular;
}

void Crit3DMeteoGridStructure::setIsRegular(bool isRegular)
{
    _isRegular = isRegular;
}

bool Crit3DMeteoGridStructure::isTIN() const
{
    return _isTIN;
}

void Crit3DMeteoGridStructure::setIsTIN(bool isTIN)
{
    _isTIN = isTIN;
}

bool Crit3DMeteoGridStructure::isUTM() const
{
    return _isUTM;
}

void Crit3DMeteoGridStructure::setIsUTM(bool isUTM)
{
    _isUTM = isUTM;
}

bool Crit3DMeteoGridStructure::isLoaded() const
{
    return _isLoaded;
}

void Crit3DMeteoGridStructure::setIsLoaded(bool isLoaded)
{
    _isLoaded = isLoaded;
}

bool Crit3DMeteoGridStructure::isFixedFields() const
{
    return _isFixedFields;
}

void Crit3DMeteoGridStructure::setIsFixedFields(bool isFixedFields)
{
    _isFixedFields = isFixedFields;
}

bool Crit3DMeteoGridStructure::isHourlyDataAvailable() const
{
    return _isHourlyDataAvailable;
}

void Crit3DMeteoGridStructure::setIsHourlyDataAvailable(bool isHourlyDataAvailable)
{
    _isHourlyDataAvailable = isHourlyDataAvailable;
}

bool Crit3DMeteoGridStructure::isDailyDataAvailable() const
{
    return _isDailyDataAvailable;
}

void Crit3DMeteoGridStructure::setIsDailyDataAvailable(bool isDailyDataAvailable)
{
    _isDailyDataAvailable = isDailyDataAvailable;
}

Crit3DMeteoGridStructure Crit3DMeteoGrid::gridStructure() const
{
    return _gridStructure;
}

Crit3DMeteoGrid::Crit3DMeteoGrid()
{

}

Crit3DMeteoGrid::~Crit3DMeteoGrid()
{

    for (unsigned int i = 0; i < _meteoPoints.size(); i++)
    {
        std::vector<Crit3DMeteoPoint*> meteoPointVector = _meteoPoints[i];

        for (unsigned int j = 0; j < meteoPointVector.size(); j++)
        {
            free(meteoPointVector[j]);
        }

    }

}

bool Crit3DMeteoGrid::loadRasterGrid()
{

    dataMeteoGrid.header->cellSize = NODATA;
    dataMeteoGrid.header->llCorner->x = _gridStructure.header().llCorner->longitude;
    dataMeteoGrid.header->llCorner->y = _gridStructure.header().llCorner->latitude;

    dataMeteoGrid.header->nrCols = _gridStructure.header().nrCols;
    dataMeteoGrid.header->nrRows = _gridStructure.header().nrRows;
    dataMeteoGrid.header->flag = NODATA;
    dataMeteoGrid.initializeGrid();

    if (_meteoPoints.empty())
    {
        dataMeteoGrid.initializeGrid(NODATA);
    }
    else
    {
        for (int i = 0; i < dataMeteoGrid.header->nrRows; i++)
        {
            for (int j = 0; j < dataMeteoGrid.header->nrCols; j++)
            {
                if (_meteoPoints[i][j]->active == 0)
                {
                    dataMeteoGrid.value[i][j] = NODATA;
                }
                else
                {
                    dataMeteoGrid.value[i][j] = _meteoPoints[i][j]->currentValue;
                }
            }
        }
    }
    dataMeteoGrid.isLoaded = true;
    return true;

}

std::vector<std::vector<Crit3DMeteoPoint *> > Crit3DMeteoGrid::meteoPoints() const
{
    return _meteoPoints;
}

void Crit3DMeteoGrid::setMeteoPoints(const std::vector<std::vector<Crit3DMeteoPoint *> > &meteoPoints)
{
    _meteoPoints = meteoPoints;
}

void Crit3DMeteoGrid::initMeteoPoints(int nRow, int nCol)
{

    _meteoPoints.reserve(nRow);

    for (int i = 0; i < nRow; i++)
    {
        std::vector<Crit3DMeteoPoint*> meteoPointVector;
        meteoPointVector.reserve(nCol);

        for (int j = 0; j < nCol; j++)
        {
            Crit3DMeteoPoint* meteoPoint = new Crit3DMeteoPoint;
            meteoPoint->active = 0;
            meteoPoint->selected = 0;
            meteoPointVector.push_back(meteoPoint);
        }

        _meteoPoints.push_back(meteoPointVector);

    }

}

void Crit3DMeteoGrid::fillMeteoPoint(int row, int col, std::string code, std::string name, int height, bool active, int area)
{

    _meteoPoints[row][col]->id = code;
    _meteoPoints[row][col]->name = name;
    _meteoPoints[row][col]->point.z = height;
    _meteoPoints[row][col]->active = active;
    _meteoPoints[row][col]->area = area;

}

bool Crit3DMeteoGrid::findCellFromCode(int* row, int* col, std::string code)
{

    for (int i = 0; i < dataMeteoGrid.header->nrRows; i++)
    {
        for (int j = 0; j < dataMeteoGrid.header->nrCols; j++)
        {
            if (_meteoPoints[i][j]->id == code)
            {
                *row = i;
                *col = j;
                return true;
            }
        }
    }
    return false;
}

void Crit3DMeteoGrid::setGridStructure(const Crit3DMeteoGridStructure &gridStructure)
{
    _gridStructure = gridStructure;
}


bool Crit3DMeteoGrid::isAggregationDefined() const
{
    return _isAggregationDefined;
}

void Crit3DMeteoGrid::setIsAggregationDefined(bool isAggregationDefined)
{
    _isAggregationDefined = isAggregationDefined;
}

bool Crit3DMeteoGrid::isDateTypeSpecified() const
{
    return _isDateTypeSpecified;
}

void Crit3DMeteoGrid::setIsDateTypeSpecified(bool isDateTypeSpecified)
{
    _isDateTypeSpecified = isDateTypeSpecified;
}

Crit3DDate Crit3DMeteoGrid::firstDate() const
{
    return _firstDate;
}

void Crit3DMeteoGrid::setFirstDate(const Crit3DDate &firstDate)
{
    _firstDate = firstDate;
}

Crit3DDate Crit3DMeteoGrid::lastDate() const
{
    return _lastDate;
}

void Crit3DMeteoGrid::setLastDate(const Crit3DDate &lastDate)
{
    _lastDate = lastDate;
}

int Crit3DMeteoGrid::nrVarsArray() const
{
    return _nrVarsArray;
}

void Crit3DMeteoGrid::setNrVarsArray(int nrVarsArray)
{
    _nrVarsArray = nrVarsArray;
}








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

bool Crit3DMeteoGrid::createRasterGrid()
{
    if (_gridStructure.isUTM())
    {
        dataMeteoGrid.header->cellSize = _gridStructure.header().dx;
        dataMeteoGrid.header->llCorner->x = _gridStructure.header().llCorner->longitude;
        dataMeteoGrid.header->llCorner->y = _gridStructure.header().llCorner->latitude;
    }
    else
    {
        dataMeteoGrid.header->cellSize = NODATA;
        dataMeteoGrid.header->llCorner->x = NODATA;
        dataMeteoGrid.header->llCorner->y = NODATA;
    }

    dataMeteoGrid.header->nrCols = _gridStructure.header().nrCols;
    dataMeteoGrid.header->nrRows = _gridStructure.header().nrRows;
    dataMeteoGrid.header->flag = NODATA;

    dataMeteoGrid.initializeGrid(NODATA);

    dataMeteoGrid.isLoaded = true;
    return true;
}


bool Crit3DMeteoGrid::isNorthernEmisphere() const
{
    return _isNorthernEmisphere;
}

void Crit3DMeteoGrid::setIsNorthernEmisphere(bool isNorthernEmisphere)
{
    _isNorthernEmisphere = isNorthernEmisphere;
}

int Crit3DMeteoGrid::utmZone() const
{
    return _utmZone;
}

void Crit3DMeteoGrid::setUtmZone(int utmZone)
{
    _utmZone = utmZone;
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
            meteoPoint->active = false;
            meteoPoint->selected = false;
            meteoPointVector.push_back(meteoPoint);
        }

        _meteoPoints.push_back(meteoPointVector);
    }
}


void Crit3DMeteoGrid::fillMeteoPoint(int row, int col, std::string code, std::string name, int height, bool active)
{

    _meteoPoints[row][col]->id = code;
    _meteoPoints[row][col]->name = name;
    _meteoPoints[row][col]->point.z = height;
    _meteoPoints[row][col]->active = active;

    if (_gridStructure.isRegular())
    {
        if (_gridStructure.isUTM())
        {
            _meteoPoints[row][col]->point.utm.x = _gridStructure.header().llCorner->longitude + _gridStructure.header().dx * (col + 0.5);
            _meteoPoints[row][col]->point.utm.y = _gridStructure.header().llCorner->latitude + _gridStructure.header().dy * (row + 0.5);
            gis::utmToLatLon(_utmZone, _isNorthernEmisphere, _meteoPoints[row][col]->point.utm.x, _meteoPoints[row][col]->point.utm.y, &(_meteoPoints[row][col]->latitude), &(_meteoPoints[row][col]->longitude));
        }
        else
        {
            _meteoPoints[row][col]->longitude = _gridStructure.header().llCorner->longitude + _gridStructure.header().dx * (col + 0.5);
            _meteoPoints[row][col]->latitude = _gridStructure.header().llCorner->latitude + _gridStructure.header().dy * (row + 0.5);

            gis::Crit3DUtmPoint utmPoint;
            gis::Crit3DGeoPoint geoPoint(_meteoPoints[row][col]->latitude, _meteoPoints[row][col]->longitude);
            gis::getUtmFromLatLon(_utmZone, geoPoint, &utmPoint);
            _meteoPoints[row][col]->point.utm.x = utmPoint.x;
            _meteoPoints[row][col]->point.utm.y = utmPoint.y;

        }
    }

}

bool Crit3DMeteoGrid::fillMeteoPointDailyValue(int row, int col, int numberOfDays, int initialize, Crit3DDate date, meteoVariable variable, float value)
{
    if (initialize)
    {
        _meteoPoints[row][col]->initializeObsDataD(numberOfDays, date);
    }
    return _meteoPoints[row][col]->setMeteoPointValueD(date, variable, value);
}

bool Crit3DMeteoGrid::fillMeteoPointHourlyValue(int row, int col, int numberOfDays, int initialize, Crit3DDate date, int  hour, int minute, meteoVariable variable, float value)
{
    int myHourlyFraction = 1;

    if (initialize)
    {
        _meteoPoints[row][col]->initializeObsDataH(myHourlyFraction, numberOfDays, date);
    }

    return _meteoPoints[row][col]->setMeteoPointValueH(date, hour, minute, variable, value);

}


bool Crit3DMeteoGrid::fillMeteoPointCurrentDailyValue(Crit3DDate date, meteoVariable variable)
{
    for (int row = 0; row < gridStructure().header().nrRows; row++)
    {
        for (int col = 0; col < gridStructure().header().nrCols; col++)
        {
            if (_meteoPoints[row][col]->active && _meteoPoints[row][col]->nrObsDataDaysD != 0)
            {
                _meteoPoints[row][col]->currentValue = _meteoPoints[row][col]->getMeteoPointValueD(date, variable);
            }
            else
            {
                _meteoPoints[row][col]->currentValue = NODATA;
            }
        }
    }
    return true;
}


bool Crit3DMeteoGrid::fillMeteoPointCurrentHourlyValue(Crit3DDate date, int hour, int minute, meteoVariable variable)
{
    for (int row = 0; row < gridStructure().header().nrRows; row++)
    {
        for (int col = 0; col < gridStructure().header().nrCols; col++)
        {
            if (_meteoPoints[row][col]->active && _meteoPoints[row][col]->nrObsDataDaysD != 0)
            {
                _meteoPoints[row][col]->currentValue = _meteoPoints[row][col]->getMeteoPointValueH(date, hour, minute, variable);
            }
            else
            {
                _meteoPoints[row][col]->currentValue = NODATA;
            }
        }
    }
    return true;
}


void Crit3DMeteoGrid::fillMeteoRaster()
{
    for (int i = 0; i < dataMeteoGrid.header->nrRows; i++)
    {
        for (int j = 0; j < dataMeteoGrid.header->nrCols; j++)
        {
             dataMeteoGrid.value[i][j] = _meteoPoints[this->gridStructure().header().nrRows-1-i][j]->currentValue;
        }
    }
}


bool Crit3DMeteoGrid::findMeteoPointFromId(int* row, int* col, std::string id)
{

    for (int i = 0; i < gridStructure().header().nrRows; i++)
    {
        for (int j = 0; j < gridStructure().header().nrCols; j++)
        {
            if (_meteoPoints[i][j]->id == id)
            {
                *row = i;
                *col = j;
                return true;
            }
        }
    }
    return false;
}

bool Crit3DMeteoGrid::getMeteoPointActiveId(int row, int col, std::string* id)
{
    if (row < gridStructure().header().nrRows && col < gridStructure().header().nrCols)
    {
        if (_meteoPoints[row][col]->active)
        {
            *id = _meteoPoints[row][col]->id;
            return true;
        }

    }
    return false;
}

bool Crit3DMeteoGrid::isActiveMeteoPointFromId(std::string id)
{

    for (int i = 0; i < gridStructure().header().nrRows; i++)
    {
        for (int j = 0; j < gridStructure().header().nrCols; j++)
        {
            if (_meteoPoints[i][j]->id == id)
            {
                return true;
            }
        }
    }
    return false;
}

bool Crit3DMeteoGrid::findFirstActiveMeteoPoint(std::string* id, int* row, int* col)
{

    for (int i = *row; i < gridStructure().header().nrRows; i++)
    {
        for (int j = *col; j < gridStructure().header().nrCols; j++)
        {
            if (_meteoPoints[i][j]->active)
            {
                *row = i;
                *col = j;
                *id = _meteoPoints[i][j]->id;
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








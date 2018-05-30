/*!
    \copyright 2018 Fausto Tomei, Gabriele Antolini,
    Alberto Pistocchi, Marco Bittelli, Antonio Volta, Laura Costantini

    This file is part of CRITERIA3D.
    CRITERIA3D has been developed under contract issued by ARPAE Emilia-Romagna

    CRITERIA3D is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    CRITERIA3D is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with CRITERIA3D.  If not, see <http://www.gnu.org/licenses/>.

    contacts:
    fausto.tomei@gmail.com
    ftomei@arpae.it
*/


#include "meteoGrid.h"
#include "statistics.h"
#include "furtherMathFunctions.h"
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
    _isAggregationDefined = false;
    _gisSettings.utmZone = 32;
    _gisSettings.isNorthernEmisphere = true;
    _firstDate = Crit3DDate(1,1,1800);
    _lastDate = Crit3DDate(1,1,1800);
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


std::vector<std::vector<Crit3DMeteoPoint *> > Crit3DMeteoGrid::meteoPoints() const
{
    return _meteoPoints;
}

Crit3DMeteoPoint Crit3DMeteoGrid::meteoPoint(int row, int col)
{
    return *(_meteoPoints[row][col]);
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

void Crit3DMeteoGrid::setActive(int row, int col, bool active)
{
    _meteoPoints[row][col]->active = active;
}


// for visualization raster
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
            gis::utmToLatLon(_gisSettings.utmZone, _gisSettings.isNorthernEmisphere, _meteoPoints[row][col]->point.utm.x, _meteoPoints[row][col]->point.utm.y, &(_meteoPoints[row][col]->latitude), &(_meteoPoints[row][col]->longitude));
        }
        else
        {
            _meteoPoints[row][col]->longitude = _gridStructure.header().llCorner->longitude + _gridStructure.header().dx * (col + 0.5);
            _meteoPoints[row][col]->latitude = _gridStructure.header().llCorner->latitude + _gridStructure.header().dy * (row + 0.5);

            gis::Crit3DUtmPoint utmPoint;
            gis::Crit3DGeoPoint geoPoint(_meteoPoints[row][col]->latitude, _meteoPoints[row][col]->longitude);
            gis::getUtmFromLatLon(_gisSettings.utmZone, geoPoint, &utmPoint);
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
    for (int row = 0; row < _gridStructure.header().nrRows; row++)
    {
        for (int col = 0; col < _gridStructure.header().nrCols; col++)
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
    for (int row = 0; row < _gridStructure.header().nrRows; row++)
    {
        for (int col = 0; col < _gridStructure.header().nrCols; col++)
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
             dataMeteoGrid.value[i][j] = _meteoPoints[_gridStructure.header().nrRows-1-i][j]->currentValue;
        }
    }
}

gis::Crit3DGisSettings Crit3DMeteoGrid::getGisSettings() const
{
    return _gisSettings;
}

void Crit3DMeteoGrid::setGisSettings(const gis::Crit3DGisSettings &gisSettings)
{
    _gisSettings = gisSettings;
}


bool Crit3DMeteoGrid::findMeteoPointFromId(int* row, int* col, std::string id)
{

    for (int i = 0; i < _gridStructure.header().nrRows; i++)
    {
        for (int j = 0; j < _gridStructure.header().nrCols; j++)
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
    if (row < _gridStructure.header().nrRows && col < _gridStructure.header().nrCols)
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

    for (int i = 0; i < _gridStructure.header().nrRows; i++)
    {
        for (int j = 0; j < _gridStructure.header().nrCols; j++)
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

    for (int i = *row; i < _gridStructure.header().nrRows; i++)
    {
        for (int j = *col; j < _gridStructure.header().nrCols; j++)
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

void Crit3DMeteoGrid::findGridAggregationPoints(gis::Crit3DRasterGrid* myDTM)
{
    bool excludeNoData = false;

    for (int row = 0; row < _gridStructure.header().nrRows; row++)
    {
        for (int col = 0; col < _gridStructure.header().nrCols; col++)
        {
            if (_meteoPoints[row][col]->active)
            {
                assignCellAggregationPoints(row, col, myDTM, excludeNoData);
            }
        }
    }
    _isAggregationDefined = true;
}

void Crit3DMeteoGrid::assignCellAggregationPoints(int row, int col, gis::Crit3DRasterGrid* myDTM, bool excludeNoData)
{

    gis::Crit3DUtmPoint utmLL, utmUR;
    gis::Crit3DUtmPoint utmPoint;
    gis::Crit3DPoint point;
    gis::Crit3DUtmPoint v[4];

    if (_gridStructure.isTIN())
    {
        //TO DO
    }
    else
    {
        if (_gridStructure.isUTM())
        {

            _meteoPoints[row][col]->aggregationPoints.clear();

            utmLL.x = _meteoPoints[row][col]->point.utm.x - (_gridStructure.header().dx / 2) + (myDTM->header->cellSize / 2);
            utmUR.x = _meteoPoints[row][col]->point.utm.x + (_gridStructure.header().dx / 2);
            utmLL.y = _meteoPoints[row][col]->point.utm.y - (_gridStructure.header().dy / 2) + (myDTM->header->cellSize / 2);
            utmUR.y = _meteoPoints[row][col]->point.utm.y + (_gridStructure.header().dy / 2);

            _meteoPoints[row][col]->aggregationPointsMaxNr = 0;

            for (double x = utmLL.x; x < utmUR.x; x=x+myDTM->header->cellSize)
            {
                for (double y = utmLL.y; x < utmUR.y; y=y+myDTM->header->cellSize)
                {
                    _meteoPoints[row][col]->aggregationPointsMaxNr = _meteoPoints[row][col]->aggregationPointsMaxNr + 1;
                    if (!excludeNoData || gis::getValueFromXY(*myDTM, x, y) != myDTM->header->flag )
                    {
                         utmPoint.x = x;
                         utmPoint.y = y;
                         point.utm = utmPoint;
                         point.z = NODATA;
                        _meteoPoints[row][col]->aggregationPoints.push_back(point);
                    }
                }
            }
        }
        else
        {

            gis::Crit3DGeoPoint pointLatLon0;
            gis::Crit3DGeoPoint geoPoint;
            pointLatLon0.latitude = _gridStructure.header().llCorner->latitude + row * _gridStructure.header().dy;
            pointLatLon0.longitude = _gridStructure.header().llCorner->longitude + col * _gridStructure.header().dx;
            gis::getUtmFromLatLon(_gisSettings.utmZone, pointLatLon0, &utmPoint);
            v[0] = utmPoint;

            geoPoint.latitude = pointLatLon0.latitude + _gridStructure.header().dy;
            geoPoint.longitude = pointLatLon0.longitude;
            gis::getUtmFromLatLon(_gisSettings.utmZone, geoPoint, &utmPoint);
            v[1] = utmPoint;

            geoPoint.latitude = pointLatLon0.latitude + _gridStructure.header().dy;
            geoPoint.longitude = pointLatLon0.longitude + _gridStructure.header().dx;
            gis::getUtmFromLatLon(_gisSettings.utmZone, geoPoint, &utmPoint);
            v[2] = utmPoint;

            geoPoint.latitude = pointLatLon0.latitude;
            geoPoint.longitude = pointLatLon0.longitude + _gridStructure.header().dx;
            gis::getUtmFromLatLon(_gisSettings.utmZone, geoPoint, &utmPoint);
            v[3] = utmPoint;

            utmLL.x = minValue(v[0].x, v[1].x);
            utmLL.y = minValue(v[0].y, v[3].y);
            utmUR.x = maxValue(v[2].x, v[3].x);
            utmUR.y = maxValue(v[1].y, v[2].y);

            gis::Crit3DRasterCell demLL, demUR;

            gis::getRowColFromXY(*myDTM, utmLL.x, utmLL.y, &demLL.row, &demLL.col);
            gis::getRowColFromXY(*myDTM, utmUR.x, utmUR.y, &demUR.row, &demUR.col);
            _meteoPoints[row][col]->aggregationPoints.clear();
            _meteoPoints[row][col]->aggregationPointsMaxNr = 0;

            if ( ((demUR.row >= 0) && (demUR.row < myDTM->header->nrRows)) || ((demLL.row >= 0) && (demLL.row < myDTM->header->nrRows))
                 || ((demUR.col >= 0) && (demUR.col < myDTM->header->nrCols)) || ((demLL.col >= 0) && ( demLL.col < myDTM->header->nrCols)))
            {


                for (int myDTMRow = demUR.row; myDTMRow < demLL.row; myDTMRow++)
                {
                    for (int myDTMCol = demLL.col; myDTMCol < demUR.col; myDTMCol++)
                    {
                        double utmX, utmY;
                        gis::Crit3DGeoPoint geoP;
                        gis::Crit3DGridHeader latLonHeader;

                        gis::getUtmXYFromRowCol(*(myDTM->header), myDTMRow, myDTMCol, &utmX, &utmY);
                        gis::getLatLonFromUtm(_gisSettings, utmX, utmY, &geoP.latitude, &geoP.longitude);

                        latLonHeader.llCorner->latitude = pointLatLon0.latitude;
                        latLonHeader.llCorner->longitude = pointLatLon0.longitude;
                        latLonHeader.dx = _gridStructure.header().dx;
                        latLonHeader.dy = _gridStructure.header().dy;
                        latLonHeader.nrRows = row;
                        latLonHeader.nrCols = col;

                        if (geoP.isInsideGrid(latLonHeader))
                        {
                            _meteoPoints[row][col]->aggregationPointsMaxNr = _meteoPoints[row][col]->aggregationPointsMaxNr + 1;
                            if (!excludeNoData || myDTM->getValueFromRowCol(myDTMRow, myDTMCol) != myDTM->header->flag )
                            {
                                 gis::getUtmXYFromRowCol(*(myDTM->header), myDTMRow, myDTMCol, &utmX, &utmY);
                                 utmPoint.x = utmX;
                                 utmPoint.y = utmY;
                                 point.utm = utmPoint;
                                 point.z = NODATA;
                                _meteoPoints[row][col]->aggregationPoints.push_back(point);
                            }
                        }
                    }
                }
            }

        }
    }
    // TO DO compute std deviation
}

void Crit3DMeteoGrid::aggregateMeteoGrid(meteoVariable myVar, frequencyType freq, Crit3DDate date, int  hour, int minute, gis::Crit3DRasterGrid* myDTM, gis::Crit3DRasterGrid dataRaster, elaborationMethods elab)
{
    int numberOfDays = 1;
    int initialize;

    if (!_isAggregationDefined)
    {
        findGridAggregationPoints(myDTM);
    }

    // TO DO
    //dbGridManagement.initializeStandardDeviation


    for (int col = 0; col < _gridStructure.header().nrCols; col++)
    {
        for (int row = 0; row < _gridStructure.header().nrRows; row++)
        {
            if (_meteoPoints[row][col]->active)
            {

                double validValues = 0;
                for (unsigned int i = 0; i < _meteoPoints[row][col]->aggregationPoints.size(); i++)
                {
                    double x = _meteoPoints[row][col]->aggregationPoints[i].utm.x;
                    double y = _meteoPoints[row][col]->aggregationPoints[i].utm.y;
                    double interpolatedValue = gis::getValueFromXY(dataRaster, x, y);
                    if (interpolatedValue != dataRaster.header->flag)
                    {
                        _meteoPoints[row][col]->aggregationPoints[i].z = interpolatedValue;
                        validValues = validValues + 1;
                    }
                }

                if (!_meteoPoints[row][col]->aggregationPoints.empty())
                {

                    if ( (validValues / _meteoPoints[row][col]->aggregationPointsMaxNr) > ( GRID_MIN_COVERAGE / 100 ) )
                    {

                        double myValue = aggregateMeteoGridPoint(*(_meteoPoints[row][col]), elab);
                        // TO DO std dev
                        //.stdDev = AggregateMeteoGridPoint(Definitions.ELAB_STDDEVIATION, MeteoGrid.Point(myRow, myCol))
                        if (freq == hourly)
                        {
                            if (_meteoPoints[row][col]->nrObsDataDaysH == 0)
                            {
                                initialize = 1;
                            }
                            else
                            {
                                _meteoPoints[row][col]->obsDataH[0].date = date;
                                initialize = 0;
                            }
                            //std::cout << "id: " << _meteoPoints[row][col]->id << "row: " << row << "col: " << col <<"value: " << myValue <<std::endl;
                            fillMeteoPointHourlyValue(row, col, numberOfDays, initialize, date, hour, minute, myVar, float(myValue));
                            fillMeteoPointCurrentHourlyValue(date, hour, minute, myVar);

                        }
                        else if (freq == daily)
                        {
                            if (_meteoPoints[row][col]->nrObsDataDaysD == 0)
                            {
                                initialize = 1;
                            }
                            else
                            {
                                _meteoPoints[row][col]->obsDataD[0].date = date;
                                initialize = 0;
                            }
                            fillMeteoPointDailyValue(row, col, numberOfDays, initialize, date, myVar, float(myValue));
                            fillMeteoPointCurrentDailyValue(date, myVar);
                            //std::cout << "id: " << _meteoPoints[row][col]->id << " row: " << row << " col: " << col <<" value: " << myValue << " currentValue: " << _meteoPoints[row][col]->currentValue <<std::endl;

                        }

                    }
                }

            }
        }
    }

}


double Crit3DMeteoGrid::aggregateMeteoGridPoint(Crit3DMeteoPoint myPoint, elaborationMethods elab)
{

    std::vector <double> validValues;


    for (unsigned int i = 0; i < myPoint.aggregationPoints.size(); i++)
    {
        if (myPoint.aggregationPoints[i].z != NODATA)
        {
            validValues.push_back(myPoint.aggregationPoints[i].z);
        }
    }

    if (validValues.empty())
    {
        return NODATA;
    }

    if ( (static_cast<double>(validValues.size()) / myPoint.aggregationPointsMaxNr) < ( GRID_MIN_COVERAGE / 100.0) )
    {
        return NODATA;
    }

    if (elab == elaborationMethods::mean)
    {
        return statistics::mean(validValues.data(), validValues.size());
    }
    else if (elab == elaborationMethods::median)
    {
        int size = validValues.size();
        return sorting::percentile(validValues.data(), &size, 50.0, true);
    }
    else if (elab == elaborationMethods::stdDeviation)
    {
        return statistics::standardDeviation(validValues.data(), validValues.size());
    }
    else
    {
        return NODATA;
    }


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








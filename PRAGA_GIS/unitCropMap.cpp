#include "unitCropMap.h"
#include "commonConstants.h"
#include <float.h>
#include <algorithm>

bool zonalStatistics(Crit3DShapeHandler* shapeRef, Crit3DShapeHandler* shapeVal, std::string valField, DBFFieldType fieldType, int cellSize, opType type)
{

    // check shape type
    if ( shapeRef->getTypeString() != shapeVal->getTypeString() || shapeRef->getTypeString() != "2D Polygon" )
    {
        return false;
    }
    // check proj
    if (shapeRef->getIsWGS84() == false || shapeVal->getIsWGS84() == false)
    {
        return false;
    }
    // check utm zone
    if (shapeRef->getUtmZone() != shapeVal->getUtmZone())
    {
        return false;
    }

    gis::Crit3DRasterGrid rasterRef = shapeToRaster(*shapeRef, cellSize);
    gis::Crit3DRasterGrid rasterVal = rasterRef;

    int fieldIndex = shapeVal->getDBFFieldIndex(valField.c_str());

    if (fieldType == 0)
    {
        std::vector<std::string> varFieldVector;
        for (int record = 0; record < shapeVal->getDBFRecordCount(); record++)
        {
            std::string value = shapeVal->readStringAttribute(record,fieldIndex);
            if (std::find (varFieldVector.begin(), varFieldVector.end(), value) != varFieldVector.end())
            {
                varFieldVector.push_back(value);
            }

        }
    }
    else if (fieldType == 1)
    {
        std::vector<int> varFieldVector;
        for (int record = 0; record < shapeVal->getDBFRecordCount(); record++)
        {
            int value = shapeVal->readIntAttribute(record,fieldIndex);
            if (std::find (varFieldVector.begin(), varFieldVector.end(), value) != varFieldVector.end())
            {
                varFieldVector.push_back(value);
            }
        }
    }
    else if (fieldType == 2)
    {
        std::vector<double> varFieldVector;
        for (int record = 0; record < shapeVal->getDBFRecordCount(); record++)
        {
            double value = shapeVal->readDoubleAttribute(record,fieldIndex);
            if (std::find (varFieldVector.begin(), varFieldVector.end(), value) != varFieldVector.end())
            {
                varFieldVector.push_back(value);
            }
        }
    }


    return true;


}

gis::Crit3DRasterGrid shapeToRaster(Crit3DShapeHandler shape, int cellSize)
{
    gis::Crit3DRasterHeader header;
    ShapeObject object;

    double ymin = DBL_MAX;
    double xmin = DBL_MAX;
    double ymax = DBL_MIN;
    double xmax = DBL_MIN;

    for (int i = 0; i < shape.getShapeCount(); i++)
    {
        shape.getShape(i, object);
        if (object.getBounds().ymin < ymin)
        {
            ymin = object.getBounds().ymin;
        }
        if (object.getBounds().xmin < xmin)
        {
            xmin = object.getBounds().xmin;
        }
        if (object.getBounds().ymax > ymax)
        {
            ymax = object.getBounds().ymax;
        }
        if (object.getBounds().xmax > xmax)
        {
            xmax = object.getBounds().xmax;
        }
    }

    header.cellSize = cellSize;
    header.nrRows = (ymax - ymin) / cellSize;
    header.nrCols = (xmax - xmin) / cellSize;
    header.llCorner->x = xmin;
    header.llCorner->y = ymin;

    gis::Crit3DRasterGrid raster;
    raster.initializeGrid(header);

    int value = NODATA;
    for (int row = 0; row < header.nrRows; row++)
    {
        for (int col = 0; col < header.nrCols; col++)
        {
            gis::Crit3DUtmPoint* point = raster.utmPoint(row, col);
            Point<double> UTMpoint;
            UTMpoint.x = point->x;
            UTMpoint.y = point->y;
            for (int shapeIndex = 0; shapeIndex < shape.getShapeCount(); shapeIndex++)
            {
                shape.getShape(shapeIndex, object);
                value = object.pointInPolygon(UTMpoint);
                if (value != NODATA)
                {
                    break;
                }
            }
            raster.value[row][col] = value;
        }
    }

    return raster;
}



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
    fillRasterWithShapeNumber(&rasterRef, *shapeRef);
    gis::Crit3DRasterGrid rasterVal = shapeToRaster(*shapeRef, cellSize);
    fillRasterWithShapeNumber(&rasterVal, *shapeVal);

    int fieldIndex = shapeVal->getDBFFieldIndex(valField.c_str());

    // add new field to shapeRed
    shapeRef->addField(valField.c_str(), fieldType, shapeVal->nWidthField(fieldIndex), shapeVal->nDecimalsField(fieldIndex));

    int varFieldVectorSize = 0;

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
        varFieldVectorSize = varFieldVector.size();

        for (int row = 0; row < rasterVal.header->nrRows; row++)
        {
            for (int col = 0; col < rasterVal.header->nrCols; col++)
            {
                int shape = rasterVal.value[row][col];
                if (shape!= NODATA)
                {
                    std::string valueField = shapeVal->readStringAttribute(shape,fieldIndex);
                    int vectorFieldPos = std::distance(varFieldVector.begin(), std::find (varFieldVector.begin(), varFieldVector.end(), valueField));
                    //replace value
                    rasterVal.value[row][col] = vectorFieldPos;
                }
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

        varFieldVectorSize = varFieldVector.size();

        for (int row = 0; row < rasterVal.header->nrRows; row++)
        {
            for (int col = 0; col < rasterVal.header->nrCols; col++)
            {
                int shape = rasterVal.value[row][col];
                if (shape!= NODATA)
                {
                    int valueField = shapeVal->readIntAttribute(shape,fieldIndex);
                    int vectorFieldPos = std::distance(varFieldVector.begin(), std::find (varFieldVector.begin(), varFieldVector.end(), valueField));
//                    //replace value
                    rasterVal.value[row][col] = vectorFieldPos;
                }
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

        varFieldVectorSize = varFieldVector.size();

        for (int row = 0; row < rasterVal.header->nrRows; row++)
        {
            for (int col = 0; col < rasterVal.header->nrCols; col++)
            {
                int shape = rasterVal.value[row][col];
                if (shape!= NODATA)
                {
                    double valueField = shapeVal->readDoubleAttribute(shape,fieldIndex);
                    int vectorFieldPos = std::distance(varFieldVector.begin(), std::find (varFieldVector.begin(), varFieldVector.end(), valueField));
//                    //replace value
                    rasterVal.value[row][col] = vectorFieldPos;
                }
            }
        }
    }

    int matrix[shapeRef->getShapeCount()][varFieldVectorSize];
    std::fill(*matrix, *matrix + shapeRef->getShapeCount()*varFieldVectorSize, 0);


    for (int row = 0; row < rasterRef.header->nrRows; row++)
    {
        for (int col = 0; col < rasterRef.header->nrCols; col++)
        {
            int refValue = rasterRef.value[row][col];
            int valValue = rasterVal.value[row][col];

            if ( refValue != NODATA && valValue != NODATA)
            {
                matrix[refValue][valValue] = matrix[refValue][valValue] + 1;
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
    return raster;
}

void fillRasterWithShapeNumber(gis::Crit3DRasterGrid* raster, Crit3DShapeHandler shape)
{
    ShapeObject object;
    int value = NODATA;
    for (int row = 0; row < raster->header->nrRows; row++)
    {
        for (int col = 0; col < raster->header->nrCols; col++)
        {
            gis::Crit3DUtmPoint* point = raster->utmPoint(row, col);
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
            raster->value[row][col] = value;
        }
    }
    return;
}



#include "unitCropMap.h"
#include "commonConstants.h"
#include <float.h>
#include <algorithm>
#include <math.h>

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

    int nShape = shapeRef->getShapeCount();

    gis::Crit3DRasterGrid rasterRef = shapeToRaster(*shapeRef, cellSize);
    fillRasterWithShapeNumber(&rasterRef, *shapeRef);
    gis::Crit3DRasterGrid rasterVal = shapeToRaster(*shapeRef, cellSize);
    fillRasterWithShapeNumber(&rasterVal, *shapeVal);

    // add new field to shapeRef
    int fieldIndex = shapeVal->getDBFFieldIndex(valField.c_str());
    shapeRef->addField(valField.c_str(), fieldType, shapeVal->nWidthField(fieldIndex), shapeVal->nDecimalsField(fieldIndex));

    // LC da fare restyle codice affinchè gestisca i 3 casi
    std::vector<std::string> varFieldVectorString;
    std::vector<int> varFieldVectorInt;
    std::vector<double> varFieldVectorDouble;
    int varFieldVectorSize = 0;


    if (fieldType == 0)
    {
        // fill varFieldVector
        for (int record = 0; record < shapeVal->getDBFRecordCount(); record++)
        {
            std::string value = shapeVal->readStringAttribute(record,fieldIndex);
            if (std::find (varFieldVectorString.begin(), varFieldVectorString.end(), value) != varFieldVectorString.end())
            {
                varFieldVectorString.push_back(value);
            }
        }
        varFieldVectorSize = varFieldVectorString.size();

        // assign varFieldVector index to each pixel of rasterVal
        for (int row = 0; row < rasterVal.header->nrRows; row++)
        {
            for (int col = 0; col < rasterVal.header->nrCols; col++)
            {
                int shape = rasterVal.value[row][col];
                if (shape!= NODATA)
                {
                    std::string valueField = shapeVal->readStringAttribute(shape,fieldIndex);
                    int vectorFieldPos = std::distance(varFieldVectorString.begin(), std::find (varFieldVectorString.begin(), varFieldVectorString.end(), valueField));
                    //replace value
                    rasterVal.value[row][col] = vectorFieldPos;
                }
            }
        }
    }
    else if (fieldType == 1)
    {

        for (int record = 0; record < shapeVal->getDBFRecordCount(); record++)
        {
            int value = shapeVal->readIntAttribute(record,fieldIndex);
            if (std::find (varFieldVectorInt.begin(), varFieldVectorInt.end(), value) != varFieldVectorInt.end())
            {
                varFieldVectorInt.push_back(value);
            }
        }

        varFieldVectorSize = varFieldVectorInt.size();

        for (int row = 0; row < rasterVal.header->nrRows; row++)
        {
            for (int col = 0; col < rasterVal.header->nrCols; col++)
            {
                int shape = rasterVal.value[row][col];
                if (shape!= NODATA)
                {
                    int valueField = shapeVal->readIntAttribute(shape,fieldIndex);
                    int vectorFieldPos = std::distance(varFieldVectorInt.begin(), std::find (varFieldVectorInt.begin(), varFieldVectorInt.end(), valueField));
//                    //replace value
                    rasterVal.value[row][col] = vectorFieldPos;
                }
            }
        }
    }
    else if (fieldType == 2)
    {

        for (int record = 0; record < shapeVal->getDBFRecordCount(); record++)
        {
            double value = shapeVal->readDoubleAttribute(record,fieldIndex);
            if (std::find (varFieldVectorDouble.begin(), varFieldVectorDouble.end(), value) != varFieldVectorDouble.end())
            {
                varFieldVectorDouble.push_back(value);
            }
        }

        varFieldVectorSize = varFieldVectorDouble.size();

        for (int row = 0; row < rasterVal.header->nrRows; row++)
        {
            for (int col = 0; col < rasterVal.header->nrCols; col++)
            {
                int shape = rasterVal.value[row][col];
                if (shape!= NODATA)
                {
                    double valueField = shapeVal->readDoubleAttribute(shape,fieldIndex);
                    int vectorFieldPos = std::distance(varFieldVectorDouble.begin(), std::find (varFieldVectorDouble.begin(), varFieldVectorDouble.end(), valueField));
//                    //replace value
                    rasterVal.value[row][col] = vectorFieldPos;
                }
            }
        }
    }

    // analysis matrix

    int matrix[nShape][varFieldVectorSize];
    std::fill(*matrix, *matrix + nShape *varFieldVectorSize, 0);


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

    // save right matrix element for each shape
    int index[nShape];

    if (type == MAJORITY)
    {
        for (int row = 0; row < rasterRef.header->nrRows; row++)
        {
            index[row] = 0;
            for (int col = 0; col < rasterRef.header->nrCols; col++)
            {
                if (matrix[row][col] > index[row])
                {
                    index[row] = matrix[row][col];
                }
            }
        }
    }
    else if (type == MIN)
    {
        for (int row = 0; row < rasterRef.header->nrRows; row++)
        {
            // TO DO
        }
    }
    else if (type == MAX)
    {
        for (int row = 0; row < rasterRef.header->nrRows; row++)
        {
            // TO DO
        }
    }
    else if (type == AVG)
    {
        for (int row = 0; row < rasterRef.header->nrRows; row++)
        {
            // TO DO
        }
    }

    // save value of the new field
    for (int shapeIndex = 0; shapeIndex < nShape; shapeIndex++)
    {
        if (fieldType == 0)
        {
            std::string varFieldFound = varFieldVectorString[index[shapeIndex]];
            shapeRef->writeStringAttribute(shapeIndex, shapeRef->getDBFFieldIndex(valField.c_str()), varFieldFound.c_str());
        }
        else if (fieldType == 1)
        {
            int varFieldFound = varFieldVectorInt[index[shapeIndex]];
            shapeRef->writeIntAttribute(shapeIndex, shapeRef->getDBFFieldIndex(valField.c_str()), varFieldFound);
        }
        else if (fieldType == 2)
        {
            int varFieldFound = varFieldVectorDouble[index[shapeIndex]];
            shapeRef->writeDoubleAttribute(shapeIndex, shapeRef->getDBFFieldIndex(valField.c_str()), varFieldFound);
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

    xmin = floor(xmin);
    ymin = floor(ymin);
    header.cellSize = cellSize;
    header.llCorner->x = xmin;
    header.llCorner->y = ymin;
    header.nrRows = floor((ymax - ymin) / cellSize)+1;
    header.nrCols = floor((xmax - xmin) / cellSize)+1;

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




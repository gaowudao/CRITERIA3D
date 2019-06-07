#include "shapeToRaster.h"

#include "commonConstants.h"
#include <float.h>
#include <math.h>

#include <QtWidgets> // debug

gis::Crit3DRasterGrid* initializeRasterFromShape(Crit3DShapeHandler* shape, gis::Crit3DRasterGrid* raster, int cellSize)
{
    gis::Crit3DRasterHeader header;
    ShapeObject object;
    Box<double> bounds;

    double ymin = DBL_MAX;
    double xmin = DBL_MAX;
    double ymax = DBL_MIN;
    double xmax = DBL_MIN;

    for (int i = 0; i < shape->getShapeCount(); i++)
    {
        shape->getShape(i, object);
        bounds = object.getBounds();
        ymin = minValue(ymin, bounds.ymin);
        xmin = minValue(xmin, bounds.xmin);
        ymax = maxValue(ymax, bounds.ymax);
        xmax = maxValue(xmax, bounds.xmax);
    }

    xmin = floor(xmin);
    ymin = floor(ymin);
    header.cellSize = cellSize;
    header.llCorner->x = xmin;
    header.llCorner->y = ymin;
    header.nrRows = int(floor((ymax - ymin) / cellSize))+1;
    header.nrCols = int(floor((xmax - xmin) / cellSize))+1;

    raster->initializeGrid(header);
    return raster;
}


void fillRasterWithShapeNumber(gis::Crit3DRasterGrid* raster, Crit3DShapeHandler *shape)
{
    ShapeObject object;
    int value = NODATA;
    int nShape = shape->getShapeCount();


    for (int shapeIndex = 0; shapeIndex < nShape; shapeIndex++)
    {
        shape->getShape(shapeIndex, object);
        for (int row = 0; row < raster->header->nrRows; row++)
        {
            for (int col = 0; col < raster->header->nrCols; col++)
            {
                value = raster->value[row][col];
                if (value == raster->header->flag)
                {
                    gis::Crit3DUtmPoint* point = raster->utmPoint(row, col);
                    Point<double> UTMpoint;
                    UTMpoint.x = point->x;
                    UTMpoint.y = point->y;
                    value = object.pointInPolygon(UTMpoint);
                    if (value != NODATA)
                    {
                        raster->value[row][col] = value;
                    }
                }

            }
        }
    }

    return;
}


void fillRasterWithField(gis::Crit3DRasterGrid* raster, Crit3DShapeHandler* shape, std::string valField)
{
    ShapeObject object;
    int shapeFound = NODATA;
    int fieldIndex = shape->getDBFFieldIndex(valField.c_str());
    int nShape = shape->getShapeCount();
    DBFFieldType fieldType = shape->getFieldType(fieldIndex);


    for (int shapeIndex = 0; shapeIndex < nShape; shapeIndex++)
    {
        shape->getShape(shapeIndex, object);
        for (int row = 0; row < raster->header->nrRows; row++)
        {
            for (int col = 0; col < raster->header->nrCols; col++)
            {
                shapeFound = raster->value[row][col];
                if (shapeFound == raster->header->flag)
                {
                    gis::Crit3DUtmPoint* point = raster->utmPoint(row, col);
                    Point<double> UTMpoint;
                    UTMpoint.x = point->x;
                    UTMpoint.y = point->y;
                    shapeFound = object.pointInPolygon(UTMpoint);
                    if (shapeFound != NODATA)
                    {
                        if (fieldType == FTInteger)
                        {
                            int intValue = shape->readIntAttribute(shapeFound,fieldIndex);
                            raster->value[row][col] = intValue;
                        }
                        else if (fieldType == FTDouble)
                        {
                            double doubleValue = shape->readDoubleAttribute(shapeFound,fieldIndex);
                            raster->value[row][col] = float(doubleValue);
                        }
                    }
                }

            }
        }
    }


}

// TODO funzione fillRasterWithPrevailingShapeNumber con valore prevalente (nrSubdivision, minPercentage)
// shift = cellsize / (nrSubdivision +1)
// x primo punto = utmPoint.x - cellsize/2 + shift

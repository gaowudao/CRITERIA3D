#include "shapeToRaster.h"

#include "commonConstants.h"
#include <float.h>
#include <math.h>

gis::Crit3DRasterGrid* initializeRasterFromShape(Crit3DShapeHandler* shape, int cellSize)
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

    gis::Crit3DRasterGrid* raster = new(gis::Crit3DRasterGrid);
    raster->initializeGrid(header);
    return raster;
}


void fillRasterWithShapeNumber(gis::Crit3DRasterGrid* raster, Crit3DShapeHandler *shape)
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
            for (int shapeIndex = 0; shapeIndex < shape->getShapeCount(); shapeIndex++)
            {
                shape->getShape(shapeIndex, object);
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


void fillRasterWithField(gis::Crit3DRasterGrid* raster, Crit3DShapeHandler* shape, std::string valField)
{
    ShapeObject object;
    int shapeFound = NODATA;
    int fieldIndex = shape->getDBFFieldIndex(valField.c_str());
    DBFFieldType fieldType = shape->getFieldType(fieldIndex);

    for (int row = 0; row < raster->header->nrRows; row++)
    {
        for (int col = 0; col < raster->header->nrCols; col++)
        {
            gis::Crit3DUtmPoint* point = raster->utmPoint(row, col);
            Point<double> UTMpoint;
            UTMpoint.x = point->x;
            UTMpoint.y = point->y;
            for (int shapeIndex = 0; shapeIndex < shape->getShapeCount(); shapeIndex++)
            {
                shape->getShape(shapeIndex, object);
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

                    break;
                }
            }

        }
    }
    return;
}

// TODO funzione fillRasterWithPrevailingShapeNumber con valore prevalente (nrSubdivision, minPercentage)
// shift = cellsize / (nrSubdivision +1)
// x primo punto = utmPoint.x - cellsize/2 + shift

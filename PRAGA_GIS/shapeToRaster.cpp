#include "shapeToRaster.h"

#include "commonConstants.h"
#include <float.h>
#include <math.h>

#include <QtWidgets> // debug

gis::Crit3DRasterGrid* initializeRasterFromShape(Crit3DShapeHandler* shape, gis::Crit3DRasterGrid* raster, double cellSize)
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


void fillRasterWithShapeNumber(gis::Crit3DRasterGrid* raster, Crit3DShapeHandler *shapeHandler)
{
    ShapeObject object;
    double x, y;
    int nShape = shapeHandler->getShapeCount();

    for (int shapeIndex = 0; shapeIndex < nShape; shapeIndex++)
    {
        shapeHandler->getShape(shapeIndex, object);
        for (int row = 0; row < raster->header->nrRows; row++)
        {
            for (int col = 0; col < raster->header->nrCols; col++)
            {
                if (raster->value[row][col] == raster->header->flag)
                {
                    raster->getXY(row, col, &x, &y);
                    if (object.pointInPolygon(x, y))
                    {
                        raster->value[row][col] = shapeIndex;
                    }
                }
            }
        }
    }
}


void fillRasterWithField(gis::Crit3DRasterGrid* raster, Crit3DShapeHandler* shapeHandler, std::string valField)
{
    ShapeObject object;
    double x, y, fieldValue;
    int fieldIndex = shapeHandler->getDBFFieldIndex(valField.c_str());
    int nShape = shapeHandler->getShapeCount();
    DBFFieldType fieldType = shapeHandler->getFieldType(fieldIndex);

    for (int shapeIndex = 0; shapeIndex < nShape; shapeIndex++)
    {
        shapeHandler->getShape(shapeIndex, object);

        fieldValue = NODATA;
        if (fieldType == FTInteger)
        {
            fieldValue = shapeHandler->readIntAttribute(shapeIndex, fieldIndex);
        }
        else if (fieldType == FTDouble)
        {
            fieldValue = shapeHandler->readDoubleAttribute(shapeIndex, fieldIndex);
        }

        if (fieldValue != NODATA)
        {
            for (int row = 0; row < raster->header->nrRows; row++)
            {
                for (int col = 0; col < raster->header->nrCols; col++)
                {
                    if (raster->value[row][col] == raster->header->flag)
                    {
                        raster->getXY(row, col, &x, &y);
                        if (object.pointInPolygon(x, y))
                        {
                            raster->value[row][col] = float(fieldValue);
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

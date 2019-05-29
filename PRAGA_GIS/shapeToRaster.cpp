#include "shapeToRaster.h"

#include "commonConstants.h"
#include <float.h>
#include <math.h>

gis::Crit3DRasterGrid initializeRasterFromShape(Crit3DShapeHandler shape, int cellSize)
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
    header.nrRows = int(floor((ymax - ymin) / cellSize))+1;
    header.nrCols = int(floor((xmax - xmin) / cellSize))+1;

    gis::Crit3DRasterGrid raster;
    raster.initializeGrid(header);
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

}

// TODO funzione fillRasterWithPrevailingShapeNumber con valore prevalente (nrSubdivision, minPercentage)
// shift = cellsize / (nrSubdivision +1)
// x primo punto = utmPoint.x - cellsize/2 + shift

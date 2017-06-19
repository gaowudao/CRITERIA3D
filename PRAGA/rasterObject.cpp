#include <QtDebug>
#include "commonConstants.h"
#include "rasterObject.h"


extern Project myProject;

RasterObject::RasterObject(MapGraphicsView* view, MapGraphicsObject *parent) :
    MapGraphicsObject(true, parent)
{
    this->setFlag(MapGraphicsObject::ObjectIsSelectable, false);
    this->setFlag(MapGraphicsObject::ObjectIsMovable, false);
    this->setFlag(MapGraphicsObject::ObjectIsFocusable);
    _view = view;

    this->geoMap = new gis::Crit3DGeoMap();
    this->isDrawing = false;
    this->updateCenter();
}


void RasterObject::setDrawing(bool value)
{
 this->isDrawing = value;
}


void RasterObject::setColorLegend(ColorLegend* myLegend)
{
 this->legend = myLegend;
}


/*!
\brief You need to implement this. If sizeIsZoomInvariant() is true, this should return the size of the
 rectangle you want in PIXELS. If false, this should return the size of the rectangle in METERS. The
 rectangle should be centered at (0,0) regardless.
*/
 QRectF RasterObject::boundingRect() const
{
     return QRectF(- this->_view->width() *0.5,
                   - this->_view->height() *0.5,
                   this->_view->width(),
                   this->_view->height());
}


void RasterObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    this->setMapResolution();
    //TO DO scelta della mappa da visualizzare
    drawRaster(&myProject, &(myProject.DTM), painter);
    this->legend->update();
}


void RasterObject::updateCenter()
{
    QPointF newCenter = _view->mapToScene(QPoint(_view->width() * 0.5, _view->height() * 0.5));
    geoMap->referencePoint.latitude = newCenter.y();
    geoMap->referencePoint.longitude = newCenter.x();
    this->setPos(newCenter);
}


bool RasterObject::setMapResolution()
{
    QPointF bottomLeft = this->_view->mapToScene(QPoint(0.0, this->_view->height()));
    QPointF topRight = this->_view->mapToScene(QPoint(this->_view->width(),0.0));

    this->geoMap->bottomLeft.longitude = bottomLeft.x();
    this->geoMap->bottomLeft.latitude = bottomLeft.y();
    this->geoMap->topRight.longitude = topRight.x();
    this->geoMap->topRight.latitude = topRight.y();

    const qreal widthLon = qAbs<qreal>(topRight.x() - bottomLeft.x());
    const qreal heightlat = qAbs<qreal>(topRight.y() - bottomLeft.y());

    qreal dxdegree = widthLon / this->_view->width();
    qreal dydegree = heightlat / this->_view->height();

    this->geoMap->setResolution(dxdegree, dydegree);
    return true;
}


bool RasterObject::drawRaster(Project* myProject, gis::Crit3DRasterGrid *myRaster, QPainter* myPainter)
{
    if (! this->isDrawing) return false;
    if ( myProject == NULL) return false;
    if (! myRaster->isLoaded) return false;

    // current view extent
    long row0, row1, col0, col1;
    gis::getRowColFromXY(myProject->rowMatrix, this->geoMap->bottomLeft.longitude, this->geoMap->bottomLeft.latitude, &row0, &col0);
    gis::getRowColFromXY(myProject->rowMatrix, this->geoMap->topRight.longitude, this->geoMap->topRight.latitude, &row1, &col1);

    // check extent
    if (((col0 < 0) && (col1 < 0))
    || ((row0 < 0) && (row1 < 0))
    || ((col0 >= myProject->rowMatrix.header->nrCols) && (col1 >= myProject->rowMatrix.header->nrCols))
    || ((row0 >= myProject->rowMatrix.header->nrRows) && (row1 >= myProject->rowMatrix.header->nrRows)))
    {
        myRaster->minimum = NODATA;
        myRaster->maximum = NODATA;
        return false;
    }

    row1--;  //Serve veramente?
    row0 = std::min(myProject->rowMatrix.header->nrRows-1, std::max(long(0), row0));
    row1 = std::min(myProject->rowMatrix.header->nrRows-1, std::max(long(0), row1));
    col0 = std::min(myProject->rowMatrix.header->nrCols-1, std::max(long(0), col0));
    col1 = std::min(myProject->rowMatrix.header->nrCols-1, std::max(long(0), col1));

    // dynamic color scale
    gis::updateColorScale(myRaster, myProject->rowMatrix.value[row0][col0],
                                    myProject->rowMatrix.value[row1][col1],
                                    myProject->colMatrix.value[row0][col0],
                                    myProject->colMatrix.value[row1][col1]);

    gis::Crit3DGeoPoint llCorner;
    gis::Crit3DPixel pixelLL;
    llCorner.longitude = myProject->rowMatrix.header->llCorner->x + col0 * myProject->rowMatrix.header->cellSize;
    llCorner.latitude = myProject->rowMatrix.header->llCorner->y
            + (myProject->rowMatrix.header->nrRows-1 - row0) * myProject->rowMatrix.header->cellSize;
    pixelLL.x = (llCorner.longitude - this->geoMap->referencePoint.longitude) * this->geoMap->degreeToPixelX;
    pixelLL.y = (llCorner.latitude - this->geoMap->referencePoint.latitude) * this->geoMap->degreeToPixelY;

    double dx = myProject->rowMatrix.header->cellSize * this->geoMap->degreeToPixelX;
    double dy = myProject->rowMatrix.header->cellSize * this->geoMap->degreeToPixelY;
    int step = std::max(int(1. / (std::min(dx, dy))), 1);

    int x0, y0, x1, y1, lx, ly;
    long utmRow, utmCol;
    gis::Crit3DColor* myColor;
    QColor myQColor;
    float myValue;

    y0 = pixelLL.y;
    for (long row = row0; row >= row1; row -= step)
    {
        y1 = pixelLL.y + (row0-row + step) * dy;
        x0 = pixelLL.x;
        for (long col = col0; col <= col1; col += step)
        {
            x1 = pixelLL.x + (col-col0 + step) * dx;

            utmRow = myProject->rowMatrix.value[row][col];
            utmCol = myProject->colMatrix.value[row][col];
            myValue = myRaster->getValueFromRowCol(utmRow, utmCol);

            if (myValue != myRaster->header->flag)
            {
                myColor = myRaster->colorScale->getColor(myValue);
                myQColor = QColor(myColor->red, myColor->green, myColor->blue);
                myPainter->setBrush(myQColor);

                lx = (x1 - x0) +1;
                ly = (y1 - y0) +1;
                myPainter->fillRect(x0, y0, lx, ly, myPainter->brush());
            }
            x0 = ++x1;
        }
        y0 = ++y1;
    }

    return true;
}



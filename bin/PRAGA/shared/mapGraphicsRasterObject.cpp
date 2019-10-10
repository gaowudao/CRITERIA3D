/*!
    \file mapGraphicsRasterObject.cpp

    \abstract draw raster in MapGraphics widget

    This file is part of CRITERIA-3D distribution.

    CRITERIA-3D has been developed by A.R.P.A.E. Emilia-Romagna.

    \copyright
    CRITERIA-3D is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    CRITERIA-3D is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.
    You should have received a copy of the GNU Lesser General Public License
    along with CRITERIA-3D.  If not, see <http://www.gnu.org/licenses/>.

    \authors
    Fausto Tomei ftomei@arpae.it
    Gabriele Antolini gantolini@arpae.it
    Laura Costantini laura.costantini0@gmail.com
*/


#include "commonConstants.h"
#include "mapGraphicsRasterObject.h"
#include <math.h>

#define MAPBORDER 10


RasterObject::RasterObject(MapGraphicsView* _view, MapGraphicsObject *parent) :
    MapGraphicsObject(true, parent)
{
    this->setFlag(MapGraphicsObject::ObjectIsSelectable, false);
    this->setFlag(MapGraphicsObject::ObjectIsMovable, false);
    this->setFlag(MapGraphicsObject::ObjectIsFocusable, false);
    this->view = _view;

    this->matrix = nullptr;
    this->rasterPointer = nullptr;
    this->colorScaleLegend = nullptr;
    this->isLatLon = false;
    this->isGrid = false;
    this->geoMap = new gis::Crit3DGeoMap();
    this->referencePixel = QPointF(NODATA, NODATA);
    this->isDrawing = false;
    this->drawBorder = false;
}


void RasterObject::clear()
{
    setDrawing(false);
    setDrawBorders(false);
    freeIndexesMatrix();
    this->latLonHeader.nrCols = 0;
    this->latLonHeader.nrRows = 0;
    this->colorScaleLegend = nullptr;
}


void RasterObject::setDrawing(bool value)
{
    this->isDrawing = value;
}


void RasterObject::setDrawBorders(bool value)
{
    this->drawBorder = value;
}


void RasterObject::setColorLegend(ColorLegend* myLegend)
{
    this->colorScaleLegend = myLegend;
}


QPointF RasterObject::getPixel(const QPointF &latLonPoint)
{
    QPointF pixel = this->view->tileSource()->ll2qgs(latLonPoint, this->view->zoomLevel());
    pixel.setX(pixel.x() - this->referencePixel.x());
    pixel.setY(this->referencePixel.y() - pixel.y());
    return pixel;
}


/*!
\brief If sizeIsZoomInvariant() is true, this should return the size of the
 rectangle you want in PIXELS. If false, this should return the size of the rectangle in METERS. The
 rectangle should be centered at (0,0) regardless.
*/
 QRectF RasterObject::boundingRect() const
{
    int widthPixels = this->view->width() - MAPBORDER*2;
    int heightPixels = this->view->height() - MAPBORDER*2;

    return QRectF( -widthPixels, -heightPixels, widthPixels*2, heightPixels*2);
}


void RasterObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    if (this->isDrawing)
    {
        setMapExtents();

        if (this->rasterPointer != nullptr)
            drawRaster(this->rasterPointer, painter, this->drawBorder);

        if (this->colorScaleLegend != nullptr)
            this->colorScaleLegend->update();
    }
}


void RasterObject::updateCenter()
{
    if (! isDrawing) return;

    QPointF newCenter;
    int widthPixels = view->width() - MAPBORDER*2;
    int heightPixels = view->height() - MAPBORDER*2;
    newCenter = view->mapToScene(QPoint(widthPixels/2, heightPixels/2));

    // reference point
    referencePixel = view->tileSource()->ll2qgs(newCenter, view->zoomLevel());

    if (isDrawing) setPos(newCenter);
}


void RasterObject::setRaster(gis::Crit3DRasterGrid* rasterPtr)
{
    this->rasterPointer = rasterPtr;
}


gis::Crit3DRasterGrid* RasterObject::getRaster()
{
    return this->rasterPointer;
}


/*!
 * \brief RasterObject::getRasterMaxSize
 * \return max of raster width and height (decimal degree)
 */
float RasterObject::getRasterMaxSize()
{
    return float(MAXVALUE(this->latLonHeader.nrRows * this->latLonHeader.dy,
                          this->latLonHeader.nrCols * this->latLonHeader.dx));
}


/*!
 * \brief RasterObject::getRasterCenter
 * \return center of raster (lat lon)
 */
gis::Crit3DGeoPoint* RasterObject::getRasterCenter()
{
    gis::Crit3DGeoPoint* center = new(gis::Crit3DGeoPoint);
    center->latitude = this->latLonHeader.llCorner->latitude + (this->latLonHeader.nrRows * this->latLonHeader.dy) * 0.5;
    center->longitude = this->latLonHeader.llCorner->longitude + (this->latLonHeader.nrCols * this->latLonHeader.dx) * 0.5;
    return center;
}


void RasterObject::freeIndexesMatrix()
{
    if (this->matrix == nullptr) return;

    for (int row = 0; row < this->latLonHeader.nrRows; row++)
        if (this->matrix[row] != nullptr) ::free(this->matrix[row]);

    if (this->latLonHeader.nrRows != 0) ::free(this->matrix);

    this->matrix = nullptr;
}


void RasterObject::initializeIndexesMatrix()
{
    this->matrix = new RowCol*[unsigned(latLonHeader.nrRows)];

    for (int row = 0; row < this->latLonHeader.nrRows; row++)
        this->matrix[row] = new RowCol[unsigned(latLonHeader.nrCols)];

    for (int row = 0; row < this->latLonHeader.nrRows; row++)
        for (int col = 0; col < this->latLonHeader.nrCols; col++)
        {
            this->matrix[row][col].row = NODATA;
            this->matrix[row][col].col = NODATA;
        }
}


void RasterObject::setMapExtents()
{
    int widthPixels = view->width() - MAPBORDER*2;
    int heightPixels = view->height() - MAPBORDER*2;
    QPointF botLeft = view->mapToScene(QPoint(0, heightPixels));
    QPointF topRight = view->mapToScene(QPoint(widthPixels, 0));

    geoMap->bottomLeft.longitude = MAXVALUE(-180, botLeft.x());
    geoMap->bottomLeft.latitude = MAXVALUE(-84, botLeft.y());
    geoMap->topRight.longitude = MINVALUE(180, topRight.x());
    geoMap->topRight.latitude = MINVALUE(84, topRight.y());
}


bool RasterObject::initializeUTM(gis::Crit3DRasterGrid* myRaster, const gis::Crit3DGisSettings& gisSettings, bool isGrid_)
{
    if (myRaster == nullptr) return false;
    if (! myRaster->isLoaded) return false;

    double lat, lon, x, y;
    int utmRow, utmCol;

    isLatLon = false;

    isGrid = isGrid_;
    utmZone = gisSettings.utmZone;
    rasterPointer = myRaster;

    freeIndexesMatrix();
    gis::getGeoExtentsFromUTMHeader(gisSettings, myRaster->header, &latLonHeader);
    initializeIndexesMatrix();

    for (int row = 0; row < latLonHeader.nrRows; row++)
        for (int col = 0; col < latLonHeader.nrCols; col++)
        {
            gis::getLatLonFromRowCol(latLonHeader, row, col, &lat, &lon);
            gis::latLonToUtmForceZone(gisSettings.utmZone, lat, lon, &x, &y);
            if (! gis::isOutOfGridXY(x, y, myRaster->header))
            {
                gis::getRowColFromXY(*(myRaster->header), x, y, &utmRow, &utmCol);
                if (isGrid || myRaster->getValueFromRowCol(utmRow, utmCol) != myRaster->header->flag)
                {
                    matrix[row][col].row = utmRow;
                    matrix[row][col].col = utmCol;
                }
            }
        }

    setDrawing(true);
    setDrawBorders(isGrid);

    return true;
}


bool RasterObject::initializeLatLon(gis::Crit3DRasterGrid* myRaster, const gis::Crit3DGisSettings& gisSettings, const gis::Crit3DGridHeader &latLonHeader_, bool isGrid_)
{
    if (myRaster == nullptr) return false;
    if (! myRaster->isLoaded) return false;

    this->isLatLon = true;

    this->isGrid = isGrid_;
    this->utmZone = gisSettings.utmZone;
    this->rasterPointer = myRaster;

    freeIndexesMatrix();

    this->latLonHeader = latLonHeader_;

    setDrawing(true);
    setDrawBorders(isGrid_);
    return true;
}


bool RasterObject::drawRaster(gis::Crit3DRasterGrid *myRaster, QPainter* myPainter, bool drawBorder)
{
    if (myRaster == nullptr) return false;
    if (! myRaster->isLoaded) return false;

    // current view extent
    int rowBottom, rowTop, colLeft, colRight;
    gis::getRowColFromLatLon(this->latLonHeader, this->geoMap->bottomLeft, &rowBottom, &colLeft);
    gis::getRowColFromLatLon(this->latLonHeader, this->geoMap->topRight, &rowTop, &colRight);

    // check if current view is out of data
    if (((colLeft < 0) && (colRight < 0))
    || ((rowBottom < 0) && (rowTop < 0))
    || ((colLeft >= this->latLonHeader.nrCols) && (colRight >= this->latLonHeader.nrCols))
    || ((rowBottom >= this->latLonHeader.nrRows) && (rowTop >= this->latLonHeader.nrRows)))
    {
        myRaster->minimum = NODATA;
        myRaster->maximum = NODATA;
        return false;
    }

    // fix extent
    rowBottom = std::min(this->latLonHeader.nrRows-1, std::max(0, rowBottom));
    rowTop = std::min(this->latLonHeader.nrRows-1, std::max(0, rowTop));
    colLeft = std::min(this->latLonHeader.nrCols-1, std::max(0, colLeft));
    colRight = std::min(this->latLonHeader.nrCols-1, std::max(0, colRight));

    // dynamic color scale
    gis::Crit3DRasterWindow* latLonWindow = new gis::Crit3DRasterWindow(rowTop, colLeft, rowBottom, colRight);
    if (this->isLatLon)
    {
        gis::updateColorScale(myRaster, *latLonWindow);
    }
    else
    {
        // UTM raster
        gis::Crit3DRasterWindow* utmWindow = new gis::Crit3DRasterWindow();
        gis::getUtmWindow(this->latLonHeader, *(myRaster->header), *latLonWindow, utmWindow, this->utmZone);
        gis::updateColorScale(myRaster, *utmWindow);
    }

    roundColorScale(myRaster->colorScale, 4, true);

    // boundary pixels position
    QPointF lowerLeft, topRight, pixelLL, pixelRT;
    lowerLeft.setX(latLonHeader.llCorner->longitude + colLeft * latLonHeader.dx);
    lowerLeft.setY(latLonHeader.llCorner->latitude + (latLonHeader.nrRows-1 - rowBottom) * latLonHeader.dy);
    topRight.setX(latLonHeader.llCorner->longitude + (colRight+1) * latLonHeader.dx);
    topRight.setY(latLonHeader.llCorner->latitude + (latLonHeader.nrRows - rowTop) * latLonHeader.dy);
    pixelLL = getPixel(lowerLeft);
    pixelRT = getPixel(topRight);

    // step
    double dx = (pixelRT.x() - pixelLL.x()) / double(colRight - colLeft +1);
    double dy = (pixelRT.y() - pixelLL.y()) / double(rowBottom - rowTop +1);
    int step = int(round(1.0 / MINVALUE(dx, dy)));
    step = MAXVALUE(1, step);

    // draw
    int x0, y0, x1, y1, lx, ly;
    float value;
    QPointF newPoint, newPixel;
    Crit3DColor* myColor;
    QColor myQColor;

    newPoint.setX(lowerLeft.x());
    y0 = int(pixelLL.y());
    for (int row = rowBottom; row >= rowTop; row -= step)
    {
        newPoint.setY(lowerLeft.y() + (rowBottom-row + step) * latLonHeader.dy);
        newPixel = getPixel(newPoint);
        y1 = int(newPixel.y());
        x0 = int(pixelLL.x());

        for (int col = colLeft; col <= colRight; col += step)
        {
            x1 = int(pixelLL.x() + (col-colLeft + step) * dx);

            if (this->isLatLon)
                value = myRaster->value[row][col];
            else
            {
                value = INDEX_ERROR;
                if (this->matrix[row][col].row != NODATA)
                    value = myRaster->value[matrix[row][col].row][matrix[row][col].col];
            }
            if (int(value) != int(myRaster->header->flag) && int(value) != int(INDEX_ERROR))
            {
                myColor = myRaster->colorScale->getColor(value);
                myQColor = QColor(myColor->red, myColor->green, myColor->blue);
                myPainter->setBrush(myQColor);

                lx = (x1 - x0) + 1;
                ly = (y1 - y0) + 1;
                myPainter->fillRect(x0, y0, lx, ly, myPainter->brush());
            }
            else if (this->isGrid && value == myRaster->header->flag && drawBorder)
            {
                lx = (x1 - x0) + 1;
                ly = (y1 - y0) + 1;
                myPainter->setPen(QColor(64, 64, 64));
                myPainter->setBrush(Qt::NoBrush);
                myPainter->drawRect(x0, y0, lx, ly);
            }

            x0 = ++x1;
        }
        y0 = ++y1;
    }

    return true;
}


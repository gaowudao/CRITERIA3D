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
    this->isDrawing = false;
    this->drawBorder = false;
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


void RasterObject::clear()
{
    setDrawing(false);
    setDrawBorders(false);
    freeIndexesMatrix();
    this->latLonHeader.nrCols = 0;
    this->latLonHeader.nrRows = 0;
    this->colorScaleLegend = nullptr;
}


/*!
\brief If sizeIsZoomInvariant() is true, this should return the size of the
 rectangle you want in PIXELS. If false, this should return the size of the rectangle in METERS. The
 rectangle should be centered at (0,0) regardless.
*/
 QRectF RasterObject::boundingRect() const
{
     return QRectF( -this->view->width() * 1.0, -this->view->height() * 1.0,
                     this->view->width() * 2.0,  this->view->height() * 2.0);
}


void RasterObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    if (this->isDrawing)
    {
        setMapResolution();

        if (this->rasterPointer != nullptr)
            drawRaster(this->rasterPointer, painter, this->drawBorder);

        if (this->colorScaleLegend != nullptr)
            this->colorScaleLegend->update();
    }
}


void RasterObject::setCenter()
{
    QPointF sceneCenter = view->mapToScene(QPoint(view->width()/2, view->height()/2));

    QPointF rasterCenter, center;
    rasterCenter.setX(latLonHeader.llCorner->longitude + latLonHeader.dx * latLonHeader.nrCols*0.5);
    rasterCenter.setY(latLonHeader.llCorner->latitude + latLonHeader.dy * latLonHeader.nrRows*0.5);

    if (fabs(sceneCenter.x()-rasterCenter.x()) <  latLonHeader.dx * latLonHeader.nrCols
        && fabs(sceneCenter.y()-rasterCenter.y()) <  latLonHeader.dy * latLonHeader.nrRows)
    {
        center = sceneCenter;
    }
    else
    {
        center = rasterCenter;
    }

    this->geoMap->referencePoint.longitude = center.x();
    this->geoMap->referencePoint.latitude = center.y();

    this->setPos(center);
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


void RasterObject::setMapResolution()
{
    QPointF bottomLeft = this->view->mapToScene(QPoint(0.f, this->view->height()));
    QPointF topRight = this->view->mapToScene(QPoint(this->view->width(), 0.f));

    this->geoMap->bottomLeft.longitude = bottomLeft.x();
    this->geoMap->bottomLeft.latitude = bottomLeft.y();
    this->geoMap->topRight.longitude = topRight.x();
    this->geoMap->topRight.latitude = topRight.y();

    double widthLon = topRight.x() - bottomLeft.x();
    double heightlat = topRight.y() - bottomLeft.y();

    double dxdegree = widthLon / this->view->width();
    double dydegree = heightlat / this->view->height();

    this->geoMap->setResolution(dxdegree, dydegree);
}


bool RasterObject::initializeUTM(gis::Crit3DRasterGrid* myRaster, const gis::Crit3DGisSettings& gisSettings, bool isGrid_)
{
    if (myRaster == nullptr) return false;
    if (! myRaster->isLoaded) return false;

    double lat, lon, x, y;
    int utmRow, utmCol;

    this->isLatLon = false;

    this->isGrid = isGrid_;
    this->utmZone = gisSettings.utmZone;
    this->rasterPointer = myRaster;

    freeIndexesMatrix();
    gis::getGeoExtentsFromUTMHeader(gisSettings, myRaster->header, &latLonHeader);
    initializeIndexesMatrix();

    for (int row = 0; row < this->latLonHeader.nrRows; row++)
        for (int col = 0; col < this->latLonHeader.nrCols; col++)
        {
            gis::getLatLonFromRowCol(this->latLonHeader, row, col, &lat, &lon);
            gis::latLonToUtmForceZone(gisSettings.utmZone, lat, lon, &x, &y);
            if (! gis::isOutOfGridXY(x, y, myRaster->header))
            {
                gis::getRowColFromXY(*(myRaster->header), x, y, &utmRow, &utmCol);
                if (this->isGrid || myRaster->getValueFromRowCol(utmRow, utmCol) != myRaster->header->flag)
                {
                    this->matrix[row][col].row = utmRow;
                    this->matrix[row][col].col = utmCol;
                }
            }
        }

    setDrawing(true);
    setDrawBorders(this->isGrid);
    return true;
}


bool RasterObject::initializeLatLon(gis::Crit3DRasterGrid* myRaster, const gis::Crit3DGisSettings& gisSettings, const gis::Crit3DGridHeader& latLonHeader_, bool isGrid_)
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
    int rowBottom, rowTop, col0, col1;
    gis::getRowColFromLatLon(this->latLonHeader, this->geoMap->bottomLeft, &rowBottom, &col0);
    gis::getRowColFromLatLon(this->latLonHeader, this->geoMap->topRight, &rowTop, &col1);
    rowTop--;

    // check if current view is out of data
    if (((col0 < 0) && (col1 < 0))
    || ((rowBottom < 0) && (rowTop < 0))
    || ((col0 >= this->latLonHeader.nrCols) && (col1 >= this->latLonHeader.nrCols))
    || ((rowBottom >= this->latLonHeader.nrRows) && (rowTop >= this->latLonHeader.nrRows)))
    {
        myRaster->minimum = NODATA;
        myRaster->maximum = NODATA;
        return false;
    }

    // fix extent
    rowBottom = std::min(this->latLonHeader.nrRows-1, std::max(0, rowBottom));
    rowTop = std::min(this->latLonHeader.nrRows-1, std::max(0, rowTop));
    col0 = std::min(this->latLonHeader.nrCols-1, std::max(0, col0));
    col1 = std::min(this->latLonHeader.nrCols-1, std::max(0, col1));

    // dynamic color scale
    gis::Crit3DRasterWindow* latLonWindow = new gis::Crit3DRasterWindow(rowTop, col0, rowBottom, col1);
    if (this->isLatLon)
        gis::updateColorScale(myRaster, *latLonWindow);
    else
    {
        // UTM raster
        gis::Crit3DRasterWindow* utmWindow = new gis::Crit3DRasterWindow();
        gis::getUtmWindow(this->latLonHeader, *(myRaster->header), *latLonWindow, utmWindow, this->utmZone);
        gis::updateColorScale(myRaster, *utmWindow);
    }

    roundColorScale(myRaster->colorScale, 4, true);

    // lower left position
    gis::Crit3DGeoPoint llCorner;
    gis::Crit3DPixel pixelLL;
    llCorner.longitude =this->latLonHeader.llCorner->longitude + col0 * this->latLonHeader.dx;
    llCorner.latitude = this->latLonHeader.llCorner->latitude + (this->latLonHeader.nrRows-1 - rowBottom) * this->latLonHeader.dy;
    pixelLL.x = int(floor(this->geoMap->degreeToPixelX * (llCorner.longitude - this->geoMap->referencePoint.longitude)));
    pixelLL.y = int(floor(this->geoMap->degreeToPixelY * (llCorner.latitude - this->geoMap->referencePoint.latitude)));

    double dx = latLonHeader.dx * this->geoMap->degreeToPixelX;
    double dy = latLonHeader.dy * this->geoMap->degreeToPixelY;
    int step = std::max(int(1.0/(std::min(dx, dy))), 1);

    int x0, y0, x1, y1, lx, ly;
    Crit3DColor* myColor;
    QColor myQColor;
    float value;

    y0 = pixelLL.y;
    for (int row = rowBottom; row >= rowTop; row -= step)
    {
        y1 = int(pixelLL.y + (rowBottom-row + step) * dy);
        x0 = pixelLL.x;

        for (int col = col0; col <= col1; col += step)
        {
            x1 = int(pixelLL.x + (col-col0 + step) * dx);

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


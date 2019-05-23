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


RasterObject::RasterObject(MapGraphicsView* _view, MapGraphicsObject *parent) :
    MapGraphicsObject(true, parent)
{
    this->setFlag(MapGraphicsObject::ObjectIsSelectable, false);
    this->setFlag(MapGraphicsObject::ObjectIsMovable, false);
    this->setFlag(MapGraphicsObject::ObjectIsFocusable);
    this->view = _view;

    this->matrix = nullptr;
    this->currentRaster = nullptr;
    this->legend = nullptr;
    this->isLatLon = false;
    this->isGrid = false;
    this->geoMap = new gis::Crit3DGeoMap();
    this->isDrawing = false;
    this->drawBorder = false;
    this->updateCenter();
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
 this->legend = myLegend;
}


void RasterObject::clean()
{
    setDrawing(false);
    setDrawBorders(false);
    freeIndexesMatrix();
    this->latLonHeader.nrCols = 0;
    this->latLonHeader.nrRows = 0;
    this->legend = nullptr;
}


/*!
\brief If sizeIsZoomInvariant() is true, this should return the size of the
 rectangle you want in PIXELS. If false, this should return the size of the rectangle in METERS. The
 rectangle should be centered at (0,0) regardless.
*/
 QRectF RasterObject::boundingRect() const
{
     return QRectF( -this->view->width() * 0.6, -this->view->height() * 0.6,
                     this->view->width() * 1.2,  this->view->height() * 1.2);
}


void RasterObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    if (this->isDrawing)
    {
        setMapResolution();

        if (this->currentRaster != nullptr)
            drawRaster(this->currentRaster, painter, this->drawBorder);

        if (this->legend != nullptr)
            this->legend->repaint();
    }
}


void RasterObject::updateCenter()
{
    QPointF newCenter = this->view->mapToScene(QPoint(int(view->width() * 0.5), int(view->height() * 0.5)));
    this->geoMap->referencePoint.latitude = newCenter.y();
    this->geoMap->referencePoint.longitude = newCenter.x();
    this->setPos(newCenter);
}


void RasterObject::setCurrentRaster(gis::Crit3DRasterGrid* rasterPointer)
{
    this->currentRaster = rasterPointer;
}


/*!
 * \brief RasterObject::getRasterMaxSize
 * \return max of raster width and height (decimal degree)
 */
float RasterObject::getRasterMaxSize()
{
    return float(maxValue(this->latLonHeader.nrRows * this->latLonHeader.dy,
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
    this->matrix = (RowCol **) calloc(unsigned(this->latLonHeader.nrRows), sizeof(RowCol *));

    for (int row = 0; row < this->latLonHeader.nrRows; row++)
        this->matrix[row] = (RowCol *) calloc(unsigned(this->latLonHeader.nrCols), sizeof(RowCol));

    for (int row = 0; row < this->latLonHeader.nrRows; row++)
        for (int col = 0; col < this->latLonHeader.nrCols; col++)
        {
            this->matrix[row][col].row = NODATA_UNSIGNED_SHORT;
            this->matrix[row][col].col = NODATA_UNSIGNED_SHORT;
        }
}


bool RasterObject::setMapResolution()
{
    QPointF bottomLeft = this->view->mapToScene(QPoint(0.0, this->view->height()));
    QPointF topRight = this->view->mapToScene(QPoint(this->view->width(),0.0));

    this->geoMap->bottomLeft.longitude = bottomLeft.x();
    this->geoMap->bottomLeft.latitude = bottomLeft.y();
    this->geoMap->topRight.longitude = topRight.x();
    this->geoMap->topRight.latitude = topRight.y();

    const qreal widthLon = qAbs<qreal>(topRight.x() - bottomLeft.x());
    const qreal heightlat = qAbs<qreal>(topRight.y() - bottomLeft.y());

    qreal dxdegree = widthLon / this->view->width();
    qreal dydegree = heightlat / this->view->height();

    this->geoMap->setResolution(dxdegree, dydegree);
    return true;
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
    this->currentRaster = myRaster;

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
                    this->matrix[row][col].row = unsigned(utmRow);
                    this->matrix[row][col].col = unsigned(utmCol);
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
    this->currentRaster = myRaster;

    freeIndexesMatrix();

    this->latLonHeader = latLonHeader_;

    setDrawing(true);
    setDrawBorders(isGrid_);
    return true;
}


bool RasterObject::drawRaster(gis::Crit3DRasterGrid *myRaster, QPainter* myPainter, bool drawBorder)
{
    if (myRaster == nullptr) return false;
    if (! this->isDrawing) return false;
    if (! myRaster->isLoaded) return false;

    // current view extent
    int rowBottom, rowTop, col0, col1;
    gis::getRowColFromLatLon(latLonHeader, this->geoMap->bottomLeft, &rowBottom, &col0);
    gis::getRowColFromLatLon(latLonHeader, this->geoMap->topRight, &rowTop, &col1);
    rowTop--;

    // check if current view is out of data
    if (((col0 < 0) && (col1 < 0))
    || ((rowBottom < 0) && (rowTop < 0))
    || ((col0 >= latLonHeader.nrCols) && (col1 >= latLonHeader.nrCols))
    || ((rowBottom >= latLonHeader.nrRows) && (rowTop >= latLonHeader.nrRows)))
    {
        myRaster->minimum = NODATA;
        myRaster->maximum = NODATA;
        return false;
    }

    // fix extent
    rowBottom = std::min(latLonHeader.nrRows-1, std::max(0, rowBottom));
    rowTop = std::min(latLonHeader.nrRows-1, std::max(0, rowTop));
    col0 = std::min(latLonHeader.nrCols-1, std::max(0, col0));
    col1 = std::min(latLonHeader.nrCols-1, std::max(0, col1));

    // dynamic color scale
    gis::Crit3DRasterWindow* latLonWindow = new gis::Crit3DRasterWindow(rowTop, col0, rowBottom, col1);
    if (isLatLon)
        gis::updateColorScale(myRaster, *latLonWindow);
    else
    {
        // UTM raster
        gis::Crit3DRasterWindow* utmWindow = new gis::Crit3DRasterWindow();
        gis::getUtmWindow(latLonHeader, *(myRaster->header), *latLonWindow, utmWindow, this->utmZone);
        gis::updateColorScale(myRaster, *utmWindow);
    }

    roundColorScale(myRaster->colorScale, 4, true);

    // lower left position
    gis::Crit3DGeoPoint llCorner;
    gis::Crit3DPixel pixelLL;
    llCorner.longitude = latLonHeader.llCorner->longitude + col0 * latLonHeader.dx;
    llCorner.latitude = latLonHeader.llCorner->latitude + (latLonHeader.nrRows-1 - rowBottom) * latLonHeader.dy;
    pixelLL.x = int(this->geoMap->degreeToPixelX * (llCorner.longitude - this->geoMap->referencePoint.longitude));
    pixelLL.y = int(this->geoMap->degreeToPixelY * (llCorner.latitude - this->geoMap->referencePoint.latitude));

    double dx = latLonHeader.dx * this->geoMap->degreeToPixelX;
    double dy = latLonHeader.dy * this->geoMap->degreeToPixelY;
    int step = std::max(int(1.0/(std::min(dx, dy))), 1);

    int x0, y0, x1, y1, lx, ly;
    Crit3DColor* myColor;
    QColor myQColor;
    float myValue;

    y0 = pixelLL.y;
    for (int row = rowBottom; row >= rowTop; row -= step)
    {
        y1 = int(pixelLL.y + (rowBottom-row + step) * dy);
        x0 = pixelLL.x;
        for (int col = col0; col <= col1; col += step)
        {
            x1 = int(pixelLL.x + (col-col0 + step) * dx);

            if (isLatLon)
                myValue = myRaster->value[row][col];
            else
            {
                myValue = INDEX_ERROR;
                if (matrix[row][col].row != NODATA_UNSIGNED_SHORT)
                    myValue = myRaster->value[matrix[row][col].row][matrix[row][col].col];
            }

            if (myValue != myRaster->header->flag && myValue != INDEX_ERROR)
            {
                myColor = myRaster->colorScale->getColor(myValue);
                myQColor = QColor(myColor->red, myColor->green, myColor->blue);
                myPainter->setBrush(myQColor);

                lx = (x1 - x0) + 1;
                ly = (y1 - y0) + 1;
                myPainter->fillRect(x0, y0, lx, ly, myPainter->brush());

            }
            else if (isGrid && myValue == myRaster->header->flag && drawBorder)
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


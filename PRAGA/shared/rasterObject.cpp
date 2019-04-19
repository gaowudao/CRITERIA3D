#include "commonConstants.h"
#include "rasterObject.h"


RasterObject::RasterObject(MapGraphicsView* view, MapGraphicsObject *parent) :
    MapGraphicsObject(true, parent)
{
    this->setFlag(MapGraphicsObject::ObjectIsSelectable, false);
    this->setFlag(MapGraphicsObject::ObjectIsMovable, false);
    this->setFlag(MapGraphicsObject::ObjectIsFocusable);
    _view = view;

    matrix = nullptr;
    currentRaster = nullptr;
    legend = nullptr;
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
    latLonHeader.nrCols = 0;
    latLonHeader.nrRows = 0;
    legend = nullptr;
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

    if (isDrawing)
    {
        setMapResolution();

        if (currentRaster != nullptr)
            drawRaster(currentRaster, painter, drawBorder);

        if (legend != nullptr)
            legend->update();
    }
}


void RasterObject::updateCenter()
{
    QPointF newCenter = _view->mapToScene(QPoint(int(_view->width() * 0.5), int(_view->height() * 0.5)));
    geoMap->referencePoint.latitude = newCenter.y();
    geoMap->referencePoint.longitude = newCenter.x();
    this->setPos(newCenter);
}


void RasterObject::setCurrentRaster(gis::Crit3DRasterGrid* rasterPointer)
{
    currentRaster = rasterPointer;
}


/*!
 * \brief RasterObject::getRasterMaxSize
 * \return max of raster width and height (decimal degree)
 */
float RasterObject::getRasterMaxSize()
{
    return float(maxValue(latLonHeader.nrRows * latLonHeader.dy, latLonHeader.nrCols * latLonHeader.dx));
}


/*!
 * \brief RasterObject::getRasterCenter
 * \return center of raster (lat lon)
 */
gis::Crit3DGeoPoint* RasterObject::getRasterCenter()
{
    gis::Crit3DGeoPoint* center = new(gis::Crit3DGeoPoint);
    center->latitude = latLonHeader.llCorner->latitude + (latLonHeader.nrRows * latLonHeader.dy) * 0.5;
    center->longitude = latLonHeader.llCorner->longitude + (latLonHeader.nrCols * latLonHeader.dx) * 0.5;
    return center;
}


void RasterObject::freeIndexesMatrix()
{
    if (matrix == nullptr) return;

    for (int row = 0; row < latLonHeader.nrRows; row++)
        if (matrix[row] != nullptr) ::free(matrix[row]);

    if (latLonHeader.nrRows != 0) ::free(matrix);

    matrix = nullptr;
}


void RasterObject::initializeIndexesMatrix()
{
    matrix = (RowCol **) calloc(unsigned(latLonHeader.nrRows), sizeof(RowCol *));

    for (int row = 0; row < latLonHeader.nrRows; row++)
        matrix[row] = (RowCol *) calloc(unsigned(this->latLonHeader.nrCols), sizeof(RowCol));

    for (int row = 0; row < latLonHeader.nrRows; row++)
        for (int col = 0; col < latLonHeader.nrCols; col++)
        {
            matrix[row][col].row = NODATA_UNSIGNED_SHORT;
            matrix[row][col].col = NODATA_UNSIGNED_SHORT;
        }
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


bool RasterObject::initializeUTM(gis::Crit3DRasterGrid* myRaster, const gis::Crit3DGisSettings& gisSettings, bool isGrid_)
{
    if (myRaster == nullptr) return false;
    if (! myRaster->isLoaded) return false;

    double lat, lon, x, y;
    int utmRow, utmCol;

    isLatLon = false;

    isGrid = isGrid_;
    utmZone = gisSettings.utmZone;
    currentRaster = myRaster;

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
                    matrix[row][col].row = unsigned(utmRow);
                    matrix[row][col].col = unsigned(utmCol);
                }
            }
        }

    setDrawing(true);
    setDrawBorders(isGrid);
    return true;
}


bool RasterObject::initializeLatLon(gis::Crit3DRasterGrid* myRaster, const gis::Crit3DGisSettings& gisSettings, const gis::Crit3DGridHeader& latLonHeader_, bool isGrid_)
{
    if (myRaster == nullptr) return false;
    if (! myRaster->isLoaded) return false;

    isLatLon = true;

    isGrid = isGrid_;
    utmZone = gisSettings.utmZone;
    currentRaster = myRaster;

    freeIndexesMatrix();

    latLonHeader = latLonHeader_;

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
    int step = std::max(int(1. / (std::min(dx, dy))), 1);

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

                lx = (x1 - x0) +1;
                ly = (y1 - y0) +1;
                myPainter->fillRect(x0, y0, lx, ly, myPainter->brush());

            }
            else if (isGrid && myValue == myRaster->header->flag && drawBorder)
            {
                lx = (x1 - x0) +1;
                ly = (y1 - y0) +1;
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


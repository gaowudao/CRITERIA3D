#include "mapGraphicsShapeObject.h"


MapGraphicsShapeObject::MapGraphicsShapeObject(MapGraphicsView* _view, MapGraphicsObject *parent) :
    MapGraphicsObject(true, parent)
{
    this->setFlag(MapGraphicsObject::ObjectIsSelectable, false);
    this->setFlag(MapGraphicsObject::ObjectIsMovable, false);
    this->setFlag(MapGraphicsObject::ObjectIsFocusable);
    view = _view;

    this->geoMap = new gis::Crit3DGeoMap();
    this->isDrawing = false;
    this->shapePointer = nullptr;
    this->updateCenter();
}


/*!
\brief If sizeIsZoomInvariant() is true, this should return the size of the
 rectangle you want in PIXELS. If false, this should return the size of the rectangle in METERS. The
 rectangle should be centered at (0,0) regardless.
*/
QRectF MapGraphicsShapeObject::boundingRect() const
{
     return QRectF( -this->view->width() * 0.6, -this->view->height() * 0.6,
                     this->view->width() * 1.2,  this->view->height() * 1.2);
}


void MapGraphicsShapeObject::updateCenter()
{
     QPointF newCenter = this->view->mapToScene(QPoint(int(view->width() * 0.5), int(view->height() * 0.5)));
     this->geoMap->referencePoint.latitude = newCenter.y();
     this->geoMap->referencePoint.longitude = newCenter.x();
     this->setPos(newCenter);
}


void MapGraphicsShapeObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    if (this->isDrawing)
    {
        setMapResolution();

        if (this->shapePointer != nullptr)
            drawShape(painter);
    }
}


gis::Crit3DPixel MapGraphicsShapeObject::getPixel(const gis::Crit3DGeoPoint& geoPoint)
{
    gis::Crit3DPixel pixel;
    pixel.x = int((geoPoint.longitude - this->geoMap->referencePoint.longitude) * this->geoMap->degreeToPixelX);
    pixel.y = int((geoPoint.latitude - this->geoMap->referencePoint.latitude) * this->geoMap->degreeToPixelY);
    return pixel;
}


void MapGraphicsShapeObject::drawShape(QPainter* myPainter)
{
    myPainter->setBrush(Qt::black);
    gis::Crit3DPixel p1, p2;

    unsigned long nrShapes = unsigned(this->geoPoints.size());
    for (unsigned int i = 0; i < nrShapes; i++)
    {
        p1 = getPixel(this->geoPoints[i][0]);
        unsigned long nrVertices = unsigned(this->geoPoints[i].size());
        for (unsigned long j = 1; j < nrVertices; j++)
        {
            p2 = getPixel(this->geoPoints[i][j]);
            myPainter->drawLine(p1.x, p1.y, p2.x, p2.y);
            p1 = p2;
        }
    }
}


bool MapGraphicsShapeObject::initializeUTM(Crit3DShapeHandler* shapePtr, const gis::Crit3DGisSettings& gisSettings)
{
    if (shapePtr == nullptr) return false;
    this->setShape(shapePtr);

    double lat, lon;
    ShapeObject myShape;

    unsigned int nrShapes = unsigned(shapePtr->getShapeCount());
    this->geoPoints.resize(nrShapes);

    for (unsigned int i = 0; i < nrShapes; i++)
    {
        shapePtr->getShape(int(i), myShape);
        unsigned long nrVertices = myShape.getVertexCount();
        this->geoPoints[i].resize(nrVertices);

        const Point<double> *p_ptr = myShape.getVertices();
        for (unsigned long j = 0; j < nrVertices; j++)
        {
            gis::getLatLonFromUtm(gisSettings, p_ptr->x, p_ptr->y, &lat, &lon);
            this->geoPoints[i][j].latitude = lat;
            this->geoPoints[i][j].longitude = lon;
            p_ptr++;
        }
    }

    setDrawing(true);
    return true;
}


void MapGraphicsShapeObject::setShape(Crit3DShapeHandler* shapePtr)
{
    this->shapePointer = shapePtr;
}


Crit3DShapeHandler* MapGraphicsShapeObject::getShape()
{
    return this->shapePointer;
}


void MapGraphicsShapeObject::setDrawing(bool value)
{
    this->isDrawing = value;
}


void MapGraphicsShapeObject::setMapResolution()
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
}

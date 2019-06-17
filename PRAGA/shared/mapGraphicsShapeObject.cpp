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


QPointF MapGraphicsShapeObject::getPoint(latLonPoint geoPoint)
{
    QPointF pixel;
    pixel.setX((geoPoint.lon - this->geoMap->referencePoint.longitude) * this->geoMap->degreeToPixelX);
    pixel.setY((geoPoint.lat - this->geoMap->referencePoint.latitude) * this->geoMap->degreeToPixelY);
    return pixel;
}


void MapGraphicsShapeObject::drawShape(QPainter* myPainter)
{
    QPointF point;
    QPolygonF polygon;
    unsigned long j;

    myPainter->setPen(Qt::black);
    myPainter->setBrush(Qt::red);

    unsigned int nrShapes = unsigned(shapePointer->getShapeCount());
    for (unsigned long i = 0; i < nrShapes; i++)
    {
        if (geoBounds[i].bottomLeft.longitude < geoMap->topRight.longitude
            && geoBounds[i].topRight.longitude > geoMap->bottomLeft.longitude
            && geoBounds[i].bottomLeft.latitude < geoMap->topRight.latitude
            && geoBounds[i].topRight.latitude > geoMap->bottomLeft.latitude)
        {
            for (unsigned int part = 0; part < shapeParts[i].size(); part++)
            {
                if (! hole[i][part])
                {
                    unsigned long offset = shapeParts[i][part].offset;
                    unsigned long lenght = shapeParts[i][part].length;

                    polygon.clear();
                    for (unsigned long v = 0; v < lenght; v++)
                    {
                        j = offset + v;
                        point = getPoint(geoPoints[i][j]);
                        polygon.append(point);
                    }
                    myPainter->drawPolygon(polygon);
                }
            }
        }
    }
}


bool MapGraphicsShapeObject::initializeUTM(Crit3DShapeHandler* shapePtr, const gis::Crit3DGisSettings& gisSettings)
{
    if (shapePtr == nullptr) return false;
    this->setShape(shapePtr);

    double lat, lon;
    ShapeObject myShape;
    Box<double> bounds;
    const Point<double> *p_ptr;

    unsigned int nrShapes = unsigned(this->shapePointer->getShapeCount());
    this->shapeParts.resize(nrShapes);
    this->hole.resize(nrShapes);
    this->geoBounds.resize(nrShapes);
    this->geoPoints.resize(nrShapes);

    for (unsigned int i = 0; i < nrShapes; i++)
    {
        this->shapePointer->getShape(int(i), myShape);
        this->shapeParts[i] = myShape.getParts();

        // holes
        this->hole[i].resize(myShape.getPartCount());
        for (unsigned long j = 0; j < myShape.getPartCount(); j++)
        {
            hole[i][j] = myShape.isHole(j);
        }

        // shape bounds
        bounds = myShape.getBounds();
        gis::getLatLonFromUtm(gisSettings, bounds.xmin, bounds.ymin, &lat, &lon);
        this->geoBounds[i].bottomLeft.latitude = lat;
        this->geoBounds[i].bottomLeft.longitude = lon;
        gis::getLatLonFromUtm(gisSettings, bounds.xmax, bounds.ymax, &lat, &lon);
        this->geoBounds[i].topRight.latitude = lat;
        this->geoBounds[i].topRight.longitude = lon;

        // vertices
        unsigned long nrVertices = myShape.getVertexCount();
        this->geoPoints[i].resize(nrVertices);
        p_ptr = myShape.getVertices();
        for (unsigned long j = 0; j < nrVertices; j++)
        {
            gis::getLatLonFromUtm(gisSettings, p_ptr->x, p_ptr->y, &lat, &lon);
            this->geoPoints[i][j].lat = lat;
            this->geoPoints[i][j].lon = lon;
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


Crit3DShapeHandler* MapGraphicsShapeObject::getShapePointer()
{
    return this->shapePointer;
}


void MapGraphicsShapeObject::setDrawing(bool value)
{
    this->isDrawing = value;
}


void MapGraphicsShapeObject::setMapResolution()
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


void MapGraphicsShapeObject::clear()
{
    setDrawing(false);

    shapeParts.clear();
    geoPoints.clear();
    hole.clear();
    geoBounds.clear();
}

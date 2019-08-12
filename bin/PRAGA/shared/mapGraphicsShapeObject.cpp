#include "mapGraphicsShapeObject.h"
#include "commonConstants.h"


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
    this->nrShapes = 0;
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


QPointF MapGraphicsShapeObject::getPoint(LatLonPoint geoPoint)
{
    QPointF pixel;
    pixel.setX((geoPoint.lon - this->geoMap->referencePoint.longitude) * this->geoMap->degreeToPixelX);
    pixel.setY((geoPoint.lat - this->geoMap->referencePoint.latitude) * this->geoMap->degreeToPixelY);
    return pixel;
}


void MapGraphicsShapeObject::setPolygon(unsigned int i, unsigned int j, QPolygonF* polygon)
{
    QPointF point;

    polygon->clear();
    unsigned long offset = shapeParts[i][j].offset;
    unsigned long lenght = shapeParts[i][j].length;

    for (unsigned long v = 0; v < lenght; v++)
    {
        j = offset + v;
        point = getPoint(geoPoints[i][j]);
        polygon->append(point);
    }
}


void MapGraphicsShapeObject::drawShape(QPainter* myPainter)
{
    QPolygonF polygon;
    QPainterPath path;
    QPainterPath inner;

    myPainter->setPen(Qt::black);
    myPainter->setBrush(Qt::red);

    for (unsigned long i = 0; i < nrShapes; i++)
    {
        for (unsigned int j = 0; j < shapeParts[i].size(); j++)
        {
            if (shapeParts[i][j].hole)
                continue;

            if (geoBounds[i][j].v0.lon > geoMap->topRight.longitude
                    || geoBounds[i][j].v0.lat > geoMap->topRight.latitude
                    || geoBounds[i][j].v1.lon < geoMap->bottomLeft.longitude
                    || geoBounds[i][j].v1.lat < geoMap->bottomLeft.latitude)
            {
                continue;
            }

            setPolygon(i, j, &polygon);

            if (holes[i][j].size() == 0)
            {
                myPainter->drawPolygon(polygon);
            }
            else
            {
                path.addPolygon(polygon);

                for (unsigned int k = 0; k < holes[i][j].size(); k++)
                {
                    setPolygon(i, holes[i][j][k], &polygon);
                    inner.addPolygon(polygon);
                }

                myPainter->drawPath(path.subtracted(inner));
                inner.clear();
                path.clear();
            }
        }
    }
}


bool MapGraphicsShapeObject::initializeUTM(Crit3DShapeHandler* shapePtr)
{
    if (shapePtr == nullptr) return false;
    setShape(shapePtr);

    double lat, lon;
    ShapeObject myShape;
    Box<double>* bounds;
    const Point<double> *p_ptr;
    Point<double> point;

    nrShapes = unsigned(shapePointer->getShapeCount());
    shapeParts.resize(nrShapes);
    holes.resize(nrShapes);
    geoBounds.resize(nrShapes);
    geoPoints.resize(nrShapes);
    double refLatitude = geoMap->referencePoint.latitude;
    int zoneNumber = shapePtr->getUtmZone();

    for (unsigned int i = 0; i < nrShapes; i++)
    {
        shapePointer->getShape(int(i), myShape);
        shapeParts[i] = myShape.getParts();

        unsigned int nrParts = myShape.getPartCount();
        holes[i].resize(nrParts);
        geoBounds[i].resize(nrParts);

        for (unsigned int j = 0; j < nrParts; j++)
        {
            // bounds
            bounds = &(shapeParts[i][j].boundsPart);
            gis::utmToLatLon(zoneNumber, refLatitude, bounds->xmin, bounds->ymin, &lat, &lon);
            geoBounds[i][j].v0.lat = lat;
            geoBounds[i][j].v0.lon = lon;

            gis::utmToLatLon(zoneNumber, refLatitude, bounds->xmax, bounds->ymax, &lat, &lon);
            geoBounds[i][j].v1.lat = lat;
            geoBounds[i][j].v1.lon = lon;

            // holes
            if (shapeParts[i][j].hole)
            {
                // check first point
                unsigned long offset = shapeParts[i][j].offset;
                point = myShape.getVertex(offset);
                int index = myShape.getIndexPart(point.x, point.y);
                if (index != NODATA)
                {
                    holes[i][unsigned(index)].push_back(j);
                }
            }
        }

        // vertices
        unsigned long nrVertices = myShape.getVertexCount();
        geoPoints[i].resize(nrVertices);
        p_ptr = myShape.getVertices();
        for (unsigned long j = 0; j < nrVertices; j++)
        {
            gis::utmToLatLon(zoneNumber, refLatitude, p_ptr->x, p_ptr->y, &lat, &lon);
            geoPoints[i][j].lat = lat;
            geoPoints[i][j].lon = lon;
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

    for (unsigned int i = 0; i < nrShapes; i++)
    {
        shapeParts[i].clear();
        holes[i].clear();
        geoBounds[i].clear();
        geoPoints[i].clear();
    }
    shapeParts.clear();
    holes.clear();
    geoBounds.clear();
    geoPoints.clear();

    nrShapes = 0;
}

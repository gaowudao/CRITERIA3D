#include "mapGraphicsShapeObject.h"


MapGraphicsShapeObject::MapGraphicsShapeObject(MapGraphicsView* _view, MapGraphicsObject *parent) :
    MapGraphicsObject(true, parent)
{
    this->setFlag(MapGraphicsObject::ObjectIsSelectable, false);
    this->setFlag(MapGraphicsObject::ObjectIsMovable, false);
    this->setFlag(MapGraphicsObject::ObjectIsFocusable);
    view = _view;

    this->isDrawing = false;
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
        /*setMapResolution();

        if (this->shapePtr != nullptr)
            drawRaster(this->currentRaster, painter, this->drawBorder);

        if (this->legend != nullptr)
            this->legend->repaint();*/
    }
}

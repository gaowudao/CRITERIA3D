#include "RasterObject.h"
#include "raster.h"
#include "Position.h"

#include <QtDebug>
#include <QKeyEvent>


extern gis::Crit3DGeoMap *geoMap;
extern gis::Crit3DRasterGrid *DTM;

 RasterObject::RasterObject(int xpos, int ypos, int width, int height, MapGraphicsView* view, MapGraphicsObject *parent) :
    MapGraphicsObject(true, parent), _xpos(xpos), _ypos(ypos), _width(width), _height(height)
{
    this->setFlag(MapGraphicsObject::ObjectIsSelectable, false);
    this->setFlag(MapGraphicsObject::ObjectIsMovable, false);
    this->setFlag(MapGraphicsObject::ObjectIsFocusable);
    _view = view;
}

 RasterObject::~RasterObject()
{
}

 /*!
  \brief You need to implement this. If sizeIsZoomInvariant() is true, this should return the size of the
  rectangle you want in PIXELS. If false, this should return the size of the rectangle in METERS. The
  rectangle should be centered at (0,0) regardless.

  \return QRectF
 */


 QRectF RasterObject::boundingRect() const
{
     return QRectF(_xpos,
                       _ypos,
                       _width,
                       _height);
}


void RasterObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    //painter->setRenderHint(QPainter::Antialiasing, true);
    setMapResolution(_view);

    drawRaster(DTM, geoMap, painter);

    painter->setPen(QColor(0, 0, 0));
    painter->drawRect(-5, -5, 10, 10);
}


void RasterObject::moveCenter()
{
    QPointF newCenter = _view->mapToScene(QPoint(_view->width()/2, _view->height()/2));
    this->setPos(newCenter);
}


//protected
//virtual from MapGraphicsObject
void RasterObject::keyReleaseEvent(QKeyEvent *event)
{
    if (event->matches(QKeySequence::Delete))
    {
        this->deleteLater();
        event->accept();
    }
    else
        event->ignore();
}



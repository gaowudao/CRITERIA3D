#include <QtDebug>
#include "stationMarker.h"
#include "project.h"


extern Project myProject;


StationMarker::StationMarker(qreal radius,bool sizeIsZoomInvariant, QColor fillColor, MapGraphicsView* view, CircleObject *parent) :
    CircleObject(radius, sizeIsZoomInvariant, fillColor, parent)
{

    this->setFlag(MapGraphicsObject::ObjectIsSelectable);
    this->setFlag(MapGraphicsObject::ObjectIsMovable);
    this->setFlag(MapGraphicsObject::ObjectIsFocusable);
    _view = view;

}


void StationMarker::mousePressEvent(QGraphicsSceneMouseEvent *event)
{

    this->setFillColor(QColor(255,0,0,255));

}



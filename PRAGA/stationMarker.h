#ifndef STATIONMARKER_H
#define STATIONMARKER_H

#include "MapGraphics_global.h"
#include "CircleObject.h"
#include "MapGraphicsView.h"

#include <QMenu>

class StationMarker : public CircleObject
{

    public:
        explicit StationMarker(qreal radius, bool sizeIsZoomInvariant, QColor fillColor, MapGraphicsView* view, MapGraphicsObject *parent = 0);
    private:
        MapGraphicsView* _view;


    protected:
        void mousePressEvent(QGraphicsSceneMouseEvent *event);


};

#endif // STATIONMARKER_H



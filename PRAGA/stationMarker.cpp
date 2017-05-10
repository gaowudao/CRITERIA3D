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

    gis::Crit3DGeoPoint pointSelected;
    pointSelected.latitude = this->latitude();
    pointSelected.longitude = this->longitude();
    QPoint pos(this->longitude(), this->latitude());

    if (event->buttons() & Qt::LeftButton)
    {
        QColor color = this->color();
        if ( color ==  Qt::white )
        {
            //this->setFillColor(QColor(255,0,0,255));
            this->setFillColor(QColor((Qt::red)));
            myProject.meteoPointsSelected << pointSelected;
        }
        else
        {
            this->setFillColor(QColor((Qt::white)));
            for (int i = 0; i < myProject.meteoPointsSelected.size(); i++)
            {
                if (myProject.meteoPointsSelected[i].latitude == pointSelected.latitude && myProject.meteoPointsSelected[i].longitude == pointSelected.longitude)
                    myProject.meteoPointsSelected.removeAt(i);
            }
        }
    }
    else if (event->buttons() & Qt::RightButton)
    {

        QMenu menu;
        menu.addAction("Menu Item 1");
        menu.addAction("Menu Item 2");
        menu.addAction("Menu Item 3");
        menu.exec(QCursor::pos());

    }

}


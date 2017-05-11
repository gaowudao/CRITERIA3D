#include <QtDebug>
#include "stationMarker.h"
#include "project.h"


extern Project myProject;


StationMarker::StationMarker(qreal radius,bool sizeIsZoomInvariant, QColor fillColor, MapGraphicsView* view, MapGraphicsObject *parent) :
    CircleObject(radius, sizeIsZoomInvariant, fillColor, parent)
{

    this->setFlag(MapGraphicsObject::ObjectIsSelectable);
    this->setFlag(MapGraphicsObject::ObjectIsMovable);
    this->setFlag(MapGraphicsObject::ObjectIsFocusable);
    _view = view;
}


void StationMarker::mousePressEvent(QGraphicsSceneMouseEvent *event)
{

    this->setToolTip("test");
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
        QAction *firstItem = menu.addAction("Menu Item 1");
        QAction *secondItem = menu.addAction("Menu Item 2");
        QAction *thirdItem = menu.addAction("Menu Item 3");
        QAction *selection =  menu.exec(QCursor::pos());

        if (selection == firstItem)
        {
            this->setFillColor(QColor((Qt::yellow)));
        }
        else if (selection == secondItem)
        {
            this->setFillColor(QColor((Qt::blue)));
        }
        else if (selection == thirdItem)
        {
            this->setFillColor(QColor((Qt::green)));
        }

    }

}


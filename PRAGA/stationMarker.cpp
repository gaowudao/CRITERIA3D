#include <QtDebug>
#include "stationMarker.h"
#include "project.h"
#include "commonConstants.h"


extern Project myProject;


StationMarker::StationMarker(qreal radius,bool sizeIsZoomInvariant, QColor fillColor, MapGraphicsView* view, MapGraphicsObject *parent) :
    CircleObject(radius, sizeIsZoomInvariant, fillColor, parent)
{

    this->setFlag(MapGraphicsObject::ObjectIsSelectable);
    this->setFlag(MapGraphicsObject::ObjectIsMovable);
    this->setFlag(MapGraphicsObject::ObjectIsFocusable);
    _view = view;
}


void StationMarker::setToolTip(int i)
{
    QString idpoint = QString::fromStdString(myProject.meteoPoints[i].id);
    QString name = QString::fromStdString(myProject.meteoPoints[i].name);
    QString dataset = QString::fromStdString(myProject.meteoPoints[i].dataset);
    float altitude = myProject.meteoPoints[i].point.z;
    QString municipality = QString::fromStdString(myProject.meteoPoints[i].municipality);

    QString toolTipText = QString("<b> %1 </b> <br/> ID: %2 <br/> dataset: %3 <br/> altitude: %4 m <br/> municipality: %5")
                            .arg(name).arg(idpoint).arg(dataset).arg(altitude).arg(municipality);

    float myValue = myProject.meteoPoints[i].currentValue;
    if (myValue != NODATA)
    {
        QString value = QString::number(myValue);

        QString myQuality = "";
        if (myProject.meteoPoints[i].quality == quality::wrong_syntactic)
            myQuality = "WRONG DATA (syntax control)";
        if (myProject.meteoPoints[i].quality == quality::wrong_spatial)
            myQuality = "WRONG DATA (spatial control)";

        toolTipText = QString("value: <b> %1 <br/> %2 <br/> </b>").arg(value).arg(myQuality) + toolTipText;
    }

    CircleObject::setToolTip(toolTipText);
}


void StationMarker::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    gis::Crit3DGeoPoint pointSelected;
    pointSelected.latitude = this->latitude();
    pointSelected.longitude = this->longitude();

    if (event->buttons() & Qt::LeftButton)
    {
        QColor color = this->color();
        if ( color ==  Qt::white )
        {
            this->setFillColor(QColor((Qt::red)));
            myProject.meteoPointsSelected << pointSelected;
        }
        else
        {
            this->setFillColor(QColor((Qt::white)));
            for (int i = 0; i < myProject.meteoPointsSelected.size(); i++)
            {
                if (myProject.meteoPointsSelected[i].latitude == pointSelected.latitude
                    && myProject.meteoPointsSelected[i].longitude == pointSelected.longitude)
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

//////////////// abilitare sotto se si preferisce selezionare la stazione con doppio click invece che con singolo click di sinistra ed al singolo click collegare altre azioni///////////////
/*
void StationMarker::mousePressEvent(QGraphicsSceneMouseEvent *event)
{

    if (event->buttons() & Qt::LeftButton)
    {

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


void StationMarker::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event)
{

    gis::Crit3DGeoPoint pointSelected;
    pointSelected.latitude = this->latitude();
    pointSelected.longitude = this->longitude();

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
*/

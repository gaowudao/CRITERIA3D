#include <QtGui>
#include <QString>
#include <algorithm>
#include <math.h>
#include "commonConstants.h"
#include "mapWidget.h"
#include "project.h"
#include "gis.h"
#include "solarRadiation.h"

using namespace std;
using namespace gis;

extern Crit3DProject myProject;

float metreToPixel;
Crit3DUtmPoint utmCenter;
Crit3DPixel pixelCenter;

void mapWidget::setCurrentView(TMapVariable myVar)
{
    currentViewVariable = myVar;
}

mapWidget::mapWidget()
{
    zoomFactor = 1.0;
    minZoomFactor = (float)0.001;
    currentViewVariable = dtmMap;
}

void mapWidget::initializeWindow()
{
    float dx, dy;

    pixelCenter.x = this->width()/2;
    pixelCenter.y = this->height()/2;

    if (myProject.dtm.isLoaded)
    {
        dx = this->width() / (myProject.dtm.header->nrCols * myProject.dtm.header->cellSize);
        dy = this->height() / (myProject.dtm.header->nrRows * myProject.dtm.header->cellSize);
        metreToPixel = min(dx, dy) * zoomFactor;

        utmCenter.x = myProject.dtm.header->llCorner->x + (myProject.dtm.header->nrCols * myProject.dtm.header->cellSize) /2.;
        utmCenter.y = myProject.dtm.header->llCorner->y + (myProject.dtm.header->nrRows * myProject.dtm.header->cellSize) /2.;
    }
    else if (myProject.nrMeteoPoints > 0)
    {
        Crit3DUtmPoint pointsLL, pointsUR;
        pointsLL = myProject.meteoPoints[0].point.utm;
        pointsUR = myProject.meteoPoints[0].point.utm;

        for (int i=1; i < myProject.nrMeteoPoints; i++)
        {
            pointsLL.x = min(pointsLL.x, myProject.meteoPoints[i].point.utm.x);
            pointsLL.y = min(pointsLL.y, myProject.meteoPoints[i].point.utm.y);
            pointsUR.x = max(pointsUR.x, myProject.meteoPoints[i].point.utm.x);
            pointsUR.y = max(pointsUR.y, myProject.meteoPoints[i].point.utm.y);
        }

        dx = this->width() / (pointsUR.x - pointsLL.x);
        dy = this->height() / (pointsUR.y - pointsLL.y);
        metreToPixel = min(dx, dy) * zoomFactor;

        utmCenter.x = (pointsUR.x + pointsLL.x) /2.;
        utmCenter.y = (pointsUR.y + pointsLL.y) /2.;
    }
}


void mapWidget::redrawMeteoPoints()
{
    if (myProject.nrMeteoPoints == 0) return;

    QPainter myPainter(this);
    QRect myRect;
    QPointF myPoint;
    QString myStr;

    myPainter.setBrush(QBrush(Qt::transparent, Qt::SolidPattern));
    myPainter.setPen(QPen(Qt::black, 0.2, Qt::SolidLine));

    int myX, myY;

    float myRadius = min(this->height(), this->width()) / 200.0 * zoomFactor;
    myRadius = max(min(myRadius, float(5)), float(1));
    bool isDrawValue = false;

    for (int i=0; i < myProject.nrMeteoPoints; i++)
    {
        myX = pixelCenter.x + ((myProject.meteoPoints[i].point.utm.x - utmCenter.x) * metreToPixel);
        myY = pixelCenter.y - ((myProject.meteoPoints[i].point.utm.y - utmCenter.y) * metreToPixel);

        isDrawValue = false;
        float myValue;
        if (myProject.isObsDataLoaded)
            if (myProject.meteoPoints[i].nrObsDataDaysH > 0)
            {
                myValue = myProject.meteoPoints[i].getMeteoPointValueH(myProject.currentTime.date, myProject.currentTime.getHour(),
                                                                myProject.currentTime.getMinutes(), myProject.currentVar);
                if (myValue != NODATA)
                {
                    myPoint.setX(myX);
                    myPoint.setY(myY);
                    myStr = QString::number(myValue);
                    //myStr = QString::fromStdString(myProject.meteoPoints[i].id);
                    myPainter.drawText(myPoint, myStr);
                    isDrawValue = true;
                }
            }


        if (! isDrawValue)
        {
            myRect.setBottom(myY - myRadius);
            myRect.setTop(myY + myRadius);
            myRect.setLeft((myX - myRadius));
            myRect.setRight((myX + myRadius));
            myPainter.drawEllipse(myRect);
        }
    }
}


void mapWidget::redrawMap(Crit3DRasterGrid* myGrid)
{
    if (! myGrid->isLoaded) return;

    // create a QPainter and pass a pointer to the device.
    // a paint device can be a QWidget, a QPixmap or a QImage
    QPainter myPainter(this);
    Crit3DColor* myColor;
    QColor myQColor;

    int x0, y0, x1, y1, x, y, dx, dy;
    float myValue;

    float pixelCellSize = myGrid->header->cellSize * metreToPixel;

    int step = max(int(1 / pixelCellSize), 1);

    myPainter.setPen(QPen(Qt::red, 1, Qt::SolidLine));
    myPainter.setBrush(QBrush(Qt::red, Qt::SolidPattern));

    Crit3DPixel pixelLL;
    pixelLL.x = pixelCenter.x - (utmCenter.x - myGrid->header->llCorner->x) * metreToPixel;
    pixelLL.y = pixelCenter.y - (utmCenter.y - myGrid->header->llCorner->y) * metreToPixel;

    y0 = pixelLL.y;
    for (long myRow = 0; myRow < myGrid->header->nrRows; myRow += step)
    {
        y1 = pixelLL.y + (myRow+step) * pixelCellSize;
        x0 = pixelLL.x;
        for (long myCol = 0; myCol < myGrid->header->nrCols; myCol += step)
        {
            x1 = pixelLL.x + (myCol+step) * pixelCellSize;
            myValue = myGrid->value[myRow][myCol];
            if (myValue != myGrid->header->flag)
            {
                myColor = myGrid->colorScale->getColor(myValue);
                myQColor = QColor(myColor->red, myColor->green, myColor->blue);
                myPainter.setPen(myQColor);

                dx = (x1 - x0);
                dy = (y1 - y0);
                if ((dx < 2) && (dy < 2))
                    for (x = x0; x <= x1; x++)
                        for (y = y0; y <= y1; y++)
                                myPainter.drawPoint(x,y);
                else
                {
                    myPainter.setBrush(myQColor);
                    myPainter.drawRect(x0, y0, dx, dy);
                }
            }
            x0 = ++x1;
        }
        y0 = ++y1;
    }
}


void mapWidget::paintEvent(QPaintEvent *event)
{
    if ((myProject.dtm.isLoaded)||(myProject.nrMeteoPoints > 0))
    {
        mapWidget::initializeWindow();

        if (currentViewVariable == dtmMap)
            mapWidget::redrawMap(&(myProject.dtm));
        else if (currentViewVariable == airTempMap)
            mapWidget::redrawMap(myProject.meteoMaps->airTemperatureMap);
        else if (currentViewVariable == precMap)
            mapWidget::redrawMap(myProject.meteoMaps->precipitationMap);
        else if (currentViewVariable == relHumMap)
            mapWidget::redrawMap(myProject.meteoMaps->airHumidityMap);
        else if (currentViewVariable == windIntMap)
            mapWidget::redrawMap(myProject.meteoMaps->windIntensityMap);
        else if (currentViewVariable == globRadMap)
            mapWidget::redrawMap(myProject.meteoMaps->radiationMaps->globalRadiationMap);
        else if (currentViewVariable == leafWetMap)
            mapWidget::redrawMap(myProject.meteoMaps->leafWetnessMap);
        mapWidget::redrawMeteoPoints();
    }
}


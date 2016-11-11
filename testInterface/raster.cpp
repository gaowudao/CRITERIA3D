#include <QDebug>
#include <QString>
#include <QPainter>

#include "CircleObject.h"
#include "LineObject.h"
#include "Position.h"

#include "raster.h"
#include "mainwindow.h"

gis::Crit3DRasterGrid *DTM;
gis::Crit3DGeoMap *geoMap;

using namespace std;

void initializeDTM()
{
    DTM = new gis::Crit3DRasterGrid();
    geoMap = new gis::Crit3DGeoMap();
}


bool loadRaster(QString myFileName, gis::Crit3DRasterGrid *myRaster)
{
    string* myError = new std::string();
    string fileName = myFileName.left(myFileName.length()-4).toStdString();

    if (gis::readEsriGrid(fileName, myRaster, myError))
    {
        gis::setDefaultDTMScale(myRaster->colorScale);
        qDebug() << "Raster Ok.";
        return (true);
    }
    else
    {
        qDebug() << "Load raster failed!";
        return (false);
    }
}


bool setMapResolution(MapGraphicsView* view)
{
    if(DTM == NULL) return false;

    QPointF bottomLeft = view->mapToScene(QPoint(0.0, 0.0));
    QPointF topRight = view->mapToScene(QPoint(view->width(), view->height()));

    geoMap->bottomLeft.longitude = bottomLeft.x();
    geoMap->bottomLeft.latitude = bottomLeft.y();
    geoMap->topRight.longitude = topRight.x();
    geoMap->topRight.latitude = topRight.y();

    const qreal widthLon = qAbs<qreal>(topRight.x() - bottomLeft.x());
    const qreal heightlat = qAbs<qreal>(topRight.y() - bottomLeft.y());

    qreal dxdegree = widthLon / view->width();
    qreal dydegree = heightlat / view->height();

    qDebug() << "setMapResolution degree dx:" << dxdegree;
    qDebug() << "setMapResolution degree dy:" << dydegree;

    geoMap->setResolution(dxdegree, dydegree);
    return true;
}


bool drawRaster(gis::Crit3DRasterGrid* myRaster, gis::Crit3DGeoMap* myMap, QPainter* myPainter)
{
    if ( myRaster == NULL) return false;
    if (! myRaster->isLoaded) return false;

    long row0, row1, col0, col1;
    gis::getRowColFromXY(*myRaster, myMap->bottomLeft.longitude, myMap->bottomLeft.latitude, &row0, &col0);
    gis::getRowColFromXY(*myRaster, myMap->topRight.longitude, myMap->topRight.latitude, &row1, &col1);

    row0 = max(long(0), row0);
    row1 = min(myRaster->header->nrRows, row1+1);
    col0 = max(long(0), col0);
    col1 = min(myRaster->header->nrCols, col1+1);

    gis::Crit3DGeoPoint llCorner;
    gis::Crit3DPixel pixelLL;
    llCorner.longitude = myRaster->header->llCorner->x + col0 * myRaster->header->cellSize;
    llCorner.latitude = myRaster->header->llCorner->y + (myRaster->header->nrRows - row1) * myRaster->header->cellSize;
    pixelLL.x = (llCorner.longitude - myMap->referencePoint.longitude) * myMap->degreeToPixelX;
    pixelLL.y = (llCorner.latitude - myMap->referencePoint.latitude) * myMap->degreeToPixelY;

    double dx = myRaster->header->cellSize * myMap->degreeToPixelX;
    double dy = myRaster->header->cellSize * myMap->degreeToPixelY;

    int step = std::max(int(1. / (std::min(dx, dy))), 1);

    int x0, y0, x1, y1, x, y;
    float myValue;
    gis::Crit3DColor* myColor;
    QColor myQColor;

    y0 = pixelLL.y;
    for (long myRow = row0; myRow < row1; myRow += step)
    {
        y1 = pixelLL.y + (myRow-row0+step) * dy;
        x0 = pixelLL.x;
        for (long myCol = col0; myCol < col1; myCol += step)
        {
            x1 = pixelLL.x + (myCol-col0+step) * dx;

            myValue = myRaster->value[myRaster->header->nrRows - myRow - 1][myCol];
            if (myValue != myRaster->header->flag)
            {
                myColor = myRaster->colorScale->getColor(myValue);

                myQColor = QColor(myColor->red, myColor->green, myColor->blue);
                myPainter->setPen(myQColor);

                for (x = x0; x <= x1; x++)
                   for (y = y0; y <= y1; y++)
                       myPainter->drawPoint(x, y);

                /*
                //codice per rettangoli
                dx = (x1 - x0);
                dy = (y1 - y0);
                myPainter->setBrush(myQColor);
                myPainter->drawRect(x0, y0, dx, dy);
                */
            }
            x0 = ++x1;
        }
        y0 = ++y1;
    }

    return true;
}


/*
void drawObject(MapGraphicsView* pointView)
{
    CircleObject* marker1 = new CircleObject(50.0, false, QColor(255,0,0,0), 0);

    Position point1 (11.4, 44.5, 0.0);

    marker1->setLatitude(point1.latitude());
    marker1->setLongitude(point1.longitude());

    marker1->setToolTip("marker1");
    pointView->scene()->addObject(marker1);

}

qreal degreesLatPerMeter(const qreal latitude)
{
    const qreal latRad = latitude * (pi / 180.0);
    qreal meters = 111132.954 - 559.822 * cos(2.0 * latRad) + 1.175 * cos(4.0 * latRad);
    return 1.0 / meters;
}

qreal degreesLonPerMeter(const qreal latitude)
{
    const qreal latRad = latitude * (pi / 180.0);
    qreal meters = (pi * A_EARTH * cos(latRad)) / (180.0 * sqrt(1.0 - NAV_E2 * pow(sin(latRad), 2.0)));
    return 1.0 / meters;
}

*/

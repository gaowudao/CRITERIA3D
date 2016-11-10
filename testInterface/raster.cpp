#include <QDebug>
#include <QString>
#include <QPainter>

#include "CircleObject.h"
#include "LineObject.h"
#include "Position.h"

#include "raster.h"
#include "mainwindow.h"

gis::Crit3DRasterGrid *DTM;
gis::Crit3DMapArea *mapArea;


void initializeMap()
{
    DTM = new gis::Crit3DRasterGrid();
    mapArea = new gis::Crit3DMapArea();
}


bool loadRaster(QString myFileName, gis::Crit3DRasterGrid *myRaster)
{
    std::string* myError = new std::string();
    std::string fileName = myFileName.left(myFileName.length()-4).toStdString();

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


bool setMapCenter()
{
    if(DTM == NULL) return false;

    mapArea->setUtmCenter(DTM->header->llCorner->x + DTM->header->cellSize * DTM->header->nrCols / 2,
                          DTM->header->llCorner->y + DTM->header->cellSize * DTM->header->nrRows / 2);
    return true;
}

bool setMapResolution(QPainter *myPainter, MapGraphicsView* view)
{

    if(DTM == NULL) return false;


    QPointF topLeft = view->mapToScene(QPoint(0.0,0.0));
    QPointF topRight = view->mapToScene(QPoint(view->width(),0.0));

    QPointF bottomLeft = view->mapToScene(QPoint(0.0,view->height()));
    QPointF bottomRight = view->mapToScene(QPoint(view->width(),view->height()));

    const qreal widthLon = qAbs<qreal>(topLeft.x() - topRight.x());
    const qreal heightlat = qAbs<qreal>(topLeft.y() - bottomLeft.y());

    qreal dxdegree = widthLon / view->width();
    qreal dydegree = heightlat / view->height();

    qDebug() << "setMapResolution degree dx:" << dxdegree;
    qDebug() << "setMapResolution degree dy:" << dydegree;

    mapArea->setResolution(dxdegree, dydegree);

/*
 *  // widthMeters heightMeters for utm case
 *
    const qreal avgLat = (topLeft.y() + bottomLeft.y()) / 2.0;
    const qreal lonPerMeter = degreesLonPerMeter(avgLat);
    const qreal latPerMeter = degreesLatPerMeter(avgLat);

    const qreal widthMeters = qMax<qreal>(widthLon / lonPerMeter, 5.0);
    const qreal heightMeters = qMax<qreal>(heightlat / latPerMeter, 5.0);

    qDebug() << "widthMeters:" << widthMeters;
    qDebug() << "heightMeters:" << heightMeters;

    double dxMeters = widthMeters / myPainter->window().width();
    double dyMeters = heightMeters / myPainter->window().height();

    qDebug() << "setMapResolution Meters dx:" << dxMeters;
    qDebug() << "setMapResolution Meters dy:" << dyMeters;

    mapArea->setResolution(std::max(dxMeters, dyMeters));
*/
    return true;
}


bool drawRaster(gis::Crit3DRasterGrid* myRaster, gis::Crit3DMapArea* myMap, QPainter* myPainter)
{
    if ( myRaster == NULL) return false;
    if (! myRaster->isLoaded) return false;
    if (! myMap->isCenterDefined) return false;

    gis::Crit3DPixel pixelCenter, pixelLL;

    pixelLL.x = 0;
    pixelLL.y = 0;


    double pixelCellSizeX = myRaster->header->cellSize * myMap->metreToPixelX;
    double pixelCellSizeY = myRaster->header->cellSize * myMap->metreToPixelY;

    int step = std::max(int(1. / (std::min(pixelCellSizeX,pixelCellSizeY))), 1);

    int x0, y0, x1, y1, x, y;
    float myValue;
    gis::Crit3DColor* myColor;
    QColor myQColor;

    //qDebug() << "starting painting raster";

    y0 = pixelLL.y;
    qDebug() << step;
    for (long myRow = 0; myRow < myRaster->header->nrRows; myRow += step)
    {
        y1 = pixelLL.y + (myRow+step) * pixelCellSizeY;

        if ((y1 > 0) && (y1 < myPainter->window().height()))
        {
            x0 = pixelLL.x;
            for (long myCol = 0; myCol < myRaster->header->nrCols; myCol += step)
            {
                x1 = pixelLL.x + (myCol+step) * pixelCellSizeX;
                if ((x1 > 0) && (x1 < myPainter->window().width()))
                {
                    myValue = myRaster->value[myRaster->header->nrRows - myRow - 1][myCol];

                    if (myValue != myRaster->header->flag)
                    {
                        myColor = myRaster->colorScale->getColor(myValue);

                        //alpha = 128
                        myQColor = QColor(myColor->red, myColor->green, myColor->blue, 128);
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
                }
                x0 = ++x1;
            }
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

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

//todo
bool setMapResolution(QPainter *myPainter)
{

    if(DTM == NULL) return false;

    double width = DTM->header->nrCols * DTM->header->cellSize;
    double height = DTM->header->nrRows * DTM->header->cellSize;

    double dx = width / myPainter->window().width();
    double dy = height / myPainter->window().height();

    mapArea->setResolution(std::max(dx, dy));

    return true;
}


bool drawRaster(gis::Crit3DRasterGrid* myRaster, gis::Crit3DMapArea* myMap, QPainter* myPainter)
{
    if ( myRaster == NULL) return false;
    if (! myRaster->isLoaded) return false;
    if (! myMap->isCenterDefined) return false;

    gis::Crit3DPixel pixelCenter, pixelLL;
    pixelCenter.x = myPainter->window().width() * 0.5;
    pixelCenter.y = myPainter->window().height() * 0.5;
    pixelLL.x = pixelCenter.x - (myMap->center.x - myRaster->header->llCorner->x) * myMap->metreToPixel;
    pixelLL.y = pixelCenter.y - (myMap->center.y - myRaster->header->llCorner->y) * myMap->metreToPixel;

    double pixelCellSize = myRaster->header->cellSize * myMap->metreToPixel;
    int step = std::max(int(1. / pixelCellSize), 1);

    int x0, y0, x1, y1, x, y;
    float myValue;
    gis::Crit3DColor* myColor;
    QColor myQColor;

    //qDebug() << "starting painting raster";

    y0 = pixelLL.y;
    qDebug() << step;
    for (long myRow = 0; myRow < myRaster->header->nrRows; myRow += step)
    {
        y1 = pixelLL.y + (myRow+step) * pixelCellSize;
        if ((y1 > 0) && (y1 < myPainter->window().height()))
        {
            x0 = pixelLL.x;
            for (long myCol = 0; myCol < myRaster->header->nrCols; myCol += step)
            {
                x1 = pixelLL.x + (myCol+step) * pixelCellSize;
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



void drawObject(MapGraphicsView* pointView)
{
    CircleObject* marker1 = new CircleObject(50.0, false, QColor(255,0,0,0), 0);

    Position point1 (11.4, 44.5, 0.0);

    marker1->setLatitude(point1.latitude());
    marker1->setLongitude(point1.longitude());

    marker1->setToolTip("marker1");
    pointView->scene()->addObject(marker1);

}

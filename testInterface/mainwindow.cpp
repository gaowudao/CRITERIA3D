#include <QFileDialog>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "raster.h"

#include "tileSources/GridTileSource.h"
#include "tileSources/OSMTileSource.h"
#include "tileSources/CompositeTileSource.h"
#include "guts/CompositeTileSourceConfigurationWidget.h"

#include "PolygonObject.h"

#include <QSharedPointer>
#include <QtDebug>
#include <QThread>
#include <QImage>

#include "CircleObject.h"
#include "LineObject.h"
#include "Position.h"

#include "RasterObject.h"

extern gis::Crit3DMapArea *mapArea;
extern gis::Crit3DRasterGrid *DTM;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Setup the MapGraphics scene and view
    scene = new MapGraphicsScene(this);
    view = new MapGraphicsView(scene,this);
    //view = new PointMap(scene,this);

    //The view will be our central widget
    this->setCentralWidget(view);

    //Setup some tile sources

    QSharedPointer<OSMTileSource> osmTiles(new OSMTileSource(OSMTileSource::OSMTiles), &QObject::deleteLater);
    QSharedPointer<CompositeTileSource> composite(new CompositeTileSource(), &QObject::deleteLater);

    composite->addSourceBottom(osmTiles);

    view->setTileSource(composite);

    //////////////////
    //CompositeTileSourceConfigurationWidget * tileConfigWidget = new CompositeTileSourceConfigurationWidget(composite.toWeakRef(), &myMapWidget);
    ///////////////////

    initializeMap();

    CircleObject* marker1 = new CircleObject(5.0, true, QColor(255,0,0,255), 0);
    marker1->setFlag(MapGraphicsObject::ObjectIsMovable, false);
    marker1->setFlag(MapGraphicsObject::ObjectIsSelectable, false);

    Position point1 (11.35, 44.5, 0.0);
    marker1->setLatitude(point1.latitude());
    marker1->setLongitude(point1.longitude());

    //marker1->setToolTip("marker1");
    view->scene()->addObject(marker1);

    view->setZoomLevel(8);
    view->centerOn(11.4, 44.5);
}


MainWindow::~MainWindow()
{
    delete view;
    delete scene;
    delete ui;
}

void MainWindow::on_actionLoad_Raster_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Raster"), "",
                                           tr("ESRI grid files (*.flt)"));
    if (fileName == "") return;

    qDebug() << "loading raster";

    if (loadRaster(fileName, DTM))
    {
        qDebug() << "creating raster";

        //RasterObject * rasterObj = new RasterObject(this->view->window()->width(), this->view->window()->height());
        RasterObject * rasterObj = new RasterObject(0,0,0,0);

        Position point1 (DTM->header->llCorner->x, DTM->header->llCorner->y, 0.0);
        rasterObj->setLatitude(point1.latitude());
        rasterObj->setLongitude(point1.longitude());

        qDebug() << "rasterObj topleft x: " << rasterObj->boundingRect().topLeft().x() << " topleft y: " << rasterObj->boundingRect().topLeft().y();
        qDebug() << "rasterObj bottomRight x: " << rasterObj->boundingRect().bottomRight().x() << " bottomRight y: " << rasterObj->boundingRect().bottomRight().y();
        qDebug() << "rasterObj->boundingRect().center(): " << rasterObj->boundingRect().center();

        qDebug() << "this->view->window()->width()" << this->view->window()->width();
        qDebug() << "this->view->window()->height()" << this->view->window()->height();

        this->view->scene()->addObject(rasterObj);

        this->view->repaint();
        this->update();
    }
}

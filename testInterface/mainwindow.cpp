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

extern gis::Crit3DGeoMap *geoMap;
extern gis::Crit3DRasterGrid *DTM;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Setup the MapGraphics scene and view
    this->scene = new MapGraphicsScene(this);
    this->view = new MapGraphicsView(scene,this);

    //The view will be our central widget
    this->setCentralWidget(this->view);

    //Setup some tile sources
    QSharedPointer<OSMTileSource> osmTiles(new OSMTileSource(OSMTileSource::OSMTiles), &QObject::deleteLater);
    QSharedPointer<CompositeTileSource> composite(new CompositeTileSource(), &QObject::deleteLater);
    composite->addSourceBottom(osmTiles);
    this->view->setTileSource(composite);

    //start: Bologna
    Position Bologna(11.35, 44.5, 0.0);

    // marker example
    CircleObject* marker1 = new CircleObject(5.0, true, QColor(255,0,0,255), 0);
    marker1->setFlag(MapGraphicsObject::ObjectIsMovable, false);
    marker1->setFlag(MapGraphicsObject::ObjectIsSelectable, false);
    marker1->setLatitude(Bologna.latitude());
    marker1->setLongitude(Bologna.longitude());
    this->view->scene()->addObject(marker1);

    //raster map
    this->rasterMap = new RasterObject(this->view);
    this->rasterMap->setOpacity(0.5);
    this->rasterMap->setPos(Bologna.lonLat());
    this->view->scene()->addObject(this->rasterMap);

    initializeDTM();
    geoMap->referencePoint.latitude = Bologna.latitude();
    geoMap->referencePoint.longitude = Bologna.longitude();

    this->view->setZoomLevel(10);
    this->view->centerOn(Bologna.lonLat());
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
    loadRaster(fileName, DTM);
}

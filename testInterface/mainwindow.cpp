#include <QFileDialog>
#include <QtDebug>

#include "tileSources/OSMTileSource.h"
#include "tileSources/CompositeTileSource.h"
#include "CircleObject.h"
#include "Position.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "raster.h"
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
    this->startCenter = new Position (11.35, 44.5, 0.0);

    //The view will be our central widget
    this->setCentralWidget(this->view);

    //Setup some tile sources
    QSharedPointer<OSMTileSource> osmTiles(new OSMTileSource(OSMTileSource::OSMTiles), &QObject::deleteLater);
    QSharedPointer<CompositeTileSource> composite(new CompositeTileSource(), &QObject::deleteLater);
    composite->addSourceBottom(osmTiles);
    this->view->setTileSource(composite);

    // marker example
    CircleObject* marker1 = new CircleObject(5.0, true, QColor(255,0,0,255), 0);
    marker1->setFlag(MapGraphicsObject::ObjectIsMovable, false);
    marker1->setFlag(MapGraphicsObject::ObjectIsSelectable, false);
    marker1->setLatitude(startCenter->latitude());
    marker1->setLongitude(startCenter->longitude());
    this->view->scene()->addObject(marker1);

    initializeDTM();
    geoMap->referencePoint.latitude = startCenter->latitude();
    geoMap->referencePoint.longitude = startCenter->longitude();

    this->view->setZoomLevel(10);
    this->view->centerOn(startCenter->lonLat());
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

    qreal maxSizex = DTM->header->nrCols;
    qreal maxSizey = DTM->header->nrRows;

    qDebug() << "maxSizex"<< maxSizex;
    qDebug() << "maxSizey"<< maxSizey;

    this->rasterMap = new RasterObject(-maxSizex/2,-maxSizey/2, maxSizex, maxSizey,this->view);
    this->rasterMap->setOpacity(0.5);
    this->rasterMap->setPos(startCenter->lonLat());
    this->view->scene()->addObject(this->rasterMap);

    QObject::connect(this->view, SIGNAL(mouseReleaseEvent(QMouseEvent*)), this->rasterMap, SLOT(moveCenter()));
}

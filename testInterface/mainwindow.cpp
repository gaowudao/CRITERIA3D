#include <QFileDialog>
#include <QtDebug>

#include "tileSources/OSMTileSource.h"
#include "tileSources/GridTileSource.h"
#include "tileSources/CompositeTileSource.h"
#include "guts/CompositeTileSourceConfigurationWidget.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Position.h"
#include "raster.h"


#define TOOLSWIDTH 220
extern gis::Crit3DRasterGrid *DTM;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Set the MapGraphics Scene and View
    this->mapScene = new MapGraphicsScene(this);
    this->mapView = new MapGraphicsView(mapScene, this->ui->widgetMap);
    this->legend = new ColorLegend(this->ui->widgetColorLegend);
    this->legend->resize(this->ui->widgetColorLegend->size());

    // Set tiles source
    this->setMapSource(OSMTileSource::OSMTiles);

    // Set start size and position
    Position* startCenter = new Position (11.35, 44.5, 0.0);
    this->mapView->setZoomLevel(10);
    this->mapView->centerOn(startCenter->lonLat());

    DTM = new gis::Crit3DRasterGrid();

    // Set raster object
    this->rasterObj = new RasterObject(this->mapView);
    this->rasterObj->setOpacity(this->ui->opacitySlider->value() / 100.0);
    this->rasterObj->setColorLegend(this->legend);
    this->mapView->scene()->addObject(this->rasterObj);

    //this->setMouseTracking(true);
}


MainWindow::~MainWindow()
{
    delete rasterObj;
    delete legend;
    delete mapView;
    delete mapScene;
    delete ui;
}


void MainWindow::setMapSource(OSMTileSource::OSMTileType mySource)
{
    QSharedPointer<OSMTileSource> myTiles(new OSMTileSource(mySource), &QObject::deleteLater);
    QSharedPointer<CompositeTileSource> composite(new CompositeTileSource(), &QObject::deleteLater);
    composite->addSourceBottom(myTiles);

    this->mapView->setTileSource(composite);
}


void MainWindow::on_actionLoad_Raster_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open rasterObj"), "", tr("ESRI grid files (*.flt)"));
    if (fileName == "") return;

    qDebug() << "loading raster";
    if (!loadRaster(fileName, DTM)) return;

    // center map
    gis::Crit3DGeoPoint* center = gis::getRasterGeoCenter(DTM->header);
    this->mapView->centerOn(qreal(center->longitude), qreal(center->latitude));

    // resize map
    float size = gis::getRasterMaxSize(DTM->header);
    size = log2(1000.0/size);
    this->mapView->setZoomLevel(quint8(size));
    this->mapView->centerOn(qreal(center->longitude), qreal(center->latitude));

    // active raster object
    this->rasterObj->updateCenter();
    this->rasterObj->setDrawing(true);
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event){
    Q_UNUSED(event)

    if (this->rasterObj != NULL)
        this->rasterObj->updateCenter();
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent * event)
{
    QPoint mapPoint = getMapPoint(&(event->pos()));
    if ((mapPoint.x() <= 0) || (mapPoint.y() <= 0)) return;

    Position newCenter = this->mapView->mapToScene(mapPoint);
    this->ui->statusBar->showMessage(QString::number(newCenter.latitude()) + " " + QString::number(newCenter.longitude()));

    this->rasterObj->setDrawing(false);
        if (event->button() == Qt::LeftButton)
            this->mapView->zoomIn();
        else
            this->mapView->zoomOut();

        this->mapView->centerOn(newCenter.lonLat());
        this->rasterObj->updateCenter();
    this->rasterObj->setDrawing(true);
}

void MainWindow::mouseMoveEvent(QMouseEvent * event)
{
    QPoint mapPoint = getMapPoint(&(event->pos()));
    if ((mapPoint.x() <= 0) || (mapPoint.y() <= 0)) return;

    Position geoPoint = this->mapView->mapToScene(mapPoint);
    this->ui->statusBar->showMessage(QString::number(geoPoint.latitude()) + " " + QString::number(geoPoint.longitude()));
}

void MainWindow::resizeEvent(QResizeEvent * event)
{
    Q_UNUSED(event)

    this->ui->widgetMap->setGeometry(TOOLSWIDTH, 0, this->width()-TOOLSWIDTH, this->height() - 40);
    this->mapView->resize(this->ui->widgetMap->size());

    this->ui->groupBoxRaster->move(10, this->height() - this->ui->groupBoxRaster->height() - 50);
}

QPoint MainWindow::getMapPoint(QPoint* point) const
{
    QPoint mapPoint;
    int dx, dy;
    dx = this->ui->widgetMap->x() + 10;
    dy = + this->ui->widgetMap->y() + this->ui->menuBar->height() + 10;
    mapPoint.setX(point->x() - dx);
    mapPoint.setY(point->y() - dy);
    return mapPoint;
}

void MainWindow::on_opacitySlider_sliderMoved(int position)
{
    if (this->rasterObj != NULL)
        this->rasterObj->setOpacity(position / 100.0);
}


void MainWindow::on_actionOpenstreetmap_triggered()
{
    this->setMapSource(OSMTileSource::OSMTiles);
}


void MainWindow::on_actionWikimedia_Maps_triggered()
{
    this->setMapSource(OSMTileSource::WikimediaMaps);
}

void MainWindow::on_actionTerrain_triggered()
{
    this->setMapSource(OSMTileSource::Terrain);
}

void MainWindow::on_actionToner_lite_triggered()
{
    this->setMapSource(OSMTileSource::TonerLite);
}

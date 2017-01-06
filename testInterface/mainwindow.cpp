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

#define TOOLSWIDTH 200
extern gis::Crit3DGeoMap *geoMap;
extern gis::Crit3DRasterGrid *DTM;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    /*!
     * Setup the MapGraphics scene and view
     */
    this->scene = new MapGraphicsScene(this);
    this->view = new MapGraphicsView(scene, this->ui->widgetMap);

    Position* startCenter = new Position (11.35, 44.5, 0.0);

    // Setup some tile sources
    QSharedPointer<OSMTileSource> osmTiles(new OSMTileSource(OSMTileSource::OSMTiles), &QObject::deleteLater);
    QSharedPointer<CompositeTileSource> composite(new CompositeTileSource(), &QObject::deleteLater);
    composite->addSourceBottom(osmTiles);
    this->view->setTileSource(composite);

    /*
     * marker example
     * CircleObject* marker1 = new CircleObject(5.0, true, QColor(255,0,0,255), 0);
     * marker1->setFlag(MapGraphicsObject::ObjectIsMovable, false);
     * marker1->setFlag(MapGraphicsObject::ObjectIsSelectable, false);
     * marker1->setLatitude(startCenter->latitude());
     * marker1->setLongitude(startCenter->longitude());
     * this->view->scene()->addObject(marker1);
    */

    geoMap->referencePoint.latitude = startCenter->latitude();
    geoMap->referencePoint.longitude = startCenter->longitude();

    this->rasterMap = NULL;
    this->view->setZoomLevel(10);
    this->view->centerOn(startCenter->lonLat());

    //this->setMouseTracking(true);
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
    this->rasterMap->deleteLater();

    loadRaster(fileName, DTM);

    this->rasterMap = new RasterObject(this->view);
    this->rasterMap->setOpacity(this->ui->opacitySlider->value() / 100.0);

    float size = gis::getRasterMaxSize(DTM->header);
    size = log2(1000.0/size);
    this->view->setZoomLevel(quint8(size));

    gis::Crit3DGeoPoint* center = gis::getRasterGeoCenter(DTM->header);
    this->view->centerOn(qreal(center->longitude), qreal(center->latitude));
    this->rasterMap->moveCenter();

    this->view->scene()->addObject(this->rasterMap);
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event){
    Q_UNUSED(event)

    if (this->rasterMap != NULL)
        this->rasterMap->moveCenter();
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent * event)
{
    QPoint mapPoint = getMapPoint(&(event->pos()));
    if ((mapPoint.x() <= 0) || (mapPoint.y() <= 0)) return;

    Position newCenter = this->view->mapToScene(mapPoint);
    this->ui->statusBar->showMessage(QString::number(newCenter.latitude()) + " " + QString::number(newCenter.longitude()));

    isDrawing = false;
        if (event->button() == Qt::LeftButton)
            this->view->zoomIn();
        else
            this->view->zoomOut();

        this->view->centerOn(newCenter.lonLat());

        if (this->rasterMap != NULL) this->rasterMap->moveCenter();
    isDrawing = true;
}

void MainWindow::mouseMoveEvent(QMouseEvent * event)
{
    QPoint mapPoint = getMapPoint(&(event->pos()));
    Position geoPoint = this->view->mapToScene(mapPoint);
    this->ui->statusBar->showMessage(QString::number(geoPoint.latitude()) + " " + QString::number(geoPoint.longitude()));
}

void MainWindow::resizeEvent(QResizeEvent * event)
{
    Q_UNUSED(event)

    this->ui->widgetMap->setGeometry(TOOLSWIDTH, 0, this->width()-TOOLSWIDTH, this->height()-40);
    this->view->resize(this->ui->widgetMap->size());
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
    if (this->rasterMap != NULL)
        this->rasterMap->setOpacity(position / 100.0);
}

/*!
    \file mainWindow.cpp

    \abstract Praga-Gis mainwindow

    This file is part of CRITERIA-3D distribution.

    CRITERIA-3D has been developed by A.R.P.A.E. Emilia-Romagna.

    \copyright
    CRITERIA-3D is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    CRITERIA-3D is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.
    You should have received a copy of the GNU Lesser General Public License
    along with CRITERIA-3D.  If not, see <http://www.gnu.org/licenses/>.

    \authors
    Fausto Tomei ftomei@arpae.it
    Gabriele Antolini gantolini@arpae.it
    Laura Costantini laura.costantini0@gmail.com
*/

#include <cmath>

#include <QFileDialog>
#include <QLayout>
#include <QCheckBox>
#include <QWidget>
#include <QFrame>
#include <QVBoxLayout>

#include "tileSources/CompositeTileSource.h"

#include "gis.h"
#include "gisProject.h"

#include "mainWindow.h"
#include "ui_mainWindow.h"


#define MAPBORDER 8
#define TOOLSWIDTH 260
#define INFOHEIGHT 40

extern GisProject myProject;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Set the MapGraphics Scene and View
    this->mapScene = new MapGraphicsScene(this);
    this->mapView = new MapGraphicsView(mapScene, this->ui->widgetMap);

    // Set tiles source
    this->setMapSource(OSMTileSource::OSMTiles);

    // Set start size and position
    this->startCenter = new Position (myProject.gisSettings.startLocation.longitude, myProject.gisSettings.startLocation.latitude, 0.0);
    this->mapView->setZoomLevel(8);
    this->mapView->centerOn(startCenter->lonLat());

    this->setMouseTracking(true);
}


MainWindow::~MainWindow()
{
    if (! this->rasterObjList.empty())
        for (unsigned int i = 0; i < this->rasterObjList.size(); i++)
            delete this->rasterObjList[i];

    if (! this->rasterColorScaleList.empty())
        for (unsigned int i = 0; i < this->rasterColorScaleList.size(); i++)
            delete this->rasterColorScaleList[i];

    delete mapView;
    delete mapScene;
    delete ui;
}


void MainWindow::resizeEvent(QResizeEvent * event)
{
    Q_UNUSED(event)

    ui->widgetMap->setGeometry(TOOLSWIDTH, 0, this->width()-TOOLSWIDTH, this->height() - INFOHEIGHT);
    mapView->resize(ui->widgetMap->size());

    ui->groupBox->move(MAPBORDER/2, MAPBORDER);
    ui->groupBox->resize(TOOLSWIDTH, this->height() - INFOHEIGHT - MAPBORDER * 2);
}


void MainWindow::updateCenter()
{
    if (! this->rasterObjList.empty())
        for (unsigned int i = 0; i < this->rasterObjList.size(); i++)
            this->rasterObjList[i]->updateCenter();
}


void MainWindow::mouseReleaseEvent(QMouseEvent *event){
    Q_UNUSED(event)

    this->updateCenter();
}


void MainWindow::mouseDoubleClickEvent(QMouseEvent * event)
{
    QPoint pos = event->pos();
    QPoint mapPoint = getMapPoint(&pos);
    if ((mapPoint.x() <= 0) || (mapPoint.y() <= 0)) return;

    Position newCenter = this->mapView->mapToScene(mapPoint);
    this->ui->statusBar->showMessage(QString::number(newCenter.latitude()) + " " + QString::number(newCenter.longitude()));

    if (event->button() == Qt::LeftButton)
        this->mapView->zoomIn();
    else
        this->mapView->zoomOut();

    this->mapView->centerOn(newCenter.lonLat());
    this->updateCenter();
}


void MainWindow::mouseMoveEvent(QMouseEvent * event)
{
    QPoint pos = event->pos();
    QPoint mapPoint = getMapPoint(&pos);
    if ((mapPoint.x() <= 0) || (mapPoint.y() <= 0)) return;

    Position geoPoint = this->mapView->mapToScene(mapPoint);
    this->ui->statusBar->showMessage(QString::number(geoPoint.latitude()) + " " + QString::number(geoPoint.longitude()));

}


void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton)
    {
        // TODO
    }
}

/*
void MainWindow::on_rasterOpacitySlider_sliderMoved(int position)
{
    this->rasterObj->setOpacity(position / 100.0);
}*/


void MainWindow::on_actionMapTerrain_triggered()
{
    this->setMapSource(OSMTileSource::Terrain);
}


void MainWindow::on_actionMapOpenStreetMap_triggered()
{
    this->setMapSource(OSMTileSource::OSMTiles);
}


void MainWindow::on_actionMapESRISatellite_triggered()
{
    this->setMapSource(OSMTileSource::ESRIWorldImagery);
}


void MainWindow::setMapSource(OSMTileSource::OSMTileType mySource)
{
    // set menu
    ui->actionMapOpenStreetMap->setChecked(false);
    ui->actionMapTerrain->setChecked(false);
    ui->actionMapESRISatellite->setChecked(false);

    if (mySource == OSMTileSource::OSMTiles)
    {
        ui->actionMapOpenStreetMap->setChecked(true);
    }
    else if (mySource == OSMTileSource::Terrain)
    {
        ui->actionMapTerrain->setChecked(true);
    }
    else if (mySource == OSMTileSource::ESRIWorldImagery)
    {
        ui->actionMapESRISatellite->setChecked(true);
    }

    // set tiles source
    QSharedPointer<OSMTileSource> myTiles(new OSMTileSource(mySource), &QObject::deleteLater);
    QSharedPointer<CompositeTileSource> composite(new CompositeTileSource(), &QObject::deleteLater);
    composite->addSourceBottom(myTiles);

    this->mapView->setTileSource(composite);
}


QPoint MainWindow::getMapPoint(QPoint* point) const
{
    QPoint mapPoint;
    int dx, dy;
    dx = this->ui->widgetMap->x();
    dy = this->ui->widgetMap->y() + this->ui->menuBar->height();
    mapPoint.setX(point->x() - dx);
    mapPoint.setY(point->y() - dy);
    return mapPoint;
}


void MainWindow::addRaster(QString fileName, gis::Crit3DRasterGrid *myRaster)
{
    if (ui->groupBox->layout() == nullptr)
    {
        QVBoxLayout *vbox = new QVBoxLayout;
        vbox->setAlignment(Qt::AlignTop);
        ui->groupBox->setLayout(vbox);
    }

    QCheckBox *newCheckBox = new QCheckBox(fileName);
    newCheckBox->setCheckState(Qt::Checked);
    ui->groupBox->layout()->addWidget(newCheckBox);

    // TODO non funziona
    ColorLegend* myColorScale = new ColorLegend();
    myColorScale->resize(ui->groupBox->width(), 100);
    myColorScale->colorScale = myRaster->colorScale;
    ui->groupBox->layout()->addWidget(myColorScale);

    RasterObject* newRasterObj = new RasterObject(this->mapView);
    newRasterObj->setOpacity(0.66);
    newRasterObj->setColorLegend(myColorScale);
    newRasterObj->initializeUTM(myRaster, myProject.gisSettings, false);
    this->rasterObjList.push_back(newRasterObj);

    this->mapView->scene()->addObject(newRasterObj);
    newRasterObj->redrawRequested();
}


void MainWindow::on_actionLoadRaster_triggered()
{
    QString fileNameWithPath = QFileDialog::getOpenFileName(this, tr("Open raster Grid"), "", tr("ESRI grid files (*.flt)"));
    if (fileNameWithPath == "") return;

    qDebug() << "loading raster";
    if (! myProject.loadRaster(fileNameWithPath))
        return;

    QString fileName = getFileName(fileNameWithPath);
    this->addRaster(fileName, myProject.rasterList.back());

    // resize and center map
    gis::Crit3DGeoPoint* center = this->rasterObjList.back()->getRasterCenter();
    float size = this->rasterObjList.back()->getRasterMaxSize();
    this->mapView->setZoomLevel(quint8(log2(ui->widgetMap->width() / size)));
    this->mapView->centerOn(qreal(center->longitude), qreal(center->latitude));
    this->updateCenter();
}

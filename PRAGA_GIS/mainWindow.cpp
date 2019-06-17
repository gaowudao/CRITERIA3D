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
#include <QWidget>
#include <QFrame>
#include <QVBoxLayout>

#include "tileSources/CompositeTileSource.h"

#include "gis.h"
#include "gisProject.h"
#include "dbfTableDialog.h"
#include "dbfNumericFieldsDialog.h"
#include "ucmDialog.h"


#include "mainWindow.h"
#include "ui_mainWindow.h"


#define MAPBORDER 10
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

    connect(ui->checkList, &QListWidget::itemClicked, [=](QListWidgetItem* item){ this->itemClicked(item); });
    ui->checkList->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->checkList, &QListWidget::customContextMenuRequested, [=](const QPoint point){ this->itemMenuRequested(point); });

}


MainWindow::~MainWindow()
{
    if (! this->rasterObjList.empty())
        for (unsigned int i = 0; i < this->rasterObjList.size(); i++)
            delete this->rasterObjList[i];

    ui->checkList->clear();
    delete mapView;
    delete mapScene;
    delete ui;
}


void MainWindow::resizeEvent(QResizeEvent * event)
{
    Q_UNUSED(event)

    ui->widgetMap->setGeometry(TOOLSWIDTH, 0, this->width()-TOOLSWIDTH, this->height() - INFOHEIGHT);
    mapView->resize(ui->widgetMap->size());

    ui->checkList->move(MAPBORDER/2, MAPBORDER);
    ui->checkList->resize(TOOLSWIDTH, this->height() - INFOHEIGHT - MAPBORDER * 2);
}


void MainWindow::updateCenter()
{
    if (! this->rasterObjList.empty())
        for (unsigned int i = 0; i < this->rasterObjList.size(); i++)
            this->rasterObjList[i]->updateCenter();
    if (! this->shapeObjList.empty())
        for (unsigned int i = 0; i < this->shapeObjList.size(); i++)
            this->shapeObjList[i]->updateCenter();
}


void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
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


void MainWindow::wheelEvent(QWheelEvent * event)
{
    this->updateCenter();
}


void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton)
    {
        // TODO
    }
}


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


void MainWindow::addRasterObject(GisObject* myObject)
{
    QListWidgetItem* item = new QListWidgetItem("[RASTER] " + myObject->fileName);
    item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
    item->setCheckState(Qt::Checked);
    ui->checkList->addItem(item);

    RasterObject* newRasterObj = new RasterObject(this->mapView);
    newRasterObj->setOpacity(0.66);
    newRasterObj->initializeUTM(myObject->getRaster(), myProject.gisSettings, false);
    this->rasterObjList.push_back(newRasterObj);

    this->mapView->scene()->addObject(newRasterObj);
    newRasterObj->redrawRequested();
}


void MainWindow::addShapeObject(GisObject* myObject)
{

    QListWidgetItem* item = new QListWidgetItem("[SHAPE] " + myObject->fileName);
    item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
    item->setCheckState(Qt::Checked);
    ui->checkList->addItem(item);

    MapGraphicsShapeObject* newShapeObj = new MapGraphicsShapeObject(this->mapView);
    newShapeObj->setOpacity(0.5);
    newShapeObj->initializeUTM(myObject->getShapeHandler(), myProject.gisSettings);
    this->shapeObjList.push_back(newShapeObj);

    this->mapView->scene()->addObject(newShapeObj);
    newShapeObj->redrawRequested();
}


void MainWindow::on_actionLoadRaster_triggered()
{
    QString fileNameWithPath = QFileDialog::getOpenFileName(this, tr("Open raster Grid"), "", tr("ESRI grid files (*.flt)"));
    if (fileNameWithPath == "") return;

    if (! myProject.loadRaster(fileNameWithPath))
        return;

    this->addRasterObject(myProject.objectList.back());

    // resize and center map
    gis::Crit3DGeoPoint* center = this->rasterObjList.back()->getRasterCenter();
    float size = this->rasterObjList.back()->getRasterMaxSize();
    this->mapView->setZoomLevel(quint8(log2(ui->widgetMap->width() / size)));
    this->mapView->centerOn(qreal(center->longitude), qreal(center->latitude));
    this->updateCenter();
}


void MainWindow::on_actionLoadShapefile_triggered()
{
    QString fileNameWithPath = QFileDialog::getOpenFileName(this, tr("Open Shapefile"), "", tr("shp files (*.shp)"));
    if (fileNameWithPath == "") return;

    if (! myProject.loadShapefile(fileNameWithPath))
        return;

    this->addShapeObject(myProject.objectList.back());

    // TODO resize and center map
}

void MainWindow::itemClicked(QListWidgetItem* item)
{

    int pos = ui->checkList->row(item);
    GisObject* myObject = myProject.objectList.at(unsigned(pos));

    if (myObject->type == gisObjectRaster)
    {
        unsigned int i;
        for (i = 0; i < rasterObjList.size(); i++)
        {
            if (rasterObjList.at(i)->getRaster() == myObject->getRaster())
            {
                break;
            }
        }

        myObject->isSelected = item->checkState();
        rasterObjList.at(i)->setVisible(myObject->isSelected);
    }
    else if (myObject->type == gisObjectShape)
    {
        unsigned int i;
        for (i = 0; i < shapeObjList.size(); i++)
        {
            if (shapeObjList.at(i)->getShapePointer() == myObject->getShapeHandler())
            {
                break;
            }
        }

        myObject->isSelected = item->checkState();
        shapeObjList.at(i)->setVisible(myObject->isSelected);
    }
}


void MainWindow::itemMenuRequested(const QPoint point)
{
    QPoint itemPoint = ui->checkList->mapToGlobal(point);
    QListWidgetItem* item = ui->checkList->itemAt(point);
    int pos = ui->checkList->row(item);
    GisObject* myObject = myProject.objectList.at(unsigned(pos));

    QMenu submenu;
    submenu.addAction("Close");
    if (myObject->type == gisObjectShape)
    {
        submenu.addAction("Show data");
        submenu.addAction("Open attribute table");
    }
    else if (myObject->type == gisObjectRaster)
    {
        submenu.addAction("Save as");
    }
    QAction* rightClickItem = submenu.exec(itemPoint);

    if (rightClickItem && rightClickItem->text().contains("Close") )
    {
        unsigned int i;
        if (myObject->type == gisObjectRaster)
        {
            for (i = 0; i < rasterObjList.size(); i++)
            {
                if (rasterObjList.at(i)->getRaster() == myObject->getRaster()) break;
            }
            if (i < rasterObjList.size())
            {
                // remove from scene
                this->mapView->scene()->removeObject(this->rasterObjList.at(i));
                this->rasterObjList.at(i)->clear();
                this->rasterObjList.erase(this->rasterObjList.begin()+i);
            }
        }
        else if (myObject->type == gisObjectShape)
        {
            for (i = 0; i < shapeObjList.size(); i++)
            {
                if (shapeObjList.at(i)->getShapePointer() == myObject->getShapeHandler()) break;
            }
            // remove from scene
            this->mapView->scene()->removeObject(this->shapeObjList.at(i));
            this->shapeObjList.at(i)->clear();
            this->shapeObjList.erase(this->shapeObjList.begin()+i);
        }

        myObject->close();
        myProject.objectList.erase(myProject.objectList.begin()+pos);

        ui->checkList->takeItem(ui->checkList->indexAt(point).row());
    }

    else if (rightClickItem && rightClickItem->text().contains("Show data"))
    {
        ShowProperties showData(myObject->getShapeHandler(), myObject->fileName);
    }
    else if (rightClickItem && rightClickItem->text().contains("Open attribute table"))
    {
        DbfTableDialog Table(myObject->getShapeHandler(), myObject->fileName);

    }
    else if (rightClickItem && rightClickItem->text().contains("Save as"))
    {
        DbfTableDialog Table(myObject->getShapeHandler(), myObject->fileName);

    }
    return;
}

void MainWindow::on_actionRasterize_shape_triggered()
{
    QListWidgetItem * itemSelected = ui->checkList->currentItem();
    if (itemSelected == nullptr)
    {
        QMessageBox::information(nullptr, "No items selected", "Select a shape");
        return;
    }
    else if (!itemSelected->text().contains("SHAPE"))
    {
        QMessageBox::information(nullptr, "No shape selected", "Select a shape");
        return;
    }
    else
    {
        int pos = ui->checkList->row(itemSelected);
        GisObject* myObject = myProject.objectList.at(unsigned(pos));
        DbfNumericFieldsDialog numericFields(myObject->getShapeHandler(), myObject->fileName);
        if (numericFields.result() == QDialog::Accepted)
        {
            myProject.getRasterFromShape(myObject->getShapeHandler(), numericFields.getFieldSelected(), numericFields.getOutputName(), numericFields.getCellSize(), true);
            addRasterObject(myProject.objectList.back());
            this->updateCenter();
        }
    }
    return;

}

void MainWindow::on_actionCompute_Unit_Crop_Map_triggered()
{

    if (shapeObjList.empty())
    {
        QMessageBox::information(nullptr, "No shape loaded", "Load a shape");
        return;
    }

    UcmDialog ucmDialog(shapeObjList);
    if (ucmDialog.result() == QDialog::Rejected)
        return;

    if (myProject.addUnitCropMap(ucmDialog.getCrop(), ucmDialog.getSoil(), ucmDialog.getMeteo(), ucmDialog.getIdSoil().toStdString(), ucmDialog.getIdMeteo().toStdString(), ucmDialog.getOutputName(), ucmDialog.getCellSize()))
    {
        addShapeObject(myProject.objectList.back());
    }

    return;

}


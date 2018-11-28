#include <QGridLayout>
#include <QFileDialog>
#include <QtDebug>
#include <QMessageBox>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QListWidget>
#include <QRadioButton>
#include <QTextBrowser>
#include <QLineEdit>
#include <QLabel>

#include <sstream>
#include <iostream>
#include <fstream>

#include "tileSources/OSMTileSource.h"
#include "tileSources/GridTileSource.h"
#include "tileSources/CompositeTileSource.h"

#include "formInfo.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Position.h"
#include "dbMeteoPoints.h"
#include "crit3dProject.h"
#include "utilities.h"
#include "commonConstants.h"
#include "dialogWindows.h"
#include "gis.h"
#include "spatialControl.h"
#include "interpolationDialog.h"
#include "settingsDialog.h"

#include <Qt3DRender/QCamera>
#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DExtras/QFirstPersonCameraController>
#include <Qt3DInput>

#include "scene.h"

extern Crit3DProject myProject;

#define MAPBORDER 8
#define TOOLSWIDTH 260

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->showPoints = true;

    this->myRubberBand = NULL;

    // Set the MapGraphics Scene and View
    this->mapScene = new MapGraphicsScene(this);
    this->mapView = new MapGraphicsView(mapScene, this->ui->widgetMap);

    this->rasterLegend = new ColorLegend(this->ui->widgetColorLegendRaster);
    this->rasterLegend->resize(this->ui->widgetColorLegendRaster->size());

    this->meteoGridLegend = new ColorLegend(this->ui->widgetColorLegendGrid);
    this->meteoGridLegend->resize(this->ui->widgetColorLegendGrid->size());

    this->meteoPointsLegend = new ColorLegend(this->ui->widgetColorLegendPoints);
    this->meteoPointsLegend->resize(this->ui->widgetColorLegendPoints->size());
    this->meteoPointsLegend->colorScale = myProject.meteoPointsColorScale;

    // Set tiles source
    this->setMapSource(OSMTileSource::OSMTiles);

    // Set start size and position
    this->startCenter = new Position (myProject.gisSettings.startLocation.longitude, myProject.gisSettings.startLocation.latitude, 0.0);
    this->mapView->setZoomLevel(8);
    this->mapView->centerOn(startCenter->lonLat());
    this->setMapSource(OSMTileSource::Terrain);

    // Set raster objects
    this->rasterObj = new RasterObject(this->mapView);
    this->meteoGridObj = new RasterObject(this->mapView);

    this->rasterObj->setOpacity(this->ui->rasterOpacitySlider->value() / 100.0);
    this->meteoGridObj->setOpacity(this->ui->meteoGridOpacitySlider->value() / 100.0);

    this->rasterObj->setColorLegend(this->rasterLegend);
    this->meteoGridObj->setColorLegend(this->meteoGridLegend);

    this->mapView->scene()->addObject(this->rasterObj);
    this->mapView->scene()->addObject(this->meteoGridObj);

    this->updateVariable();
    this->updateDateTime();

    connect(this->ui->dateEdit, SIGNAL(editingFinished()), this, SLOT(on_dateChanged()));

    this->setMouseTracking(true);
}


MainWindow::~MainWindow()
{
    delete rasterObj;
    delete meteoGridObj;
    delete rasterLegend;
    delete meteoGridLegend;
    delete meteoPointsLegend;
    delete mapView;
    delete mapScene;
    delete ui;
}


void MainWindow::resizeEvent(QResizeEvent * event)
{
    Q_UNUSED(event)
    const int INFOHEIGHT = 40;

    ui->widgetMap->setGeometry(TOOLSWIDTH, 0, this->width()-TOOLSWIDTH, this->height() - INFOHEIGHT);
    mapView->resize(ui->widgetMap->size());

    ui->groupBoxVariable->move(MAPBORDER/2, MAPBORDER);
    ui->groupBoxVariable->resize(TOOLSWIDTH, ui->groupBoxVariable->height());

    ui->groupBoxMeteoPoints->move(MAPBORDER/2, ui->groupBoxVariable->y() + ui->groupBoxVariable->height() + MAPBORDER);
    ui->groupBoxMeteoPoints->resize(TOOLSWIDTH, ui->groupBoxMeteoPoints->height());

    ui->groupBoxMeteoGrid->move(MAPBORDER/2, ui->groupBoxMeteoPoints->y() + ui->groupBoxMeteoPoints->height() + MAPBORDER);
    ui->groupBoxMeteoGrid->resize(TOOLSWIDTH, ui->groupBoxMeteoGrid->height());

    ui->groupBoxRaster->move(MAPBORDER/2, ui->groupBoxMeteoGrid->y() + ui->groupBoxMeteoGrid->height() + MAPBORDER);
    ui->groupBoxRaster->resize(TOOLSWIDTH, ui->groupBoxRaster->height());

    // TODO sembrano non funzionare
    ui->widgetColorLegendRaster->resize(TOOLSWIDTH, ui->widgetColorLegendPoints->height());
    ui->widgetColorLegendPoints->resize(TOOLSWIDTH, ui->widgetColorLegendPoints->height());
}


void MainWindow::mouseReleaseEvent(QMouseEvent *event){
    Q_UNUSED(event)

    this->rasterObj->updateCenter();
    this->meteoGridObj->updateCenter();

    gis::Crit3DGeoPoint pointSelected;

    if (myRubberBand != NULL && myRubberBand->isVisible())
    {
        QPointF lastCornerOffset = event->localPos();
        QPointF firstCornerOffset = myRubberBand->getFirstCorner();
        QPoint pixelTopLeft;
        QPoint pixelBottomRight;

        if (firstCornerOffset.y() > lastCornerOffset.y())
        {
            if (firstCornerOffset.x() > lastCornerOffset.x())
            {
                qDebug() << "bottom to left";
                pixelTopLeft = lastCornerOffset.toPoint();
                pixelBottomRight = firstCornerOffset.toPoint();
            }
            else
            {
                qDebug() << "bottom to right";

                pixelTopLeft = QPoint(firstCornerOffset.toPoint().x(), lastCornerOffset.toPoint().y());
                pixelBottomRight = QPoint(lastCornerOffset.toPoint().x(), firstCornerOffset.toPoint().y());
            }
        }
        else
        {
            if (firstCornerOffset.x() > lastCornerOffset.x())
            {
                qDebug() << "top to left";
                pixelTopLeft = QPoint(lastCornerOffset.toPoint().x(), firstCornerOffset.toPoint().y());
                pixelBottomRight = QPoint(firstCornerOffset.toPoint().x(), lastCornerOffset.toPoint().y());
            }
            else
            {
                qDebug() << "top to right";
                pixelTopLeft = firstCornerOffset.toPoint();
                pixelBottomRight = lastCornerOffset.toPoint();
            }
        }

        QPointF topLeft = this->mapView->mapToScene(getMapPoint(&pixelTopLeft));
        QPointF bottomRight = this->mapView->mapToScene(getMapPoint(&pixelBottomRight));
        QRectF rectF(topLeft, bottomRight);

        foreach (StationMarker* marker, pointList)
        {
            if (rectF.contains(marker->longitude(), marker->latitude()))
            {
                if ( marker->color() ==  Qt::white )
                {
                    marker->setFillColor(QColor((Qt::red)));
                    pointSelected.latitude = marker->latitude();
                    pointSelected.longitude = marker->longitude();
                    myProject.meteoPointsSelected << pointSelected;
                }
            }
        }
        myRubberBand->hide();
    }
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

    this->rasterObj->updateCenter();
    this->meteoGridObj->updateCenter();
}


void MainWindow::mouseMoveEvent(QMouseEvent * event)
{
    QPoint pos = event->pos();
    QPoint mapPoint = getMapPoint(&pos);
    if ((mapPoint.x() <= 0) || (mapPoint.y() <= 0)) return;

    Position geoPoint = this->mapView->mapToScene(mapPoint);
    this->ui->statusBar->showMessage(QString::number(geoPoint.latitude()) + " " + QString::number(geoPoint.longitude()));

    if (myRubberBand != NULL)
    {
        myRubberBand->setGeometry(QRect(myRubberBand->getOrigin(), mapPoint).normalized());
    }
}


void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton)
    {
        if (myRubberBand != NULL)
        {
            QPoint pos = event->pos();
            QPointF firstCorner = event->localPos();
            myRubberBand->setFirstCorner(firstCorner);
            QPoint mapPoint = getMapPoint(&pos);
            myRubberBand->setOrigin(mapPoint);
            myRubberBand->setGeometry(QRect(mapPoint, QSize()));
            myRubberBand->show();
        }

        #ifdef NETCDF
        if (myProject.netCDF.isLoaded)
        {
            QPoint pos = event->pos();
            Position myPos = mapView->mapToScene(getMapPoint(&pos));
            gis::Crit3DGeoPoint geoPoint = gis::Crit3DGeoPoint(myPos.latitude(), myPos.longitude());

            exportNetCDFDataSeries(geoPoint);
        }
        #endif
    }
}


void MainWindow::on_rasterOpacitySlider_sliderMoved(int position)
{
    this->rasterObj->setOpacity(position / 100.0);
}


void MainWindow::on_meteoGridOpacitySlider_sliderMoved(int position)
{
    this->meteoGridObj->setOpacity(position / 100.0);
}


void MainWindow::on_actionMapToner_triggered()
{
    this->setMapSource(OSMTileSource::TonerLite);
}


void MainWindow::on_actionMapOpenStreetMap_triggered()
{
    this->setMapSource(OSMTileSource::OSMTiles);
}


void MainWindow::on_actionMapESRISatellite_triggered()
{
    this->setMapSource(OSMTileSource::ESRIWorldImagery);
}


void MainWindow::on_actionMapTerrain_triggered()
{
    this->setMapSource(OSMTileSource::Terrain);
}


void MainWindow::on_actionRectangle_Selection_triggered()
{
    if (myRubberBand != NULL)
    {
        delete myRubberBand;
        myRubberBand = NULL;
    }

    if (ui->actionRectangle_Selection->isChecked())
    {
        myRubberBand = new RubberBand(QRubberBand::Rectangle, this->mapView);
        QPoint origin(this->mapView->width()*0.5 , this->mapView->height()*0.5);
        QPoint mapPoint = getMapPoint(&origin);
        myRubberBand->setOrigin(mapPoint);
        myRubberBand->setGeometry(QRect(myRubberBand->getOrigin(), QSize()));
        myRubberBand->show();
     }
}


void MainWindow::on_actionLoadDEM_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open raster Grid"), "", tr("ESRI grid files (*.flt)"));

    if (fileName == "") return;

    qDebug() << "loading raster";
    if (!myProject.loadDEM(fileName)) return;

    this->setCurrentRaster(&(myProject.DTM));
    ui->labelRasterScale->setText(QString::fromStdString(getVariableString(noMeteoTerrain)));
    this->ui->rasterOpacitySlider->setEnabled(true);

    // center map
    gis::Crit3DGeoPoint* center = this->rasterObj->getRasterCenter();
    this->mapView->centerOn(qreal(center->longitude), qreal(center->latitude));

    // resize map
    float size = this->rasterObj->getRasterMaxSize();
    size = log2(1000.0/size);
    this->mapView->setZoomLevel(quint8(size));
    this->mapView->centerOn(qreal(center->longitude), qreal(center->latitude));

    // active raster object
    this->rasterObj->updateCenter();
}


void MainWindow::on_actionOpen_meteo_points_DB_triggered()
{
    QString dbName = QFileDialog::getOpenFileName(this, tr("Open DB meteo"), "", tr("DB files (*.db)"));

    if (dbName != "")
    {
        this->loadMeteoPointsDB(dbName);
    }
    redrawMeteoPoints(true);
}


void MainWindow::on_actionOpen_meteo_grid_triggered()
{
    QString xmlName = QFileDialog::getOpenFileName(this, tr("Open XML DB grid"), "", tr("xml files (*.xml)"));

    if (xmlName != "")
    {
        this->loadMeteoGridDB(xmlName);
    }

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


void MainWindow::resetMeteoPoints()
{
    for (int i = 0; i < this->pointList.size(); i++)
        this->mapView->scene()->removeObject(this->pointList[i]);

    this->pointList.clear();

    this->myRubberBand = NULL;
}


void MainWindow::on_actionVariableQualitySpatial_triggered()
{
    myProject.checkSpatialQuality = ui->actionVariableQualitySpatial->isChecked();
    updateVariable();
}


void MainWindow::interpolateDemGUI()
{
    meteoVariable myVar = myProject.getCurrentVariable();

    if (myProject.interpolationDemMain(myVar,myProject.getCurrentTime(), &(myProject.dataRaster), true))
    {
        setColorScale(myVar, myProject.dataRaster.colorScale);
        setCurrentRaster(&(myProject.dataRaster));

        ui->labelRasterScale->setText(QString::fromStdString(getVariableString(myVar)));
    }
    else
        myProject.logError();
}


void MainWindow::updateVariable()
{
    // FREQUENCY
    if (myProject.getFrequency() == noFrequency)
    {
        this->ui->labelFrequency->setText("None");
    }
    else
    {
        if (myProject.currentVariable != noMeteoVar)
        {
            this->ui->actionShowLocation->setChecked(false);
            if (this->meteoGridObj != NULL) this->meteoGridObj->setDrawBorders(false);
        }

        if (myProject.getFrequency() == daily)
        {
            this->ui->labelFrequency->setText("Daily");

            //check
            if (myProject.currentVariable == airTemperature)
                myProject.currentVariable = dailyAirTemperatureAvg;

            else if (myProject.currentVariable == precipitation)
                myProject.currentVariable = dailyPrecipitation;

            else if (myProject.currentVariable == globalIrradiance)
                myProject.currentVariable = dailyGlobalRadiation;

            else if (myProject.currentVariable == airRelHumidity)
                myProject.currentVariable = dailyAirRelHumidityAvg;
        }

        else if (myProject.getFrequency() == hourly)
        {
            this->ui->labelFrequency->setText("Hourly");

            //check
            if ((myProject.currentVariable == dailyAirTemperatureAvg)
                    || (myProject.currentVariable == dailyAirTemperatureMax)
                    || (myProject.currentVariable == dailyAirTemperatureMin))
                myProject.currentVariable = airTemperature;

            else if ((myProject.currentVariable == dailyAirRelHumidityAvg)
                     || (myProject.currentVariable == dailyAirRelHumidityMax)
                     || (myProject.currentVariable == dailyAirRelHumidityMin))
                 myProject.currentVariable = airRelHumidity;

            else if (myProject.currentVariable == dailyPrecipitation)
                    myProject.currentVariable = precipitation;

            else if (myProject.currentVariable == dailyGlobalRadiation)
                myProject.currentVariable = globalIrradiance;
        }
    }

    std::string myString = getVariableString(myProject.currentVariable);
    ui->labelVariable->setText(QString::fromStdString(myString));

    redrawMeteoPoints(true);
    redrawMeteoGrid();
}


void MainWindow::updateDateTime()
{
    int myHour = myProject.getCurrentHour();
    this->ui->dateEdit->setDate(myProject.getCurrentDate());
    this->ui->timeEdit->setTime(QTime(myHour,0,0));

}


void MainWindow::on_dateChanged()
{

    QDate date = this->ui->dateEdit->date();

    if (date != myProject.getCurrentDate())
    {

        myProject.setCurrentDate(date);
        myProject.loadMeteoPointsData(date, date, true);
        myProject.loadMeteoGridData(date, date, true);

    }

    redrawMeteoPoints(true);
    redrawMeteoGrid();
}


void MainWindow::on_timeEdit_timeChanged(const QTime &time)
{
    //hour
    if (time.hour() != myProject.getCurrentHour())
    {
        myProject.setCurrentHour(time.hour());
    }

    redrawMeteoPoints(true);
    redrawMeteoGrid();
}


void MainWindow::redrawMeteoPoints(bool updateColorSCale)
{
    if (myProject.nrMeteoPoints == 0)
        return;

    // hide all meteo points
    for (int i = 0; i < myProject.nrMeteoPoints; i++)
        pointList[i]->setVisible(false);

    if (! this->showPoints)
        return;

    meteoPointsLegend->setVisible(true);
    // show location
    if (myProject.getCurrentVariable() == noMeteoVar)
    {

        for (int i = 0; i < myProject.nrMeteoPoints; i++)
        {
                myProject.meteoPoints[i].currentValue = NODATA;
                pointList[i]->setFillColor(QColor(Qt::white));
                pointList[i]->setRadius(5);
                pointList[i]->setToolTip(&(myProject.meteoPoints[i]));
                pointList[i]->setVisible(true);
        }

        myProject.meteoPointsColorScale->setRange(NODATA, NODATA);
        meteoPointsLegend->update();
        return;
    }

    // quality control
    checkData(myProject.quality, myProject.getCurrentVariable(),
              myProject.meteoPoints, myProject.nrMeteoPoints, myProject.getCurrentTime(),
              &myProject.qualityInterpolationSettings, myProject.checkSpatialQuality);

    if (updateColorSCale)
    {
        float minimum, maximum;
        myProject.getMeteoPointsRange(&minimum, &maximum);

        myProject.meteoPointsColorScale->setRange(minimum, maximum);
    }

    roundColorScale(myProject.meteoPointsColorScale, 4, true);
    setColorScale(myProject.currentVariable, myProject.meteoPointsColorScale);

    Crit3DColor *myColor;
    for (int i = 0; i < myProject.nrMeteoPoints; i++)
    {
        if (myProject.meteoPoints[i].currentValue != NODATA)
        {
            if (myProject.meteoPoints[i].quality == quality::accepted)
            {
                pointList[i]->setRadius(5);
                myColor = myProject.meteoPointsColorScale->getColor(myProject.meteoPoints[i].currentValue);
                pointList[i]->setFillColor(QColor(myColor->red, myColor->green, myColor->blue));
                pointList[i]->setOpacity(1.0);
            }
            else
            {
                // Wrong data
                pointList[i]->setRadius(10);
                pointList[i]->setFillColor(QColor(Qt::black));
                pointList[i]->setOpacity(0.5);
            }

            pointList[i]->setToolTip(&(myProject.meteoPoints[i]));
            pointList[i]->setVisible(true);
        }
    }

    meteoPointsLegend->update();
}

void MainWindow::redrawMeteoGrid()
{

    if (myProject.meteoGridDbHandler == NULL)
        return;

    frequencyType frequency = myProject.getFrequency();
    meteoVariable variable = myProject.getCurrentVariable();

    if (myProject.getCurrentVariable() == noMeteoVar)
    {
        meteoGridLegend->setVisible(false);
        ui->labelMeteoGridScale->setText("");
        meteoGridObj->redrawRequested();
        return;
    }

    Crit3DTime time = myProject.getCurrentTime();

    if (frequency == daily)
    {
        myProject.meteoGridDbHandler->meteoGrid()->fillMeteoPointCurrentDailyValue(time.date, variable);
    }
    else if (frequency == hourly)
    {
        myProject.meteoGridDbHandler->meteoGrid()->fillMeteoPointCurrentHourlyValue(time.date, time.getHour(), time.getMinutes(), variable);
    }
    else
        return;

    myProject.meteoGridDbHandler->meteoGrid()->fillMeteoRaster();
    meteoGridLegend->setVisible(true);

    setColorScale(variable, myProject.meteoGridDbHandler->meteoGrid()->dataMeteoGrid.colorScale);
    ui->labelMeteoGridScale->setText(QString::fromStdString(getVariableString(myProject.currentVariable)));

    meteoGridObj->redrawRequested();
    meteoGridLegend->update();
}


bool MainWindow::loadMeteoPointsDB(QString dbName)
{
    this->resetMeteoPoints();

    if (!myProject.loadMeteoPointsDB(dbName))
        return false;

    this->addMeteoPoints();

    if (myProject.meteoGridDbHandler == NULL)
    {
        myProject.loadLastMeteoData();
        this->updateDateTime();
    }
    else
    {
        myProject.loadMeteoPointsData(myProject.getCurrentDate(), myProject.getCurrentDate(), true);
    }

    return true;
}


bool MainWindow::loadMeteoGridDB(QString xmlName)
{
    if (!myProject.loadMeteoGridDB(xmlName))
    {
        myProject.logError();
        return false;
    }

    myProject.meteoGridDbHandler->meteoGrid()->createRasterGrid();

    if (myProject.meteoGridDbHandler->gridStructure().isUTM() == false)
    {
        meteoGridObj->initializeLatLon(&(myProject.meteoGridDbHandler->meteoGrid()->dataMeteoGrid), myProject.gisSettings, myProject.meteoGridDbHandler->gridStructure().header(), true);
    }
    else
    {
        meteoGridObj->initializeUTM(&(myProject.meteoGridDbHandler->meteoGrid()->dataMeteoGrid), myProject.gisSettings, true);
    }

    meteoGridLegend->colorScale = myProject.meteoGridDbHandler->meteoGrid()->dataMeteoGrid.colorScale;
    ui->meteoGridOpacitySlider->setEnabled(true);

    // update dateTime Edit if there are not MeteoPoints
    if (this->pointList.isEmpty())
    {
        myProject.setCurrentDate(myProject.meteoGridDbHandler->lastDate());
        this->updateDateTime();
    }

    myProject.loadMeteoGridData(myProject.getCurrentDate(), myProject.getCurrentDate(), true);

    meteoGridObj->redrawRequested();

    redrawMeteoGrid();

    return true;
}


void MainWindow::addMeteoPoints()
{
    myProject.meteoPointsSelected.clear();
    for (int i = 0; i < myProject.nrMeteoPoints; i++)
    {
        StationMarker* point = new StationMarker(5.0, true, QColor((Qt::white)), this->mapView);

        point->setFlag(MapGraphicsObject::ObjectIsMovable, false);
        point->setLatitude(myProject.meteoPoints[i].latitude);
        point->setLongitude(myProject.meteoPoints[i].longitude);

        this->pointList.append(point);
        this->mapView->scene()->addObject(this->pointList[i]);

        point->setToolTip(&(myProject.meteoPoints[i]));
    }
}


void MainWindow::setMapSource(OSMTileSource::OSMTileType mySource)
{
    QSharedPointer<OSMTileSource> myTiles(new OSMTileSource(mySource), &QObject::deleteLater);
    QSharedPointer<CompositeTileSource> composite(new CompositeTileSource(), &QObject::deleteLater);
    composite->addSourceBottom(myTiles);

    this->mapView->setTileSource(composite);
}


void MainWindow::on_rasterScaleButton_clicked()
{
    if (this->rasterObj->currentRaster == NULL)
    {
        QMessageBox::information(NULL, "No Raster", "Load raster before");
        return;
    }

    meteoVariable myVar = chooseColorScale();
    if (myVar != noMeteoVar)
    {
        setColorScale(myVar, this->rasterObj->currentRaster->colorScale);
        ui->labelRasterScale->setText(QString::fromStdString(getVariableString(myVar)));
    }
}

void MainWindow::on_variableButton_clicked()
{
    if (chooseMeteoVariable(&myProject))
    {
       this->ui->actionShowLocation->setChecked(false);
       this->updateVariable();
       if (this->meteoGridObj != NULL) this->meteoGridObj->setDrawBorders(false);
    }
}

void MainWindow::on_frequencyButton_clicked()
{
   frequencyType myFrequency = chooseFrequency();

   if (myFrequency != noFrequency)
   {
       myProject.setFrequency(myFrequency);
       this->updateVariable();
   }
}

void MainWindow::on_actionPointsVisible_triggered()
{
    this->showPoints = ui->actionPointsVisible->isChecked();
    redrawMeteoPoints(false);
}

void MainWindow::on_rasterRestoreButton_clicked()
{
    if (this->rasterObj->currentRaster == NULL)
    {
        QMessageBox::information(NULL, "No Raster", "Load raster before");
        return;
    }

    setDefaultDTMScale(myProject.DTM.colorScale);
    this->setCurrentRaster(&(myProject.DTM));
    ui->labelRasterScale->setText(QString::fromStdString(getVariableString(noMeteoTerrain)));
}

void MainWindow::setCurrentRaster(gis::Crit3DRasterGrid *myRaster)
{
    this->rasterObj->initializeUTM(myRaster, myProject.gisSettings, false);
    this->rasterLegend->colorScale = myRaster->colorScale;
    this->rasterObj->redrawRequested();
}

void MainWindow::on_dateEdit_dateChanged(const QDate &date)
{
    Q_UNUSED(date);
    this->on_dateChanged();
}

void MainWindow::on_actionClose_meteo_points_triggered()
{
    resetMeteoPoints();
    meteoPointsLegend->setVisible(false);
    myProject.closeMeteoPointsDB();
}

void MainWindow::on_actionClose_meteo_grid_triggered()
{
    if (myProject.meteoGridDbHandler != NULL)
    {
        myProject.meteoGridDbHandler->meteoGrid()->dataMeteoGrid.isLoaded = false;
        meteoGridObj->clean();
        meteoGridObj->redrawRequested();
        meteoGridLegend->setVisible(false);
        myProject.closeMeteoGridDB();
        ui->meteoGridOpacitySlider->setEnabled(false);
    }
}


void MainWindow::on_actionInterpolation_to_DTM_triggered()
{
    FormInfo myInfo;
    myInfo.start("Interpolation...", 0);

    interpolateDemGUI();

    myInfo.close();
}


void MainWindow::on_actionInterpolationSettings_triggered()
{
    if (myProject.meteoPointsDbHandler == NULL)
    {
        QMessageBox::information(NULL, "No DB open", "Open DB Points");
        return;
    }

    InterpolationDialog* myInterpolationDialog = new InterpolationDialog(&myProject);
    myInterpolationDialog->close();
}


void MainWindow::on_actionParameters_triggered()
{
    SettingsDialog* mySettingsDialog = new SettingsDialog(myProject.pathSetting, myProject.settings, &myProject.gisSettings, myProject.quality, myProject.meteoSettings);
    mySettingsDialog->exec();
    if (startCenter->latitude() != myProject.gisSettings.startLocation.latitude || startCenter->longitude() != myProject.gisSettings.startLocation.longitude)
    {
        startCenter->setLatitude(myProject.gisSettings.startLocation.latitude);
        startCenter->setLongitude(myProject.gisSettings.startLocation.longitude);
        this->mapView->centerOn(startCenter->lonLat());
    }

    mySettingsDialog->close();
}


void MainWindow::on_actionShowLocation_triggered()
{
    myProject.currentVariable = noMeteoVar;
    this->ui->actionShowLocation->setChecked(true);
    if (this->meteoGridObj != NULL) this->meteoGridObj->setDrawBorders(true);
    this->updateVariable();
}


void MainWindow::on_actionOpen_model_parameters_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open DB parameters"), "", tr("SQLite files (*.db)"));
    if (fileName == "") return;

    if (myProject.loadModelParameters(fileName))
        QMessageBox::information(NULL, "", "Model parameters loaded");
}


void MainWindow::on_actionOpen_soil_map_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open soil map"), "", tr("ESRI grid files (*.flt)"));
    if (fileName == "") return;

    if (myProject.loadSoilMap(fileName))
        QMessageBox::information(NULL, "", "Soil map loaded.");
}


void MainWindow::on_actionOpen_soil_data_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Load DB soil"), "", tr("SQLite files (*.db)"));
    if (fileName == "") return;

    myProject.loadSoilData(fileName);
}

void MainWindow::on_actionShow_DTM_triggered()
{
    if (myProject.DTM.isLoaded)
    {
        setColorScale(noMeteoTerrain, myProject.DTM.colorScale);
        this->setCurrentRaster(&(myProject.DTM));
    }
}

void MainWindow::on_actionShow_boundary_triggered()
{
    if (myProject.boundaryMap.isLoaded)
    {
        setColorScale(noMeteoTerrain, myProject.boundaryMap.colorScale);
        this->setCurrentRaster(&(myProject.boundaryMap));
    }
}

void MainWindow::on_actionShow_soil_triggered()
{
    if (myProject.soilMap.isLoaded)
    {
        setColorScale(noMeteoTerrain, myProject.soilMap.colorScale);
        this->setCurrentRaster(&(myProject.soilMap));
    }
}

void MainWindow::on_actionCriteria3D_settings_triggered()
{

}


void mouseManager(Qt3DExtras::Qt3DWindow *view3D, QMouseEvent *e)
{
    if(e->buttons() == Qt::RightButton)
    {
        QVector3D camPos = view3D->camera()->position();
        camPos.normalize();
        float dy = float(e->x()) / 300.f;
        camPos = view3D->camera()->position() - QVector3D(dy, dy, dy);
        view3D->camera()->setPosition(camPos);
    }
}


void MainWindow::on_actionCriteria3D_Initialize_triggered()
{
    if (myProject.initializeCriteria3D())
        QMessageBox::information(NULL, "", "Criteria3D initialized.");
}


void MainWindow::on_actionShow_3D_triggered()
{
    if (! myProject.DTM.isLoaded)
    {
        myProject.logError("Missing DTM.");
        return;
    }

    if (! myProject.isInitialized)
        myProject.createIndexMap();

    // 3d Window
    Qt3DExtras::Qt3DWindow *view3D = new Qt3DExtras::Qt3DWindow();
    view3D->defaultFrameGraph()->setClearColor(QColor::fromRgbF(1, 1, 1, 1.0));
    view3D->setTitle("CRITERIA-3D");
    view3D->setWidth(1000);
    view3D->setHeight(600);

    // Camera
    Qt3DRender::QCamera *camera = view3D->camera();
    camera->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 1.f, 1000000.0f);
    camera->setUpVector(QVector3D(0, 0, 1));

    gis::Crit3DUtmPoint utmCenter;

    float dz = maxValue(myProject.DTM.maximum - myProject.DTM.minimum, 10.f);
    float z = myProject.DTM.minimum + dz * 0.5;
    float dy = myProject.DTM.header->nrRows * myProject.DTM.header->cellSize;
    float dx = myProject.DTM.header->nrCols * myProject.DTM.header->cellSize;
    utmCenter.x = myProject.DTM.header->llCorner->x + dx * 0.5;
    utmCenter.y = myProject.DTM.header->llCorner->y + dy * 0.5;
    float size = sqrt(dx*dy);
    float ratio = size / dz;
    float magnify = maxValue(1., minValue(10.f, ratio / 6.f));

    camera->setPosition(QVector3D(utmCenter.x, utmCenter.y - dy, z*magnify + size));
    camera->setViewCenter(QVector3D(utmCenter.x, utmCenter.y, z*magnify));

    // Scene
    Qt3DCore::QEntity *scene = createScene(&(myProject.DTM), &(myProject.indexMap), magnify);
    view3D->setRootEntity(scene);

    // Camera controls
    Qt3DExtras::QFirstPersonCameraController *camController = new Qt3DExtras::QFirstPersonCameraController(scene);
    camController->setCamera(camera);

    view3D->show();
}

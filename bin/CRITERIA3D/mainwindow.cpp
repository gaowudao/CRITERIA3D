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
#include "tileSources/CompositeTileSource.h"

#include "formInfo.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Position.h"
#include "crit3dProject.h"
#include "utilities.h"
#include "soilDbTools.h"
#include "gis.h"
#include "commonConstants.h"
#include "dialogSelection.h"
#include "spatialControl.h"
#include "dialogInterpolation.h"
#include "dialogSettings.h"

#include <Qt3DRender/QCamera>
#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DExtras/QFirstPersonCameraController>
#include <Qt3DInput>

#include "crit3dProject.h"

extern Crit3DProject myProject;

#define MAPBORDER 8
#define TOOLSWIDTH 260

MainWindow::MainWindow(QWidget *parent) :
    SharedMainWindow (),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->myRubberBand = nullptr;
    this->viewer3D = nullptr;
    this->currentMap = mapType::mapNone;

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

    this->currentPointsVisualization = notShown;
    // show menu
    showPointsGroup = new QActionGroup(this);
    showPointsGroup->setExclusive(true);
    showPointsGroup->addAction(this->ui->actionView_PointsHide);
    showPointsGroup->addAction(this->ui->actionView_PointsLocation);
    showPointsGroup->addAction(this->ui->actionView_PointsCurrentVariable);
    showPointsGroup->setEnabled(false);

    this->ui->groupBoxMeteoGrid->setVisible(false);

    // Set tiles source
    this->setMapSource(OSMTileSource::OSMTiles);

    // Set start size and position
    this->startCenter = new Position (myProject.gisSettings.startLocation.longitude, myProject.gisSettings.startLocation.latitude, 0.0);
    this->mapView->setZoomLevel(8);
    this->mapView->centerOn(startCenter->lonLat());

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

    ui->groupBoxVariable->move(MAPBORDER/2, this->height()/2
                          - ui->groupBoxVariable->height() - ui->groupBoxMeteoPoints->height()*0.66);
    ui->groupBoxVariable->resize(TOOLSWIDTH, ui->groupBoxVariable->height());

    ui->groupBoxMeteoPoints->move(MAPBORDER/2, ui->groupBoxVariable->y() + ui->groupBoxVariable->height() + MAPBORDER);
    ui->groupBoxMeteoPoints->resize(TOOLSWIDTH, ui->groupBoxMeteoPoints->height());

    ui->groupBoxRaster->move(MAPBORDER/2, ui->groupBoxMeteoPoints->y() + ui->groupBoxMeteoPoints->height() + MAPBORDER);
    ui->groupBoxRaster->resize(TOOLSWIDTH, ui->groupBoxRaster->height());

    ui->groupBoxMeteoGrid->move(MAPBORDER/2, ui->groupBoxRaster->y() + ui->groupBoxRaster->height() + MAPBORDER);
    ui->groupBoxMeteoGrid->resize(TOOLSWIDTH, ui->groupBoxMeteoGrid->height());

    // TODO sembrano non funzionare
    ui->widgetColorLegendRaster->resize(TOOLSWIDTH, ui->widgetColorLegendPoints->height());
    ui->widgetColorLegendPoints->resize(TOOLSWIDTH, ui->widgetColorLegendPoints->height());
}


void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    this->rasterObj->updateCenter();
    this->meteoGridObj->updateCenter();

    if (myRubberBand != nullptr && myRubberBand->isVisible())
    {
        gis::Crit3DGeoPoint pointSelected;
        QPointF lastCornerOffset = event->localPos();
        QPointF firstCornerOffset = myRubberBand->getFirstCorner();
        QPoint pixelTopLeft;
        QPoint pixelBottomRight;

        if (firstCornerOffset.y() > lastCornerOffset.y())
        {
            if (firstCornerOffset.x() > lastCornerOffset.x())
            {
                // bottom to left
                pixelTopLeft = lastCornerOffset.toPoint();
                pixelBottomRight = firstCornerOffset.toPoint();
            }
            else
            {
                // bottom to right
                pixelTopLeft = QPoint(firstCornerOffset.toPoint().x(), lastCornerOffset.toPoint().y());
                pixelBottomRight = QPoint(lastCornerOffset.toPoint().x(), firstCornerOffset.toPoint().y());
            }
        }
        else
        {
            if (firstCornerOffset.x() > lastCornerOffset.x())
            {
                // top to left
                pixelTopLeft = QPoint(lastCornerOffset.toPoint().x(), firstCornerOffset.toPoint().y());
                pixelBottomRight = QPoint(firstCornerOffset.toPoint().x(), lastCornerOffset.toPoint().y());
            }
            else
            {
                // top to right
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
    {
        this->mapView->zoomIn();
    }
    else if (event->button() == Qt::RightButton)
    {
        this->mapView->zoomOut();
    }

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

    if (myRubberBand != nullptr && myRubberBand->isActive)
    {
        myRubberBand->setGeometry(QRect(myRubberBand->getOrigin(), mapPoint).normalized());
    }
}


void MainWindow::mousePressEvent(QMouseEvent *event)
{
    QPoint pos = event->pos();
    QPoint mapPoint = getMapPoint(&pos);

    if (event->button() == Qt::RightButton)
    {
        if (myRubberBand != nullptr)
        {
            // rubber band
            QPointF firstCorner = event->localPos();
            myRubberBand->setFirstCorner(firstCorner);
            myRubberBand->setOrigin(mapPoint);
            myRubberBand->setGeometry(QRect(mapPoint, QSize()));
            myRubberBand->isActive = true;
            myRubberBand->show();
        }
        else if (event->type() != QEvent::MouseButtonDblClick)
        {
            // context menu
            contextMenuRequested(event->globalPos());
        }

        #ifdef NETCDF
        if (myProject.netCDF.isLoaded)
        {
            Position myPos = mapView->mapToScene(getMapPoint(&pos));
            gis::Crit3DGeoPoint geoPoint = gis::Crit3DGeoPoint(myPos.latitude(), myPos.longitude());

            exportNetCDFDataSeries(geoPoint);
        }
        #endif
    }
}


void MainWindow::on_actionRectangle_Selection_triggered()
{
    if (myRubberBand != nullptr)
    {
        delete myRubberBand;
        myRubberBand = nullptr;
    }

    if (ui->actionRectangle_Selection->isChecked())
    {
        myRubberBand = new RubberBand(QRubberBand::Rectangle, this->mapView);
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


void MainWindow::drawMeteoPoints()
{
    this->resetMeteoPoints();
    this->addMeteoPoints();

    this->updateDateTime();

    myProject.loadMeteoPointsData (myProject.getCurrentDate(), myProject.getCurrentDate(), true);

    showPointsGroup->setEnabled(true);

    currentPointsVisualization = showLocation;
    redrawMeteoPoints(currentPointsVisualization, true);
}


void MainWindow::drawProject()
{
    mapView->centerOn(startCenter->lonLat());

    if (myProject.DEM.isLoaded)
        this->renderDEM();

    drawMeteoPoints();
    // drawMeteoGrid();

    QString title = "CRITERIA3D";
    if (myProject.projectName != "")
        title += " - " + myProject.projectName;

    this->setWindowTitle(title);
}


void MainWindow::clearDEM()
{
    this->rasterObj->clear();
    this->rasterObj->redrawRequested();
    ui->labelRasterScale->setText("");
    this->ui->rasterOpacitySlider->setEnabled(false);
}


void MainWindow::renderDEM()
{
    this->setCurrentRaster(&(myProject.DEM));
    ui->labelRasterScale->setText(QString::fromStdString(getVariableString(noMeteoTerrain)));
    this->ui->rasterOpacitySlider->setEnabled(true);

    // center map
    gis::Crit3DGeoPoint* center = this->rasterObj->getRasterCenter();
    this->mapView->centerOn(qreal(center->longitude), qreal(center->latitude));

    // resize map
    float size = this->rasterObj->getRasterMaxSize();
    size = log2(1000.f/size);
    this->mapView->setZoomLevel(quint8(size));
    this->mapView->centerOn(qreal(center->longitude), qreal(center->latitude));

    // active raster object
    this->rasterObj->updateCenter();

    if (this->viewer3D != nullptr)
    {
        initializeViewer3D();
        //this->viewer3D->close();
        //this->viewer3D = nullptr;
    }
}


void MainWindow::on_actionLoadDEM_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Digital Elevation Model"), "", tr("ESRI grid files (*.flt)"));

    if (fileName == "") return;

    if (! myProject.setDEM(fileName)) return;

    this->renderDEM();
}


void MainWindow::on_actionOpen_meteo_points_DB_triggered()
{
    QString dbName = QFileDialog::getOpenFileName(this, tr("Open meteo points DB"), "", tr("DB files (*.db)"));

    if (dbName != "")
    {
        this->loadMeteoPointsDB(dbName);
    }

    currentPointsVisualization = showLocation;
    redrawMeteoPoints(currentPointsVisualization, true);
}


void MainWindow::on_actionOpen_meteo_grid_triggered()
{
    QString xmlName = QFileDialog::getOpenFileName(this, tr("Open XML meteo grid"), "", tr("xml files (*.xml)"));

    if (xmlName != "")
    {
        if (this->loadMeteoGridDB(xmlName))
            ui->groupBoxMeteoGrid->setVisible(true);
    }
}


void MainWindow::on_actionOpen_Project_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open project file"), "", tr("ini files (*.ini)"));
    if (fileName == "") return;

    if (myProject.isProjectLoaded)
    {
        on_actionClose_meteo_grid_triggered();
        on_actionClose_meteo_points_triggered();
        clearDEM();
    }

    if (! myProject.loadCriteria3DProject(fileName))
    {
        myProject.loadCriteria3DProject(myProject.getApplicationPath() + "default.ini");
    }

    drawProject();
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

    this->myRubberBand = nullptr;
}


void MainWindow::on_actionVariableQualitySpatial_triggered()
{
    myProject.checkSpatialQuality = ui->actionVariableQualitySpatial->isChecked();
    updateVariable();
}


void MainWindow::interpolateDemGUI()
{
    meteoVariable myVar = myProject.getCurrentVariable();

    if (myProject.interpolationDemMain(myVar, myProject.getCurrentTime(), &(myProject.dataRaster), true))
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
        if (myProject.getCurrentVariable() != noMeteoVar)
        {
            if (this->meteoGridObj != nullptr) this->meteoGridObj->setDrawBorders(false);
        }

        if (myProject.getFrequency() == daily)
        {
            this->ui->labelFrequency->setText("Daily");

            //check
            if (myProject.getCurrentVariable() == airTemperature)
                myProject.setCurrentVariable(dailyAirTemperatureAvg);

            else if (myProject.getCurrentVariable() == precipitation)
                myProject.setCurrentVariable(dailyPrecipitation);

            else if (myProject.getCurrentVariable() == globalIrradiance)
                myProject.setCurrentVariable(dailyGlobalRadiation);

            else if (myProject.getCurrentVariable() == airRelHumidity)
                myProject.setCurrentVariable(dailyAirRelHumidityAvg);

            else if (myProject.getCurrentVariable() == airDewTemperature)
                myProject.setCurrentVariable(dailyAirDewTemperatureAvg);

            else if (myProject.getCurrentVariable() == windIntensity)
                myProject.setCurrentVariable(dailyWindIntensityAvg);
        }

        else if (myProject.getFrequency() == hourly)
        {
            this->ui->labelFrequency->setText("Hourly");

            //check
            if ((myProject.getCurrentVariable() == dailyAirTemperatureAvg)
                    || (myProject.getCurrentVariable() == dailyAirTemperatureMax)
                    || (myProject.getCurrentVariable() == dailyAirTemperatureMin))
                myProject.setCurrentVariable(airTemperature);

            else if ((myProject.getCurrentVariable() == dailyAirRelHumidityAvg)
                     || (myProject.getCurrentVariable() == dailyAirRelHumidityMax)
                     || (myProject.getCurrentVariable() == dailyAirRelHumidityMin))
                 myProject.setCurrentVariable(airRelHumidity);

            else if (myProject.getCurrentVariable() == dailyAirDewTemperatureAvg)
                myProject.setCurrentVariable(airDewTemperature);

            else if (myProject.getCurrentVariable() == dailyPrecipitation)
                    myProject.setCurrentVariable(precipitation);

            else if (myProject.getCurrentVariable() == dailyGlobalRadiation)
                myProject.setCurrentVariable(globalIrradiance);

            else if (myProject.getCurrentVariable() == dailyWindIntensityAvg)
                myProject.setCurrentVariable(windIntensity);
        }
    }

    std::string myString = getVariableString(myProject.getCurrentVariable());
    ui->labelVariable->setText(QString::fromStdString(myString));
    ui->labelVariablePoints->setText(QString::fromStdString(myString));

    redrawMeteoPoints(currentPointsVisualization, true);
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

    redrawMeteoPoints(currentPointsVisualization, true);
    redrawMeteoGrid();
}


void MainWindow::on_timeEdit_timeChanged(const QTime &time)
{
    //hour
    if (time.hour() != myProject.getCurrentHour())
    {
        myProject.setCurrentHour(time.hour());
    }

    redrawMeteoPoints(currentPointsVisualization, true);
    redrawMeteoGrid();
}


void MainWindow::redrawMeteoPoints(visualizationType myType, bool updateColorSCale)
{
    currentPointsVisualization = myType;

    if (myProject.nrMeteoPoints == 0)
        return;

    // hide all meteo points
    for (int i = 0; i < myProject.nrMeteoPoints; i++)
        pointList[i]->setVisible(false);

    meteoPointsLegend->setVisible(true);

    switch(currentPointsVisualization)
    {
    case notShown:
    {
        meteoPointsLegend->setVisible(false);
        this->ui->actionView_PointsHide->setChecked(true);
        break;
    }
    case showLocation:
    {
        this->ui->actionView_PointsLocation->setChecked(true);
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
        break;
    }
    case showCurrentVariable:
    {
        this->ui->actionView_PointsCurrentVariable->setChecked(true);
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
        setColorScale(myProject.getCurrentVariable(), myProject.meteoPointsColorScale);

        Crit3DColor *myColor;
        for (int i = 0; i < myProject.nrMeteoPoints; i++)
        {
            if (int(myProject.meteoPoints[i].currentValue) != NODATA)
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
        break;
    }
    }
}


void MainWindow::redrawMeteoGrid()
{

    if (myProject.meteoGridDbHandler == nullptr)
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
    ui->labelMeteoGridScale->setText(QString::fromStdString(getVariableString(myProject.getCurrentVariable())));

    meteoGridObj->redrawRequested();
    meteoGridLegend->update();
}


bool MainWindow::loadMeteoPointsDB(QString dbName)
{
    this->resetMeteoPoints();

    if (!myProject.loadMeteoPointsDB(dbName))
    {
        return false;
    }

    this->addMeteoPoints();

    if (myProject.meteoGridDbHandler == nullptr)
    {
        myProject.findLastMeteoDate();
        this->updateDateTime();
    }

    myProject.loadMeteoPointsData(myProject.getCurrentDate(), myProject.getCurrentDate(), true);

    this->showPointsGroup->setEnabled(true);

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


void MainWindow::on_rasterScaleButton_clicked()
{
    if (this->rasterObj->getRaster() == nullptr)
    {
        QMessageBox::information(nullptr, "No Raster", "Load raster before");
        return;
    }

    meteoVariable myVar = chooseColorScale();
    if (myVar != noMeteoVar)
    {
        setColorScale(myVar, this->rasterObj->getRaster()->colorScale);
        ui->labelRasterScale->setText(QString::fromStdString(getVariableString(myVar)));
    }
}

void MainWindow::on_variableButton_clicked()
{
    if (chooseMeteoVariable(&myProject))
    {
       this->currentPointsVisualization = showCurrentVariable;
       this->updateVariable();
       if (this->meteoGridObj != nullptr) this->meteoGridObj->setDrawBorders(false);
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


void MainWindow::on_rasterRestoreButton_clicked()
{
    if (this->rasterObj->getRaster() == nullptr)
    {
        QMessageBox::information(nullptr, "No Raster", "Load raster before");
        return;
    }

    setDefaultDEMScale(myProject.DEM.colorScale);
    this->setCurrentRaster(&(myProject.DEM));
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
    this->resetMeteoPoints();
    this->meteoPointsLegend->setVisible(false);
    myProject.closeMeteoPointsDB();
    this->showPointsGroup->setEnabled(false);
}

void MainWindow::on_actionClose_meteo_grid_triggered()
{
    if (myProject.meteoGridDbHandler != nullptr)
    {
        myProject.meteoGridDbHandler->meteoGrid()->dataMeteoGrid.isLoaded = false;
        meteoGridObj->clear();
        meteoGridObj->redrawRequested();
        meteoGridLegend->setVisible(false);
        myProject.closeMeteoGridDB();
        ui->meteoGridOpacitySlider->setEnabled(false);
    }
}


void MainWindow::on_actionInterpolation_to_DEM_triggered()
{
    interpolateDemGUI();
}


void MainWindow::on_actionInterpolationSettings_triggered()
{
    if (myProject.meteoPointsDbHandler == nullptr)
    {
        myProject.logInfoGUI("Open a Meteo Points DB before.");
        return;
    }

    DialogInterpolation* myInterpolationDialog = new DialogInterpolation(&myProject);
    myInterpolationDialog->close();
}


void MainWindow::on_actionParameters_triggered()
{
    DialogSettings* mySettingsDialog = new DialogSettings(&myProject);
    mySettingsDialog->exec();
    if (startCenter->latitude() != myProject.gisSettings.startLocation.latitude
            || startCenter->longitude() != myProject.gisSettings.startLocation.longitude)
    {
        startCenter->setLatitude(myProject.gisSettings.startLocation.latitude);
        startCenter->setLongitude(myProject.gisSettings.startLocation.longitude);
        this->mapView->centerOn(startCenter->lonLat());
    }

    mySettingsDialog->close();
}


void MainWindow::on_actionOpen_model_parameters_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open DB parameters"), "", tr("SQLite files (*.db)"));
    if (fileName == "") return;

    if (myProject.loadModelParameters(fileName))
        QMessageBox::information(nullptr, "", "Model parameters loaded");
}


void MainWindow::on_actionOpen_soil_map_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open soil map"), "", tr("ESRI grid files (*.flt)"));
    if (fileName == "") return;

    if (myProject.loadSoilMap(fileName))
        ui->rasterOpacitySlider->setEnabled(true);
}


void MainWindow::on_actionOpen_soil_data_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Load DB soil"), "", tr("SQLite files (*.db)"));
    if (fileName == "") return;

    myProject.loadSoilData(fileName);
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
    myProject.initializeCriteria3D();
}


void MainWindow::on_viewer3DClosed()
{
    this->viewer3D = nullptr;
}


void MainWindow::initializeViewer3D()
{
    if (viewer3D != nullptr)
    {
        if (! myProject.isInitialized)
        {
            myProject.setIndexMaps();
        }
        viewer3D->initialize(&myProject);
    }
}


void MainWindow::on_actionView_3D_triggered()
{
    if (! myProject.DEM.isLoaded)
    {
        myProject.logInfoGUI("Load a Digital Elevation Model before.");
        return;
    }

    if (viewer3D == nullptr || !viewer3D->isVisible())
    {
        viewer3D = new Viewer3D(this);
        initializeViewer3D();
    }

    viewer3D->show();
    connect (viewer3D, SIGNAL(destroyed()), this, SLOT(on_viewer3DClosed()));
}


void MainWindow::on_actionView_DEM_triggered()
{
    if (myProject.DEM.isLoaded)
    {
        setColorScale(noMeteoTerrain, myProject.DEM.colorScale);
        this->setCurrentRaster(&(myProject.DEM));
        ui->labelRasterScale->setText(QString::fromStdString(getVariableString(noMeteoTerrain)));
        this->currentMap = mapType::mapDEM;
    }
    else
    {
        myProject.logInfoGUI("Load a Digital Elevation Model before.");
        return;
    }
}


void MainWindow::on_actionView_Soil_triggered()
{
    if (myProject.soilMap.isLoaded)
    {
        setColorScale(noMeteoTerrain, myProject.soilMap.colorScale);
        this->setCurrentRaster(&(myProject.soilMap));
        ui->labelRasterScale->setText("Soil map");
        this->currentMap = mapType::mapSoil;
    }
    else
    {
        myProject.logError("Load a soil map before.");
        return;
    }
}


void MainWindow::on_actionView_Boundary_triggered()
{
    if (myProject.boundaryMap.isLoaded)
    {
        setColorScale(noMeteoTerrain, myProject.boundaryMap.colorScale);
        this->setCurrentRaster(&(myProject.boundaryMap));
        ui->labelRasterScale->setText("Boundary map");
    }
    else
    {
        myProject.logInfoGUI("Initialize 3D Model before.");
        return;
    }
}


void MainWindow::on_actionView_Slope_triggered()
{
    if (myProject.DEM.isLoaded)
    {
        setColorScale(noMeteoTerrain, myProject.radiationMaps->slopeMap->colorScale);
        this->setCurrentRaster(myProject.radiationMaps->slopeMap);
        ui->labelRasterScale->setText("Slope °");
    }
    else
    {
        myProject.logInfoGUI("Load a Digital Elevation Model before.");
        return;
    }
}


void MainWindow::on_actionView_Aspect_triggered()
{
    if (myProject.DEM.isLoaded)
    {
        setColorScale(airTemperature, myProject.radiationMaps->aspectMap->colorScale);
        this->setCurrentRaster(myProject.radiationMaps->aspectMap);
        ui->labelRasterScale->setText("Aspect °");
    }
    else
    {
        myProject.logInfoGUI("Load a Digital Elevation Model before.");
        return;
    }
}


bool MainWindow::checkMapVariable(bool isComputed)
{
    if (! myProject.DEM.isLoaded)
    {
        myProject.logInfoGUI("Load a Digital Elevation Model before.");
        return false;
    }

    if (! isComputed)
    {
        myProject.logInfoGUI("Compute variable before.");
        return false;
    }

    return true;
}


void MainWindow::setMapVariable(meteoVariable myVar, gis::Crit3DRasterGrid *myGrid)
{
    setColorScale(myVar, myGrid->colorScale);
    this->setCurrentRaster(myGrid);
    ui->labelRasterScale->setText(QString::fromStdString(getVariableString(myVar)));
    this->currentMap = mapType::mapVariable;
}


void MainWindow::on_actionView_Transmissivity_triggered()
{
    if (this->checkMapVariable(myProject.radiationMaps->isComputed))
        setMapVariable(atmTransmissivity, myProject.radiationMaps->transmissivityMap);
}


void MainWindow::on_actionView_Global_radiation_triggered()
{
    if (this->checkMapVariable(myProject.radiationMaps->isComputed))
        setMapVariable(globalIrradiance, myProject.radiationMaps->globalRadiationMap);
}


void MainWindow::on_actionView_ET0_triggered()
{
    if (this->checkMapVariable(myProject.meteoMaps->isComputed))
        setMapVariable(referenceEvapotranspiration, myProject.meteoMaps->ET0Map);
}


void MainWindow::on_actionView_Air_temperature_triggered()
{
    if (this->checkMapVariable(myProject.meteoMaps->isComputed))
        setMapVariable(airTemperature, myProject.meteoMaps->airTemperatureMap);
}


void MainWindow::on_actionView_Precipitation_triggered()
{
    if (this->checkMapVariable(myProject.meteoMaps->isComputed))
        setMapVariable(precipitation, myProject.meteoMaps->precipitationMap);
}


void MainWindow::on_actionView_Air_relative_humidity_triggered()
{
    if (this->checkMapVariable(myProject.meteoMaps->isComputed))
        setMapVariable(airRelHumidity, myProject.meteoMaps->airRelHumidityMap);
}


void MainWindow::on_actionView_Wind_intensity_triggered()
{
    if (this->checkMapVariable(myProject.meteoMaps->isComputed))
        setMapVariable(windIntensity, myProject.meteoMaps->windIntensityMap);
}


void MainWindow::on_actionView_PointsHide_triggered()
{
    redrawMeteoPoints(notShown, true);
}


void MainWindow::on_actionView_PointsLocation_triggered()
{
    redrawMeteoPoints(showLocation, true);
}


void MainWindow::on_actionView_PointsCurrentVariable_triggered()
{
    redrawMeteoPoints(showCurrentVariable, true);
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


void MainWindow::on_actionCompute_solar_radiation_triggered()
{
    if (myProject.nrMeteoPoints == 0)
    {
        myProject.logInfoGUI("Open a Meteo Points DB before.");
        return;
    }

    myProject.setFrequency(hourly);
    myProject.setCurrentVariable(globalIrradiance);
    this->currentPointsVisualization = showCurrentVariable;
    this->updateVariable();
    this->interpolateDemGUI();
}


void MainWindow::on_actionCompute_AllMeteoMaps_triggered()
{
    if (! myProject.computeAllMeteoMaps(myProject.getCurrentTime(), true))
    {
        myProject.logError();
        return;
    }
}


void MainWindow::contextMenuRequested(const QPoint globalPos)
{
    QMenu submenu;

    submenu.addAction("Test");
    if (myProject.soilMap.isLoaded)
    {
        submenu.addAction("Soil data");
    }

    QAction* myAction = submenu.exec(globalPos);

    if (myAction)
    {
        if (myAction->text().contains("Soil data") )
        {
            // shows soil
        }
    }
}

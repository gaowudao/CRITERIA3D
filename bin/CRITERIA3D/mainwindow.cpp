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
#include "dialogSelection.h"
#include "spatialControl.h"
#include "dialogInterpolation.h"
#include "dialogSettings.h"
#include "dialogRadiation.h"

#include <Qt3DRender/QCamera>
#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DExtras/QFirstPersonCameraController>
#include <Qt3DInput>

#include "crit3dProject.h"
#include "commonConstants.h"

extern Crit3DProject myProject;

#define MAPBORDER 11
#define TOOLSWIDTH 260

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->myRubberBand = nullptr;
    this->viewer3D = nullptr;

    // Set the MapGraphics Scene and View
    this->mapScene = new MapGraphicsScene(this);
    this->mapView = new MapGraphicsView(mapScene, this->ui->widgetMap);

    this->inputRasterColorLegend = new ColorLegend(this->ui->colorScaleInputRaster);
    this->inputRasterColorLegend->resize(this->ui->colorScaleInputRaster->size());

    this->outputRasterColorLegend = new ColorLegend(this->ui->colorScaleOutputRaster);
    this->outputRasterColorLegend->resize(this->ui->colorScaleOutputRaster->size());

    this->meteoPointsLegend = new ColorLegend(this->ui->colorScaleMeteoPoints);
    this->meteoPointsLegend->resize(this->ui->colorScaleMeteoPoints->size());
    this->meteoPointsLegend->colorScale = myProject.meteoPointsColorScale;

    // initialize
    this->ui->opacitySliderRasterInput->setVisible(false);
    this->ui->opacitySliderRasterOutput->setVisible(false);
    ui->labelInputRaster->setText("");
    ui->labelOutputRaster->setText("");
    this->currentPointsVisualization = notShown;

    // show menu
    showPointsGroup = new QActionGroup(this);
    showPointsGroup->setExclusive(true);
    showPointsGroup->addAction(this->ui->actionView_PointsHide);
    showPointsGroup->addAction(this->ui->actionView_PointsLocation);
    showPointsGroup->addAction(this->ui->actionView_PointsCurrentVariable);
    showPointsGroup->setEnabled(false);

    // Set tiles source
    this->setMapSource(OSMTileSource::OSMTiles);

    // Set start size and position
    this->startCenter = new Position (myProject.gisSettings.startLocation.longitude, myProject.gisSettings.startLocation.latitude, 0.0);
    this->mapView->setZoomLevel(8);
    this->mapView->centerOn(startCenter->lonLat());

    // Set raster object
    this->rasterObj = new RasterObject(this->mapView);
    this->rasterObj->setOpacity(this->ui->opacitySliderRasterInput->value() / 100.0);
    this->rasterObj->setColorLegend(this->inputRasterColorLegend);
    this->mapView->scene()->addObject(this->rasterObj);

    this->updateVariable();
    this->updateDateTime();

    connect(this->ui->dateEdit, SIGNAL(editingFinished()), this, SLOT(on_dateChanged()));

    this->setMouseTracking(true);
}


void MainWindow::resizeEvent(QResizeEvent * event)
{
    Q_UNUSED(event)
    const int INFOHEIGHT = 40;

    ui->widgetMap->setGeometry(TOOLSWIDTH, 0, this->width()-TOOLSWIDTH, this->height() - INFOHEIGHT);
    mapView->resize(ui->widgetMap->size());

    ui->groupBoxInput->move(MAPBORDER/2, MAPBORDER);
    ui->groupBoxInput->resize(TOOLSWIDTH, ui->groupBoxInput->height());

    ui->groupBoxOutput->move(MAPBORDER/2, ui->groupBoxInput->y() + ui->groupBoxInput->height() + MAPBORDER);
    ui->groupBoxOutput->resize(TOOLSWIDTH, ui->groupBoxOutput->height());
}


void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    this->rasterObj->updateCenter();

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
    if (event->button() == Qt::RightButton)
    {
        if (myRubberBand != nullptr)
        {
            // rubber band
            QPoint pos = event->pos();
            QPoint mapPoint = getMapPoint(&pos);
            QPointF firstCorner = event->localPos();
            myRubberBand->setFirstCorner(firstCorner);
            myRubberBand->setOrigin(mapPoint);
            myRubberBand->setGeometry(QRect(mapPoint, QSize()));
            myRubberBand->isActive = true;
            myRubberBand->show();
        }
        else
        {
            contextMenuRequested(event->pos(), event->globalPos());
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
    rasterObj->clear();
    rasterObj->redrawRequested();
    ui->labelInputRaster->setText("");
    ui->labelOutputRaster->setText("");
    setInputRasterVisible(false);
    setOutputRasterVisible(false);
}


void MainWindow::clearMeteoPoints()
{
    resetMeteoPoints();
    myProject.closeMeteoPointsDB();
    meteoPointsLegend->setVisible(false);
    showPointsGroup->setEnabled(false);
}


void MainWindow::renderDEM()
{
    setCurrentRasterInput(&(myProject.DEM));
    setInputRasterVisible(true);
    ui->labelInputRaster->setText(QString::fromStdString(getVariableString(noMeteoTerrain)));

    // center map
    gis::Crit3DGeoPoint* center = this->rasterObj->getRasterCenter();
    mapView->centerOn(qreal(center->longitude), qreal(center->latitude));

    // resize map
    float size = this->rasterObj->getRasterMaxSize();
    size = log2(1000.f/size);
    mapView->setZoomLevel(quint8(size));
    mapView->centerOn(qreal(center->longitude), qreal(center->latitude));

    // active raster object
    rasterObj->updateCenter();

    if (viewer3D != nullptr)
    {
        initializeViewer3D();
        //this->viewer3D->close();
        //this->viewer3D = nullptr;
    }
}


void MainWindow::on_actionLoad_DEM_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Digital Elevation Model"), "", tr("ESRI grid files (*.flt)"));

    if (fileName == "") return;

    if (! myProject.loadDEM(fileName)) return;

    myProject.meteoMaps = new Crit3DMeteoMaps(myProject.DEM);

    this->renderDEM();
}


void MainWindow::on_actionOpen_Project_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open project file"), "", tr("ini files (*.ini)"));
    if (fileName == "") return;

    if (myProject.isProjectLoaded)
    {
        clearMeteoPoints();
        clearDEM();
    }

    if (! myProject.loadCriteria3DProject(fileName))
    {
        myProject.loadCriteria3DProject(myProject.getApplicationPath() + "default.ini");
    }

    drawProject();
}


void MainWindow::on_actionClose_Project_triggered()
{
    if (! myProject.isProjectLoaded) return;

    clearMeteoPoints();
    clearDEM();

    myProject.loadCriteria3DProject(myProject.getApplicationPath() + "default.ini");
    drawProject();
}


QPoint MainWindow::getMapPoint(QPoint* point) const
{
    QPoint mapPoint;
    int dx, dy;
    dx = this->ui->widgetMap->x();
    dy = this->ui->widgetMap->y() + this->ui->menuBar->height();
    mapPoint.setX(point->x() - dx - MAPBORDER);
    mapPoint.setY(point->y() - dy - MAPBORDER);
    return mapPoint;
}



void MainWindow::resetMeteoPoints()
{
    for (int i = 0; i < this->pointList.size(); i++)
    {
        mapView->scene()->removeObject(this->pointList[i]);
    }

    pointList.clear();
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
        setCurrentRasterOutput(&(myProject.dataRaster));

        ui->labelOutputRaster->setText(QString::fromStdString(getVariableString(myVar)));
    }
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

    redrawMeteoPoints(currentPointsVisualization, true);
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
}


void MainWindow::on_timeEdit_timeChanged(const QTime &time)
{
    //hour
    if (time.hour() != myProject.getCurrentHour())
    {
        myProject.setCurrentHour(time.hour());
    }

    redrawMeteoPoints(currentPointsVisualization, true);
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
                      &myProject.qualityInterpolationSettings, &(myProject.climateParameters), myProject.checkSpatialQuality);

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
        default:
        {
            meteoPointsLegend->setVisible(false);
            this->ui->actionView_PointsHide->setChecked(true);
            break;
        }
    }
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


void MainWindow::on_variableButton_clicked()
{
    if (chooseMeteoVariable(&myProject))
    {
       this->currentPointsVisualization = showCurrentVariable;
       this->updateVariable();
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

void MainWindow::setInputRasterVisible(bool value)
{
    inputRasterColorLegend->setVisible(value);
    ui->labelInputRaster->setVisible(value);
    ui->opacitySliderRasterInput->setVisible(value);
}

void MainWindow::setOutputRasterVisible(bool value)
{
    outputRasterColorLegend->setVisible(value);
    ui->labelOutputRaster->setVisible(value);
    ui->opacitySliderRasterOutput->setVisible(value);
}


void MainWindow::setCurrentRasterInput(gis::Crit3DRasterGrid *myRaster)
{
    setInputRasterVisible(true);
    setOutputRasterVisible(false);

    rasterObj->initializeUTM(myRaster, myProject.gisSettings, false);
    rasterObj->setColorLegend(inputRasterColorLegend);
    rasterObj->setOpacity(ui->opacitySliderRasterInput->value() / 100.0);
    inputRasterColorLegend->colorScale = myRaster->colorScale;

    rasterObj->redrawRequested();
}


void MainWindow::setCurrentRasterOutput(gis::Crit3DRasterGrid *myRaster)
{
    setInputRasterVisible(false);
    setOutputRasterVisible(true);

    rasterObj->initializeUTM(myRaster, myProject.gisSettings, false);
    rasterObj->setColorLegend(outputRasterColorLegend);
    rasterObj->setOpacity(ui->opacitySliderRasterOutput->value() / 100.0);
    outputRasterColorLegend->colorScale = myRaster->colorScale;

    rasterObj->redrawRequested();
}


void MainWindow::on_dateEdit_dateChanged(const QDate &date)
{
    Q_UNUSED(date);
    this->on_dateChanged();
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


void MainWindow::on_actionRadiationSettings_triggered()
{
    DialogRadiation* myDialogRadiation = new DialogRadiation(&myProject);
    myDialogRadiation->close();
}


void MainWindow::on_actionProjectSettings_triggered()
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


void MainWindow::on_actionCriteria3D_Initialize_triggered()
{
    myProject.initializeCriteria3D();
}


void MainWindow::on_viewer3DClosed()
{
    this->viewer3D = nullptr;
}


bool MainWindow::initializeViewer3D()
{
    if (viewer3D == nullptr) return false;

    if (! myProject.isCriteria3DInitialized)
    {
        myProject.logError("Initialize 3D model before");
        return false;
    }
    else
    {
        viewer3D->initialize(&myProject);
        return true;
    }
}


void MainWindow::on_actionView_3D_triggered()
{
    if (! myProject.DEM.isLoaded)
    {
        myProject.logInfoGUI("Load a Digital Elevation Model before.");
        return;
    }

    if (viewer3D == nullptr || ! viewer3D->isVisible())
    {
        viewer3D = new Viewer3D(this);
        if (! initializeViewer3D()) return;
    }

    viewer3D->show();
    connect (viewer3D, SIGNAL(destroyed()), this, SLOT(on_viewer3DClosed()));
}


void MainWindow::on_actionView_DEM_triggered()
{
    if (myProject.DEM.isLoaded)
    {
        setColorScale(noMeteoTerrain, myProject.DEM.colorScale);
        setCurrentRasterInput(&(myProject.DEM));
        ui->labelInputRaster->setText(QString::fromStdString(getVariableString(noMeteoTerrain)));
    }
    else
    {
        myProject.logInfoGUI("Load a Digital Elevation Model before.");
        return;
    }
}


void MainWindow::on_actionView_SoilMap_triggered()
{
    if (myProject.soilMap.isLoaded)
    {
        setColorScale(airTemperature, myProject.soilMap.colorScale);
        setCurrentRasterInput(&(myProject.soilMap));
        ui->labelInputRaster->setText("Soil index map");
        rasterObj->updateCenter();
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
        setCurrentRasterInput(&(myProject.boundaryMap));
        ui->labelInputRaster->setText("Boundary map");
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
        setCurrentRasterInput(myProject.radiationMaps->slopeMap);
        ui->labelInputRaster->setText("Slope °");
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
        setCurrentRasterInput(myProject.radiationMaps->aspectMap);
        ui->labelInputRaster->setText("Aspect °");
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
    setCurrentRasterOutput(myGrid);
    ui->labelOutputRaster->setText(QString::fromStdString(getVariableString(myVar)));
    ui->opacitySliderRasterOutput->setVisible(true);

    myProject.setCurrentVariable(myVar);
    myProject.setFrequency(getFrequency(myVar));
    currentPointsVisualization = showCurrentVariable;
    updateVariable();
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


void MainWindow::openSoilWidget(QPoint localPos)
{
    double x, y;

    Position geoPos = mapView->mapToScene(getMapPoint(&localPos));
    gis::latLonToUtmForceZone(myProject.gisSettings.utmZone, geoPos.latitude(), geoPos.longitude(), &x, &y);
    QString soilCode = myProject.getCrit3DSoilCode(x, y);

    if (soilCode == "") {
        myProject.logInfoGUI("No soil.");
    }
    else if (soilCode == "NOT FOUND") {
        myProject.logError("Soil code not found: check soil database.");
    }
    else {
        soilWidget = new Crit3DSoilWidget();
        soilWidget->setDbSoil(myProject.dbSoilName, soilCode);
        soilWidget->show();
    }
}


void MainWindow::contextMenuRequested(QPoint localPos, QPoint globalPos)
{
    QMenu submenu;
    int nrItems = 0;

    if (myProject.soilMap.isLoaded)
    {
        submenu.addAction("Soil data");
        nrItems++;
    }
    if (nrItems == 0) return;

    QAction* myAction = submenu.exec(globalPos);

    if (myAction)
    {
        if (myAction->text().contains("Soil data") )
        {
            if (myProject.nrSoils > 0) {
                openSoilWidget(localPos);
            }
            else {
                myProject.logInfoGUI("Load soil database before.");
            }
        }
    }
}


void MainWindow::on_actionLoad_meteo_points_DB_triggered()
{
    QString dbName = QFileDialog::getOpenFileName(this, tr("Open meteo points DB"), "", tr("DB files (*.db)"));

    if (dbName != "")
    {
        this->loadMeteoPointsDB(dbName);
    }

    currentPointsVisualization = showLocation;
    redrawMeteoPoints(currentPointsVisualization, true);
}


void MainWindow::on_actionLoad_soil_map_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open soil map"), "", tr("ESRI grid files (*.flt)"));
    if (fileName == "") return;

    if (myProject.loadSoilMap(fileName))
    {
        ui->opacitySliderRasterInput->setVisible(true);
        on_actionView_SoilMap_triggered();
    }
}


void MainWindow::on_actionLoad_soil_data_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Load DB soil"), "", tr("SQLite files (*.db)"));
    if (fileName == "") return;

    myProject.loadSoilDatabase(fileName);
}


void MainWindow::on_actionLoad_model_parameters_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open DB parameters"), "", tr("SQLite files (*.db)"));
    if (fileName == "") return;

    if (myProject.loadModelParameters(fileName))
        myProject.logInfoGUI("Model parameters loaded");
}


void MainWindow::on_opacitySliderRasterInput_sliderMoved(int position)
{
    this->rasterObj->setOpacity(position / 100.0);
}


void MainWindow::on_opacitySliderRasterOutput_sliderMoved(int position)
{
    this->rasterObj->setOpacity(position / 100.0);
}

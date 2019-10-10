#include "commonConstants.h"

#include "tileSources/OSMTileSource.h"
#include "tileSources/CompositeTileSource.h"

#include "basicMath.h"
#include "formInfo.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "crit3dProject.h"
#include "soilDbTools.h"
#include "dialogSelection.h"
#include "spatialControl.h"
#include "dialogInterpolation.h"
#include "dialogSettings.h"
#include "dialogRadiation.h"

#include "crit3dProject.h"

extern Crit3DProject myProject;

#define MAPBORDER 10
#define TOOLSWIDTH 243

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

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

    this->setTileMapSource(OSMTileSource::OSMTiles);

    // Set start size and position
    this->startCenter = new Position (myProject.gisSettings.startLocation.longitude, myProject.gisSettings.startLocation.latitude, 0.0);
    this->mapView->setZoomLevel(8);
    this->mapView->centerOn(startCenter->lonLat());
    connect(this->mapView, SIGNAL(zoomLevelChanged(quint8)), this, SLOT(updateMaps()));

    // Set raster object
    this->rasterObj = new RasterObject(this->mapView);
    this->rasterObj->setOpacity(this->ui->opacitySliderRasterInput->value() / 100.0);
    this->rasterObj->setColorLegend(this->inputRasterColorLegend);
    this->mapView->scene()->addObject(this->rasterObj);

    this->updateVariable();
    this->updateDateTime();

    this->setMouseTracking(true);
}


void MainWindow::resizeEvent(QResizeEvent * event)
{
    Q_UNUSED(event)

    const int INFOHEIGHT = 40;
    int x1 = this->width() - TOOLSWIDTH - MAPBORDER;
    int dy = ui->groupBoxMeteoPoints->height() + ui->groupBoxInput->height() + ui->groupBoxOutput->height() + MAPBORDER*2;
    int y1 = (this->height() - INFOHEIGHT - dy) / 2;

    ui->widgetMap->setGeometry(0, 0, x1, this->height() - INFOHEIGHT);
    mapView->resize(ui->widgetMap->size());

    ui->groupBoxMeteoPoints->move(x1, y1);
    ui->groupBoxMeteoPoints->resize(TOOLSWIDTH, ui->groupBoxMeteoPoints->height());

    ui->groupBoxInput->move(x1, y1 + ui->groupBoxMeteoPoints->height() + MAPBORDER);
    ui->groupBoxInput->resize(TOOLSWIDTH, ui->groupBoxInput->height());

    ui->groupBoxOutput->move(x1, ui->groupBoxInput->y() + ui->groupBoxInput->height() + MAPBORDER);
    ui->groupBoxOutput->resize(TOOLSWIDTH, ui->groupBoxOutput->height());
}


void MainWindow::updateMaps()
{
    rasterObj->updateCenter();
}


void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    updateMaps();
}


void MainWindow::mouseDoubleClickEvent(QMouseEvent * event)
{
    QPoint mapPos = getMapPos(event->pos());
    if (! isInsideMap(mapPos)) return;

    Position newCenter = this->mapView->mapToScene(mapPos);
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
}


void MainWindow::mouseMoveEvent(QMouseEvent * event)
{
    QPoint mapPos = getMapPos(event->pos());
    if (! isInsideMap(mapPos)) return;

    Position geoPoint = this->mapView->mapToScene(mapPos);
    this->ui->statusBar->showMessage(QString::number(geoPoint.latitude()) + " " + QString::number(geoPoint.longitude()));
}


void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton)
    {
        contextMenuRequested(event->pos(), event->globalPos());
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
    resetMeteoPoints();
    if (! myProject.meteoPointsLoaded || myProject.nrMeteoPoints == 0) return;
    addMeteoPoints();

    myProject.loadMeteoPointsData (myProject.getCurrentDate(), myProject.getCurrentDate(), true);

    showPointsGroup->setEnabled(true);

    currentPointsVisualization = showLocation;
    redrawMeteoPoints(currentPointsVisualization, true);

    updateDateTime();
}


void MainWindow::setProjectTileMap()
{
    if (myProject.tileMap != "")
    {
        if (myProject.tileMap.toUpper() == "ESRI" || myProject.tileMap == "ESRIWorldImagery")
        {
            this->setTileMapSource(OSMTileSource::ESRIWorldImagery);
        }
        else if (myProject.tileMap.toUpper() == "TERRAIN")
        {
            this->setTileMapSource(OSMTileSource::Terrain);
        }
        else
        {
            this->setTileMapSource(OSMTileSource::OSMTiles);
        }
    }
    else
    {
        // default: Open Street Map
        this->setTileMapSource(OSMTileSource::OSMTiles);
    }
}


void MainWindow::drawProject()
{
    setProjectTileMap();

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

    updateMaps();

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

    myProject.hourlyMeteoMaps = new Crit3DHourlyMeteoMaps(myProject.DEM);

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


QPoint MainWindow::getMapPos(const QPoint& pos)
{
    QPoint mapPoint;
    int dx = ui->widgetMap->x();
    int dy = ui->widgetMap->y() + ui->menuBar->height();
    mapPoint.setX(pos.x() - dx - MAPBORDER);
    mapPoint.setY(pos.y() - dy - MAPBORDER);
    return mapPoint;
}


bool MainWindow::isInsideMap(const QPoint& pos)
{
    if (pos.x() > 0 && pos.y() > 0 &&
        pos.x() < (mapView->width() - MAPBORDER*2) &&
        pos.y() < (mapView->height() - MAPBORDER*2) )
    {
        return true;
    }
    else return false;
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


void MainWindow::on_dateEdit_dateChanged(const QDate &date)
{
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
    if (myProject.loadMeteoPointsDB(dbName))
        {
            drawMeteoPoints();
            return true;
        }
        else
            return false;
}


void MainWindow::on_variableButton_clicked()
{
    myProject.setCurrentVariable(chooseMeteoVariable(&myProject));
    this->currentPointsVisualization = showCurrentVariable;
    this->updateVariable();
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
    if (! isEqual(startCenter->latitude(), myProject.gisSettings.startLocation.latitude)
            || ! isEqual(startCenter->longitude(), myProject.gisSettings.startLocation.longitude))
    {
        startCenter->setLatitude(myProject.gisSettings.startLocation.latitude);
        startCenter->setLongitude(myProject.gisSettings.startLocation.longitude);
        this->mapView->centerOn(startCenter->lonLat());
    }

    mySettingsDialog->close();
}


void MainWindow::on_actionCriteria3D_Initialize_triggered()
{
    myProject.initializeCriteria3DModel();
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
        updateMaps();
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
    if (this->checkMapVariable(myProject.hourlyMeteoMaps->isComputed))
        setMapVariable(referenceEvapotranspiration, myProject.hourlyMeteoMaps->mapHourlyET0);
}


void MainWindow::on_actionView_Air_temperature_triggered()
{
    if (this->checkMapVariable(myProject.hourlyMeteoMaps->isComputed))
        setMapVariable(airTemperature, myProject.hourlyMeteoMaps->mapHourlyT);
}


void MainWindow::on_actionView_Precipitation_triggered()
{
    if (this->checkMapVariable(myProject.hourlyMeteoMaps->isComputed))
        setMapVariable(precipitation, myProject.hourlyMeteoMaps->mapHourlyPrec);
}


void MainWindow::on_actionView_Air_relative_humidity_triggered()
{
    if (this->checkMapVariable(myProject.hourlyMeteoMaps->isComputed))
        setMapVariable(airRelHumidity, myProject.hourlyMeteoMaps->mapHourlyRelHum);
}


void MainWindow::on_actionView_Wind_intensity_triggered()
{
    if (this->checkMapVariable(myProject.hourlyMeteoMaps->isComputed))
        setMapVariable(windIntensity, myProject.hourlyMeteoMaps->mapHourlyWindInt);
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
    this->setTileMapSource(OSMTileSource::Terrain);
}


void MainWindow::on_actionMapOpenStreetMap_triggered()
{
    this->setTileMapSource(OSMTileSource::OSMTiles);
}


void MainWindow::on_actionMapESRISatellite_triggered()
{
    this->setTileMapSource(OSMTileSource::ESRIWorldImagery);
}


void MainWindow::setTileMapSource(OSMTileSource::OSMTileType mySource)
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


void MainWindow::openSoilWidget(QPoint mapPos)
{
    double x, y;
    Position geoPos = mapView->mapToScene(mapPos);
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
        QString fileName = myProject.getCompleteFileName(myProject.soilDbFileName, PATH_SOIL);
        soilWidget->show();
        soilWidget->setDbSoil(fileName, soilCode);
    }
}


void MainWindow::contextMenuRequested(QPoint localPos, QPoint globalPos)
{
    QMenu submenu;
    int nrItems = 0;

    QPoint mapPos = getMapPos(localPos);
    if (! isInsideMap(mapPos)) return;

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
                openSoilWidget(mapPos);
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
    if (dbName != "") this->loadMeteoPointsDB(dbName);
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

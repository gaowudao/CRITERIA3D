#include <QGridLayout>
#include <QFileDialog>
#include <QtDebug>
#include <QMessageBox>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QListWidget>
#include <QRadioButton>
#include <QTextBrowser>
#include <QIODevice>

#include <sstream>
#include <iostream>
#include <fstream>

#include "tileSources/OSMTileSource.h"
#include "tileSources/CompositeTileSource.h"

#include "mainWindow.h"
#include "ui_mainWindow.h"
#include "formInfo.h"
#include "dbMeteoPointsHandler.h"
#include "dbArkimet.h"
#include "download.h"
#include "pragaProject.h"
#include "commonConstants.h"
#include "dialogSelection.h"
#include "dialogDownloadMeteoData.h"
#include "dialogMeteoComputation.h"
#include "dialogClimateFields.h"
#include "dialogSeriesOnZones.h"
#include "dialogInterpolation.h"
#include "dialogPragaSettings.h"
#include "gis.h"
#include "spatialControl.h"

extern PragaProject myProject;

#define MAPBORDER 8
#define TOOLSWIDTH 260

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    this->setMouseTracking(true);
    ui->setupUi(this);

    this->myRubberBand = nullptr;

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

    KeyboardFilter *keyboardFilter = new KeyboardFilter();
    this->ui->dateEdit->installEventFilter(keyboardFilter);
    //connect(this->ui->dateEdit, SIGNAL(editingFinished()), this, SLOT(on_dateChanged()));

    ui->meteoPoints->setEnabled(false);
    ui->grid->setEnabled(false);

    // show menu
    showPointsGroup = new QActionGroup(this);
    showPointsGroup->setExclusive(true);
    showPointsGroup->addAction(this->ui->actionShowPointsHide);
    showPointsGroup->addAction(this->ui->actionShowPointsLocation);
    showPointsGroup->addAction(this->ui->actionShowPointsCurrent);
    showPointsGroup->addAction(this->ui->actionShowPointsElab);
    showPointsGroup->addAction(this->ui->actionShowPointsAnomalyAbs);
    showPointsGroup->addAction(this->ui->actionShowPointsAnomalyPerc);
    showPointsGroup->addAction(this->ui->actionShowPointsClimate);

    showPointsGroup->setEnabled(false);
    this->ui->menuShowPointsAnomaly->setEnabled(false);

    showGridGroup = new QActionGroup(this);
    showGridGroup->setExclusive(true);
    showGridGroup->addAction(this->ui->actionShowGridHide);
    showGridGroup->addAction(this->ui->actionShowGridLocation);
    showGridGroup->addAction(this->ui->actionShowGridCurrent);
    showGridGroup->addAction(this->ui->actionShowGridElab);
    showGridGroup->addAction(this->ui->actionShowGridAnomalyAbs);
    showGridGroup->addAction(this->ui->actionShowGridAnomalyPerc);
    showGridGroup->addAction(this->ui->actionShowGridClimate);

    showGridGroup->setEnabled(false);
    this->ui->menuShowGridAnomaly->setEnabled(false);

    this->currentPointsVisualization = notShown;
    this->currentGridVisualization = notShown;

    this->setWindowTitle("PRAGA");

    ui->groupBoxElab->hide();
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


void MainWindow::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)
    mapView->resize(ui->widgetMap->size());
}


void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event)

    this->rasterObj->updateCenter();
    this->meteoGridObj->updateCenter();

    gis::Crit3DGeoPoint pointSelected;

    if (myRubberBand != nullptr && myRubberBand->isVisible())
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
            QPoint pos = event->pos();
            QPointF firstCorner = event->localPos();
            myRubberBand->setFirstCorner(firstCorner);
            QPoint mapPoint = getMapPoint(&pos);
            myRubberBand->setOrigin(mapPoint);
            myRubberBand->setGeometry(QRect(mapPoint, QSize()));
            myRubberBand->isActive = true;
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
}

void MainWindow::on_actionOpen_DEM_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open raster Grid"), "", tr("ESRI grid files (*.flt)"));

    if (fileName == "") return;

    if (!myProject.loadDEM(fileName)) return;

    renderDEM();

}

void MainWindow::on_actionOpen_meteo_points_DB_triggered()
{
    QString dbName = QFileDialog::getOpenFileName(this, tr("Open DB meteo points"), "", tr("DB files (*.db)"));
    if (dbName != "") this->loadMeteoPoints(dbName);
}


void MainWindow::on_actionOpen_meteo_grid_triggered()
{
    QString xmlName = QFileDialog::getOpenFileName(this, tr("Open XML DB meteo grid"), "", tr("xml files (*.xml)"));
    if (xmlName != "") this->loadMeteoGrid(xmlName);
}


void MainWindow::on_actionNewMeteoPointsArkimet_triggered()
{
    resetMeteoPoints();

    QString templateFileName = myProject.getDefaultPath() + PATH_TEMPLATE + "template_meteo_arkimet.db";

    QString dbName = QFileDialog::getSaveFileName(this, tr("Save as"), "", tr("DB files (*.db)"));
    if (dbName == "")
    {
        qDebug() << "missing new db file name";
        return;
    }

    QFile dbFile(dbName);
    if (dbFile.exists())
    {
        myProject.closeMeteoPointsDB();
        myProject.setIsElabMeteoPointsValue(false);

        if (! dbFile.remove())
        {
            myProject.logError("Remove file failed: " + dbName + "\n" + dbFile.errorString());
            return;
        }
    }

    if (! QFile::copy(templateFileName, dbName))
    {
        myProject.logError("Copy file failed: " + templateFileName);
        return;
    }

    Download myDownload(dbName);

    QStringList dataset = myDownload.getDbArkimet()->getDatasetsList();

    QDialog datasetDialog;

    datasetDialog.setWindowTitle("Datasets");
    datasetDialog.setFixedWidth(500);
    QVBoxLayout layout;

    for (int i = 0; i < dataset.size(); i++)
    {
        QCheckBox* dat = new QCheckBox(dataset[i]);
        layout.addWidget(dat);

        datasetCheckbox.append(dat);
    }

    all = new QCheckBox("ALL");
    layout.addSpacing(30);
    layout.addWidget(all);

    connect(all, SIGNAL(toggled(bool)), this, SLOT(enableAllDataset(bool)));

    for (int i = 0; i < dataset.size(); i++)
    {
        connect(datasetCheckbox[i], SIGNAL(toggled(bool)), this, SLOT(disableAllButton(bool)));
    }

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                         | QDialogButtonBox::Cancel);

    connect(buttonBox, SIGNAL(accepted()), &datasetDialog, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), &datasetDialog, SLOT(reject()));

    layout.addWidget(buttonBox);
    datasetDialog.setLayout(&layout);

    QString datasetSelected = selectArkimetDataset(&datasetDialog);

    if (!datasetSelected.isEmpty())
    {
        myDownload.getDbArkimet()->setDatasetsActive(datasetSelected);
        QStringList datasets = datasetSelected.remove("'").split(",");

        FormInfo myInfo;
        myInfo.start("download points properties...", 0);
            if (myDownload.getPointProperties(datasets))
            {
                myProject.loadMeteoPointsDB(dbName);
                this->addMeteoPoints();
            }
            else
            {
                QMessageBox::information(nullptr, "Network Error!", "Error in function getPointProperties");
            }

        myInfo.close();
    }
    else
    {
        QFile::remove(dbName);
    }

    delete buttonBox;
    delete all;
}


QString MainWindow::selectArkimetDataset(QDialog* datasetDialog) {

        datasetDialog->exec();

        if (datasetDialog->result() == QDialog::Accepted)
        {

            QString datasetSelected = "";
            foreach (QCheckBox *checkBox, datasetCheckbox)
            {
                if (checkBox->isChecked())
                {
                    datasetSelected = datasetSelected % "'" % checkBox->text() % "',";
                }
            }

            if (!datasetSelected.isEmpty())
            {
                datasetSelected = datasetSelected.left(datasetSelected.size() - 1);
                return datasetSelected;
            }
            else
            {
                QMessageBox msgBox;
                msgBox.setText("Select a dataset");
                msgBox.exec();
                return selectArkimetDataset(datasetDialog);
            }
        }
        else
            return "";
}


void MainWindow::enableAllDataset(bool toggled)
{
    bool AllChecked = 1;

    foreach (QCheckBox *checkBox, datasetCheckbox)
    {
        if (toggled)
        {
            checkBox->setChecked(toggled);
        }
        else
        {
            if (!checkBox->isChecked())
            {
                AllChecked = 0;
            }
        }
    }

    foreach (QCheckBox *checkBox, datasetCheckbox)
    {
        if(AllChecked)
        {
            checkBox->setChecked(toggled);
        }
    }
}

void MainWindow::disableAllButton(bool toggled)
{
    if (!toggled)
    {
        if (all->isChecked())
        {
            all->setChecked(false);
        }
    }
}

void MainWindow::on_actionDownload_meteo_data_triggered()
{
    if (downloadMeteoData(&myProject))
        this->loadMeteoPoints(myProject.meteoPointsDbHandler->getDbName());
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

    datasetCheckbox.clear();

    this->myRubberBand = nullptr;
}


void MainWindow::on_actionVariableQualitySpatial_triggered()
{
    myProject.checkSpatialQuality = ui->actionVariableQualitySpatial->isChecked();
    updateVariable();
    redrawMeteoPoints(currentPointsVisualization, true);
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

void MainWindow::interpolateGridGUI()
{
    if (myProject.interpolationMeteoGrid(myProject.getCurrentVariable(), myProject.getFrequency(), myProject.getCurrentTime(), &(myProject.dataRaster), true))
    {
        setCurrentRaster(&(myProject.meteoGridDbHandler->meteoGrid()->dataMeteoGrid));
        ui->labelRasterScale->setText(QString::fromStdString(getVariableString(myProject.getCurrentVariable())));

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

            else if (myProject.getCurrentVariable() == dailyPrecipitation)
                    myProject.setCurrentVariable(precipitation);

            else if (myProject.getCurrentVariable() == dailyGlobalRadiation)
                myProject.setCurrentVariable(globalIrradiance);
        }
    }

    std::string myString = getVariableString(myProject.getCurrentVariable());
    ui->labelVariable->setText(QString::fromStdString(myString));
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
    redrawMeteoGrid(currentGridVisualization);
}


void MainWindow::on_timeEdit_timeChanged(const QTime &time)
{
    //hour
    if (time.hour() != myProject.getCurrentHour())
    {
        myProject.setCurrentHour(time.hour());
    }

    redrawMeteoPoints(currentPointsVisualization, true);
    redrawMeteoGrid(currentGridVisualization);
}





#ifdef NETCDF
    void MainWindow::on_actionOpen_NetCDF_data_triggered()
    {
        QString fileName = QFileDialog::getOpenFileName(this, tr("Open NetCDF data"), "", tr("NetCDF files (*.nc)"));

        if (fileName == "") return;

        myProject.netCDF.initialize(myProject.gisSettings.utmZone);

        std::stringstream buffer;
        myProject.netCDF.readProperties(fileName.toStdString(), &buffer);

        if (myProject.netCDF.isLatLon)
            meteoGridObj->initializeLatLon(&(myProject.netCDF.dataGrid), myProject.gisSettings, myProject.netCDF.latLonHeader, true);
        else
            meteoGridObj->initializeUTM(&(myProject.netCDF.dataGrid), myProject.gisSettings, true);

        myProject.netCDF.dataGrid.emptyGrid();
        meteoGridObj->updateCenter();

        QDialog myDialog;
        QVBoxLayout mainLayout;

        myDialog.setWindowTitle("NetCDF file info  ");

        QTextBrowser textBrowser;
        textBrowser.setText(QString::fromStdString(buffer.str()));
        buffer.clear();

        mainLayout.addWidget(&textBrowser);

        myDialog.setLayout(&mainLayout);
        myDialog.setFixedSize(800,600);
        myDialog.exec();
    }

    // deactivated
    void MainWindow::on_actionExtract_NetCDF_series_triggered()
    {
        int idVar;
        QDateTime firstDate, lastDate;

        if (chooseNetCDFVariable(&idVar, &firstDate, &lastDate))
        {
            QMessageBox::information(nullptr, "Variable",
                                     "Variable: " + QString::number(idVar)
                                     + "\nfirst date: " + firstDate.toString()
                                     + "\nLast Date: " + lastDate.toString());
        }
    }

    void exportNetCDFDataSeries(gis::Crit3DGeoPoint geoPoint)
    {
        if (myProject.netCDF.isPointInside(geoPoint))
        {
            int idVar;
            QDateTime firstTime, lastTime;

            if (chooseNetCDFVariable(&idVar, &firstTime, &lastTime))
            {
                std::stringstream buffer;
                if (! myProject.netCDF.exportDataSeries(idVar, geoPoint, firstTime.toTime_t(), lastTime.toTime_t(), &buffer))
                    QMessageBox::information(nullptr, "ERROR", QString::fromStdString(buffer.str()));
                else
                {
                    QString fileName = QFileDialog::getSaveFileName(nullptr, "Save data series", "", "csv files (*.csv)");
                    std::ofstream myFile;
                    myFile.open(fileName.toStdString());
                    myFile << buffer.str();
                    myFile.close();
                }
            }
        }
    }
#endif

void MainWindow::drawMeteoPoints()
{
    this->resetMeteoPoints();
    this->addMeteoPoints();

    this->updateDateTime();

    myProject.loadMeteoPointsData (myProject.getCurrentDate(), myProject.getCurrentDate(), true);

    this->ui->meteoPoints->setEnabled(true);
    this->ui->meteoPoints->setChecked(true);
    showPointsGroup->setEnabled(true);
    this->ui->actionShowPointsCurrent->setEnabled(false);
    this->ui->actionShowPointsElab->setEnabled(false);
    this->ui->actionShowPointsClimate->setEnabled(false);

    this->ui->grid->setChecked(false);

    currentPointsVisualization = showLocation;
    redrawMeteoPoints(currentPointsVisualization, true);
}

void MainWindow::redrawMeteoPoints(visualizationType showType, bool updateColorSCale)
{
    currentPointsVisualization = showType;
    ui->groupBoxElab->hide();

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
        this->ui->actionShowPointsHide->setChecked(true);
        break;
    }
    case showLocation:
    {
        this->ui->actionShowPointsLocation->setChecked(true);
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
        this->ui->actionShowPointsCurrent->setChecked(true);
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
    case showElaboration:
    {
        this->ui->actionShowPointsElab->setChecked(true);
        showElabResult(true, false, false, false, false, nullptr);
        break;
    }
    case showAnomalyAbsolute:
    {
        this->ui->actionShowPointsAnomalyAbs->setChecked(true);
        showElabResult(true, false, true, false, false, nullptr);
        break;
    }
    case showAnomalyPercentage:
    {
        this->ui->actionShowPointsAnomalyPerc->setChecked(true);
        showElabResult(true, false, true, true, false, nullptr);
        break;
    }
    case showClimate:
    {
        this->ui->actionShowPointsClimate->setChecked(true);
        showElabResult(true, false, false, false, true, myProject.climateIndex);
        break;
    }
    }
}

bool MainWindow::loadMeteoPoints(QString dbName)
{
    if (myProject.loadMeteoPointsDB(dbName))
    {
        drawMeteoPoints();
        return true;
    }
    else
        return false;
}

void MainWindow::drawMeteoGrid()
{
    if (myProject.meteoGridDbHandler == nullptr) return;

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

    myProject.setCurrentDate(myProject.meteoGridDbHandler->lastDate());
    this->updateDateTime();

    myProject.loadMeteoGridData(myProject.getCurrentDate(), myProject.getCurrentDate(), true);

    meteoGridObj->redrawRequested();

    this->ui->meteoPoints->setChecked(false);
    this->ui->grid->setEnabled(true);
    this->ui->grid->setChecked(true);
    showGridGroup->setEnabled(true);
    if (myProject.getCurrentVariable() != noMeteoVar && myProject.getFrequency() != noFrequency)
    {
        this->ui->actionShowGridCurrent->setEnabled(true);
    }
    else
    {
        this->ui->actionShowGridCurrent->setEnabled(false);
    }
    this->ui->actionShowGridElab->setEnabled(false);
    this->ui->actionShowGridClimate->setEnabled(false);

    currentGridVisualization = showLocation;
    redrawMeteoGrid(currentGridVisualization);
}



void MainWindow::redrawMeteoGrid(visualizationType showType)
{
    currentGridVisualization = showType;
    ui->groupBoxElab->hide();

    if (myProject.meteoGridDbHandler == nullptr)
        return;

    switch(currentGridVisualization)
    {
    case notShown:
    {
        this->ui->actionShowGridHide->setChecked(true);
        myProject.meteoGridDbHandler->meteoGrid()->fillMeteoRasterNoData();
        meteoGridObj->setDrawBorders(false);
        meteoGridLegend->setVisible(false);
        break;
    }
    case showLocation:
    {
        this->ui->actionShowGridLocation->setChecked(true);
        myProject.meteoGridDbHandler->meteoGrid()->fillMeteoRasterNoData();
        meteoGridObj->setDrawBorders(true);
        break;
    }
    case showCurrentVariable:
    {
        this->ui->actionShowGridCurrent->setChecked(true);
        frequencyType frequency = myProject.getFrequency();
        meteoVariable variable = myProject.getCurrentVariable();
        meteoGridObj->setDrawBorders(false);

        if (myProject.getCurrentVariable() == noMeteoVar)
        {
            meteoGridLegend->setVisible(false);
            ui->labelMeteoGridScale->setText("");
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

        meteoGridLegend->update();
        break;
    }
    case showElaboration:
    {
        this->ui->actionShowGridElab->setChecked(true);
        showElabResult(true, true, false, false, false, nullptr);
        break;
    }
    case showAnomalyAbsolute:
    {
        this->ui->actionShowGridAnomalyAbs->setChecked(true);
        showElabResult(true, true, true, false, false, nullptr);
        break;
    }
    case showAnomalyPercentage:
    {
        this->ui->actionShowGridAnomalyPerc->setChecked(true);
        showElabResult(true, true, true, true, false, nullptr);
        break;
    }
    case showClimate:
    {
        this->ui->actionShowGridClimate->setChecked(true);
        showElabResult(true, true, false, false, true, myProject.climateIndex);
        break;
    }

    }
    meteoGridObj->updateCenter();
    meteoGridObj->redrawRequested();
}

bool MainWindow::loadMeteoGrid(QString xmlName)
{
    if (myProject.loadMeteoGridDB(xmlName))
    {
        drawMeteoGrid();
        return true;
    }
    else
    {
        myProject.logError();
        return false;
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
        this->updateVariable();

        if (myProject.getFrequency() != noFrequency)
        {
            this->ui->actionShowPointsCurrent->setEnabled(true);
            this->ui->actionShowGridCurrent->setEnabled(true);
            redrawMeteoPoints(showCurrentVariable, true);
            redrawMeteoGrid(showCurrentVariable);
        }
    }
}

void MainWindow::on_frequencyButton_clicked()
{
   frequencyType myFrequency = chooseFrequency();

   if (myFrequency != noFrequency)
   {
       myProject.setFrequency(myFrequency);
       this->updateVariable();

       if (myProject.getCurrentVariable() != noMeteoVar)
       {
           this->ui->actionShowPointsCurrent->setEnabled(true);
           this->ui->actionShowGridCurrent->setEnabled(true);
           redrawMeteoPoints(showCurrentVariable, true);
           redrawMeteoGrid(showCurrentVariable);
       }
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
    resetMeteoPoints();
    meteoPointsLegend->setVisible(false);

    myProject.closeMeteoPointsDB();

    myProject.setIsElabMeteoPointsValue(false);
    ui->groupBoxElab->hide();

    this->ui->meteoPoints->setChecked(false);
    this->ui->meteoPoints->setEnabled(false);

    showPointsGroup->setEnabled(false);
    this->ui->menuShowPointsAnomaly->setEnabled(false);

    if (myProject.meteoGridDbHandler != nullptr)
    {
        this->ui->grid->setChecked(true);
    }
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
        ui->groupBoxElab->hide();
        ui->meteoGridOpacitySlider->setEnabled(false);

        this->ui->grid->setChecked(false);
        this->ui->grid->setEnabled(false);

        showGridGroup->setEnabled(false);
        this->ui->menuShowGridAnomaly->setEnabled(false);

        if (myProject.meteoPointsDbHandler != nullptr)
        {
            this->ui->meteoPoints->setChecked(true);
        }
    }

}

void MainWindow::on_actionInterpolation_to_DEM_triggered()
{
    FormInfo myInfo;
    myInfo.start("Interpolation...", 0);

    interpolateDemGUI();

    myInfo.close();
}


void MainWindow::on_actionInterpolation_to_Grid_triggered()
{
    FormInfo myInfo;
    myInfo.start("Interpolation Grid...", 0);

    interpolateGridGUI();

    myInfo.close();
}

void MainWindow::on_actionSave_meteo_grid_triggered()
{
    if (myProject.meteoGridDbHandler != nullptr)
    {
        myProject.saveGrid(myProject.getCurrentVariable(), myProject.getFrequency(), myProject.getCurrentTime(), true);
    }
}

void MainWindow::on_actionCompute_elaboration_triggered()
{

    if (!ui->meteoPoints->isChecked() && !ui->grid->isChecked())
    {
        myProject.errorString = "Load meteo Points or grid";
        myProject.logError();
        return;
    }

    bool isMeteoGrid = ui->grid->isChecked();
    bool isAnomaly = false;
    bool saveClima = false;

    if (myProject.elaborationCheck(isMeteoGrid, isAnomaly))
    {
        DialogMeteoComputation compDialog(myProject.pragaDefaultSettings, isAnomaly, saveClima);
        if (compDialog.result() != QDialog::Accepted)
            return;

        if (!myProject.elaboration(isMeteoGrid, isAnomaly, saveClima))
        {
            myProject.logError();
        }
        else
        {
            if (isMeteoGrid)
            {
                this->ui->actionShowGridElab->setEnabled(true);
                redrawMeteoGrid(showElaboration);
            }
            else
            {
                this->ui->actionShowPointsElab->setEnabled(true);
                redrawMeteoPoints(showElaboration, true);
            }
        }
        if (compDialog.result() == QDialog::Accepted)
            on_actionCompute_elaboration_triggered();
    }
    else
    {
         myProject.logError();
    }
    return;

}

void MainWindow::on_actionCompute_anomaly_triggered()
{

    if (!ui->meteoPoints->isChecked() && !ui->grid->isChecked())
    {
        myProject.errorString = "Load meteo Points or grid";
        myProject.logError();
        return;
    }

    bool isMeteoGrid = ui->grid->isChecked();

    bool isAnomaly = true;
    bool saveClima = false;

    if (myProject.elaborationCheck(isMeteoGrid, isAnomaly))
    {
        DialogMeteoComputation compDialog(myProject.pragaDefaultSettings, isAnomaly, saveClima);
        if (compDialog.result() != QDialog::Accepted)
            return;

        isAnomaly = false;

        bool res = myProject.elaboration(isMeteoGrid, isAnomaly, saveClima);
        if (!res)
        {
            myProject.logError();
        }
        else
        {
            isAnomaly = true;

            myProject.elaboration(isMeteoGrid, isAnomaly, saveClima);
            if (isMeteoGrid)
            {
                this->ui->menuShowGridAnomaly->setEnabled(true);
                redrawMeteoGrid(showAnomalyAbsolute);
            }
            else
            {
                this->ui->menuShowPointsAnomaly->setEnabled(true);
                redrawMeteoPoints(showAnomalyAbsolute, true);
            }
        }
        if (compDialog.result() == QDialog::Accepted)
            on_actionCompute_anomaly_triggered();
    }
    else
    {
         myProject.logError();
    }
    return;
}

void MainWindow::on_actionCompute_climate_triggered()
{
    if (!ui->meteoPoints->isChecked() && !ui->grid->isChecked())
    {
        myProject.errorString = "Load meteo Points or grid";
        myProject.logError();
        return;
    }

    bool isMeteoGrid = ui->grid->isChecked();
    bool isAnomaly = false;
    bool saveClima = true;

    if (myProject.elaborationCheck(isMeteoGrid, isAnomaly))
    {
        myProject.clima->resetListElab();
        DialogMeteoComputation compDialog(myProject.pragaDefaultSettings, isAnomaly, saveClima);
        if (compDialog.result() != QDialog::Accepted)
            return;

        myProject.clima->getListElab()->setListClimateElab(compDialog.getElabSaveList());
        if (!myProject.elaboration(isMeteoGrid, isAnomaly, saveClima))
        {
            myProject.logError();
        }

        if (compDialog.result() == QDialog::Accepted)
            on_actionCompute_climate_triggered();

    }
    else
    {
         myProject.logError();
    }

    return;
}

void MainWindow::on_actionClimate_fields_triggered()
{
    if (!ui->meteoPoints->isChecked() && !ui->grid->isChecked())
    {
        myProject.errorString = "Load meteo Points or grid";
        myProject.logError();
        return;
    }

    bool isMeteoGrid = ui->grid->isChecked();
    QStringList climateDbElab;
    QStringList climateDbVarList;
    myProject.clima->resetListElab();
    if (myProject.showClimateFields(isMeteoGrid, &climateDbElab, &climateDbVarList))
    {
        DialogClimateFields climateDialog(climateDbElab, climateDbVarList);
        if (climateDialog.result() == QDialog::Accepted)
        {
            QString climaSelected = climateDialog.getSelected();

            if (climateDialog.getIsShowClicked())
            {
                QString index = climateDialog.getIndexSelected();
                myProject.climateIndex = index;
                myProject.saveClimateResult(isMeteoGrid, climaSelected, index.toInt(), true);
                if (isMeteoGrid)
                {
                    this->ui->actionShowGridClimate->setEnabled(true);
                    redrawMeteoGrid(showClimate);
                }
                else
                {
                    this->ui->actionShowPointsClimate->setEnabled(true);
                    redrawMeteoPoints(showClimate, true);
                }
            }
            else
            {
                myProject.deleteClima(isMeteoGrid, climaSelected);
            }

        }
        else
        {
            return;
        }

    }
    return;

}

void MainWindow::showElabResult(bool updateColorSCale, bool isMeteoGrid, bool isAnomaly, bool isAnomalyPerc, bool isClima, QString index)
{

    if (isMeteoGrid)
    {
        meteoGridObj->setDrawBorders(false);
        if (!isAnomaly)
        {
            if (!isClima)
            {
                myProject.meteoGridDbHandler->meteoGrid()->fillMeteoRasterElabValue();
            }
            else
            {
                myProject.meteoGridDbHandler->meteoGrid()->fillMeteoRasterClimateValue();
            }
        }
        else
        {
            if (isAnomalyPerc)
            {
                myProject.meteoGridDbHandler->meteoGrid()->fillMeteoRasterAnomalyPercValue();
            }
            else
            {
                myProject.meteoGridDbHandler->meteoGrid()->fillMeteoRasterAnomalyValue();
            }

        }
        setColorScale(myProject.clima->variable(), myProject.meteoGridDbHandler->meteoGrid()->dataMeteoGrid.colorScale);
        ui->labelMeteoGridScale->setText(QString::fromStdString(getVariableString(myProject.clima->variable())));
        meteoGridLegend->setVisible(true);
        meteoGridLegend->update();
    }
    else
    {
        meteoPointsLegend->setVisible(true);

        if (updateColorSCale)
        {
            float minimum = NODATA;
            float maximum = NODATA;
            for (int i = 0; i < myProject.nrMeteoPoints; i++)
            {
                if (!isAnomaly)
                {
                    if (!isClima)
                    {
                        myProject.meteoPoints[i].currentValue = myProject.meteoPoints[i].elaboration;
                    }
                    else
                    {
                        myProject.meteoPoints[i].currentValue = myProject.meteoPoints[i].climate;
                    }
                }
                else
                {
                    if (isAnomalyPerc)
                    {
                        myProject.meteoPoints[i].currentValue = myProject.meteoPoints[i].anomalyPercentage;
                    }
                    else
                    {
                        myProject.meteoPoints[i].currentValue = myProject.meteoPoints[i].anomaly;
                    }

                }

                // hide all meteo points
                pointList[i]->setVisible(false);

                float v = myProject.meteoPoints[i].currentValue;

                if (int(v) != NODATA)
                {
                    if (int(minimum) == NODATA)
                    {
                        minimum = v;
                        maximum = v;
                    }
                    else if (v < minimum) minimum = v;
                    else if (v > maximum) maximum = v;
                }

            }
            myProject.meteoPointsColorScale->setRange(minimum, maximum);
            roundColorScale(myProject.meteoPointsColorScale, 4, true);
            setColorScale(myProject.clima->variable(), myProject.meteoPointsColorScale);
        }


        Crit3DColor *myColor;
        for (int i = 0; i < myProject.nrMeteoPoints; i++)
        {

            if (!updateColorSCale)
            {
                if (!isAnomaly)
                {
                    if (!isClima)
                    {
                        myProject.meteoPoints[i].currentValue = myProject.meteoPoints[i].elaboration;
                    }
                    else
                    {
                        myProject.meteoPoints[i].currentValue = myProject.meteoPoints[i].climate;
                    }
                }
                else
                {
                    if (isAnomalyPerc)
                    {
                        myProject.meteoPoints[i].currentValue = myProject.meteoPoints[i].anomalyPercentage;
                    }
                    else
                    {
                        myProject.meteoPoints[i].currentValue = myProject.meteoPoints[i].anomaly;
                    }
                }
                // hide all meteo points
                pointList[i]->setVisible(false);
            }
            if (int(myProject.meteoPoints[i].currentValue) != NODATA)
            {

                pointList[i]->setRadius(5);
                myColor = myProject.meteoPointsColorScale->getColor(myProject.meteoPoints[i].currentValue);
                pointList[i]->setFillColor(QColor(myColor->red, myColor->green, myColor->blue));
                pointList[i]->setToolTip(&(myProject.meteoPoints[i]));
                pointList[i]->setVisible(true);
            }
        }

        meteoPointsLegend->update();

    }


    if (int(myProject.clima->param1()) != NODATA)
    {
        if (!isAnomaly)
        {

            ui->lineEditElab1->setText(myProject.clima->elab1() + " " + QString::number(myProject.clima->param1()));
        }
        else
        {
            if (isAnomalyPerc)
            {
                ui->lineEditElab1->setText(myProject.clima->elab1() + "%Anomaly of: " + QString::number(myProject.clima->param1()));
            }
            else
            {
                ui->lineEditElab1->setText(myProject.clima->elab1() + "Anomaly of: " + QString::number(myProject.clima->param1()));
            }

        }
    }
    else
    {
        if (!isAnomaly)
        {
            ui->lineEditElab1->setText(myProject.clima->elab1());
        }
        else
        {
            if (isAnomalyPerc)
            {
                ui->lineEditElab1->setText("%Anomaly respect to " + myProject.clima->elab1());
            }
            else
            {
                ui->lineEditElab1->setText("Anomaly respect to " + myProject.clima->elab1());
            }

        }
    }
    if (myProject.clima->elab2().isEmpty())
    {
        ui->lineEditElab2->setVisible(false);
    }
    else
    {
        ui->lineEditElab2->setVisible(true);
        if (int(myProject.clima->param2()) != NODATA)
        {
            ui->lineEditElab2->setText(myProject.clima->elab2() + " " + QString::number(myProject.clima->param2()));
        }
        else
        {
            ui->lineEditElab2->setText(myProject.clima->elab2());
        }

    }
    std::string var = MapDailyMeteoVarToString.at(myProject.clima->variable());
    ui->lineEditVariable->setText(QString::fromStdString(var));
    QString startDay = QString::number(myProject.clima->genericPeriodDateStart().day());
    QString startMonth = QString::number(myProject.clima->genericPeriodDateStart().month());
    QString endDay = QString::number(myProject.clima->genericPeriodDateEnd().day());
    QString endMonth = QString::number(myProject.clima->genericPeriodDateEnd().month());

    QString startYear = QString::number(myProject.clima->yearStart());
    QString endYear = QString::number(myProject.clima->yearEnd());
    if (!isClima)
    {
        ui->lineEditPeriod->setText(startDay + "/" + startMonth + "-" + endDay + "/" + endMonth + " " + startYear + "÷" + endYear);
    }
    else
    {
        if (myProject.clima->periodType() != genericPeriod && myProject.clima->periodType() != annualPeriod)
        {
            ui->lineEditPeriod->setText(startYear + "÷" + endYear + "-" + myProject.clima->periodStr() + " index: " + index);
        }
        else
        {
            ui->lineEditPeriod->setText(startYear + "÷" + endYear + "-" + myProject.clima->periodStr());
        }
    }

    ui->lineEditElab1->setReadOnly(true);
    ui->lineEditElab2->setReadOnly(true);
    ui->lineEditVariable->setReadOnly(true);
    ui->lineEditPeriod->setReadOnly(true);
    ui->groupBoxElab->show();


}

void MainWindow::on_actionInterpolationSettings_triggered()
{
    DialogInterpolation* myDialogInterpolation = new DialogInterpolation(&myProject);
    myDialogInterpolation->close();
}


void MainWindow::on_actionParameters_triggered()
{
    DialogPragaSettings* mySettingsDialog = new DialogPragaSettings(myProject.projectSettings, myProject.pragaDefaultSettings, &myProject.gisSettings, myProject.quality, myProject.meteoSettings, myProject.clima->getElabSettings());
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


void MainWindow::on_actionWriteTAD_triggered()
{
    if (! myProject.writeTopographicDistanceMaps())
        myProject.logError();
}

void MainWindow::on_actionLoadTAD_triggered()
{
    if (! myProject.loadTopographicDistanceMaps())
        myProject.logError();
}

void MainWindow::on_meteoPoints_clicked()
{
    redrawMeteoPoints(currentPointsVisualization, true);
}

void MainWindow::on_grid_clicked()
{
    redrawMeteoGrid(currentGridVisualization);
}

void MainWindow::on_actionShowPointsHide_triggered()
{
    redrawMeteoPoints(notShown, true);
}

void MainWindow::on_actionShowPointsLocation_triggered()
{
    redrawMeteoPoints(showLocation, true);
}


void MainWindow::on_actionShowPointsCurrent_triggered()
{
    redrawMeteoPoints(showCurrentVariable, true);
}

void MainWindow::on_actionShowPointsElab_triggered()
{
    redrawMeteoPoints(showElaboration, true);
}

void MainWindow::on_actionShowPointsAnomalyAbs_triggered()
{
    redrawMeteoPoints(showAnomalyAbsolute, true);
}

void MainWindow::on_actionShowPointsAnomalyPerc_triggered()
{
    redrawMeteoPoints(showAnomalyPercentage, true);
}

void MainWindow::on_actionShowPointsClimate_triggered()
{
    redrawMeteoPoints(showClimate, true);
}

void MainWindow::on_actionShowGridHide_triggered()
{
    redrawMeteoGrid(notShown);
}

void MainWindow::on_actionShowGridLocation_triggered()
{
    redrawMeteoGrid(showLocation);
}

void MainWindow::on_actionShowGridCurrent_triggered()
{
    redrawMeteoGrid(showCurrentVariable);
}

void MainWindow::on_actionShowGridElab_triggered()
{
    redrawMeteoGrid(showElaboration);
}

void MainWindow::on_actionShowGridAnomalyAbs_triggered()
{
    redrawMeteoGrid(showAnomalyAbsolute);
}

void MainWindow::on_actionShowGridAnomalyPerc_triggered()
{
    redrawMeteoGrid(showAnomalyPercentage);
}

void MainWindow::on_actionShowGridClimate_triggered()
{
    redrawMeteoGrid(showClimate);
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

bool MainWindow::openRaster(QString fileName, gis::Crit3DRasterGrid *myRaster)
{

        std::string* myError = new std::string();
        std::string fnWithoutExt = fileName.left(fileName.length()-4).toStdString();

        if (! gis::readEsriGrid(fnWithoutExt, myRaster, myError))
        {
            qDebug("Load raster failed!");
            return (false);
        }
        return true;
}

bool MainWindow::openShape(QString fileName)
{
// TO DO
    return false;
}


bool MainWindow::on_actionAggregate_from_grid_triggered()
{
    if (!ui->grid->isChecked())
    {
        myProject.errorString = "Load grid";
        myProject.logError();
        return false;
    }
    if (myProject.aggregationDbHandler == nullptr)
    {
        QMessageBox::information(nullptr, "Missing DB", "Open or Create a Aggregation DB");
        return false;
    }

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open raster or Shape file"), "", tr("files (*.flt *.shp)"));
    if (fileName == "")
    {
        QMessageBox::information(nullptr, "No Raster or Shape", "Load raster/shape before");
        return false;
    }

    gis::Crit3DRasterGrid *myRaster = new(gis::Crit3DRasterGrid);
    // raster
    if (fileName.contains(".flt"))
    {
        openRaster(fileName, myRaster);
    }
    // shape
    else if (fileName.contains(".shp"))
    {
        // TO DO
        // sarà necessaria una finestra in cui è selezionabile il campo dello shape
        openShape(fileName);
    }

    DialogSeriesOnZones zoneDialog(myProject.pragaDefaultSettings);
    if (zoneDialog.result() != QDialog::Accepted)
    {
        delete myRaster;
        return false;
    }
    else
    {
        std::vector<float> outputValues;
        float threshold = NODATA;
        meteoComputation elab1MeteoComp = noMeteoComp;
        QString periodType = "D";
        if (!myProject.averageSeriesOnZonesMeteoGrid(zoneDialog.getVariable(), elab1MeteoComp, zoneDialog.getSpatialElaboration(), threshold, myRaster, zoneDialog.getStartDate(), zoneDialog.getEndDate(), periodType, outputValues, true))
        {
            QMessageBox::information(nullptr, "Error", "Error writing aggregation data");
            delete myRaster;
            return false;
        }
    }
    delete myRaster;
    return true;
}


void MainWindow::on_actionOpen_aggregation_DB_triggered()
{
    QString dbName = QFileDialog::getOpenFileName(this, tr("Open DB meteo points"), "", tr("DB files (*.db)"));
    if (dbName != "")
    {
        myProject.loadAggregationdDB(dbName);
    }

}

void MainWindow::on_actionNew_aggregation_DB_triggered()
{
    QString templateFileName = myProject.getDefaultPath() + PATH_TEMPLATE + "template_meteo_aggregation.db";

    QString dbName = QFileDialog::getSaveFileName(this, tr("Save as"), "", tr("DB files (*.db)"));
    if (dbName == "")
    {
        qDebug() << "missing new db file name";
        return;
    }

    QFile dbFile(dbName);
    if (dbFile.exists())
    {
        if (!dbFile.remove())
        {
            myProject.logError("Remove file failed: " + dbName + "\n" + dbFile.errorString());
            return;
        }
    }

    if (!QFile::copy(templateFileName, dbName))
    {
        myProject.logError("Copy file failed: " + templateFileName);
        return;
    }
    myProject.loadAggregationdDB(dbName);
}

void MainWindow::drawProject()
{
    mapView->centerOn(startCenter->lonLat());

    if (myProject.DEM.isLoaded)
        renderDEM();

    drawMeteoPoints();
    drawMeteoGrid();

    QString title = "PRAGA";
    if (myProject.projectName != "")
        title += " - " + myProject.projectName;

    this->setWindowTitle(title);
}

void MainWindow::on_actionOpen_project_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open project file"), "", tr("ini files (*.ini)"));
    if (fileName == "") return;

    if (myProject.isProjectLoaded)
    {
        on_actionClose_meteo_grid_triggered();
        on_actionClose_meteo_points_triggered();
        clearDEM();
    }

    if (myProject.loadPragaProject(fileName))
    {
        drawProject();
    }
    else
    {
        this->mapView->centerOn(startCenter->lonLat());
        if (myProject.loadPragaProject(myProject.getApplicationPath() + "default.ini")) drawProject();
    }

}

void MainWindow::on_actionClose_project_triggered()
{
    if (! myProject.isProjectLoaded) return;

    on_actionClose_meteo_grid_triggered();
    on_actionClose_meteo_points_triggered();

    clearDEM();

    this->mapView->centerOn(startCenter->lonLat());

    if (! myProject.loadPragaProject(myProject.getApplicationPath() + "default.ini")) return;

    drawProject();
}

void MainWindow::on_actionSave_project_as_triggered()
{
    QString projectFileName = QFileDialog::getSaveFileName(this, tr("Save project as"), "", tr("ini files (*.ini)"));
    if (projectFileName == "") return;

    bool isOk;
    QString projectName = QInputDialog::getText(this, tr("Project name"), tr("Enter project name"), QLineEdit::Normal, "", &isOk);

    QString parametersFileName = QFileDialog::getSaveFileName(this, tr("Save parameters as"), "", tr("ini files (*.ini)"));
    if (parametersFileName == "") return;

    myProject.createProjectSettings(projectName, projectFileName, parametersFileName);
}


bool KeyboardFilter::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type() == QEvent::KeyPress) {
        return true;
    } else {
        return QObject::eventFilter(obj, event);
    }
}

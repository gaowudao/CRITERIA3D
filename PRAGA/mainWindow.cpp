#include <QGridLayout>
#include <QFileDialog>
#include <QtDebug>
#include <QMessageBox>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QListWidget>
#include <QRadioButton>
#include <QTextBrowser>

#include <sstream>
#include <iostream>
#include <fstream>

#include "tileSources/OSMTileSource.h"
#include "tileSources/GridTileSource.h"
#include "tileSources/CompositeTileSource.h"

#include "mainWindow.h"
#include "ui_mainWindow.h"
#include "formInfo.h"
#include "dbMeteoPoints.h"
#include "dbArkimet.h"
#include "download.h"
#include "pragaProject.h"
#include "commonConstants.h"
#include "dialogWindows.h"
#include "pragaDialogs.h"
#include "computationDialog.h"
#include "climateFieldsDialog.h"
#include "interpolationDialog.h"
#include "pragaSettingsDialog.h"
#include "gis.h"
#include "spatialControl.h"

extern PragaProject myProject;

#define MAPBORDER 8
#define TOOLSWIDTH 260

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->showPoints = true;

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

    elabType1 = new QLineEdit;
    elabType2 = new QLineEdit;
    elabVariable = new QLineEdit;
    elabPeriod = new QLineEdit;

    QVBoxLayout *vbox = new QVBoxLayout(ui->groupBoxElaboration);
    QLabel *secondElab = new QLabel();
    secondElab->setText("<font color='black'>second Elab:</font>");
    secondElab->setBuddy(elabType2);
    QLabel *variable = new QLabel();
    variable->setText("<font color='black'>variable:</font>");
    variable->setBuddy(elabVariable);
    QLabel *period  = new QLabel();
    period->setText("<font color='black'>period:</font>");
    period->setBuddy(elabPeriod);
    vbox->addWidget(elabType1);
    vbox->addWidget(elabType2);
    vbox->addWidget(variable);
    vbox->addWidget(elabVariable);
    vbox->addWidget(period);
    vbox->addWidget(elabPeriod);
    ui->groupBoxElaboration->setLayout(vbox);

    ui->groupBoxElaboration->hide();

    ui->meteoPoints->setEnabled(false);
    ui->grid->setEnabled(false);
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
      mapView->resize(ui->widgetMap->size());
}


void MainWindow::mouseReleaseEvent(QMouseEvent *event){
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

    if (myRubberBand != nullptr)
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
    if (myRubberBand != nullptr)
    {
        delete myRubberBand;
        myRubberBand = nullptr;
    }

    if (ui->actionRectangle_Selection->isChecked())
    {
        myRubberBand = new RubberBand(QRubberBand::Rectangle, this->mapView);
        QPoint origin(int(this->mapView->width() * 0.5f) , int(this->mapView->height() * 0.5f));
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
    size = log2(1000.f/size);
    this->mapView->setZoomLevel(quint8(size));
    this->mapView->centerOn(qreal(center->longitude), qreal(center->latitude));

    // active raster object
    this->rasterObj->updateCenter();
}


void MainWindow::on_actionOpen_meteo_points_DB_triggered()
{
    QString dbName = QFileDialog::getOpenFileName(this, tr("Open DB meteo points"), "", tr("DB files (*.db)"));

    if (dbName != "")
    {
        this->loadMeteoPointsDB(dbName);
    }
    redrawMeteoPoints(true);
}


void MainWindow::on_actionOpen_meteo_grid_triggered()
{
    QString xmlName = QFileDialog::getOpenFileName(this, tr("Open XML DB meteo grid"), "", tr("xml files (*.xml)"));

    if (xmlName != "")
    {
        this->loadMeteoGridDB(xmlName);
    }
    redrawMeteoGrid();

}


void MainWindow::on_actionNewMeteoPointsArkimet_triggered()
{
    resetMeteoPoints();

    QString templateFileName = myProject.path + "DATA/template/template_meteo.db";

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


void MainWindow::on_actionVariableNone_triggered()
{
    //myProject.setFrequency(noFrequency);
    myProject.currentVariable = noMeteoVar;
    this->ui->actionVariableNone->setChecked(true);
    if (this->meteoGridObj != nullptr) this->meteoGridObj->setDrawBorders(true);
    this->updateVariable();
}


void MainWindow::on_actionDownload_meteo_data_triggered()
{
    if (downloadMeteoData(&myProject))
        this->loadMeteoPointsDB(myProject.meteoPointsDbHandler->getDbName());
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


void MainWindow::on_actionVariableChoose_triggered()
{
    if (chooseMeteoVariable(&myProject))
    {
       this->ui->actionVariableNone->setChecked(false);
       if (this->meteoGridObj != nullptr) this->meteoGridObj->setDrawBorders(false);
       this->updateVariable();
    }
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
        if (myProject.currentVariable != noMeteoVar)
        {
            this->ui->actionVariableNone->setChecked(false);
            if (this->meteoGridObj != nullptr) this->meteoGridObj->setDrawBorders(false);
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
    if (myProject.nrMeteoPoints == 0 || myProject.getIsElabMeteoPointsValue())
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
}

void MainWindow::redrawMeteoGrid()
{

    if (myProject.meteoGridDbHandler == nullptr)
        return;

    frequencyType frequency = myProject.getFrequency();
    meteoVariable variable = myProject.getCurrentVariable();

    if (myProject.getCurrentVariable() == noMeteoVar && !myProject.meteoGridDbHandler->meteoGrid()->getIsElabValue())
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


bool MainWindow::loadMeteoPointsDB(QString dbName)
{
    this->resetMeteoPoints();

    if (!myProject.loadMeteoPointsDB(dbName))
        return false;

    this->addMeteoPoints();

    if (myProject.meteoGridDbHandler == nullptr)
    {
        myProject.getLastMeteoData();
        this->updateDateTime();
    }
    myProject.loadMeteoPointsData (myProject.getCurrentDate(), myProject.getCurrentDate(), true);

    this->ui->meteoPoints->setEnabled(true);
    this->ui->meteoPoints->setChecked(true);
    this->ui->grid->setChecked(false);

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

    this->ui->meteoPoints->setChecked(false);
    this->ui->grid->setEnabled(true);
    this->ui->grid->setChecked(true);

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
    if (this->rasterObj->currentRaster == nullptr)
    {
        QMessageBox::information(nullptr, "No Raster", "Load raster before");
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
       this->ui->actionVariableNone->setChecked(false);
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

void MainWindow::on_actionPointsVisible_triggered()
{
    this->showPoints = ui->actionPointsVisible->isChecked();
    redrawMeteoPoints(false);
}

void MainWindow::on_rasterRestoreButton_clicked()
{
    if (this->rasterObj->currentRaster == nullptr)
    {
        QMessageBox::information(nullptr, "No Raster", "Load raster before");
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
    myProject.setIsElabMeteoPointsValue(false);
    ui->groupBoxElaboration->hide();

    this->ui->meteoPoints->setChecked(false);
    this->ui->meteoPoints->setEnabled(false);

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
        meteoGridObj->clean();
        meteoGridObj->redrawRequested();
        meteoGridLegend->setVisible(false);
        myProject.closeMeteoGridDB();
        ui->groupBoxElaboration->hide();
        ui->meteoGridOpacitySlider->setEnabled(false);

        this->ui->grid->setChecked(false);
        this->ui->grid->setEnabled(false);

        if (myProject.meteoPointsDbHandler != nullptr)
        {
            this->ui->meteoPoints->setChecked(true);
        }
    }

}

void MainWindow::on_actionInterpolation_to_DTM_triggered()
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
        ComputationDialog compDialog(myProject.settings, isAnomaly, saveClima);
        if (compDialog.result() != QDialog::Accepted)
            return;

        if (!myProject.elaboration(isMeteoGrid, isAnomaly, saveClima))
        {
            myProject.logError();
        }
        else
        {
            showElabResult(true, isMeteoGrid, isAnomaly);
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
        ComputationDialog compDialog(myProject.settings, isAnomaly, saveClima);
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
            showElabResult(true, isMeteoGrid, isAnomaly);
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
        ComputationDialog compDialog(myProject.settings, isAnomaly, saveClima);
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
    if (myProject.showClimateFields(isMeteoGrid, &climateDbElab, &climateDbVarList))
    {
        ClimateFieldsDialog climateDialog(climateDbElab, climateDbVarList);
        if (climateDialog.result() == QDialog::Accepted)
        {
            QString climaSelected = climateDialog.getSelected();

            if (climateDialog.getIsShowClicked())
            {
                meteoVariable variable = climateDialog.getVar();
                QString index = climateDialog.getIndexSelected();
                myProject.saveClimateResult(isMeteoGrid, climaSelected, index.toInt(), true);
                showClimateResult(true, isMeteoGrid, variable, climaSelected);
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

void MainWindow::showClimateResult(bool updateColorSCale, bool isMeteoGrid, meteoVariable variable, QString climaSelected)
{

    if (isMeteoGrid)
    {
        setColorScale(variable, myProject.meteoGridDbHandler->meteoGrid()->dataMeteoGrid.colorScale);
        ui->labelMeteoGridScale->setText(QString::fromStdString(getVariableString(variable)));
        meteoGridLegend->setVisible(true);
        meteoGridLegend->update();
    }
    else
    {
        if (!this->showPoints)
        {
            return;
        }

        meteoPointsLegend->setVisible(true);

        if (updateColorSCale)
        {
            float minimum = NODATA;
            float maximum = NODATA;
            for (int i = 0; i < myProject.nrMeteoPoints; i++)
            {
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
            setColorScale(variable, myProject.meteoPointsColorScale);
        }


        Crit3DColor *myColor;
        for (int i = 0; i < myProject.nrMeteoPoints; i++)
        {

            if (!updateColorSCale)
            {
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

    QStringList words = climaSelected.split('_');

    elabType1->setText("Climate - " + words[3]);
    elabVariable->setText(words[1]);
    elabPeriod->setText(words[2]);
    elabType1->setReadOnly(true);
    elabVariable->setReadOnly(true);
    elabPeriod->setReadOnly(true);
    ui->groupBoxElaboration->show();


}

void MainWindow::showElabResult(bool updateColorSCale, bool isMeteoGrid, bool isAnomaly)
{

    if (isMeteoGrid)
    {
        setColorScale(myProject.clima->variable(), myProject.meteoGridDbHandler->meteoGrid()->dataMeteoGrid.colorScale);
        ui->labelMeteoGridScale->setText(QString::fromStdString(getVariableString(myProject.clima->variable())));
        meteoGridLegend->setVisible(true);
        meteoGridLegend->update();
    }
    else
    {

        if (!this->showPoints)
        {
            return;
        }

        meteoPointsLegend->setVisible(true);

        if (updateColorSCale)
        {
            float minimum = NODATA;
            float maximum = NODATA;
            for (int i = 0; i < myProject.nrMeteoPoints; i++)
            {
                if (!isAnomaly)
                {
                    myProject.meteoPoints[i].currentValue = myProject.meteoPoints[i].elaboration;
                }
                else
                {
                    myProject.meteoPoints[i].currentValue = myProject.meteoPoints[i].anomaly;
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
                    myProject.meteoPoints[i].currentValue = myProject.meteoPoints[i].elaboration;
                }
                else
                {
                    myProject.meteoPoints[i].currentValue = myProject.meteoPoints[i].anomaly;
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
            elabType1->setText(myProject.clima->elab1() + " " + QString::number(myProject.clima->param1()));
        }
        else
        {
            elabType1->setText(myProject.clima->elab1() + " Anomaly of: " + QString::number(myProject.clima->param1()));
        }
    }
    else
    {
        if (!isAnomaly)
        {
            elabType1->setText(myProject.clima->elab1());
        }
        else
        {
            elabType1->setText("Anomaly respect to " + myProject.clima->elab1());
        }
    }
    if (myProject.clima->elab2().isEmpty())
    {
        elabType2->setVisible(false);
    }
    else
    {
        elabType2->setVisible(true);
        elabType2->setText(myProject.clima->elab2());
    }
    std::string var = MapDailyMeteoVarToString.at(myProject.clima->variable());
    elabVariable->setText(QString::fromStdString(var));
    QString startDay = QString::number(myProject.clima->genericPeriodDateStart().day());
    QString startMonth = QString::number(myProject.clima->genericPeriodDateStart().month());
    QString endDay = QString::number(myProject.clima->genericPeriodDateEnd().day());
    QString endMonth = QString::number(myProject.clima->genericPeriodDateEnd().month());

    QString startYear = QString::number(myProject.clima->yearStart());
    QString endYear = QString::number(myProject.clima->yearEnd());
    elabPeriod->setText(startDay + "/" + startMonth + "-" + endDay + "/" + endMonth + " " + startYear + "÷" + endYear);

    elabType1->setReadOnly(true);
    elabType2->setReadOnly(true);
    elabVariable->setReadOnly(true);
    elabPeriod->setReadOnly(true);
    ui->groupBoxElaboration->show();
}

void MainWindow::on_actionInterpolationSettings_triggered()
{
    InterpolationDialog* myInterpolationDialog = new InterpolationDialog(&myProject);
    myInterpolationDialog->close();
}


void MainWindow::on_actionParameters_triggered()
{
    PragaSettingsDialog* mySettingsDialog = new PragaSettingsDialog(myProject.pathSetting, myProject.settings, &myProject.gisSettings, myProject.quality, myProject.meteoSettings, myProject.clima->getElabSettings());
    mySettingsDialog->exec();
    if (startCenter->latitude() != myProject.gisSettings.startLocation.latitude || startCenter->longitude() != myProject.gisSettings.startLocation.longitude)
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



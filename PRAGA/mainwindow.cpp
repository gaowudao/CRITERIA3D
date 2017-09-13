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

#include "tileSources/OSMTileSource.h"
#include "tileSources/GridTileSource.h"
#include "tileSources/CompositeTileSource.h"
#include "guts/CompositeTileSourceConfigurationWidget.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "formInfo.h"
#include "Position.h"
#include "dbMeteoPoints.h"
#include "dbArkimet.h"
#include "download.h"
#include "project.h"
#include "utilities.h"
#include "commonConstants.h"
#include "dialogWindows.h"
#include "quality.h"
#include "interpolation.h"
#include "gis.h"


extern Project myProject;
#define MAPBORDER 8


MainWindow::MainWindow(environment menu, QWidget *parent) :
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

    this->pointsLegend = new ColorLegend(this->ui->widgetColorLegendPoints);
    this->pointsLegend->resize(this->ui->widgetColorLegendPoints->size());
    this->pointsLegend->colorScale = myProject.colorScalePoints;

    // Set tiles source
    this->setMapSource(OSMTileSource::OSMTiles);

    // Set start size and position
    Position* startCenter = new Position (11.35, 44.5, 0.0);
    this->mapView->setZoomLevel(8);
    this->mapView->centerOn(startCenter->lonLat());
    this->setMapSource(OSMTileSource::Terrain);

    // Set raster objects
    this->rasterObj = new RasterObject(this->mapView);
    this->gridObj = new RasterObject(this->mapView);

    this->rasterObj->setOpacity(this->ui->rasterOpacitySlider->value() / 100.0);
    this->gridObj->setOpacity(this->ui->rasterOpacitySlider->value() / 100.0);

    this->rasterObj->setColorLegend(this->rasterLegend);
    // gridobj colorLegend

    this->mapView->scene()->addObject(this->rasterObj);
    this->mapView->scene()->addObject(this->gridObj);

    this->updateVariable();
    this->updateDateTime();

    this->setMouseTracking(true);

    this->menu = menu;

    //set menu
    switch(this->menu)
    {
        case praga :
            ui->actionDownload_meteo_data->setVisible(true);
            ui->actionNewMeteoPointsArkimet->setVisible(true);
            break;
        case criteria1D:
            ui->actionDownload_meteo_data->setVisible(false);
            ui->actionNewMeteoPointsArkimet->setVisible(false);
            break;
        case criteria3D :
            ui->actionDownload_meteo_data->setVisible(false);
            ui->actionNewMeteoPointsArkimet->setVisible(false);
            break;
    }
}


MainWindow::~MainWindow()
{
    delete rasterObj;
    delete gridObj;
    delete rasterLegend;
    delete pointsLegend;
    delete mapView;
    delete mapScene;
    delete ui;
}


void MainWindow::on_rasterOpacitySlider_sliderMoved(int position)
{
    this->rasterObj->setOpacity(position / 100.0);
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


void MainWindow::on_actionSetUTMzone_triggered()
{
    QString currentUTMZone = QString::number(myProject.gisSettings.utmZone);
    int newUTMZone;
    bool isOk = false;
    while (! isOk)
    {
        QString retValue = editValue("UTM Zone number", currentUTMZone);
        if (retValue == "")
            return;

        newUTMZone = retValue.toInt(&isOk);
        if (! isOk)
            QMessageBox::information(NULL, "Wrong value", "Insert a valid UTM zone number");
    }

    myProject.gisSettings.utmZone = newUTMZone;
}


void MainWindow::on_actionRectangle_Selection_triggered()
{
    if (myRubberBand != NULL)
        delete myRubberBand;

    myRubberBand = new RubberBand(QRubberBand::Rectangle, this->mapView);
    QPoint origin(this->mapView->width()*0.5 , this->mapView->height()*0.5);
    QPoint mapPoint = getMapPoint(&origin);
    myRubberBand->setOrigin(mapPoint);
    myRubberBand->setGeometry(QRect(myRubberBand->getOrigin(), QSize()));
    //myRubberBand->show();
}


void MainWindow::on_actionLoadRaster_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open raster Grid"), "", tr("ESRI grid files (*.flt)"));

    if (fileName == "") return;

    qDebug() << "loading raster";
    if (!myProject.loadRaster(fileName)) return;

    // set DTM color scale
    setColorScale(noMeteoTerrain, myProject.DTM.colorScale);
    ui->labelRasterScale->setText(QString::fromStdString(getVariableString(noMeteoTerrain)));
    this->rasterLegend->colorScale = myProject.currentRaster->colorScale;

    this->ui->rasterOpacitySlider->setEnabled(true);

    // set raster object
    this->rasterObj->initializeUTM(&(myProject.DTM), myProject.gisSettings, false);

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


void MainWindow::on_actionNewMeteoPointsArkimet_triggered()
{
    resetMeteoPoints();

    QString templateName = QFileDialog::getOpenFileName(this, tr("Choose template DB meteo"), "", tr("DB files (*.db)"));
    if (templateName == "")
    {
        qDebug() << "missing template";
        return;
    }
    else
    {
        QString dbName = QFileDialog::getSaveFileName(this, tr("Save as"), "", tr("DB files (*.db)"));
        if (dbName == "")
        {
            qDebug() << "missing new db file name";
            return;
        }
        else
        {
            QFile::remove(dbName);
            QFile::copy(templateName, dbName);

            Download * myDownload = new Download(dbName);
            DbArkimet* myDbArkimet = myDownload->getDbArkimet();

            QStringList dataset = myDbArkimet->getDatasetsList();

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

            QCheckBox* all = new QCheckBox("ALL");
            layout.addSpacing(30);
            layout.addWidget(all);

            connect(all, SIGNAL(toggled(bool)), this, SLOT(enableAllDataset(bool)));

            QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                                 | QDialogButtonBox::Cancel);

            connect(buttonBox, SIGNAL(accepted()), &datasetDialog, SLOT(accept()));
            connect(buttonBox, SIGNAL(rejected()), &datasetDialog, SLOT(reject()));

            layout.addWidget(buttonBox);
            datasetDialog.setLayout(&layout);

            QString datasetSelected = selectArkimetDataset(&datasetDialog);

            if (!datasetSelected.isEmpty())
            {
                myDbArkimet->setDatasetsActive(datasetSelected);
                QStringList datasets = datasetSelected.remove("'").split(",");

                formInfo myInfo;
                myInfo.start("download points properties...", 0);
                    if (myDownload->getPointProperties(datasets))
                    {
                        myProject.loadMeteoPointsDB(dbName);
                        this->addMeteoPoints();
                    }
                    else
                    {
                        QMessageBox::information(NULL, "Network Error!", "Error in function getPointProperties");
                    }

                myInfo.close();
            }
            else
            {
                QFile::remove(dbName);
                delete myDbArkimet;
            }

            delete buttonBox;
            delete all;
        }
    }
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

    foreach (QCheckBox *checkBox, datasetCheckbox)
    {
        checkBox->setChecked(toggled);
    }

}


void MainWindow::on_actionOpen_meteo_points_DB_triggered()
{
    QString dbName = QFileDialog::getOpenFileName(this, tr("Open DB meteo"), "", tr("DB files (*.db)"));

    if (dbName != "") this->loadMeteoPointsDB(dbName);
}


void MainWindow::on_actionVariableNone_triggered()
{
    //myProject.setFrequency(noFrequency);
    myProject.currentVariable = noMeteoVar;
    this->ui->actionVariableNone->setChecked(true);

    this->updateVariable();
}


void MainWindow::on_actionDownload_meteo_data_triggered()
{
    if (downloadMeteoData())
        this->loadMeteoPointsDB(myProject.dbMeteoPoints->getDbName());
}


void MainWindow::mouseReleaseEvent(QMouseEvent *event){
    Q_UNUSED(event)

    this->rasterObj->updateCenter();
    this->gridObj->updateCenter();

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
    this->gridObj->updateCenter();
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

        if (myProject.netCDF.isLoaded)
        {
            QPoint pos = event->pos();
            Position myPos = mapView->mapToScene(getMapPoint(&pos));
            gis::Crit3DGeoPoint geoPoint = gis::Crit3DGeoPoint(myPos.latitude(), myPos.longitude());

            exportNetCDFDataSeries(geoPoint);
        }
    }

}

void MainWindow::resizeEvent(QResizeEvent * event)
{
    Q_UNUSED(event)
    const int INFOHEIGHT = 40;
    const int TOOLSWIDTH = 260;

    ui->widgetMap->setGeometry(TOOLSWIDTH, 0, this->width()-TOOLSWIDTH, this->height() - INFOHEIGHT);
    mapView->resize(ui->widgetMap->size());

    ui->groupBoxVariable->move(MAPBORDER/2, (this->height() - INFOHEIGHT) / 2 - ui->groupBoxVariable->height() - MAPBORDER);
    ui->groupBoxVariable->resize(TOOLSWIDTH, ui->groupBoxVariable->height());

    ui->groupBoxRaster->move(MAPBORDER/2, (this->height() - INFOHEIGHT) / 2 + MAPBORDER);
    ui->groupBoxRaster->resize(TOOLSWIDTH, ui->groupBoxRaster->height());

    //TODO sembrano non funzionare
    ui->widgetColorLegendRaster->resize(TOOLSWIDTH, ui->widgetColorLegendPoints->height());
    ui->widgetColorLegendPoints->resize(TOOLSWIDTH, ui->widgetColorLegendPoints->height());
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

    this->myRubberBand = NULL;
}


void MainWindow::on_actionVariableChoose_triggered()
{
    if (chooseMeteoVariable())
    {
       this->ui->actionVariableNone->setChecked(false);
       this->updateVariable();
    } 
}


void MainWindow::on_actionVariableQualitySpatial_triggered()
{
    myProject.quality->setSpatialControl(ui->actionVariableQualitySpatial->isChecked());

    updateVariable();
}


void MainWindow::on_actionInterpolation_triggered()
{
    myProject.quality->checkData(myProject.getCurrentVariable(), myProject.getFrequency(),
                                 myProject.meteoPoints, myProject.nrMeteoPoints, myProject.getCurrentTime());

    std::string myError;
    if (! checkInterpolationRaster(myProject.DTM, &myError))
    {
        QMessageBox::information(NULL, "Error!", QString::fromStdString(myError));
        return;
    }

    Crit3DInterpolationSettings interpolationSettings;
    formInfo myInfo;
    myInfo.start("Interpolation...", 0);

        myProject.dataRaster.initializeGrid(myProject.DTM);

        if (interpolationRaster(myProject.getCurrentVariable(), &interpolationSettings,
                    &(myProject.dataRaster), myProject.DTM, myProject.getCurrentTime(), &myError))
        {
            myProject.currentRaster = &(myProject.dataRaster);
            rasterObj->setCurrentRaster(&(myProject.dataRaster));
            setColorScale(myProject.getCurrentVariable(), myProject.currentRaster->colorScale);
            QString myString = QString::fromStdString(getVariableString(myProject.getCurrentVariable()));
            ui->labelRasterScale->setText(myString);
            this->rasterLegend->colorScale = myProject.currentRaster->colorScale;
        }
        else
            QMessageBox::information(NULL, "Error!", QString::fromStdString(myError));

    myInfo.close();
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
            this->ui->actionVariableNone->setChecked(false);

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

            else if (myProject.currentVariable == airHumidity)
                myProject.currentVariable = dailyAirHumidityAvg;
        }

        else if (myProject.getFrequency() == hourly)
        {
            this->ui->labelFrequency->setText("Hourly");

            //check
            if ((myProject.currentVariable == dailyAirTemperatureAvg)
                    || (myProject.currentVariable == dailyAirTemperatureMax)
                    || (myProject.currentVariable == dailyAirTemperatureMin))
                myProject.currentVariable = airTemperature;

            else if ((myProject.currentVariable == dailyAirHumidityAvg)
                     || (myProject.currentVariable == dailyAirHumidityMax)
                     || (myProject.currentVariable == dailyAirHumidityMin))
                 myProject.currentVariable = airHumidity;

            else if (myProject.currentVariable == dailyPrecipitation)
                    myProject.currentVariable = precipitation;

            else if (myProject.currentVariable == dailyGlobalRadiation)
                myProject.currentVariable = globalIrradiance;
        }
    }

    std::string myString = getVariableString(myProject.currentVariable);
    ui->labelVariable->setText(QString::fromStdString(myString));

    redrawMeteoPoints();
}


void MainWindow::updateDateTime()
{
    this->ui->dateTimeEdit->setDate(myProject.getCurrentDate());
    this->ui->dateTimeEdit->setTime(QTime(myProject.getCurrentHour(),0,0,0));
}


void MainWindow::on_dateTimeEdit_dateTimeChanged(const QDateTime &dateTime)
{
    //date
    if (dateTime.date() != myProject.getCurrentDate())
    {
        myProject.setCurrentDate(dateTime.date());
        myProject.loadMeteoPointsData(dateTime.date(), dateTime.date(), true);
    }

    //hour
    if (dateTime.time().hour() != myProject.getCurrentHour())
    {
        myProject.setCurrentHour(dateTime.time().hour());
    }

    redrawMeteoPoints();
}



void MainWindow::redrawMeteoPoints()
{
    if (myProject.nrMeteoPoints == 0)
        return;

    // hide all meteo points
    for (int i = 0; i < myProject.nrMeteoPoints; i++)
        pointList[i]->setVisible(false);

    if (! this->showPoints)
        return;

    // show location
    if (myProject.getCurrentVariable() == noMeteoVar)
    {
        for (int i = 0; i < myProject.nrMeteoPoints; i++)
        {
                myProject.meteoPoints[i].value = NODATA;
                pointList[i]->setFillColor(QColor(Qt::white));
                pointList[i]->setRadius(5);
                pointList[i]->setToolTip(i);
                pointList[i]->setVisible(true);
        }

        myProject.colorScalePoints->setRange(NODATA, NODATA);
        pointsLegend->update();
        return;
    }

    // quality control
    myProject.quality->checkData(myProject.getCurrentVariable(), myProject.getFrequency(),
                                 myProject.meteoPoints, myProject.nrMeteoPoints, myProject.getCurrentTime());

    float minimum, maximum;
    myProject.getMeteoPointsRange(&minimum, &maximum);

    myProject.colorScalePoints->setRange(minimum, maximum);
    roundColorScale(myProject.colorScalePoints, 4, true);

    setColorScale(myProject.currentVariable, myProject.colorScalePoints);

    Crit3DColor *myColor;
    for (int i = 0; i < myProject.nrMeteoPoints; i++)
    {
        if (myProject.meteoPoints[i].value != NODATA)
        {
            if (myProject.meteoPoints[i].myQuality == quality::accepted)
            {
                pointList[i]->setRadius(5);
                myColor = myProject.colorScalePoints->getColor(myProject.meteoPoints[i].value);
                pointList[i]->setFillColor(QColor(myColor->red, myColor->green, myColor->blue));
            }
            else
            {
                // Wrong data
                pointList[i]->setRadius(15);
                pointList[i]->setFillColor(QColor(Qt::black));
            }

            pointList[i]->setToolTip(i);
            pointList[i]->setVisible(true);
        }
    }

    pointsLegend->update();
}


bool MainWindow::loadMeteoPointsDB(QString dbName)
{
    this->resetMeteoPoints();

    if (!myProject.loadMeteoPointsDB(dbName))
        return false;

    this->addMeteoPoints();

    if (! myProject.loadlastMeteoData()) qDebug ("NO data");

    this->updateDateTime();

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

        point->setToolTip(i);
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
    if (!myProject.DTM.isLoaded)
    {
        QMessageBox::information(NULL, "No Raster", "Load raster before");
        return;
    }

    meteoVariable myVar = chooseColorScale();
    if (myVar != noMeteoVar)
    {
        setColorScale(myVar, myProject.currentRaster->colorScale);
        ui->labelRasterScale->setText(QString::fromStdString(getVariableString(myVar)));
    }
}


void MainWindow::on_variableButton_clicked()
{
    if (chooseMeteoVariable())
    {
       this->ui->actionVariableNone->setChecked(false);
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


void MainWindow::on_actionPointsVisible_triggered()
{
    this->showPoints = ui->actionPointsVisible->isChecked();
    redrawMeteoPoints();
}


void MainWindow::on_action_Open_NetCDF_data_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open NetCDF data"), "", tr("NetCDF files (*.nc)"));

    if (fileName == "") return;

    myProject.netCDF.initialize(myProject.gisSettings.utmZone);

    std::stringstream buffer;
    myProject.netCDF.readProperties(fileName.toStdString(), &buffer);

    if (myProject.netCDF.isLatLon)
        gridObj->initializeLatLon(&(myProject.netCDF.dataGrid), myProject.gisSettings, myProject.netCDF.latLonHeader, true);
    else
        gridObj->initializeUTM(&(myProject.netCDF.dataGrid), myProject.gisSettings, true);

    myProject.netCDF.dataGrid.emptyGrid();
    gridObj->updateCenter();

    /*QDialog myDialog;
    QVBoxLayout mainLayout;

    myDialog.setWindowTitle("NetCDF file info  ");

    QTextBrowser textBrowser;
    textBrowser.setText(QString::fromStdString(buffer.str()));
    buffer.clear();

    mainLayout.addWidget(&textBrowser);

    myDialog.setLayout(&mainLayout);
    myDialog.setFixedSize(800,600);
    myDialog.exec();
    */
}


void MainWindow::on_action_Extract_NetCDF_series_triggered()
{
    int idVar;
    QDateTime firstDate, lastDate;

    if (chooseNetCDFVariable(&idVar, &firstDate, &lastDate))
    {
        QMessageBox::information(NULL, "Variable",
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
                QMessageBox::information(NULL, "ERROR", QString::fromStdString(buffer.str()));
            else
                QMessageBox::information(NULL, "Result", QString::fromStdString(buffer.str()));
        }
    }
}

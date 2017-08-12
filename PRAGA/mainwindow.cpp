#include <QFileDialog>
#include <QtDebug>
#include <QCheckBox>
#include <QMessageBox>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QListWidget>
#include <QRadioButton>

#include "tileSources/OSMTileSource.h"
#include "tileSources/GridTileSource.h"
#include "tileSources/CompositeTileSource.h"
#include "guts/CompositeTileSourceConfigurationWidget.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "formInfo.h"
#include "Position.h"
#include "formSingleValue.h"
#include "dbMeteoPoints.h"
#include "dbArkimet.h"
#include "download.h"
#include "project.h"
#include "utilities.h"
#include "commonConstants.h"


extern Project myProject;
#define MAPBORDER 8

MainWindow::MainWindow(environment menu, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->myRubberBand = NULL;

    // Set the MapGraphics Scene and View
    this->mapScene = new MapGraphicsScene(this);
    this->mapView = new MapGraphicsView(mapScene, this->ui->widgetMap);

    this->rasterLegend = new ColorLegend(this->ui->widgetColorLegendRaster);
    this->rasterLegend->resize(this->ui->widgetColorLegendRaster->size());
    this->rasterLegend->colorScale = myProject.DTM.colorScale;

    this->pointsLegend = new ColorLegend(this->ui->widgetColorLegendPoints);
    this->pointsLegend->resize(this->ui->widgetColorLegendPoints->size());
    this->pointsLegend->colorScale = myProject.colorScalePoints;

    // Set tiles source
    this->setMapSource(OSMTileSource::OSMTiles);

    // Set start size and position
    Position* startCenter = new Position (11.35, 44.5, 0.0);
    this->mapView->setZoomLevel(10);
    this->mapView->centerOn(startCenter->lonLat());

    // Set raster object
    this->rasterObj = new RasterObject(this->mapView);
    this->rasterObj->setOpacity(this->ui->rasterOpacitySlider->value() / 100.0);
    this->rasterObj->setColorLegend(this->rasterLegend);
    this->mapView->scene()->addObject(this->rasterObj);

    this->updateVariable();
    this->updateDateTime();

    this->setMouseTracking(true);

    this->menu = menu;

    //set menu
    switch(this->menu)
    {
        case praga :
            ui->actionDownload_meteo_data->setVisible(true);
            ui->actionMeteoPointsArkimet->setVisible(true);
            break;
        case criteria1D:
            ui->actionDownload_meteo_data->setVisible(false);
            ui->actionMeteoPointsArkimet->setVisible(false);
            break;
        case criteria3D :
            ui->actionDownload_meteo_data->setVisible(false);
            ui->actionMeteoPointsArkimet->setVisible(false);
            break;
    }
}


MainWindow::~MainWindow()
{
    delete rasterObj;
    delete rasterLegend;
    delete pointsLegend;
    delete mapView;
    delete mapScene;
    delete ui;
}


void MainWindow::on_rasterOpacitySlider_sliderMoved(int position)
{
    if (this->rasterObj != NULL)
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
    FormSingleValue myForm;
    myForm.setValue(QString::number(myProject.gisSettings.utmZone));
    myForm.setModal(true);
    myForm.show();

    int utmZone;
    bool isOk = false;
    while (! isOk)
    {
        int myReturn = myForm.exec();
        if (myReturn == QDialog::Rejected) return;

        utmZone = myForm.getValue().toInt(&isOk);
        if (! isOk) qDebug("Wrong value!");
    }

    myProject.gisSettings.utmZone = utmZone;
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

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open rasterObj"), "", tr("ESRI grid files (*.flt)"));
    if (fileName == "") return;

    qDebug() << "loading raster";
    if (!myProject.loadRaster(fileName)) return;

    // set default color scale
    setRasterColorScale(noMeteoVar, myProject.DTM.colorScale, ui->labelRasterScale);

    this->ui->rasterOpacitySlider->setEnabled(true);

    // center map
    gis::Crit3DGeoPoint* center = gis::getRasterGeoCenter(myProject.rowMatrix.header);
    this->mapView->centerOn(qreal(center->longitude), qreal(center->latitude));

    // resize map
    float size = gis::getRasterMaxSize(myProject.rowMatrix.header);
    size = log2(1000.0/size);
    this->mapView->setZoomLevel(quint8(size));
    this->mapView->centerOn(qreal(center->longitude), qreal(center->latitude));

    // active raster object
    this->rasterObj->updateCenter();
    this->rasterObj->setDrawing(true);
}


void MainWindow::on_actionMeteoPointsArkimet_triggered()
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

                QApplication::setOverrideCursor(Qt::WaitCursor);

                if (myDownload->getPointProperties(datasets))
                {
                    QApplication::restoreOverrideCursor();

                    myProject.loadMeteoPointsDB(dbName, true);

                    this->addMeteoPoints();
                }
                else
                {
                    QApplication::restoreOverrideCursor();

                    QMessageBox *msgBox = new QMessageBox(this);
                    msgBox->setText("Network Error");
                    msgBox->exec();
                    delete msgBox;
                }
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
    myProject.setFrequency(noFrequency);
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

    if (this->rasterObj != NULL)
        this->rasterObj->updateCenter();

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

    this->rasterObj->setDrawing(false);
        if (event->button() == Qt::LeftButton)
            this->mapView->zoomIn();
        else
            this->mapView->zoomOut();

    this->mapView->centerOn(newCenter.lonLat());
    this->rasterObj->updateCenter();
    this->rasterObj->setDrawing(true);
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
    }

}

void MainWindow::resizeEvent(QResizeEvent * event)
{
    Q_UNUSED(event)
    const int INFOHEIGHT = 40;
    const int TOOLSWIDTH = 250;

    ui->widgetMap->setGeometry(TOOLSWIDTH, 0, this->width()-TOOLSWIDTH, this->height() -INFOHEIGHT + MAPBORDER);
    mapView->resize(ui->widgetMap->size());

    ui->groupBoxRaster->move(MAPBORDER/2, this->height() - ui->groupBoxRaster->height() -INFOHEIGHT);
    ui->groupBoxRaster->resize(TOOLSWIDTH, ui->groupBoxRaster->height());

    ui->groupBoxVariable->move(MAPBORDER/2, ui->groupBoxRaster->y() - ui->groupBoxVariable->height() - 20);
    ui->groupBoxVariable->resize(TOOLSWIDTH, ui->groupBoxVariable->height());

    //TODO sembrano non funzionare
    ui->widgetColorLegendRaster->resize(TOOLSWIDTH, ui->widgetColorLegendPoints->height());
    ui->widgetColorLegendPoints->resize(TOOLSWIDTH, ui->widgetColorLegendPoints->height());
}


QPoint MainWindow::getMapPoint(QPoint* point) const
{
    QPoint mapPoint;
    int dx, dy;
    dx = this->ui->widgetMap->x() + MAPBORDER;
    dy = this->ui->widgetMap->y() + this->ui->menuBar->height() + MAPBORDER ;
    mapPoint.setX(point->x() - dx);
    mapPoint.setY(point->y() - dy);
    return mapPoint;
}


void MainWindow::resetMeteoPoints()
{
    datasetCheckbox.clear();

    for (int i = 0; i < this->pointList.size(); i++)
    {
        this->mapView->scene()->removeObject(this->pointList[i]);
    }
    qDeleteAll(this->pointList.begin(), this->pointList.end());
    this->pointList.clear();

    this->myRubberBand = NULL;
}


void MainWindow::on_actionVariableChoose_triggered()
{
    if (chooseVariable())
    {
       this->ui->actionVariableNone->setChecked(false);
       this->updateVariable();
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

    //VARIABLE
    if ((myProject.currentVariable == airTemperature)
            || (myProject.currentVariable == dailyAirTemperatureAvg))
        this->ui->labelVariable->setText("Avg. air temperature °C");
    else if ((myProject.currentVariable == airHumidity)
            || (myProject.currentVariable == dailyAirHumidityAvg))
        this->ui->labelVariable->setText("Avg. relative humidity %");
    else if ((myProject.currentVariable == dailyPrecipitation)
            ||  (myProject.currentVariable == precipitation))
        this->ui->labelVariable->setText("Precipitation mm");
    else if (myProject.currentVariable == dailyAirTemperatureMax)
        this->ui->labelVariable->setText("Max. air temperature °C");
    else if (myProject.currentVariable == dailyAirTemperatureMin)
        this->ui->labelVariable->setText("Min. air temperature °C");
    else if (myProject.currentVariable == dailyGlobalRadiation)
        this->ui->labelVariable->setText("Solar radiation MJ m-2");
    else if (myProject.currentVariable == dailyAirHumidityMax)
        this->ui->labelVariable->setText("Max. relative humidity %");
    else if (myProject.currentVariable == dailyAirHumidityMin)
        this->ui->labelVariable->setText("Min. relative humidity %");
    else if (myProject.currentVariable == globalIrradiance)
        this->ui->labelVariable->setText("Solar irradiance W m-2");

    else
        this->ui->labelVariable->setText("None");

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
    if (myProject.meteoPoints.size() == 0) return;

    float v, minimum, maximum;
    Crit3DColor *myColor;

    myProject.getMeteoPointsRange(&minimum, &maximum);

    myProject.colorScalePoints->setRange(minimum, maximum);
    roundColorScale(myProject.colorScalePoints, 4, true);

    setColorScale(myProject.colorScalePoints, myProject.currentVariable);

    for (int i = 0; i < myProject.meteoPoints.size(); i++)
    {
        if (myProject.getFrequency() == noFrequency || myProject.currentVariable == noMeteoVar)
        {
            pointList[i]->setVisible(true);
            pointList[i]->setFillColor(QColor(Qt::white));
            myProject.meteoPoints[i].value = NODATA;
            pointList[i]->setToolTip(i);
        }
        else
        {
            if (myProject.getFrequency() == daily)
                v =  myProject.meteoPoints[i].getMeteoPointValueD(getCrit3DDate(myProject.getCurrentDate()), myProject.currentVariable);
            else if (myProject.getFrequency() == hourly)
                v =  myProject.meteoPoints[i].getMeteoPointValueH(getCrit3DDate(myProject.getCurrentDate()), myProject.getCurrentHour(), 0, myProject.currentVariable);

            if (v == NODATA)
            {
                pointList[i]->setVisible(false);
            }
            else
            {
                pointList[i]->setVisible(true);
                myColor = myProject.colorScalePoints->getColor(v);
                pointList[i]->setFillColor(QColor(myColor->red, myColor->green, myColor->blue));  
                myProject.meteoPoints[i].value = v;
                pointList[i]->setToolTip(i);
            }
        }
    }

    pointsLegend->update();
}



bool MainWindow::loadMeteoPointsDB(QString dbName)
{
    this->resetMeteoPoints();

    if (!myProject.loadMeteoPointsDB(dbName, true))
        return false;

    this->addMeteoPoints();

    if (! myProject.loadlastMeteoData()) qDebug ("NO data");

    this->updateDateTime();

    return true;
}


void MainWindow::addMeteoPoints()
{
    myProject.meteoPointsSelected.clear();
    for (int i = 0; i < myProject.meteoPoints.size(); i++)
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

    chooseRasterColorScale(myProject.DTM.colorScale, ui->labelRasterScale);
}


void MainWindow::on_variableButton_clicked()
{
    if (chooseVariable())
    {
       this->ui->actionVariableNone->setChecked(false);
       this->updateVariable();
    }
}


void MainWindow::on_frequencyButton_clicked()
{
   if (chooseFrequency())
       this->updateVariable();
}


bool chooseVariable()
{
    if (myProject.getFrequency() == noFrequency)
    {
        QMessageBox::information(NULL, "No frequency", "Choose frequency before");
        return false;
    }

    QDialog myDialog;
    QVBoxLayout mainLayout;
    QVBoxLayout layoutVariable;
    QHBoxLayout layoutOk;

    myDialog.setWindowTitle("Choose variable");
    myDialog.setFixedWidth(300);

    QRadioButton Tavg("Average temperature °C");
    QRadioButton Tmin("Minimum temperature °C");
    QRadioButton Tmax("Maximum temperature °C");
    QRadioButton Prec("Precipitation mm");
    QRadioButton RHavg("Average relative humidity %");
    QRadioButton RHmin("Minimum relative humidity %");
    QRadioButton RHmax("Maximum relative humidity %");
    QRadioButton Rad("Solar radiation MJ m-2");

    if (myProject.getFrequency() == daily)
    {
        layoutVariable.addWidget(&Tmin);
        layoutVariable.addWidget(&Tavg);
        layoutVariable.addWidget(&Tmax);
        layoutVariable.addWidget(&Prec);
        layoutVariable.addWidget(&RHmin);
        layoutVariable.addWidget(&RHavg);
        layoutVariable.addWidget(&RHmax);
        layoutVariable.addWidget(&Rad);
    }
    else if (myProject.getFrequency() == hourly)
    {
        Tavg.setText("Average temperature °C");
        Prec.setText("Precipitation mm");
        RHavg.setText("Average relative humidity %");
        Rad.setText("Solar irradiance W m-2");

        layoutVariable.addWidget(&Tavg);
        layoutVariable.addWidget(&Prec);
        layoutVariable.addWidget(&RHavg);
        layoutVariable.addWidget(&Rad);
    }
    else return false;

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    myDialog.connect(&buttonBox, SIGNAL(accepted()), &myDialog, SLOT(accept()));
    myDialog.connect(&buttonBox, SIGNAL(rejected()), &myDialog, SLOT(reject()));

    layoutOk.addWidget(&buttonBox);

    mainLayout.addLayout(&layoutVariable);
    mainLayout.addLayout(&layoutOk);
    myDialog.setLayout(&mainLayout);
    myDialog.exec();

    if (myDialog.result() != QDialog::Accepted)
        return false;

   if (myProject.getFrequency() == daily)
   {
       if (Tmin.isChecked())
           myProject.currentVariable = dailyAirTemperatureMin;
       else if (Tmax.isChecked())
           myProject.currentVariable = dailyAirTemperatureMax;
       else if (Tavg.isChecked())
           myProject.currentVariable = dailyAirTemperatureAvg;
       else if (Prec.isChecked())
           myProject.currentVariable = dailyPrecipitation;
       else if (Rad.isChecked())
           myProject.currentVariable = dailyGlobalRadiation;
       else if (RHmin.isChecked())
           myProject.currentVariable = dailyAirHumidityMin;
       else if (RHmax.isChecked())
           myProject.currentVariable = dailyAirHumidityMax;
       else if (RHavg.isChecked())
           myProject.currentVariable = dailyAirHumidityAvg;
   }
   else if (myProject.getFrequency() == hourly)
   {
       if (Tavg.isChecked())
           myProject.currentVariable = airTemperature;
       else if (RHavg.isChecked())
           myProject.currentVariable = airHumidity;
       else if (Prec.isChecked())
           myProject.currentVariable = precipitation;
       else if (Rad.isChecked())
           myProject.currentVariable = globalIrradiance;
   }
   else
       return false;

   return true;
}


bool chooseFrequency()
{
    QDialog myDialog;
    QVBoxLayout mainLayout;
    QVBoxLayout layoutFrequency;
    QHBoxLayout layoutOk;

    myDialog.setWindowTitle("Choose frequency");
    myDialog.setFixedWidth(300);

    QRadioButton Daily("Daily");
    QRadioButton Hourly("Hourly");

    layoutFrequency.addWidget(&Daily);
    layoutFrequency.addWidget(&Hourly);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    myDialog.connect(&buttonBox, SIGNAL(accepted()), &myDialog, SLOT(accept()));
    myDialog.connect(&buttonBox, SIGNAL(rejected()), &myDialog, SLOT(reject()));

    layoutOk.addWidget(&buttonBox);

    mainLayout.addLayout(&layoutFrequency);
    mainLayout.addLayout(&layoutOk);
    myDialog.setLayout(&mainLayout);
    myDialog.exec();

    if (myDialog.result() != QDialog::Accepted)
        return false;

   if (Daily.isChecked())
       myProject.setFrequency(daily);
   else if (Hourly.isChecked())
       myProject.setFrequency(hourly);

   return true;
}


bool downloadMeteoData()
{
    if(myProject.meteoPoints.isEmpty())
    {
         QMessageBox::information(NULL, "DB not existing", "Create or Open a meteo points database before download");
         return false;
    }

    QDialog downloadDialog;
    QVBoxLayout mainLayout;
    QHBoxLayout timeVarLayout;
    QHBoxLayout dateLayout;
    QHBoxLayout buttonLayout;

    downloadDialog.setWindowTitle("Download Data");

    QCheckBox hourly("Hourly");
    QCheckBox daily("Daily");

    QListWidget variable;
    variable.setSelectionMode(QAbstractItemView::MultiSelection);

    QListWidgetItem item1("Air Temperature");
    QListWidgetItem item2("Precipitation");
    QListWidgetItem item3("Air Humidity");
    QListWidgetItem item4("Radiation");
    QListWidgetItem item5("Wind");
    QListWidgetItem item6("All variables");
    QFont font("Helvetica", 10, QFont::Bold);
    item6.setFont(font);

    variable.addItem(&item1);
    variable.addItem(&item2);
    variable.addItem(&item3);
    variable.addItem(&item4);
    variable.addItem(&item5);
    variable.addItem(&item6);

    timeVarLayout.addWidget(&daily);
    timeVarLayout.addWidget(&hourly);
    timeVarLayout.addWidget(&variable);

    QDateEdit *FirstDateEdit = new QDateEdit;
    FirstDateEdit->setDate(QDate::currentDate());
    QLabel *FirstDateLabel = new QLabel("   Start Date:");
    FirstDateLabel->setBuddy(FirstDateEdit);

    QDateEdit *LastDateEdit = new QDateEdit;
    LastDateEdit->setDate(QDate::currentDate());
    QLabel *LastDateLabel = new QLabel("    End Date:");
    LastDateLabel->setBuddy(LastDateEdit);

    dateLayout.addWidget(FirstDateLabel);
    dateLayout.addWidget(FirstDateEdit);

    dateLayout.addWidget(LastDateLabel);
    dateLayout.addWidget(LastDateEdit);

    QDialogButtonBox buttonBox;
    QPushButton downloadButton("Download");
    downloadButton.setCheckable(true);
    downloadButton.setAutoDefault(false);

    QPushButton cancelButton("Cancel");
    cancelButton.setCheckable(true);
    cancelButton.setAutoDefault(false);

    buttonBox.addButton(&downloadButton, QDialogButtonBox::AcceptRole);
    buttonBox.addButton(&cancelButton, QDialogButtonBox::RejectRole);

    downloadDialog.connect(&buttonBox, SIGNAL(accepted()), &downloadDialog, SLOT(accept()));
    downloadDialog.connect(&buttonBox, SIGNAL(rejected()), &downloadDialog, SLOT(reject()));

    buttonLayout.addWidget(&buttonBox);
    mainLayout.addLayout(&timeVarLayout);
    mainLayout.addLayout(&dateLayout);
    mainLayout.addLayout(&buttonLayout);
    downloadDialog.setLayout(&mainLayout);

    downloadDialog.exec();

    if (downloadDialog.result() != QDialog::Accepted)
        return false;

   QDate firstDate = FirstDateEdit->date();
   QDate lastDate = LastDateEdit->date();

   if (!daily.isChecked() && !hourly.isChecked())
   {
       QMessageBox::information(NULL, "Missing parameter", "Select hourly or daily");
       return downloadMeteoData();
   }
   else if ((! firstDate.isValid()) || (! lastDate.isValid()))
   {
       QMessageBox::information(NULL, "Missing parameter", "Select download period");
       return downloadMeteoData();
   }
   else if (!item1.isSelected() && !item2.isSelected() && !item3.isSelected() && !item4.isSelected() && !item5.isSelected() && !item6.isSelected())
   {
       QMessageBox::information(NULL, "Missing parameter", "Select variable");
       return downloadMeteoData();
   }
   else
   {
        QListWidgetItem* item = 0;
        QStringList var;
        for (int i = 0; i < variable.count()-1; ++i)
        {
               item = variable.item(i);
               if (item6.isSelected() || item->isSelected())
                   var.append(item->text());

        }
        if (daily.isChecked())
        {
            bool prec0024 = true;
            if ( item2.isSelected() || item6.isSelected() )
            {
                QDialog precDialog;
                precDialog.setFixedWidth(350);
                precDialog.setWindowTitle("Choose daily precipitation time");
                QVBoxLayout precLayout;
                QRadioButton first("0-24");
                QRadioButton second("08-08");

                QDialogButtonBox confirm(QDialogButtonBox::Ok);

                precDialog.connect(&confirm, SIGNAL(accepted()), &precDialog, SLOT(accept()));

                precLayout.addWidget(&first);
                precLayout.addWidget(&second);
                precLayout.addWidget(&confirm);
                precDialog.setLayout(&precLayout);
                precDialog.exec();

                if (second.isChecked())
                    prec0024 = false;
            }

            if (! myProject.downloadDailyDataArkimet(var, prec0024, firstDate, lastDate, true))
            {
                QMessageBox::information(NULL, "Error!", "Error in daily download");
                return false;
            }
        }

        if (hourly.isChecked())
        {
            // QApplication::setOverrideCursor(Qt::WaitCursor);
            // QApplication::restoreOverrideCursor();

            if (! myProject.downloadHourlyDataArkimet(var, firstDate, lastDate, true))
            {
                QMessageBox::information(NULL, "Error!", "Error in hourly download");
                return false;
            }
        }

        return true;
    }
}


bool chooseRasterColorScale(Crit3DColorScale *myColorScale, QLabel *myLabel)
{
    if (myColorScale == NULL)
        return false;

    QDialog myDialog;
    QVBoxLayout mainLayout;
    QVBoxLayout layoutVariable;
    QHBoxLayout layoutOk;

    myDialog.setWindowTitle("Choose color scale");
    myDialog.setFixedWidth(400);

    QRadioButton DTM("Digital Terrain map m");
    QRadioButton Temp("Temperature °C");
    QRadioButton Prec("Precipitation mm");
    QRadioButton RH("Relative humidity %");
    QRadioButton Rad("Solar radiation MJ m-2");
    QRadioButton Wind("Wind intensity m s-1");

    layoutVariable.addWidget(&DTM);
    layoutVariable.addWidget(&Temp);
    layoutVariable.addWidget(&Prec);
    layoutVariable.addWidget(&RH);
    layoutVariable.addWidget(&Rad);
    layoutVariable.addWidget(&Wind);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    myDialog.connect(&buttonBox, SIGNAL(accepted()), &myDialog, SLOT(accept()));
    myDialog.connect(&buttonBox, SIGNAL(rejected()), &myDialog, SLOT(reject()));

    layoutOk.addWidget(&buttonBox);

    mainLayout.addLayout(&layoutVariable);
    mainLayout.addLayout(&layoutOk);
    myDialog.setLayout(&mainLayout);
    myDialog.exec();

    if (myDialog.result() != QDialog::Accepted)
        return false;

    meteoVariable myVar = noMeteoVar;

    if (DTM.isChecked()) myVar = noMeteoVar;
    else if (Temp.isChecked()) myVar = airTemperature;
    else if (Prec.isChecked()) myVar = precipitation;
    else if (RH.isChecked()) myVar = airHumidity;
    else if (Rad.isChecked()) myVar = globalIrradiance;
    else if (Wind.isChecked()) myVar = windIntensity;

    setRasterColorScale(myVar, myColorScale, myLabel);
    return true;
}


void setRasterColorScale(meteoVariable myVar, Crit3DColorScale *myColorScale, QLabel *myLabel)
{
    switch(myVar)
    {
    case airTemperature:
        setTemperatureScale(myColorScale);
        myLabel->setText("Air temperature  °C");
        break;

    case precipitation:
        setPrecipitationScale(myColorScale);
        myLabel->setText("Precipitation  mm");
        break;

    case globalIrradiance:
    case dailyGlobalRadiation:
        setRadiationScale(myColorScale);
        myLabel->setText("Solar irradiance W m-2");
        break;

    case (airHumidity):
        setRelativeHumidityScale(myColorScale);
        myLabel->setText("Relative humidity  %");
        break;

    case windIntensity:
        setWindIntensityScale(myColorScale);
        myLabel->setText("Wind intensity  m s-1");
        break;

    default:
        setDefaultDTMScale(myColorScale);
        myLabel->setText("Digital Terrain Map  m");
    }
}



#include <QFileDialog>
#include <QtDebug>
#include <QCheckBox>
#include <QMessageBox>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QListWidget>
#include <QStringBuilder>
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
#include "commonConstants.h"
#include "utils.h"


#define TOOLSWIDTH 250
extern Project myProject;

MainWindow::MainWindow(environment menu, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    this->myRubberBand = NULL;

    ui->setupUi(this);

    // Set the MapGraphics Scene and View
    this->mapScene = new MapGraphicsScene(this);
    this->mapView = new MapGraphicsView(mapScene, this->ui->widgetMap);
    this->legend = new ColorLegend(this->ui->widgetColorLegendRaster);
    this->legend->resize(this->ui->widgetColorLegendRaster->size());

    // Set tiles source
    this->setMapSource(OSMTileSource::OSMTiles);

    // Set start size and position
    Position* startCenter = new Position (11.35, 44.5, 0.0);
    this->mapView->setZoomLevel(10);
    this->mapView->centerOn(startCenter->lonLat());

    // Set raster object
    this->rasterObj = new RasterObject(this->mapView);
    this->rasterObj->setOpacity(this->ui->rasterOpacitySlider->value() / 100.0);
    this->rasterObj->setColorLegend(this->legend);
    this->mapView->scene()->addObject(this->rasterObj);

    this->updateVariable();
    this->updateDateTime();

    this->setMouseTracking(true);

    this->menu = menu;

    //set menu
    switch(this->menu)
    {
        case praga  :
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
    delete legend;
    delete mapView;
    delete mapScene;
    delete ui;
}


void MainWindow::setMapSource(OSMTileSource::OSMTileType mySource)
{
    QSharedPointer<OSMTileSource> myTiles(new OSMTileSource(mySource), &QObject::deleteLater);
    QSharedPointer<CompositeTileSource> composite(new CompositeTileSource(), &QObject::deleteLater);
    composite->addSourceBottom(myTiles);

    this->mapView->setTileSource(composite);
}


void MainWindow::on_actionLoadRaster_triggered()
{

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open rasterObj"), "", tr("ESRI grid files (*.flt)"));
    if (fileName == "") return;

    qDebug() << "loading raster";
    if (!myProject.loadRaster(fileName)) return;

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

                    myProject.dbMeteoPoints = new DbMeteoPoints(dbName);
                    myProject.meteoPoints = myProject.dbMeteoPoints->getPropertiesFromDb();

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
    resetMeteoPoints();

    QString dbName = QFileDialog::getOpenFileName(this, tr("Open DB meteo"), "", tr("DB files (*.db)"));
    if (dbName == "") return;

    myProject.dbMeteoPoints = new DbMeteoPoints(dbName);
    myProject.meteoPoints =  myProject.dbMeteoPoints->getPropertiesFromDb();
    addMeteoPoints();

    loadMeteoPointsData(myProject.dbMeteoPoints);
    this->updateDateTime();
}


void MainWindow::loadMeteoPointsData(DbMeteoPoints* myDbMeteo)
{
    QDialog load;
    QVBoxLayout mainLayout;
    QHBoxLayout layoutTime;
    QHBoxLayout layoutPeriod;
    QHBoxLayout layoutOk;

    load.setWindowTitle("Load Data from DB");
    load.setFixedWidth(300);

    QCheckBox daily("Daily");
    QCheckBox hourly("Hourly");
    layoutTime.addWidget(&daily);
    layoutTime.addWidget(&hourly);

    QRadioButton lastDay("last day available");
    QRadioButton all("all data");

    char dayHour;

    layoutPeriod.addWidget(&lastDay);
    layoutPeriod.addWidget(&all);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok
                                         | QDialogButtonBox::Cancel);

    connect(&buttonBox, SIGNAL(accepted()), &load, SLOT(accept()));
    connect(&buttonBox, SIGNAL(rejected()), &load, SLOT(reject()));

    layoutOk.addWidget(&buttonBox);

    mainLayout.addLayout(&layoutTime);
    mainLayout.addLayout(&layoutPeriod);
    mainLayout.addLayout(&layoutOk);
    load.setLayout(&mainLayout);
    load.exec();

    if (load.result() != QDialog::Accepted)
        return;

    if (!daily.isChecked() && !hourly.isChecked())
    {
       QMessageBox::information(NULL, "Missing parameter", "Select hourly and/or daily");
       return;
    }

    if (!lastDay.isChecked() && !all.isChecked())
    {
       QMessageBox::information(NULL, "Missing parameter", "Select loading period from DB");
       return;
    }

    if (daily.isChecked())
    {
        dayHour = 'D';
        lastDate = myDbMeteo->getLastDay(dayHour).date();
        firstDate = lastDate;
        if ( all.isChecked() )
        {
            firstDate = myDbMeteo->getFirstDay(dayHour).date();
        }

        Crit3DDate dateStart = getCrit3DDate(firstDate);
        Crit3DDate dateEnd = getCrit3DDate(lastDate);

        formInfo myInfo;
        int step = myInfo.start("Load daily data...", myProject.meteoPoints.size());

        for (int i=0; i < myProject.meteoPoints.size(); i++)
        {
            if ((i % step) == 0) myInfo.setValue(i);

            myDbMeteo->getDailyData(dateStart, dateEnd, &(myProject.meteoPoints[i]));
        }
        myInfo.close();
    }

    if (hourly.isChecked())
    {
        dayHour = 'H';
        lastDate = myDbMeteo->getLastDay(dayHour).date();
        firstDate = lastDate;
        if ( all.isChecked() )
        {
            firstDate = myDbMeteo->getFirstDay(dayHour).date();
        }

        Crit3DDate dateStart = getCrit3DDate(firstDate);
        Crit3DDate dateEnd = getCrit3DDate(lastDate);

        formInfo myInfo;
        int step = myInfo.start("Load hourly data...", myProject.meteoPoints.size());

        for (int i=0; i < myProject.meteoPoints.size(); i++)
        {
            if ((i % step) == 0) myInfo.setValue(i);

            myDbMeteo->getHourlyData(dateStart, dateEnd, &(myProject.meteoPoints[i]));
        }
        myInfo.close();
    }

    myProject.setCurrentDate(lastDate);
    myProject.setCurrentHour(12);
}


void MainWindow::addMeteoPoints()
{
    QApplication::setOverrideCursor(Qt::ArrowCursor);

    myProject.meteoPointsSelected.clear();
    for (int i = 0; i < myProject.meteoPoints.size(); i++)
    {
        StationMarker* point = new StationMarker(5.0, true, QColor((Qt::white)), this->mapView);

        point->setFlag(MapGraphicsObject::ObjectIsMovable, false);
        point->setLatitude(myProject.meteoPoints[i].latitude);
        point->setLongitude(myProject.meteoPoints[i].longitude);

        this->pointList.append(point);
        this->mapView->scene()->addObject(this->pointList[i]);

        point->setToolTip();
    }
}


void MainWindow::on_actionDownload_meteo_data_triggered()
{
    if(myProject.meteoPoints.isEmpty())
    {
         QMessageBox::information(NULL, "DB not existing", "Create or Open a meteo points database before download");
         return;
    }

    QDialog downloadDialog;
    QVBoxLayout mainLayout;
    QHBoxLayout timeVarLayout;
    QVBoxLayout calendarLayout;
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

    calendar = new QCalendarWidget;
    calendar->setGridVisible(true);
    QLabel label("Enter download period");
    label.setAlignment(Qt::AlignCenter);

    FirstDateEdit = new QDateEdit;
    FirstDateEdit->setDate(calendar->selectedDate());
    FirstDateLabel = new QLabel(tr("&Start Date:"));
    FirstDateLabel->setBuddy(FirstDateEdit);

    LastDateEdit = new QDateEdit;
    LastDateEdit->setDate(calendar->selectedDate());
    LastDateLabel = new QLabel(tr("&End Date:"));
    LastDateLabel->setBuddy(LastDateEdit);

    calendarLayout.addWidget(&label);
    calendarLayout.addWidget(calendar);

    dateLayout.addWidget(FirstDateLabel);
    dateLayout.addWidget(FirstDateEdit);

    dateLayout.addWidget(LastDateLabel);
    dateLayout.addWidget(LastDateEdit);

    connect(FirstDateEdit, SIGNAL(dateChanged(QDate)), calendar, SLOT(setSelectedDate(QDate)));
    connect(LastDateEdit, SIGNAL(dateChanged(QDate)), calendar, SLOT(setSelectedDate(QDate)));
    connect(calendar,SIGNAL(clicked(const QDate)),this,SLOT(setCalendarDate(const QDate)));

    QDialogButtonBox buttonBox;
    QPushButton downloadButton(tr("&Download"));
    downloadButton.setCheckable(true);
    downloadButton.setAutoDefault(false);

    QPushButton cancelButton(tr("&Cancel"));
    cancelButton.setCheckable(true);
    cancelButton.setAutoDefault(false);

    buttonBox.addButton(&downloadButton, QDialogButtonBox::AcceptRole);
    buttonBox.addButton(&cancelButton, QDialogButtonBox::RejectRole);

    connect(&buttonBox, SIGNAL(accepted()), &downloadDialog, SLOT(accept()));
    connect(&buttonBox, SIGNAL(rejected()), &downloadDialog, SLOT(reject()));

    buttonLayout.addWidget(&buttonBox);
    mainLayout.addLayout(&timeVarLayout);
    mainLayout.addLayout(&calendarLayout);
    mainLayout.addLayout(&dateLayout);
    mainLayout.addLayout(&buttonLayout);
    downloadDialog.setLayout(&mainLayout);

    downloadDialog.exec();

    if (downloadDialog.result() == QDialog::Accepted)
    {

       firstDate = FirstDateEdit->date();
       lastDate = LastDateEdit->date();
       isFirstDate = true;

       if (!daily.isChecked() && !hourly.isChecked())
       {
           QMessageBox::information(NULL, "Missing parameter", "Select hourly or daily");
           on_actionDownload_meteo_data_triggered();
       }
       else if ((! firstDate.isValid()) || (! lastDate.isValid()))
       {
           QMessageBox::information(NULL, "Missing parameter", "Select download period");
           on_actionDownload_meteo_data_triggered();
       }
       else if (!item1.isSelected() && !item2.isSelected() && !item3.isSelected() && !item4.isSelected() && !item5.isSelected() && !item6.isSelected())
       {
           QMessageBox::information(NULL, "Missing parameter", "Select variable");
           on_actionDownload_meteo_data_triggered();
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
                bool prec24 = true;
                if ( item2.isSelected() || item6.isSelected() )
                {
                    QDialog precDialog;
                    precDialog.setFixedWidth(350);
                    precDialog.setWindowTitle("Choose daily precipitation time");
                    QVBoxLayout precLayout;
                    QRadioButton first("0-24");
                    QRadioButton second("08-08");

                    QDialogButtonBox confirm(QDialogButtonBox::Ok);

                    connect(&confirm, SIGNAL(accepted()), &precDialog, SLOT(accept()));

                    precLayout.addWidget(&first);
                    precLayout.addWidget(&second);
                    precLayout.addWidget(&confirm);
                    precDialog.setLayout(&precLayout);
                    precDialog.exec();

                    if (second.isChecked())
                        prec24 = false;
                }

                QApplication::setOverrideCursor(Qt::WaitCursor);

                if (! myProject.downloadDailyDataArkimet(var, prec24, getCrit3DDate(firstDate), getCrit3DDate(lastDate)))
                {
                    QMessageBox *msgBox = new QMessageBox(this);
                    msgBox->setText("Daily Download Error");
                    msgBox->exec();
                    delete msgBox;
                }

                QApplication::restoreOverrideCursor();
            }

            if (hourly.isChecked())
            {

                QApplication::setOverrideCursor(Qt::WaitCursor);

                QMessageBox *msgBox = new QMessageBox(this);
                if (myProject.downloadHourlyDataArkimet(var, getCrit3DDate(firstDate), getCrit3DDate(lastDate)))
                {
                    QApplication::restoreOverrideCursor();
                    msgBox->setText("Hourly Download Completed");
                }
                else
                {
                    QApplication::restoreOverrideCursor();
                    msgBox->setText("Hourly Download Error");
                }
                msgBox->exec();
                delete msgBox;
            }
       }
    }
}


void MainWindow::setCalendarDate(const QDate& date)
{
    if (isFirstDate)
    {
        firstDate = date;
        FirstDateEdit->setDate(calendar->selectedDate());
        isFirstDate = false;
    }
    else
    {
        if (firstDate <= date)
        {
            lastDate = date;
            LastDateEdit->setDate(calendar->selectedDate());
        }
        else
        {
            QMessageBox::information(NULL, "Invalid Date", "Last date is earlier than start date");
        }
        isFirstDate = true;
    }
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

    this->ui->widgetMap->setGeometry(TOOLSWIDTH, 0, this->width()-TOOLSWIDTH, this->height()-40);
    this->mapView->resize(this->ui->widgetMap->size());

    this->ui->groupBoxRaster->move(10, this->height() - this->ui->groupBoxRaster->height() - 50);
    this->ui->groupBoxVariable->move(10, this->ui->groupBoxRaster->y() - this->ui->groupBoxVariable->height() - 30);
}

QPoint MainWindow::getMapPoint(QPoint* point) const
{
    QPoint mapPoint;
    int dx, dy;
    dx = this->ui->widgetMap->x() + 10;
    dy = + this->ui->widgetMap->y() + 10 + this->ui->menuBar->height();
    mapPoint.setX(point->x() - dx);
    mapPoint.setY(point->y() - dy);
    return mapPoint;
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

void MainWindow::on_actionMapWikimedia_triggered()
{
    this->setMapSource(OSMTileSource::WikimediaMaps);
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


void MainWindow::resetMeteoPoints()
{
    this->myRubberBand = NULL;
    myProject.meteoPoints.clear();

    myProject.meteoPointsSelected.clear();

    firstDate.setDate(0,0,0);
    lastDate.setDate(0,0,0);

    datasetCheckbox.clear();

    for (int i = 0; i < this->pointList.size(); i++)
    {
        this->mapView->scene()->removeObject(this->pointList[i]);
    }
    qDeleteAll(this->pointList.begin(), this->pointList.end());
    this->pointList.clear();
}


void MainWindow::on_actionVariableChoose_triggered()
{
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
    else return;

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    connect(&buttonBox, SIGNAL(accepted()), &myDialog, SLOT(accept()));
    connect(&buttonBox, SIGNAL(rejected()), &myDialog, SLOT(reject()));

    layoutOk.addWidget(&buttonBox);

    mainLayout.addLayout(&layoutVariable);
    mainLayout.addLayout(&layoutOk);
    myDialog.setLayout(&mainLayout);
    myDialog.exec();

    if (myDialog.result() == QDialog::Accepted)
    {
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
    }

    this->updateVariable();
}


void MainWindow::on_actionVariableHourly_triggered()
{
    this->ui->actionVariableHourly->setChecked(true);
    if (this->ui->actionVariableDaily->isChecked())
        this->ui->actionVariableDaily->setChecked(false);

    myProject.setFrequency(hourly);
    this->updateVariable();
}

void MainWindow::on_actionVariableDaily_triggered()
{
    this->ui->actionVariableDaily->setChecked(true);
    if (this->ui->actionVariableHourly->isChecked())
        this->ui->actionVariableHourly->setChecked(false);

    myProject.setFrequency(daily);
    this->updateVariable();
}


void MainWindow::updateVariable()
{
    // FREQUENCY
    if (myProject.getFrequency() == daily)
    {
        this->ui->labelFrequency->setText("Frequency: daily");

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
        this->ui->labelFrequency->setText("Frequency: hourly");

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

    //VARIABLE
    if ((myProject.currentVariable == airTemperature)
            || (myProject.currentVariable == dailyAirTemperatureAvg))
        this->ui->labelVariable->setText("Average air temperature °C");
    else if ((myProject.currentVariable == airHumidity)
            || (myProject.currentVariable == dailyAirHumidityAvg))
        this->ui->labelVariable->setText("Average relative humidity %");
    else if ((myProject.currentVariable == dailyPrecipitation)
            ||  (myProject.currentVariable == precipitation))
        this->ui->labelVariable->setText("Precipitation mm");
    else if (myProject.currentVariable == dailyAirTemperatureMax)
        this->ui->labelVariable->setText("Maximum air temperature °C");
    else if (myProject.currentVariable == dailyAirTemperatureMin)
        this->ui->labelVariable->setText("Minimum air temperature °C");
    else if (myProject.currentVariable == dailyGlobalRadiation)
        this->ui->labelVariable->setText("Global solar radiation MJ m-2");
    else if (myProject.currentVariable == dailyAirHumidityMax)
        this->ui->labelVariable->setText("Maximum relative humidity %");
    else if (myProject.currentVariable == dailyAirHumidityMin)
        this->ui->labelVariable->setText("Minimum relative humidity %");
    else if (myProject.currentVariable == globalIrradiance)
        this->ui->labelVariable->setText("Solar irradiance W m-2");

    else
        this->ui->labelVariable->setText("None");

    redrawMeteoPoints();
    // TODO colorLegend
}


void MainWindow::updateDateTime()
{
    this->ui->dateTimeEdit->setDate(myProject.getCurrentDate());
    this->ui->dateTimeEdit->setTime(QTime(myProject.getCurrentHour(),0,0,0));
    redrawMeteoPoints();
}


void MainWindow::redrawMeteoPoints()
{
    if (myProject.meteoPoints.size() == 0) return;

    Crit3DDate myDate = getCrit3DDate(myProject.getCurrentDate());
    int hour = myProject.getCurrentHour();

    float v = NODATA;
    float minimum = NODATA;
    float maximum = NODATA;
    for (int i = 0; i < myProject.meteoPoints.size(); i++)
    {
        if (myProject.getFrequency() == daily)
            v =  myProject.meteoPoints[i].getMeteoPointValueD(myDate, myProject.currentVariable);
        else if (myProject.getFrequency() == hourly)
            v =  myProject.meteoPoints[i].getMeteoPointValueH(myDate, hour, 0, myProject.currentVariable);

        if (v != NODATA)
        {
            if (minimum == NODATA)
            {
                minimum = v;
                maximum = v;
            }
            else if (v < minimum) minimum = v;
            else if (v > maximum) maximum = v;
        }
    }

    qDebug() << "Min, max: " << QString::number(minimum) << QString::number(maximum);

}

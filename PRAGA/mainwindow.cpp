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
#include "Position.h"
#include "formSingleValue.h"
#include "formDownload.h"
#include "dbMeteoPoints.h"
#include "dbArkimet.h"
#include "download.h"
#include "project.h"
#include "commonConstants.h"


#define TOOLSWIDTH 220
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
    this->legend = new ColorLegend(this->ui->widgetColorLegend);
    this->legend->resize(this->ui->widgetColorLegend->size());

    // Set tiles source
    this->setMapSource(OSMTileSource::OSMTiles);

    // Set start size and position
    Position* startCenter = new Position (11.35, 44.5, 0.0);
    this->mapView->setZoomLevel(10);
    this->mapView->centerOn(startCenter->lonLat());

    // Set raster object
    this->rasterObj = new RasterObject(this->mapView);
    this->rasterObj->setOpacity(this->ui->opacitySlider->value() / 100.0);
    this->rasterObj->setColorLegend(this->legend);
    this->mapView->scene()->addObject(this->rasterObj);

    //this->setMouseTracking(true);

    this->menu = menu;

    //set menu
    switch(this->menu)
    {
        case praga  :
            ui->actionDownload_meteo_data->setVisible(true);
            ui->actionArkimet->setVisible(true);
            break;
        case criteria1D:
            ui->actionDownload_meteo_data->setVisible(false);
            ui->actionArkimet->setVisible(false);
            break;
        case criteria3D :
            ui->actionDownload_meteo_data->setVisible(false);
            ui->actionArkimet->setVisible(false);
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


void MainWindow::on_actionArkimet_triggered()
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

            connect(all, SIGNAL(toggled(bool)), this, SLOT(enableAll(bool)));

            QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                                 | QDialogButtonBox::Cancel);

            connect(buttonBox, SIGNAL(accepted()), &datasetDialog, SLOT(accept()));
            connect(buttonBox, SIGNAL(rejected()), &datasetDialog, SLOT(reject()));

            layout.addWidget(buttonBox);
            datasetDialog.setLayout(&layout);

            QString datasetSelected = on_actionArkimet_Dataset(&datasetDialog);

            if (!datasetSelected.isEmpty())
            {
                myDbArkimet->setDatasetsActive(datasetSelected);
                QStringList datasets = datasetSelected.remove("'").split(",");

                QApplication::setOverrideCursor(Qt::WaitCursor);

                FormDownload downloadWindow;
                downloadWindow.setModal(true);
                downloadWindow.show();
                QMessageBox *msgBox = new QMessageBox(this);

                if (myDownload->getPointProperties(datasets))
                {
                    downloadWindow.close();
                    QApplication::restoreOverrideCursor();

                    myProject.dbMeteoPoints = new DbMeteoPoints(dbName);
                    myProject.meteoPoints = myProject.dbMeteoPoints->getPropertiesFromDb();

                    delete msgBox;
                    displayMeteoPoints();
                }
                else
                {
                    downloadWindow.close();
                    QApplication::restoreOverrideCursor();

                    msgBox->setText("Download Error");
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


QString MainWindow::on_actionArkimet_Dataset(QDialog* datasetDialog) {

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
                return on_actionArkimet_Dataset(datasetDialog);
            }

        }
        else
            return "";

}


void MainWindow::enableAll(bool toggled)
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
    displayMeteoPoints();

    loadMeteoPointsData(myProject.dbMeteoPoints);
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
    QDateTime firstD;
    QDateTime lastD;

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

    if (load.result() == QDialog::Accepted)
    {

       if (!daily.isChecked() && !hourly.isChecked())
       {
           QMessageBox::information(NULL, "Missing parameter", "Select hourly and/or daily");
           return;
       }
       else if (!lastDay.isChecked() && !all.isChecked())
       {
           QMessageBox::information(NULL, "Missing parameter", "Select loading period from DB");
           return;
       }
       else
       {
            if (daily.isChecked())
            {
                dayHour = 'D';
                lastD = myDbMeteo->getLastDay(dayHour);
                firstD.setDate(lastD.date());
                firstD.setTime(QTime(0, 0, 0));
                if ( all.isChecked() )
                {
                    firstD = myDbMeteo->getFirstDay(dayHour);
                }

                Crit3DDate dateStart(firstD.date().day(), firstD.date().month(), firstD.date().year());
                Crit3DDate dateEnd(lastD.date().day(), lastD.date().month(), lastD.date().year());
                myDbMeteo->getDataFromDailyDb(dateStart, dateEnd, myProject.meteoPoints);

                /*
                qDebug() << "myProject.meteoPoints[0].id" << QString::fromStdString(myProject.meteoPoints[0].id);

                qDebug() << "myProject.meteoPoints[0].obsDataD[0].date.day" << myProject.meteoPoints[0].obsDataD[0].date.day;
                qDebug() << "myProject.meteoPoints[0].obsDataD[0].tMin" << myProject.meteoPoints[0].obsDataD[0].tMin;

                qDebug() << "myProject.meteoPoints[0].obsDataD[1].date.day" << myProject.meteoPoints[0].obsDataD[1].date.day;
                qDebug() << "myProject.meteoPoints[0].obsDataD[1].tMin" << myProject.meteoPoints[0].obsDataD[1].tMin;
                */

            }

            if (hourly.isChecked())
            {
                dayHour = 'H';
                lastD = myDbMeteo->getLastDay(dayHour);
                firstD.setDate(lastD.date());
                firstD.setTime(QTime(0, 0, 0));
                if ( all.isChecked() )
                {
                    firstD = myDbMeteo->getFirstDay(dayHour);
                }

                Crit3DDate dateStart(firstD.date().day(), firstD.date().month(), firstD.date().year());
                Crit3DDate dateEnd(lastD.date().day(), lastD.date().month(), lastD.date().year());
                myDbMeteo->getDataFromHourlyDb(dateStart, dateEnd, myProject.meteoPoints);

                /*
                qDebug() << "myProject.meteoPoints[0].id" << QString::fromStdString(myProject.meteoPoints[0].id);

                qDebug() << "myProject.meteoPoints[0].obsDataD[0].date.day" << myProject.meteoPoints[0].obsDataH[0].date.day;
                qDebug() << "myProject.meteoPoints[0].obsDataD[0].tAir" << myProject.meteoPoints[0].obsDataH[0].tAir;

                qDebug() << "myProject.meteoPoints[0].obsDataD[1].date.day" << myProject.meteoPoints[0].obsDataH[1].date.day;
                qDebug() << "myProject.meteoPoints[0].obsDataD[1].tAir" << myProject.meteoPoints[0].obsDataH[1].tAir;

                qDebug() << "myProject.meteoPoints[0].obsDataD[1].date.day" << myProject.meteoPoints[0].obsDataH[6].date.day;
                qDebug() << "myProject.meteoPoints[0].obsDataD[1].rhAir[0]" << myProject.meteoPoints[0].obsDataH[6].rhAir[0];
                qDebug() << "myProject.meteoPoints[0].obsDataD[1].rhAir[1]" << myProject.meteoPoints[0].obsDataH[6].rhAir[1];
                */
            }
       }
    }

    qDebug() << "firstD = " << firstD;
    qDebug() << "lastD = " << lastD;
}


void MainWindow::displayMeteoPoints()
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

    if (myProject.startDate.isValid() && myProject.endDate.isValid())
    {
        FirstDateEdit->setDate(myProject.startDate);
        LastDateEdit->setDate(myProject.endDate);
    }

    calendarLayout.addWidget(&label);
    calendarLayout.addWidget(calendar);

    dateLayout.addWidget(FirstDateLabel);
    dateLayout.addWidget(FirstDateEdit);

    dateLayout.addWidget(LastDateLabel);
    dateLayout.addWidget(LastDateEdit);

    connect(FirstDateEdit, SIGNAL(dateChanged(QDate)), calendar, SLOT(setSelectedDate(QDate)));
    connect(LastDateEdit, SIGNAL(dateChanged(QDate)), calendar, SLOT(setSelectedDate(QDate)));
    connect(calendar,SIGNAL(clicked(const QDate)),this,SLOT(slotClicked(const QDate)));

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

       myProject.startDate = FirstDateEdit->date();
       myProject.endDate = LastDateEdit->date();
       initDate = true;

       if(myProject.meteoPoints.isEmpty())
       {
            QMessageBox::information(NULL, "DB not existing", "Create or Open DB before download");
            downloadDialog.close();
            return;
       }

       if (!daily.isChecked() && !hourly.isChecked())
       {
           QMessageBox::information(NULL, "Missing parameter", "Select hourly or daily");
           on_actionDownload_meteo_data_triggered();
       }
       else if (!myProject.startDate.isValid() || !myProject.endDate.isValid())
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
                bool precSelection = true;
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
                        precSelection = false;

                }
                QApplication::setOverrideCursor(Qt::WaitCursor);
                FormDownload downloadWindow;
                downloadWindow.setModal(true);
                downloadWindow.show();

                QMessageBox *msgBox = new QMessageBox(this);

                if ( myProject.downloadArkimetDailyVar(var, precSelection))
                {
                    downloadWindow.close();
                    QApplication::restoreOverrideCursor();

                    msgBox->setText("Daily Download Completed");
                }
                else
                {
                    downloadWindow.close();
                    QApplication::restoreOverrideCursor();

                    msgBox->setText("Daily Download Error");
                }
                msgBox->exec();
                delete msgBox;
            }

            if (hourly.isChecked())
            {

                QApplication::setOverrideCursor(Qt::WaitCursor);
                FormDownload downloadWindow;
                downloadWindow.setModal(true);
                downloadWindow.show();

                QMessageBox *msgBox = new QMessageBox(this);
                if (myProject.downloadArkimetHourlyVar(var))
                {
                    downloadWindow.close();
                    QApplication::restoreOverrideCursor();
                    msgBox->setText("Hourly Download Completed");

                }
                else
                {
                    downloadWindow.close();
                    QApplication::restoreOverrideCursor();
                    msgBox->setText("Hourly Download Error");
                }
                msgBox->exec();
                delete msgBox;
            }

       }
    }
}



void MainWindow::slotClicked(const QDate& date)
{

  if (initDate)
  {
    myProject.startDate = date;
    FirstDateEdit->setDate(calendar->selectedDate());
    initDate = false;
  }
  else
  {
    if (myProject.startDate <= date)
    {
        myProject.endDate = date;
        LastDateEdit->setDate(calendar->selectedDate());
    }
    else
    {
        QMessageBox::information(NULL, "Invalid Date", "Last date is earlier than start date");
    }
    initDate = true;
  }

}


void MainWindow::mouseReleaseEvent(QMouseEvent *event){
    Q_UNUSED(event)

    if (this->rasterObj != NULL)
        this->rasterObj->updateCenter();

    gis::Crit3DGeoPoint pointSelected;;
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

    this->ui->widgetMap->setGeometry(TOOLSWIDTH, 0, this->width()-TOOLSWIDTH, this->height() - 40);
    this->mapView->resize(this->ui->widgetMap->size());

    this->ui->groupBoxRaster->move(10, this->height() - this->ui->groupBoxRaster->height() - 50);
}

QPoint MainWindow::getMapPoint(QPoint* point) const
{
    QPoint mapPoint;
    int dx, dy;
    dx = this->ui->widgetMap->x() + 10;
    dy = + this->ui->widgetMap->y() + this->ui->menuBar->height() + 10;
    mapPoint.setX(point->x() - dx);
    mapPoint.setY(point->y() - dy);
    return mapPoint;
}

void MainWindow::on_opacitySlider_sliderMoved(int position)
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

    qDebug() << "on_actionRectangle_Selection_triggered" ;
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

    myProject.startDate.setDate(0,0,0);
    myProject.endDate.setDate(0,0,0);

    datasetCheckbox.clear();

    for (int i = 0; i < this->pointList.size(); i++)
    {
        this->mapView->scene()->removeObject(this->pointList[i]);
    }
    qDeleteAll(this->pointList.begin(), this->pointList.end());
    this->pointList.clear();
}

#include <QtGui>
#include <QFileDialog>
#include <QDateTime>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "formSingleValue.h"
#include "formDate.h"

#include "commonConstants.h"
#include "parserXML.h"
#include "modelCore.h"
#include "waterBalance.h"
#include "gis.h"
#include "dataHandler.h"
#include "project.h"
#include "atmosphere.h"
#include "utilities.h"


//cout
#include <iostream>
using namespace std;

extern Crit3DProject myProject;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setCentralWidget(&myMapWidget);
}


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_menuDB_Load_data_triggered()
{
    formDate myForm;
    myForm.show();
    int myReturn = myForm.exec();
    if (myReturn == QDialog::Rejected) return;

    myProject.loadObsDataAllPoints(myForm.dateTimeFirst.date(), myForm.dateTimeLast.date());
    this->update();
}

void MainWindow::on_menuDB_Load_Points_Postgres_triggered()
{
    myProject.loadDBPoints();
    this->update();
}

void MainWindow::on_menuFile_load_dtm_triggered()
{
    QString myFileName;

    myFileName = QFileDialog::getOpenFileName(this, tr("Open DTM"), "",
                                           tr("ESRI grid files (*.flt)"));

    if (myFileName != "")
        if (myProject.loadDTM(myFileName))
        {
            setDefaultDTMScale(myProject.dtm.colorScale);
            myMapWidget.setCurrentView(dtmMap);
            this->update();
        }
}

bool MainWindow::interpolateMap(meteoVariable myVar)
{
    bool myResult;
    if (myProject.nrMeteoPoints == 0)
    {
        myProject.logError("No points avalaible\n");
        return (false);
    }
    else
    {
        formDate myForm;
        myForm.show();
        int myReturn = myForm.exec();
        if (myReturn == QDialog::Rejected) return false;

        myProject.currentTime = getCrit3DTime(myForm.dateTimeFirst);
        myProject.currentVar = myVar;

        QDate currentDate = myForm.dateTimeFirst.date();
        myProject.loadObsDataAllPoints(currentDate, currentDate);

        myResult = interpolationProjectDtmMain(&myProject, myVar, myProject.currentTime, true);
        if (! myResult) myProject.logError();
        return (myResult);
    }
}

void MainWindow::on_menuInterpolationAirTemperature_triggered()
{
    if (interpolateMap(airTemperature))
    {
        this->myMapWidget.setCurrentView(airTempMap);
        this->update();
    }
}

void MainWindow::on_menuInterpolationPrecipitation_triggered()
{
    if (interpolateMap(precipitation))
    {
        this->myMapWidget.setCurrentView(precMap);
        this->update();
    }
}

void MainWindow::on_menuInterpolationAirHumidity_triggered()
{
    if (interpolateMap(airHumidity))
    {
        this->myMapWidget.setCurrentView(relHumMap);
        this->update();
    }
}

void MainWindow::on_menuInterpolationWindIntensity_triggered()
{
    if (interpolateMap(windIntensity))
    {
        this->myMapWidget.setCurrentView(windIntMap);
        this->update();
    }
}

void MainWindow::on_menuInterpolationLeafWetness_triggered()
{
    if (interpolateMap(wetnessDuration))
    {
        this->myMapWidget.setCurrentView(leafWetMap);
        this->update();
    }
}

void MainWindow::on_menuInterpolationSolarIrradiance_triggered()
{
    if (interpolateMap(globalIrradiance))
    {
        this->myMapWidget.setCurrentView(globRadMap);
        this->update();
    }
}

void MainWindow::on_actionLoad_soil_triggered()
{
    if (myProject.loadVanGenuchtenParameters())
        if (myProject.loadSoils())
            myProject.logInfo("Done.");
}


void MainWindow::on_actionLoad_Project_triggered()
{
    QString myFileName = QFileDialog::getOpenFileName(this,tr("Open Project"), "", tr("Project files (*.xml)"));
    if (myFileName != "")
    {
        if (myProject.loadProject(myFileName))
        {
            setDefaultDTMScale(myProject.dtm.colorScale);
            myMapWidget.setCurrentView(dtmMap);
        }
        else myProject.logError("Open project failed:\n " + myFileName);
        this->update();
    }
}

void MainWindow::on_actionInitialize_waterbalance_triggered()
{
    if (!initializeWaterBalance(&myProject))
        myProject.logError();
}


void MainWindow::on_actionTry_Model_cycle_triggered()
{
    if (! myProject.isProjectLoaded) return;

    formDate myForm;
    myForm.show();
    int myReturn = myForm.exec();
    if (myReturn == QDialog::Rejected) return;

    myProject.runModels(myForm.dateTimeFirst, myForm.dateTimeLast, true, myProject.idArea);
}


void MainWindow::on_actionClose_Project_triggered()
{
    if (! myProject.isProjectLoaded) return;
    myProject.closeProject();
    this->update();
}

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "testHeat.h"
#include <QString>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QApplication>

#include "commonConstants.h"
#include "graphs.h"

long myHourIni, myHourFin, myCurrentHour;
bool Initialized;
bool useInputData;

double *myTempInput;
double *myPrecInput;
double *myRHInput;
double *myWSInput;
double *myRadInput;
double *myNetRadInput;

long myInputNumber;

bool meteoDataLoaded = false;
bool soilDataLoaded = false;

QString myOutput("");

Qsoil *myInputSoils = NULL;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    outPlot = new Plot( this );
    ui->layoutPlot->addWidget(outPlot);
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::initializeModel()
{

    useInputData = ui->chkUseInput->isChecked() && (soilDataLoaded || meteoDataLoaded);

    // geometry
    setDimension(1);

    if (ui->chkBoxGeometricProgression->isChecked())
        setProgression(PROGRESSION_GEOMETRIC);
    else
        setProgression(PROGRESSION_ARITHMETIC);

    setMinThickness(ui->lineEditMinThickness->text().toDouble());
    setMaxThickness(ui->lineEditMaxThickness->text().toDouble());
    setTotalDepth(ui->lineEditDepth->text().toDouble());

    // initialization
    setInitialSaturation(ui->lineEditIniWaterContent->text().toDouble());
    setInitialTemperature(ui->lineEditIniTTop->text().toDouble(), ui->lineEditIniTBottom->text().toDouble());

    // simulation
    if (!useInputData)
    {
        myHourIni = ui->lineEditSimStart->text().toInt();
        myHourFin = ui->lineEditSimStop->text().toInt();
    }

    setSimulationStart(myHourIni);
    setSimulationStop(myHourFin);

    // processes
    bool computeHeat = ui->chkBoxHeat->isChecked();
    bool computeWater = ui->chkBoxWater->isChecked();

    setProcesses(computeWater, computeHeat, false);

    // surface
    setSurface(ui->lineEditWidth->text().toDouble(),
               ui->lineEditAlbedo->text().toDouble(),
               ui->lineEditManningRoughness->text().toDouble(),
               ui->lineEditPlough->text().toDouble(),
               ui->lineEditRoughnessHeight->text().toDouble());


    // bottom boundary
    setBottomTemperature(ui->lineEditBottomT->text().toDouble());

    // set soil
    if (! soilDataLoaded)
        setSoil(ui->lineEditThetaS->text().toDouble(), ui->lineEditThetaR->text().toDouble(), ui->lineEditClay->text().toDouble(), ui->lineEditOrganic->text().toDouble());

    Initialized = initializeTestHeat(&myHourIni, &myHourFin, soilDataLoaded);

    myCurrentHour = myHourIni;

    return true;
}

void MainWindow::on_pushInitialize_clicked()
{
    Initialized = initializeModel();

    myOutput = "";

    getHourlyOutput(myHourIni, 0, getNodesNumber(), myOutput);

    if (Initialized)
    {
        ui->plainTextEdit->setPlainText(myOutput);
        ui->prgBar->setMaximum(myHourFin);
    }
    else
    {
        myOutput = "Initialization failed";
        ui->plainTextEdit->setPlainText(myOutput);
    }

}

void MainWindow::on_pushRunNextHour_clicked()
{
    if (Initialized)
    {
        setHour(++myCurrentHour);

        double myPIniHour, myPHours;
        double myT, myRH, myWS, myNR, myP;

        myPIniHour = ui->lineEditPrecStart->text().toInt();
        myPHours = ui->lineEditPrecHours->text().toInt();

        if (useInputData)
        {
            myT = myTempInput[myCurrentHour-1] + 273.16;
            myP = myPrecInput[myCurrentHour-1];
            myRH = myRHInput[myCurrentHour-1];
            myWS = myWSInput[myCurrentHour-1];
            myNR = myNetRadInput[myCurrentHour-1];
        }
        else
        {
            myT = ui->lineEditAtmT->text().toDouble() + 273.16;
            myRH = ui->lineEditAtmRH->text().toDouble();
            myWS = ui->lineEditAtmWS->text().toDouble();
            myNR = 0.;

            if ((myCurrentHour >= myPIniHour) && (myCurrentHour < myPIniHour + myPHours))
                myP = ui->lineEditPrecHourlyAmount->text().toDouble();
            else
                myP = 0.;
        }

        runTestHeat(myT, myRH, myWS, myNR, myP);

        getHourlyOutput(myCurrentHour, 0, getNodesNumber(), myOutput);

        ui->plainTextEdit->setPlainText(myOutput);
        ui->prgBar->setValue(myCurrentHour);
    }
}

void MainWindow::on_pushRunAllPeriod_clicked()
{

    myOutput = "";
    emptyOutput();
    heat_output* myHeatOutput = new heat_output();

    myHeatOutput->nrLayers = ceil(ui->lineEditDepth->text().toDouble() / ui->lineEditMinThickness->text().toDouble()) + 1;

    initializeModel();

    if (Initialized)
        ui->prgBar->setMaximum(myHourFin);
    else
        ui->plainTextEdit->setPlainText("Initialization failed");

    if (Initialized)
    {
        getHourlyOutputAllPeriod(1, getNodesNumber(), myOutput, myHeatOutput);

        outPlot->setSamples(myHeatOutput->profileOutput[0].temperature);

        double myPIniHour, myPHours;
        double myT, myRH, myWS, myGR, myNR, myP;

        myPIniHour = ui->lineEditPrecStart->text().toInt();
        myPHours = ui->lineEditPrecHours->text().toInt();
        myGR = NODATA;
        myNR = NODATA;

        do
        {
            setHour(++myCurrentHour);

            qApp->processEvents();

            if (useInputData)
            {
                myT = myTempInput[myCurrentHour-1] + 273.16;
                myP = myPrecInput[myCurrentHour-1];
                myRH = myRHInput[myCurrentHour-1];
                myWS = myWSInput[myCurrentHour-1];
                myNR = myNetRadInput[myCurrentHour-1];
            }
            else
            {
                myT = ui->lineEditAtmT->text().toDouble() + 273.16;
                myRH = ui->lineEditAtmRH->text().toDouble();
                myWS = ui->lineEditAtmWS->text().toDouble();
                myGR = ui->lineEditAtmGR->text().toDouble();

                myNR = 0.;

                if ((myCurrentHour >= myPIniHour) && (myCurrentHour < myPIniHour + myPHours))
                    myP = ui->lineEditPrecHourlyAmount->text().toDouble();
                else
                    myP = 0.;
            }

            runTestHeat(myT, myRH, myWS, myNR, myP);

            getHourlyOutputAllPeriod(1, getNodesNumber(), myOutput, myHeatOutput);

            outPlot->setSamples(myHeatOutput->profileOutput[myCurrentHour].temperature);

            ui->prgBar->setValue(myCurrentHour);

        } while (myCurrentHour < myHourFin);

        ui->plainTextEdit->setPlainText(myOutput);
    }
}


void MainWindow::on_pushLoadFileSoil_clicked()
{
    QString myFilename = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                     "",
                                                     tr("Files (*.*)"));

    myInputNumber = 0;

    QFile myFile(myFilename);
    QTextStream myStream(&myFile);
    QStringList myWords;

    if (myFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        while (!myStream.atEnd()) {
            QString myLine = myStream.readLine();
            myWords += myLine.split(",");
            myInputNumber++;
        }

        myInputSoils = (Qsoil *) calloc(myInputNumber, sizeof(Qsoil));

        long myIndex;
        int myFieldNumber = 22;

        setSoilHorizonNumber(myInputNumber-1);

        for (myIndex = 1; myIndex < myInputNumber; myIndex++)
        {
            myInputSoils[myIndex-1].profSup = myWords.at(myIndex*myFieldNumber).toDouble();
            myInputSoils[myIndex-1].profInf = myWords.at(myIndex*myFieldNumber + 1).toDouble();
            myInputSoils[myIndex-1].Clay = myWords.at(myIndex*myFieldNumber + 5).toDouble();
            myInputSoils[myIndex-1].OrganicMatter = myWords.at(myIndex*myFieldNumber + 8).toDouble();
            myInputSoils[myIndex-1].CEC = myWords.at(myIndex*myFieldNumber + 9).toDouble();
            myInputSoils[myIndex-1].K_sat = myWords.at(myIndex*myFieldNumber + 11).toDouble();
            myInputSoils[myIndex-1].PH = myWords.at(myIndex*myFieldNumber + 12).toDouble();
            myInputSoils[myIndex-1].VG_alfa = myWords.at(myIndex*myFieldNumber + 15).toDouble();
            myInputSoils[myIndex-1].VG_n = myWords.at(myIndex*myFieldNumber + 16).toDouble();
            myInputSoils[myIndex-1].VG_m = 1.0 - 1.0 / myInputSoils[myIndex-1].VG_n;
            myInputSoils[myIndex-1].Theta_r = myWords.at(myIndex*myFieldNumber + 18).toDouble();
            myInputSoils[myIndex-1].Theta_s = myWords.at(myIndex*myFieldNumber + 19).toDouble();
            myInputSoils[myIndex-1].VG_he = myWords.at(myIndex*myFieldNumber + 20).toDouble();
            myInputSoils[myIndex-1].Mualem_L = myWords.at(myIndex*myFieldNumber + 21).toDouble();
            myInputSoils[myIndex-1].Plough = 0.005;
            myInputSoils[myIndex-1].Roughness = 0.024;
        }

        soilDataLoaded = true;
    }

    myFile.close();
}



void MainWindow::on_pushLoadFileMeteo_clicked()
{
    QString myFilename = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                     "",
                                                     tr("Files (*.*)"));

    myInputNumber = 0;

    QFile myFile(myFilename);
    QTextStream myStream(&myFile);
    QStringList myWords;

    if (myFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        while (!myStream.atEnd()) {
            QString myLine = myStream.readLine();
            myWords += myLine.split(",");
            myInputNumber++;
        }

        myTempInput = (double *) calloc(myInputNumber, sizeof(double));
        myPrecInput = (double *) calloc(myInputNumber, sizeof(double));
        myRHInput= (double *) calloc(myInputNumber, sizeof(double));
        myWSInput = (double *) calloc(myInputNumber, sizeof(double));
        myNetRadInput = (double *) calloc(myInputNumber, sizeof(double));

        long myHourIndex;

        myHourIni=0;
        myHourFin=0;

        int myFieldNumber = 7;

        for (myHourIndex = 1; myHourIndex < myInputNumber; myHourIndex++)
        {
            myTempInput[myHourIndex-1] = myWords.at(myHourIndex*myFieldNumber+1).toDouble();
            myPrecInput[myHourIndex-1] = myWords.at(myHourIndex*myFieldNumber+2).toDouble();
            myRHInput[myHourIndex-1] = myWords.at(myHourIndex*myFieldNumber+3).toDouble();
            myWSInput[myHourIndex-1] = myWords.at(myHourIndex*myFieldNumber+4).toDouble();
            myNetRadInput[myHourIndex-1] = myWords.at(myHourIndex*myFieldNumber+6).toDouble();

            myHourFin++;
        }

        meteoDataLoaded = true;
    }

    myFile.close();

}


void MainWindow::on_chkBoxWater_clicked()
{

}

void MainWindow::on_chkBoxHeat_clicked()
{

}



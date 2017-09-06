#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "heat1D.h"
#include <QString>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QApplication>
#include <qclipboard.h>

#include "qwt_plot_curve.h"

#include "graphs.h"

#include "commonConstants.h"

long myHourIni, myHourFin, myCurrentHour;
bool Initialized;
bool useInputMeteoData, useInputSoilData;

double *myTempInput;
double *myPrecInput;
double *myRHInput;
double *myWSInput;
double *myRadInput;
double *myNetRadInput;

long myInputNumber;

bool meteoDataLoaded = false;
bool soilDataLoaded = false;

QString myTextOutput("");
Crit3DOut myHeatOutput;
Qsoil *myInputSoils = NULL;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    outPlot = new Plot( this );
    ui->layoutPlot->addWidget(outPlot);

    ui->listWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->listWidget->addItem("soil temperature (degC)");
    ui->listWidget->addItem("soil water content (m3 m-3)");
    ui->listWidget->addItem("total heat flux (W m-2)");
    ui->listWidget->addItem("diffusive heat flux (W m-2)");
    ui->listWidget->addItem("isothermal latent heat flux (W m-2)");
    ui->listWidget->addItem("thermal latent heat flux (W m-2)");
    ui->listWidget->addItem("isothermal water liquid flux (l s-1)");
    ui->listWidget->addItem("thermal water liquid flux (l s-1)");
    ui->listWidget->addItem("isothermal water vapor flux (l s-1)");
    ui->listWidget->addItem("thermal water vapor flux (l s-1)");
    ui->listWidget->addItem("surface energy balance (W m-2)");
    ui->listWidget->addItem("surface resistences (s m-1)");
    ui->listWidget->addItem("error balance ()");
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::initializeModel()
{

    useInputMeteoData = ui->chkUseInputMeteo->isChecked() && meteoDataLoaded;
    useInputSoilData = ui->chkUseInputSoil->isChecked() && soilDataLoaded;

    setThickness(ui->lineEditThickness->text().toDouble());
    setTotalDepth(ui->lineEditDepth->text().toDouble());

    // initialization
    setInitialSaturation(ui->lineEditIniWaterContent->text().toDouble());
    setInitialTemperature(ui->lineEditIniTTop->text().toDouble(), ui->lineEditIniTBottom->text().toDouble());

    // simulation
    if (!useInputMeteoData)
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
    setBottomTemperature(ui->lineEditBottomT->text().toDouble(), ui->lineEditBottomZ->text().toDouble());

    // set soil
    if (! soilDataLoaded)
        setSoil(ui->lineEditThetaS->text().toDouble(), ui->lineEditThetaR->text().toDouble(), ui->lineEditClay->text().toDouble(), ui->lineEditOrganic->text().toDouble());

    Initialized = initializeHeat1D(&myHourIni, &myHourFin, useInputSoilData);

    myCurrentHour = myHourIni;

    return true;
}

/*
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

        //getHourlyOutput(myCurrentHour, 0, getNodesNumber(), myOutput);

        ui->prgBar->setValue(myCurrentHour);
    }
}
*/

void MainWindow::on_pushRunAllPeriod_clicked()
{
    myHeatOutput.clean();

    initializeModel();

    long nodesNr = getNodesNumber();
    myHeatOutput.nrLayers = nodesNr;
    myHeatOutput.layerThickness = ui->lineEditThickness->text().toFloat();

    if (Initialized)
        ui->prgBar->setMaximum(myHourFin);
    else
    {
        ui->labelInfo->setText("Initialization failed");
        return;
    }

    getHourlyOutputAllPeriod(1, nodesNr, &myHeatOutput);

    double myPIniHour, myPHours;
    double myT, myRH, myWS, myNR, myP;

    myPIniHour = ui->lineEditPrecStart->text().toInt();
    myPHours = ui->lineEditPrecHours->text().toInt();
    myNR = NODATA;

    do
    {
        setHour(++myCurrentHour);

        qApp->processEvents();

        if (useInputMeteoData)
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

        runHeat1D(myT, myRH, myWS, myNR, myP);

        getHourlyOutputAllPeriod(1, getNodesNumber(), &myHeatOutput);

        ui->prgBar->setValue(myCurrentHour);

    } while (myCurrentHour < myHourFin);

    myTextOutput = myHeatOutput.getTextOutput();

    outPlot->drawOutput(outputGroup::soilTemperature, &myHeatOutput);
    ui->listWidget->item(outputGroup::soilTemperature)->setSelected(true);
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

        int myFieldNumber = 6;

        for (myHourIndex = 1; myHourIndex < myInputNumber; myHourIndex++)
        {
            myTempInput[myHourIndex-1] = myWords.at(myHourIndex*myFieldNumber+1).toDouble();
            myPrecInput[myHourIndex-1] = myWords.at(myHourIndex*myFieldNumber+2).toDouble();
            myRHInput[myHourIndex-1] = myWords.at(myHourIndex*myFieldNumber+3).toDouble();
            myWSInput[myHourIndex-1] = myWords.at(myHourIndex*myFieldNumber+4).toDouble();
            myNetRadInput[myHourIndex-1] = myWords.at(myHourIndex*myFieldNumber+5).toDouble();

            myHourFin++;
        }

        meteoDataLoaded = true;
    }

    myFile.close();

}


void MainWindow::on_pushCopyOutput_clicked()
{
    QClipboard *myClip = QApplication::clipboard();
    myClip->setText(myTextOutput);
}

void MainWindow::on_listWidget_itemClicked(QListWidgetItem * selItem)
{
    outputGroup myType = (outputGroup)ui->listWidget->row(selItem);
    outPlot->drawOutput(myType, &myHeatOutput);
}


void MainWindow::on_chkUseInput_clicked()
{
    ui->groupBox_atmFixedData->setEnabled(! ui->chkUseInputMeteo->isChecked());
    ui->pushLoadFileMeteo->setEnabled(ui->chkUseInputMeteo->isChecked());
}

void MainWindow::on_chkUseInputSoil_clicked()
{
    ui->groupBox_soil->setEnabled(! ui->chkUseInputSoil->isCheckable());
    ui->pushLoadFileSoil->setEnabled(ui->chkUseInputSoil->isCheckable());
}

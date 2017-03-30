#include "mainwindow.h"
#include "download.h"

// test
#ifndef CRIT3DDATE_H
    #include "crit3dDate.h" //solo x test, cancellare
#endif


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{

    QStringList datasets;
    datasets << "fiduli" ;
    //datasets << "agrmet";

    Download* download = new Download("/home/laura/criteria3D/DATA/template/prova.db", this);
    //download->getPointProperties(datasets);

    Crit3DDate dateStart(10,1,2017);
    Crit3DDate dateEnd(15,1,2017);
    QList<int> stations;
    //stations << 7353 << 2308 << 6238;
    stations << 12328 << 12345 << 12260 << 12291 << 12362 << 12267 << 12937;

    QList<int> variableHourly;
    QList<int> variableDaily;

    //variableHourly << 166 << 165 << 139 << 164 << 159 << 158 << 78;
    variableHourly << 160 << 159 << 78;


    Crit3DTime dateStartTime(dateStart,0);
    Crit3DTime dateEndTime(dateEnd,0);
    download->downloadHourlyVar(dateStartTime, dateEndTime, datasets, stations, variableHourly);

    //variableDaily << 232 << 233 << 231 << 250 << 241 << 242 << 240 << 706 << 227 << 230;
    //bool precSelection = true;
    //download->downloadDailyVar(dateStart, dateEnd, datasets, stations, variableDaily, precSelection);

    //download->debugFromFile();

    delete download;
}


MainWindow::~MainWindow()
{
}

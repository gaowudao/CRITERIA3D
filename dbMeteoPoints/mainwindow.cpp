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
    //datasets << "agrmet" << "cer" ;
    datasets << "agrmet";

    Download* download = new Download("/home/laura/criteria3D/DATA/template/prova.db", this);
    //download->getPointProperties(datasets);

    Crit3DDate dateStart(1,1,2017);
    Crit3DDate dateEnd(10,2,2017);
    QList<int> stations;
    stations << 2287 << 2288 << 2289 << 7353;
    QList<int> variable;
    variable << 232 << 233 << 231 << 250 << 241 << 242 << 240 << 706 << 227 << 230;
    bool precSelection = true;
    //download->downloadDailyVar(dateStart, dateEnd, datasets, stations, variable, precSelection);

    Crit3DTime dateStartTime(dateStart,0);
    Crit3DTime dateEndTime(dateEnd,0);
    download->downloadHourlyVar(dateStartTime, dateEndTime, datasets, stations, variable);

    //download->debugFromFile();

}


MainWindow::~MainWindow()
{

}

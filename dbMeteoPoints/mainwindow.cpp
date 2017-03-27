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
    Crit3DDate dateEnd(3,1,2017);
    QList<int> stations;
    stations << 7353 << 2308 << 6238;
    QList<int> variable;
    //variable << 232 << 233 << 231 << 250 << 241 << 242 << 240 << 706 << 227 << 230;
    //variable << 166 << 165 << 139 << 164 << 159 << 158 << 78;
    variable << 159 ;
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

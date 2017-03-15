#include "mainwindow.h"
#include "download.h"

// test


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{

    QStringList datasets;
    //datasets << "agrmet" << "cer" ;
    datasets << "agrmet";

    Download* download = new Download("/home/laura/criteria3D/DATA/template/prova.db", this);
    //downloadTest->getPointProperties(datasets);

    download->debugFromFile();

}


MainWindow::~MainWindow()
{

}

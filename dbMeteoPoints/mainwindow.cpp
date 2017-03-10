#include "mainwindow.h"
#include "download.h"

// test


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{

    QStringList datasets;
    datasets << "agrmet" << "cer" ;

    Download* downloadTest = new Download(this);
    QString dbName = "/home/laura/criteria3D/DATA/template/prova.db";
    downloadTest->getPointProperties(datasets,dbName);

}


MainWindow::~MainWindow()
{

}

#include "mainwindow.h"
#include "download.h"

// test


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    Download* downloadTest = new Download(this);
    QStringList datasets;
    datasets << "agrmet" << "cer" ;
    downloadTest->getPointProperties("/home/laura/criteria3D/DATA/template/prova.db", datasets);
}


MainWindow::~MainWindow()
{

}

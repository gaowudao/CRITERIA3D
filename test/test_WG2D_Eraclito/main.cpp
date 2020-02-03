#include "commonConstants.h"
#include "dbMeteoGrid.h"
#include <iostream>
#include <QFileDialog>
#include <QApplication>


static Crit3DMeteoGridDbHandler* meteoGridDbHandler;


void logInfo(QString myStr)
{
     std::cout << myStr.toStdString() << std::endl;
}


bool loadMeteoGridDB()
{
    QString xmlName = QFileDialog::getOpenFileName(nullptr, "Open XML grid", "", "XML files (*.xml)");
    if (xmlName == "") return false;

    QString errorString;
    meteoGridDbHandler = new Crit3DMeteoGridDbHandler();

    // todo
    //meteoGridDbHandler->meteoGrid()->setGisSettings(this->gisSettings);

    if (! meteoGridDbHandler->parseXMLGrid(xmlName, &errorString)) return false;

    if (! meteoGridDbHandler->openDatabase(&errorString)) return false;

    if (! meteoGridDbHandler->loadCellProperties(&errorString)) return false;

    meteoGridDbHandler->updateGridDate(&errorString);

    logInfo("Meteo Grid = " + xmlName);

    return true;
}



int main(int argc, char *argv[])
{
    QApplication myApp(argc, argv);

    loadMeteoGridDB();
    return 0;
}




#include "commonConstants.h"
#include "dbMeteoGrid.h"
#include "meteo.h"
#include "meteoPoint.h"

#include <iostream>
#include <QFileDialog>
#include <QApplication>
#include <malloc.h>


static Crit3DMeteoGridDbHandler* meteoGridDbHandler;


void logInfo(QString myStr)
{
     std::cout << myStr.toStdString() << std::endl;
}


bool loadMeteoGridDB(QString* errorString)
{
    QString xmlName = QFileDialog::getOpenFileName(nullptr, "Open XML grid", "", "XML files (*.xml)");
    if (xmlName == "") return false;

    meteoGridDbHandler = new Crit3DMeteoGridDbHandler();

    // todo
    //meteoGridDbHandler->meteoGrid()->setGisSettings(this->gisSettings);

    if (! meteoGridDbHandler->parseXMLGrid(xmlName, errorString)) return false;

    if (! meteoGridDbHandler->openDatabase(errorString))return false;

    if (! meteoGridDbHandler->loadCellProperties(errorString)) return false;

    if (! meteoGridDbHandler->updateGridDate(errorString)) return false;

    logInfo("Meteo Grid = " + xmlName);

    return true;
}



int main(int argc, char *argv[])
{
    QApplication myApp(argc, argv);
    QString myError;
    Crit3DMeteoPoint* meteoPointTemp = new Crit3DMeteoPoint;
    meteoVariable variable;
    QDate firstDay(1961,1,1);
    QDate lastDay(1990,12,31);
    QDate firstDateDB(1,1,1);
    TObsDataD** obsDataD = nullptr;

    std::vector<float> dailyVariable;
    QString errorString;
    if (! loadMeteoGridDB(&errorString))
    {
        std::cout << errorString.toStdString() << std::endl;
        return -1;
    }

    std::string id;
    int nrActivePoints = 0;
    int lengthSeries;
    for (int row = 0; row < meteoGridDbHandler->gridStructure().header().nrRows; row++)
    {
        //if (showInfo && (row % infoStep) == 0)
            //myInfo.setValue(row);

        for (int col = 0; col < meteoGridDbHandler->gridStructure().header().nrCols; col++)
        {
            /*if (meteoGridDbHandler->meteoGrid()->getMeteoPointActiveId(row, col, &id))
            {
                nrActivePoints++;
                if (col == 0 && row == 0)
                {
                    dailyVariable = meteoGridDbHandler->loadGridDailyVar(&myError,QString::fromStdString(meteoPointTemp->id),variable,firstDay,lastDay,&firstDateDB);
                    lengthSeries = dailyVariable.size();
                }
                //printf("%d %d\n", nrActivePoints, lengthSeries);
            }*/
           if (meteoGridDbHandler->meteoGrid()->getMeteoPointActiveId(row, col, &id))
           {
               Crit3DMeteoPoint* meteoPoint = meteoGridDbHandler->meteoGrid()->meteoPointPointer(row,col);

               // copy data to MPTemp
               /*meteoPointTemp->id = meteoPoint->id;
               meteoPointTemp->point.z = meteoPoint->point.z;
               meteoPointTemp->latitude = meteoPoint->latitude;
               meteoPointTemp->elaboration = meteoPoint->elaboration;

               // meteoPointTemp should be init
               meteoPointTemp->nrObsDataDaysH = 0;
               meteoPointTemp->nrObsDataDaysD = 0;*/

               variable = dailyAirTemperatureMin;
               dailyVariable = meteoGridDbHandler->loadGridDailyVar(&myError, QString::fromStdString(meteoPoint->id),
                                                                    variable, firstDay, lastDay, &firstDateDB);
               lengthSeries = dailyVariable.size();
               printf("%d %d\n", ++nrActivePoints, lengthSeries);
           }
        }
    }


    obsDataD = (TObsDataD **)calloc(nrActivePoints, sizeof(TObsDataD*));
    for (int i=0;i<nrActivePoints;i++)
    {
        obsDataD[i] = (TObsDataD *)calloc(lengthSeries, sizeof(TObsDataD));
    }
    // ciclo su tutte le celle, meteopoint ha la struttra vedere se e attiva
    variable = dailyAirTemperatureMin;
    //minDailyTemperature = meteoGridDbHandler->loadGridDailyVar(&myError,QString::fromStdString(meteoPointTemp->id),variable,firstDay,lastDay,&firstDateDB);
    variable = dailyAirTemperatureMax;
    //maxDailyTemperature = meteoGridDbHandler->loadGridDailyVar(&myError,myMeteoPoint,variable,firstDay,lastDay,&firstDateDB);
    variable = dailyPrecipitation;
    //maxDailyTemperature = meteoGridDbHandler->loadGridDailyVar(&myError,myMeteoPoint,variable,firstDay,lastDay,&firstDateDB);

    return 0;
}




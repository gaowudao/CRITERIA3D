#include "commonConstants.h"
#include "dbMeteoGrid.h"
#include "meteo.h"
#include "meteoPoint.h"
#include "wg2D.h"


#include <iostream>
#include <QFileDialog>
#include <QApplication>
#include <malloc.h>
#include <time.h>

#define NR_SIMULATION_YEARS 1
// [ 1 - 10 ]
//#define NR_STATIONS 10
#define STARTING_YEAR 2001
#define PREC_THRESHOLD 0.25

static Crit3DMeteoGridDbHandler* meteoGridDbHandler;
static weatherGenerator2D WG2D;

void logInfo(QString myStr)
{
     std::cout << myStr.toStdString() << std::endl;
}


bool loadMeteoGridDB(QString* errorString)
{
    //QString xmlName = QFileDialog::getOpenFileName(nullptr, "Open XML grid", "", "XML files (*.xml)");
    QString xmlName = "../../../PRAGA/DATA/METEOGRID/DBGridXML_Eraclito4.xml";
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
    time_t rawtime;
    struct tm * timeinfo;

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    printf ( "Current local time and date: %s", asctime (timeinfo) );

    QApplication myApp(argc, argv);
    QString myError;
    //Crit3DMeteoPoint* meteoPointTemp = new Crit3DMeteoPoint;
    meteoVariable variable;
    QDate firstDay(1961,1,1);
    QDate lastDay(1990,12,31);
    QDate currentDay;
    QDate firstDateDB(1,1,1);
    TObsDataD** obsDataD = nullptr;

    QString errorString;
    if (! loadMeteoGridDB(&errorString))
    {
        std::cout << errorString.toStdString() << std::endl;
        return -1;
    }

    std::string id;
    int nrActivePoints = 0;
    int lengthSeries = 0;
    std::vector<float> dailyVariable;


    for (int row = 0; row < meteoGridDbHandler->gridStructure().header().nrRows; row++)
    {

        for (int col = 0; col < meteoGridDbHandler->gridStructure().header().nrCols; col++)
        {

           if (meteoGridDbHandler->meteoGrid()->getMeteoPointActiveId(row, col, &id))
           {
               ++nrActivePoints;
               if (nrActivePoints == 1)
               {
                   variable = dailyAirTemperatureMin;
                   dailyVariable = meteoGridDbHandler->loadGridDailyVar(&myError, QString::fromStdString(id),
                                                                        variable, firstDay, lastDay, &firstDateDB);
                   lengthSeries = int(dailyVariable.size());
               }
           }
        }
    }
    //nrActivePoints = 5;
    printf("%d  %d\n", lengthSeries,nrActivePoints);
    obsDataD = (TObsDataD **)calloc(nrActivePoints, sizeof(TObsDataD*));
    for (int i=0;i<nrActivePoints;i++)
    {
        obsDataD[i] = (TObsDataD *)calloc(lengthSeries, sizeof(TObsDataD));
    }

    for (int i=0;i<nrActivePoints;i++)
    {
        currentDay = firstDay;
        for (int j=0;j<lengthSeries;j++)
        {
            obsDataD[i][j].date.day = currentDay.day();
            obsDataD[i][j].date.month = currentDay.month();
            obsDataD[i][j].date.year = currentDay.year();
            currentDay = currentDay.addDays(1);
        }
    }
    printf("initialize date\n");



    int counter = 0;
    for (int row = 0; row < meteoGridDbHandler->gridStructure().header().nrRows; row++)
    {

        for (int col = 0; col < meteoGridDbHandler->gridStructure().header().nrCols; col++)
        {

           if (meteoGridDbHandler->meteoGrid()->getMeteoPointActiveId(row, col, &id) && counter<nrActivePoints)
           {

               variable = dailyAirTemperatureMin;
               dailyVariable = meteoGridDbHandler->loadGridDailyVar(&myError, QString::fromStdString(id),
                                                                    variable, firstDay, lastDay, &firstDateDB);
               for (int iLength=0; iLength<lengthSeries; iLength++) obsDataD[counter][iLength].tMin = dailyVariable[iLength];
               variable = dailyAirTemperatureMax;
               dailyVariable = meteoGridDbHandler->loadGridDailyVar(&myError, QString::fromStdString(id),
                                                                    variable, firstDay, lastDay, &firstDateDB);
               for (int iLength=0; iLength<lengthSeries; iLength++) obsDataD[counter][iLength].tMax = dailyVariable[iLength];
               variable = dailyPrecipitation;
               dailyVariable = meteoGridDbHandler->loadGridDailyVar(&myError, QString::fromStdString(id),
                                                                    variable, firstDay, lastDay, &firstDateDB);
               for (int iLength=0; iLength<lengthSeries; iLength++) obsDataD[counter][iLength].prec = dailyVariable[iLength];
               counter++;
           }
        }
        //std::cout << row << "\n";
    }
    dailyVariable.clear();
    meteoGridDbHandler->closeDatabase();
    WG2D.initializeData(lengthSeries,nrActivePoints);
    WG2D.setObservedData(obsDataD);

    ToutputWeatherData* results;
    bool computePrecipitation = true;
    bool computeTemperature = true;
    printf("weather generator\n");
    int startingYear = STARTING_YEAR;
    int lengthArraySimulation;
    static int distributionType = 1;
    lengthArraySimulation = 365 * NR_SIMULATION_YEARS;
    WG2D.initializeParameters(PREC_THRESHOLD, NR_SIMULATION_YEARS, distributionType,
                              computePrecipitation, computeTemperature,false);
    WG2D.computeWeatherGenerator2D();
    results = WG2D.getWeatherGeneratorOutput(startingYear);

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    printf ( "Current local time and date: %s", asctime (timeinfo) );
    return 0;
}




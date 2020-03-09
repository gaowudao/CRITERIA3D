#include "commonConstants.h"
#include "utilities.h"
#include "dbMeteoGrid.h"
#include "meteo.h"
#include "meteoPoint.h"
#include "wg2D.h"
#include "crit3dDate.h"

#include <QDebug>
#include <iostream>
#include <QFileDialog>
#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QString>
#include <malloc.h>
#include <time.h>

#define NR_SIMULATION_YEARS 10
//#define MAX_NR_POINTS 10

// [ 1 - 10 ]
//#define NR_STATIONS 10
#define STARTING_YEAR 3001
#define PREC_THRESHOLD 0.25
void printSimulationResults(ToutputWeatherData* output,int nrStations,int lengthArray);
static Crit3DMeteoGridDbHandler* meteoGridDbHandler;
static Crit3DMeteoGridDbHandler* meteoGridDbHandlerWG2D;
static weatherGenerator2D WG2D;

void logInfo(QString myStr)
{
     std::cout << myStr.toStdString() << std::endl;
}


bool loadMeteoGridDB(QString* errorString)
{
    //QString xmlName = QFileDialog::getOpenFileName(nullptr, "Open XML grid", "", "XML files (*.xml)");

    QString path;
    if (! searchDataPath(&path)) return -1;
    QString xmlName = path + "METEOGRID/DBGridXML_Eraclito4.xml";

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

bool saveOnMeteoGridDB(QString* errorString)
{
    //QString xmlName = QFileDialog::getOpenFileName(nullptr, "Open XML grid", "", "XML files (*.xml)");
    QString path;
    if (! searchDataPath(&path)) return -1;
    QString xmlName = path + "METEOGRID/DBGridXML_Eraclito_WG2D.xml";

    meteoGridDbHandlerWG2D = new Crit3DMeteoGridDbHandler();

    // todo
    //meteoGridDbHandler->meteoGrid()->setGisSettings(this->gisSettings);

    if (! meteoGridDbHandlerWG2D->parseXMLGrid(xmlName, errorString)) return false;

    if (! meteoGridDbHandlerWG2D->openDatabase(errorString))return false;

    if (! meteoGridDbHandlerWG2D->loadCellProperties(errorString)) return false;

    //if (! meteoGridDbHandlerWG2D->updateGridDate(errorString)) return false;

    logInfo("Meteo Grid = " + xmlName);

    return true;
}

int main(int argc, char *argv[])
{
    int startingYear = STARTING_YEAR;
    printf("insert the starting year for the synthethic series:\n");
    scanf("%d",&startingYear);
    int nrYearSimulations = NR_SIMULATION_YEARS;
    printf("insert the number of years of the the synthethic series:\n");
    scanf("%d",&nrYearSimulations);
    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    printf ( "Current local time and date: %s", asctime (timeinfo) );

    QApplication myApp(argc, argv);
    QString appPath = myApp.applicationDirPath() + "/";

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

    #ifdef MAX_NR_POINTS
        nrActivePoints = MINVALUE(nrActivePoints, MAX_NR_POINTS);
    #endif

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

    printf("load data...\n");

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

    int lengthArraySimulation;
    static int distributionType = 1;
    lengthArraySimulation = 365 * nrYearSimulations;
    WG2D.initializeParameters(PREC_THRESHOLD, nrYearSimulations, distributionType,
                              computePrecipitation, computeTemperature,false);

    WG2D.computeWeatherGenerator2D();

    results = WG2D.getWeatherGeneratorOutput(startingYear);
    //printSimulationResults(results,nrActivePoints,lengthArraySimulation);
    std::vector<TObsDataD> outputDataD;
    int nrLeapYears = 0;
    for (int iYear = startingYear;iYear<startingYear+nrYearSimulations;iYear++ )
    {
        nrLeapYears += isLeapYear(iYear);
    }
    outputDataD.resize(lengthArraySimulation+nrLeapYears);

    // fill the new database
    QDate firstDayOutput(startingYear,1,1);
    QDate lastDayOutput(startingYear+nrYearSimulations-1,12,31);

    if (! saveOnMeteoGridDB(&errorString))
    {
        std::cout << errorString.toStdString() << std::endl;
        return -1;
    }
    QList<meteoVariable> listMeteoVariable = {dailyAirTemperatureMin,dailyAirTemperatureMax,dailyPrecipitation};
    counter = 0;

    for (int row = 0; row < meteoGridDbHandlerWG2D->gridStructure().header().nrRows; row++)
    {
        for (int col = 0; col < meteoGridDbHandlerWG2D->gridStructure().header().nrCols; col++)
        {
           if (meteoGridDbHandlerWG2D->meteoGrid()->getMeteoPointActiveId(row, col, &id) && counter<nrActivePoints)
           {
               meteoGridDbHandlerWG2D->meteoGrid()->meteoPointPointer(row,col)->initializeObsDataD(lengthArraySimulation+nrLeapYears, getCrit3DDate(firstDayOutput));
               int k = NODATA;
               for (int j=0;j<lengthArraySimulation;j++)
               {
                   if (j == 0) k = j;
                   outputDataD[k].date.day = results[counter].daySimulated[j];
                   outputDataD[k].date.month = results[counter].monthSimulated[j];
                   outputDataD[k].date.year = results[counter].yearSimulated[j];
                   outputDataD[k].tMin = results[counter].minT[j];
                   outputDataD[k].tMax = results[counter].maxT[j];
                   outputDataD[k].prec = results[counter].precipitation[j];
                   //printf("%d  %d  %d\n",outputDataD[k].date.day,outputDataD[k].date.month,outputDataD[k].date.year);
                   //printf("%f  %f  %f\n",outputDataD[k].tMin,outputDataD[k].tMax,outputDataD[k].prec);
                   if (results[counter].daySimulated[j] == 28 && results[counter].monthSimulated[j] == 2)
                   {
                        if (isLeapYear(results[counter].yearSimulated[j]))
                        {
                            ++k;
                            outputDataD[k].date.day = results[counter].daySimulated[j] + 1;
                            outputDataD[k].date.month = results[counter].monthSimulated[j];
                            outputDataD[k].date.year = results[counter].yearSimulated[j];
                            outputDataD[k].tMin = results[counter].minT[j];
                            outputDataD[k].tMax = results[counter].maxT[j];
                            outputDataD[k].prec = results[counter].precipitation[j];
                            //printf("%d  %d  %d\n",outputDataD[k].date.day,outputDataD[k].date.month,outputDataD[k].date.year);
                            //printf("%f  %f  %f\n",outputDataD[k].tMin,outputDataD[k].tMax,outputDataD[k].prec);
                        }
                   }

                   k++;


                   //getchar();
               }
               //getchar();
               meteoGridDbHandlerWG2D->meteoGrid()->meteoPointPointer(row,col)->obsDataD = outputDataD;
               /*for (int j=0;j<lengthArraySimulation+nrYearSimulations;j++)
               {
                   qDebug() << getQDate(meteoGridDbHandlerWG2D->meteoGrid()->meteoPointPointer(row,col)->obsDataD[j].date);
                   //printf("%f  %f  %f\npress enter to continue",meteoGridDbHandlerWG2D->meteoGrid()->meteoPointPointer(row,col)->obsDataD[j].tMin,meteoGridDbHandlerWG2D->meteoGrid()->meteoPointPointer(row,col)->obsDataD[j].tMax,meteoGridDbHandlerWG2D->meteoGrid()->meteoPointPointer(row,col)->obsDataD[j].prec );

               }*/
               meteoGridDbHandlerWG2D->saveCellGridDailyData(&myError, QString::fromStdString(id),row,col,firstDayOutput,lastDayOutput,listMeteoVariable);
               counter++;
           }
           meteoGridDbHandlerWG2D->meteoGrid()->meteoPointPointer(row,col)->obsDataD.clear();
        }
        //std::cout << row << "\n";
    }
    meteoGridDbHandlerWG2D->closeDatabase();
    return 0;
}

void printSimulationResults(ToutputWeatherData* output,int nrStations,int lengthArray)
{
    FILE* fp;
    QString outputName;
    for (int iStation=0; iStation<nrStations;iStation++)
    {
        outputName = "wgStation_" + QString::number(iStation) + ".csv";
        QFile file(outputName);
        file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
        QTextStream stream( &file );
        for (int m=0; m<lengthArray; m++)
        {
            stream <<  output[iStation].daySimulated[m] << "/" << output[iStation].monthSimulated[m] << "/" << output[iStation].yearSimulated[m] << "," << output[iStation].doySimulated[m] << "," << output[iStation].minT[m]<< "," << output[iStation].maxT[m]<< "," << output[iStation].precipitation[m]<<endl;
        }
        file.close();
    }

}


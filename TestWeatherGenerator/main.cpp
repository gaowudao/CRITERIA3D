/*-------------------------------------------------------------------

   TESTWEATHERGENERATOR
   uses testWG.ini in /data directory
   this project is part of CRITERIA3D distribution

-------------------------------------------------------------------
*/

#include <QFile>
#include <QDir>
#include <QStringList>
#include <QDebug>
#include <QSettings>
#include <QCoreApplication>

#include <float.h>
#include <time.h>

#include "weatherGenerator.h"
#include "wgClimate.h"
#include "timeUtility.h"
#include "fileUtility.h"
#include "commonConstants.h"


void printSeasonalForecastInfo(TXMLSeasonalAnomaly *XMLAnomaly)
{
    qDebug() << "\npoint.name = " << XMLAnomaly->point.name;
    qDebug() << "point.longitude = " << XMLAnomaly->point.longitude;
    qDebug() << "point.latitude = " << XMLAnomaly->point.latitude;
    qDebug() << "climate first year = " << XMLAnomaly->climatePeriod.yearFrom;
    qDebug() << "climate last year = " << XMLAnomaly->climatePeriod.yearTo;
    qDebug() << "number of models = " << XMLAnomaly->modelNumber;
    qDebug() << "models = " << XMLAnomaly->modelName;
    qDebug() << "number of members = " << XMLAnomaly->modelMember;
    qDebug() << "number of repetitions = " << XMLAnomaly->repetitions;
    qDebug() << "anomaly year = " << XMLAnomaly->anomalyYear;
    qDebug() << "anomaly season = " << XMLAnomaly->anomalySeason;
    qDebug() << "";
}


int main(int argc, char *argv[])
{
    TXMLSeasonalAnomaly XMLAnomaly;
    TinputObsData climateDailyObsData;
    TinputObsData lastYearDailyObsData;
    TwheatherGenClimate wGenClimate;

    QCoreApplication a(argc, argv);

    QString settingsFileName;

    if (argc > 1)
        settingsFileName = argv[1];
    else
    {  
        QString path = a.applicationDirPath();
        settingsFileName = path + "/../data/testWG.ini";

        /*
        qDebug() << "A settings file is required.";
        qDebug() << "\nUSAGE: WG settings.ini\n";
        return 0;
        */
    }

    // read settings
    QFile myFile(settingsFileName);
    if (!myFile.exists())
    {
        qDebug() << "Error!" << "\nMissing settings file:" << settingsFileName;
        return -1;
    }

    QFileInfo fileInfo(settingsFileName);
    QString path = fileInfo.path() + "/";

    QSettings* mySettings = new QSettings(settingsFileName, QSettings::IniFormat);
    mySettings->beginGroup("settings");

    QString climatePath = mySettings->value("climate",0).toString();
    if (climatePath.left(1) == ".") climatePath = path + climatePath;

    QString observedPath = mySettings->value("observed",0).toString();
    if (observedPath.left(1) == ".") observedPath = path + observedPath;

    QString seasForPath = mySettings->value("seasonalPredictions",0).toString();
    if (seasForPath.left(1) == ".") seasForPath = path + seasForPath;

    QString outputPath = mySettings->value("output",0).toString();
    if (outputPath.left(1) == ".") outputPath = path + outputPath;
    QDir outputDirectory(outputPath);

    //check output directory
    if (!outputDirectory.exists())
        if (!outputDirectory.mkdir(outputPath))
        {
            qDebug() << "Error: missing output directory" << outputPath;
            return -1;
        }

    QString mySeparator = mySettings->value("separator",0).toString();
    char separator = mySeparator.toStdString().c_str()[0];
    float minPercData = mySettings->value("minDataPercentage",0).toFloat();
    float thresholdPrec = mySettings->value("rainfallThreshold",0).toFloat();

    QString season;
    int wgDoy1 = NODATA;
    int wgDoy2 = NODATA;
    Crit3DDate climateDateIni, climateDateFin;

    // iterate input files on climate (climateName.csv = observedName.csv = forecastName.xml)
    QString fileName, climateFileName, observedFileName, xmlFileName, outputFileName;
    QDir climateDirectory(climatePath);
    QStringList filters ("*.csv");
    QFileInfoList fileList = climateDirectory.entryInfoList (filters);

    bool isOk;
    QFile *testFile;
    for (int i = 0; i < fileList.size(); ++i)
    {
        fileName = fileList.at(i).fileName();
        climateFileName = climatePath + "/" + fileName;
        observedFileName = observedPath + "/" + fileName;
        xmlFileName = seasForPath + "/" + fileName.left(fileName.length()-4) + ".xml";
        outputFileName = outputPath + "/" + fileName;

        //check observed data
        isOk = true;
        testFile = new QFile(observedFileName);
        if (!testFile->exists())
        {
            qDebug() << "ERROR: missing observed data:" << fileName;
            isOk = false;
        }

        // check xml file
        testFile = new QFile(xmlFileName);
        if (!testFile->exists())
        {
            qDebug() << "ERROR: missing seasonal forecast:" << xmlFileName;
            isOk = false;
        }

        if (isOk)
        {
            qDebug() << "\n..........COMPUTE:" << fileName << "\n";

            // read SEASONAL PREDICTIONS
            if (! parseXMLClimate(xmlFileName, &XMLAnomaly))
                return -1;

            // compute first and last day of the year of the season period
            season = XMLAnomaly.anomalySeason.toUpper();
            getDoyFromSeason(season, XMLAnomaly.anomalyYear, &wgDoy1, &wgDoy2);

            // set climate dates
            climateDateIni = Crit3DDate(1,1,XMLAnomaly.climatePeriod.yearFrom);
            climateDateFin = Crit3DDate(31, 12, XMLAnomaly.climatePeriod.yearTo);

            printSeasonalForecastInfo(&XMLAnomaly);

            // read CLIMATE data
            if ( !readMeteoDataCsv(climateFileName, separator, NODATA, &climateDailyObsData) )
                return -1;

            // read OBSERVED data (at least last 9 months)
            if ( !readMeteoDataCsv(observedFileName, separator, NODATA, &lastYearDailyObsData) )
                return -1;

            //check climate dates
            Crit3DDate climateObsFirstDate = climateDailyObsData.inputFirstDate;
            climateObsFirstDate = std::max(climateDateIni, climateObsFirstDate);

            Crit3DDate climateObsLastDate = climateDailyObsData.inputFirstDate.addDays(climateDailyObsData.dataLenght-1);
            climateObsLastDate = std::min(climateDateFin, climateObsLastDate);

            int requestedClimateDays = climateDateIni.daysTo(climateDateFin);
            int obsClimateDays = climateObsFirstDate.daysTo(climateObsLastDate);

            if ((float(obsClimateDays) / float(requestedClimateDays)) < minPercData)
            {
                qDebug() << "\nError!" << "\nRequested climate period is:" << XMLAnomaly.climatePeriod.yearFrom << "-" << XMLAnomaly.climatePeriod.yearTo;
                qDebug() << "Percentage of climate data are less than requested (" << (minPercData*100) << "%)";
                qDebug() << "\n***** ERROR! *****" << fileName << "Computation FAILED\n";
            }
            else
            {
                // weather generator - computes climate without anomaly
                if (!climateGenerator(climateDailyObsData.dataLenght, climateDailyObsData, climateObsFirstDate, climateObsLastDate, thresholdPrec, minPercData, &wGenClimate))
                {
                    qDebug() << "Error in climateGenerator";
                    qDebug() << "\n***** ERROR! *****" << fileName << "Computation FAILED\n";
                }
                else
                {
                    qDebug() << "\n...Climate OK\n";

                    /* initialize random seed: */
                    srand (time(nullptr));

                    // SEASONAL FORECAST
                    if (! makeSeasonalForecast(outputFileName, separator, &XMLAnomaly, wGenClimate,
                          &lastYearDailyObsData, XMLAnomaly.repetitions, XMLAnomaly.anomalyYear, wgDoy1, wgDoy2, thresholdPrec))
                        qDebug() << "\n***** ERROR! *****" << fileName << "Computation FAILED\n";

                }
                free(climateDailyObsData.inputTMin);
                free(climateDailyObsData.inputTMax);
                free(climateDailyObsData.inputPrecip);
            }
        }
    }

    qDebug() << "\nEND\n";
}

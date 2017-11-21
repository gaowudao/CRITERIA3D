#include <QCoreApplication>
#include <QDateTime>
#include <iostream>
#include <math.h>

#include "creekProject.h"
#include "csvReader.h"
#include "hydrology.h"

int main(int argc, char *argv[])
{
    QCoreApplication myApp(argc, argv);

    QString settingsFileName;
    CreekProject myProject;
    std::vector<QStringList> values;
    std::vector<QDate> whcDate;
    std::vector<QDateTime> precDateTime;
    std::vector<float> whc, prec;

    if (argc > 1)
        settingsFileName = argv[1];
    else
    {
        settingsFileName = "../example/RavoneUrbanSIS.ini";

        // myProject.logError("USAGE: CREEK settings_filename.ini");
        // return false;
    }

    if (settingsFileName.left(1) == ".")
        settingsFileName = myApp.applicationDirPath() + "/" + settingsFileName;

    if (!myProject.initializeProject(settingsFileName))
        return false;

    // read WHC
    if (! readCSV(myProject.WHCdata, ",", 1, &(myProject.errorString), &values))
    {
        myProject.logError();
        return false;
    }

    for (unsigned int i = 0; i < values.size(); i++)
    {
        whcDate.push_back(QDateTime::fromString(values[i].value(0), "yyyy-MM-dd hh:mm:ss").date());
        whc.push_back(values[i].value(1).toFloat());
        //std::cout << whcDate[i].toString("yyyy-MM-dd").toStdString() << ", " << whc[i] << std::endl;
    }

    // read Prec
    values.clear();
    if (! readCSV(myProject.precData, ",", 1, &(myProject.errorString), &values))
    {
        myProject.logError();
        return false;
    }

    for (unsigned int i = 0; i < values.size(); i++)
    {
        precDateTime.push_back(QDateTime::fromString(values[i].value(0), "yyyy-MM-dd hh:mm:ss"));
        prec.push_back(values[i].value(1).toFloat());
    }

    float maxIntensity, sumPrec, adjSum, intensityRatio;
    float peak, runoffPrec, currentWHC;
    unsigned long firstIndex = 0;
    int lenght, whcIndex;
    QDate myDate;

    std::cout << "Date\t\tPrec\t\tWHC\tmax\thours\tRunoff\tPeak(m)\n";
    while (searchRainfallEvent(&firstIndex, &sumPrec, &maxIntensity, &lenght, &prec))
    {
        if (sumPrec > 2. && maxIntensity >= 1.)
        {
            myDate = precDateTime[firstIndex].date();
            whcIndex = whcDate[0].daysTo(myDate);
            if (whcIndex >= 0)
            {
                currentWHC = whc[whcIndex];

                intensityRatio = sumPrec / maxIntensity;
                if (intensityRatio > 7.)
                    adjSum = sumPrec * (7. / intensityRatio);
                else
                    adjSum = sumPrec;

                runoffPrec = adjSum - currentWHC;

                if (runoffPrec > 0)
                    peak = pow(runoffPrec, 1.5) * 0.0048;
                else
                    peak = 0.0;

                // output
                if (peak > 0.1)
                {
                    std::cout << myDate.toString("yyyy-MM-dd").toStdString();
                    std::cout << "\t" << int(sumPrec) << "->" << int(adjSum);
                    std::cout << "\t\t" << currentWHC;
                    std::cout << "\t" << maxIntensity << "\t" << lenght;
                    std::cout << "\t" << int(runoffPrec);
                    std::cout << "\t" << peak << std::endl;
                }
            }
        }

        firstIndex += lenght;
    }

    myProject.logInfo("END");
    return true;
}


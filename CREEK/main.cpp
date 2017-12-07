#include <QCoreApplication>
#include <QDateTime>
#include <iostream>
#include <algorithm>
#include <math.h>

#include "commonConstants.h"
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
        settingsFileName = "../example/Ravone.ini";

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

    float maxIntensity, sumPrec, adjSum, lastSumPrec;
    float peak, runoffPrec, currentWHC;
    unsigned long firstIndex = 0;
    int lenght, whcIndex;
    QDate myDate, lastDate;
    QString outStr;

    myProject.writeOutput("Date,Prec,adjPrec,WHC,max,hours,Runoff,Peak(m)");
    while (searchRainfallEvent(&firstIndex, &sumPrec, &maxIntensity, &lenght, &prec))
    {
        myDate = precDateTime[firstIndex].date();

        if (sumPrec > 2. && maxIntensity >= 1.)
        {
            whcIndex = whcDate[0].daysTo(myDate);

            if (whcIndex >= 0)
            {
                currentWHC = whc[whcIndex];
                /*if (myDate == lastDate)
                {
                    currentWHC -= lastSumPrec * 0.33;
                    if (currentWHC < 0.) currentWHC = 0.;
                }*/

                adjSum = minValue(sumPrec, maxIntensity * 7.0);

                runoffPrec = adjSum - currentWHC;

                // output
                if (runoffPrec > 10)
                {
                    peak = runoffPrec * 0.036 -0.3;

                    outStr = myDate.toString("yyyy-MM-dd");
                    outStr += "," + QString::number(sumPrec,'f',1);
                    outStr += "," + QString::number(adjSum,'f',1);
                    outStr += "," + QString::number(currentWHC,'f',1);
                    outStr += "," + QString::number(maxIntensity,'f',1);
                    outStr += "," + QString::number(lenght);
                    outStr += "," + QString::number(runoffPrec,'f',1);
                    outStr += "," + QString::number(peak,'f',2);
                    myProject.writeOutput(outStr);
                }
            }
        }

        lastSumPrec = sumPrec;
        lastDate = myDate;
        firstIndex += lenght;
    }

    myProject.logInfo("END");
    return true;
}


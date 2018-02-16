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

    float maxIntensity, sumPrec, adjSum;
    float peak, runoff, currentWHC;
    unsigned long firstIndex = 0;
    int lenght, peakHour, whcIndex;
    QDate dateStart, datePeak;
    QString outStr;

    myProject.writeOutput("Date,Prec,adjPrec,WHC,max,peak hour,Runoff,Peak(m)");

    while (searchRainfallEvent(&firstIndex, &sumPrec, &maxIntensity, &lenght, &peakHour, &prec))
    {
        dateStart = precDateTime[firstIndex].date();
        datePeak = precDateTime[firstIndex + peakHour].date();

        if (sumPrec > 2. && maxIntensity >= 1.)
        {
            whcIndex = whcDate[0].daysTo(dateStart);

            if (whcIndex >= 0)
            {
                currentWHC = whc[whcIndex];

                if ((sumPrec - currentWHC) > 0)
                {
                    //refineRainfallEvent(firstIndex, lenght, currentWHC, &sumPrec, &maxIntensity, &peakHour, &prec);

                    adjSum = minValue(sumPrec, maxIntensity * 7);

                    runoff = adjSum - currentWHC;

                    // output (ravone 10, ghironda 3)
                    if (runoff > 7)
                    {
                        //Ravone
                        peak = 0.0054 * powl(runoff, 1.5) - 0.08;
                        //peak = 0.0033 * powl(runoff, 1.62) - 0.05;
                        // Ghironda
                        //peak = 0.017 * powl(runoffPrec, 1) + 0.12;

                        outStr = datePeak.toString("yyyy-MM-dd");
                        outStr += "," + QString::number(sumPrec,'f',1);
                        outStr += "," + QString::number(adjSum,'f',1);
                        outStr += "," + QString::number(currentWHC,'f',1);
                        outStr += "," + QString::number(maxIntensity,'f',1);
                        outStr += "," + QString::number(peakHour);
                        outStr += "," + QString::number(runoff,'f',1);
                        outStr += "," + QString::number(peak,'f',2);
                        myProject.writeOutput(outStr);
                    }
                }
            }
        }

        firstIndex += lenght;
    }

    myProject.logInfo("END");
    return true;
}


#include <math.h>

#include "commonConstants.h"
#include "wgClimate.h"
#include "weatherGenerator.h"
#include "timeUtility.h"
#include "crit3dDate.h"
#include <iostream>
#include <QFile>
#include <QTextStream>

using namespace std;


/*!
  * \brief Compute climate (monthly values)
  * \returns true if the input data are valid
  * \param  nrDays          [-] number of data (366 x n where n is the number of years)
  * \param  inputFirstDate  [Crit3DDate]
  * \param  *inputTMin      [°C] array(1..nrDays) of minimum temperature
  * \param  *inputTMax      [°C] array(1..nrDays) of maximum temperature
  * \param  *inputPrec      [mm] array(1..nrDays) of precipitation
*/
bool computeWGClimate(int nrDays, Crit3DDate inputFirstDate, float *inputTMin, float *inputTMax,
                      float *inputPrec, float precThreshold, float minPercData,
                      TwheatherGenClimate* wGen, bool writeOutput)
{
    int nValidData = 0;
    float dataPresence = 0;
    float sumTMin[12] = {0};
    float sumTMax[12] = {0};
    float sumPrec[12] = {0};
    float sumTMin2[12] = {0};
    float sumTMax2[12] = {0};
    int nWetDays[12] = {0};
    int nWetWetDays[12] = {0};
    int nDryDays[12] = {0};
    int nrData[12] = {0};
    float sumTmaxWet[12] = {0};
    float sumTmaxDry[12] = {0};
    int daysInMonth;
    bool isPreviousDayWet = false;

    //read data
    Crit3DDate tmpCurrentDate;
    int m;
    for (int n = 0; n < nrDays; n++)
    {
        tmpCurrentDate = inputFirstDate.addDays(n);
        m = tmpCurrentDate.month-1;

        // the day is valid if all values are different from nodata
        if (int(inputTMin[n]) != int(NODATA)
            && int(inputTMax[n]) != int(NODATA)
            && int(inputPrec[n]) != int(NODATA))
        {
            nValidData = nValidData + 1;
            nrData[m] = nrData[m] + 1;
            sumTMin[m] = sumTMin[m] + inputTMin[n];
            sumTMin2[m] = sumTMin2[m] + (inputTMin[n] * inputTMin[n]);
            sumTMax[m] = sumTMax[m] + inputTMax[n];
            sumTMax2[m] = sumTMax2[m] + (inputTMax[n] * inputTMax[n]);
            sumPrec[m] = sumPrec[m] + inputPrec[n];

            if (inputPrec[n] > precThreshold)
            {
                if (isPreviousDayWet) nWetWetDays[m]++;
                nWetDays[m] = nWetDays[m] + 1;
                sumTmaxWet[m] = sumTmaxWet[m] + inputTMax[n];
                isPreviousDayWet = true;
            }
            else
            {
                nDryDays[m] = nDryDays[m] + 1;
                sumTmaxDry[m] = sumTmaxDry[m] + inputTMax[n];
                isPreviousDayWet = false;
            }
        }
    }

    dataPresence = float(nValidData) / float(nrDays);
    if (dataPresence < minPercData)
        return false;

    // compute Climate
    for (m=0; m<12; m++)
    {
        if (nrData[m] > 0)
        {
            wGen->monthly.monthlyTmax[m] = sumTMax[m] / nrData[m]; //computes mean monthly values of maximum temperature
            wGen->monthly.monthlyTmin[m] = sumTMin[m] / nrData[m]; //computes mean monthly values of minimum temperature

            daysInMonth = getDefaultDaysInMonth(m);

            wGen->monthly.sumPrec[m] = sumPrec[m] / nrData[m] * daysInMonth;

            wGen->monthly.fractionWetDays[m] = float(nWetDays[m]) / float(nrData[m]);
            wGen->monthly.probabilityWetWet[m] = float(nWetWetDays[m]) / float(nWetDays[m]);

            if ( (nDryDays[m] > 0) && (nWetDays[m] > 0) )
                wGen->monthly.dw_Tmax[m] = (sumTmaxDry[m] / nDryDays[m]) - (sumTmaxWet[m] / nWetDays[m]);
            else
                wGen->monthly.dw_Tmax[m] = 0;

            wGen->monthly.stDevTmax[m] = sqrt(std::max(nrData[m]*sumTMax2[m]-(sumTMax[m]*sumTMax[m]), 0.f) / (nrData[m]*(nrData[m]-1)));
            wGen->monthly.stDevTmin[m] = sqrt(std::max(nrData[m]*sumTMin2[m]-(sumTMin[m]*sumTMin[m]), 0.f) / (nrData[m]*(nrData[m]-1)));
        }
        else
        {
            wGen->monthly.monthlyTmax[m] = NODATA;
            wGen->monthly.monthlyTmin[m] = NODATA;
            wGen->monthly.sumPrec[m] = NODATA;
            wGen->monthly.fractionWetDays[m] = NODATA;
            wGen->monthly.stDevTmax[m] = NODATA;
            wGen->monthly.stDevTmin[m] = NODATA;
            wGen->monthly.dw_Tmax[m] = NODATA;
        }
    }


    if (writeOutput)
    {
        QString filename="climateWG.txt";
        cout << "...Write WG climate file -->" << filename.toStdString();

        QFile file(filename);
        file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);

        QTextStream stream( &file );
        stream << "----------------- CLIMATE ----------------";
        for (m=0; m<12; m++)
        {
            stream << "month = " << m +1 << endl;
            stream << "wGen->monthly.monthlyTmin = " << wGen->monthly.monthlyTmin[m] << endl;
            stream << "wGen->monthly.monthlyTmax = " << wGen->monthly.monthlyTmax[m] << endl;
            stream << "wGen->monthly.sumPrec = " << wGen->monthly.sumPrec[m] << endl;
            stream << "wGen->monthly.stDevTmin[m] = " << wGen->monthly.stDevTmin[m] << endl;
            stream << "wGen->monthly.stDevTmax = " << wGen->monthly.stDevTmax[m] << endl;
            stream << "wGen->monthly.fractionWetDays[m] = " << wGen->monthly.fractionWetDays[m] << endl;
            stream << "wGen->monthly.probabilityWetWet[m] = " << wGen->monthly.probabilityWetWet[m] << endl;
            stream << "wGen->monthly.dw_Tmax[m] = " << wGen->monthly.dw_Tmax[m] << endl;
            stream << "-------------------------------------------" << endl;
        }
    }

    return true;
}


/*!
  * \brief Generates a climate starting from daily weather
  */
bool climateGenerator(int nrData, TinputObsData climateDailyObsData, Crit3DDate climateDateIni, Crit3DDate climateDateFin, float precThreshold, float minPercData, TwheatherGenClimate* wGen)

{
    int startIndex, nrDays;
    TinputObsData newDailyObsData;
    bool result = false;

    startIndex = difference(climateDailyObsData.inputFirstDate, climateDateIni); // starts from 0
    nrDays = difference(climateDateIni,climateDateFin)+1;

    newDailyObsData.inputFirstDate = climateDateIni;
    newDailyObsData.inputTMin = (float*) malloc(nrDays*sizeof(float));
    newDailyObsData.inputTMax = (float*) malloc(nrDays*sizeof(float));
    newDailyObsData.inputPrecip = (float*) malloc(nrDays*sizeof(float));

    int j = 0;

    for (int i = 0; i < nrData; i++)
    {
        if (i >= startIndex && i < startIndex+nrDays)
        {
            newDailyObsData.inputTMin[j] = climateDailyObsData.inputTMin[i];
            newDailyObsData.inputTMax[j] = climateDailyObsData.inputTMax[i];
            newDailyObsData.inputPrecip[j] = climateDailyObsData.inputPrecip[i];
            j++;
        }
    }

    result = computeWGClimate(nrDays, newDailyObsData.inputFirstDate, newDailyObsData.inputTMin,
                              newDailyObsData.inputTMax, newDailyObsData.inputPrecip,
                              precThreshold, minPercData, wGen, false);

    free(newDailyObsData.inputTMin);
    free(newDailyObsData.inputTMax);
    free(newDailyObsData.inputPrecip);

    return result;
}


/*!
  * \brief Compute sample standard deviation
*/
float sampleStdDeviation(float values[], int nElement)
{
    float sum = 0;
    float sum2 = 0;
    int i;

    float stdDeviation = 0;

    if (nElement <= 1)
        stdDeviation = NODATA;
    else
    {
        for (i = 0; i < nElement; i++)
        {
            sum = sum + values[i];
            sum2 = sum2 + (values[i] * values[i]);
        }

        stdDeviation = sqrt( std::max(nElement * sum2 - (sum * sum), 0.f) / float(nElement * (nElement - 1)) );
    }
    return stdDeviation;
}

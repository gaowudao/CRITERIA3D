#include <math.h>

#include "commonConstants.h"
#include "climate.h"
#include "weatherGenerator.h"
#include "timeUtility.h"


/*-----------------------------------------------------------------
    computeWGClimate
-------------------------------------------------------------------
    Compute mean monthly values
    Returns true if the input data are valid data
-------------------------------------------------------------------
    INPUT
    int         nData           [-] number of data (366 x n where n is the number of years)
    Crit3DDate  inputFirstDate
    float       *inputTMin      [°C] array(1...nData) of minimum temp. data
    float       *inputTMax      [°C] array(1...nData) of maximum temp. data
    float       *inputPrec      [mm] array(1...nData) of precipitation data
-------------------------------------------------------------------*/
bool computeWGClimate(int nData, Crit3DDate inputFirstDate, float *inputTMin, float *inputTMax, float *inputPrec, float precThreshold, float minPercData, TwheatherGenClimate* wGen)
{
    int nValidData = 0;
    double dataPresence = 0;
    float sumTMin[12] = {0};
    float sumTMax[12] = {0};
    float sumPrec[12] = {0};
    float sumTMin2[12] = {0};
    float sumTMax2[12] = {0};
    int nWetDays[12] = {0};
    int nWetWetDays[12] = {0};
    int nDryDays[12] = {0};
    int numData[12] = {0};
    float sumTmaxWet[12] = {0};
    float sumTmaxDry[12] = {0};
    int daysInMonth;
    bool isPreviousDayWet = false;

    int monthends [13] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};

    //read data
    Crit3DDate tmpCurrentDate;
    int m;
    for (int n = 0; n < nData; n++)
    {
        tmpCurrentDate = inputFirstDate.addDays(n);
        m = tmpCurrentDate.month-1;

        // the day is valid if all values are different from nodata
        if ( (inputTMin[n] != NODATA) && (inputTMax[n] != NODATA) && (inputPrec[n] != NODATA) )
        {
            nValidData = nValidData + 1;
            numData[m] = numData[m] + 1;
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

    dataPresence = (double)nValidData / (double)nData;
    if (dataPresence < minPercData)
        return false;

    // compute Climate
    for (m=0; m<12; m++)
    {
        if (numData[m] > 0)
        {
            wGen->monthly.monthlyTmax[m] = sumTMax[m] / numData[m]; //computes mean monthly values of maximum temperature
            wGen->monthly.monthlyTmin[m] = sumTMin[m] / numData[m]; //computes mean monthly values of minimum temperature

            if (m !=11)
                daysInMonth = monthends[m+1] - monthends[m];
            else
                daysInMonth = 31; //December

            wGen->monthly.sumPrec[m] = sumPrec[m] / numData[m] * daysInMonth;

            wGen->monthly.fractionWetDays[m] = ((double)nWetDays[m] / (double)numData[m]);
            wGen->monthly.probabilityWetWet[m] = ((double)nWetWetDays[m] / (double)nWetDays[m]);

            if ( (nDryDays[m] > 0) && (nWetDays[m] > 0) )
                wGen->monthly.dw_Tmax[m] = (sumTmaxDry[m] / nDryDays[m]) - (sumTmaxWet[m] / nWetDays[m]);
            else
                wGen->monthly.dw_Tmax[m] = 0;

            wGen->monthly.stDevTmax[m] = sqrt( std::max(numData[m]*sumTMax2[m]-(sumTMax[m]*sumTMax[m]),(float)0.0) / (numData[m]*(numData[m]-1)));
            wGen->monthly.stDevTmin[m] = sqrt( std::max(numData[m]*sumTMin2[m]-(sumTMin[m]*sumTMin[m]),(float)0.0) / (numData[m]*(numData[m]-1)));
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


    /*--------------------------------- WRITE CLIMATE -----------------------------------
    QString filename="climateWG.txt";
    qDebug() << "...Write WG climate file -->" << filename;
    QFile file(filename);
    file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
    QTextStream stream( &file );
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
    */

    return true;
}


/*-----------------------------------------------------
    climateGenerator
    Generates a climate starting from daily weather
-------------------------------------------------------*/
bool climateGenerator(int nData, TinputObsData climateDailyObsData, Crit3DDate climateDateIni, Crit3DDate climateDateFin, float precThreshold, float minPercData, TwheatherGenClimate* wGen)

{
    int startIndex, numDays;
    TinputObsData newDailyObsData;
    bool result = false;

    startIndex = difference(climateDailyObsData.inputFirstDate,climateDateIni); // starts from 0
    numDays = difference(climateDateIni,climateDateFin)+1;

    newDailyObsData.inputFirstDate = climateDateIni;
    newDailyObsData.inputTMin = (float*)malloc(numDays*sizeof(float));
    newDailyObsData.inputTMax = (float*)malloc(numDays*sizeof(float));
    newDailyObsData.inputPrecip = (float*)malloc(numDays*sizeof(float));

    int j = 0;

    for (int i = 0; i < nData; i++)
    {
        if (i >= startIndex && i <= startIndex+numDays)
        {
            newDailyObsData.inputTMin[j] = climateDailyObsData.inputTMin[i];
            newDailyObsData.inputTMax[j] = climateDailyObsData.inputTMax[i];
            newDailyObsData.inputPrecip[j] = climateDailyObsData.inputPrecip[i];
            j++;
        }
    }

    result = computeWGClimate(numDays, newDailyObsData.inputFirstDate, newDailyObsData.inputTMin, newDailyObsData.inputTMax, newDailyObsData.inputPrecip, precThreshold, minPercData, wGen);

    free(newDailyObsData.inputTMin);
    free(newDailyObsData.inputTMax);
    free(newDailyObsData.inputPrecip);

    return result;
}


/*-------------------------------------------------------------------------------
 Compute sample standard deviation
---------------------------------------------------------------------------------*/
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

        stdDeviation = sqrt(std::max(nElement * sum2 - (sum * sum), (float)0.0) / (float(nElement) * float(nElement - 1)));
    }
    return stdDeviation;
}



// BOZZA
// computePeriodStatistic(season, inputTmin etc)
// partendo da computeWGClimate produrre i 6 indici trimestrali per ogni anno
// dividendo tra indici legati alle temp e alle prec (o facendo funzioni separate)
// salvare 6 array e passarli a sampleStdDeviation per avere le sei dev std
// salvare queste in seasonalStdDeviation
/*
void computePeriodStatistic(float* seasonObsInputTMin, float* seasonObsInputTMax, float* seasonObsInputPrecip, int seasonDataLength, TperiodStatistics* statistic)
{

    //read data
    for (int n = 0; n < seasonDataLength; n++)
    {

        // the day is valid if all values are different from nodata
        if ( (inputTMin[n] != NODATA) && (inputTMax[n] != NODATA) && (inputPrec[n] != NODATA) )
        {
            nValidData = nValidData + 1;
            numData = numData + 1;
            sumTMin = sumTMin + inputTMin[n];
            sumTMax = sumTMax + inputTMax[n];
            sumPrec = sumPrec + inputPrec[n];

            if (inputPrec[n] > precThreshold)
            {
                if (isPreviousDayWet) nWetWetDays++;
                nWetDays = nWetDays + 1;
                sumTmaxWet = sumTmaxWet + inputTMax[n];
                isPreviousDayWet = true;
            }
            else
            {
                nDryDays = nDryDays + 1;
                sumTmaxDry = sumTmaxDry + inputTMax[n];
                isPreviousDayWet = false;
            }
        }
    }

    dataPresence = (double)nValidData / (double)nData;
    if (dataPresence < minPercData)
        return false;

    if (numData > 0)
    {
        statistic->periodMeanMinTemp.push_back(sumTMin / numData); //computes mean period values of minimum temperature
        statistic->periodMeanMaxTemp.push_back(sumTMax / numData); //computes mean period values of maximum temperature

        if (m !=11)
            daysInMonth = monthends[m+1] - monthends[m];
        else
            daysInMonth = 31; //December

        statistic->periodPrecip.push_back(sumPrec[m] / numData[m] * daysInMonth);

        statistic->periodWetDaysFrequency.push_back(((double)nWetDays[m] / (double)numData[m]));
        statistic->periodWetWetDaysFrequency.push_back(((double)nWetWetDays[m] / (double)nWetDays[m]));

        if ( (nDryDays > 0) && (nWetDays > 0) )
            statistic->periodDeltaTmaxDryWet.push_back((sumTmaxDry / nDryDays) - (sumTmaxWet / nWetDays));
        else
            statistic->periodDeltaTmaxDryWet.push_back(0.0);

    }
    else
    {
        statistic->periodMeanMinTemp.push_back(NODATA);
        statistic->periodMeanMaxTemp.push_back(NODATA);
        statistic->periodPrecip.push_back(NODATA);
        statistic->periodWetDaysFrequency.push_back(NODATA);
        statistic->periodWetWetDaysFrequency.push_back(NODATA);
        statistic->periodDeltaTmaxDryWet.push_back(NODATA);
    }

}
*/

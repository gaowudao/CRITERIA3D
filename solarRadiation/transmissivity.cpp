#include "commonConstants.h"
#include "transmissivity.h"
#include "radiationSettings.h"
#include "solarRadiation.h"


float computePointTransmissivitySamani(float tmin, float tmax, float samaniCoeff)
{
    if (samaniCoeff != NODATA && tmin != NODATA && tmax != NODATA)
        if (tmin <= tmax)
            return samaniCoeff * sqrt(tmax - tmin);
        else
            return false;
    else
        return NODATA;
}


int computeTransmissivity(Crit3DMeteoPoint* meteoPoints, int nrMeteoPoints, int intervalWidth,
                          Crit3DTime myTime, const gis::Crit3DRasterGrid& myDtm)
{
    if (nrMeteoPoints <= 0) return 0;

    int hourlyFraction = meteoPoints[0].hourlyFraction;
    int deltaSeconds = 3600 / hourlyFraction;

    int semiInterval = (intervalWidth - 1)/2;
    float semiIntervalSeconds = float(semiInterval * deltaSeconds);
    float* myObsRad;
    int myIndex;
    Crit3DTime myTimeIni =  myTime.addSeconds(-semiIntervalSeconds);
    Crit3DTime myTimeFin =  myTime.addSeconds(semiIntervalSeconds);
    Crit3DTime myCurrentTime;
    int myCounter = 0;
    int indexDate;
    int indexSubDaily;

    gis::Crit3DPoint myPoint;

    for (int i = 0; i < nrMeteoPoints; i++)
        if (meteoPoints[i].getMeteoPointValueH(myTime.date, myTime.getHour(), myTime.getMinutes(), globalIrradiance) != NODATA)
        {
            myIndex = 0;
            myObsRad = (float *) calloc(intervalWidth, sizeof(float));
            myCurrentTime = myTimeIni;
            while (myCurrentTime <= myTimeFin)
            {
                myObsRad[myIndex] = meteoPoints[i].getMeteoPointValueH(myCurrentTime.date, myCurrentTime.getHour(), myCurrentTime.getMinutes(), globalIrradiance);
                myCurrentTime = myCurrentTime.addSeconds(float(deltaSeconds));
                myIndex++;
            }

            myPoint.utm.x = meteoPoints[i].point.utm.x;
            myPoint.utm.y = meteoPoints[i].point.utm.y;
            myPoint.z = meteoPoints[i].point.z;

            indexDate = -myTime.date.daysTo(meteoPoints[i].obsDataH->date);
            indexSubDaily = (hourlyFraction * myTime.getHour()) + myTime.getMinutes() % (60 / hourlyFraction);

            meteoPoints[i].obsDataH[indexDate].transmissivity[indexSubDaily] =
                    radiation::computePointTransmissivity(myPoint, myTime, myObsRad, intervalWidth, deltaSeconds, myDtm);

            myCounter++;
        }

    return myCounter;
}


int computeTransmissivityFromTRange(Crit3DMeteoPoint* meteoPoints, int nrMeteoPoints, Crit3DTime currentTime)
{
    if (nrMeteoPoints <= 0) return 0;

    int hourlyFraction = meteoPoints[0].hourlyFraction;
    int deltaSeconds = 3600 / hourlyFraction;

    int counter = 0;
    int indexDate;
    Crit3DTime timeTmp;
    float temp, tmin, tmax;
    float transmissivity;
    int i;

    for (i = 0; i < nrMeteoPoints; i++)
    {
        tmin = NODATA;
        tmax = NODATA;
        int mySeconds = deltaSeconds;

        timeTmp.date = currentTime.date;
        timeTmp.time = 0;

        while (mySeconds < DAY_SECONDS)
        {
            timeTmp = timeTmp.addSeconds(float(deltaSeconds));
            temp = meteoPoints[i].getMeteoPointValueH(timeTmp.date, timeTmp.getHour(), timeTmp.getMinutes(), airTemperature);
            if (temp != NODATA)
            {
                if (tmin == NODATA || temp < tmin)
                    tmin = temp;
                if (tmax == NODATA || temp > tmax)
                    tmax = temp;
            }
            mySeconds += deltaSeconds;
        }

        if (tmin != NODATA && tmax != NODATA)
        {
            transmissivity = computePointTransmissivitySamani(tmin, tmax, float(0.17));
            if (transmissivity != NODATA)
            {
                indexDate = -currentTime.date.daysTo(meteoPoints[i].obsDataH->date);
                int nrDayValues = hourlyFraction * 24 +1;
                for (int j = 0; j < nrDayValues; j++)
                    meteoPoints[i].obsDataH[indexDate].transmissivity[j] = transmissivity;
                //midnight
                meteoPoints[i].obsDataH[indexDate+1].transmissivity[0] = transmissivity;
                counter++;
            }
        }
    }

    return counter;
}

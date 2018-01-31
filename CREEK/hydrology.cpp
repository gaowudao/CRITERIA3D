#include "hydrology.h"
#include <algorithm>


bool searchRainfallEvent(unsigned long *firstIndex, float *sumPrec, float *maxIntensity,
                         int *lenght, int *peakHour, std::vector<float> *prec)
{
    bool isEvent = false;
    unsigned long i = *firstIndex;
    int nrHoursZero = 0;
    *maxIntensity = 0.0;
    *sumPrec = 0.0;
    *lenght = 0;
    *peakHour = 0;

    while (nrHoursZero < 3 && i < (*prec).size())
    {
        if (prec->at(i) > 0.2)
        {
            if (! isEvent)
            {
                isEvent = true;
                *firstIndex = i;
            }

            *sumPrec += prec->at(i);
            nrHoursZero = 0;

            if (prec->at(i) > *maxIntensity)
            {
                *maxIntensity = prec->at(i);
                *peakHour = i - *firstIndex;
            }
        }
        else
            if (isEvent)
            {
                nrHoursZero++;
                *sumPrec += prec->at(i);
            }

        i++;
    }

    *lenght = i - *firstIndex - nrHoursZero;

    return ((*lenght) > 0);
}


void refineRainfallEvent(unsigned long firstIndex, int lenght, float whc,
                         float *sumPrec, float *maxIntensity,
                         int *peakHour, std::vector<float> *prec)
{
    *maxIntensity = 0.0;
    *peakHour = 0;

    // step 1 - soil infiltration
    unsigned long i = firstIndex;
    while ((i < firstIndex + lenght) && (whc > 0))
    {
        whc -= prec->at(i);
        i++;
    }
    unsigned long firstRunoffHour = i;

    // step 2 - check peak after soil saturation
    while (i < firstIndex + lenght)
    {
        if (prec->at(i) > *maxIntensity)
        {
            *maxIntensity = prec->at(i);
            *peakHour = i - firstRunoffHour;
        }
        else if (prec->at(i) >= (*maxIntensity * 0.6))
        {
            *peakHour = i - firstRunoffHour;
        }
        i++;
    }

    // step 3 - precipitation event for runoff
    *sumPrec = -whc;
    for (int h = 0; h <= *peakHour; h++)
    {
        i = firstRunoffHour + h;
        *sumPrec += prec->at(i);
    }

    *peakHour += (firstRunoffHour - firstIndex);
}

#include "hydrology.h"
#include <algorithm>


bool searchRainfallEvent(unsigned long *firstIndex, float *sumPrec, float *maxIntensity,
                         int *lenght, std::vector<float> *prec)
{
    bool isEvent = false;
    unsigned long i = *firstIndex;
    int nrHoursZero = 0;
    *maxIntensity = 0.0;
    *sumPrec = 0.0;

    while (nrHoursZero < 3 && i < (*prec).size())
    {
        if (prec->at(i) > 0.2)
        {
            if (! isEvent)
            {
                isEvent = true;
                *sumPrec = 0.0;
                *firstIndex = i;
            }

            *sumPrec += prec->at(i);
            *maxIntensity = std::max(*maxIntensity, prec->at(i));
            nrHoursZero = 0;
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
